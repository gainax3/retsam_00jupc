/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MATH - 
  File:     crc.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: crc.c,v $
  Revision 1.5  2006/01/18 02:11:19  kitase_hirotake
  do-indent

  Revision 1.4  2005/04/13 11:57:54  seiki_masashi
  GetDigest -> GetHash

  Revision 1.3  2005/04/11 10:35:37  seiki_masashi
  ユーティリティ関数の追加

  Revision 1.2  2005/04/11 08:24:27  seiki_masashi
  small fix

  Revision 1.1  2005/04/11 01:48:53  seiki_masashi
  crc.c の追加

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro/math/crc.h>

/*****************************************************************************/
/* CRC-8 共通                                                                */
/*****************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         MATHi_CRC8InitTable

  Description:  CRC-8 値を求めるために使う MATHCRC8Table 構造体を初期化する。
  
  Arguments:    table MATHCRC8Table 構造体
                poly  生成多項式のビット表現から最上位ビットを除いた数
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void MATHi_CRC8InitTable(MATHCRC8Table * table, u8 poly)
{
    u32     r;
    u32     i, j;
    u8     *t = table->table;

    for (i = 0; i < 256; i++)
    {
        r = i;
        for (j = 0; j < 8; j++)
        {
            if (r & 0x80)
            {
                r = (r << 1) ^ poly;
            }
            else
            {
                r <<= 1;
            }
        }
        t[i] = (u8)r;
    }
}

/*---------------------------------------------------------------------------*
  Name:         MATHi_CRC8InitTableRev

  Description:  CRC-8 値を求めるために使う MATHCRC8Table 構造体を初期化する。
                下位ビットを上位の桁として扱う。
  
  Arguments:    table MATHCRC8Table 構造体
                poly  生成多項式のビット表現から最上位ビットを除いた数を
                      上下反転させたもの
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void MATHi_CRC8InitTableRev(MATHCRC8Table * table, u8 poly)
{
    u32     r;
    u32     i, j;
    u8     *t = table->table;

    for (i = 0; i < 256; i++)
    {
        r = i;
        for (j = 0; j < 8; j++)
        {
            if (r & 1)
            {
                r = (r >> 1) ^ poly;
            }
            else
            {
                r >>= 1;
            }
        }
        t[i] = (u8)r;
    }
}

/*---------------------------------------------------------------------------*
  Name:         MATHi_CRC8Update

  Description:  CRC-8 値を与えたデータで更新する。
  
  Arguments:    table   計算用のテーブル MATHCRC8Table へのポインタ
                context MATHCRC8Context 構造体
                input   入力データへのポインタ
                length  入力データ長
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void
MATHi_CRC8Update(const MATHCRC8Table * table, MATHCRC8Context * context, const void *input,
                 u32 length)
{
    u32     r;
    u32     i;
    const u8 *t = table->table;
    u8     *data = (u8 *)input;

    r = (u32)*context;
    for (i = 0; i < length; i++)
    {
        r = t[(r ^ *data) & 0xff];
        data++;
    }
    *context = (MATHCRC8Context) r;
}

/*****************************************************************************/
/* CRC-16 共通                                                               */
/*****************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         MATHi_CRC16InitTable

  Description:  CRC-16 値を求めるために使う MATHCRC16Table 構造体を初期化する。
  
  Arguments:    table MATHCRC16Table 構造体
                poly  生成多項式のビット表現から最上位ビットを除いた数
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void MATHi_CRC16InitTable(MATHCRC16Table * table, u16 poly)
{
    u32     r;
    u32     i, j;
    u16    *t = table->table;

    for (i = 0; i < 256; i++)
    {
        r = i << 8;
        for (j = 0; j < 8; j++)
        {
            if (r & 0x8000)
            {
                r = (r << 1) ^ poly;
            }
            else
            {
                r <<= 1;
            }
        }
        t[i] = (u16)r;
    }
}

/*---------------------------------------------------------------------------*
  Name:         MATHi_CRC16InitTableRev

  Description:  CRC-16 値を求めるために使う MATHCRC16Table 構造体を初期化する。
                下位ビットを上位の桁として扱う。
  
  Arguments:    table MATHCRC16Table 構造体
                poly  生成多項式のビット表現から最上位ビットを除いた数を
                      上下反転させたもの
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void MATHi_CRC16InitTableRev(MATHCRC16Table * table, u16 poly)
{
    u32     r;
    u32     i, j;
    u16    *t = table->table;

    for (i = 0; i < 256; i++)
    {
        r = i;
        for (j = 0; j < 8; j++)
        {
            if (r & 1)
            {
                r = (r >> 1) ^ poly;
            }
            else
            {
                r >>= 1;
            }
        }
        t[i] = (u16)r;
    }
}

/*---------------------------------------------------------------------------*
  Name:         MATHi_CRC16Update

  Description:  CRC-16 値を与えたデータで更新する。
  
  Arguments:    table   計算用のテーブル MATHCRC16Table へのポインタ
                context MATHCRC16Context 構造体
                input   入力データへのポインタ
                length  入力データ長
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void
MATHi_CRC16Update(const MATHCRC16Table * table, MATHCRC16Context * context, const void *input,
                  u32 length)
{
    u32     r;
    u32     i;
    const u16 *t = table->table;
    u8     *data = (u8 *)input;

    r = (u32)*context;
    for (i = 0; i < length; i++)
    {
        r = (r << 8) ^ t[((r >> 8) ^ *data) & 0xff];
        data++;
    }
    *context = (MATHCRC16Context) r;
}

/*---------------------------------------------------------------------------*
  Name:         MATHi_CRC16UpdateRev

  Description:  CRC-16 値を与えたデータで更新する。
                下位ビットを上位の桁として扱う。
  
  Arguments:    table   計算用のテーブル MATHCRC16Table へのポインタ
                context MATHCRC16Context 構造体
                input   入力データへのポインタ
                length  入力データ長
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void
MATHi_CRC16UpdateRev(const MATHCRC16Table * table, MATHCRC16Context * context, const void *input,
                     u32 length)
{
    u32     r;
    u32     i;
    const u16 *t = table->table;
    u8     *data = (u8 *)input;

    r = (u32)*context;
    for (i = 0; i < length; i++)
    {
        r = (r >> 8) ^ t[(r ^ *data) & 0xff];
        data++;
    }
    *context = (MATHCRC16Context) r;
}

/*****************************************************************************/
/* CRC-32 共通                                                               */
/*****************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         MATHi_CRC32InitTable

  Description:  CRC-32 値を求めるために使う MATHCRC32Table 構造体を初期化する。
  
  Arguments:    table MATHCRC32Table 構造体
                poly  生成多項式のビット表現から最上位ビットを除いた数
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void MATHi_CRC32InitTable(MATHCRC32Table * table, u32 poly)
{
    u32     r;
    u32     i, j;
    u32    *t = table->table;

    for (i = 0; i < 256; i++)
    {
        r = i << 24;
        for (j = 0; j < 8; j++)
        {
            if (r & 0x80000000U)
            {
                r = (r << 1) ^ poly;
            }
            else
            {
                r <<= 1;
            }
        }
        t[i] = r;
    }
}

/*---------------------------------------------------------------------------*
  Name:         MATHi_CRC32InitTableRev

  Description:  CRC-32 値を求めるために使う MATHCRC32Table 構造体を初期化する。
                下位ビットを上位の桁として扱う。
  
  Arguments:    table MATHCRC32Table 構造体
                poly  生成多項式のビット表現から最上位ビットを除いた数を
                      上下反転させたもの
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void MATHi_CRC32InitTableRev(MATHCRC32Table * table, u32 poly)
{
    u32     r;
    u32     i, j;
    u32    *t = table->table;

    for (i = 0; i < 256; i++)
    {
        r = i;
        for (j = 0; j < 8; j++)
        {
            if (r & 1)
            {
                r = (r >> 1) ^ poly;
            }
            else
            {
                r >>= 1;
            }
        }
        t[i] = r;
    }
}

/*---------------------------------------------------------------------------*
  Name:         MATHi_CRC32Update

  Description:  CRC-32 値を与えたデータで更新する。
  
  Arguments:    table   計算用のテーブル MATHCRC32Table へのポインタ
                context MATHCRC32Context 構造体
                input   入力データへのポインタ
                length  入力データ長
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void
MATHi_CRC32Update(const MATHCRC32Table * table, MATHCRC32Context * context, const void *input,
                  u32 length)
{
    u32     r;
    u32     i;
    const u32 *t = table->table;
    u8     *data = (u8 *)input;

    r = (u32)*context;
    for (i = 0; i < length; i++)
    {
        r = (r << 8) ^ t[((r >> 24) ^ *data) & 0xff];
        data++;
    }
    *context = (MATHCRC32Context) r;
}

/*---------------------------------------------------------------------------*
  Name:         MATHi_CRC32UpdateRev

  Description:  CRC-32 値を与えたデータで更新する。
                下位ビットを上位の桁として扱う。
  
  Arguments:    table   計算用のテーブル MATHCRC32Table へのポインタ
                context MATHCRC32Context 構造体
                input   入力データへのポインタ
                length  入力データ長
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void
MATHi_CRC32UpdateRev(const MATHCRC32Table * table, MATHCRC32Context * context, const void *input,
                     u32 length)
{
    u32     r;
    u32     i;
    const u32 *t = table->table;
    u8     *data = (u8 *)input;

    r = (u32)*context;
    for (i = 0; i < length; i++)
    {
        r = (r >> 8) ^ t[(r ^ *data) & 0xff];
        data++;
    }
    *context = (MATHCRC32Context) r;
}


/*****************************************************************************/
/* ユーティリティ関数                                                        */
/*****************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         MATH_CalcCRC8

  Description:  CRC-8 値を計算する。

  Arguments:    table   計算用のテーブル MATHCRC8Table へのポインタ
                data    入力データのポインタ
                dataLength  入力データ長

  Returns:      計算結果.
 *---------------------------------------------------------------------------*/
