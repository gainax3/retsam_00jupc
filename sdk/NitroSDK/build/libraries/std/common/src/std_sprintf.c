/*---------------------------------------------------------------------------*
  Project:  NitroSDK - libraries - STD
  File:     std_sprintf.c

  Copyright 2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/


#include <nitro.h>


/*---------------------------------------------------------------------------*/
/* functions */

/* inner functions for sized-buffer output */
typedef struct dst_string_tag
{
    size_t  len;
    char   *cur;
    char   *base;
}
dst_string;

static void string_put_char(dst_string * p, char c)
{
    if (p->len > 0)
        *p->cur = c, --p->len;
    ++p->cur;
}

static void string_fill_char(dst_string * p, char c, int n)
{
    if (n > 0)
    {
        size_t  i, k = p->len;
        if (k > (size_t) n)
            k = (size_t) n;
        for (i = 0; i < k; ++i)
            p->cur[i] = c;
        p->len -= k;
        p->cur += n;
    }
}

static void string_put_string(dst_string * p, const char *s, int n)
{
    if (n > 0)
    {
        size_t  i, k = p->len;
        if (k > (size_t) n)
            k = (size_t) n;
        for (i = 0; i < k; ++i)
            p->cur[i] = s[i];
        p->len -= k;
        p->cur += n;
    }
}

/*---------------------------------------------------------------------------*
  Name:         STD_TSPrintf

  Description:  引数の形式以外は STD_TVSNPrintf と同じ.

  Arguments:    dst           結果を格納するバッファ
                fmt           書式制御文字列

  Returns:      STD_VSNPrintf と同じ.
 *---------------------------------------------------------------------------*/
SDK_WEAK_SYMBOL int STD_TSPrintf(char *dst, const char *fmt, ...)
{
    int     ret;
    va_list vlist;
    va_start(vlist, fmt);
    ret = STD_TVSNPrintf(dst, 0x7FFFFFFF, fmt, vlist);
    va_end(vlist);
    return ret;
}

/*---------------------------------------------------------------------------*
  Name:         STD_TVSPrintf

  Description:  引数の形式以外は STD_TVSNPrintf と同じ.

  Arguments:    dst           結果を格納するバッファ
                fmt           書式制御文字列
                vlist         パラメータ

  Returns:      STD_VSNPrintf と同じ.
 *---------------------------------------------------------------------------*/
SDK_WEAK_SYMBOL int STD_TVSPrintf(char *dst, const char *fmt, va_list vlist)
{
    return STD_TVSNPrintf(dst, 0x7FFFFFFF, fmt, vlist);
}

/*---------------------------------------------------------------------------*
  Name:         STD_TSNPrintf

  Description:  引数の形式以外は STD_TVSNPrintf と同じ.

  Arguments:    dst           結果を格納するバッファ
                len           バッファ長
                fmt           書式制御文字列
 
  Returns:      STD_VSNPrintf と同じ.
 *---------------------------------------------------------------------------*/
SDK_WEAK_SYMBOL int STD_TSNPrintf(char *dst, size_t len, const char *fmt, ...)
{
    int     ret;
    va_list vlist;
    va_start(vlist, fmt);
    ret = STD_TVSNPrintf(dst, len, fmt, vlist);
    va_end(vlist);
    return ret;
}

/*---------------------------------------------------------------------------*
  Name:         STD_TVSNPrintf

  Description:  省サイズを目的とした sprintf.
                基本的な書式指定に対応.
                %([-+# ]?)([0-9]*)(\.?)([0-9]*)([l|ll|h||hh]?)([diouxXpncs%])

  Note:         CodeWarrior の MSL sprintf() の挙動にあわせて
                '+' と '#' は無効にしてある.
                { // exsample
                  char buf[5];
                  sprintf(buf, "%-i\n", 45);  // "45"  (OK)
                  sprintf(buf, "%0i\n", 45);  // "45"  (OK)
                  sprintf(buf, "% i\n", 45);  // " 45" (OK)
                  sprintf(buf, "%+i\n", 45);  // "%+i" ("+45" expected)
                  sprintf(buf, "%#x\n", 45);  // "%#x" ("0x2d" expected)
                  // but, this works correctly!
                  sprintf(buf, "% +i\n", 45); // "+45" (OK)
                }

  Arguments:    dst           結果を格納するバッファ
                len           バッファ長
                fmt           書式制御文字列
                vlist         パラメータ

  Returns:      書式文字列を正しく出力した場合の文字数を返す. ('\0'を含まない)
                バッファサイズが充分なら全ての文字を出力して終端を付与する.
                バッファサイズが不足なら切り詰めて dst[len-1] を終端とする.
                len が 0 の場合は何もしない.

 *---------------------------------------------------------------------------*/
