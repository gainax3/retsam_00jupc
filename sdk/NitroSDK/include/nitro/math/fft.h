/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MATH - 
  File:     math/fft.h

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: fft.h,v $
  Revision 1.4  2007/04/24 12:04:00  yosizaki
  update copyright.

  Revision 1.3  2007/04/20 09:59:46  yosizaki
  fix include dependency.

  Revision 1.2  2006/01/18 02:11:19  kitase_hirotake
  do-indent

  Revision 1.1  2005/05/13 09:23:56  seiki_masashi
  FFT 関数の追加

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_MATH_FFT_H_
#define NITRO_MATH_FFT_H_

#include <nitro/types.h>
#include <nitro/fx/fx.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------
// Type definition
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Declaration of function
//----------------------------------------------------------------------------

/*---------------------------------------------------------------------------*
  Name:         MATH_MakeFFTSinTable

  Description:  高速フーリエ変換で用いる sin テーブルを作成する

  Arguments:    sinTable - 2^nShift * 3/4 の sin テーブルを格納する場所へのポインタ
                nShift - データ数の log2

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    MATH_MakeFFTSinTable(fx16 *sinTable, u32 nShift);

/*---------------------------------------------------------------------------*
  Name:         MATHi_FFT

  Description:  高速フーリエ変換を行う内部関数

  Arguments:    data - 偶数番目に実部、奇数番目に虚部を収めた変換を行うデータ。
                       変換結果は上書きされて返される。
                nShift - データ数の log2
                sinTable - 円周をデータ数で等分した時の sin の値のテーブル

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    MATHi_FFT(fx32 *data, u32 nShift, const fx16 *sinTable);

/*---------------------------------------------------------------------------*
  Name:         MATHi_IFFT

  Description:  高速フーリエ変換の逆変換を行う内部関数

  Arguments:    data - 偶数番目に実部、奇数番目に虚部を収めた変換を行うデータ。
                       変換結果は上書きされて返される。
                nShift - データ数の log2
                sinTable - 円周をデータ数で等分した時の sin の値のテーブル

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    MATHi_IFFT(fx32 *data, u32 nShift, const fx16 *sinTable);

/*---------------------------------------------------------------------------*
  Name:         MATH_FFT

  Description:  高速フーリエ変換を行う

  Arguments:    data - 偶数番目に実部、奇数番目に虚部を収めた変換を行うデータ。
                       変換結果は上書きされて返される。
                nShift - データ数の log2
                sinTable - 円周をデータ数で等分した時の sin の値のテーブル

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    MATH_FFT(fx32 *data, u32 nShift, const fx16 *sinTable);

/*---------------------------------------------------------------------------*
  Name:         MATH_IFFT

  Description:  高速フーリエ変換の逆変換を行う

  Arguments:    data - 偶数番目に実部、奇数番目に虚部を収めた変換を行うデータ。
                       変換結果は上書きされて返される。
                nShift - データ数の log2
                sinTable - 円周をデータ数で等分した時の sin の値のテーブル

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    MATH_IFFT(fx32 *data, u32 nShift, const fx16 *sinTable);

/*---------------------------------------------------------------------------*
  Name:         MATH_FFTReal

  Description:  高速フーリエ変換を行う

  Arguments:    data - 実部のみを収めたデータ。
                       変換結果は上書きされて返される。
                nShift - データ数の log2
                sinTable - 円周をデータ数で等分した時の sin の値のテーブル

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    MATH_FFTReal(fx32 *data, u32 nShift, const fx16 *sinTable, const fx16 *sinTable2);

/*---------------------------------------------------------------------------*
  Name:         MATH_IFFTReal

  Description:  高速フーリエ変換の逆変換を行う

  Arguments:    data - 実部のみを収めたデータ。
                       変換結果は上書きされて返される。
                nShift - データ数の log2
                sinTable - 円周をデータ数で等分した時の sin の値のテーブル

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    MATH_IFFTReal(fx32 *data, u32 nShift, const fx16 *sinTable, const fx16 *sinTable2);


#ifdef __cplusplus
}/* extern "C" */
#endif

/* NITRO_MATH_FFT_H_ */
#endif
