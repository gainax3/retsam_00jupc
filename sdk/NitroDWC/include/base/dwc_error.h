/*---------------------------------------------------------------------------*
  Project:  NitroDWC - base - dwc_error
  File:     dwc_error.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc_error.h,v $
  Revision 1.38  2007/04/06 13:02:13  takayama
  不正文字列チェック機能のエラー定義を見直し。

  Revision 1.37  2007/03/28 09:43:46  nakata
  DWC_ECODE_TYPE_PROFに関する修正。

  Revision 1.36  2006/06/22 02:03:17  nakata
  ダウンロードライブラリのバッファ不足エラーを取り除いた。(DWCErrorの種類を増やしたくないため)

  Revision 1.35  2006/06/22 00:49:17  nakata
  ダウンロードライブラリのバッファ不足エラーを追加

  Revision 1.34  2006/06/01 04:48:49  takayama
  コンパイル時にワーニングが出る問題を修正。

  Revision 1.33  2006/05/29 01:00:42  nakata
  DWC_Nd関連で必要のないエラーコードを削除

  Revision 1.32  2006/05/25 09:06:53  nakata
  DWC_ERROR_SVL_HTTPを追加

  Revision 1.31  2006/05/25 09:05:36  takayama
  GHTTPライブラリのエラーをFriendsMatchライブラリのエラーと分けて、独立性を
  高めた。

  Revision 1.30  2006/05/25 05:09:50  nakata
  DWC_Nd/DWC_SVLにDWCエラーを返す機能を追加したことに伴う変更

  Revision 1.29  2006/05/17 00:50:34  nakata
  DWC_Nd用エラー定数名の変更

  Revision 1.28  2006/04/28 12:25:13  nakata
  定数名変更に伴う修正

  Revision 1.27  2006/04/28 08:16:51  nakata
  Ndサーバエラー用番号を削除(必要がなくなった)

  Revision 1.26  2006/04/27 07:33:04  nakata
  Ndライブラリ向けUNKNOWNエラーコードを変更

  Revision 1.25  2006/04/27 05:15:25  nakata
  Ndライブラリ用のエラーコードを追加

  Revision 1.24  2006/04/26 10:52:17  nakata
  拡張機能用のエラーコードを追加

  Revision 1.23  2006/01/23 04:50:43  arakit
  統一用語に合わせてコメントを修正

  Revision 1.22  2005/12/26 23:59:55  arakit
  コメントの間違いを修正

  Revision 1.21  2005/10/24 12:14:49  arakit
  サーバクライアント型マッチングで、サーバが定員オーバーであるというエラー
  DWC_ERROR_SERVER_FULLを追加した。

  Revision 1.20  2005/10/20 09:59:45  sasakit
  GT2関係のエラーを追加。

  Revision 1.19  2005/10/14 07:43:23  onozawa
  ghttp用のエラーコードを追加

  Revision 1.18  2005/10/14 06:58:31  arakit
  ghttp用のエラーコードをさらに追加

  Revision 1.17  2005/10/14 06:41:37  arakit
  ghttp用のエラーコードを追加

  Revision 1.16  2005/10/11 04:08:18  arakit
  エラー処理タイプも取得できる関数DWC_GetLastErrorEx()を追加した。

  Revision 1.15  2005/10/05 08:29:43  arakit
  サーバクライアントマッチング時に、マッチング完了後、マッチングをブロックし、ブロックされた
  クライアントに、専用のエラーを返すマッチングオプションを追加した。

  Revision 1.14  2005/10/03 05:06:49  arakit
  DWC_ECODE_SEQ_TRANSPORTをDWC_ECODE_SEQ_ETCに変更した。

  Revision 1.13  2005/09/30 02:49:04  arakit
  サーバクライアント型の接続失敗エラーコードを追加した。

  Revision 1.12  2005/09/28 12:40:54  arakit
  NATネゴシエーション規定回数失敗のエラーを追加した。

  Revision 1.11  2005/09/24 12:57:10  arakit
  コネクション数オーバのエラーを追加した。

  Revision 1.10  2005/09/13 07:35:19  sasakit
  エラーステートにDWC_ERROR_DISCONNECTEDを追加。

  Revision 1.9  2005/09/06 09:12:38  sasakit
  ConnectInet時のエラーの処理を追加。

  Revision 1.8  2005/09/01 12:15:16  arakit
  エラーのコメントを修正

  Revision 1.7  2005/08/29 06:34:51  arakit
  gt2のsocketErrorCallback関数を作成した。

  Revision 1.6  2005/08/26 08:15:22  arakit
  ・ASSERTとモジュール制御変数のチェックを整理した。
  ・外部関数DWC_ShutdownFriendsMatch()の引数をとらないように変更した。
  また、同関数から各モジュールの制御変数をNULLクリアするようにした。
  ・エラー処理関数を変更し、エラーセット、エラーチェックを必要な箇所に入れた。

  Revision 1.5  2005/08/20 07:01:19  sasakit
  ヘッダインクルードガードの書式を統一した。
  bm/dwc_init.h -> bm/dwc_bm_init.hに変更
  ヘッダにCopyrightをつけた。
  構造体の名前空間をできるだけルールにちかづけた。
  util_wifiidtool.hとdwc_backup.hの関係を若干修正。


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef DWC_ERROR_H_
#define DWC_ERROR_H_

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------
// define
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// enum
//----------------------------------------------------------------------------
// エラー列挙子
typedef enum {
    DWC_ERROR_NONE = 0,            // エラーなし
    DWC_ERROR_DS_MEMORY_ANY,       // 本体セーブデータに関するエラー
    DWC_ERROR_AUTH_ANY,            // 認証に関するエラー
    DWC_ERROR_AUTH_OUT_OF_SERVICE, // サービス終了
    DWC_ERROR_AUTH_STOP_SERVICE,   // サービス一時中断
    DWC_ERROR_AC_ANY,              // 自動接続に関するエラー
    DWC_ERROR_NETWORK,             // その他のネットワークエラー
    DWC_ERROR_GHTTP_ANY,           // GHTTP(HTTP通信)に関するエラー
    DWC_ERROR_DISCONNECTED,        // 切断された
    DWC_ERROR_FATAL,               // 致命的なエラー

    // [arakit] main 051011
    // シーケンス上の軽度のエラー（DWCErrorType = DWC_ETYPE_LIGHT）
    DWC_ERROR_FRIENDS_SHORTAGE,    // 指定の人数の有効な友達がいないのに友達指定ピアマッチメイクを始めようとした
    DWC_ERROR_NOT_FRIEND_SERVER,   // サーバクライアントマッチメイクで、サーバに指定した友達が相互に友達ではない、もしくはサーバを立てていない
    DWC_ERROR_MO_SC_CONNECT_BLOCK, // サーバクライアントマッチメイクのマッチメイクオプションで、サーバから接続を拒否された
    // [arakit] main 051011
    // [arakit] main 051024
    DWC_ERROR_SERVER_FULL,         // サーバクライアントマッチメイクのサーバが定員オーバー
	
	// [nakata] Ndライブラリ用のエラー種別追加
	DWC_ERROR_ND_ANY,
	DWC_ERROR_ND_HTTP,
	
	// [nakata] SVLライブラリ用のエラー種別追加
	DWC_ERROR_SVL_ANY,
	DWC_ERROR_SVL_HTTP,
    
    // [nakata] 不正文字列チェック用のエラー種別追加
    DWC_ERROR_PROF_ANY,
    DWC_ERROR_PROF_HTTP,
        
    DWC_ERROR_NUM
} DWCError;

// [arakit] main 051010
// エラー検出時に必要な処理を示すエラータイプ
typedef enum {
    DWC_ETYPE_NO_ERROR = 0,   // エラーなし
    DWC_ETYPE_LIGHT,          // ゲーム固有の表示のみで、エラーコード表示は必要ない
    DWC_ETYPE_SHOW_ERROR,     // エラーコード表示のみを行う
    DWC_ETYPE_SHUTDOWN_FM,    // DWC_ShutdownFriendsMatch()を呼び出して、FriendsMatchライブラリを終了する必要がある。
                              // エラーコードの表示も必要。
    DWC_ETYPE_SHUTDOWN_GHTTP, // DWC_ShutdownGHTTP()を呼び出して、GHTTPライブラリを終了する必要がある。
                              // エラーコードの表示も必要。
    DWC_ETYPE_SHUTDOWN_ND,    // DWC_NdCleanupAsync()を呼び出してDWC_Ndライブラリを終了する必要がある。
                              // エラーコードの表示も必要
    DWC_ETYPE_DISCONNECT,     // FriendsMatch処理中ならDWC_ShutdownFriendsMatch()を呼び出し、更にDWC_CleanupInet()で通信の切断も行う必要がある。
                              //エラーコードの表示も必要。
    DWC_ETYPE_FATAL,          // Fatal Error 相当なので、電源OFFを促す必要がある。
                              // エラーコードの表示も必要。
    DWC_ETYPE_NUM
} DWCErrorType;
// [arakit] main 051010

// デバッグ用dwc_baseエラーコード各要素
enum {
    // どのシーケンス中に起こったエラーか
    DWC_ECODE_SEQ_LOGIN          = (-60000),  // ログイン処理でのエラー
    DWC_ECODE_SEQ_FRIEND         = (-70000),  // 友達管理処理でのエラー
    DWC_ECODE_SEQ_MATCH          = (-80000),  // マッチメイク処理でのエラー
    DWC_ECODE_SEQ_ETC            = (-90000),  // 上記以外の処理でのエラー

    // GameSpyのエラーの場合、どのSDKが出したエラーか
    DWC_ECODE_GS_GP              =  (-1000),  // GameSpy GP のエラー
    DWC_ECODE_GS_PERS            =  (-2000),  // GameSpy Persistent のエラー
    DWC_ECODE_GS_STATS           =  (-3000),  // GameSpy Stats のエラー
    DWC_ECODE_GS_QR2             =  (-4000),  // GameSpy QR2 のエラー
    DWC_ECODE_GS_SB              =  (-5000),  // GameSpy Server Browsing のエラー
    DWC_ECODE_GS_NN              =  (-6000),  // GameSpy Nat Negotiation のエラー
    DWC_ECODE_GS_GT2             =  (-7000),  // GameSpy gt2 のエラー
    DWC_ECODE_GS_HTTP            =  (-8000),  // GameSpy HTTP のエラー
    DWC_ECODE_GS_ETC             =  (-9000),  // GameSpy その他のSDKのエラー

    // 通常エラー種別
    DWC_ECODE_TYPE_NETWORK       =   ( -10),  // ネットワーク障害
    DWC_ECODE_TYPE_SERVER        =   ( -20),  // GameSpyサーバ障害
    DWC_ECODE_TYPE_DNS           =   ( -30),  // DNS障害
    DWC_ECODE_TYPE_DATA          =   ( -40),  // 不正なデータを受信
    DWC_ECODE_TYPE_SOCKET        =   ( -50),  // ソケット通信エラー
    DWC_ECODE_TYPE_BIND          =   ( -60),  // ソケットのバインドエラー
    DWC_ECODE_TYPE_TIMEOUT       =   ( -70),  // タイムアウト発生
    DWC_ECODE_TYPE_PEER          =   ( -80),  // １対１の通信で不具合
    DWC_ECODE_TYPE_CONN_OVER     =   (-100),  // コネクション数オーバ
    DWC_ECODE_TYPE_STATS_AUTH    =   (-200),  // STATSサーバログインエラー
    DWC_ECODE_TYPE_STATS_LOAD    =   (-210),  // STATSサーバデータロードエラー
    DWC_ECODE_TYPE_STATS_SAVE    =   (-220),  // STATSサーバデータセーブエラー
    DWC_ECODE_TYPE_NOT_FRIEND    =   (-400),  // 指定された相手が相互に登録された友達ではない
    DWC_ECODE_TYPE_OTHER         =   (-410),  // 相手が通信切断した（主にサーバクライアントマッチメイク時）
    DWC_ECODE_TYPE_MUCH_FAILURE  =   (-420),  // NATネゴシエーションに規定回数以上失敗した
    DWC_ECODE_TYPE_SC_CL_FAIL    =   (-430),  // サーバクライアントマッチメイク・クライアントの接続失敗
    DWC_ECODE_TYPE_CLOSE         =   (-600),  // コネクションクローズ時のエラー
    DWC_ECODE_TYPE_TRANS_HEADER  =   (-610),  // ありえない受信ステートでReliableデータを受信した
    DWC_ECODE_TYPE_TRANS_BODY    =   (-620),  // 受信バッファオーバーフロー
    DWC_ECODE_TYPE_AC_FATAL      =   (-700),  // AC処理中のACでのエラーグループ。この場合下位桁は専用のstate値。
    DWC_ECODE_TYPE_OPEN_FILE     =   (-800),  // GHTTPファイルオープン失敗
    DWC_ECODE_TYPE_INVALID_POST  =   (-810),  // GHTTP無効な送信
    DWC_ECODE_TYPE_REQ_INVALID   =   (-820),  // GHTTPファイル名等無効
    DWC_ECODE_TYPE_UNSPECIFIED   =   (-830),  // GHTTP詳細不明のエラー
    DWC_ECODE_TYPE_BUFF_OVER     =   (-840),  // GHTTPバッファオーバーフロー
    DWC_ECODE_TYPE_PARSE_URL     =   (-850),  // GHTTP URLの解析エラー
    DWC_ECODE_TYPE_BAD_RESPONSE  =   (-860),  // GHTTPサーバからのレスポンス解析エラー
    DWC_ECODE_TYPE_REJECTED      =   (-870),  // GHTTPサーバからのリクエスト等拒否
    DWC_ECODE_TYPE_FILE_RW       =   (-880),  // GHTTPローカルファイルRead/Writeエラー
    DWC_ECODE_TYPE_INCOMPLETE    =   (-890),  // GHTTPダウンロード中断
    DWC_ECODE_TYPE_TO_BIG        =   (-900),  // GHTTPファイルサイズが大きすぎてダウンロード不可
    DWC_ECODE_TYPE_ENCRYPTION    =   (-910),  // GHTTPエンクリプションエラー
	
    // Fatal Error専用種別
    DWC_ECODE_TYPE_ALLOC         =     (-1),  // メモリ確保失敗
    DWC_ECODE_TYPE_PARAM         =     (-2),  // パラメータエラー
    DWC_ECODE_TYPE_SO_SOCKET     =     (-3),  // SOのエラーが原因のGameSpy gt2のソケットエラー
    DWC_ECODE_TYPE_NOT_INIT      =     (-4),  // ライブラリが初期化されていない。
    DWC_ECODE_TYPE_DUP_INIT      =     (-5),  // ライブラリが二度初期化された。
    DWC_ECODE_TYPE_WM_INIT       =     (-6),  // WMの初期化に失敗した。
    DWC_ECODE_TYPE_UNEXPECTED    =     (-9),  // 予期しない状態、もしくはUnkwonなGameSpyエラー発生

	// 30000台エラー番号の定義(拡張機能用)
	DWC_ECODE_SEQ_ADDINS        =   (-30000), // 拡張機能エラー
	
	DWC_ECODE_FUNC_ND            =   (-1000), // DWC_Nd用領域
    DWC_ECODE_FUNC_PROF          =   (-3000), // 不正文字列チェック用領域
	
	DWC_ECODE_TYPE_ND_ALLOC      =   ( -1), // メモリ確保失敗
	DWC_ECODE_TYPE_ND_FATAL      =   ( -9), // 致命的エラー
	DWC_ECODE_TYPE_ND_BUSY       =   (-10), // ダウンロード中に新規リクエストを要求した
	DWC_ECODE_TYPE_ND_HTTP       =   (-20), // HTTP通信に失敗
	DWC_ECODE_TYPE_ND_BUFFULL    =   (-30), // HTTP通信バッファが不十分
	DWC_ECODE_TYPE_ND_SERVER     =   (  0), // サーバがエラーコードを返してきた

	DWC_ECODE_TYPE_PROF_CONN     =   ( -10),// 不正文字列チェックでサーバとの接続に問題があった
	DWC_ECODE_TYPE_PROF_PARSE    =   ( -20),// 不正文字列チェックでサーバの応答が正しくなかった
	DWC_ECODE_TYPE_PROF_OTHER    =   ( -30) // 不正文字列チェックでその他のエラーが発生した
};


//----------------------------------------------------------------------------
// function - external
//----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*
  エラー取得関数
  引数　：errorCode エラーコード格納先ポインタ。必要なければNULLを渡せば良い。
  戻り値：エラー種別
  用途　：最後の通信エラー種別、エラーコードを取得する
 *---------------------------------------------------------------------------*/
