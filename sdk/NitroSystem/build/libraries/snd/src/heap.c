/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - snd
  File:     heap.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.24 $
 *---------------------------------------------------------------------------*/
#include <nnsys/snd/heap.h>

#include <nnsys/misc.h>
#include <nnsys/fnd.h>

/******************************************************************************
	macro definition
 ******************************************************************************/

#define HEAP_ALIGN 32

#define ROUNDUP( value, align ) ( ( (u32)(value) + ( (align) - 1 ) ) & ~( (align) - 1 ) )

/******************************************************************************
	structure definition
 ******************************************************************************/

typedef struct NNSSndHeap
{
    NNSFndHeapHandle handle;
    NNSFndList sectionList;
} NNSSndHeap;

typedef struct NNSSndHeapBlock
{
    NNSFndLink link;
    u32 size;
    NNSSndHeapDisposeCallback callback;
    u32 data1;
    u32 data2;
    u8 padding[ 0x20 - ( ( sizeof( NNSFndLink ) + sizeof( NNSSndHeapDisposeCallback ) + sizeof( u32 ) * 3 ) & 0x1f ) ];
    u32 buffer[ 0 ];
} NNSSndHeapBlock; // NOTE: must be 32 byte boundary

typedef struct NNSSndHeapSection
{
    NNSFndList blockList;
    NNSFndLink link;
} NNSSndHeapSection;

/******************************************************************************
	static function declarations
 ******************************************************************************/

static void InitHeapSection( NNSSndHeapSection* section );
static BOOL InitHeap( NNSSndHeap* heap, NNSFndHeapHandle handle );
static BOOL NewSection( NNSSndHeap* heap );
static void EraseSync( void );

/******************************************************************************
	public functions
 ******************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         NNS_SndHeapCreate

  Description:  ヒープを作成

  Arguments:    startAddress - 開始アドレス
                size         - メモリサイズ

  Returns:      ヒープハンドル
 *---------------------------------------------------------------------------*/
