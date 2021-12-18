/*---------------------------------------------------------------------------*
  Project:  NitroSDK - DGT - include
  File:     dgt.h

  Copyright 2003-2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dgt.h,v $
  Revision 1.10  2005/09/08 11:35:39  yasu
  SDK_FROM_TOOL の対応

  Revision 1.9  2005/07/14 02:49:03  seiki_masashi
  些細な修正

  Revision 1.8  2005/07/14 02:32:02  seiki_masashi
  コンパイルスイッチの整理

  Revision 1.7  2005/07/08 12:34:33  seiki_masashi
  HASH2 アルゴリズムの高速化

  Revision 1.6  2005/06/27 00:12:05  seiki_masashi
  C の標準型に修正

  Revision 1.5  2005/06/24 01:16:46  seiki_masashi
  旧実装を削除

  Revision 1.4  2005/06/24 01:10:28  seiki_masashi
  MD5 の実装を変更

  Revision 1.3  2005/03/29 03:59:26  seiki_masashi
  Copyright の修正

  Revision 1.2  2005/03/29 00:07:23  seiki_masashi
  C の標準型に修正

  Revision 1.1  2005/03/28 04:18:34  seiki_masashi
  common.h の一部を dgt.h に移動

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_DGT_DGT_H_
#define NITRO_DGT_DGT_H_

#include <nitro/dgt/common.h>

#ifdef __cplusplus
extern "C" {
#endif

// コンテキスト構造体の宣言
typedef struct DGTHash1Context
{
    union
    {
        struct
        {
            unsigned long a, b, c, d;
        };
        unsigned long state[4];
    };
    unsigned long long length;
    union
    {
        unsigned long buffer32[16];
        unsigned char buffer8[64];
    };
} DGTHash1Context;

#if defined(SDK_DGT_HASH2_CODE_SAFE) || defined(SDK_WIN32) || defined(SDK_FROM_TOOL)
typedef struct DGTHash2Context
{
    unsigned long Intermediate_Hash[5];
    unsigned long Length_Low;
    unsigned long Length_High;
    int Message_Block_Index;
    unsigned char Message_Block[64];
    int Computed;
    int Corrupted;
} DGTHash2Context;
#else
typedef struct DGTHash2Context
{
    unsigned long h0,h1,h2,h3,h4;
    unsigned long Nl,Nh;
    int num;
    unsigned long data[64/4];
    int dummy[2];
} DGTHash2Context;
#endif // defined(SDK_DGT_HASH2_CODE_SAFE) || defined(SDK_WIN32) || defined(SDK_FROM_TOOL)


// Hash を算出
#ifdef SDK_DGT_HASH1_CODE_SAFE
void DGT_Hash1Reset(DGTHash1Context*);
void DGT_Hash1SetSource(DGTHash1Context*, unsigned char*, unsigned long);
void DGT_Hash1GetDigest_R(unsigned char digest[16], DGTHash1Context*);
#else
void DGT_Hash1Reset(DGTHash1Context*);
void DGT_Hash1SetSource(DGTHash1Context*, const void*, unsigned long);
void DGT_Hash1GetDigest_R(void* digest, DGTHash1Context*);
#endif

#if defined(SDK_DGT_HASH2_CODE_SAFE) || defined(SDK_WIN32) || defined(SDK_FROM_TOOL)
void DGT_Hash2Reset(DGTHash2Context*);
void DGT_Hash2SetSource(DGTHash2Context*, unsigned char*, unsigned long);
void DGT_Hash2GetDigest(DGTHash2Context*, unsigned char digest[20]);
#else
void DGT_Hash2Reset(DGTHash2Context*);
void DGT_Hash2SetSource(DGTHash2Context*, const unsigned char*, unsigned long);
void DGT_Hash2GetDigest(DGTHash2Context*, unsigned char* digest);
#endif


// HMACを算出
void	DGT_Hash1CalcHmac(
                    void*   digest,     //  ダイジェスト出力領域
		    void*   bin_ptr,    //  データへのポインタ
		    int     bin_len,    //  データの長さ
		    void*   key_ptr,    //  キーへのポインタ
		    int     key_len     //  キーの長さ
		    );

void	DGT_Hash2CalcHmac(
                    void*   digest,     //  ダイジェスト出力領域
		    void*   bin_ptr,    //  データへのポインタ
		    int     bin_len,    //  データの長さ
		    void*   key_ptr,    //  キーへのポインタ
		    int     key_len     //  キーの長さ
		    );

//  RomHeader、main、subの３領域からHMAC値を生成
int     DGT_Hash1CalcHmacForRms(
                    void*   digest,     //  生成ダイジェスト領域
                    void*   romh_ptr,   //  データへのポインタ
                    int     romh_len,   //  データの長さ
                    void*   mbin_ptr,   //  データへのポインタ
                    int     mbin_len,   //  データの長さ
                    void*   sbin_ptr,   //  データへのポインタ
                    int     sbin_len,   //  データの長さ
                    void*   key_ptr,    //  キーへのポインタ
                    int     key_len     //  キーの長さ
                    );

int     DGT_Hash2CalcHmacForRms(
                    void*   digest,     //  生成ダイジェスト領域
                    void*   romh_ptr,   //  データへのポインタ
                    int     romh_len,   //  データの長さ
                    void*   mbin_ptr,   //  データへのポインタ
                    int     mbin_len,   //  データの長さ
                    void*   sbin_ptr,   //  データへのポインタ
                    int     sbin_len,   //  データの長さ
                    void*   key_ptr,    //  キーへのポインタ
                    int     key_len     //  キーの長さ
                    );

//  RomHeader、main、subの３領域からHMAC値を確認
int     DGT_Hash1TestHmacForRms(
                    void*   digest,     //  確認ダイジェスト領域
                    void*   romh_ptr,   //  データへのポインタ
                    int     romh_len,   //  データの長さ
                    void*   mbin_ptr,   //  データへのポインタ
                    int     mbin_len,   //  データの長さ
                    void*   sbin_ptr,   //  データへのポインタ
                    int     sbin_len,   //  データの長さ
                    void*   key_ptr,    //  キーへのポインタ
                    int     key_len     //  キーの長さ
                    );

int     DGT_Hash2TestHmacForRms(
                    void*   digest,     //  確認ダイジェスト領域
                    void*   romh_ptr,   //  データへのポインタ
                    int     romh_len,   //  データの長さ
                    void*   mbin_ptr,   //  データへのポインタ
                    int     mbin_len,   //  データの長さ
                    void*   sbin_ptr,   //  データへのポインタ
                    int     sbin_len,   //  データの長さ
                    void*   key_ptr,    //  キーへのポインタ
                    int     key_len     //  キーの長さ
                    );

int     DGT_SetOverlayTableMode( int flag );

#ifdef __cplusplus
}
#endif
#endif