u8 MATH_CalcCRC8(const MATHCRC8Table * table, const void *data, u32 dataLength)
{
    MATHCRC8Context context;
    MATH_CRC8Init(&context);
    MATH_CRC8Update(table, &context, data, dataLength);
    return MATH_CRC8GetHash(&context);
}

/*---------------------------------------------------------------------------*
  Name:         MATH_CalcCRC16

  Description:  CRC-16 値を計算する。

  Arguments:    table   計算用のテーブル MATHCRC16Table へのポインタ
                data    入力データのポインタ
                dataLength  入力データ長

  Returns:      計算結果.
 *---------------------------------------------------------------------------*/
u16 MATH_CalcCRC16(const MATHCRC16Table * table, const void *data, u32 dataLength)
{
    MATHCRC16Context context;
    MATH_CRC16Init(&context);
    MATH_CRC16Update(table, &context, data, dataLength);
    return MATH_CRC16GetHash(&context);
}

/*---------------------------------------------------------------------------*
  Name:         MATH_CalcCRC16CCITT

  Description:  CRC-16/CCITT 値を計算する。

  Arguments:    table   計算用のテーブル MATHCRC16Table へのポインタ
                data    入力データのポインタ
  Arguments:    data    入力データのポインタ
                dataLength  入力データ長

  Returns:      計算結果.
 *---------------------------------------------------------------------------*/
