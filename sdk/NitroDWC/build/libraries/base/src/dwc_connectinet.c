/*---------------------------------------------------------------------------*
  Project:  NitroDWC - base - connectinet
  File:     dwc_connectinet.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc_connectinet.c,v $
  Revision 1.55  2006/12/28 08:22:31  nakata
  DWC_TESTINET_NOT_INITIALIZEDが正しく返されない問題があったため修正した。

  Revision 1.54  2006/11/22 12:29:18  nakata
  DWC_TestInetProcessでエラーコードを設定するように変更

  Revision 1.53  2006/11/17 06:54:02  nakata
  TestInet関連の定数の名称変更

  Revision 1.52  2006/11/07 04:53:58  nakata
  DWC_TestInet系の追加

  Revision 1.51  2006/10/06 12:02:16  takayama
  DWC_AC_GetApType()の値チェックを追加した。

  Revision 1.50  2006/07/27 01:03:02  nakata
  DWC_UpdateConnectionでエラーコードを540xxに設定するように変更

  Revision 1.49  2006/06/27 06:43:52  nakata
  FriendsMatchライブラリなし用のDCF監視関数を追加

  Revision 1.48  2006/06/01 07:48:55  nakata
  FINALROMビルド時のワーニングを抑制

  Revision 1.47  2006/05/25 23:54:11  uemura
  DWC_CheckWiFiStation関数を追加

  Revision 1.46  2006/04/17 07:33:40  nakata
  ブラウザ用ホットスポット設定関数DWC_EnableHotSpotを追加

  Revision 1.45  2006/03/09 08:05:52  uemura
  DWC_SetConnectApTypeの引数をintからDWCApInfoTypeへ変更

  Revision 1.44  2006/03/09 07:35:13  nakata
  DWCApInfoからspotinfoを消去し、関連部分を変更

  Revision 1.43  2006/03/09 06:07:41  uemura
  DWC_DebugConnectInetAsyncの引数WEPに16進０値が入っていると正しく判定できないバグを修正
  引数にWEPMODEを渡すように変更しました

  Revision 1.42  2006/03/09 04:38:38  uemura
  コメント修正

  Revision 1.41  2006/03/09 02:50:37  uemura
  コメント修正

  Revision 1.40  2006/03/09 02:27:17  uemura
  DWC_SetConnectApType,DWC_DebugConnectInetAsyncの追加
  この2関数をdwc_connect_inet_sampleへコンパイルスイッチで使用できるように追加する

  Revision 1.39  2006/02/24 10:31:29  sotoike
  DWC_CleanupInet 関数においてNULL ポインタへのアクセスの可能性があるのを修正。

  Revision 1.38  2006/02/23 08:21:56  nakata
  DWCApInfo周りの定数名等を変更。

  Revision 1.37  2006/02/23 06:48:34  nakata
  DWC_GetApInfo関数が不正なデータを返すバグを修正。

  Revision 1.36  2006/02/23 04:32:07  sotoike
  (none)

  Revision 1.35  2006/02/23 02:55:40  nakata
  DWC_GetApInfo関数関連のマニュアルを追加。

  Revision 1.34  2006/02/23 01:02:19  nakata
  AP情報取得関数DWC_GetApInfo内のバグを修正

  Revision 1.33  2006/02/23 00:53:54  nakata
  AP情報取得関数DWC_GetApInfoをdwc_connectinet.cに追加
  dwc_sampleの不必要なデバッグメッセージ表示を抑制

  Revision 1.32  2006/02/22 05:10:36  sotoike
  stpInetCntl がNULLの時(= DWC_InitInet を呼んでいないとき)
  DWC_ProcessInet をコールするとハングアップする問題を修正
  Revision 1.31  2006/02/22 04:29:44  sotoike
  DWC_InitInet と DWC_ConnectInetAsync の間で DWC_GetInetStatus を呼ぶと
  NULLポインタアクセスでハングアップしていた問題を修正。

  Revision 1.30  2006/01/23 04:50:26  arakit
  統一用語に合わせてコメントを修正

  Revision 1.29  2005/12/27 09:29:54  arakit
  DWC_GetUdpPacketDropNum()を、FINALROMビルドでも正常に動作するようにした。

  Revision 1.28  2005/11/07 06:18:30  sasakit
  GsStatus->OwnStatusに変更。

  Revision 1.27  2005/10/18 00:08:25  sasakit
  DWCのauth接続時にスレッドプライオリティを下げる機能をやめた。

  Revision 1.26  2005/10/17 07:31:44  sasakit
  CPSのSSL実行中のスレッドプライオリティを調整する機能を削除した。

  Revision 1.25  2005/10/17 06:35:56  sasakit
  SSL実行中のスレッドプライオリティを調整する機能を有効化した。(include追加)
  Warning削除

  Revision 1.24  2005/10/17 06:34:33  sasakit
  SSL実行中のスレッドプライオリティを調整する機能を有効化した。

  Revision 1.23  2005/10/13 08:14:09  arakit
  主要な外部関数呼び出し時に、レポートレベルDWC_REPORTFLAG_DEBUGで
  デバッグプリント表示を行うようにした。

  Revision 1.22  2005/10/11 02:00:12  sasakit
  接続中にエラーが発生したときに、正常にCleanupInetできない問題を修正。
  SOCL_CalmDown()がSOC_Startup()よりも前に呼べない問題の修正。(呼び出しをACに移動)

  Revision 1.21  2005/10/06 05:59:34  sasakit
  SSLの多倍長累乗計算スレッドの優先順位を下げる機能を外部から調整できるようにした。

  Revision 1.20  2005/10/05 07:10:11  sasakit
  SSLの多倍長累乗計算スレッドの優先順位を下げる関数の追加。

  Revision 1.19  2005/10/01 14:57:33  sasakit
  DWC_GetInetStatus()がNULL ptrアクセスでとまる問題を修正。

  Revision 1.18  2005/10/01 12:07:01  sasakit
  ConnectInet中の処理に問題があったので、修正。

  Revision 1.17  2005/10/01 10:56:44  sasakit
  DWC_CleanupInetAsync()中に、DWC_ProcessInet()を呼んでもDISCONNECTEDにならないようにした。

  Revision 1.16  2005/09/29 09:06:53  sasakit
  DWC_CleanupInetAsync()をなるべくブロックしないようにした。（SOCL_CalmDown()対応）

  Revision 1.15  2005/09/27 13:22:41  sasakit
  接続結果（エラー状態）がDWC_GetLastError()に入らなかった問題を修正。

  Revision 1.14  2005/09/20 10:56:51  sasakit
  元に戻した。
  >> conntest.nintendowifi.netが落ちたので、www.nintendo.co.jpを参照するようにした。

  Revision 1.13  2005/09/19 18:57:35  sasakit
  conntest.nintendowifi.netが落ちたので、www.nintendo.co.jpを参照するようにした。

  Revision 1.12  2005/09/17 00:32:28  sasakit
  UDPパケットを落とした数を取得する関数の追加。
  DWC_GetLinkLevel()の実装を修正。

  Revision 1.11  2005/09/14 01:55:00  sasakit
  APからの切断処理を整理した。

  Revision 1.10  2005/09/13 07:29:16  sasakit
  PowerModeをACTIVEに必ずするようにした。

  Revision 1.9  2005/09/07 07:13:34  sasakit
  DWC_AC関数群を厳密に呼び出すようにステートマシンを入れた。
  DWC_InitInet()を二重で呼び出したときに、エラーにしないようにした。（無視するようにした）

  Revision 1.8  2005/09/06 09:12:09  sasakit
  ConnectInet時のエラーの処理を追加。

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>
#include <nitroWiFi/socl.h>

#include <ac/dwc_ac.h>
#include <base/dwc_connectinet.h>
#include <base/dwc_memfunc.h>
#include <base/dwc_report.h>
#include <base/dwc_error.h>

#include <auth/dwc_auth.h>
#include <auth/dwc_netcheck.h>

/** --------------------------------------------------------------------
  defines
  ----------------------------------------------------------------------*/
