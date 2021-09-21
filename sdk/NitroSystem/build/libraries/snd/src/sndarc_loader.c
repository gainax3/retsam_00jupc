/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - snd
  File:     sndarc_loader.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.30 $
 *---------------------------------------------------------------------------*/
#include <nnsys/snd/sndarc_loader.h>

#include <nitro/types.h>
#include <nnsys/snd/seqdata.h>
#include <nnsys/snd/sndarc.h>

#include <nitro/snd.h>
#include <nnsys/misc.h>
#include <nnsys/snd/heap.h>

/******************************************************************************
	macro definition
 ******************************************************************************/

#define RESERVED_AREASIZE 32

#define ROUNDUP( value, align ) ( ( (value) + ( (align) - 1 ) ) & ~( (align) - 1 ) )
#define SNDHEAP_ALIGN( value ) ( ROUNDUP( value + RESERVED_AREASIZE, 32 ) + 32 )

/******************************************************************************
    static function declarations
 ******************************************************************************/

static NNSSndSeqData* LoadSeq( u32 fileId, NNSSndHeapHandle heap, BOOL bSetAddr );
static NNSSndSeqArc*  LoadSeqArc( u32 fileId, NNSSndHeapHandle heap, BOOL bSetAddr );
static SNDBankData*   LoadBank( u32 fileId, NNSSndHeapHandle heap, BOOL bSetAddr );
static SNDWaveArc*    LoadWaveArc( u32 fileId, NNSSndHeapHandle heap, BOOL bSetAddr );
static SNDWaveArc*    LoadWaveArcTable( u32 fileId, NNSSndHeapHandle heap, BOOL bSetAddr );

static BOOL LoadSingleWaves( SNDWaveArc* waveArc, const SNDBankData* bank, int waveArcNo, u32 fileId, NNSSndHeapHandle heap );
static BOOL LoadSingleWave( SNDWaveArc* waveArc, int waveNo, u32 fileId, NNSSndHeapHandle heap );

static void DisposeCallback( void* mem, NNSSndArc* arc, u32 fileId );
static void SeqDisposeCallback( void* mem, u32 size, u32 data1, u32 data2 );
static void BankDisposeCallback( void* mem, u32 size, u32 data1, u32 data2 );
static void WaveArcDisposeCallback( void* mem, u32 size, u32 data1, u32 data2 );
static void WaveArcTableDisposeCallback( void* mem, u32 size, u32 data1, u32 data2 );
static void SingleWaveDisposeCallback( void* mem, u32 size, u32 data1, u32 data2 );

/******************************************************************************
    public function
 ******************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         NNS_SndArcLoadGroup

  Description:  グループをロード

  Arguments:    groupNo - グループ番号
                heap    - データを格納するヒープ

  Returns:      ロードに成功したかどうか
 *---------------------------------------------------------------------------*/
