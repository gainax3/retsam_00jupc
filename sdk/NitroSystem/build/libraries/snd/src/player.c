/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - snd
  File:     player.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.84 $
 *---------------------------------------------------------------------------*/
#include <nnsys/snd/player.h>

#include <nitro/os.h>
#include <nitro/snd.h>
#include <nnsys/misc.h>
#include <nnsys/snd/main.h>

/******************************************************************************
    macro definition
 ******************************************************************************/

#define FADER_SHIFT 8

/******************************************************************************
    structure definition
 ******************************************************************************/

typedef struct NNSSndPlayerHeap
{
    NNSFndLink link;
    NNSSndHeapHandle handle;
    NNSSndSeqPlayer* player;
    int playerNo;
} NNSSndPlayerHeap;

/******************************************************************************
    static variables
 ******************************************************************************/

static NNSSndSeqPlayer sSeqPlayer[ SND_PLAYER_NUM ];
static NNSSndPlayer sPlayer[ NNS_SND_PLAYER_NUM ];
static NNSFndList sPrioList;
static NNSFndList sFreeList;

/******************************************************************************
    static function declarations
 ******************************************************************************/

static void InitPlayer( NNSSndSeqPlayer* seqPlayer );
static void ShutdownPlayer( NNSSndSeqPlayer* seqPlayer );
static void ForceStopSeq( NNSSndSeqPlayer* seqPlayer );
static NNSSndSeqPlayer* AllocSeqPlayer( int prio );
static void InsertPlayerList( NNSSndPlayer* player, NNSSndSeqPlayer* seqPlayer );
static void InsertPrioList( NNSSndSeqPlayer* seqPlayer );
static void SetPlayerPriority( NNSSndSeqPlayer* seqPlayer, int priority );
static void PlayerHeapDisposeCallback( void* mem, u32 size, u32 data1, u32 data2 );

/******************************************************************************
    public functions
 ******************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerSetPlayerVolume

  Description:  プレイヤー毎のボリューム設定

  Arguments:    playerNo - プレイヤー番号
                volume   - ボリューム

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerSetPlayerVolume( int playerNo, int volume )
{
    NNS_MINMAX_ASSERT( playerNo, 0, NNS_SND_PLAYER_NO_MAX );
    NNS_MINMAX_ASSERT( volume, 0, SND_CALC_DECIBEL_SCALE_MAX );
    
    sPlayer[ playerNo ].volume = (u8)volume;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerSetPlayableSeqCount

  Description:  シーケンス最大同時再生数の設定

  Arguments:    playerNo - プレイヤー番号
                seqCount - シーケンス最大同時再生数

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerSetPlayableSeqCount( int playerNo, int seqCount )
{
    NNS_MINMAX_ASSERT( playerNo, 0, NNS_SND_PLAYER_NO_MAX );
    NNS_MINMAX_ASSERT( seqCount, 0, SND_PLAYER_NUM );
    
    sPlayer[ playerNo ].playableSeqCount = (u16)seqCount;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerSetAllocatableChannel

  Description:  確保可能なチャンネルの設定

  Arguments:    playerNo - プレイヤー番号
                chBitFlag - 確保可能なチャンネルのビットフラグ
                            ０を指定すると、全て確保可能.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerSetAllocatableChannel( int playerNo, u32 chBitFlag )
{
    NNS_MINMAX_ASSERT( playerNo, 0, NNS_SND_PLAYER_NO_MAX );
    NNS_MINMAX_ASSERT( chBitFlag, 0, 0xffff );
    
    sPlayer[ playerNo ].allocChBitFlag = chBitFlag;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerCreateHeap

  Description:  プレイヤーヒープを作成

  Arguments:    playerNo - プレイヤー番号
                heap - サウンドヒープ
                size - プレイヤーヒープのサイズ

  Returns:      プレイヤーヒープの作成に成功したかどうか
 *---------------------------------------------------------------------------*/