#define DWC_CONNECTINET_DMA_DEFAULT      3
#define DWC_CONNECTINET_POWERMODE_SAVE   0
#define DWC_CONNECTINET_POWERMODE_ACTIVE 1

#define DWC_CONNECTINET_SSL_DEFAULT_PRIORITY 20

/** --------------------------------------------------------------------
  statics
  ----------------------------------------------------------------------*/
static DWCInetControl* stpInetCntl;

/*---------------------------------------------------------------------------*
  Name:         DWC_InitInetEx

  Description:  インターネット接続管理の初期化

  Arguments:    inetCntl     : インターネット接続管理用の構造体へのポインタ
                dmaNo        : 使用するdma番号(wmで使用)
                powerMode    : WMのパワーモードの設定
                ssl_priority : ssl threadのプライオリティ

  Returns:      なし
 *---------------------------------------------------------------------------*/
void
DWC_InitInetEx( DWCInetControl* inetCntl, u16 dmaNo, u16 powerMode, u16 ssl_priority )
{
#ifdef SDK_FINALROM
#pragma unused(powerMode)
#endif
    SDK_ASSERT( powerMode == DWC_CONNECTINET_POWERMODE_ACTIVE );

    if ( stpInetCntl == NULL )
    {
        DWC_Printf( DWC_REPORTFLAG_CONNECTINET, "InitInet dma : %d, powerMode: %d\n", dmaNo, powerMode );

        SDK_ASSERT( inetCntl );

        MI_CpuClear8( inetCntl, sizeof( DWCInetControl ) );

        inetCntl->dmaNo = dmaNo;
        inetCntl->powerMode = DWC_CONNECTINET_POWERMODE_ACTIVE;

        inetCntl->state = DWC_CONNECTINET_STATE_IDLE;
        inetCntl->online = FALSE;

        inetCntl->wontExecuteWDS = FALSE;
        inetCntl->processWDSstate = DWC_WDS_STATE_IDLE;

#ifndef SDK_FINALROM
		inetCntl->connectType = 0;
#endif	// SDK_FINALROM

        stpInetCntl = inetCntl;

#if SDK_WIFI_VERSION_DATE >= 20051005
        CPS_SetSslLowThreadPriority( ssl_priority );
#else
        (void)ssl_priority;
#endif
    }
}

