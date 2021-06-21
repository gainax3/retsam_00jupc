/*---------------------------------------------------------------------------*
  Project:  NitroDWC - auth - dwc_http
  File:     dwc_http.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc_http.h,v $
  Revision 1.49  2006/11/07 04:49:50  nakata
  名前解決後のIPアドレスを取得できるように変更

  Revision 1.48  2006/03/10 04:07:15  nakata
  DWC_Httpが128文字以上のURLでフリーズする問題を修正

  Revision 1.47  2006/03/02 05:21:58  nakata
  DWC_Svcモジュール追加
  DWC_Util_Alloc/DWC_Util_Free関数の名前をDWC_Auth_Alloc/DWC_Auth_Free関数に名前を変更

  Revision 1.46  2005/12/21 12:02:19  nakata
  Rollback Auth related code to versiont "2005-11-21 08:00:00"
  Fixed retry times count bug in DWC_Auth.

  Revision 1.31  2005/11/17 12:17:16  nakata
  Resurrected old memory management functions.
  DWC_Netcheck sends entire HTTP packet from Wayport router to NAS.

  Revision 1.30  2005/11/17 07:52:34  nakata
  Now DWC_Auth/DWC_Netcheck/DWC_Http don't use DWC_Alloc/DWC_Free.
  DWC_Netcheck sends whole HTTP packet including HTTP header to NAS
  when receives 302 message from conntest.

  Revision 1.29  2005/11/15 06:29:47  nakata
  Now, DWC_Auth sends ESSID when the NDS connects to Hotspot AP.
  Removed unneccesary file "util_alloc.c".

  Revision 1.28  2005/11/15 01:43:26  nakata
  DWC_Http now uses DWC_Alloc/DWC_Free in base library for memory management.
  For this modification, Makefiles for AcSample/UtilSample were modified.

  Revision 1.27  2005/11/15 01:05:47  nakata
  DWC_Http now recognises "http://xxx:yyy/zzz" style URL that has custom port number.

  Revision 1.26  2005/10/18 00:09:11  sasakit
  DWCのauth接続時にスレッドプライオリティを下げる機能をやめた。

  Revision 1.25  2005/10/17 06:33:14  sasakit
  SSL実行中のスレッドプライオリティを調整する機能を有効化した。

  Revision 1.24  2005/10/07 13:10:53  nakata
  Added SSL priority setting code to DWC_Http. But disabled by DWC_HTTP_SETSSLPRIO switch.
  Fixed ingamesn length detection part. It used strlen and now it uses wcslen for ingamesn.

  Revision 1.23  2005/09/14 10:36:47  nakata
  To fix stuck bug that can be occured when DWC_Netcheck is aborted, modified DWC_Http.
  Modified DWC_Http not to use OS_DisableIrq/RestoreIrq.
  Modified DWC_Http to use CPS_SocGetLength().
  Refine debug outputs.

  Revision 1.22  2005/09/12 06:19:29  nakata
  Added DWC_Netcheck_SetCustomBlacklist function.
  Refined DWC_Http timeout/receive finish detection.
  Modified some Makefile for successful compilation.

  Revision 1.21  2005/09/07 02:47:00  nakata
  Added progress notification function to DWC_Http.

  Revision 1.20  2005/09/05 23:34:51  nakata
  Added interface function for getting loading progress persentage to DWC_Http.

  Revision 1.19  2005/09/05 02:11:29  nakata
  Now, DWC_Http can download data to pre-allocated receive buffer.

  Revision 1.18  2005/08/22 11:48:02  nakata
  Add runtime custom nas setting function to DWC_Auth.

  Revision 1.17  2005/08/20 07:01:18  sasakit
  ヘッダインクルードガードの書式を統一した。
  bm/dwc_init.h -> bm/dwc_bm_init.hに変更
  ヘッダにCopyrightをつけた。
  構造体の名前空間をできるだけルールにちかづけた。
  util_wifiidtool.hとdwc_backup.hの関係を若干修正。


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef DWC_HTTP_H_
#define DWC_HTTP_H_

#include <nitroWiFi.h>
#include <nitrowifi/version.h>

#include "util_alloc.h"
#include "util_base64.h"

#ifdef __cplusplus
extern "C" {
#endif // [nakata] __cplusplus

//=============================================================================
// [nakata] 定義
//=============================================================================

// [nakata] URLの最大長
#define DWCHTTP_MAXURL			256

// [nakata] スタックサイズ
#define DWCHTTP_STACKSIZE		4096

#define DWCHTTP_MAX_LABELVALUE	32

// [nakata] SSLプライオリティ設定をONにするときはdefineすること
// [nakata] ただし都合の悪いSVC_WaitVBlankIntr()がないことを十分に確認すること！
//#define DWC_HTTP_SETSSLPRIO // [sasakit] 予想以上にブロックするのでやめた。

typedef enum
{
	DWCHTTP_E_NOERR,
	
	DWCHTTP_E_MEMERR,
	DWCHTTP_E_DNSERR,
	DWCHTTP_E_CONERR,
	DWCHTTP_E_SENDTOUT,
	DWCHTTP_E_SENDERR,
	DWCHTTP_E_RECVTOUT,
	DWCHTTP_E_ABORT,
	
	DWCHTTP_E_FINISH,
	DWCHTTP_E_MAX
} DWCHttpError;

// [nakata] POSTかGETかを決定するフラグ
typedef enum
{
	DWCHTTP_POST,
	DWCHTTP_GET
} DWCHttpAction;

// [nakata] バッファをまとめて管理するための構造体
typedef struct {
	char *buffer;
	char *write_index;
	char *buffer_tail;
	unsigned long length;
} DWCHttpBuffer;

// [nakata] ラベルと結果の文字列を対にして管理するための構造体
typedef struct
{
	char *label;
	char *value;
} DWCHttpLabelValue;

// [nakata] ラベルの塊を管理するための構造体
typedef struct
{
	DWCHttpLabelValue *entry;
	int len;
	int index;
} DWCHttpParseResult;

// [nakata] DWCHttp初期化パラメータ
typedef struct {
	const char		*url;
	DWCHttpAction	action;
	unsigned long	len_recvbuf;
	
	DWCAuthAlloc	alloc;
	DWCAuthFree		free;
	
	BOOL			ignoreca;
	
	int				timeout;
} DWCHttpParam;

// [nakata] DWCHttpが使用する内部情報を格納するための構造体

typedef struct {
	unsigned char	stack[DWCHTTP_STACKSIZE];
	
	// [nakata] 初期化フラグ
	u8				initflag;
	
	// [nakata] 初期化パラメータ
	DWCHttpParam	param;
	DWCHttpError	error;					// [nakata] 最後に発生したエラーコード
	
	// [nakata] ホスト/SSL関連
	char			url[DWCHTTP_MAXURL];	// [nakata] URL保存用配列
	char			*hostname;				// [nakata] ホスト名へのポインタ(url内のどこかを指す)
	char			*filepath;				// [nakata] ファイルパスへのポインタ(url内のどこかを指す)
	u32				hostip;					// [nakata] 名前解決の結果得られたIPアドレス
	BOOL			ssl_enabled;				// [nakata] SSLを使用するかしないかのフラグ
	unsigned short		port;
	
	// [nakata] CPSソケット関連
	CPSSoc				soc;
	CPSSslConnection	con;
	unsigned char		*lowrecvbuf;//[1460*2];
	unsigned char		*lowsendbuf;//[1514];
	u32					lowentropydata[OS_LOW_ENTROPY_DATA_SIZE/sizeof(u32)];
	
	// [nakata] 送受信バッファポインタなど
	int				num_postitem;
	DWCHttpBuffer	req;
	DWCHttpBuffer	rep;
	
	// [nakata] よく使う値を保持する領域
	OSMutex			content_len_mutex;
	int				content_len;
	int				receivedbody_len;
	
	// [nakata] 受信データパース結果格納領域
	DWCHttpLabelValue	labelvalue[DWCHTTP_MAX_LABELVALUE];

	// [nakata] 送受信スレッド関連
	OSThread		thread;
	OSMutex			mutex;
	BOOL			abort;
} DWCHttp;

//=============================================================================
// [nakata] 関数
//=============================================================================

//-----------------------------------------------------------------------------
// 機能 : DWC_Httpで使用する構造体の初期化/メモリ確保
// 引数 : http              - 処理対象のDWC_Http構造体へのポインタ
//      : max_recv_bodysize - 受信する最大のパケット長
//      : alloc             - 内部で使用するメモリ確保関数へのポインタ
//      : free              - 内部で使用するメモリ解放関数へのポインタ
// 返値 : BOOL  - TRUE  : 成功
//                FALSE : 失敗
//-----------------------------------------------------------------------------
DWCHttpError DWC_Http_Create(DWCHttp *http, DWCHttpParam *param);

//-----------------------------------------------------------------------------
// 機能 : DWC_Httpで使用したメモリ解放/ソケットクローズ/SSL解放
// 引数 : http  - 処理対象のDWC_Http構造体へのポインタ
// 返値 : BOOL  - TRUE  : 成功
//                FALSE : 失敗
//-----------------------------------------------------------------------------
void DWC_Http_Destroy(DWCHttp *http);

//-----------------------------------------------------------------------------
// 機能 : DWCHttpスレッド作成関数
//        
// 引数 : http - 処理対象のDWC_Http構造体へのポインタ
//      : prio - HTTPスレッド優先度
// 返値 : DWCHttpError
//-----------------------------------------------------------------------------
void DWC_Http_StartThread(DWCHttp *http, u32 prio);

//-----------------------------------------------------------------------------
// 機能 : DWCHttpアボート
//        
// 引数 : http  - 処理対象のDWC_Http構造体へのポインタ
// 返値 : なし
//-----------------------------------------------------------------------------
void DWC_Http_Abort(DWCHttp *http);

//-----------------------------------------------------------------------------
// 機能 : DWCHttp受信情報取得
//        
// 引数 : http - 処理対象のDWC_Http構造体へのポインタ
// 返値 : int  - 0から255 受信パーセンテージ
//             - -1       受信中でない
//-----------------------------------------------------------------------------
int DWC_Http_GetRecvProgress(DWCHttp *http);

//-----------------------------------------------------------------------------
// 機能 : 送信ヘッダに項目を追加する
// 引数 : http  - 処理対象のDWC_Http構造体へのポインタ
//      : label - ラベル(NULL終端している必要がある)
//      : data  - データ(NULL終端している必要がある)
// 返値 : DWCHttpError
//-----------------------------------------------------------------------------
DWCHttpError DWC_Http_Add_HeaderItem(DWCHttp *http, char *label, char *data);

//-----------------------------------------------------------------------------
// 機能 : 送信POST情報にBase64エンコードした項目を追加する
// 引数 : http      - 処理対象のDWC_Http構造体へのポインタ
//      : label     - ラベル(NULL終端している必要がある)
//      : data      - データ(NULL終端していなくてもよい)
//      : data_len  - データ長
// 返値 : DWCHttpError
//-----------------------------------------------------------------------------
DWCHttpError DWC_Http_Add_PostBase64Item(DWCHttp *http, const char *label, const char *data, unsigned long data_len);

//-----------------------------------------------------------------------------
// 機能 : 送信パケットのBODYに指定した文字列を追加する
// 引数 : http      - 処理対象のDWC_Http構造体へのポインタ
//      : data      - データ(NULL終端されていなければならない)
//      : data_len  - データ長
// 返値 : DWCHttpError
//-----------------------------------------------------------------------------
DWCHttpError DWC_Http_Add_Body(DWCHttp *http, const char *data);

//-----------------------------------------------------------------------------
// 機能 : DWCHttp内の送信パケットにContent-Lengthフィールドを追加し送信可能状態にする
//        
// 引数 : http  - 処理対象のDWC_Http構造体へのポインタ
// 返値 : DWCHttpError
//-----------------------------------------------------------------------------
DWCHttpError DWC_Http_FinishHeader(DWCHttp *http);

//-----------------------------------------------------------------------------
// 機能 : 受信パケットのヘッダエントリ/FORM情報文字列をラベル付き文字列としてarray_entryに格納
//        受信パケットの内容はヌル文字で破壊される
//        
// 引数 : http - 処理対象のDWCHttpへのポインタ
//        noparsebody - TRUEならばHTTP BODYをパースせずにhttpbodyエントリとして登録する
//        buffer      - 解析対象のHTTPパケット
// 返値 : BOOL - TRUE  : 解析成功 
//               FALSE : 解析不可能なHTTP
//-----------------------------------------------------------------------------
BOOL DWC_Http_ParseResult(DWCHttp *http, BOOL noparsebody);

//-----------------------------------------------------------------------------
// 機能 : DWCHttpのTCP接続時プライオリティを変化させる
// 引数 : prio - プライオリティ(0から31)
// 返値 : なし
//-----------------------------------------------------------------------------
#ifdef DWC_HTTP_SETSSLPRIO
void DWC_Http_SetSSLPrio(u32 prio);
#endif

//-----------------------------------------------------------------------------
// パース結果操作系関数群
//-----------------------------------------------------------------------------
char	*DWC_Http_GetResult(DWCHttp *http, char *label);
int		DWC_Http_GetBase64DecodedResult(DWCHttp *http, char *label, char *buffer, unsigned long buffer_len);
BOOL	DWC_Http_GetRawResult(DWCHttp *http, char *label, char *buffer, int buffer_len);

#ifdef __cplusplus
}
#endif // [nakata] __cplusplus

#endif // DWC_HTTP_H_
