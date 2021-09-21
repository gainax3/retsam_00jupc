/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MATH - 
  File:     math/dgt.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dgt.h,v $
  Revision 1.8  2006/01/18 02:11:19  kitase_hirotake
  do-indent

  Revision 1.7  2005/04/13 11:58:43  seiki_masashi
  GetDigest -> GetHash

  Revision 1.6  2005/04/11 10:34:25  seiki_masashi
  MATH_CalcMD5, MATH_CalcSHA1 関数の追加

  Revision 1.5  2005/04/04 01:23:08  seiki_masashi
  small fix

  Revision 1.4  2005/04/01 12:29:14  seiki_masashi
  u8* digest を void* digest に変更

  Revision 1.3  2005/04/01 07:45:37  seiki_masashi
  HMAC 関連関数の変更

  Revision 1.2  2005/03/29 09:30:35  seiki_masashi
  公開用に整理

  Revision 1.1  2005/03/28 04:20:27  seiki_masashi
  math/dgt.h を追加

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_MATH_DGT_H_
#define NITRO_MATH_DGT_H_

#include <nitro/misc.h>
#include <nitro/types.h>
#include <nitro/dgt/common.h>
#include <nitro/dgt/dgt.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------
// Constant definition
//----------------------------------------------------------------------------

// ダイジェスト長
#define MATH_MD5_DIGEST_SIZE    DGT_HASH1_DIGEST_SIZE   // 128bit
#define MATH_SHA1_DIGEST_SIZE   DGT_HASH2_DIGEST_SIZE   // 160bit

// 処理ブロック長
#define MATH_MD5_BLOCK_SIZE     DGT_HASH_BLOCK_SIZE     // 512bit
#define MATH_SHA1_BLOCK_SIZE    DGT_HASH_BLOCK_SIZE     // 512bit

//----------------------------------------------------------------------------
// Type definition
//----------------------------------------------------------------------------

typedef DGTHash1Context MATHMD5Context;
typedef DGTHash2Context MATHSHA1Context;

//----------------------------------------------------------------------------
// Declaration of function
//----------------------------------------------------------------------------

/*****************************************************************************/
/* MD5                                                                       */
/*****************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         MATH_MD5Init

  Description:  MD5 値を求めるために使う MATHMD5Context 構造体を初期化する。
  
  Arguments:    context MATHMD5Context 構造体
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void MATH_MD5Init(MATHMD5Context * context)
{
    DGT_Hash1Reset(context);
}

/*---------------------------------------------------------------------------*
  Name:         MATH_MD5Update

  Description:  MD5 値を与えたデータで更新する。
  
  Arguments:    context MATHMD5Context 構造体
                input   入力データへのポインタ
                length  入力データ長
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void MATH_MD5Update(MATHMD5Context * context, const void *input, u32 length)
{
    DGT_Hash1SetSource(context, (u8 *)input, length);
}

/*---------------------------------------------------------------------------*
  Name:         MATH_MD5GetHash

  Description:  最終的な MD5 値を得る。
  
  Arguments:    context MATHMD5Context 構造体
                digest  MD5 値を格納する場所へのポインタ
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void MATH_MD5GetHash(MATHMD5Context * context, void *digest)
{
    DGT_Hash1GetDigest_R((u8 *)digest, context);
}

// 下位互換のため
static inline void MATH_MD5GetDigest(MATHMD5Context * context, void *digest)
{
    MATH_MD5GetHash(context, digest);
}


/*****************************************************************************/
/* SHA-1                                                                     */
/*****************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         MATH_SHA1Init

  Description:  SHA1 値を求めるために使う MATHSHA1Context 構造体を初期化する。
  
  Arguments:    context MATHSHA1Context 構造体
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void MATH_SHA1Init(MATHSHA1Context * context)
{
    DGT_Hash2Reset(context);
}

/*---------------------------------------------------------------------------*
  Name:         MATH_SHA1Update

  Description:  SHA1 値を与えたデータで更新する。
  
  Arguments:    context MATHSHA1Context 構造体
                input   入力データへのポインタ
                length  入力データ長
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void MATH_SHA1Update(MATHSHA1Context * context, const void *input, u32 length)
{
    DGT_Hash2SetSource(context, (u8 *)input, length);
}

/*---------------------------------------------------------------------------*
  Name:         MATH_SHA1GetHash

  Description:  最終的な SHA1 値を得る。
  
  Arguments:    context MATHSHA1Context 構造体
                digest  SHA1 値を格納する場所へのポインタ
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void MATH_SHA1GetHash(MATHSHA1Context * context, void *digest)
{
    DGT_Hash2GetDigest(context, (u8 *)digest);
}

// 下位互換のため
static inline void MATH_SHA1GetDigest(MATHSHA1Context * context, void *digest)
{
    MATH_SHA1GetHash(context, digest);
}


/*****************************************************************************/
/* ユーティリティ関数                                                        */
/*****************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         MATH_CalcMD5

  Description:  MD5 を計算する。
  
  Arguments:    digest  MD5 値を格納する場所へのポインタ
                data    入力データのポインタ
                dataLength  入力データ長
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void    MATH_CalcMD5(void *digest, const void *data, u32 dataLength);

/*---------------------------------------------------------------------------*
  Name:         MATH_CalcSHA1

  Description:  SHA-1 を計算する。
  
  Arguments:    digest  SHA-1 値を格納する場所へのポインタ
                data    入力データのポインタ
                dataLength  入力データ長
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void    MATH_CalcSHA1(void *digest, const void *data, u32 dataLength);


/*---------------------------------------------------------------------------*
  Name:         MATH_CalcHMACMD5

  Description:  HMAC-MD5 を計算する。
  
  Arguments:    digest  HMAC-MD5 値を格納する場所へのポインタ
                data    入力データのポインタ
                dataLength  入力データ長
                key     鍵のポインタ
                keyLength   鍵の長さ
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void
MATH_CalcHMACMD5(void *digest, const void *data, u32 dataLength, const void *key, u32 keyLength)
{
    DGT_Hash1CalcHmac(digest, (void *)data, (int)dataLength, (void *)key, (int)keyLength);
}

/*---------------------------------------------------------------------------*
  Name:         MATH_CalcHMACSHA1

  Description:  HMAC-SHA-1 を計算する。
  
  Arguments:    digest  HMAC-SHA-1 値を格納する場所へのポインタ
                data    入力データのポインタ
                dataLength  入力データ長
                key     鍵のポインタ
                keyLength   鍵の長さ
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void
MATH_CalcHMACSHA1(void *digest, const void *data, u32 dataLength, const void *key, u32 keyLength)
{
    DGT_Hash2CalcHmac(digest, (void *)data, (int)dataLength, (void *)key, (int)keyLength);
}

#ifdef __cplusplus
}/* extern "C" */
#endif

/* NITRO_MATH_DGT_H_ */
#endif