BOOL NNS_SndPlayerCreateHeap( int playerNo, NNSSndHeapHandle heap, u32 size )
{
    NNSSndHeapHandle playerHeapHandle;
    NNSSndPlayerHeap* playerHeap;    
    void* buffer;
    
    NNS_MINMAX_ASSERT( playerNo, 0, NNS_SND_PLAYER_NO_MAX );
    
    /* プレイヤーヒープ用バッファの確保 */
    buffer = NNS_SndHeapAlloc( heap, sizeof( NNSSndPlayerHeap ) + size, PlayerHeapDisposeCallback, 0, 0 );
    if ( buffer == NULL ) {
        return FALSE;
    }
    
    /* プレイヤーヒープ構造体初期化 */
    playerHeap = (NNSSndPlayerHeap*)buffer;
    
    playerHeap->player = NULL;
    playerHeap->playerNo = playerNo;
    playerHeap->handle = NNS_SND_HEAP_INVALID_HANDLE;
    
    /* プレイヤーヒープ構築 */
    playerHeapHandle = NNS_SndHeapCreate(
        (u8*)buffer + sizeof( NNSSndPlayerHeap ),
        size
    );
    if ( playerHeapHandle == NNS_SND_HEAP_INVALID_HANDLE ) {
        return FALSE;
    }
    
    playerHeap->handle = playerHeapHandle;    
    NNS_FndAppendListObject( &sPlayer[ playerNo ].heapList, playerHeap );
    
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerStopSeq

  Description:  シーケンス停止

  Arguments:    handle    - サウンドハンドル
                fadeFrame - フェードアウトフレーム

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerStopSeq( NNSSndHandle* handle, int fadeFrame )
{
    NNS_NULL_ASSERT( handle );
    
    NNSi_SndPlayerStopSeq( handle->player, fadeFrame );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerStopSeqByPlayerNo

  Description:  シーケンス停止

  Arguments:    playerNo  - プレイヤー番号
                fadeFrame - フェードアウトフレーム

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerStopSeqByPlayerNo( int playerNo, int fadeFrame )
{
    NNSSndSeqPlayer* seqPlayer;
    int i;
    
    NNS_MINMAX_ASSERT( playerNo , 0, NNS_SND_PLAYER_NO_MAX );
    
    for( i = 0; i < SND_PLAYER_NUM ; i++ )
    {
        seqPlayer = & sSeqPlayer[ i ];
        
        if ( seqPlayer->status != NNS_SND_SEQ_PLAYER_STATUS_STOP &&
             seqPlayer->player == & sPlayer[ playerNo ] )
        {
            NNSi_SndPlayerStopSeq( seqPlayer, fadeFrame );            
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerStopSeqBySeqNo

  Description:  シーケンス停止

  Arguments:    seqNo     - シーケンス番号
                fadeFrame - フェードアウトフレーム

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerStopSeqBySeqNo( int seqNo, int fadeFrame )
{
    NNSSndSeqPlayer* seqPlayer;
    int i;
    
    for( i = 0; i < SND_PLAYER_NUM ; i++ )
    {
        seqPlayer = & sSeqPlayer[ i ];
        
        if ( seqPlayer->status != NNS_SND_SEQ_PLAYER_STATUS_STOP &&
             seqPlayer->seqType == NNS_SND_PLAYER_SEQ_TYPE_SEQ &&
             seqPlayer->seqNo == seqNo )
        {
            NNSi_SndPlayerStopSeq( seqPlayer ,fadeFrame );
        }
    }    
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerStopSeqBySeqArcNo

  Description:  シーケンス停止

  Arguments:    seqArcNo  - シーケンスアーカイブ番号
                fadeFrame - フェードアウトフレーム

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerStopSeqBySeqArcNo( int seqArcNo, int fadeFrame )
{
    NNSSndSeqPlayer* seqPlayer;
    int i;
    
    for( i = 0; i < SND_PLAYER_NUM ; i++ )
    {
        seqPlayer = & sSeqPlayer[ i ];
        
        if ( seqPlayer->status != NNS_SND_SEQ_PLAYER_STATUS_STOP &&
             seqPlayer->seqType == NNS_SND_PLAYER_SEQ_TYPE_SEQARC &&
             seqPlayer->seqNo == seqArcNo )
        {
            NNSi_SndPlayerStopSeq( seqPlayer ,fadeFrame );
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerStopSeqBySeqArcIdx

  Description:  シーケンス停止

  Arguments:    seqArcNo  - シーケンスアーカイブ番号
                index     - インデックス番号
                fadeFrame - フェードアウトフレーム

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerStopSeqBySeqArcIdx( int seqArcNo, int index, int fadeFrame )
{
    NNSSndSeqPlayer* seqPlayer;
    int i;
    
    for( i = 0; i < SND_PLAYER_NUM ; i++ )
    {
        seqPlayer = & sSeqPlayer[ i ];
        
        if ( seqPlayer->status != NNS_SND_SEQ_PLAYER_STATUS_STOP &&
             seqPlayer->seqType == NNS_SND_PLAYER_SEQ_TYPE_SEQARC &&
             seqPlayer->seqNo == seqArcNo &&
             seqPlayer->seqArcIndex == index )
        {
            NNSi_SndPlayerStopSeq( seqPlayer ,fadeFrame );
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerStopSeqAll

  Description:  全シーケンス停止

  Arguments:    fadeFrame - フェードアウトフレーム

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerStopSeqAll( int fadeFrame )
{
    NNSSndSeqPlayer* seqPlayer;
    int i;
    
    for( i = 0; i < SND_PLAYER_NUM ; i++ )
    {
        seqPlayer = & sSeqPlayer[ i ];
        
        if ( seqPlayer->status != NNS_SND_SEQ_PLAYER_STATUS_STOP )
        {
            NNSi_SndPlayerStopSeq( seqPlayer, fadeFrame );
        }
    }    
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerPause

  Description:  シーケンスの一時停止または再開

  Arguments:    handle - サウンドハンドル
                flag   - 一時停止か再開か

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerPause( NNSSndHandle* handle, BOOL flag )
{
    NNS_NULL_ASSERT( handle );

    NNSi_SndPlayerPause( handle->player, flag );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerPauseByPlayerNo

  Description:  シーケンスの一時停止または再開

  Arguments:    playerNo - プレイヤー番号
                flag     - 一時停止か再開か

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerPauseByPlayerNo( int playerNo, BOOL flag )
{
    NNSSndSeqPlayer* seqPlayer;
    NNSSndSeqPlayer* next;
    
    NNS_MINMAX_ASSERT( playerNo , 0, NNS_SND_PLAYER_NO_MAX );
    
    for( seqPlayer = (NNSSndSeqPlayer*)NNS_FndGetNextListObject( & sPlayer[ playerNo ].playerList, NULL );
         seqPlayer != NULL ; seqPlayer = next )
    {
        next = (NNSSndSeqPlayer*)NNS_FndGetNextListObject( & sPlayer[ playerNo ].playerList, seqPlayer );
        
        NNSi_SndPlayerPause( seqPlayer, flag );
    }    
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerPauseAll

  Description:  全シーケンスの一時停止または再開

  Arguments:    flag - 一時停止か再開か

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerPauseAll( BOOL flag )
{
    NNSSndSeqPlayer* seqPlayer;
    NNSSndSeqPlayer* next;
    
    for( seqPlayer = (NNSSndSeqPlayer*)NNS_FndGetNextListObject( & sPrioList, NULL );
         seqPlayer != NULL ; seqPlayer = next )
    {
        next = (NNSSndSeqPlayer*)NNS_FndGetNextListObject( & sPrioList, seqPlayer );
        
        NNSi_SndPlayerPause( seqPlayer, flag );
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndHandleInit

  Description:  サウンドハンドルの初期化

  Arguments:    handle - サウンドハンドル

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndHandleInit( NNSSndHandle* handle )
{
    NNS_NULL_ASSERT( handle );
    
    handle->player = NULL;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndHandleReleaseSeq

  Description:  サウンドハンドルからシーケンスを解放

  Arguments:    handle - サウンドハンドル

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndHandleReleaseSeq( NNSSndHandle* handle )
{
    NNS_NULL_ASSERT( handle );
    
    if ( ! NNS_SndHandleIsValid( handle ) ) return;
    
    NNS_ASSERT( handle == handle->player->handle );
    
    handle->player->handle = NULL;  
    handle->player = NULL;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerCountPlayingSeqByPlayerNo

  Description:  再生中のシーケンス数取得

  Arguments:    playerNo - プレイヤー番号

  Returns:      再生中のシーケンス数
 *---------------------------------------------------------------------------*/
int NNS_SndPlayerCountPlayingSeqByPlayerNo( int playerNo )
{
    NNS_MINMAX_ASSERT( playerNo , 0, NNS_SND_PLAYER_NO_MAX );
    
    return sPlayer[ playerNo ].playerList.numObjects;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerCountPlayingSeqBySeqNo

  Description:  再生中のシーケンス数取得

  Arguments:    seqNo - シーケンス番号

  Returns:      再生中のシーケンス数
 *---------------------------------------------------------------------------*/
int NNS_SndPlayerCountPlayingSeqBySeqNo( int seqNo )
{
    int count = 0;
    
    NNSSndSeqPlayer* seqPlayer = NULL;
    while ( ( seqPlayer = (NNSSndSeqPlayer*)NNS_FndGetNextListObject( & sPrioList, seqPlayer ) ) != NULL )
    {
        if ( seqPlayer->seqType == NNS_SND_PLAYER_SEQ_TYPE_SEQ &&
             seqPlayer->seqNo == seqNo )
        {
            count++;
        }
    }
    
    return count;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerCountPlayingSeqBySeqArcNo

  Description:  再生中のシーケンス数取得

  Arguments:    seqArcNo - シーケンスアーカイブ番号

  Returns:      再生中のシーケンス数
 *---------------------------------------------------------------------------*/
int NNS_SndPlayerCountPlayingSeqBySeqArcNo( int seqArcNo )
{
    int count = 0;
    
    NNSSndSeqPlayer* seqPlayer = NULL;
    while ( ( seqPlayer = (NNSSndSeqPlayer*)NNS_FndGetNextListObject( & sPrioList, seqPlayer ) ) != NULL )
    {
        if ( seqPlayer->seqType == NNS_SND_PLAYER_SEQ_TYPE_SEQARC &&
             seqPlayer->seqNo == seqArcNo )
        {
            count++;
        }
    }
    
    return count;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerCountPlayingSeqBySeqArcIdx

  Description:  再生中のシーケンス数取得

  Arguments:    seqArcNo - シーケンスアーカイブ番号
                index    - インデックス番号

  Returns:      再生中のシーケンス数
 *---------------------------------------------------------------------------*/
int NNS_SndPlayerCountPlayingSeqBySeqArcIdx( int seqArcNo, int index )
{
    int count = 0;
    
    NNSSndSeqPlayer* seqPlayer = NULL;
    while ( ( seqPlayer = (NNSSndSeqPlayer*)NNS_FndGetNextListObject( & sPrioList, seqPlayer ) ) != NULL )
    {
        if ( seqPlayer->seqType == NNS_SND_PLAYER_SEQ_TYPE_SEQARC &&
             seqPlayer->seqNo == seqArcNo &&
             seqPlayer->seqArcIndex == index )
        {
            count++;
        }
    }
    
    return count;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerSetVolume

  Description:  シーケンスボリュームの変更

  Arguments:    handle - サウンドハンドル
                volume - ボリューム

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerSetVolume( NNSSndHandle* handle, int volume )
{
    NNS_NULL_ASSERT( handle );
    NNS_MINMAX_ASSERT( volume, 0, SND_CALC_DECIBEL_SCALE_MAX );
    
    if ( ! NNS_SndHandleIsValid( handle ) ) return;
    
    handle->player->extVolume = (u8)volume;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerSetInitialVolume

  Description:  シーケンス初期ボリュームの設定

  Arguments:    handle - サウンドハンドル
                volume - ボリューム

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerSetInitialVolume( NNSSndHandle* handle, int volume )
{
    NNS_NULL_ASSERT( handle );
    NNS_MINMAX_ASSERT( volume, 0, SND_CALC_DECIBEL_SCALE_MAX );
    
    if ( ! NNS_SndHandleIsValid( handle ) ) return;
    
    handle->player->initVolume = (u8)volume;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerMoveVolume

  Description:  シーケンスボリュームを時間変化

  Arguments:    handle       - サウンドハンドル
                targetVolume - ターゲットボリューム
                frames       - 変化フレーム数

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerMoveVolume( NNSSndHandle* handle, int targetVolume, int frames )
{
    NNS_NULL_ASSERT( handle );
    NNS_MINMAX_ASSERT( targetVolume, 0, SND_CALC_DECIBEL_SCALE_MAX );
    
    if ( ! NNS_SndHandleIsValid( handle ) ) return;
    
    // フェードアウト中は禁止    
    if ( handle->player->status == NNS_SND_SEQ_PLAYER_STATUS_FADEOUT ) return;
    
    NNSi_SndFaderSet( & handle->player->fader, targetVolume << FADER_SHIFT, frames );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerSetPlayerPriority

  Description:  プレイヤープライオリティを変更

  Arguments:    handle   - サウンドハンドル
                priority - プレイヤープライオリティ

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerSetPlayerPriority( NNSSndHandle* handle, int priority )
{
    NNS_NULL_ASSERT( handle );
    NNS_MINMAX_ASSERT( priority, 0, NNS_SND_PLAYER_PRIO_MAX );
    
    if ( ! NNS_SndHandleIsValid( handle ) ) return;
    
    SetPlayerPriority( handle->player, priority );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerSetChannelPriority

  Description:  発音プライオリティを変更

  Arguments:    handle   - サウンドハンドル
                priority - 発音プライオリティ

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerSetChannelPriority( NNSSndHandle* handle, int priority )
{
    NNS_NULL_ASSERT( handle );
    NNS_MINMAX_ASSERT( priority, 0, 127 );
    
    if ( ! NNS_SndHandleIsValid( handle ) ) return;
    
    SND_SetPlayerChannelPriority( handle->player->playerNo, priority );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerSetTrackMute

  Description:  トラックミュート

  Arguments:    handle       - サウンドハンドル
                trackBitMask - トラックビットマスク
                flag         - ミュートフラグ

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerSetTrackMute( NNSSndHandle* handle, u16 trackBitMask, BOOL flag )
{
    NNS_NULL_ASSERT( handle );
    
    if ( ! NNS_SndHandleIsValid( handle ) ) return;
    
    SND_SetTrackMute(
        handle->player->playerNo,
        trackBitMask,
        flag
    );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerSetTrackMuteEx

  Description:  トラックミュート

  Arguments:    handle       - サウンドハンドル
                trackBitMask - トラックビットマスク
                mute         - ミュート設定

  Returns:      None.
 *---------------------------------------------------------------------------*/
#if SDK_CURRENT_VERSION_NUMBER >= SDK_VERSION_NUMBER(3,1,0)
void NNS_SndPlayerSetTrackMuteEx( NNSSndHandle* handle, u16 trackBitMask, NNSSndSeqMute mute )
{
    NNS_NULL_ASSERT( handle );
    
    if ( ! NNS_SndHandleIsValid( handle ) ) return;
    
    SND_SetTrackMuteEx(
        handle->player->playerNo,
        trackBitMask,
        (SNDSeqMute)mute
    );
}
void SND_SetTrackMuteEx(int playerNo, u32 trackBitMask, SNDSeqMute mute);
SDK_WEAK_SYMBOL void SND_SetTrackMuteEx(int playerNo, u32 trackBitMask, SNDSeqMute mute)
{
    (void)playerNo;
    (void)trackBitMask;
    (void)mute;
    
    NNS_WARNING( FALSE, "SND_SetTrackMuteEx is not supported." );
}
#endif

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerSetTrackVolume

  Description:  トラックボリュームの変更

  Arguments:    handle       - サウンドハンドル
                trackBitMask - トラックビットマスク
                volume       - ボリューム

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerSetTrackVolume( NNSSndHandle* handle, u16 trackBitMask, int volume )
{
    NNS_NULL_ASSERT( handle );
    NNS_MINMAX_ASSERT( volume, 0, SND_CALC_DECIBEL_SCALE_MAX );
    
    if ( ! NNS_SndHandleIsValid( handle ) ) return;
    
    SND_SetTrackVolume(
        handle->player->playerNo,
        trackBitMask,
        SND_CalcDecibel( volume )
    );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerSetTrackPitch

  Description:  トラックピッチの変更

  Arguments:    handle       - サウンドハンドル
                trackBitMask - トラックビットマスク
                pitch        - 音程

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerSetTrackPitch( NNSSndHandle* handle, u16 trackBitMask, int pitch )
{
    NNS_NULL_ASSERT( handle );
    NNS_MINMAX_ASSERT( pitch, NNS_SND_PLAYER_TRACK_PITCH_MIN, NNS_SND_PLAYER_TRACK_PITCH_MAX );
    
    if ( ! NNS_SndHandleIsValid( handle ) ) return;
    
    SND_SetTrackPitch( handle->player->playerNo, trackBitMask, pitch );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerSetTrackPan

  Description:  トラックパンの変更

  Arguments:    handle       - サウンドハンドル
                trackBitMask - トラックビットマスク
                pan          - パン

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerSetTrackPan( NNSSndHandle* handle, u16 trackBitMask, int pan )
{
    NNS_NULL_ASSERT( handle );
    NNS_MINMAX_ASSERT( pan, NNS_SND_PLAYER_TRACK_PAN_MIN, NNS_SND_PLAYER_TRACK_PAN_MAX );
    
    if ( ! NNS_SndHandleIsValid( handle ) ) return;
    
    SND_SetTrackPan( handle->player->playerNo, trackBitMask, pan );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerSetTrackPanRange

  Description:  トラックパンレンジの変更

  Arguments:    handle       - サウンドハンドル
                trackBitMask - トラックビットマスク
                panRange     - パンレンジ

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerSetTrackPanRange( NNSSndHandle* handle, u16 trackBitMask, int panRange )
{
    NNS_NULL_ASSERT( handle );
    NNS_MINMAX_ASSERT( panRange, NNS_SND_PLAYER_TRACK_PAN_RANGE_MIN, NNS_SND_PLAYER_TRACK_PAN_RANGE_MAX );
    
    if ( ! NNS_SndHandleIsValid( handle ) ) return;
    
    SND_SetTrackPanRange( handle->player->playerNo, trackBitMask, panRange );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerSetTrackModDepth

  Description:  モジュレーションデプスの変更

  Arguments:    handle       - サウンドハンドル
                trackBitMask - トラックビットマスク
                depth        - モジュレーションデプス

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerSetTrackModDepth( NNSSndHandle* handle, u16 trackBitMask, int depth )
{
    NNS_NULL_ASSERT( handle );
    NNS_MINMAX_ASSERT( depth, 0, 255 );
    
    if ( ! NNS_SndHandleIsValid( handle ) ) return;
    
    SND_SetTrackModDepth( handle->player->playerNo, trackBitMask, depth );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerSetTrackModSpeed

  Description:  モジュレーションスピードの変更

  Arguments:    handle       - サウンドハンドル
                trackBitMask - トラックビットマスク
                speed        - モジュレーションスピード

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerSetTrackModSpeed( NNSSndHandle* handle, u16 trackBitMask, int speed )
{
    NNS_NULL_ASSERT( handle );
    NNS_MINMAX_ASSERT( speed, 0, 255 );
    
    if ( ! NNS_SndHandleIsValid( handle ) ) return;
    
    SND_SetTrackModSpeed( handle->player->playerNo, trackBitMask, speed );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerSetTrackAllocatableChannel

  Description:  確保できるチャンネルの変更

  Arguments:    handle       - サウンドハンドル
                trackBitMask - トラックビットマスク
                chBitFlag    - 確保できるチャンネルのビットフラグ
                               ０を指定すると、全て確保可能.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerSetTrackAllocatableChannel( NNSSndHandle* handle, u16 trackBitMask, u32 chBitFlag )
{
    NNS_NULL_ASSERT( handle );
    NNS_MINMAX_ASSERT( chBitFlag, 0, 0xffff );
    
    if ( ! NNS_SndHandleIsValid( handle ) ) return;
    
    SND_SetTrackAllocatableChannel( handle->player->playerNo, trackBitMask, chBitFlag );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerSetTempoRatio

  Description:  テンポ比率の変更

  Arguments:    handle - サウンドハンドル
                ratio  - テンポの比率

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerSetTempoRatio( NNSSndHandle* handle, int ratio )
{
    NNS_NULL_ASSERT( handle );
    NNS_MINMAX_ASSERT( ratio, 1, 65535 );
    
    if ( ! NNS_SndHandleIsValid( handle ) ) return;
    
    SND_SetPlayerTempoRatio( handle->player->playerNo, ratio );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerSetSeqNo

  Description:  シーケンス番号の設定

  Arguments:    handle - サウンドハンドル
                seqNo  - シーケンス番号

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerSetSeqNo( NNSSndHandle* handle, int seqNo )
{
    NNS_NULL_ASSERT( handle );
    NNS_MINMAX_ASSERT( seqNo, 0, NNS_SND_PLAYER_SEQ_NO_MAX );
    
    if ( ! NNS_SndHandleIsValid( handle ) ) return;
    
    handle->player->seqType = NNS_SND_PLAYER_SEQ_TYPE_SEQ;
    handle->player->seqNo = (u16)seqNo;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerSetSeqArcNo

  Description:  シーケンスアーカイブ番号の設定

  Arguments:    handle   - サウンドハンドル
                seqArcNo - シーケンスアーカイブ番号
                index    - インデックス番号

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerSetSeqArcNo( NNSSndHandle* handle, int seqArcNo, int index )
{
    NNS_NULL_ASSERT( handle );
    NNS_MINMAX_ASSERT( seqArcNo, 0, NNS_SND_PLAYER_SEQ_NO_MAX );
    NNS_MINMAX_ASSERT( index, 0, NNS_SND_PLAYER_SEQARC_INDEX_MAX );
    
    if ( ! NNS_SndHandleIsValid( handle ) ) return;
    
    handle->player->seqType = NNS_SND_PLAYER_SEQ_TYPE_SEQARC;
    handle->player->seqNo       = (u16)seqArcNo;
    handle->player->seqArcIndex = (u16)index;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerGetSeqType

  Description:  シーケンスタイプの取得

  Arguments:    handle - サウンドハンドル

  Returns:      シーケンスタイプ
 *---------------------------------------------------------------------------*/
NNSSndPlayerSeqType NNS_SndPlayerGetSeqType( NNSSndHandle* handle )
{
    NNS_NULL_ASSERT( handle );
    
    if ( ! NNS_SndHandleIsValid( handle ) ) return NNS_SND_PLAYER_SEQ_TYPE_INVALID;
    
    return (NNSSndPlayerSeqType)( handle->player->seqType );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerGetSeqNo

  Description:  シーケンス番号の取得

  Arguments:    handle - サウンドハンドル

  Returns:      シーケンス番号または、-1
 *---------------------------------------------------------------------------*/
int NNS_SndPlayerGetSeqNo( NNSSndHandle* handle )
{
    NNS_NULL_ASSERT( handle );

    if ( ! NNS_SndHandleIsValid( handle ) ) return -1;
    
    if ( handle->player->seqType != NNS_SND_PLAYER_SEQ_TYPE_SEQ ) return -1;

    return handle->player->seqNo;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerGetSeqArcNo

  Description:  シーケンスアーカイブ番号の取得

  Arguments:    handle - サウンドハンドル

  Returns:      シーケンスアーカイブ番号または、-1
 *---------------------------------------------------------------------------*/
int NNS_SndPlayerGetSeqArcNo( NNSSndHandle* handle )
{
    NNS_NULL_ASSERT( handle );

    if ( ! NNS_SndHandleIsValid( handle ) ) return -1;
    
    if ( handle->player->seqType != NNS_SND_PLAYER_SEQ_TYPE_SEQARC ) return -1;

    return handle->player->seqNo;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerGetSeqArcIdx

  Description:  シーケンスアーカイブインデックスの取得

  Arguments:    handle - サウンドハンドル

  Returns:      シーケンスアーカイブインデックスまたは、-1
 *---------------------------------------------------------------------------*/
int NNS_SndPlayerGetSeqArcIdx( NNSSndHandle* handle )
{
    NNS_NULL_ASSERT( handle );

    if ( ! NNS_SndHandleIsValid( handle ) ) return -1;
    
    if ( handle->player->seqType != NNS_SND_PLAYER_SEQ_TYPE_SEQARC ) return -1;
    
    return handle->player->seqArcIndex;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerReadVariable

  Description:  シーケンスローカル変数の読みとり

  Arguments:    handle - サウンドハンドル
                varNo  - 変数番号
                var    - 読み込み先アドレス

  Returns:      成功したかどうか
 *---------------------------------------------------------------------------*/
BOOL NNS_SndPlayerReadVariable( NNSSndHandle* handle, int varNo, s16* var )
{
    NNSSndSeqPlayer* seqPlayer;
    
    NNS_NULL_ASSERT( handle );
    NNS_NULL_ASSERT( var );
    NNS_MINMAX_ASSERT( varNo, 0, SND_PLAYER_VARIABLE_NUM-1 );
    
    if ( ! NNS_SndHandleIsValid( handle ) ) return FALSE;
    
    seqPlayer = handle->player;
    
    if ( ! seqPlayer->startFlag ) {
        *var = SND_DEFAULT_VARIABLE;
        return TRUE;
    }
    
    *var = SND_GetPlayerLocalVariable( seqPlayer->playerNo, varNo );
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerReadGlobalVariable

  Description:  シーケンスグローバル変数の読みとり

  Arguments:    varNo - 変数番号
                var   - 読み込み先アドレス

  Returns:      成功したかどうか
 *---------------------------------------------------------------------------*/
BOOL NNS_SndPlayerReadGlobalVariable( int varNo, s16* var )
{
    NNS_NULL_ASSERT( var );
    NNS_MINMAX_ASSERT( varNo, 0, SND_GLOBAL_VARIABLE_NUM-1 );
    
    *var = SND_GetPlayerGlobalVariable( varNo );
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerWriteVariable

  Description:  シーケンスローカル変数の書き込み

  Arguments:    handle - サウンドハンドル
                varNo  - 変数番号
                var    - 書き込む値

  Returns:      成功したかどうか
 *---------------------------------------------------------------------------*/
BOOL NNS_SndPlayerWriteVariable( NNSSndHandle* handle, int varNo, s16 var )
{
    NNS_NULL_ASSERT( handle );
    NNS_MINMAX_ASSERT( varNo, 0, SND_PLAYER_VARIABLE_NUM-1 );
    
    if ( ! NNS_SndHandleIsValid( handle ) ) return FALSE;
    
    SND_SetPlayerLocalVariable( handle->player->playerNo, varNo, var );

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerWriteGlobalVariable

  Description:  シーケンスグローバル変数の書き込み

  Arguments:    varNo - 変数番号
                var   - 書き込む値

  Returns:      成功したかどうか
 *---------------------------------------------------------------------------*/
BOOL NNS_SndPlayerWriteGlobalVariable( int varNo, s16 var )
{
    NNS_MINMAX_ASSERT( varNo, 0, SND_GLOBAL_VARIABLE_NUM-1 );
    
    SND_SetPlayerGlobalVariable( varNo, var );

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerGetTick

  Description:  再生開始からのティック数の取得

  Arguments:    handle - サウンドハンドル

  Returns:      ティック数
 *---------------------------------------------------------------------------*/
u32 NNS_SndPlayerGetTick( NNSSndHandle* handle )
{
    NNSSndSeqPlayer* seqPlayer;
    
    NNS_NULL_ASSERT( handle );
    
    if ( ! NNS_SndHandleIsValid( handle ) ) return 0;
    
    seqPlayer = handle->player;
    
    if ( ! seqPlayer->startFlag ) {
        // pre start
        return 0;
    }
    
    return SND_GetPlayerTickCounter( seqPlayer->playerNo );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerReadDriverPlayerInfo

  Description:  ドライバのプレイヤー情報の取得

  Arguments:    handle - サウンドハンドル
                info - 取得したプレイヤー情報を格納するプレイヤー情報構造体

  Returns:      情報の取得に成功したかどうか
 *---------------------------------------------------------------------------*/
BOOL NNS_SndPlayerReadDriverPlayerInfo( NNSSndHandle* handle, SNDPlayerInfo* info )
{
    NNSSndSeqPlayer* seqPlayer;
    
    NNS_NULL_ASSERT( handle );
    
    if ( ! NNS_SndHandleIsValid( handle ) ) return FALSE;
    
    seqPlayer = handle->player;
    NNS_NULL_ASSERT( seqPlayer );
    
    return NNSi_SndReadDriverPlayerInfo( seqPlayer->playerNo, info );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerReadDriverTrackInfo

  Description:  ドライバのトラック情報の取得

  Arguments:    handle - サウンドハンドル
                trackNo - トラック番号
                info - 取得したプレイヤー情報を格納するトラック情報構造体

  Returns:      情報の取得に成功したかどうか
 *---------------------------------------------------------------------------*/
BOOL NNS_SndPlayerReadDriverTrackInfo( NNSSndHandle* handle, int trackNo, SNDTrackInfo* info )
{
    NNSSndSeqPlayer* seqPlayer;
    
    NNS_NULL_ASSERT( handle );
    
    if ( ! NNS_SndHandleIsValid( handle ) ) return FALSE;
    
    seqPlayer = handle->player;
    NNS_NULL_ASSERT( seqPlayer );
    
    return NNSi_SndReadDriverTrackInfo( seqPlayer->playerNo, trackNo, info );
}

/******************************************************************************
    private functions
 ******************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         NNSi_SndPlayerInit

  Description:  プレイヤーライブラリの初期化

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNSi_SndPlayerInit( void )
{
    NNSSndPlayer* player;
    int playerNo;
    
    NNS_FND_INIT_LIST( &sPrioList, NNSSndSeqPlayer, prioLink );
    NNS_FND_INIT_LIST( &sFreeList, NNSSndSeqPlayer, prioLink );
    
    for( playerNo = 0; playerNo < SND_PLAYER_NUM ; playerNo++ )
    {
        sSeqPlayer[ playerNo ].status = NNS_SND_SEQ_PLAYER_STATUS_STOP;
        sSeqPlayer[ playerNo ].playerNo = (u8)playerNo;
        NNS_FndAppendListObject( & sFreeList, & sSeqPlayer[ playerNo ] );
    }
    
    for( playerNo = 0; playerNo < NNS_SND_PLAYER_NUM ; playerNo++ )
    {
        player = &sPlayer[ playerNo ];
        
        NNS_FND_INIT_LIST( & player->playerList, NNSSndSeqPlayer, playerLink );
        NNS_FND_INIT_LIST( & player->heapList, NNSSndPlayerHeap, link );
        player->volume = 127;
        player->playableSeqCount = 1;
        player->allocChBitFlag = 0;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_SndPlayerMain

  Description:  プレイヤーライブラリのフレームワーク

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNSi_SndPlayerMain( void )
{
    NNSSndSeqPlayer* seqPlayer;
    NNSSndSeqPlayer* next;
    u32 status;
    int fader;
    
    // プレイヤーステータスの取得
    status = SND_GetPlayerStatus();
    
    for( seqPlayer = (NNSSndSeqPlayer*)NNS_FndGetNextListObject( & sPrioList, NULL );
         seqPlayer != NULL ; seqPlayer = next )
    {
        next = (NNSSndSeqPlayer*)NNS_FndGetNextListObject( & sPrioList, seqPlayer );
        
        // スタートフラグチェック
        if ( ! seqPlayer->startFlag ) {
            if ( SND_IsFinishedCommandTag( seqPlayer->commandTag ) )
            {
                seqPlayer->startFlag = TRUE;
            }
        }
        
        // ARM7側からの停止チェック
        if ( seqPlayer->startFlag )
        {
            if ( ( status & ( 1 << seqPlayer->playerNo ) ) == 0 ) {
                ShutdownPlayer( seqPlayer );
                continue;
            }
        }
        
        // フェーダー更新
        NNSi_SndFaderUpdate( & seqPlayer->fader );
        
        // パラメータの更新
        fader
            = SND_CalcDecibel( seqPlayer->initVolume )
            + SND_CalcDecibel( seqPlayer->extVolume )
            + SND_CalcDecibel( seqPlayer->player->volume )
            + SND_CalcDecibel( NNSi_SndFaderGet( & seqPlayer->fader ) >> FADER_SHIFT )
            ;
        if ( fader < -32768 ) fader = -32768;
        else if ( fader > 32767 ) fader = 32767;
        
        if ( fader != seqPlayer->volume )
        {
            SND_SetPlayerVolume( seqPlayer->playerNo, fader );
            seqPlayer->volume = (s16)fader;
        }
        
        // フェードアウト完了チェック
        if ( seqPlayer->status == NNS_SND_SEQ_PLAYER_STATUS_FADEOUT )
        {
            if ( NNSi_SndFaderIsFinished( & seqPlayer->fader ) )
            {
                ForceStopSeq( seqPlayer );
            }
        }
        
        // 準備完了フラグ
        if ( seqPlayer->prepareFlag ) {
            SND_StartPreparedSeq( seqPlayer->playerNo );
            seqPlayer->prepareFlag = FALSE;
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_SndPlayerAllocSeqPlayer

  Description:  シーケンスプレイヤーを確保

  Arguments:    handle   - サウンドハンドル
                playerNo - プレイヤー番号
                prio     - プレイヤープライオリティ

  Returns:      確保したシーケンスプレイヤー
                失敗時はNULL
 *---------------------------------------------------------------------------*/
NNSSndSeqPlayer* NNSi_SndPlayerAllocSeqPlayer( NNSSndHandle* handle, int playerNo, int prio )
{
    NNSSndSeqPlayer* seqPlayer;
    NNSSndPlayer* player;

    NNS_NULL_ASSERT( handle );
    NNS_MINMAX_ASSERT( playerNo, 0, NNS_SND_PLAYER_NO_MAX );
    NNS_MINMAX_ASSERT( prio, 0, NNS_SND_PLAYER_PRIO_MAX );
    
    player = & sPlayer[ playerNo ];
    
    // サウンドハンドルの切断
    if ( NNS_SndHandleIsValid( handle ) ) {
        NNS_SndHandleReleaseSeq( handle );
    }
    
    // プレイヤー毎の最大同時再生数のチェック
    if ( player->playerList.numObjects >= player->playableSeqCount )
    {
        // 一番プライオリティの低いものを取得
        seqPlayer = (NNSSndSeqPlayer*)NNS_FndGetNextListObject( & player->playerList, NULL );
        if ( seqPlayer == NULL ) return NULL;
        if ( prio < seqPlayer->prio ) return NULL;
        
        ForceStopSeq( seqPlayer );
    }
    
    // 確保処理
    seqPlayer = AllocSeqPlayer( prio );
    if ( seqPlayer == NULL ) return NULL;
    
    // 初期化
    InsertPlayerList( player, seqPlayer );
    
    // サウンドハンドルとの接続
    seqPlayer->handle = handle;
    handle->player = seqPlayer;
    
    return seqPlayer;
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_SndPlayerFreeSeqPlayer

  Description:  シーケンスプレイヤーを解放

  Arguments:    seqPlayer - シーケンスプレイヤー

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNSi_SndPlayerFreeSeqPlayer( NNSSndSeqPlayer* seqPlayer )
{
    NNS_NULL_ASSERT( seqPlayer );

    ShutdownPlayer( seqPlayer );
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_SndPlayerStartSeq

  Description:  シーケンス開始

  Arguments:    seqPlayer     - シーケンスプレイヤー
                seqDataBase   - シーケンスデータベースアドレス
                seqDataOffset - シーケンスデータのオフセット
                bank          - バンクデータ

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNSi_SndPlayerStartSeq(
    NNSSndSeqPlayer* seqPlayer,
    const void* seqDataBase,
    u32 seqDataOffset,
    const SNDBankData* bank
)
{
    NNSSndPlayer* player;
    
    NNS_NULL_ASSERT( seqPlayer );
    NNS_NULL_ASSERT( seqDataBase );
    NNS_NULL_ASSERT( bank );
    
    player = seqPlayer->player;
    NNS_NULL_ASSERT( player );
    
    SND_PrepareSeq(
        seqPlayer->playerNo,
        seqDataBase,
        seqDataOffset,
        bank
    );
    if ( player->allocChBitFlag ) {
        SND_SetTrackAllocatableChannel(
            seqPlayer->playerNo,
            0xffff,
            player->allocChBitFlag
        );
    }
    
    // 初期化処理
    InitPlayer( seqPlayer );
    seqPlayer->commandTag = SND_GetCurrentCommandTag();
    seqPlayer->prepareFlag = TRUE;
    seqPlayer->status = NNS_SND_SEQ_PLAYER_STATUS_PLAY;
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_SndPlayerStopSeq

  Description:  シーケンス停止

  Arguments:    seqPlayer - シーケンスプレイヤー
                fadeFrame - フェードアウトフレーム

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNSi_SndPlayerStopSeq( NNSSndSeqPlayer* seqPlayer, int fadeFrame )
{
    if ( seqPlayer == NULL ) return;
    if ( seqPlayer->status == NNS_SND_SEQ_PLAYER_STATUS_STOP ) return;
    
    if ( fadeFrame == 0 )
    {
        ForceStopSeq( seqPlayer );
        return;
    }
    
    NNSi_SndFaderSet( & seqPlayer->fader, 0, fadeFrame );
    
    SetPlayerPriority( seqPlayer, 0 );
    
    seqPlayer->status = NNS_SND_SEQ_PLAYER_STATUS_FADEOUT;
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_SndPlayerPause

  Description:  シーケンスの一時停止または再開

  Arguments:    seqPlayer - シーケンスプレイヤー
                flag      - 一時停止または再開

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNSi_SndPlayerPause( NNSSndSeqPlayer* seqPlayer, BOOL flag )
{
    if ( seqPlayer == NULL ) return;
    
    if ( flag != seqPlayer->pauseFlag )
    {
        SND_PauseSeq( seqPlayer->playerNo, flag );        
        seqPlayer->pauseFlag = (u8)flag;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_SndPlayerAllocHeap

  Description:  プレイヤーヒープの確保

  Arguments:    playerNo - プレイヤー番号
                seqPlayer - ヒープを使用するシーケンスプレイヤー

  Returns:      確保したヒープ
 *---------------------------------------------------------------------------*/
NNSSndHeapHandle NNSi_SndPlayerAllocHeap( int playerNo, NNSSndSeqPlayer* seqPlayer )
{
    NNSSndPlayer* player;
    NNSSndPlayerHeap* heap;
    
    NNS_MINMAX_ASSERT( playerNo, 0, NNS_SND_PLAYER_NO_MAX );
    
    player = & sPlayer[ playerNo ];
    
    heap = (NNSSndPlayerHeap*)NNS_FndGetNextListObject( & player->heapList, NULL );
    if ( heap == NULL ) return NULL;
    
    NNS_FndRemoveListObject( & player->heapList, heap );
    
    heap->player = seqPlayer;
    seqPlayer->heap = heap;
    
    NNS_SndHeapClear( heap->handle );
    
    return heap->handle;
}

/******************************************************************************
    static functions
 ******************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         InitPlayer

  Description:  シーケンスプレイヤーの初期化

  Arguments:    seqPlayer - シーケンスプレイヤー

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void InitPlayer( NNSSndSeqPlayer* seqPlayer )
{
    NNS_NULL_ASSERT( seqPlayer );
    
    seqPlayer->pauseFlag = FALSE;
    seqPlayer->startFlag = FALSE;
    seqPlayer->prepareFlag = FALSE;
    
    seqPlayer->seqType = NNS_SND_PLAYER_SEQ_TYPE_INVALID;
    
    seqPlayer->volume = 0;
    
    seqPlayer->initVolume = 127;
    seqPlayer->extVolume = 127;
    
    NNSi_SndFaderInit( & seqPlayer->fader );
    NNSi_SndFaderSet( & seqPlayer->fader, 127 << FADER_SHIFT, 1 );
}

/*---------------------------------------------------------------------------*
  Name:         InsertPlayerList

  Description:  プレイヤーリストへの追加

  Arguments:    player    - プレイヤー
                seqPlayer - シーケンスプレイヤー

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void InsertPlayerList( NNSSndPlayer* player, NNSSndSeqPlayer* seqPlayer )
{
    NNSSndSeqPlayer* next = NULL;
    while ( ( next = (NNSSndSeqPlayer*)NNS_FndGetNextListObject( & player->playerList, next ) ) != NULL )
    {
        if ( seqPlayer->prio < next->prio ) break;
    }
    
    NNS_FndInsertListObject( & player->playerList, next, seqPlayer );
    
    seqPlayer->player = player;
}

/*---------------------------------------------------------------------------*
  Name:         InsertPrioList

  Description:  プライオリティリストへの追加

  Arguments:    seqPlayer - シーケンスプレイヤー

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void InsertPrioList( NNSSndSeqPlayer* seqPlayer )
{
    NNSSndSeqPlayer* next = NULL;
    while ( ( next = (NNSSndSeqPlayer*)NNS_FndGetNextListObject( & sPrioList, next ) ) != NULL )
    {
        if ( seqPlayer->prio < next->prio ) break;
    }

    NNS_FndInsertListObject( & sPrioList, next, seqPlayer );
}

/*---------------------------------------------------------------------------*
  Name:         ForceStopSeq

  Description:  シーケンスの強制停止

  Arguments:    seqPlayer - シーケンスプレイヤー

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void ForceStopSeq( NNSSndSeqPlayer* seqPlayer )
{
    if ( seqPlayer->status == NNS_SND_SEQ_PLAYER_STATUS_FADEOUT ) {
        SND_SetPlayerVolume( seqPlayer->playerNo, SND_VOLUME_DB_MIN );
    }
    SND_StopSeq( seqPlayer->playerNo );
    ShutdownPlayer( seqPlayer );
}

/*---------------------------------------------------------------------------*
  Name:         AllocSeqPlayer

  Description:  シーケンスプレイヤーの確保

  Arguments:    prio - プレイヤープライオリティ

  Returns:      確保したシーケンスプレイヤー
                失敗時はNULL
 *---------------------------------------------------------------------------*/
static NNSSndSeqPlayer* AllocSeqPlayer( int prio )
{
    NNSSndSeqPlayer* seqPlayer;
    
    NNS_MINMAX_ASSERT( prio, 0, NNS_SND_PLAYER_PRIO_MAX );
    
    seqPlayer = (NNSSndSeqPlayer*)NNS_FndGetNextListObject( & sFreeList, NULL );
    if ( seqPlayer == NULL )
    {
        // get lowest priority player
        seqPlayer = (NNSSndSeqPlayer*)NNS_FndGetNextListObject( & sPrioList, NULL );        
        NNS_NULL_ASSERT( seqPlayer );
        
        if ( prio < seqPlayer->prio ) return NULL;
        
        ForceStopSeq( seqPlayer );
    }
    NNS_FndRemoveListObject( & sFreeList, seqPlayer );
    
    seqPlayer->prio = (u8)prio;
    
    InsertPrioList( seqPlayer );
    
    return seqPlayer;
}

/*---------------------------------------------------------------------------*
  Name:         ShutdownPlayer

  Description:  シーケンス停止の後始末

  Arguments:    seqPlayer - シーケンスプレイヤー

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void ShutdownPlayer( NNSSndSeqPlayer* seqPlayer )
{
    NNSSndPlayer* player;
    
    NNS_NULL_ASSERT( seqPlayer );
    
    // サウンドハンドルとの切断
    if ( seqPlayer->handle != NULL ) {
        NNS_ASSERT( seqPlayer == seqPlayer->handle->player );
        seqPlayer->handle->player = NULL;
        seqPlayer->handle = NULL;
    }
    
    // プレイヤーとの切断
    player = seqPlayer->player;
    NNS_NULL_ASSERT( player );
    NNS_FndRemoveListObject( & player->playerList, seqPlayer );        
    seqPlayer->player = NULL;
    
    // ヒープの解放
    if ( seqPlayer->heap != NULL ) {
        NNS_FndAppendListObject( & player->heapList, seqPlayer->heap );
        seqPlayer->heap->player = NULL;
        seqPlayer->heap = NULL;
    }
    
    // フリーリストへ移動
    NNS_FndRemoveListObject( &sPrioList, seqPlayer );
    NNS_FndAppendListObject( &sFreeList, seqPlayer );
    
    seqPlayer->status = NNS_SND_SEQ_PLAYER_STATUS_STOP;
}

/*---------------------------------------------------------------------------*
  Name:         PlayerHeapDisposeCallback

  Description:  プレイヤーヒープ破棄時の処理

  Arguments:    mem   - メモリブロックの先頭アドレス
                size  - メモリブロックのサイズ
                data1 - ユーザーデータ（未使用）
                data2 - ユーザーデータ（未使用）

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void PlayerHeapDisposeCallback( void* mem, u32 /*size*/, u32 /*data1*/, u32 /*data2*/ )
{
    NNSSndPlayerHeap* heap = (NNSSndPlayerHeap*)mem;
    NNSSndSeqPlayer* seqPlayer;
    
    if ( heap->handle == NNS_SND_HEAP_INVALID_HANDLE ) return;
    
    // ヒープクリア
    NNS_SndHeapDestroy( heap->handle );
    
    seqPlayer = heap->player;
    if ( seqPlayer != NULL ) {
        // 使用中は、プレイヤーと切断
        seqPlayer->heap = NULL;
    }
    else {
        // 未使用時には、ヒープリストから削除
        NNS_FndRemoveListObject( &sPlayer[ heap->playerNo ].heapList, heap );
    }
}

/*---------------------------------------------------------------------------*
  Name:         SetPlayerPriority

  Description:  プレイヤープライオリティを変更

  Arguments:    seqPlayer - シーケンスプレイヤー
                priority  - プレイヤープライオリティ

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void SetPlayerPriority( NNSSndSeqPlayer* seqPlayer, int priority )
{
    NNSSndPlayer* player;
    
    NNS_NULL_ASSERT( seqPlayer );
    NNS_MINMAX_ASSERT( priority, 0, NNS_SND_PLAYER_PRIO_MAX );
    
    player = seqPlayer->player;
    if ( player != NULL ) {
        NNS_FndRemoveListObject( & player->playerList, seqPlayer );
        seqPlayer->player = NULL;
    }
    NNS_FndRemoveListObject( & sPrioList, seqPlayer );
    
    seqPlayer->prio = (u8)priority;
    
    if ( player != NULL ) {
        InsertPlayerList( player, seqPlayer );
    }
    InsertPrioList( seqPlayer );
}


/*====== End of player.c ======*/

