/*---------------------------------------------------------------------------*
  Project:  NitroDWC - bm - dwc_bm_init
  File:     dwc_bm_init.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc_bm_init.h,v $
  Revision 1.2  2005/08/20 09:39:28  akimaru
  #define DWC_INIT_WORK_SIZE 0x700

  Revision 1.1  2005/08/20 07:01:20  sasakit
  ヘッダインクルードガードの書式を統一した。
  bm/dwc_init.h -> bm/dwc_bm_init.hに変更
  ヘッダにCopyrightをつけた。
  構造体の名前空間をできるだけルールにちかづけた。
  util_wifiidtool.hとdwc_backup.hの関係を若干修正。


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef DWC_BM_INIT_H_
#define DWC_BM_INIT_H_

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
// 定義
//=============================================================================
// 初期化関数ワークメモリサイズ
#define DWC_BM_INIT_WORK_SIZE 0x700
#define DWC_INIT_WORK_SIZE 0x700

//=============================================================================
// 関数
//=============================================================================
//-----------------------------------------------------------------------------
// 機能 : 初期化関数
//        ※ ワークメモリは DWC_INIT_WORK_SIZE 以上でかつ、
//           32 バイトアラインされている必要があります
//        ※ 関数終了後にワークメモリは破棄してかまいません
// 引数 : work  - ワークメモリ
// 返値 : int   -      0 : 正常終了
//                -10002 : 接続先設定が消失したが正常終了
//                -10003 : ユーザ ID が消失したが正常終了
//                -10000 : DS バックアップメモリの書き込みエラー終了
//                -10001 : DS バックアップメモリの読み込みエラー終了
//-----------------------------------------------------------------------------
int  DWC_BM_Init(void* work);

#ifdef __cplusplus
}
#endif

#endif // DWC_BM_INIT_H_
