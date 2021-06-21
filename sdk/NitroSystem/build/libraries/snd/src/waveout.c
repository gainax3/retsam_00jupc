/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - snd
  File:     waveout.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.9 $
 *---------------------------------------------------------------------------*/
#include <nnsys/snd/waveout.h>

#include <nitro/snd.h>
#include <nnsys/misc.h>
#include <nnsys/snd/resource_mgr.h>

/******************************************************************************
	structure definition
 ******************************************************************************/

typedef struct NNSSndWaveOut
{
    int chNo;
    int sampleRate;
    BOOL activeFlag;
    BOOL startFlag;
    u32 startCommandTag;
} NNSSndWaveOut;

/******************************************************************************
	static variable
 ******************************************************************************/

static NNSSndWaveOut sWaveOut[ SND_CHANNEL_NUM ];

/******************************************************************************
	public functions
 ******************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         NNS_SndWaveOutAllocChannel

  Description:  波形再生用のチャンネルを確保します

  Arguments:    chNo - チャンネル番号

  Returns:      確保に成功したら、波形再生ハンドルを返す
                確保に失敗したら、NNS_SND_WAVEOUT_INVALID_HANDLEを返す
 *---------------------------------------------------------------------------*/
NNSSndWaveOutHandle NNS_SndWaveOutAllocChannel( int chNo )
{
    NNSSndWaveOut* waveout;
    
    NNS_MINMAX_ASSERT( chNo, SND_CHANNEL_MIN, SND_CHANNEL_MAX );
    
    if ( ! NNS_SndLockChannel( (u32)( 1 << chNo ) ) ) return NNS_SND_WAVEOUT_INVALID_HANDLE;
    
    waveout = & sWaveOut[ chNo ];
    waveout->chNo = chNo;
    waveout->activeFlag = FALSE;
    
    return waveout;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndWaveOutFreeChannel

  Description:  波形再生用に確保したチャンネルを解放します

  Arguments:    handle - 波形再生ハンドル

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndWaveOutFreeChannel( NNSSndWaveOutHandle handle )
{
    NNS_NULL_ASSERT( handle );
    
    NNS_SndUnlockChannel( (u32)( 1 << handle->chNo ) );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndWaveOutStart

  Description:  波形再生を開始します

  Arguments:    handle   - 波形再生ハンドル
                format   - 波形データフォーマット
                dataaddr - 波形データ先頭アドレス
                loopFlag - ループフラグ
                loopStartSample - ループ開始サンプル位置
                samples         - サンプル数
                sampleRate - サンプリングレート
                volume     - ボリューム
                speed      - 再生スピード
                pan        - パン

  Returns:      成功したかどうか
 *---------------------------------------------------------------------------*/
BOOL NNS_SndWaveOutStart(
    NNSSndWaveOutHandle handle,
    NNSSndWaveFormat format,
    const void* dataaddr,
    BOOL loopFlag,
    int loopStartSample,
    int samples,
    int sampleRate,
    int volume, 
    int speed,
    int pan
)
{
    int loopStart;
    int loopLen;
    u64 timer;
    
    NNS_NULL_ASSERT( handle );
    NNS_ALIGN4_ASSERT( dataaddr );
    NNS_MINMAX_ASSERT( volume, NNS_SND_WAVEOUT_VOLUME_MIN, NNS_SND_WAVEOUT_VOLUME_MAX );
    NNS_MINMAX_ASSERT( pan, NNS_SND_WAVEOUT_PAN_MIN, NNS_SND_WAVEOUT_PAN_MAX );    
    
    switch( format )
    {
    case NNS_SND_WAVE_FORMAT_PCM8:
        NNS_ASSERTMSG( ( loopStartSample & 0x03 ) == 0 , "loopStartSample must be a multiple of 4" );
        NNS_ASSERTMSG( ( samples & 0x03 ) == 0 , "samples must be a multiple of 4" );
        loopStart = ( loopStartSample >> 2 );
        loopLen = ( samples >> 2 ) - loopStart;
        break;
    case NNS_SND_WAVE_FORMAT_PCM16:
        NNS_ASSERTMSG( ( loopStartSample & 0x01 ) == 0 , "loopStartSample must be a multiple of 2" );
        NNS_ASSERTMSG( ( samples & 0x01 ) == 0 , "samples must be a multiple of 2" );
        loopStart = ( loopStartSample >> 1 );
        loopLen = ( samples >> 1 ) - loopStart;
        break;
    }
    
    timer = (u64)SND_TIMER_CLOCK * 0x8000 / sampleRate / speed;
    if ( timer < SND_CHANNEL_TIMER_MIN ) timer = SND_CHANNEL_TIMER_MIN;
    else if ( timer > SND_CHANNEL_TIMER_MAX ) timer = SND_CHANNEL_TIMER_MAX;
    
    SND_SetupChannelPcm(
        handle->chNo,
        (SNDWaveFormat)format,
        dataaddr,
        loopFlag ? SND_CHANNEL_LOOP_REPEAT : SND_CHANNEL_LOOP_1SHOT,
        loopStart,
        loopLen,
        volume,
        SND_CHANNEL_DATASHIFT_NONE,
        (int)timer,
        pan
    );
    SND_StartTimer(
        (u32)( 1 << handle->chNo ),
        0, 0, 0
    );
    
    handle->activeFlag = TRUE;
    handle->startFlag = FALSE;
    handle->startCommandTag = SND_GetCurrentCommandTag();
    handle->sampleRate = sampleRate;
    
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndWaveOutStop

  Description:  波形再生を停止します

  Arguments:    handle - 波形再生ハンドル

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndWaveOutStop( NNSSndWaveOutHandle handle )
{
    NNS_NULL_ASSERT( handle );

    if ( ! handle->activeFlag ) return;
    
    SND_StopTimer(
        (u32)( 1 << handle->chNo ),
        0, 0, 0
    );
    
    handle->activeFlag = FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndWaveOutSetVolume

  Description:  波形再生の音量を変更します

  Arguments:    handle - 波形再生ハンドル
                volume - ボリューム

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndWaveOutSetVolume( NNSSndWaveOutHandle handle, int volume )
{
    NNS_NULL_ASSERT( handle );
    
    if ( ! handle->activeFlag ) return;

    SND_SetChannelVolume(
        (u32)( 1 << handle->chNo ),
        volume,
        SND_CHANNEL_DATASHIFT_NONE
    );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndWaveOutSetSpeed

  Description:  波形再生の再生スピードを変更します

  Arguments:    handle - 波形再生ハンドル
                speed - 再生スピード

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndWaveOutSetSpeed( NNSSndWaveOutHandle handle, int speed )
{
    u64 timer;
    
    NNS_NULL_ASSERT( handle );
    
    if ( ! handle->activeFlag ) return;

    timer = (u64)SND_TIMER_CLOCK * 0x8000 / handle->sampleRate / speed;
    if ( timer < SND_CHANNEL_TIMER_MIN ) timer = SND_CHANNEL_TIMER_MIN;
    else if ( timer > SND_CHANNEL_TIMER_MAX ) timer = SND_CHANNEL_TIMER_MAX;
    
    SND_SetChannelTimer(
        (u32)( 1 << handle->chNo ),
        (int)timer
    );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndWaveOutSetPan

  Description:  波形再生のパンを変更します

  Arguments:    handle - 波形再生ハンドル
                pan    - パン

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndWaveOutSetPan( NNSSndWaveOutHandle handle, int pan )
{
    NNS_NULL_ASSERT( handle );
    
    if ( ! handle->activeFlag ) return;

    SND_SetChannelPan(
        (u32)( 1 << handle->chNo ),
        pan
    );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndWaveOutIsPlaying

  Description:  波形再生が再生中かどうかを調べます。

  Arguments:    handle - 波形再生ハンドル

  Returns:      再生中ならTRUE、停止中ならFALSE
 *---------------------------------------------------------------------------*/
BOOL NNS_SndWaveOutIsPlaying( NNSSndWaveOutHandle handle )
{
    NNS_NULL_ASSERT( handle );
    
    if ( ! handle->activeFlag ) return FALSE;
    
    if ( ! handle->startFlag ) {
        if ( ! SND_IsFinishedCommandTag( handle->startCommandTag ) ) return TRUE;
        
        handle->startFlag = TRUE;
    }
    
    if ( SND_GetChannelStatus() & ( 1 << handle->chNo ) ) {
        return TRUE;
    }
    
    handle->activeFlag = FALSE;
    
    return FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndWaveOutWaitForChannelStop

  Description:  波形再生が完全に停止するまで待ちます

  Arguments:    handle - 波形再生ハンドル

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndWaveOutWaitForChannelStop( NNSSndWaveOutHandle handle )
{
    u32 commandTag;
    
    NNS_NULL_ASSERT( handle );
    
    if ( ! handle->activeFlag ) {
        if ( ( SND_GetChannelStatus() & ( 1 << handle->chNo ) ) == 0 ) return;
        
        commandTag = SND_GetCurrentCommandTag();
        (void)SND_FlushCommand( SND_COMMAND_BLOCK );
        SND_WaitForCommandProc( commandTag );
        
        return;
    }
    else {
        if ( ! handle->startFlag ) {
            SND_WaitForCommandProc( handle->startCommandTag );
            handle->startFlag = TRUE;
        }
        while( 1 ) {
            if ( ( SND_GetChannelStatus() & ( 1 << handle->chNo ) ) == 0 ) return;        
        }
    }
}

/*====== End of waveout.c ======*/