/*---------------------------------------------------------------------------*
  Name:         DWC_InitInet

  Description:  インターネット接続管理の初期化

  Arguments:    inetCntl  : インターネット接続管理用の構造体へのポインタ

  Returns:      なし
 *---------------------------------------------------------------------------*/
void
DWC_InitInet( DWCInetControl* inetCntl )
{
    DWC_InitInetEx( inetCntl, DWC_CONNECTINET_DMA_DEFAULT, DWC_CONNECTINET_POWERMODE_ACTIVE, DWC_CONNECTINET_SSL_DEFAULT_PRIORITY );
}

/*---------------------------------------------------------------------------*
  Name:         DWC_SetAuthServer

  Description:  認証に使用するサーバの変更

  Arguments:    type : 認証サーバのタイプ

  Returns:      なし
 *---------------------------------------------------------------------------*/
void
DWC_SetAuthServer( DWCInetAuthType type )
{
    switch ( type )
    {
    case DWC_CONNECTINET_AUTH_TEST:
        DWC_Auth_SetCustomNas("https://nas.test.nintendowifi.net/ac");
        break;

    case DWC_CONNECTINET_AUTH_DEVELOP:
        DWC_Printf(DWC_REPORTFLAG_AC,"******************************************************************************************\n");
        DWC_Printf(DWC_REPORTFLAG_AC," DWC_CONNECTINET_AUTH_DEVELOP: This setting is for authentication server developing ONLY. \n");
        DWC_Printf(DWC_REPORTFLAG_AC,"******************************************************************************************\n");
        DWC_Auth_SetCustomNas("https://nas.dev.nintendowifi.net/ac");
        break;

    case DWC_CONNECTINET_AUTH_RELEASE:
        DWC_Auth_SetCustomNas("https://nas.nintendowifi.net/ac");
        break;
    }
}

/*---------------------------------------------------------------------------*
  Name:         DWC_SetConnectApType

  Description:  インターネットに接続する際のAPタイプを指定する

  Arguments:    type : 接続するAPのタイプ

  Returns:      なし
 *---------------------------------------------------------------------------*/
#ifndef SDK_FINALROM
void
DWC_SetConnectApType( DWCApInfoType type )
{
    // 初期化してない場合、デバッグ用関数ということもあるのでエラー処理を行わずアサートさせる
    SDK_ASSERTMSG( stpInetCntl, "DWC_InitInet[Ex] is not called." );
    
    // nintendo spot はタイプ指定に対応していない
    SDK_ASSERTMSG( 0 <= type && type <= DWC_APINFO_TYPE_NINTENDOWFC, "Invalid argument 'type'" );
    
    DWC_Printf(DWC_REPORTFLAG_AC,"***************************************************************\n");
    DWC_Printf(DWC_REPORTFLAG_AC," DWC_CONNECTINET_SET_AP_TYPE: This setting is debug ONLY. \n");
    DWC_Printf(DWC_REPORTFLAG_AC,"***************************************************************\n");
    
	// 接続するAPのtypeを指定する
	stpInetCntl->connectType = type + 1;
    
    // nintendo spotへは接続しない
    stpInetCntl->wontExecuteWDS = TRUE;
}
#endif	// SDK_FINALROM

/*---------------------------------------------------------------------------*
  Name:         DWC_ConnectInetAsync

  Description:  インターネットに接続する。

  Arguments:    なし

  Returns:      なし
 *---------------------------------------------------------------------------*/