SDK_WEAK_SYMBOL int STD_TVSNPrintf(char *dst, size_t len, const char *fmt, va_list vlist)
{
    char    buf[24];
    int     n_buf;
    char    prefix[2];
    int     n_prefix;

    const char *s = fmt;

    dst_string str;
    str.len = len, str.cur = str.base = dst;

    while (*s)
    {
        if ((unsigned int)(((unsigned char)*s ^ 0x20) - 0xA1) < 0x3C)
        {
            /* Shift JIS character */
            string_put_char(&str, *s++);
            if (*s)
                string_put_char(&str, *s++);
        }
        else if (*s != '%')
        {
            /* normal ASCII character */
            string_put_char(&str, *s++);
        }
        else
        {
            /* output with format */
            enum
            {
                flag_blank = 000001,   /* ' ' */
                flag_plus = 000002,    /* '+' */
                flag_sharp = 000004,   /* '#' */
                flag_minus = 000010,   /* '-' */
                flag_zero = 000020,    /* '0' */
                flag_l1 = 000040,      /* "l" */
                flag_h1 = 000100,      /* "h" */
                flag_l2 = 000200,      /* "ll" */
                flag_h2 = 000400,      /* "hh" */
                flag_unsigned = 010000, /* 'o', 'u', ... */
                flag_end
            };
            int     flag = 0, width = 0, precision = -1, radix = 10;
            char    hex_char = 'a' - 10;
            const char *p_start = s;
            /* flags */
            for (;;)
            {
                switch (*++s)
                {
                case '+':
                    if (s[-1] != ' ')
                        break;
                    flag |= flag_plus;
                    continue;
                case ' ':
                    flag |= flag_blank;
                    continue;
                case '-':
                    flag |= flag_minus;
                    continue;
                case '0':
                    flag |= flag_zero;
                    continue;
                }
                break;
            }
            /* width */
            if (*s == '*')
            {
                ++s, width = va_arg(vlist, int);
                if (width < 0)
                    width = -width, flag |= flag_minus;
            }
            else
            {
                while ((*s >= '0') && (*s <= '9'))
                    width = (width * 10) + *s++ - '0';
            }
            /* precision */
            if (*s == '.')
            {
                ++s, precision = 0;
                if (*s == '*')
                {
                    ++s, precision = va_arg(vlist, int);
                    if (precision < 0)
                        precision = -1;
                }
                else
                {
                    while ((*s >= '0') && (*s <= '9'))
                        precision = (precision * 10) + *s++ - '0';
                }
            }
            /* option */
            switch (*s)
            {
            case 'h':
                if (*++s != 'h')
                    flag |= flag_h1;
                else
                    ++s, flag |= flag_h2;
                break;
            case 'l':
                if (*++s != 'l')
                    flag |= flag_l1;
                else
                    ++s, flag |= flag_l2;
                break;
            }

            /* type */
            switch (*s)
            {
            case 'd':                 /* signed decimal */
            case 'i':                 /* signed decimal */
                goto put_integer;
            case 'o':                 /* unsigned octal */
                radix = 8;
                flag |= flag_unsigned;
                goto put_integer;
            case 'u':                 /* unsigned decimal */
                flag |= flag_unsigned;
                goto put_integer;
            case 'X':                 /* unsigned hexadecimal */
                hex_char = 'A' - 10;
                goto put_hexadecimal;
            case 'x':                 /* unsigned hexadecimal */
                goto put_hexadecimal;
            case 'p':                 /* pointer */
                /* equal to code warrior */
                flag |= flag_sharp;
                precision = 8;
                goto put_hexadecimal;

            case 'c':                 /* character */
                if (precision >= 0)
                    goto put_invalid;
                {
                    int     c = va_arg(vlist, int);
                    width -= 1;
                    if (flag & flag_minus)
                    {
                        string_put_char(&str, (char)c);
                        string_fill_char(&str, ' ', width);
                    }
                    else
                    {
                        char    pad = (char)((flag & flag_zero) ? '0' : ' ');
                        string_fill_char(&str, pad, width);
                        string_put_char(&str, (char)c);
                    }
                    ++s;
                }
                break;

            case 's':                 /* string */
                {
                    int     n_buf = 0;
                    const char *p_buf = va_arg(vlist, const char *);
                    if (precision < 0)
                    {
                        while (p_buf[n_buf])
                            ++n_buf;
                    }
                    else
                    {
                        while ((n_buf < precision) && p_buf[n_buf])
                            ++n_buf;
                    }
                    width -= n_buf;
                    if (flag & flag_minus)
                    {
                        string_put_string(&str, p_buf, n_buf);
                        string_fill_char(&str, ' ', width);
                    }
                    else
                    {
                        char    pad = (char)((flag & flag_zero) ? '0' : ' ');
                        string_fill_char(&str, pad, width);
                        string_put_string(&str, p_buf, n_buf);
                    }
                    ++s;
                }
                break;

            case 'n':                 /* store the number of output */
                {
                    int     pos = str.cur - str.base;
                    if (flag & flag_h2)
                        ;
                    else if (flag & flag_h1)
                        *va_arg(vlist, signed short *) = (signed short)pos;
                    else if (flag & flag_l2)
                        *va_arg(vlist, u64 *) = (u64)pos;
                    else
                        *va_arg(vlist, signed int *) = (signed int)pos;
                }
                ++s;
                break;

            case '%':                 /* output '%' */
                if (p_start + 1 != s)
                    goto put_invalid;
                string_put_char(&str, *s++);
                break;

            default:                  /* invalid type */
                goto put_invalid;

              put_invalid:
                string_put_string(&str, p_start, s - p_start);
                break;

              put_hexadecimal:
                radix = 16;
                flag |= flag_unsigned;
              put_integer:
                {
                    u64     val = 0;
                    n_prefix = 0;

                    if (flag & flag_minus)
                        flag &= ~flag_zero;
                    if (precision < 0)
                        precision = 1;
                    else
                        flag &= ~flag_zero;

                    if (flag & flag_unsigned)
                    {
                        if (flag & flag_h2)
                            val = va_arg(vlist, unsigned char);
                        else if (flag & flag_h1)
                            val = va_arg(vlist, unsigned short);
                        else if (flag & flag_l2)
                            val = va_arg(vlist, u64);
                        else
                            val = va_arg(vlist, unsigned long);
                        flag &= ~(flag_plus | flag_blank);
                        if (flag & flag_sharp)
                        {
                            if (radix == 16)
                            {
                                if (val != 0)
                                {
                                    prefix[0] = (char)(hex_char + (10 + 'x' - 'a'));
                                    prefix[1] = '0';
                                    n_prefix = 2;
                                }
                            }
                            else if (radix == 8)
                            {
                                prefix[0] = '0';
                                n_prefix = 1;
                            }
                        }
                    }
                    else
                    {
                        if (flag & flag_h2)
                            val = va_arg(vlist, char);
                        else if (flag & flag_h1)
                            val = va_arg(vlist, short);
                        else if (flag & flag_l2)
                            val = va_arg(vlist, u64);
                        else
                            val = va_arg(vlist, long);
                        if ((val >> 32) & 0x80000000)
                        {
                            val = ~val + 1;
                            prefix[0] = '-';
                            n_prefix = 1;
                        }
                        else
                        {
                            if (val || precision)
                            {
                                if (flag & flag_plus)
                                {
                                    prefix[0] = '+';
                                    n_prefix = 1;
                                }
                                else if (flag & flag_blank)
                                {
                                    prefix[0] = ' ';
                                    n_prefix = 1;
                                }
                            }
                        }
                    }
                    n_buf = 0;
                    switch (radix)
                    {
                    case 8:
                        while (val != 0)
                        {
                            int     d = (int)(val & 0x07);
                            val >>= 3;
                            buf[n_buf++] = (char)(d + '0');
                        }
                        break;
                    case 10:
                        if ((val >> 32) == 0)
                        {
#if defined(SDK_CW) || defined(__MWERKS__)
#pragma optimize_for_size off
#endif
                            u32     v = (u32)val;
                            while (v != 0)
                            {
                                // u32 と定数の除算であれば、コンパイラが自動的に
                                // マジックナンバーを使用した積算に変換する
                                u32     r = v / 10;
                                int     d = (int)(v - (r * 10));
                                v = r;
                                buf[n_buf++] = (char)(d + '0');
                            }
                        }
                        else
                        {
                            while (val != 0)
                            {
                                u64     r = val / 10;
                                int     d = (int)(val - (r * 10));
                                val = r;
                                buf[n_buf++] = (char)(d + '0');
                            }
                        }
                        break;
                    case 16:
                        while (val != 0)
                        {
                            int     d = (int)(val & 0x0f);
                            val >>= 4;
                            buf[n_buf++] = (char)((d < 10) ? (d + '0') : (d + hex_char));
                        }
                        break;
                    }
                    if ((n_prefix > 0) && (prefix[0] == '0'))
                    {
                        n_prefix = 0;
                        buf[n_buf++] = '0';
                    }
                }
                goto put_to_stream;

              put_to_stream:
                {
                    int     n_pad = precision - n_buf;
                    if (flag & flag_zero)
                    {
                        if (n_pad < width - n_buf - n_prefix)
                            n_pad = width - n_buf - n_prefix;
                    }
                    if (n_pad > 0)
                        width -= n_pad;
                    width -= n_prefix + n_buf;
                    if (!(flag & flag_minus))
                        string_fill_char(&str, ' ', width);
                    while (n_prefix > 0)
                        string_put_char(&str, prefix[--n_prefix]);
                    string_fill_char(&str, '0', n_pad);
                    while (n_buf > 0)
                        string_put_char(&str, buf[--n_buf]);
                    if (flag & flag_minus)
                        string_fill_char(&str, ' ', width);
                    ++s;
                }
                break;
            }
        }
    }

    if (str.len > 0)
    {
        *str.cur = '\0';
    }
    else if (len > 0)
    {
        str.base[len - 1] = '\0';
    }
    return str.cur - str.base;
}


/*---------------------------------------------------------------------------*
  $Log: std_sprintf.c,v $
  Revision 1.1  2007/04/20 09:46:38  yosizaki
  initial upload.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