NNSSndHeapHandle NNS_SndHeapCreate( void* startAddress, u32 size )
{
    NNSSndHeap* heap;
    void* endAddress;
    NNSFndHeapHandle handle;
    
    NNS_NULL_ASSERT( startAddress );
    
    endAddress   = (u8*)startAddress + size;
    startAddress = (void*)ROUNDUP( startAddress, 4 ); // NNSSndHeap align
    
    if ( startAddress > endAddress ) return NNS_SND_HEAP_INVALID_HANDLE;
    
    size = (u32)( (u8*)endAddress - (u8*)startAddress );
    if ( size < sizeof( NNSSndHeap ) ) {
        return NNS_SND_HEAP_INVALID_HANDLE;
    }
    
    size -= sizeof( NNSSndHeap );
    
    heap = (NNSSndHeap*)startAddress;
    startAddress = heap + 1;
    
    handle = NNS_FndCreateFrmHeap( startAddress, size );
    if ( handle == NNS_FND_HEAP_INVALID_HANDLE ) {
        return NULL;
    }
    
    if ( ! InitHeap( heap, handle ) ) {
        NNS_FndDestroyFrmHeap( handle );
        return NNS_SND_HEAP_INVALID_HANDLE;
    }
    
    return heap;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndHeapDestroy

  Description:  ヒープを破棄します

  Arguments:    heap - サウンドヒープ

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndHeapDestroy( NNSSndHeapHandle heap )
{
    NNS_ASSERT( heap != NNS_SND_HEAP_INVALID_HANDLE );
    
    NNS_SndHeapClear( heap );
    
    NNS_FndDestroyFrmHeap( heap->handle );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndHeapClear

  Description:  ヒープを作成時の状態に戻す

  Arguments:    heap - .サウンドヒープ

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndHeapClear( NNSSndHeapHandle heap )
{
    NNSSndHeapSection* section=NULL;
    void* object;
    BOOL result;
    BOOL doCallback = FALSE;
    
    NNS_ASSERT( heap != NNS_SND_HEAP_INVALID_HANDLE );
    
    // セクションの破棄
    while ( ( section = (NNSSndHeapSection*)NNS_FndGetPrevListObject( & heap->sectionList, NULL ) ) != NULL )
    {
        // コールバックの呼び出し
        object = NULL;
        while ( ( object = NNS_FndGetPrevListObject( & section->blockList, object ) ) != NULL )
        {
            NNSSndHeapBlock* block = (NNSSndHeapBlock*)object;
            if ( block->callback != NULL ) {
                block->callback( block->buffer, block->size, block->data1, block->data2 );
                doCallback = TRUE;
            }
        }
        
        // セクションリストからの削除
        NNS_FndRemoveListObject( & heap->sectionList, section );
    }
    
    // ヒープのクリア
    NNS_FndFreeToFrmHeap( heap->handle, NNS_FND_FRMHEAP_FREE_ALL );
    
    // サウンドデータの使用停止の同期
    if ( doCallback ) EraseSync();
    
    // ベースセクションの作成
    result = NewSection( heap );
    NNS_ASSERTMSG( result, "NNS_SndHeapClear(): NewSection is Failed");
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndHeapAlloc

  Description:  ヒープからメモリを確保

  Arguments:    heap     - サウンドヒープ
                size     - メモリサイズ
                callback - メモリが破棄されたときに呼びだされるコールバック関数
                data1    - コールバックデータ1
                data2    - コールバックデータ2

  Returns:      確保したメモリのポインタ
 *---------------------------------------------------------------------------*/
void* NNS_SndHeapAlloc( NNSSndHeapHandle heap, u32 size, NNSSndHeapDisposeCallback callback, u32 data1, u32 data2 )
{
    NNSSndHeapSection* section;
    NNSSndHeapBlock* block;
    
    NNS_ASSERT( heap != NNS_SND_HEAP_INVALID_HANDLE );
    
    block = (NNSSndHeapBlock*)NNS_FndAllocFromFrmHeapEx(
        heap->handle, sizeof( NNSSndHeapBlock ) + ROUNDUP( size, HEAP_ALIGN ), HEAP_ALIGN );
    if ( block == NULL ) return NULL;
    
    section = (NNSSndHeapSection*)NNS_FndGetPrevListObject( & heap->sectionList, NULL );
    
    block->size = size;
    block->callback = callback;
    block->data1 = data1;
    block->data2 = data2;
    NNS_FndAppendListObject( & section->blockList, block );
    
    NNS_ASSERTMSG( ( (u32)( block->buffer ) & 0x1f ) == 0, "NNS_SndHeapAlloc: Internal Error" );
    
    return block->buffer;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndHeapSaveState

  Description:  ヒープの状態を保存

  Arguments:    heap - サウンドヒープ.

  Returns:      保存した階層レベルを返す
                失敗時には、-1
 *---------------------------------------------------------------------------*/
int NNS_SndHeapSaveState( NNSSndHeapHandle heap )
{
    BOOL result;
    
    NNS_ASSERT( heap != NNS_SND_HEAP_INVALID_HANDLE );
    
    if ( ! NNS_FndRecordStateForFrmHeap( heap->handle, heap->sectionList.numObjects ) ) {
        return -1;
    }
    
    if ( ! NewSection( heap ) ) {
        result = NNS_FndFreeByStateToFrmHeap( heap->handle, 0 );
        NNS_ASSERTMSG( result, "NNS_SndHeapSaveState(): NNS_FndFreeByStateToFrmHeap is Failed");
        return -1;
    }
    
    return heap->sectionList.numObjects - 1;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndHeapLoadState

  Description:  ヒープの状態を戻す

  Arguments:    heap  - サウンドヒープ
                level - 階層レベル

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndHeapLoadState( NNSSndHeapHandle heap, int level )
{
    NNSSndHeapSection* section;
    void* object = NULL;
    BOOL result;
    BOOL doCallback = FALSE;
    
    NNS_ASSERT( heap != NNS_SND_HEAP_INVALID_HANDLE );
    NNS_MINMAX_ASSERT( level, 0, heap->sectionList.numObjects-1 );
    
    if ( level == 0 ) {
        NNS_SndHeapClear( heap );
        return;
    }
    while( level < heap->sectionList.numObjects )
    {
        // get latest section
        section = (NNSSndHeapSection*)NNS_FndGetPrevListObject( & heap->sectionList, NULL );
        
        // call dispose callback
        while ( ( object = NNS_FndGetPrevListObject( & section->blockList, object ) ) != NULL )
        {
            NNSSndHeapBlock* block = (NNSSndHeapBlock*)object;
            if ( block->callback != NULL ) {
                block->callback( block->buffer, block->size, block->data1, block->data2 );
                doCallback = TRUE;
            }
        }
        
        // セクションリストからの削除
        NNS_FndRemoveListObject( & heap->sectionList, section );
    }
    
    // ヒープ状態を復元
    result = NNS_FndFreeByStateToFrmHeap( heap->handle, (u32)level );
    NNS_ASSERTMSG( result, "NNS_SndHeapLoadState(): NNS_FndFreeByStateToFrmHeap is Failed");   

    // サウンドデータの使用停止の同期
    if ( doCallback ) EraseSync();
    
    // 再度記録
    result =  NNS_FndRecordStateForFrmHeap( heap->handle, heap->sectionList.numObjects );
    NNS_ASSERTMSG( result, "NNS_SndHeapLoadState(): NNS_FndRecordStateForFrmHeap is Failed");
    
    // セクションの作成
    result = NewSection( heap );
    NNS_ASSERTMSG( result, "NNS_SndHeapLoadState(): NewSection is Failed");
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndHeapGetCurrentLevel

  Description:  ヒープの現在の階層レベルを取得

  Arguments:    heap - サウンドヒープ

  Returns:      現在の階層レベル
 *---------------------------------------------------------------------------*/
int NNS_SndHeapGetCurrentLevel( NNSSndHeapHandle heap )
{
    NNS_ASSERT( heap != NNS_SND_HEAP_INVALID_HANDLE );
    
    return heap->sectionList.numObjects - 1;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndHeapGetSize

  Description:  ヒープの容量を取得

  Arguments:    heap - サウンドヒープ

  Returns:      ヒープの容量
 *---------------------------------------------------------------------------*/
u32 NNS_SndHeapGetSize( NNSSndHeapHandle heap )
{
    NNS_ASSERT( heap != NNS_SND_HEAP_INVALID_HANDLE );
    
    return
        (u32)( (u8*)NNS_FndGetHeapEndAddress( heap->handle ) -
               (u8*)NNS_FndGetHeapStartAddress( heap->handle ) )
        ;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndHeapGetFreeSize

  Description:  ヒープの空き容量を取得

  Arguments:    heap - サウンドヒープ

  Returns:      空き容量
 *---------------------------------------------------------------------------*/
u32 NNS_SndHeapGetFreeSize( NNSSndHeapHandle heap )
{
    u32 size;
    
    NNS_ASSERT( heap != NNS_SND_HEAP_INVALID_HANDLE );
    
    size = NNS_FndGetAllocatableSizeForFrmHeapEx( heap->handle, HEAP_ALIGN );
    
    if ( size < sizeof( NNSSndHeapBlock ) ) return 0;
    size -= sizeof( NNSSndHeapBlock );
    
    size &= ~0x1f;
    
    return size;
}

/******************************************************************************
	static functions
 ******************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         InitHeapSection

  Description:  NNSSndHeapSection 構造体を初期化

  Arguments:    section - ヒープセクション

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void InitHeapSection( NNSSndHeapSection* section )
{
    NNS_FND_INIT_LIST( & section->blockList, NNSSndHeapBlock, link );    
}

/*---------------------------------------------------------------------------*
  Name:         InitHeap

  Description:  NNSSndHeap 構造体を初期化

  Arguments:    heap   - サウンドヒープ
                handle - フレームヒープハンドル

  Returns:      成功したかどうか
 *---------------------------------------------------------------------------*/
static BOOL InitHeap( NNSSndHeap* heap, NNSFndHeapHandle handle )
{
    NNS_FND_INIT_LIST( & heap->sectionList, NNSSndHeapSection, link );
    heap->handle = handle;
    
    // ベースセクションの作成
    if ( ! NewSection( heap ) ) {
        return FALSE;
    }
    
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         NewSection

  Description:  新しいセクションを作成

  Arguments:    heap - サウンドヒープ

  Returns:      成功したかどうか
 *---------------------------------------------------------------------------*/
static BOOL NewSection( NNSSndHeap* heap )
{
    NNSSndHeapSection* section;
    
    // new NNSSndHeapSection
    section = (NNSSndHeapSection*)NNS_FndAllocFromFrmHeap( heap->handle, sizeof( NNSSndHeapSection ) );
    if ( section == NULL ) {
        return FALSE;
    }
    InitHeapSection( section );
    
    NNS_FndAppendListObject( & heap->sectionList, section );
    
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         EraseSync

  Description:  ヒープの削除時に、データの使用を完全に停止するための同期

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void EraseSync( void )
{
    u32 commandTag;
    
    commandTag = SND_GetCurrentCommandTag();
    (void)SND_FlushCommand( SND_COMMAND_BLOCK );
    SND_WaitForCommandProc( commandTag );
}

/*====== End of heap.c ======*/

