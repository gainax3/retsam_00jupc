/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - snd
  File:     stream.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.9 $
 *---------------------------------------------------------------------------*/
#include <nnsys/snd/stream.h>

#include <nitro/snd.h>
#include <nnsys/misc.h>
#include <nnsys/snd/resource_mgr.h>

/******************************************************************************
	structure definition
 ******************************************************************************/

typedef struct NNSSndStrmChannel
{
    void* buffer;
    int volume;
} NNSSndStrmChannel;

/******************************************************************************
	static variable
 ******************************************************************************/

static NNSSndStrmChannel sStrmChannel[ SND_CHANNEL_NUM ];
static NNSFndList sStrmList;

/******************************************************************************
	static function declaration
 ******************************************************************************/

static void ShutdownStrm( NNSSndStrm* stream );
static void ForceStopStrm( NNSSndStrm* stream );
static void AlarmCallback( void* arg );
static void StrmCallback( NNSSndStrm* stream, NNSSndStrmCallbackStatus status );
static void BeginSleep( void* arg );
static void EndSleep( void* arg );

/******************************************************************************
	public functions
 ******************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         NNS_SndStrmInit

  Description:  ストリームの初期化

  Arguments:    stream - ストリームオブジェクト

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndStrmInit( NNSSndStrm* stream )
{
    NNS_NULL_ASSERT( stream );
    
    {
        static BOOL bInitialized = FALSE;

        if ( ! bInitialized ) {
            NNS_FND_INIT_LIST( &sStrmList, NNSSndStrm, link );
            
            bInitialized = TRUE;
        }
    }
    
    PM_SetSleepCallbackInfo( &stream->preSleepInfo, BeginSleep, stream );
    PM_SetSleepCallbackInfo( &stream->postSleepInfo, EndSleep, stream );
    
    stream->chBitMask = 0;
    stream->numChannels = 0;
    stream->activeFlag = FALSE;
    stream->startFlag = FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndStrmAllocChannel

  Description:  ストリーム用チャンネルの確保

  Arguments:    stream - ストリームオブジェクト
                numChannels - チャンネル数
                chNoList    - チャンネル番号リスト

  Returns:      確保に成功したかどうか
 *---------------------------------------------------------------------------*/
