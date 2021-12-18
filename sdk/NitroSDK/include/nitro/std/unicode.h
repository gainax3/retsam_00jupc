/*---------------------------------------------------------------------------*
  Project:  NitroSDK - STD - include
  File:     unicode.h

  Copyright 2006,2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: unicode.h,v $
  Revision 1.4  2007/02/20 00:28:09  kitase_hirotake
  indent source

  Revision 1.3  2006/08/10 07:52:49  yosizaki
  add STDResult and change API interface.

  Revision 1.2  2006/07/24 13:16:42  yosizaki
  beautify indent.
  change of STD-API interface.

  Revision 1.1  2006/07/05 07:42:51  kitase_hirotake
  std に sjis <-> unicode 変換ライブラリの追加

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_STD_UNICODE_H_
#define NITRO_STD_UNICODE_H_

#ifdef __cplusplus
extern "C" {
#endif


/*****************************************************************************/
/* constant */

typedef enum STDResult
{
    STD_RESULT_SUCCESS,
    STD_RESULT_ERROR,
    STD_RESULT_INVALID_PARAM,
    STD_RESULT_CONVERSION_FAILED
}
STDResult;

/*****************************************************************************/
/* declaration */

//---- ConvertString callback
typedef STDResult(*STDConvertUnicodeCallback) (u16 *dst, int *dst_len, const char *src,
                                               int *src_len);
typedef STDResult(*STDConvertSjisCallback) (char *dst, int *dst_len, const u16 *src, int *src_len);


/*****************************************************************************/
/* function */

/*---------------------------------------------------------------------------*
  Name:         STD_IsSjisLeadByte

  Description:  指定した文字がShiftJISの先行バイトであるか判定.

  Arguments:    c                 判定する文字.

  Returns:      ShiftJISの先行バイトならTRUE.
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL STD_IsSjisLeadByte(int c)
{
    return ((unsigned int)((((u8)c) ^ 0x20) - 0xA1) < 0x3C);
}

/*---------------------------------------------------------------------------*
  Name:         STD_IsSjisTrailByte

  Description:  指定した文字がShiftJISの後続バイトであるか判定.

  Arguments:    c                 判定する文字.

  Returns:      ShiftJISの後続バイトならTRUE.
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL STD_IsSjisTrailByte(int c)
{
    return (c != 0x7F) && ((u8)(c - 0x40) <= (0xFC - 0x40));
}

/*---------------------------------------------------------------------------*
  Name:         STD_IsSjisCharacter

  Description:  指定した文字列がShiftJISの1文字であるか判定.

  Arguments:    s                 判定する文字列.

  Returns:      ShiftJISの1文字ならTRUE.
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL STD_IsSjisCharacter(const char *s)
{
    return STD_IsSjisLeadByte(s[0]) && STD_IsSjisTrailByte(s[1]);
}

/*---------------------------------------------------------------------------*
  Name:         STD_ConvertStringSjisToUnicode

  Description:  ShiftJIS文字列をUnicode文字列に変換.

  Arguments:    dst               変換先バッファ.
                                  NULL を指定すると格納処理は無視される.
                dst_len           変換先バッファの最大文字数を格納して渡し,
                                  実際に格納された文字数を受け取るポインタ.
                                  NULL を与えた場合は無視される.
                src               変換元バッファ.
                src_len           変換すべき最大文字数を格納して渡し,
                                  実際に変換された文字数を受け取るポインタ.
                                  この指定よりも文字列終端の位置が優先される.
                                  負の値を格納して渡すか NULL を与えた場合は
                                  終端位置までの文字数を指定したとみなされる.
                callback          変換できない文字が現れた時に呼ばれるコールバック.
                                  NULLを指定した場合, 変換できない文字の位置で
                                  変換処理を終了する.

  Returns:      変換処理の結果.
 *---------------------------------------------------------------------------*/
STDResult STD_ConvertStringSjisToUnicode(u16 *dst, int *dst_len,
                                         const char *src, int *src_len,
                                         STDConvertUnicodeCallback callback);

/*---------------------------------------------------------------------------*
  Name:         STD_ConvertCharSjisToUnicode.

  Description:  ShiftJIS文字をUnicode文字に変換.

  Arguments:    dst               変換先バッファ.
                src               変換元文字列.

  Returns:      変換した文字数.
                変換に失敗した場合は -1.
 *---------------------------------------------------------------------------*/
SDK_INLINE int STD_ConvertCharSjisToUnicode(u16 *dst, const char *src)
{
    int     src_len = STD_IsSjisCharacter(src) ? 2 : 1;
    int     dst_len = 1;
    STDResult ret = STD_ConvertStringSjisToUnicode(dst, &dst_len, src, &src_len, NULL);
    return (ret == STD_RESULT_SUCCESS) ? dst_len : -1;
}

/*---------------------------------------------------------------------------*
  Name:         STD_ConvertStringUnicodeToSjis

  Description:  Unicode文字列をShiftJIS文字列に変換

  Arguments:    dst               変換先バッファ.
                                  NULL を指定すると格納処理は無視される.
                dst_len           変換先バッファの最大文字数を格納して渡し,
                                  実際に格納された文字数を受け取るポインタ.
                                  NULL を与えた場合は無視される.
                src               変換元バッファ.
                src_len           変換すべき最大文字数を格納して渡し,
                                  実際に変換された文字数を受け取るポインタ.
                                  この指定よりも文字列終端の位置が優先される.
                                  負の値を格納して渡すか NULL を与えた場合は
                                  終端位置までの文字数を指定したとみなされる.
                callback          変換できない文字が現れた時に呼ばれるコールバック.
                                  NULLを指定した場合, 変換できない文字の位置で
                                  変換処理を終了する.

  Returns:      変換処理の結果.
 *---------------------------------------------------------------------------*/
STDResult STD_ConvertStringUnicodeToSjis(char *dst, int *dst_len,
                                         const u16 *src, int *src_len,
                                         STDConvertSjisCallback callback);

/*---------------------------------------------------------------------------*
  Name:         STD_ConvertCharUnicodeToSjis

  Description:  Unicode文字列をShiftJIS文字列に変換.

  Arguments:    dst               変換先バッファ.
                                  2バイトを確保する必要がある.
                src               変換元文字列.

  Returns:      変換したバイト数.
                変換に失敗した場合は -1.
 *---------------------------------------------------------------------------*/
SDK_INLINE int STD_ConvertCharUnicodeToSjis(char *dst, u16 src)
{
    int     src_len = 1;
    int     dst_len = 2;
    STDResult ret = STD_ConvertStringUnicodeToSjis(dst, &dst_len, &src, &src_len, NULL);
    return (ret == STD_RESULT_SUCCESS) ? dst_len : -1;
}


#ifdef __cplusplus
} /* extern "C" */
#endif

/* NITRO_STD_UNICODE_H_ */
#endif
