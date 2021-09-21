/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - snd
  File:     main.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.41 $
 *---------------------------------------------------------------------------*/
#include <nnsys/snd/main.h>

#include <nitro/spi.h>
#include <nnsys/misc.h>
#include <nnsys/snd/capture.h>
#include <nnsys/snd/player.h>
#include <nnsys/snd/sndarc_stream.h>
#include <nnsys/snd/resource_mgr.h>
#include <nnsys/snd/config.h>

/******************************************************************************
	static variables
 ******************************************************************************/

static PMSleepCallbackInfo sPreSleepCallback;
static PMSleepCallbackInfo sPostSleepCallback;

static SNDDriverInfo sDriverInfo[2] ATTRIBUTE_ALIGN( 32 );
static u32 sDriverInfoCommandTag;
static s8 sCurDriverInfo;
static BOOL sDriverInfoFirstFlag;

/******************************************************************************
	static function declaration
 ******************************************************************************/
static void BeginSleep( void* );
static void EndSleep( void* );

static const SNDDriverInfo* GetCurDriverInfo( void );

/******************************************************************************
	inline functions
 ******************************************************************************/
static NNS_SND_INLINE const SNDDriverInfo* GetCurDriverInfo( void )
{
    if ( sCurDriverInfo < 0 ) return NULL;
    return &sDriverInfo[ sCurDriverInfo ];
}

