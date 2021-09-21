/*---------------------------------------------------------------------------*
  Project:  NitroDWC -  - 
  File:     dwc_nd.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc_nd.h,v $
  Revision 1.8  2006/09/05 05:37:09  takayama
  DWC_NdCleanupAsync関数の返り値を void から BOOL に変更した。

  Revision 1.7  2006/05/15 01:18:19  nakata
  5/12版のDWC_Ndライブラリに対応

  Revision 1.6  2006/05/12 11:25:10  nakata
  大木さんの指摘に対応

  Revision 1.5  2006/05/10 11:59:21  nakata
  Ndライブラリの仕様変更に伴って初期化関数を変更

  Revision 1.4  2006/05/02 00:27:15  nakata
  DWC_Ndを5月2日版に更新

  Revision 1.3  2006/04/28 12:26:58  nakata
  DWC_ND_PARAMERRのコンパイル時warning抑制

  Revision 1.2  2006/04/28 10:09:02  nakata
  DWC_NdInitAsyncにpassword追加
  DWC_NdGetFileAsyncの仕様書変更に伴う関数変更
  各種定数をガイドラインに沿ったものに変更するヘッダを追加

  Revision 1.1  2006/04/25 11:46:56  nakata
  DWC_Ndラッパを追加



  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef DWC_ND_H_
#define DWC_ND_H_

#include <nd/DWCi_Nd.h>

#ifdef __cplusplus
extern "C" {
#endif


/* -------------------------------------------------------------------------
        define
   ------------------------------------------------------------------------- */

//


/* -------------------------------------------------------------------------
        enum
   ------------------------------------------------------------------------- */

//


/* -------------------------------------------------------------------------
        typedef - function
   ------------------------------------------------------------------------- */

//


/* -------------------------------------------------------------------------
        struct
   ------------------------------------------------------------------------- */

//


/* -------------------------------------------------------------------------
        function - external
   ------------------------------------------------------------------------- */

/*---------------------------------------------------------------------------*
  Name:			DWC_NdInitAsync

  Description:	Ndライブラリ初期化関数

  Arguments:	callback - Ndライブラリが呼び出す通知コールバック関数へのポインタ
                gamecd   - ダウンロードサーバへのアクセスに必要なゲームコード
                passwd   - ダウンロードサーバへのアクセスに必要なパスワード
  Returns:		TRUE  - 初期化成功
				FALSE - 初期化失敗
 *---------------------------------------------------------------------------*/
BOOL DWC_NdInitAsync(DWCNdCallback callback, char *gamecd, char *passwd);

/*---------------------------------------------------------------------------*
  Name:			DWC_NdCleanupAsync

  Description:	Ndライブラリ解放関数

  Arguments:	callback - Ndライブラリが呼び出す解放完了通知コールバック関数へのポインタ
  Returns:		なし
 *---------------------------------------------------------------------------*/
BOOL DWC_NdCleanupAsync(DWCNdCleanupCallback callback);

/*---------------------------------------------------------------------------*
  Name:			DWC_NdSetAttr

  Description:	ダウンロード時属性設定関数

  Arguments:	attr1 - 属性文字列1
				attr2 - 属性文字列2
				attr3 - 属性文字列3
  Returns:		TRUE  - 属性設定成功
				FALSE - 属性設定失敗
 *---------------------------------------------------------------------------*/
BOOL DWC_NdSetAttr(const char* attr1, const char* attr2, const char* attr3);

/*---------------------------------------------------------------------------*
  Name:			DWC_NdGetFileListNumAsync

  Description:	ファイルリスト総エントリ数取得処理開始関数

  Arguments:	entrynum - 通信完了時にファイルリスト総エントリ数が格納される領域へのポインタ
  Returns:		TRUE  - 通信が開始され処理完了待ち状態
				FALSE - 処理失敗
 *---------------------------------------------------------------------------*/
BOOL DWC_NdGetFileListNumAsync(int* entrynum);

/*---------------------------------------------------------------------------*
  Name:			DWC_NdFileListAsync

  Description:	ファイルリスト取得処理開始関数

  Arguments:	filelist - 取得されたファイルリストが格納される配列へのポインタ
				offset   - 取得するファイルリストの開始位置のオフセット
				num      - 取得するファイルリストの最大長
  Returns:		TRUE  - 通信が開始され処理完了待ち状態
				FALSE - 処理失敗
 *---------------------------------------------------------------------------*/
BOOL DWC_NdGetFileListAsync(DWCNdFileInfo* filelist, unsigned int offset, unsigned int num);

/*---------------------------------------------------------------------------*
  Name:			DWC_NdGetFileAsync

  Description:	ファイル取得処理開始関数

  Arguments:	filename - 取得するファイルの名前
				buf      - 取得されたファイルのデータが格納される領域へのポインタ
				size     - 取得されたファイルのデータが格納される領域のサイズ
  Returns:		TRUE  - 通信が開始され処理完了待ち状態
				FALSE - 処理失敗
 *---------------------------------------------------------------------------*/
BOOL DWC_NdGetFileAsync(DWCNdFileInfo *fileinfo, char* buf, unsigned int size);

/*---------------------------------------------------------------------------*
  Name:			DWC_NdCancelAsync

  Description:	実行中処理の中断通知関数

  Arguments:	なし
  Returns:		TRUE  - 処理中断に成功し、処理完了待ち状態
				FALSE - 処理失敗
 *---------------------------------------------------------------------------*/
BOOL DWC_NdCancelAsync(void);

/*---------------------------------------------------------------------------*
  Name:			DWC_NdGetProgress

  Description:	実行中処理の進行度取得関数

  Arguments:	received   - 受信済みデータのバイト数を格納する領域へのポインタ
				contentlen - 受信中のデータのContent-Lengthを格納する領域へのポインタ
  Returns:		TRUE  - 進行度取得成功
				FALSE - 進行度取得失敗、あるいは処理を実行していない
 *---------------------------------------------------------------------------*/
BOOL DWC_NdGetProgress(u32* received, u32* contentlen);

#ifdef __cplusplus
}
#endif

#endif // DWC_ND_H_