u16 MATH_CalcCRC16CCITT(const MATHCRC16Table * table, const void *data, u32 dataLength)
{
    MATHCRC16Context context;
    MATH_CRC16CCITTInit(&context);
    MATH_CRC16CCITTUpdate(table, &context, data, dataLength);
    return MATH_CRC16CCITTGetHash(&context);
}

/*---------------------------------------------------------------------------*
  Name:         MATH_CalcCRC32

  Description:  CRC-32 値を計算する。

  Arguments:    table   計算用のテーブル MATHCRC32Table へのポインタ
                data    入力データのポインタ
                dataLength  入力データ長

  Returns:      計算結果.
 *---------------------------------------------------------------------------*/
u32 MATH_CalcCRC32(const MATHCRC32Table * table, const void *data, u32 dataLength)
{
    MATHCRC32Context context;
    MATH_CRC32Init(&context);
    MATH_CRC32Update(table, &context, data, dataLength);
    return MATH_CRC32GetHash(&context);
}

/*---------------------------------------------------------------------------*
  Name:         MATH_CalcCRC32POSIX

  Description:  CRC-32/POSIX 値を計算する。

  Arguments:    table   計算用のテーブル MATHCRC32Table へのポインタ
                data    入力データのポインタ
                dataLength  入力データ長

  Returns:      計算結果.
 *---------------------------------------------------------------------------*/
u32 MATH_CalcCRC32POSIX(const MATHCRC32Table * table, const void *data, u32 dataLength)
{
    MATHCRC32Context context;
    MATH_CRC32POSIXInit(&context);
    MATH_CRC32POSIXUpdate(table, &context, data, dataLength);
    return MATH_CRC32POSIXGetHash(&context);
}