BOOL NNS_SndStrmAllocChannel( NNSSndStrm* stream, int numChannels, const u8 chNoList[] )
{
    u32 chBitMask;
    int i;
    
    NNS_NULL_ASSERT( stream );
    NNS_MINMAX_ASSERT( numChannels, 1, SND_CHANNEL_NUM );
    
    chBitMask = 0;
    for( i = 0; i < numChannels ; i++ )
    {
        NNS_MINMAX_ASSERT( chNoList[ i ], SND_CHANNEL_MIN, SND_CHANNEL_MAX );
        
        stream->channelNo[ i ] = chNoList[ i ];
        chBitMask |= ( 1 << chNoList[ i ] );
    }
    
    if ( ! NNS_SndLockChannel( chBitMask ) ) {
        return FALSE;
    } 
    
    stream->numChannels = numChannels;
    stream->chBitMask = chBitMask;

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndStrmFreeChannel

  Description:  ストリーム用チャンネルの解放

  Arguments:    stream - ストリームオブジェクト

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndStrmFreeChannel( NNSSndStrm* stream )
{
    NNS_NULL_ASSERT( stream );

    if ( stream->chBitMask == 0 ) return;
    
    NNS_SndUnlockChannel( stream->chBitMask );
    
    stream->chBitMask = 0;
    stream->numChannels = 0;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndStrmSetup

  Description:  ストリームのセットアップ

  Arguments:    stream - ストリームオブジェクト
                format - データフォーマット
                buffer - ストリームバッファの先頭アドレス
                bufSize - ストリームバッファのサイズ
                timer  - チャンネルタイマー値
                interval - コールバック間隔
                callback - コールバック関数
                arg      - コールバック引数

  Returns:      成功したかどうか
 *---------------------------------------------------------------------------*/
BOOL NNS_SndStrmSetup(
    NNSSndStrm* stream,
    NNSSndStrmFormat format,
    void* buffer,
    u32 bufSize,
    int timer,
    int interval,
    NNSSndStrmCallback callback,
    void* arg
)
{
    NNSSndStrmChannel* chp;
    unsigned int samples;
    unsigned int alarmTimer;
    int chNo;
    int index;
    
    NNS_NULL_ASSERT( stream );
    NNS_ASSERTMSG( ( (u32)buffer & 0x1f ) == 0, "buffer address must be aligned to 32 bytes boundary.");
    NNS_ASSERTMSG( ( bufSize % ( interval * stream->numChannels * 32 ) ) == 0,
                   "buffer size must be a multiple of interval * numChannels * 32.");
    NNS_MINMAX_ASSERT( timer, NNS_SND_STRM_TIMER_MIN, NNS_SND_STRM_TIMER_MAX );
    NNS_NULL_ASSERT( callback );
    
    /* 強制停止 */
    if ( stream->activeFlag ) {
        NNS_SndStrmStop( stream );
    }
    
    /* チャンネル毎のバッファ長の計算 */
    bufSize /= 32 * interval * stream->numChannels;
    stream->chBufLen = bufSize * interval * 32;
    
    /* サンプル数の計算 */
    samples = stream->chBufLen;
    if ( format == NNS_SND_STRM_FORMAT_PCM16 ) samples >>= 1;
    
    /* タイマー値の計算 */
    NNS_ASSERTMSG( ( samples % interval ) == 0, "interval must be a multiple of sample count." );     
    alarmTimer = timer * samples / interval;
    
    /* リソースの確保 */
    stream->alarmNo = NNS_SndAllocAlarm();
    if ( stream->alarmNo < 0 ) return FALSE;
    
    /* チャンネルのセットアップ */
    for( index = 0; index < stream->numChannels ; index++ )
    {
        chNo = stream->channelNo[ index ];
        
        chp = & sStrmChannel[ chNo ];
        
        chp->buffer = (u8*)buffer + stream->chBufLen * index;
        NNS_ALIGN4_ASSERT( chp->buffer );
        
        chp->volume = 0;
        
        SND_SetupChannelPcm(
            chNo,
            (SNDWaveFormat)format,
            chp->buffer,
            SND_CHANNEL_LOOP_REPEAT,
            0,
            (int)( stream->chBufLen >> 2 ),
            127,
            SND_CHANNEL_DATASHIFT_NONE,
            timer << 5,
            64
        );
    }
    
    /* アラームの設定 */
    SND_SetupAlarm(
        stream->alarmNo,
        alarmTimer,
        alarmTimer,
        AlarmCallback,
        stream
    );
    
    /* リストへの登録 */
    NNS_FndAppendListObject( &sStrmList, stream );
    
    /* パラメータの初期化 */
    stream->format = format;
    stream->interval = interval;
    stream->callback = callback;
    stream->callbackArg = arg;
    stream->curBuffer = 0;
    
    stream->volume = 0;
    
    stream->activeFlag = TRUE;
    
    /* セットアップコールバック呼び出し */
    {
        OSIntrMode old = OS_DisableInterrupts();
        
        stream->interval = 1;
        StrmCallback( stream, NNS_SND_STRM_CALLBACK_SETUP );
        stream->interval = interval;
        
        (void)OS_RestoreInterrupts( old );
    }
    
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndStrmStart

  Description:  ストリームの再生開始

  Arguments:    stream - ストリームオブジェクト

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndStrmStart( NNSSndStrm* stream )
{
    NNS_NULL_ASSERT( stream );    
    
    SND_StartTimer(
        stream->chBitMask,
        0,
        (u32)( 1 << stream->alarmNo ),
        0
    );
    
    if ( ! stream->startFlag )
    {
        PM_PrependPreSleepCallback( & stream->preSleepInfo );
        PM_AppendPostSleepCallback( & stream->postSleepInfo );
        
        stream->startFlag = TRUE;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndStrmStop

  Description:  ストリームの再生停止

  Arguments:    stream - ストリームオブジェクト

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndStrmStop( NNSSndStrm* stream )
{
    NNS_NULL_ASSERT( stream );
    
    if ( ! stream->activeFlag ) return;
    
    ForceStopStrm( stream );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndStrmSetVolume

  Description:  ストリームボリュームの変更

  Arguments:    stream - ストリームオブジェクト
                volume - ボリューム

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndStrmSetVolume( NNSSndStrm* stream, int volume )
{
    u16 chVolume;
    int vol;
    int chNo;
    int i;
    
    NNS_NULL_ASSERT( stream );
    
    stream->volume = volume;
    
    for( i = 0 ; i < stream->numChannels ; i++ )
    {
        chNo = stream->channelNo[ i ];
        vol = stream->volume + sStrmChannel[ chNo ].volume;
        chVolume = SND_CalcChannelVolume( vol );
        
        SND_SetChannelVolume(
            (u32)( 1 << chNo ),
            chVolume & 0xff,
            (SNDChannelDataShift)( chVolume >> 8 )
        );
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndStrmSetChannelVolume

  Description:  ストリームチャンネルボリュームの変更

  Arguments:    stream - ストリームオブジェクト
                chNo   - チャンネル番号
                volume - ボリューム

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndStrmSetChannelVolume( NNSSndStrm* stream, int chNo, int volume )
{
    u16 chVolume;
    int vol;
    
    NNS_NULL_ASSERT( stream );
    
    if ( chNo > stream->numChannels - 1 ) return;
    
    chNo = stream->channelNo[ chNo ];
    sStrmChannel[ chNo ].volume = volume;
    
    vol = stream->volume + sStrmChannel[ chNo ].volume;
    chVolume = SND_CalcChannelVolume( vol );
    
    SND_SetChannelVolume(
        (u32)( 1 << chNo ),
        chVolume & 0xff,
        (SNDChannelDataShift)( chVolume >> 8 )
    );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndStrmSetChannelPan

  Description:  ストリームチャンネルパンの変更

  Arguments:    stream - ストリームオブジェクト
                chNo   - チャンネル番号
                pan    - パン.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndStrmSetChannelPan( NNSSndStrm* stream, int chNo, int pan )
{
    NNS_NULL_ASSERT( stream );
    NNS_MINMAX_ASSERT( pan, NNS_SND_STRM_PAN_MIN, NNS_SND_STRM_PAN_MAX );
    
    if ( chNo > stream->numChannels - 1 ) return;
    
    SND_SetChannelPan( (u32)( 1 << stream->channelNo[ chNo ] ) , pan );
}

/******************************************************************************
	static functions
 ******************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         ForceStopStrm

  Description:  ストリームの強制停止

  Arguments:    stream - ストリームオブジェクト.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void ForceStopStrm( NNSSndStrm* stream )
{
    u32 commandTag;
    
    NNS_NULL_ASSERT( stream );
    
    if ( stream->startFlag )
    {
        SND_StopTimer(
            stream->chBitMask,
            0,
            (u32)( 1 << stream->alarmNo ),
            0
        );
        
        PM_DeletePreSleepCallback( & stream->preSleepInfo );
        PM_DeletePostSleepCallback( & stream->postSleepInfo );
        
        stream->startFlag = FALSE;
        
        commandTag = SND_GetCurrentCommandTag();
        (void)SND_FlushCommand( SND_COMMAND_BLOCK );
        SND_WaitForCommandProc( commandTag );
    }
    
    ShutdownStrm( stream );
}

/*---------------------------------------------------------------------------*
  Name:         ShutdownStrm

  Description:  ストリームを閉じる

  Arguments:    stream - ストリームオブジェクト

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void ShutdownStrm( NNSSndStrm* stream )
{
    NNS_NULL_ASSERT( stream );
    
    NNS_SndFreeAlarm( stream->alarmNo );    
    
    NNS_FndRemoveListObject( & sStrmList, stream );
    
    stream->activeFlag = FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         AlarmCallback

  Description:  アラームコールバック

  Arguments:    arg - ストリームオブジェクト

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void AlarmCallback( void* arg )
{
    StrmCallback( (NNSSndStrm*)arg, NNS_SND_STRM_CALLBACK_INTERVAL );
}

/*---------------------------------------------------------------------------*
  Name:         StrmCallback

  Description:  ストリームコールバック

  Arguments:    stream - ストリームオブジェクト
                status - コールバックステータス

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void StrmCallback( NNSSndStrm* stream, NNSSndStrmCallbackStatus status )
{
    static void* buffer[ NNS_SND_STRM_CHANNEL_MAX ];
    const unsigned long blockSize = stream->chBufLen / stream->interval;
    const unsigned long  offset = blockSize * stream->curBuffer;
    int index;
    int chNo;
    
    for( index = 0; index < stream->numChannels ; index++ )
    {
        chNo = stream->channelNo[ index ];
        
        buffer[ index ] = (u8*)( sStrmChannel[ chNo ].buffer ) + offset;
    }
    
    stream->callback(
        status,
        stream->numChannels,
        buffer,
        blockSize,
        stream->format,
        stream->callbackArg
    );
    
    stream->curBuffer++;
    if ( stream->curBuffer >= stream->interval ) stream->curBuffer = 0;
}

/*---------------------------------------------------------------------------*
  Name:         BeginSleep

  Description:  スリープモード前処理

  Arguments:    arg - ストリームオブジェクト

  Returns:      None.
 *---------------------------------------------------------------------------*/
void BeginSleep( void* arg )
{
    NNSSndStrm* stream = (NNSSndStrm*)arg;
    u32 commandTag;
    
    NNS_NULL_ASSERT( stream );
    
    if ( ! stream->startFlag ) return;
    
    SND_StopTimer(
        stream->chBitMask,
        0,
        (u32)( 1 << stream->alarmNo ),
        0
    );
    
    commandTag = SND_GetCurrentCommandTag();
    (void)SND_FlushCommand( SND_COMMAND_BLOCK );
    SND_WaitForCommandProc( commandTag );
}

/*---------------------------------------------------------------------------*
  Name:         EndSleep

  Description:  スリープモード後処理

  Arguments:    arg - ストリームオブジェクト

  Returns:      None.
 *---------------------------------------------------------------------------*/
void EndSleep( void* arg )
{
    NNSSndStrm* stream = (NNSSndStrm*)arg;
    
    NNS_NULL_ASSERT( stream );
    
    if ( ! stream->startFlag ) return;
    
    while ( stream->curBuffer != 0 ) {
        OSIntrMode old = OS_DisableInterrupts();
        
        StrmCallback( stream, NNS_SND_STRM_CALLBACK_INTERVAL );
        
        (void)OS_RestoreInterrupts( old );        
    }
    
    SND_StartTimer(
        stream->chBitMask,
        0,
        (u32)( 1 << stream->alarmNo ),
        0
    );
}

/*====== End of stream.c ======*/