void
DWC_ConnectInetAsync( void )
{
    DWCACConfig inp;

    if ( stpInetCntl != NULL )
    {
        if ( stpInetCntl->state != DWC_CONNECTINET_STATE_IDLE )
        {
            return;
            // NOT REACH HERE
        }

        MI_CpuClear8( &inp, sizeof(DWCACConfig) );

        inp.dmaNo     = (u8)stpInetCntl->dmaNo;       //DMA番号
        inp.powerMode = (u8)stpInetCntl->powerMode;   //０：SAVE １：ACTIVE
        inp.alloc     = (DWCACAlloc)DWC_Alloc;    //自動接続処理で使用するalloc関数のポインタ
        inp.free      = (DWCACFree) DWC_Free;     //自動接続処理で使用するfree関数のポインタ

#ifndef SDK_FINALROM
		// 接続APタイプの指定
		inp.option.connectType = (u8)stpInetCntl->connectType;
#endif	// SDK_FINALROM

        DWC_Printf( DWC_REPORTFLAG_CONNECTINET, "ConnectInet Async\n" );

        stpInetCntl->state = DWC_CONNECTINET_STATE_OPERATING;

        if ( !DWC_AC_Create(&inp) )
        {
            // WCM_の初期化に失敗すると、ここに来る。
            DWCi_SetError( DWC_ERROR_FATAL, -50199 );
        }
    }
    else
    {
        // 初期化してない。
        DWCi_SetError( DWC_ERROR_FATAL, -50199 );
    }
}

/*---------------------------------------------------------------------------*
  Name:         DWC_DebugConnectInetAsync

  Description:  APを指定してインターネットに接続する。

  Arguments:    ssid    : 接続するAPのSSID
                wep     : 接続するAPのWEPキー
                wepMode : WEPキーのモード

  Returns:      なし
 *---------------------------------------------------------------------------*/
void
DWC_DebugConnectInetAsync( const void *ssid, const void *wep, int wepMode )
{
    DWC_DebugConnectInetExAsync( ssid, wep, wepMode, NULL, DWC_AC_AP_TYPE_USER1 );
}

void
DWC_DebugConnectInetExAsync( const void *ssid, const void *wep, int wepMode, const char *apSpotInfo, int overrideType )
{
	DWC_ConnectInetAsync();

    // nintendo spot へは接続しない
    if ( stpInetCntl != NULL )
    {
        stpInetCntl->wontExecuteWDS = TRUE;
    }
    
	if( DWC_GetInetStatus() == DWC_CONNECTINET_STATE_OPERATING )
	{
		// 接続先を再設定する
        DWC_AC_SetSpecifyApEx( ssid, wep, wepMode, apSpotInfo, overrideType );
	}
}

/*---------------------------------------------------------------------------*
  Name:         DWC_CheckInet

  Description:  インターネットに接続する処理の進行をチェックする。

  Arguments:    なし

  Returns:      TRUE  : 処理完了
                FALSE : 処理中
 *---------------------------------------------------------------------------*/