/******************************************************************************
	public functions
 ******************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         NNS_SndInit

  Description:  サウンドライブラリの初期化

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndInit( void )
{
    {
        // ２重初期化チェック
        static BOOL initialized = FALSE;
        if ( initialized ) return;
        initialized = TRUE;
    }
    
    // サウンドドライバの初期化
    SND_Init();
    
    // スリープコールバックの登録
    PM_SetSleepCallbackInfo( &sPreSleepCallback, BeginSleep, NULL );
    PM_SetSleepCallbackInfo( &sPostSleepCallback, EndSleep, NULL );
    
    PM_PrependPreSleepCallback( &sPreSleepCallback );
    PM_AppendPostSleepCallback( &sPostSleepCallback );
    
    // 各ライブラリの初期化
    NNSi_SndInitResourceMgr();
    NNSi_SndCaptureInit();
    NNSi_SndPlayerInit();
    
    // ドライバ情報の初期化
    sCurDriverInfo = -1;
    sDriverInfoFirstFlag = TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndMain

  Description:  サウンドライブラリのフレームワーク

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndMain( void )
{
    // ARM7応答受信
    while ( SND_RecvCommandReply( SND_COMMAND_NOBLOCK ) != NULL ) {}
    
    // 各ライブラリのフレームワーク
    NNSi_SndPlayerMain();
    NNSi_SndCaptureMain();
#ifndef SDK_SMALL_BUILD    
    NNSi_SndArcStrmMain();
#endif /* SDK_SMALL_BUILD */
    
    // ARM7コマンド発行
    (void)SND_FlushCommand( SND_COMMAND_NOBLOCK );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndSetMasterVolume

  Description:  マスターボリュームの設定

  Arguments:    volume - マスターボリューム

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndSetMasterVolume( int volume )
{
    NNS_MINMAX_ASSERT( volume, 0, SND_MASTER_VOLUME_MAX );
    
    SND_SetMasterVolume( volume );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndSetMonoFlag

  Description:  モノラルフラグの変更

  Arguments:    flag - モノラルを有効にするかどうか

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndSetMonoFlag( BOOL flag )
{
    if ( flag ) SND_SetMasterPan( SND_CHANNEL_PAN_CENTER );
    else SND_ResetMasterPan();
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndStopSoundAll

  Description:  全サウンドの即時停止

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndStopSoundAll( void )
{
    u32 commandTag;
    
    NNS_SndPlayerStopSeqAll( 0 );
#ifndef SDK_SMALL_BUILD
    NNS_SndArcStrmStopAll( 0 );
#endif /* SDK_SMALL_BUILD */    
    NNSi_SndCaptureStop();
    
    SNDi_SetSurroundDecay( 0 );    
    SND_StopTimer( 0xffff, 0xffff, 0xffff, 0 );
    
    // ARM7での処理待ち
    commandTag = SND_GetCurrentCommandTag();
    (void)SND_FlushCommand( SND_COMMAND_BLOCK );
    SND_WaitForCommandProc( commandTag );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndStopChannelAll

  Description:  全チャンネルの停止

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndStopChannelAll( void )
{
    SND_StopUnlockedChannel( 0xffff, 0 );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndUpdateDriverInfo

  Description:  ドライバ情報の更新

  Arguments:    None.

  Returns:      更新に成功したかどうか
 *---------------------------------------------------------------------------*/
BOOL NNS_SndUpdateDriverInfo( void )
{
    if ( ! sDriverInfoFirstFlag )
    {
        // ARM7応答受信
        while ( SND_RecvCommandReply( SND_COMMAND_NOBLOCK ) != NULL ) {}
        
        if ( ! SND_IsFinishedCommandTag( sDriverInfoCommandTag ) ) {
            // 更新未完了
            return FALSE;
        }
        
        // 更新完了
        if ( sCurDriverInfo < 0 ) sCurDriverInfo = 1;
        
        SND_ReadDriverInfo( &sDriverInfo[ sCurDriverInfo ] );
        sDriverInfoCommandTag = SND_GetCurrentCommandTag();
        
        if ( sCurDriverInfo == 0 ) sCurDriverInfo = 1;
        else sCurDriverInfo = 0;
        
        DC_InvalidateRange( &sDriverInfo[ sCurDriverInfo ], sizeof( SNDDriverInfo ) );
        
        // ARM7コマンド発行
        (void)SND_FlushCommand( SND_COMMAND_NOBLOCK );
        
        return TRUE;
    }
    else
    {
        // 始めての更新
        SND_ReadDriverInfo( &sDriverInfo[0] );
        sDriverInfoCommandTag = SND_GetCurrentCommandTag();
        sDriverInfoFirstFlag = FALSE;
        return FALSE;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndReadDriverChannelInfo

  Description:  チャンネル情報の取得

  Arguments:    chNo - チャンネル番号
                info - 取得した情報を格納するチャンネル情報構造体

  Returns:      取得に成功したかどうか
 *---------------------------------------------------------------------------*/
BOOL NNS_SndReadDriverChannelInfo( int chNo, SNDChannelInfo* info )
{
    const SNDDriverInfo* driverInfo;
    
    NNS_NULL_ASSERT( info );
    
    driverInfo = GetCurDriverInfo();
    if ( driverInfo == NULL ) return FALSE;
    
    return SND_ReadChannelInfo( driverInfo, chNo, info );
}

/******************************************************************************
	private functions
 ******************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         NNSi_SndReadDriverPlayerInfo

  Description:  プレイヤー情報の取得

  Arguments:    playerNo - プレイヤー番号（物理番号）
                info - 取得した情報を格納するプレイヤー情報構造体

  Returns:      取得に成功したかどうか
 *---------------------------------------------------------------------------*/
BOOL NNSi_SndReadDriverPlayerInfo( int playerNo, SNDPlayerInfo* info )
{
    const SNDDriverInfo* driverInfo;
    
    NNS_NULL_ASSERT( info );
    
    driverInfo = GetCurDriverInfo();
    if ( driverInfo == NULL ) return FALSE;

    return SND_ReadPlayerInfo( driverInfo, playerNo, info );
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_SndReadDriverTrackInfo

  Description:  トラック情報の取得

  Arguments:    playerNo - プレイヤー番号（物理番号）
                trackNo - トラック番号
                info - 取得した情報を格納するトラック情報構造体

  Returns:      取得に成功したかどうか
 *---------------------------------------------------------------------------*/
BOOL NNSi_SndReadDriverTrackInfo( int playerNo, int trackNo, SNDTrackInfo* info )
{
    const SNDDriverInfo* driverInfo;
    
    NNS_NULL_ASSERT( info );
    
    driverInfo = GetCurDriverInfo();
    if ( driverInfo == NULL ) return FALSE;
    
    return SND_ReadTrackInfo( driverInfo, playerNo, trackNo, info );
}

/******************************************************************************
	static functions
 ******************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         BeginSleep

  Description:  スリープモード前処理

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void BeginSleep( void* )
{
    u32 commandTag;
    
    NNSi_SndCaptureBeginSleep();
    
    // コマンドバッファを空にするため、ダミーコマンド送出
    SND_StopTimer( 0, 0, 0, 0 );
    
    // ARM7での処理待ち
    commandTag = SND_GetCurrentCommandTag();
    (void)SND_FlushCommand( SND_COMMAND_BLOCK );
    SND_WaitForCommandProc( commandTag );
}

/*---------------------------------------------------------------------------*
  Name:         EndSleep

  Description:  スリープモード後処理

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void EndSleep( void* )
{
    NNSi_SndCaptureEndSleep();
}

/*====== End of main.c ======*/