BOOL NNS_SndArcLoadGroup( int groupNo, NNSSndHeapHandle heap )
{
    NNSSndArcLoadResult result;
    
    result = NNSi_SndArcLoadGroup( groupNo, heap );
    
    return result == NNS_SND_ARC_LOAD_SUCESS ? TRUE : FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndArcLoadSeq

  Description:  シーケンスデータをロード

  Arguments:    seqNo - シーケンス番号
                heap  - データを格納するヒープ

  Returns:      ロードに成功したかどうか
 *---------------------------------------------------------------------------*/
BOOL NNS_SndArcLoadSeq( int seqNo, NNSSndHeapHandle heap )
{
    NNSSndArcLoadResult result;

    result = NNSi_SndArcLoadSeq( seqNo, NNS_SND_ARC_LOAD_ALL, heap, TRUE, NULL );
    
    return result == NNS_SND_ARC_LOAD_SUCESS ? TRUE : FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndArcLoadSeqArc

  Description:  シーケンスアーカイブをロード

  Arguments:    seqArcNo - シーケンスアーカイブ番号
                heap     - データを格納するヒープ

  Returns:      ロードに成功したかどうか
 *---------------------------------------------------------------------------*/
BOOL NNS_SndArcLoadSeqArc( int seqArcNo, NNSSndHeapHandle heap )
{
    NNSSndArcLoadResult result;

    result = NNSi_SndArcLoadSeqArc( seqArcNo, NNS_SND_ARC_LOAD_ALL, heap, TRUE, NULL );
    
    return result == NNS_SND_ARC_LOAD_SUCESS ? TRUE : FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndArcLoadBank

  Description:  バンクデータをロード

  Arguments:    bankNo - バンク番号
                heap   - データを格納するヒープ

  Returns:      ロードに成功したかどうか
 *---------------------------------------------------------------------------*/
BOOL NNS_SndArcLoadBank( int bankNo, NNSSndHeapHandle heap )
{
    NNSSndArcLoadResult result;

    result = NNSi_SndArcLoadBank( bankNo, NNS_SND_ARC_LOAD_ALL, heap, TRUE, NULL );
    
    return result == NNS_SND_ARC_LOAD_SUCESS ? TRUE : FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndArcLoadWaveArc

  Description:  波形アーカイブをロード

  Arguments:    waveArcNo - 波形アーカイブ番号
                heap      - データを格納するヒープ

  Returns:      ロードに成功したかどうか
 *---------------------------------------------------------------------------*/
BOOL NNS_SndArcLoadWaveArc( int waveArcNo, NNSSndHeapHandle heap )
{
    NNSSndArcLoadResult result;
    
    result = NNSi_SndArcLoadWaveArc( waveArcNo, NNS_SND_ARC_LOAD_ALL, heap, TRUE, NULL );
    
    return result == NNS_SND_ARC_LOAD_SUCESS ? TRUE : FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndArcLoadSeqEx

  Description:  シーケンスデータをロード

  Arguments:    seqNo    - シーケンス番号
                loadFlag - ロードするデータ指定フラグ
                heap     - データを格納するヒープ

  Returns:      ロードに成功したかどうか
 *---------------------------------------------------------------------------*/
BOOL NNS_SndArcLoadSeqEx( int seqNo, u32 loadFlag, NNSSndHeapHandle heap )
{
    NNSSndArcLoadResult result;
    
    result = NNSi_SndArcLoadSeq( seqNo, loadFlag, heap, TRUE, NULL );
    
    return result == NNS_SND_ARC_LOAD_SUCESS ? TRUE : FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndArcLoadBankEx

  Description:  バンクデータをロード

  Arguments:    bankNo - バンク番号
                loadFlag - ロードするデータ指定フラグ
                heap   - データを格納するヒープ

  Returns:      ロードに成功したかどうか
 *---------------------------------------------------------------------------*/
BOOL NNS_SndArcLoadBankEx( int bankNo, u32 loadFlag, NNSSndHeapHandle heap )
{
    NNSSndArcLoadResult result;
    
    result = NNSi_SndArcLoadBank( bankNo, loadFlag, heap, TRUE, NULL );
    
    return result == NNS_SND_ARC_LOAD_SUCESS ? TRUE : FALSE;
}

/******************************************************************************
	private function
 ******************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         NNSi_SndArcLoadGroup

  Description:  グループをロード

  Arguments:    groupNo - グループ番号
                heap    - データを格納するヒープ

  Returns:      ロードに成功したかどうか
 *---------------------------------------------------------------------------*/
NNSSndArcLoadResult NNSi_SndArcLoadGroup( int groupNo, NNSSndHeapHandle heap )
{
    const NNSSndArcGroupInfo* info;
    const NNSSndArcGroupItem* item;
    NNSSndArcLoadResult result;
    int itemNo;
    
    info = NNS_SndArcGetGroupInfo( groupNo );
    if ( info == NULL ) return NNS_SND_ARC_LOAD_ERROR_INVALID_GROUP_NO;
    
    for( itemNo = 0; itemNo < info->count ; itemNo++ )
    {
        item = & info->item[ itemNo ];
        
        switch( item->type ) {
        case NNS_SNDARC_SNDTYPE_SEQ:
            result = NNSi_SndArcLoadSeq( (int)( item->index ), item->loadFlag, heap, TRUE, NULL );
            if ( result != NNS_SND_ARC_LOAD_SUCESS ) return result;
            break;
        case NNS_SNDARC_SNDTYPE_SEQARC:
            result = NNSi_SndArcLoadSeqArc( (int)( item->index ), item->loadFlag, heap, TRUE, NULL );
            if ( result != NNS_SND_ARC_LOAD_SUCESS ) return result;
            break;
        case NNS_SNDARC_SNDTYPE_BANK:
            result = NNSi_SndArcLoadBank( (int)( item->index ), item->loadFlag, heap, TRUE, NULL );
            if ( result != NNS_SND_ARC_LOAD_SUCESS ) return result;
            break;
        case NNS_SNDARC_SNDTYPE_WAVEARC:
            result = NNSi_SndArcLoadWaveArc( (int)( item->index ), item->loadFlag, heap, TRUE, NULL );
            if ( result != NNS_SND_ARC_LOAD_SUCESS ) return result;
            break;
        default:
            NNS_ASSERT( FALSE );
            break;
        }
    }
    
    return NNS_SND_ARC_LOAD_SUCESS;
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_SndArcLoadSeq

  Description:  シーケンスデータをロード

  Arguments:    seqNo    - シーケンス番号
                loadFlag - ロードするデータ指定フラグ
                heap     - データを格納するヒープ
                bSetAddr - アドレスをNNS_SndArcSetFileAddressでセットするかどうか
                pData    - ロードしたデータの位置を取得するためのポインタ

  Returns:      結果コード
 *---------------------------------------------------------------------------*/
NNSSndArcLoadResult NNSi_SndArcLoadSeq( int seqNo, u32 loadFlag, NNSSndHeapHandle heap, BOOL bSetAddr, struct NNSSndSeqData** pData )
{
    const NNSSndArcSeqInfo* seqInfo;
    NNSSndSeqData* seqData = NULL;
    SNDBankData* bank = NULL;
    NNSSndArcLoadResult result;
    
    // 情報の取得
    seqInfo = NNS_SndArcGetSeqInfo( seqNo );
    if ( seqInfo == NULL ) return NNS_SND_ARC_LOAD_ERROR_INVALID_SEQ_NO;
    
    // バンク（波形データ）のロード
    result = NNSi_SndArcLoadBank( seqInfo->param.bankNo, loadFlag, heap, bSetAddr, NULL );
    if ( result != NNS_SND_ARC_LOAD_SUCESS ) return result;
    
    // シーケンスデータのロード
    if ( loadFlag & NNS_SND_ARC_LOAD_SEQ )
    {
        seqData = LoadSeq( seqInfo->fileId, heap, bSetAddr );
        if ( seqData == NULL ) {
            return NNS_SND_ARC_LOAD_ERROR_FAILED_LOAD_SEQ;
        }
    }
    else
    {
        seqData = (NNSSndSeqData*)NNS_SndArcGetFileAddress( seqInfo->fileId );
    }
    
    if ( pData != NULL ) *pData = seqData;
    
    return NNS_SND_ARC_LOAD_SUCESS;
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_SndArcLoadSeqArc

  Description:  シーケンスアーカイブをロード

  Arguments:    seqArcNo - シーケンスアーカイブ番号
                loadFlag - ロードするデータ指定フラグ
                heap     - データを格納するヒープ
                bSetAddr - アドレスをNNS_SndArcSetFileAddressでセットするかどうか
                pData    - ロードしたデータの位置を取得するためのポインタ

  Returns:      結果コード
 *---------------------------------------------------------------------------*/
NNSSndArcLoadResult NNSi_SndArcLoadSeqArc( int seqArcNo, u32 loadFlag, NNSSndHeapHandle heap, BOOL bSetAddr, struct NNSSndSeqArc** pData )
{
    const NNSSndArcSeqArcInfo* seqArcInfo;
    NNSSndSeqArc* seqArc = NULL;
    
    // 情報の取得
    seqArcInfo = NNS_SndArcGetSeqArcInfo( seqArcNo );
    if ( seqArcInfo == NULL ) return NNS_SND_ARC_LOAD_ERROR_INVALID_SEQARC_NO;
    
    // シーケンスアーカイブのロード
    if ( loadFlag & NNS_SND_ARC_LOAD_SEQARC )
    {
        seqArc = LoadSeqArc( seqArcInfo->fileId, heap, bSetAddr );
        if ( seqArc == NULL ) {
            return NNS_SND_ARC_LOAD_ERROR_FAILED_LOAD_SEQARC;
        }
    }
    else
    {
        seqArc = (NNSSndSeqArc*)NNS_SndArcGetFileAddress( seqArcInfo->fileId );
    }
    
    if ( pData != NULL ) *pData = seqArc;
                             
    return NNS_SND_ARC_LOAD_SUCESS;
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_SndArcLoadBank

  Description:  バンクデータをロード

  Arguments:    bankNo   - バンク番号
                loadFlag - ロードするデータ指定フラグ
                heap     - データを格納するヒープ
                bSetAddr - アドレスをNNS_SndArcSetFileAddressでセットするかどうか
                pData    - ロードしたデータの位置を取得するためのポインタ

  Returns:      結果コード
 *---------------------------------------------------------------------------*/
NNSSndArcLoadResult NNSi_SndArcLoadBank(
    int bankNo,
    u32 loadFlag,
    NNSSndHeapHandle heap,
    BOOL bSetAddr,
    struct SNDBankData** pData
)
{
    const NNSSndArcBankInfo* bankInfo;
    const NNSSndArcWaveArcInfo* waveArcInfo;
    SNDBankData* bank = NULL;
    SNDWaveArc* waveArc;
    NNSSndArcLoadResult result;
    int i;

    // 情報の取得
    bankInfo = NNS_SndArcGetBankInfo( bankNo );
    if ( bankInfo == NULL ) return NNS_SND_ARC_LOAD_ERROR_INVALID_BANK_NO;
    
    // バンクのロード
    if ( loadFlag & NNS_SND_ARC_LOAD_BANK )
    {
        bank = LoadBank( bankInfo->fileId, heap, bSetAddr );
        if ( bank == NULL ) {
            return NNS_SND_ARC_LOAD_ERROR_FAILED_LOAD_BANK;
        }
    }
    else
    {
        bank = (SNDBankData*)NNS_SndArcGetFileAddress( bankInfo->fileId );
    }
    
    // 波形データのロード
    for( i = 0; i < NNS_SND_ARC_BANK_TO_WAVEARC_NUM ; i++ )
    {
        if ( bankInfo->waveArcNo[i] == NNS_SND_ARC_INVALID_WAVEARC_NO ) continue;
            
        // 波形アーカイブ情報の取得
        waveArcInfo = NNS_SndArcGetWaveArcInfo( bankInfo->waveArcNo[i] );
        if ( waveArcInfo == NULL ) return NNS_SND_ARC_LOAD_ERROR_INVALID_WAVEARC_NO;
        
        // 波形アーカイブのロード
        result = NNSi_SndArcLoadWaveArc( bankInfo->waveArcNo[i], loadFlag, heap, bSetAddr, &waveArc );
        if ( result != NNS_SND_ARC_LOAD_SUCESS ) return result;
                
        if ( waveArcInfo->flags & NNS_SND_ARC_WAVEARC_SINGLE_LOAD )
        {
            // 波形データ個別ロード
            if ( loadFlag & NNS_SND_ARC_LOAD_WAVE )
            {
                if ( ! LoadSingleWaves( waveArc, bank, i, waveArcInfo->fileId, heap ) ) {
                    return NNS_SND_ARC_LOAD_ERROR_FAILED_LOAD_WAVE;
                }
            }
        }
            
        // 波形とバンクの関連づけ
        if ( bank != NULL && waveArc != NULL ) {
            SND_AssignWaveArc( bank, i, waveArc );
        }
    }
    
    if ( pData != NULL ) *pData = bank;
    
    return NNS_SND_ARC_LOAD_SUCESS;
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_SndArcLoadWaveArc

  Description:  波形データをロード

  Arguments:    waveArcNo - 波形アーカイブ番号
                loadFlag - ロードするデータ指定フラグ
                heap     - データを格納するヒープ
                bSetAddr - アドレスをNNS_SndArcSetFileAddressでセットするかどうか
                pData    - ロードしたデータの位置を取得するためのポインタ

  Returns:      結果コード
 *---------------------------------------------------------------------------*/
NNSSndArcLoadResult NNSi_SndArcLoadWaveArc(
    int waveArcNo,
    u32 loadFlag,
    NNSSndHeapHandle heap,
    BOOL bSetAddr,
    struct SNDWaveArc** pData
)
{
    const NNSSndArcWaveArcInfo* waveArcInfo;
    SNDWaveArc* waveArc = NULL;
    
    waveArcInfo = NNS_SndArcGetWaveArcInfo( waveArcNo );
    if ( waveArcInfo == NULL ) return NNS_SND_ARC_LOAD_ERROR_INVALID_WAVEARC_NO;
    
    if ( loadFlag & NNS_SND_ARC_LOAD_WAVE )
    {
        if ( waveArcInfo->flags & NNS_SND_ARC_WAVEARC_SINGLE_LOAD ) {
            // 個別波形ロード
            waveArc = LoadWaveArcTable( waveArcInfo->fileId, heap, bSetAddr );
        }
        else {
            // 一括波形ロード
            waveArc = LoadWaveArc( waveArcInfo->fileId, heap, bSetAddr );
        }
        
        if ( waveArc == NULL ){
            return NNS_SND_ARC_LOAD_ERROR_FAILED_LOAD_WAVE;
        }
    }
    else
    {
        waveArc = (SNDWaveArc*)NNS_SndArcGetFileAddress( waveArcInfo->fileId );
    }
    
    if ( pData != NULL ) *pData = waveArc;
    
    return NNS_SND_ARC_LOAD_SUCESS;
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_SndArcLoadFile

  Description:  ファイルをロード

  Arguments:    fileId   - ファイルID
                callback - メモリ領域の破棄時に呼ばれるコールバック関数
                data1    - ユーザーデータ
                data2    - ユーザーデータ
                heap     - データを格納するヒープ

  Returns:      ロードしたメモリアドレス
                ロード失敗時には NULL を返す
 *---------------------------------------------------------------------------*/
void* NNSi_SndArcLoadFile(
    u32 fileId,
    NNSSndHeapDisposeCallback callback,
    u32 data1,
    u32 data2,
    NNSSndHeapHandle heap
)
{
    void* buffer;
    u32 len;
    
    len = NNS_SndArcGetFileSize( fileId );
    if ( len == 0 ) return NULL;
    
    if ( heap == NNS_SND_HEAP_INVALID_HANDLE ) return NULL;
    
    buffer = NNS_SndHeapAlloc( heap, len + RESERVED_AREASIZE , callback, data1, data2 ); // NOTE: reserved 32byte
    if ( buffer == NULL ) return NULL;
    
    if ( NNS_SndArcReadFile( fileId, buffer, (s32)len, 0 ) != len ) {
        return NULL;
    }
    
    DC_StoreRange( buffer, len );
    
    return buffer;
}

/******************************************************************************
    static function
 ******************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         LoadSeq

  Description:  シーケンスデータをロード

  Arguments:    fileId - ファイルID
                heap   - データを格納するヒープ
                bSetAddr - アドレスをNNS_SndArcSetFileAddressでセットするかどうか

  Returns:      ロードしたアドレス
 *---------------------------------------------------------------------------*/
static NNSSndSeqData* LoadSeq( u32 fileId, NNSSndHeapHandle heap, BOOL bSetAddr )
{
    void* buffer;
    
    buffer = NNS_SndArcGetFileAddress( fileId );
    if ( buffer == NULL )
    {
        buffer = NNSi_SndArcLoadFile(
            fileId,
            SeqDisposeCallback,
            bSetAddr ? (u32)NNS_SndArcGetCurrent() : 0,
            fileId,
            heap
        );
        
        if ( bSetAddr && buffer != NULL ) {
            NNS_SndArcSetFileAddress( fileId, buffer );
        }
    }
    
    return (NNSSndSeqData*)buffer;
}

/*---------------------------------------------------------------------------*
  Name:         LoadSeqArc

  Description:  シーケンスアーカイブをロード

  Arguments:    fileId - ファイルID
                heap   - データを格納するヒープ
                bSetAddr - アドレスをNNS_SndArcSetFileAddressでセットするかどうか

  Returns:      ロードしたアドレス
 *---------------------------------------------------------------------------*/
static NNSSndSeqArc* LoadSeqArc( u32 fileId, NNSSndHeapHandle heap, BOOL bSetAddr )
{
    void* buffer;
    
    buffer = NNS_SndArcGetFileAddress( fileId );
    if ( buffer == NULL )
    {
        buffer = NNSi_SndArcLoadFile(
            fileId,
            SeqDisposeCallback,
            bSetAddr ? (u32)NNS_SndArcGetCurrent() : 0,
            fileId,
            heap
        );
        
        if ( bSetAddr && buffer != NULL ) {
            NNS_SndArcSetFileAddress( fileId, buffer );            
        }
    }
    
    return (NNSSndSeqArc*)buffer;
}

/*---------------------------------------------------------------------------*
  Name:         LoadBank

  Description:  バンクデータをロード

  Arguments:    fileId - ファイルID
                heap   - データを格納するヒープ
                bSetAddr - アドレスをNNS_SndArcSetFileAddressでセットするかどうか

  Returns:      ロードしたアドレス
 *---------------------------------------------------------------------------*/
static SNDBankData* LoadBank( u32 fileId, NNSSndHeapHandle heap, BOOL bSetAddr )
{
    void* buffer;
    
    buffer = NNS_SndArcGetFileAddress( fileId );
    if ( buffer == NULL )
    {
        buffer = NNSi_SndArcLoadFile(
            fileId,
            BankDisposeCallback,
            bSetAddr ? (u32)NNS_SndArcGetCurrent() : 0,
            fileId,
            heap
        );
        
        if ( bSetAddr && buffer != NULL ) {
            NNS_SndArcSetFileAddress( fileId, buffer );            
        }
    }
    
    return (SNDBankData*)buffer;
}


/*---------------------------------------------------------------------------*
  Name:         LoadWaveArc

  Description:  波形アーカイブをロードします

  Arguments:    fileId - ファイルID
                heap   - データを格納するヒープ
                bSetAddr - アドレスをNNS_SndArcSetFileAddressでセットするかどうか

  Returns:      ロードしたアドレス
 *---------------------------------------------------------------------------*/
static SNDWaveArc* LoadWaveArc( u32 fileId, NNSSndHeapHandle heap, BOOL bSetAddr )
{
    void* buffer;
    
    buffer = NNS_SndArcGetFileAddress( fileId );
    if ( buffer == NULL )
    {
        buffer = NNSi_SndArcLoadFile(
            fileId,
            WaveArcDisposeCallback,
            bSetAddr ? (u32)NNS_SndArcGetCurrent() : 0,
            fileId,
            heap
        );
        
        if ( bSetAddr && buffer != NULL ) {
            NNS_SndArcSetFileAddress( fileId, buffer );            
        }
    }
    
    return (SNDWaveArc*)buffer;
}

/*---------------------------------------------------------------------------*
  Name:         LoadWaveArcTable

  Description:  波形アーカイブのテーブル部のみロード

  Arguments:    fileId - 波形アーカイブのファイルＩＤ
                heap - サウンドヒープハンドル
                bSetAddr - アドレスをNNS_SndArcSetFileAddressでセットするかどうか

  Returns:      ロードしたアドレス
 *---------------------------------------------------------------------------*/
static SNDWaveArc*  LoadWaveArcTable( u32 fileId, NNSSndHeapHandle heap, BOOL bSetAddr )
{
    static SNDWaveArc waveArcHeader;
    SNDWaveArc* waveArc;
    u32 len;
    u32 tableSize;
    s32 readSize;
    
    waveArc = (SNDWaveArc*)NNS_SndArcGetFileAddress( fileId );
    if ( waveArc == NULL )
    {
        // ヘッダロード
        readSize = NNS_SndArcReadFile( fileId, &waveArcHeader, sizeof(waveArcHeader), 0 );
        if ( readSize != sizeof(waveArcHeader) ) {
            // ロード失敗
            return NULL;
        }
        
        tableSize = sizeof(u32) * waveArcHeader.waveCount;
        len = sizeof(waveArcHeader) + tableSize * 2;
        
        if ( heap == NNS_SND_HEAP_INVALID_HANDLE ) {
            return NULL;
        }

        // メモリ確保
        waveArc = (SNDWaveArc*)NNS_SndHeapAlloc(
            heap,
            len + RESERVED_AREASIZE , // NOTE: reserved 32byte
            WaveArcTableDisposeCallback,
            bSetAddr ? (u32)NNS_SndArcGetCurrent() : 0,
            fileId
        );
        if ( waveArc == NULL ) {
            // メモリ不足
            return NULL;
        }
        
        // ヘッダ＆テーブルロード
        readSize = NNS_SndArcReadFile(
            fileId,
            waveArc,
            (s32)( sizeof(waveArcHeader) + tableSize ),
            0
        );
        if ( readSize != sizeof(waveArcHeader) + tableSize ) {
            // ロード失敗
            // 注意：確保したメモリを解放することはできない
            return NULL;
        }
        
        // テーブルコピー
        MI_CpuCopy8( waveArc->waveOffset, &waveArc->waveOffset[ waveArc->waveCount ], tableSize );
        
        // テーブルクリア
        MI_CpuClear8( waveArc->waveOffset, tableSize );

        //
        DC_StoreRange( waveArc, len );
        
        // ファイルアドレス登録
        if ( bSetAddr ) {
            NNS_SndArcSetFileAddress( fileId, waveArc );
        }
    }
    
    return waveArc;
}

/*---------------------------------------------------------------------------*
  Name:         DisposeCallback

  Description:  サウンドデータ破棄時に呼ばれるコールバックの共通処理

  Arguments:    mem    - メモリブロックのアドレス
                arc    - サウンドアーカイブ
                fileId - ファイルID

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void DisposeCallback( void* mem, NNSSndArc* arc, u32 fileId )
{
    NNSSndArc* oldArc;
    OSIntrMode old;
    
    if ( arc == NULL ) return;
    
    old = OS_DisableInterrupts();
    
    oldArc = NNS_SndArcSetCurrent( arc );
    
    if ( mem == NNS_SndArcGetFileAddress( fileId ) )
    {
        NNS_SndArcSetFileAddress( fileId, NULL );
    }
    
    (void)NNS_SndArcSetCurrent( oldArc );
    
    (void)OS_RestoreInterrupts( old );
}

/*---------------------------------------------------------------------------*
  Name:         SeqDisposeCallback

  Description:  シーケンスデータ破棄時に呼ばれるコールバック

  Arguments:    mem   - メモリブロックのアドレス
                size  - メモリブロックのサイズ
                data1 - ユーザーデータ（サウンドアーカイブのポインタ）
                data2 - ユーザーデータ（ファイルID）

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void SeqDisposeCallback( void* mem, u32 size, u32 data1, u32 data2 )
{
    NNSSndArc* arc = (NNSSndArc*)data1;
    u32 fileId = data2;
    
    DisposeCallback( mem, arc, fileId );
    SND_InvalidateSeqData( mem, (u8*)mem + size );
}

/*---------------------------------------------------------------------------*
  Name:         BankDisposeCallback

  Description:  バンクデータ破棄時に呼ばれるコールバック

  Arguments:    mem   - メモリブロックのアドレス
                size  - メモリブロックのサイズ
                data1 - ユーザーデータ（サウンドアーカイブのポインタ）
                data2 - ユーザーデータ（ファイルID）

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void BankDisposeCallback( void* mem, u32 size, u32 data1, u32 data2 )
{
    SNDBankData* bank = (SNDBankData*)mem;
    NNSSndArc* arc = (NNSSndArc*)data1;
    u32 fileId = data2;
    
    DisposeCallback( mem, arc, fileId );
    SND_InvalidateBankData( mem, (u8*)mem + size );
    
    SND_DestroyBank( bank );
}

/*---------------------------------------------------------------------------*
  Name:         WaveArcDisposeCallback

  Description:  波形アーカイブ破棄時に呼ばれるコールバック

  Arguments:    mem   - メモリブロックのアドレス
                size  - メモリブロックのサイズ
                data1 - ユーザーデータ（サウンドアーカイブのポインタ）
                data2 - ユーザーデータ（ファイルID）

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WaveArcDisposeCallback( void* mem, u32 size, u32 data1, u32 data2 )
{
    SNDWaveArc* waveArc = (SNDWaveArc*)mem;
    NNSSndArc* arc = (NNSSndArc*)data1;
    u32 fileId = data2;
    
    DisposeCallback( mem, arc, fileId );
    SND_InvalidateWaveData( mem, (u8*)mem + size );
    
    SND_DestroyWaveArc( waveArc );
}

/*---------------------------------------------------------------------------*
  Name:         WaveArcTableDisposeCallback

  Description:  波形アーカイブテーブル破棄時に呼ばれるコールバック

  Arguments:    mem   - メモリブロックのアドレス
                size  - メモリブロックのサイズ
                data1 - ユーザーデータ（サウンドアーカイブのポインタ）
                data2 - ユーザーデータ（ファイルID）

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WaveArcTableDisposeCallback( void* mem, u32 size, u32 data1, u32 data2 )
{
    SNDWaveArc* waveArc = (SNDWaveArc*)mem;
    NNSSndArc* arc = (NNSSndArc*)data1;
    u32 fileId = data2;
       
    (void)size;

    DisposeCallback( mem, arc, fileId );
    
    SND_DestroyWaveArc( waveArc );
}

/*---------------------------------------------------------------------------*
  Name:         SingleWaveisposeCallback

  Description:  単独波形データ破棄時に呼ばれるコールバック

  Arguments:    mem   - メモリブロックのアドレス
                size  - メモリブロックのサイズ
                data1 - ユーザーデータ（波形アーカイブのポインタ）
                data2 - ユーザーデータ（波形インデックス）

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void SingleWaveDisposeCallback( void* mem, u32 size, u32 data1, u32 data2 )
{
    SNDWaveArc* waveArc = (SNDWaveArc*)data1;
    u32 waveNo = data2;
    
    if ( mem == SND_GetWaveDataAddress( waveArc, (int)waveNo ) ) {
        SND_SetWaveDataAddress( waveArc, (int)waveNo, NULL );
    }
    
    SND_InvalidateWaveData( mem, (u8*)mem + size );
}

/*---------------------------------------------------------------------------*
  Name:         LoadSingleWave

  Description:  波形アーカイブ中の波形をロード

  Arguments:    waveArc - 波形アーカイブ
                waveNo  - 波形インデックス番号
                fileId - 波形アーカイブのファイルＩＤ
                heap - サウンドヒープハンドル

  Returns:      成功したかどうか
 *---------------------------------------------------------------------------*/
static BOOL LoadSingleWave( SNDWaveArc* waveArc, int waveNo, u32 fileId, NNSSndHeapHandle heap )
{
    SNDWaveData* buffer;
    u32 len;
    u32 begin;
    u32 end;
    u32 waveCount;
    
    NNS_MINMAX_ASSERT( waveNo, 0, SND_GetWaveDataCount( waveArc )-1 );
    
    if ( SND_GetWaveDataAddress( waveArc, waveNo ) != NULL ) {
        // ロード済み
        return TRUE;
    }
    
    waveCount = SND_GetWaveDataCount( waveArc );
    
    begin = waveArc->waveOffset[ waveArc->waveCount + waveNo ];
    if ( waveNo < waveCount-1 ) {
        end = waveArc->waveOffset[ waveArc->waveCount + waveNo + 1 ];
    }
    else {
        end = waveArc->fileHeader.fileSize;
    }
    len = end - begin;
    
    if ( heap == NNS_SND_HEAP_INVALID_HANDLE ) {
        return FALSE;
    }
    
    buffer = (SNDWaveData*)NNS_SndHeapAlloc(
        heap,
        len + RESERVED_AREASIZE,
        SingleWaveDisposeCallback,
        (u32)waveArc,
        (u32)waveNo
    );
    if ( buffer == NULL ) return FALSE;
    
    if ( NNS_SndArcReadFile( fileId, buffer, (s32)len, (s32)begin ) != len ) {
        return FALSE;
    }
    
    DC_StoreRange( buffer, len );
    
    SND_SetWaveDataAddress( waveArc, waveNo, buffer );
    
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         LoadSingleWaves

  Description:  バンクに必要な波形アーカイブ中の波形をロード

  Arguments:    waveArc - 波形アーカイブ
                bank - バンク
                waveArcNo - バンク中での波形アーカイブ番号
                fileId - 波形アーカイブのファイルＩＤ
                heap - サウンドヒープハンドル

  Returns:      成功したかどうか
 *---------------------------------------------------------------------------*/
BOOL LoadSingleWaves( SNDWaveArc* waveArc, const SNDBankData* bank, int waveArcNo, u32 fileId, NNSSndHeapHandle heap )
{
    SNDInstPos pos = SND_GetFirstInstDataPos( bank );
    SNDInstData inst;
    
    NNS_NULL_ASSERT( waveArc );
    
    if ( bank == NULL ) {
        NNS_WARNING( bank != NULL, "Cannot load single wave data because the bank data is not on Memory." );
        return FALSE;
    }
    
    while( SND_GetNextInstData( bank, &inst, &pos ) ) {
        if ( inst.type == SND_INST_PCM && waveArcNo == inst.param.wave[1]  ) {
            if ( ! LoadSingleWave( waveArc, inst.param.wave[0], fileId, heap ) ) {
                return FALSE;
            }
        }
    }
    
    return TRUE;
}

/*====== End of heap.c ======*/