extern DWCError DWC_GetLastError(int* errorCode);


// [arakit] main 051011
/*---------------------------------------------------------------------------*
  拡張版エラー取得関数
  引数　：errorCode エラーコード格納先ポインタ。必要なければNULLを渡せば良い。
          errorType エラー処理タイプ格納先ポインタ。必要なければNULLを渡せば良い。
  戻り値：エラー種別
  用途　：最後の通信エラー種別、エラーコード、エラー処理タイプを取得する
 *---------------------------------------------------------------------------*/
extern DWCError DWC_GetLastErrorEx(int* errorCode, DWCErrorType* errorType);
// [arakit] main 051011


/*---------------------------------------------------------------------------*
  エラークリア関数
  引数　：なし
  戻り値：なし
  用途　：通信エラーの記録をクリアする。Fatal Errorはクリアできない。
 *---------------------------------------------------------------------------*/
extern void DWC_ClearError(void);


//----------------------------------------------------------------------------
// function - internal
//----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*
  エラーチェック関数
  引数　：なし
  戻り値：TRUE:エラーあり、FALSE:エラーなし
  用途　：通信エラーが発生したかどうかをチェックする
 *---------------------------------------------------------------------------*/
extern BOOL DWCi_IsError(void);


/*---------------------------------------------------------------------------*
  エラーセット関数
  引数　：error     エラー種別
          errorCode エラーの詳細を示すエラーコード
  戻り値：なし
  用途　：通信エラーの発生を記録する。Fatal Errorは上書きできない。
 *---------------------------------------------------------------------------*/
extern void DWCi_SetError(DWCError error, int errorCode);


#ifdef __cplusplus
}
#endif


#endif // DWC_ERROR_H_