BOOL
DWC_CheckInet( void )
{
    if ( stpInetCntl == NULL )
    {
        DWC_Printf( DWC_REPORTFLAG_CONNECTINET, "ConnectInet library is not initialized.\n" );

        return FALSE;
    }

    if ( stpInetCntl->ac_state != 0 )
    {
        stpInetCntl->state = DWC_CONNECTINET_STATE_OPERATED;

        (void)DWC_GetInetStatus(); // 結果をDWC_GetLastError()に格納。

        if ( stpInetCntl->wontExecuteWDS )
            return TRUE; // WDSへの試行を行わないなら終了 [masuda]
        else
            return FALSE;
    }

    return FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         DWC_ProcessInet

  Description:  インターネットに接続する処理を実行する。

  Arguments:    なし

  Returns:      なし
 *---------------------------------------------------------------------------*/
void
DWC_ProcessInet( void )
{
  if( stpInetCntl == NULL){ return; } // DWC_InitInet が呼ばれてない段階

    // WDSの処理中だったらWDSのプロセスをコールしてreturn
    // 完了したら、得られたAP情報で接続を開始する。[masuda]
    switch( stpInetCntl->processWDSstate )
    {
        case DWC_WDS_STATE_IDLE:
            break;
        case DWC_WDS_STATE_PROCESS:
            stpInetCntl->ac_state = 0; // 処理中とする。　本来はDWC_AC_Process関数で取得
            stpInetCntl->processWDSstate = DWC_AC_ProcessGetWDSInfo();
            return;
        case DWC_WDS_STATE_COMPLETED:
            DWC_AC_CleanupGetWDSInfo();
            DWC_DebugConnectInetExAsync(
                stpInetCntl->nspotInfo.ssid,
                stpInetCntl->nspotInfo.wep,
                stpInetCntl->nspotInfo.wepMode,
                stpInetCntl->nspotInfo.apnum,
                DWC_AC_AP_TYPE_NINTENDOSPOT
                );
            stpInetCntl->processWDSstate = DWC_WDS_STATE_IDLE;
            return;
        case DWC_WDS_STATE_FAILED:
            // AP情報の取得に失敗した場合は、DWC_GetInetStatus関数でエラーセットの処理をする。
            // DWC_CheckInetからDWC_GetInetStatusを呼んでもらうために ac_state の変更が必要。
            DWC_AC_CleanupGetWDSInfo();
            stpInetCntl->ac_state = -1; //　エラー発生とする。 本来はDWC_AC_Process関数で取得
            stpInetCntl->wontExecuteWDS = TRUE; // DWC_CheckInetで処理終了とハンドルされるように
            return;
        case DWC_WDS_STATE_ERROR:
            return;
        default:
            break;
    }// WDS処理中は、以下の処理に進ませない。[masuda]

    if ( stpInetCntl->state == DWC_CONNECTINET_STATE_OPERATING )
    {
        stpInetCntl->ac_state = DWC_AC_Process();
    }
    else if ( stpInetCntl->state == DWC_CONNECTINET_STATE_CONNECTED )
    {
        // DCFの接続チェック
        if ( stpInetCntl->online )
        {
            if ( WCM_GetPhase() != WCM_PHASE_DCF )
            {
                // 切断された。
                stpInetCntl->online = FALSE;
                stpInetCntl->state = DWC_CONNECTINET_STATE_DISCONNECTED;
            }
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         DWC_GetInetStatus

  Description:  インターネットに接続する処理が完了した後に、状態を取得する。

  Arguments:    なし

  Returns:      ステータス
 *---------------------------------------------------------------------------*/
DWCInetResult
DWC_GetInetStatus( void )
{
  int state;
  static int stateOld; // WDSでエラーコードを引き継ぐためのstate保存用変数 [masuda]
  DWCInetResult result = DWC_CONNECTINET_STATE_NOT_INITIALIZED;

  if ( stpInetCntl == NULL ){ return (result); } // 未初期化

    // WDSの処理中であった場合、DWC_CONNECTINET_STATE_OPERATINGとする。[masuda]
    switch( stpInetCntl->processWDSstate )
    {
        case DWC_WDS_STATE_IDLE:
            break;
        case DWC_WDS_STATE_PROCESS:
            return DWC_CONNECTINET_STATE_OPERATING;
        case DWC_WDS_STATE_COMPLETED:
            // 成功した場合は、DWC_ProcessInetで接続処理を始める。
            return DWC_CONNECTINET_STATE_OPERATING;
        case DWC_WDS_STATE_FAILED:
            // 通常の接続で発生したエラーをセットして終了
            DWC_ProcessInet(); // DWC_GetInetStatus()がProcessより先に呼ばれた場合の対応(解放処理等)            
            DWCi_SetError( DWC_ERROR_AC_ANY, stateOld );
            result = DWC_CONNECTINET_STATE_ERROR;
            stpInetCntl->state = result;
            stpInetCntl->processWDSstate = DWC_WDS_STATE_ERROR;        
            return result;
        case DWC_WDS_STATE_ERROR: // WDSがエラーで終了し、解放処理が終わったときは常にエラーを返す。
            result = DWC_CONNECTINET_STATE_ERROR;
            return result;
        default:
            break;
    }// WDS処理中は、以下の処理に進ませない。[masuda]

  result =  (DWCInetResult)stpInetCntl->state;
  
  if(result == DWC_CONNECTINET_STATE_IDLE){ return (result); } // この段階ではまだDWC_AC_GetStatus()を呼べない

  state = DWC_AC_GetStatus();

  if ( state ==  DWC_AC_STATE_COMPLETE )
	{
	  // 接続済み。
	  result = DWC_CONNECTINET_STATE_CONNECTED;
	  stpInetCntl->state = result;
	  stpInetCntl->online = TRUE;
	  stpInetCntl->wontExecuteWDS = TRUE; // 成功したらもうWDS接続は行わない。[masuda]
	}
  else if ( state < 0 )
	{
	  // エラー処理。
	  if ( state >= -10 )
		{
		  // FATALエラー
		  DWCi_SetError( DWC_ERROR_FATAL, -50199 );
		  
		  result = DWC_CONNECTINET_STATE_FATAL_ERROR;
		  stpInetCntl->state = result;
		}
      else if ( !stpInetCntl->wontExecuteWDS )
        {
          // APが見つからなかった。もしくはFatal以外のエラーで
          // 接続できなかったので、NintendoSpotに接続を試みる。この試行は1度きり[masuda]
          // **** WDSの処理では、DWC_ProcessInet関数で処理が終わったとハンドリグされないように
          // **** ac base のステートを偽る必要がある。
          
          // Init関連情報を一時保存
          DWCInetControl *stepInetCntlTemp = stpInetCntl;
          u16 dmaNoTemp = stpInetCntl->dmaNo;
          u32 ssl_priority_Temp = CPS_GetSslLowThreadPriority();
          
          // エラーコードを一時保存
          stateOld = state;
          
          // 開放処理。
          DWC_CleanupInet();
          
          // 再初期化(これをしないとstpInetCntlが使えない)
          DWC_InitInetEx( stepInetCntlTemp, dmaNoTemp, DWC_CONNECTINET_POWERMODE_ACTIVE, (u16)ssl_priority_Temp);
          
          // NintendoSpotの情報 ssid wep wepmode を取得
          DWC_AC_StartupGetWDSInfo( &(stpInetCntl->nspotInfo) );
          stpInetCntl->processWDSstate = DWC_WDS_STATE_PROCESS;
          
          // 処理中ということにする。
          result = DWC_CONNECTINET_STATE_OPERATING;
          stpInetCntl->ac_state = 0; // 処理中とする。　本来はDWC_AC_Process関数で取得
        }
	  else
		{
		  // ACでエラー発生。
          
          // 通常の接続で発生したエラーと、
          // その後のWDSへの接続で発生したエラーで、より深いシーケンスのエラーをセットする
          if( stateOld < state)
          {
            state = stateOld;
          }
		  
		  DWCi_SetError( DWC_ERROR_AC_ANY, state );
		  
		  result = DWC_CONNECTINET_STATE_ERROR;
		  stpInetCntl->state = result;
		}
	}
  else
	{
	  // 処理中。
	  result = DWC_CONNECTINET_STATE_OPERATING;
	}
  
  return result;
}

/*---------------------------------------------------------------------------*
  Name:         DWC_CleanupInet

  Description:  インターネットに接続を終了し、メモリを開放する。

  Arguments:    なし

  Returns:      なし
 *---------------------------------------------------------------------------*/
void
DWC_CleanupInet( void )
{
    // [arakit] main 051013
    DWC_Printf( DWC_REPORTFLAG_DEBUG, "!!DWC_CleanupInet() was called!!\n" );
    
    if ( stpInetCntl == NULL )
    {
        DWC_Printf( DWC_REPORTFLAG_CONNECTINET, "ConnectInet library is not initialized.\n" );
        // まだ、初期化されてないので、何もしない。
        return;
    }

	if(stpInetCntl->state == DWC_CONNECTINET_STATE_IDLE){ // まだ何もメモリ確保されていない
	  stpInetCntl = NULL; // コントロールだけ使わなくする
	  return; 
	}

    // 開放処理。
    while ( !DWC_AC_Destroy() )
    {
        OS_Sleep( 10 );
    }

    stpInetCntl = NULL;
}

/*---------------------------------------------------------------------------*
  Name:         DWC_CleanupInetAsync

  Description:  インターネットに接続を終了し、メモリを開放する。(非同期版)
                TRUEが戻るまで、毎ゲームフレーム呼んでください。

  Arguments:    なし

  Returns:      TRUE  : 開放が終了した。
                FALSE : 開放中。
 *---------------------------------------------------------------------------*/
BOOL
DWC_CleanupInetAsync( void )
{
    // [arakit] main 051013
    DWC_Printf( DWC_REPORTFLAG_DEBUG, "!!DWC_CleanupInetAsync() was called!!\n" );
    
    if ( stpInetCntl == NULL )
    {
        DWC_Printf( DWC_REPORTFLAG_CONNECTINET, "ConnectInet library is not initialized.\n" );
        // まだ、初期化されてないので、何もしない。
        return TRUE;
    }


    // 安定ステートのときだけ。
    if ( stpInetCntl->state == DWC_CONNECTINET_STATE_FATAL_ERROR )
    {
        // FATALエラーのときは、何もしない。
        // [arakit] main 051013
        DWC_Printf( DWC_REPORTFLAG_DEBUG, "CleanupInet Status: FATAL ERROR.\n" );
        return FALSE;
    }
    else if ( stpInetCntl->state == DWC_CONNECTINET_STATE_IDLE ) // IDLEのときは、クリアして終わり。
    {
        DWC_Printf( DWC_REPORTFLAG_DEBUG, "CleanupInet Status: %d\n", stpInetCntl->state );

        stpInetCntl = NULL;

        return TRUE;
    }
    else
    {
        stpInetCntl->state = DWC_CONNECTINET_STATE_DISCONNECTING;
    }

#if 0
#if SDK_WIFI_VERSION_DATE >= 20050929
    // TCPの全Close待つ。
    if ( SOCL_CalmDown() != SOCL_ESUCCESS )
    {
        // close中。
        DWC_Printf( DWC_REPORTFLAG_DEBUG, "CleanupInet Status: %d %d\n", stpInetCntl->state, SOCL_CalmDown() );

        return FALSE;
    }
#endif
#endif

    // ACの開放処理。
    if ( DWC_AC_Destroy() )
    {
        // 接続処理終了.
        stpInetCntl = NULL;

        return TRUE;
    }

    return FALSE;
}

BOOL
DWCi_CheckDisconnected( void )
{
    if ( stpInetCntl != NULL && stpInetCntl->state == DWC_CONNECTINET_STATE_DISCONNECTED )
    {
        return TRUE;
    }

    return FALSE;
}

WMLinkLevel
DWC_GetLinkLevel( void )
{
#if SDK_WIFI_VERSION_DATE >= 20050914
    return WCM_GetLinkLevel();
#else
    return WM_GetLinkLevel();
#endif
}

int
DWC_GetUdpPacketDropNum( void )
{
//#ifndef SDK_FINALROM
#if SDK_WIFI_VERSION_DATE >= 20050914
    return (int)SOCLi_CheckCount[ SOCL_CHECKCOUNT_UDPRCVCB_2 ];
#else
    return 0;
#endif
//#else
//    return 0;
//#endif
}

// [nakata] ここよりAP情報取得関連の拡張部分

// [nakata] 外部関数の定義
extern u8* WCM_GetApMacAddress( void );
extern u8* WCM_GetApEssid     ( u16* length );

/*---------------------------------------------------------------------------*
  Name:         DWC_GetApInfo

  Description:  現在接続中のAPに関する情報を取得する。

  Arguments:    apinfo  : 取得した情報を書き込むメモリ領域へのポインタ

  Returns:      TRUE  : 取得に成功した
	            FALSE : 取得に失敗した
 *---------------------------------------------------------------------------*/
BOOL DWC_GetApInfo(DWCApInfo *apinfo)
{
	OSIntrMode	preirq;
	unsigned char	*wcmapbssid;
	unsigned char	*wcmapessid;
	unsigned short	essidlen;
	u8 apSpotInfo[10];
	
	// [nakata] 格納先エリアを0クリア
	MI_CpuClear8(apinfo, sizeof(DWCApInfo));
	apinfo->area = DWC_APINFO_AREA_UNKNOWN;

	// [nakata] インターネット接続が確立していない場合にはFALSEを返す
	if(DWC_GetInetStatus() != DWC_CONNECTINET_STATE_CONNECTED)
		return FALSE;
	
	// [nakata] APタイプを取得
	apinfo->aptype = (DWCApInfoType)DWC_AC_GetApType();
	if(apinfo->aptype == DWC_AC_AP_TYPE_FALSE || apinfo->aptype > DWC_AC_AP_TYPE_UNKNOWN)
	{
		apinfo->aptype = (DWCApInfoType)DWC_AC_AP_TYPE_UNKNOWN;
		return FALSE;
	}
	
	// [nakata] BSSIDをapinfo内に取得
	preirq = OS_DisableInterrupts();
	wcmapbssid = WCM_GetApMacAddress();
	DC_InvalidateRange(wcmapbssid, WM_SIZE_BSSID);
	if(wcmapbssid == NULL) {
		preirq = OS_RestoreInterrupts(preirq);
		return FALSE;
	}
	MI_CpuCopy8(wcmapbssid, apinfo->bssid, 6);
	
	// [nakata] 必要に応じてESSIDをapinfo内に取得
	if(apinfo->aptype >= 0 && apinfo->aptype < 3) {
		wcmapessid = WCM_GetApEssid(&essidlen);
		DC_InvalidateRange(wcmapessid, WM_SIZE_SSID);
		if(wcmapessid == NULL) {
			preirq = OS_RestoreInterrupts(preirq);
			return FALSE;		
		}
		MI_CpuCopy8(wcmapessid, apinfo->essid, essidlen);
	}
	preirq = OS_RestoreInterrupts(preirq);
	
	// [nakata] APの場所情報をapinfo内に取得
	if( DWC_AC_GetApSpotInfo( apSpotInfo ) )
	{
		apinfo->area = (apSpotInfo[0] >= '0' && apSpotInfo[0] <= '9') ? (DWCApInfoArea)(apSpotInfo[0] - '0') : DWC_APINFO_AREA_UNKNOWN;
		MI_CpuCopy8(&apSpotInfo[1], apinfo->spotinfo, DWC_SIZE_SPOTINFO);
	}
	else
	{
		apinfo->area = DWC_APINFO_AREA_UNKNOWN;
	}
	
	return TRUE;
}
// [nakata] ここまでAP情報取得関連の拡張部分

// [nakata] ここよりブラウザ対応の拡張部分

/*---------------------------------------------------------------------------*
  Name:         DWC_EnableHotspot

  Description:  Nintendo Wi-FiコネクションがサポートしないHotspotの使用を許可する

  Arguments:    なし

  Returns:      なし
 *---------------------------------------------------------------------------*/
void DWC_EnableHotspot(void)
{
	DWC_Netcheck_SetAllow302(TRUE);
}

// [nakata] ここまでブラウザ対応の拡張部分

// [uemura] 2006/05/24追加
/*---------------------------------------------------------------------------*
  Name:         DWC_CheckWiFiStation

  Description:  Wi-Fiステーションかどうかを確認する

  Arguments:    ssid  : 確認するAPのSSID
                len   : SSIDの長さ

  Returns:      TRUE  : Wi-Fiステーション
                FALSE : Wi-Fiステーション以外
 *---------------------------------------------------------------------------*/
BOOL
DWC_CheckWiFiStation( const void *ssid, u16 len )
{
	return DWC_AC_CheckWiFiStation( ssid, len );
}

// [uemura] ここまで

// [nakata] 06/06/27 dwc_main.cから移動(重複する処理を統合するため)
/*---------------------------------------------------------------------------*
  DCF通信処理更新関数
  引数　：なし
  戻り値：TRUEのとき、切断された
  用途　：毎ゲームフレーム呼び出し、DCF通信の監視を行う。
 *---------------------------------------------------------------------------*/
BOOL DWC_UpdateConnection( void )
{
    DWC_ProcessInet();

    if ( DWCi_CheckDisconnected() )
    {
        // 切断された！
        u8 aptype = DWC_AC_GetApType();

        // 自動接続のエラーコードを設定
        if( aptype == DWC_AC_AP_TYPE_FALSE || aptype > DWC_AC_AP_TYPE_UNKNOWN )
        {
            aptype = DWC_AC_AP_TYPE_UNKNOWN;
        }
        DWCi_SetError( DWC_ERROR_DISCONNECTED, -54000 - aptype );

        DWC_Printf( DWC_REPORTFLAG_DEBUG, "Detected DCF shutdown.\n" );

        return TRUE;
    }

    return FALSE;
}

/*---------------------------------------------------------------------------*
  DCF通信処理終了関数
  引数　：なし
  戻り値：なし
  用途　：DCFが切断されたときに呼び出され、必要な処理がある場合にここに記述する。
 *---------------------------------------------------------------------------*/
void DWC_ForceShutdown( void )
{
    // 強制的に呼んでしまってもかまわない関数をここで呼び出す。
}
// [nakata] ここまで

// [nakata] 06/11/06 インターネット接続確認処理をラップする関数を追加

/*---------------------------------------------------------------------------*
  HTTPを使用したインターネット接続確認関数(非同期)
  引数　：タイムアウト時間(ミリ秒、0の場合は3000扱い)
  戻り値：TRUE  - 成功
          FALSE - 失敗
  用途　：HTTP通信を使用してインターネット接続を確認する
 *---------------------------------------------------------------------------*/
BOOL DWC_TestInetAsync(int timeout)
{
    if ( stpInetCntl == NULL ) // DWC_InitInet が呼ばれてない
    {
        return FALSE;
    }
    else if ( stpInetCntl->state != DWC_CONNECTINET_STATE_CONNECTED ) // DWC_ConnectInetAsync が成功していない
    {
        return FALSE;
    }

	return DWC_SimpleNetcheckAsync(timeout);
}

/*---------------------------------------------------------------------------*
  HTTPを使用したインターネット接続確認プロセス関数
  引数　：なし
  戻り値：なし
  用途　：DCFが切断されたときに呼び出され、必要な処理がある場合にここに記述する。
 *---------------------------------------------------------------------------*/
DWCTestInetResult DWC_TestInetProcess(void)
{
	DWCSimpleNetcheckResult result;
	u8 aptype;
	result = DWC_SimpleNetcheckProcess();
	
	switch(result) {
	case DWC_SIMPLENETCHECK_PROCESSING:
		return DWC_TESTINET_OPERATING;
	case DWC_SIMPLENETCHECK_SUCCESS:
		return DWC_TESTINET_CONNECTED;
	case DWC_SIMPLENETCHECK_FAIL:
	case DWC_SIMPLENETCHECK_SYSFAIL:
		aptype = DWC_AC_GetApType();
		if( aptype == DWC_AC_AP_TYPE_FALSE || aptype > DWC_AC_AP_TYPE_UNKNOWN )
			aptype = DWC_AC_AP_TYPE_UNKNOWN;
        DWCi_SetError(DWC_ERROR_DISCONNECTED, -54100-aptype);
			
		return DWC_TESTINET_DISCONNECTED;
	case DWC_SIMPLENETCHECK_NOTINITIALIZED:
		return DWC_TESTINET_NOT_INITIALIZED;
	}
	
	// [nakata] 予期しない動作
	return DWC_TESTINET_DISCONNECTED;
}
// [nakata] ここまで
