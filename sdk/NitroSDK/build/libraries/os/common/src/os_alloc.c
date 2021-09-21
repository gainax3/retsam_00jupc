/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS
  File:     os_alloc.c

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: os_alloc.c,v $
  Revision 1.30  2007/02/20 00:28:10  kitase_hirotake
  indent source

  Revision 1.29  2006/03/27 05:35:35  kitase_hirotake
  OS_CheckHeap関数でOS_CURRENT_HEAP_HANDLEが通るよう修正

  Revision 1.28  2006/01/18 02:11:30  kitase_hirotake
  do-indent

  Revision 1.27  2005/10/07 06:23:25  yada
  add OS_GetTotalOccupiedSize()

  Revision 1.26  2005/02/28 05:26:27  yosizaki
  do-indent.

  Revision 1.25  2005/02/18 08:51:35  yasu
  著作年度変更

  Revision 1.24  2005/02/18 06:19:32  yasu
  OS_DisableInterrupts 返値の型修正

  Revision 1.23  2004/11/08 00:49:29  yada
  fix assert

  Revision 1.22  2004/11/05 12:34:04  yada
  OS_GetAllocatedSize() -> OS_GetTotalAllocSize()

  Revision 1.21  2004/11/05 10:35:23  yada
  add OS_FreeAllToHeap

  Revision 1.20  2004/11/05 06:31:34  yada
  Change type of OS_CheckHeap(): long -> s32 .
  Add OS_GetAllocatedSize(), OS_GetTotalFreeSize(),
  OS_GetMaxFreeSize(), OS_ClearHeap()

  Revision 1.19  2004/10/25 10:59:56  yasu
  Support SDK_NO_MESSAGE

  Revision 1.18  2004/09/02 06:18:58  yada
  fix bug that OS_CheckHeap() don't restore interrupt status

  Revision 1.17  2004/08/31 01:56:59  yada
  fix ASSERT message

  Revision 1.16  2004/08/31 01:06:36  yada
  add OS_ClearAlloc

  Revision 1.15  2004/07/19 01:43:26  yada
  only fix comment

  Revision 1.14  2004/07/02 00:04:05  yada
  fix a small bug

  Revision 1.13  2004/07/01 09:58:23  yada
  let functions in os_alloc.c be thread safe

  Revision 1.12  2004/06/08 00:24:01  yada
  add '#pragma unused()' for FINALROM

  Revision 1.11  2004/03/08 11:51:48  yada
  InitAlloc していないヒープからallocしようとしたときのチェックを追加

  Revision 1.10  2004/02/13 10:38:26  yada
  ARENAを６個から９個に

  Revision 1.9  2004/02/12 11:08:30  yada
  ARM9/ARM7 振り分け作業

  Revision 1.8  2004/02/05 07:09:02  yasu
  change SDK prefix iris -> nitro

  Revision 1.7  2004/01/18 01:24:32  yada
  HeapInfo の初期値個数修正

  Revision 1.6  2004/01/07 08:20:53  yada
  OS_DestroyHeap() した際に、free/alloc リストポインタをクリアするようにした

  Revision 1.5  2004/01/07 02:51:44  yada
  OS_VisitAllocated で、関数呼び出し時のサイズをヘッダを除いた値に変更

  Revision 1.4  2004/01/07 00:26:10  yada
  修正

  Revision 1.3  2004/01/07 00:21:13  yada
  allocとfreeで、heap handle が負の時カレント指定とした

  Revision 1.2  2004/01/05 08:23:15  yada
  アリーナごとにヒープ情報を持つように変更した

  Revision 1.1  2004/01/05 04:29:38  yada
  初版


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro/os.h>
#include <nitro/os/common/alloc.h>

/*---------------------------------------------------------------------------*
    Error messages of OSAlloc.c
 *---------------------------------------------------------------------------*/
#define OS_ERR_ALLOCFROMHEAP_NOHEAP         "OS_AllocFromHeap(): heap is not initialized."
#define OS_ERR_ALLOCFROMHEAP_INVSIZE        "OS_AllocFromHeap(): invalid size."
#define OS_ERR_ALLOCFROMHEAP_INVHEAP        "OS_AllocFromHeap(): invalid heap handle."
#define OS_ERR_ALLOCFROMHEAP_BROKENHEAP     "OS_AllocFromHeap(): heap is broken."
#define OS_ERR_ALLOCFIXED_NOHEAP            "OS_AllocFixed(): heap is not initialized."
#define OS_ERR_ALLOCFIXED_INVRANGE          "OS_AllocFixed(): invalid range."
#define OS_ERR_FREETOHEAP_NOHEAP            "OS_FreeToHeap(): heap is not initialized."
#define OS_ERR_FREETOHEAP_INVPTR            "OS_FreeToHeap(): invalid pointer."
#define OS_ERR_FREETOHEAP_INVHEAP           "OS_FreeToHeap(): invalid heap handle."
#define OS_ERR_SETCURRENTHEAP_NOHEAP        "OS_SetCurrentHeap(): heap is not initialized."
#define OS_ERR_SETCURRENTHEAP_INVHEAP       "OS_SetCurrentHeap(): invalid heap handle."
#define OS_ERR_INITALLOC_INVNUMHEAPS        "OS_InitAlloc(): invalid number of heaps."
#define OS_ERR_INITALLOC_INVRANGE           "OS_InitAlloc(): invalid range."
#define OS_ERR_INITALLOC_INSRANGE           "OS_InitAlloc(): too small range."
#define OS_ERR_CLEARALLOC_INVID             "OS_ClearAlloc(): invalid arena id."
#define OS_ERR_CREATEHEAP_NOHEAP            "OS_CreateHeap(): heap is not initialized."
#define OS_ERR_CREATEHEAP_INVRANGE          "OS_CreateHeap(): invalid range."
#define OS_ERR_CREATEHEAP_INSRANGE          "OS_CreateHeap(): too small range."
#define OS_ERR_DESTROYHEAP_NOHEAP           "OS_DestroyHeap(): heap is not initialized."
#define OS_ERR_DESTROYHEAP_INVHEAP          "OS_DestroyHeap(): invalid heap handle."
#define OS_ERR_ADDTOHEAP_NOHEAP             "OS_AddToHeap(): heap is not initialized."
#define OS_ERR_ADDTOHEAP_INVHEAP            "OS_AddToHeap(): invalid heap handle."
#define OS_ERR_ADDTOHEAP_INVRANGE           "OS_AddToHeap(): invalid range."
#define OS_ERR_ADDTOHEAP_INSRANGE           "OS_AddToHeap(): too small range."
#define OS_ERR_REFERENT_NOHEAP              "OS_ReferentSize(): heap is not initialized."
#define OS_ERR_REFERENT_INVPTR              "OS_ReferentSize(): invalid pointer."
#define OS_ERR_DUMPHEAP_NOHEAP              "OS_DumpHeap(): heap is not initialized."
#define OS_ERR_DUMPHEAP_INVHEAP             "OS_DumpHeap(): invalid heap handle."
#define OS_ERR_DUMPHEAP_BROKENHEAP          "OS_DumpHeap(): heap is broken."
#define OS_ERR_ALLOCFROMHEAP_INVID          "OS_AllocFromHeap(): illegal arena id."
#define OS_ERR_ALLOCFROMHEAP_NOINFO         "OS_AllocFromHeap(): heap not initialized."
#define OS_ERR_ALLOCFIXED_INVID             "OS_AllocFixed(): illegal arena id."
#define OS_ERR_ALLOCFIXED_NOINFO            "OS_AllocFixed(): heap not initialized."
#define OS_ERR_FREETOHEAP_INVID             "OS_FreeToHeap(): illegal arena id."
#define OS_ERR_FREETOHEAP_NOINFO            "OS_FreeToHeap(): heap not initialized."
#define OS_ERR_SETCURRENTHEAP_INVID         "OS_SetCurrentHeap(): illegal arena id."
#define OS_ERR_SETCURRENTHEAP_NOINFO        "OS_SetCurrentHeap(): heap not initialized."
#define OS_ERR_INITALLOC_INVID              "OS_SetInitAlloc(): illegal arena id."
#define OS_ERR_INITALLOC_INVINFO            "OS_SetInitAlloc(): heap already initialized."
#define OS_ERR_CREATEHEAP_INVID             "OS_CreateHeap(): illegal arena id."
#define OS_ERR_CREATEHEAP_NOINFO            "OS_CreateHeap(): heap not initialized."
#define OS_ERR_DESTROYHEAP_INVID            "OS_DestroyHeap(): illegal arena id."
#define OS_ERR_DESTROYHEAP_NOINFO           "OS_DestroyHeap(): heap not initialized."
#define OS_ERR_ADDTOHEAP_INVID              "OS_AddToHeap(): illegal arena id."
#define OS_ERR_ADDTOHEAP_NOINFO             "OS_AddToHeap(): heap not initialized."
#define OS_ERR_CHECKHEAP_INVID              "OS_CheckHeap(): illegal arena id."
#define OS_ERR_CHECKHEAP_NOINFO             "OS_CheckHeap(): heap not initialized."
#define OS_ERR_REFERENTSIZE_INVID           "OS_ReferentSize(): illegal arena id."
#define OS_ERR_REFERENTSIZE_NOINFO          "OS_ReferrentSize(): heap not initialized."
#define OS_ERR_DUMPHEAP_INVID               "OS_DumpHeap(): illegal arena id."
#define OS_ERR_DUMPHEAP_NOINFO              "OS_DumpHeap(): heap not initialized."
#define OS_ERR_VISITALLOCATED_INVID         "OS_VisitAllocated(): illegal arena id."
#define OS_ERR_VISITALLOCATED_NOINFO        "OS_VisitAllocated(): heap not initialized."


#define OFFSET(n, a)    (((u32) (n)) & ((a) - 1))
#define TRUNC(n, a)     (((u32) (n)) & ~((a) - 1))
#define ROUND(n, a)     (((u32) (n) + (a) - 1) & ~((a) - 1))

#define ALIGNMENT       32             // alignment in bytes
#define MINOBJSIZE      (HEADERSIZE + ALIGNMENT)        // smallest object
#define HEADERSIZE      ROUND(sizeof(Cell), ALIGNMENT)

//---- InRange():       True if a <= targ < b
#define InRange(targ, a, b)                                             \
    ((u32)(a) <= (u32)(targ) && (u32)(targ) < (u32)(b))

//---- RangeOverlap():  True if the ranges a and b overlap in any way.
#define RangeOverlap(aStart, aEnd, bStart, bEnd)                        \
    ((u32)(bStart) <= (u32)(aStart) && (u32)(aStart) < (u32)(bEnd) ||   \
     (u32)(bStart) < (u32)(aEnd) && (u32)(aEnd) <= (u32)(bEnd))

//---- RangeSubset():   True if range a is a subset of range b
//                  Assume (aStart < aEnd) and (bStart < bEnd)
#define RangeSubset(aStart, aEnd, bStart, bEnd)                         \
    ((u32)(bStart) <= (u32)(aStart) && (u32)(aEnd) <= (u32)(bEnd))

typedef struct Cell Cell;
typedef struct HeapDesc HeapDesc;

// Cell: header of object which resides HEADERSIZE bytes before payload.
//       doubly linked list are needed because of non-contiguous heaps
struct Cell
{
    Cell   *prev;
    Cell   *next;
    long    size;                      // size of object plus HEADERSIZE

#ifdef  SDK_DEBUG
    HeapDesc *hd;                      // from which the block is allocated
    // (NULL in free list).
    long    requested;                 // size of object to have been requested
#endif                                 // SDK_DEBUG
};

struct HeapDesc
{
    long    size;                      // if -1 then heap is free. Note OS_AllocFixed()
    // could make a heap empty.
    Cell   *free;                      // pointer to the first free cell
    Cell   *allocated;                 // pointer to the first used cell

#ifdef  SDK_DEBUG
    u32     paddingBytes;
    u32     headerBytes;
    u32     payloadBytes;
#endif                                 // SDK_DEBUG
};


//----------------
//---- struct of Heap infomation
typedef struct
{
    // volatile because some functions use this as hidden macro parameter
    volatile OSHeapHandle currentHeap;
    int     numHeaps;
    void   *arenaStart;
    void   *arenaEnd;
    HeapDesc *heapArray;
}
OSHeapInfo;

void   *OSiHeapInfo[OS_ARENA_MAX] = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};                                     // because OS_ARENA_MAX = 9

//================================================================================
/*---------------------------------------------------------------------------*
  Name:         DLAddFront

  Description:  Inserts /cell/ into the head of the list pointed to by /list/

  Arguments:    list : pointer to the first cell in the list
                cell : pointer to a cell to be inserted

  Returns:      a pointer to the new first cell
 *---------------------------------------------------------------------------*/
static Cell *DLAddFront(Cell * list, Cell * cell)
{
    cell->next = list;
    cell->prev = NULL;
    if (list)
    {
        list->prev = cell;
    }
    return cell;
}

/*---------------------------------------------------------------------------*
  Name:         DLLookup

  Description:  Returns /cell/ if /cell/ is found in /list/.

  Arguments:    list : pointer to the first cell in the list
                cell : pointer to a cell to look for

  Returns:      /cell/ if /cell/ is in /list/. Otherwise, NULL.
 *---------------------------------------------------------------------------*/
static Cell *DLLookup(Cell * list, Cell * cell)
{
    for (; list; list = list->next)
    {
        if (list == cell)
        {
            return list;
        }
    }
    return NULL;
}

/*---------------------------------------------------------------------------*
  Name:         DLExtract

  Description:  Extracts /target/ from the list pointed to by /list/.
                If /target/ is at the head of the list, /list/ will be changed
                to reflect its removal.

  Arguments:    list : pointer to the first cell in the list
                cell : pointer to a cell to remove

  Returns:      a pointer to the new first cell
 *---------------------------------------------------------------------------*/
static Cell *DLExtract(Cell * list, Cell * cell)
{
    if (cell->next)
    {
        cell->next->prev = cell->prev;
    }

    if (cell->prev == NULL)
    {
        return cell->next;
    }
    else
    {
        cell->prev->next = cell->next;
        return list;
    }
}

/*---------------------------------------------------------------------------*
  Name:         DLInsert

  Description:  Inserts /cell/ into the list pointed to by /list/ in sorted
                order by address.  Also attempts to coalesce forward and
                backward blocks if possible.

  Arguments:    list : pointer to the first cell in the list
                cell : pointer to a cell to be inserted

  Returns:      a pointer to the new first cell
 *---------------------------------------------------------------------------*/
static Cell *DLInsert(Cell * list, Cell * cell)
{
    Cell   *prev;
    Cell   *next;

    for (next = list, prev = NULL; next; prev = next, next = next->next)
    {
        if (cell <= next)
        {
            break;
        }
    }

    cell->next = next;
    cell->prev = prev;
    if (next)
    {
        next->prev = cell;
        if ((char *)cell + cell->size == (char *)next)
        {
            //---- Coalesce forward
            cell->size += next->size;
            cell->next = next = next->next;
            if (next)
            {
                next->prev = cell;
            }
        }
    }
    if (prev)
    {
        prev->next = cell;
        if ((char *)prev + prev->size == (char *)cell)
        {
            //---- Coalesce back
            prev->size += cell->size;
            prev->next = next;
            if (next)
            {
                next->prev = prev;
            }
        }
        return list;
    }
    else
    {
        return cell;                   // cell becomes new head of list
    }
}

/*---------------------------------------------------------------------------*
  Name:         DLOverlap

  Description:  returns true if the range delimited by /start/ and /end/
                overlaps with any element of /list/.

  Arguments:    list  : pointer to the first cell in the list
                start : start of range
                end   : end of range

  Returns:      TRUE if /start/-/end/ overlaps with any element of /list/
 *---------------------------------------------------------------------------*/
static BOOL DLOverlap(Cell * list, void *start, void *end)
{
    Cell   *cell;

    for (cell = list; cell; cell = cell->next)
    {
        if (RangeOverlap(cell, (char *)cell + cell->size, start, end))
        {
            return TRUE;
        }
    }
    return FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         DLSize

  Description:  returns total number of bytes used by every element of /list/.

  Arguments:    list : pointer to the first cell in the list

  Returns:      total number of bytes used by every cell of /list/
 *---------------------------------------------------------------------------*/
static long DLSize(Cell * list)
{
    Cell   *cell;
    long    size = 0;

    for (cell = list; cell; cell = cell->next)
    {
        size += cell->size;
    }
    return size;
}

//================================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_AllocFromHeap

  Description:  Allocates /size/ bytes from /heap/. Some additional memory
                will also be consumed from /heap/.

  Arguments:    id   : arena ID
                heap : handle to a heap that was returned from OS_CreateHeap()
                size : size of object to be allocated

  Returns:      a null pointer or a pointer to the allocated space aligned
                with ALIGNMENT bytes boundaries
 *---------------------------------------------------------------------------*/
void   *OS_AllocFromHeap(OSArenaId id, OSHeapHandle heap, u32 size)
{
    OSHeapInfo *heapInfo;
    HeapDesc *hd;
    Cell   *cell;                      // candidate block
    Cell   *newCell;                   // ptr to leftover block
    long    leftoverSize;              // size of any leftover
    OSIntrMode enabled = OS_DisableInterrupts();

    //OS_Printf( "id=%d heap=%x size=%x\n",id, heap, size );

#ifdef  SDK_DEBUG
    long    requested = (long)size;
#endif // SDK_DEBUG

    //---- check arena id
    SDK_ASSERTMSG(id < OS_ARENA_MAX, OS_ERR_ALLOCFROMHEAP_INVID);
    SDK_ASSERTMSG(OSiHeapInfo[id], OS_ERR_ALLOCFROMHEAP_NOINFO);
    heapInfo = OSiHeapInfo[id];

    //---- check exist heap and size>0
    if (!heapInfo)
    {
        (void)OS_RestoreInterrupts(enabled);
        return NULL;
    }

    //---- heap<0  means current heap
    if (heap < 0)
    {
        heap = heapInfo->currentHeap;
    }

    SDK_ASSERTMSG(heapInfo->heapArray, OS_ERR_ALLOCFROMHEAP_NOHEAP);
    SDK_ASSERTMSG(0 < ((long)size), OS_ERR_ALLOCFROMHEAP_INVSIZE);
    SDK_ASSERTMSG(0 <= heap && heap < heapInfo->numHeaps, OS_ERR_ALLOCFROMHEAP_INVHEAP);
    SDK_ASSERTMSG(0 <= heapInfo->heapArray[heap].size, OS_ERR_ALLOCFROMHEAP_INVHEAP);

    hd = &heapInfo->heapArray[heap];

    // Enlarge size to smallest possible cell size
    size += HEADERSIZE;
    size = ROUND(size, ALIGNMENT);

    // Search for block large enough
    for (cell = hd->free; cell != NULL; cell = cell->next)
    {
        if ((long)size <= cell->size)
        {
            break;
        }
    }

    if (cell == NULL)
    {
#ifdef  SDK_DEBUG
        OS_Printf("OS_AllocFromHeap: Warning- failed to allocate %d bytes\n", size);
#endif // SDK_DEBUG
        (void)OS_RestoreInterrupts(enabled);
        return NULL;
    }

    SDK_ASSERTMSG(OFFSET(cell, ALIGNMENT) == 0, OS_ERR_ALLOCFROMHEAP_BROKENHEAP);
    SDK_ASSERTMSG(cell->hd == NULL, OS_ERR_ALLOCFROMHEAP_BROKENHEAP);

    leftoverSize = cell->size - (long)size;
    if (leftoverSize < MINOBJSIZE)
    {
        //---- Just extract this cell out since it's too small to split
        hd->free = DLExtract(hd->free, cell);
    }
    else
    {
        //---- cell is large enough to split into two pieces
        cell->size = (long)size;

        //---- Create a new cell
        newCell = (Cell *) ((char *)cell + size);
        newCell->size = leftoverSize;
#ifdef SDK_DEBUG
        newCell->hd = NULL;
#endif

        //---- Leave newCell in free, and take cell away
        newCell->prev = cell->prev;
        newCell->next = cell->next;

        if (newCell->next != NULL)
        {
            newCell->next->prev = newCell;
        }

        if (newCell->prev != NULL)
        {
            newCell->prev->next = newCell;
        }
        else
        {
            SDK_ASSERTMSG(hd->free == cell, OS_ERR_ALLOCFROMHEAP_BROKENHEAP);
            hd->free = newCell;
        }
    }

    //---- Add to allocated list
    hd->allocated = DLAddFront(hd->allocated, cell);

#ifdef  SDK_DEBUG
    cell->hd = hd;
    cell->requested = requested;
    hd->headerBytes += HEADERSIZE;
    hd->paddingBytes += cell->size - (HEADERSIZE + requested);
    hd->payloadBytes += requested;
#endif // SDK_DEBUG

    (void)OS_RestoreInterrupts(enabled);

    return (void *)((char *)cell + HEADERSIZE);
}

/*---------------------------------------------------------------------------*
  Name:         OS_AllocFixed

  Description:  Allocates the block of memory specified by /rstart/ and
                /rend/.  Will break up any heap.  Will not check for overlap
                with other fixed blocks.  May create a zero-length heap.

  Arguments:    id     : arena ID
                rstart : pointer to starting addr of block
                rend   : pointer to ending addr of block

  Returns:      a null pointer or a pointer to the allocated space aligned
                with ALIGNMENT bytes boundaries. /rstart/ and /rend/ might be
                adjusted to the boundaries of really allocated region.
 *---------------------------------------------------------------------------*/
void   *OS_AllocFixed(OSArenaId id, void **rstart, void **rend)
{
    OSHeapInfo *heapInfo;
    OSHeapHandle i;                    // heap iterator
    Cell   *cell;                      // object iterator
    Cell   *newCell;                   // for creating new objects if necessary
    HeapDesc *hd;
    void   *start = (void *)TRUNC(*rstart, ALIGNMENT);
    void   *end = (void *)ROUND(*rend, ALIGNMENT);
    OSIntrMode enabled = OS_DisableInterrupts();

    //---- check arena id
    SDK_ASSERTMSG(id < OS_ARENA_MAX, OS_ERR_ALLOCFIXED_INVID);
    SDK_ASSERTMSG(OSiHeapInfo[id], OS_ERR_ALLOCFIXED_NOINFO);
    heapInfo = OSiHeapInfo[id];

    SDK_ASSERTMSG(heapInfo->heapArray, OS_ERR_ALLOCFIXED_NOHEAP);
    SDK_ASSERTMSG(start < end, OS_ERR_ALLOCFIXED_INVRANGE);
    SDK_ASSERTMSG(RangeSubset(start, end, heapInfo->arenaStart, heapInfo->arenaEnd),
                  OS_ERR_ALLOCFIXED_INVRANGE);

    //---- Check overlap with any allocated blocks.
    for (i = 0; i < heapInfo->numHeaps; i++)
    {
        hd = &heapInfo->heapArray[i];
        if (hd->size < 0)              // Is inactive?
        {
            continue;
        }

        if (DLOverlap(hd->allocated, start, end))
        {
#ifdef  SDK_DEBUG
            OS_Printf("OS_AllocFixed: Warning - failed to allocate from %p to %p\n", start, end);
#endif // SDK_DEBUG
            (void)OS_RestoreInterrupts(enabled);
            return NULL;
        }
    }

    /*
       ASSUME : if we get past all this, the fixed request will
       not overlap with any non-contiguous free memory.

       Iterate over heaps breaking up appropriate blocks.
       note that we cannot change the size of the heap by simply subtracting
       out the overlap area, since the heaps may be non-contiguous.
     */
    for (i = 0; i < heapInfo->numHeaps; i++)
    {
        //---- for each free obj in heap, find and break overlaps.
        hd = &heapInfo->heapArray[i];

        if (hd->size < 0)              // Is inactive?
        {
            continue;
        }

        for (cell = hd->free; cell; cell = cell->next)
        {
            void   *cellEnd = (char *)cell + cell->size;

            if ((char *)cellEnd <= (char *)start)
            {
                continue;
            }

            if ((char *)end <= (char *)cell)
            {
                break;                 // Since free is sorted in order of start addresses
            }

            if (InRange(cell, (char *)start - HEADERSIZE, end) &&
                InRange((char *)cellEnd, start, (char *)end + MINOBJSIZE))
            {
                if ((char *)cell < (char *)start)
                    start = (void *)cell;
                if ((char *)end < (char *)cellEnd)
                    end = (void *)cellEnd;

                //---- cell is completely overlaped. Just extract this block
                hd->free = DLExtract(hd->free, cell);   // Note cell->next is intact. XXX
                hd->size -= cell->size; // Update stats
                continue;
            }

            if (InRange(cell, (char *)start - HEADERSIZE, end))
            {
                if ((char *)cell < (char *)start)
                {
                    start = (void *)cell;
                }

                // Start of object in middle of range. break off top.
                // Avoid DLExtract() and DLInsert() since we already know
                // exactly where the block should go
                SDK_ASSERT(MINOBJSIZE <= (char *)cellEnd - (char *)end);
                newCell = (Cell *) end;
                newCell->size = (char *)cellEnd - (char *)end;
#ifdef  SDK_DEBUG
                newCell->hd = NULL;
#endif // SDK_DEBUG
                newCell->next = cell->next;
                if (newCell->next)
                {
                    newCell->next->prev = newCell;
                }
                newCell->prev = cell->prev;
                if (newCell->prev)
                {
                    newCell->prev->next = newCell;
                }
                else
                {
                    hd->free = newCell; // new head
                }
                hd->size -= (char *)end - (char *)cell;
                break;
            }

            if (InRange((char *)cellEnd, start, (char *)end + MINOBJSIZE))
            {
                if ((char *)end < (char *)cellEnd)
                {
                    end = (void *)cellEnd;
                }

                //---- Nothing to change except size
                SDK_ASSERT(MINOBJSIZE <= (char *)start - (char *)cell);
                hd->size -= (char *)cellEnd - (char *)start;
                cell->size = (char *)start - (char *)cell;
                continue;
            }

            //---- Create a new cell after end of the fixed block.
            SDK_ASSERT(MINOBJSIZE <= (char *)cellEnd - (char *)end);
            newCell = (Cell *) end;
            newCell->size = (char *)cellEnd - (char *)end;
#ifdef  SDK_DEBUG
            newCell->hd = NULL;
#endif // SDK_DEBUG
            newCell->next = cell->next;
            if (newCell->next)
            {
                newCell->next->prev = newCell;
            }
            newCell->prev = cell;
            cell->next = newCell;      // cell is before newCell
            cell->size = (char *)start - (char *)cell;
            hd->size -= (char *)end - (char *)start;
            break;
        }
        SDK_ASSERT(0 <= hd->size);
    }

    SDK_ASSERT(OFFSET(start, ALIGNMENT) == 0);
    SDK_ASSERT(OFFSET(end, ALIGNMENT) == 0);
    SDK_ASSERT(start < end);
    *rstart = start;
    *rend = end;

    (void)OS_RestoreInterrupts(enabled);
    return *rstart;
}

/*---------------------------------------------------------------------------*
  Name:         OS_FreeToHeap

  Description:  Returns obj /ptr/ to /heap/.

  Arguments:    id       : arena ID
                heap     : handle to the heap that /ptr/ was allocated from
                ptr      : pointer to object previously returned from
                           OS_Alloc() or OS_AllocFromHeap().

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OS_FreeToHeap(OSArenaId id, OSHeapHandle heap, void *ptr)
{
    OSHeapInfo *heapInfo;
    HeapDesc *hd;
    Cell   *cell;

    OSIntrMode enabled = OS_DisableInterrupts();

    //---- check arena id
    SDK_ASSERTMSG(id < OS_ARENA_MAX, OS_ERR_FREETOHEAP_INVID);
    SDK_ASSERTMSG(OSiHeapInfo[id], OS_ERR_FREETOHEAP_NOINFO);
    heapInfo = OSiHeapInfo[id];

    if (heap < 0)
    {
        heap = heapInfo->currentHeap;
    }

    SDK_ASSERTMSG(heapInfo->heapArray, OS_ERR_FREETOHEAP_NOHEAP);
    SDK_ASSERTMSG(InRange
                  (ptr, (char *)heapInfo->arenaStart + HEADERSIZE, (char *)heapInfo->arenaEnd),
                  OS_ERR_FREETOHEAP_INVPTR);
    SDK_ASSERTMSG(OFFSET(ptr, ALIGNMENT) == 0, OS_ERR_FREETOHEAP_INVPTR);
    SDK_ASSERTMSG(0 <= heapInfo->heapArray[heap].size, OS_ERR_FREETOHEAP_INVHEAP);

    cell = (Cell *) ((char *)ptr - HEADERSIZE);
    hd = &heapInfo->heapArray[heap];

    SDK_ASSERTMSG(cell->hd == hd, OS_ERR_FREETOHEAP_INVPTR);
    SDK_ASSERTMSG(DLLookup(hd->allocated, cell), OS_ERR_FREETOHEAP_INVPTR);

#ifdef  SDK_DEBUG
    cell->hd = NULL;
    hd->headerBytes -= HEADERSIZE;
    hd->paddingBytes -= cell->size - (HEADERSIZE + cell->requested);
    hd->payloadBytes -= cell->requested;
#endif // SDK_DEBUG

    //---- Extract from the allocated list
    hd->allocated = DLExtract(hd->allocated, cell);

    //---- Add in sorted order to free list (coalesced with next and prev)
    hd->free = DLInsert(hd->free, cell);

    (void)OS_RestoreInterrupts(enabled);
}

/*---------------------------------------------------------------------------*
  Name:         OS_FreeAllToHeap

  Description:  free all allocated block in the specified heap

  Arguments:    id       : arena ID
                heap     : handle to the heap

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OS_FreeAllToHeap(OSArenaId id, OSHeapHandle heap)
{
    OSHeapInfo *heapInfo;
    HeapDesc *hd;
    Cell   *cell;

    OSIntrMode enabled = OS_DisableInterrupts();

    //---- check arena id
    SDK_ASSERTMSG(id < OS_ARENA_MAX, OS_ERR_FREETOHEAP_INVID);
    SDK_ASSERTMSG(OSiHeapInfo[id], OS_ERR_FREETOHEAP_NOINFO);
    heapInfo = OSiHeapInfo[id];

    if (heap < 0)
    {
        heap = heapInfo->currentHeap;
    }

    SDK_ASSERTMSG(heapInfo->heapArray, "heap not initialized");
    SDK_ASSERTMSG(0 <= heapInfo->heapArray[heap].size, "invalid heap handle");

    hd = &heapInfo->heapArray[heap];
    while ((cell = hd->allocated) != NULL)
    {
        SDK_ASSERT(cell->hd == hd);
        SDK_ASSERT(DLLookup(hd->allocated, cell));

#ifdef  SDK_DEBUG
        cell->hd = NULL;
        hd->headerBytes -= HEADERSIZE;
        hd->paddingBytes -= cell->size - (HEADERSIZE + cell->requested);
        hd->payloadBytes -= cell->requested;
#endif // SDK_DEBUG

        //---- Extract from the allocated list
        hd->allocated = DLExtract(hd->allocated, cell);

        //---- Add in sorted order to free list (coalesced with next and prev)
        hd->free = DLInsert(hd->free, cell);
    }

    (void)OS_RestoreInterrupts(enabled);
}

/*---------------------------------------------------------------------------*
  Name:         OS_SetCurrentHeap

  Description:  Sets OSi_CurrentHeap to /heap/.  All subsequent calls to
                OS_Alloc() will be performed on this heap until another
                call to OS_SetCurrentHeap().

  Arguments:    id   : arena ID
                heap : handle to a heap that was returned from OS_CreateHeap()

  Returns:      previous heap handle.
 *---------------------------------------------------------------------------*/
OSHeapHandle OS_SetCurrentHeap(OSArenaId id, OSHeapHandle heap)
{
    OSHeapInfo *heapInfo;
    OSHeapHandle prev;
    OSIntrMode enabled = OS_DisableInterrupts();

    //---- check arena id
    SDK_ASSERTMSG(id < OS_ARENA_MAX, OS_ERR_SETCURRENTHEAP_INVID);
    SDK_ASSERTMSG(OSiHeapInfo[id], OS_ERR_SETCURRENTHEAP_NOINFO);
    heapInfo = OSiHeapInfo[id];

    SDK_ASSERTMSG(heapInfo->heapArray, OS_ERR_SETCURRENTHEAP_NOHEAP);
    SDK_ASSERTMSG(0 <= heap && heap < heapInfo->numHeaps, OS_ERR_SETCURRENTHEAP_INVHEAP);
    SDK_ASSERTMSG(0 <= heapInfo->heapArray[heap].size, OS_ERR_SETCURRENTHEAP_INVHEAP);
    prev = heapInfo->currentHeap;
    heapInfo->currentHeap = heap;

    (void)OS_RestoreInterrupts(enabled);
    return prev;
}

/*---------------------------------------------------------------------------*
  Name:         OS_InitAlloc

  Description:  Initializes the arena in which all heaps will reside.
                Reserves some small amount of memory for array of heap
                descriptors.

  Arguments:    id         : arena ID
                arenaStart : beginning addr of arena
                arenaEnd   : ending addr of arena
                maxHeaps   : Maximum number of active heaps that will be
                           : used in lifetime of program

  Returns:      start of real arena, aligned with 32 bytes boundaries, after
                heap array has been allocated
 *---------------------------------------------------------------------------*/
void   *OS_InitAlloc(OSArenaId id, void *arenaStart, void *arenaEnd, int maxHeaps)
{
    OSHeapInfo *heapInfo;
    u32     arraySize;
    OSHeapHandle i;
    OSIntrMode enabled = OS_DisableInterrupts();

    //---- check arena id
    SDK_ASSERTMSG(id < OS_ARENA_MAX, OS_ERR_INITALLOC_INVID);
    SDK_ASSERTMSG(OSiHeapInfo[id] == NULL, OS_ERR_INITALLOC_INVINFO);

    SDK_ASSERTMSG(0 < maxHeaps, OS_ERR_INITALLOC_INVNUMHEAPS);
    SDK_ASSERTMSG((char *)arenaStart < (char *)arenaEnd, OS_ERR_INITALLOC_INVRANGE);
    SDK_ASSERTMSG(maxHeaps <= ((char *)arenaEnd - (char *)arenaStart) / sizeof(HeapDesc),
                  OS_ERR_INITALLOC_INSRANGE);

    //---- save heapInfo
    heapInfo = arenaStart;
    OSiHeapInfo[id] = heapInfo;

    //---- Place HeapArray at head of the arena
    arraySize = sizeof(HeapDesc) * maxHeaps;
    heapInfo->heapArray = (void *)((u32)arenaStart + sizeof(OSHeapInfo));
    heapInfo->numHeaps = maxHeaps;

    for (i = 0; i < heapInfo->numHeaps; i++)
    {
        HeapDesc *hd = &heapInfo->heapArray[i];

        hd->size = -1;
        hd->free = hd->allocated = NULL;
#ifdef  SDK_DEBUG
        hd->paddingBytes = hd->headerBytes = hd->payloadBytes = 0;
#endif // SDK_DEBUG
    }

    //---- Set OSi_CurrentHeap to an invalid value
    heapInfo->currentHeap = -1;

    //---- Reset arenaStart to the nearest reasonable location
    arenaStart = (void *)((char *)heapInfo->heapArray + arraySize);
    arenaStart = (void *)ROUND(arenaStart, ALIGNMENT);

    heapInfo->arenaStart = arenaStart;
    heapInfo->arenaEnd = (void *)TRUNC(arenaEnd, ALIGNMENT);
    SDK_ASSERTMSG(MINOBJSIZE <= (char *)heapInfo->arenaEnd - (char *)heapInfo->arenaStart,
                  OS_ERR_INITALLOC_INSRANGE);

    (void)OS_RestoreInterrupts(enabled);
    return heapInfo->arenaStart;
}

/*---------------------------------------------------------------------------*
  Name:         OS_ClearAlloc

  Description:  Clear heap pointer in system shared.
                After calling this, you can call OS_InitAlloc() again.

  Arguments:    id  :   arena ID

  Returns:      None
 *---------------------------------------------------------------------------*/
void OS_ClearAlloc(OSArenaId id)
{
    SDK_ASSERTMSG(id < OS_ARENA_MAX, OS_ERR_CLEARALLOC_INVID);
    OSiHeapInfo[id] = NULL;
}

/*---------------------------------------------------------------------------*
  Name:         OS_CreateHeap

  Description:  Reserves area of memory from /start/ to /end/ for use as a
                heap.  Initializes heap descriptor and free list.
                Will consume one entry in heap array.

  Arguments:    id    : arena ID
                start : starting addr of heap
                end   : ending addr of heap

  Returns:      If the function succeeds, it returns a new handle to heap
                for use in OS_AllocFromHeap(), OS_FreeToHeap(), etc.
                If the function fails, the return value is -1.
 *---------------------------------------------------------------------------*/
OSHeapHandle OS_CreateHeap(OSArenaId id, void *start, void *end)
{
    OSHeapInfo *heapInfo;
    OSHeapHandle heap;
    HeapDesc *hd;
    Cell   *cell;

    OSIntrMode enabled = OS_DisableInterrupts();

    //OS_Printf( "OS_CreateHeap  id=%d start=%x, end=%x\n", id, start, end );
    //---- check arena id
    SDK_ASSERTMSG(id < OS_ARENA_MAX, OS_ERR_CREATEHEAP_INVID);
    SDK_ASSERTMSG(OSiHeapInfo[id], OS_ERR_CREATEHEAP_NOINFO);
    heapInfo = OSiHeapInfo[id];

    SDK_ASSERTMSG(heapInfo->heapArray, OS_ERR_CREATEHEAP_NOHEAP);
    SDK_ASSERTMSG(start < end, OS_ERR_CREATEHEAP_INVRANGE);
    start = (void *)ROUND(start, ALIGNMENT);
    end = (void *)TRUNC(end, ALIGNMENT);
    SDK_ASSERTMSG(start < end, OS_ERR_CREATEHEAP_INVRANGE);
    SDK_ASSERTMSG(RangeSubset(start, end, heapInfo->arenaStart, heapInfo->arenaEnd),
                  OS_ERR_CREATEHEAP_INVRANGE);
    SDK_ASSERTMSG(MINOBJSIZE <= (char *)end - (char *)start, OS_ERR_CREATEHEAP_INSRANGE);

#ifdef  SDK_DEBUG
    //---- Check that the range does not overlap with
    //     any other block in this or other heaps.
    for (heap = 0; heap < heapInfo->numHeaps; heap++)
    {
        if (heapInfo->heapArray[heap].size < 0)
        {
            continue;
        }
        SDK_ASSERTMSG(!DLOverlap(heapInfo->heapArray[heap].free, start, end),
                      OS_ERR_CREATEHEAP_INVRANGE);
        SDK_ASSERTMSG(!DLOverlap(heapInfo->heapArray[heap].allocated, start, end),
                      OS_ERR_CREATEHEAP_INVRANGE);
    }
#endif // SDK_DEBUG

    //---- Search for free descriptor
    for (heap = 0; heap < heapInfo->numHeaps; heap++)
    {
        hd = &heapInfo->heapArray[heap];
        if (hd->size < 0)
        {
            hd->size = (char *)end - (char *)start;

            cell = (Cell *) start;
            cell->prev = NULL;
            cell->next = NULL;
            cell->size = hd->size;
#ifdef  SDK_DEBUG
            cell->hd = NULL;
#endif // SDK_DEBUG

            hd->free = cell;
            hd->allocated = 0;
#ifdef  SDK_DEBUG
            hd->paddingBytes = hd->headerBytes = hd->payloadBytes = 0;
#endif // SDK_DEBUG

            (void)OS_RestoreInterrupts(enabled);
            return heap;
        }
    }

    //---- Could not find free descriptor
#ifdef  SDK_DEBUG
    OS_Printf("OS_CreateHeap: Warning - Failed to find free heap descriptor.");
#endif // SDK_DEBUG

    (void)OS_RestoreInterrupts(enabled);
    return -1;
}

/*---------------------------------------------------------------------------*
  Name:         OS_DestroyHeap

  Description:  Frees up the descriptor for the /heap/.  Subsequent
                allocation requests from this heap will fail unless another
                heap is created with the same handle.

  Arguments:    id   : arena ID
                heap : handle to a live heap, previously created with OS_CreateHeap().

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OS_DestroyHeap(OSArenaId id, OSHeapHandle heap)
{
    OSHeapInfo *heapInfo;
    HeapDesc *hd;
#ifdef  SDK_DEBUG
    long    size;
#endif
    OSIntrMode enabled = OS_DisableInterrupts();

    //---- check arena id
    SDK_ASSERTMSG(id < OS_ARENA_MAX, OS_ERR_DESTROYHEAP_INVID);
    SDK_ASSERTMSG(OSiHeapInfo[id], OS_ERR_DESTROYHEAP_NOINFO);
    heapInfo = OSiHeapInfo[id];

    SDK_ASSERTMSG(heapInfo->heapArray, OS_ERR_DESTROYHEAP_NOHEAP);
    SDK_ASSERTMSG(0 <= heap && heap < heapInfo->numHeaps, OS_ERR_DESTROYHEAP_INVHEAP);
    SDK_ASSERTMSG(0 <= heapInfo->heapArray[heap].size, OS_ERR_DESTROYHEAP_INVHEAP);

    hd = &heapInfo->heapArray[heap];

#ifdef SDK_DEBUG
    //---- Check whether entire heap is empty
    size = DLSize(hd->free);
    if (hd->size != size)
    {
        OS_Printf("OS_DestroyHeap(%d): Warning - free list size %d, heap size %d\n", heap, size,
                  hd->size);
    }
#endif // SDK_DEBUG

    hd->size = -1;
    hd->free = hd->allocated = NULL;   // add to dolphin src

#ifdef  SDK_DEBUG
    hd->paddingBytes = hd->headerBytes = hd->payloadBytes = 0;
    if (heapInfo->currentHeap == heap)
    {
        heapInfo->currentHeap = -1;
    }
#endif // SDK_DEBUG

    (void)OS_RestoreInterrupts(enabled);
}

/*---------------------------------------------------------------------------*
  Name:         OS_AddToHeap

  Description:  Adds an arbitrary block of memory to /heap/.  Used to free
                blocks previously allocated with OS_AllocFixed(), or to
                create non-contiguous heaps.

  Arguments:    id    : arena ID
                heap  : handle to live heap, previously created with OS_CreateHeap().
                start : starting addr of block to add to /heap/
                end   : ending addr of block to add to /heap/

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OS_AddToHeap(OSArenaId id, OSHeapHandle heap, void *start, void *end)
{
    OSHeapInfo *heapInfo;
    HeapDesc *hd;
    Cell   *cell;
#ifdef  SDK_DEBUG
    OSHeapHandle i;
#endif // SDK_DEBUG
    OSIntrMode enabled = OS_DisableInterrupts();

    //---- check arena id
    SDK_ASSERTMSG(id < OS_ARENA_MAX, OS_ERR_ADDTOHEAP_INVID);
    SDK_ASSERTMSG(OSiHeapInfo[id], OS_ERR_ADDTOHEAP_NOINFO);
    heapInfo = OSiHeapInfo[id];

    SDK_ASSERTMSG(heapInfo->heapArray, OS_ERR_ADDTOHEAP_NOHEAP);
    SDK_ASSERTMSG(0 <= heap && heap < heapInfo->numHeaps, OS_ERR_ADDTOHEAP_INVHEAP);
    SDK_ASSERTMSG(0 <= heapInfo->heapArray[heap].size, OS_ERR_ADDTOHEAP_INVHEAP);

    hd = &heapInfo->heapArray[heap];

    SDK_ASSERTMSG(start < end, OS_ERR_ADDTOHEAP_INVRANGE);
    start = (void *)ROUND(start, ALIGNMENT);
    end = (void *)TRUNC(end, ALIGNMENT);
    SDK_ASSERTMSG(MINOBJSIZE <= (char *)end - (char *)start, OS_ERR_ADDTOHEAP_INSRANGE);
    SDK_ASSERTMSG(RangeSubset(start, end, heapInfo->arenaStart, heapInfo->arenaEnd),
                  OS_ERR_ADDTOHEAP_INVRANGE);

#ifdef  SDK_DEBUG
    //---- Check that the range does not already overlap with
    //     any other block in this or other heaps.
    for (i = 0; i < heapInfo->numHeaps; i++)
    {
        if (heapInfo->heapArray[i].size < 0)
        {
            continue;
        }
        SDK_ASSERTMSG(!DLOverlap(heapInfo->heapArray[i].free, start, end),
                      OS_ERR_ADDTOHEAP_INVRANGE);
        SDK_ASSERTMSG(!DLOverlap(heapInfo->heapArray[i].allocated, start, end),
                      OS_ERR_ADDTOHEAP_INVRANGE);
    }
#endif // SDK_DEBUG

    //---- Create a new cell
    cell = (Cell *) start;
    cell->size = (char *)end - (char *)start;
#ifdef  SDK_DEBUG
    cell->hd = NULL;
#endif // SDK_DEBUG

    //---- Insert new cell in free
    hd->size += cell->size;
    hd->free = DLInsert(hd->free, cell);

    (void)OS_RestoreInterrupts(enabled);
}

/*---------------------------------------------------------------------------*
  Name:         OS_CheckHeap

  Description:  Checks heap sanity for debugging

  Arguments:    id   : arena ID
                heap : handle to a live heap.

  Returns:      -1 if heap is not consistent. Otherwise, returns number
                of bytes available in free.
 *---------------------------------------------------------------------------*/

#ifndef  SDK_NO_MESSAGE
#define OSi_CHECK( exp )                                                 \
    do                                                                       \
{                                                                        \
    if ( !(exp) )                                                        \
    {                                                                    \
        OS_Printf( "OS_CheckHeap: Failed " #exp " in %d\n", __LINE__ );  \
        goto exit_OS_CheckHeap; /* goto is not beautiful, but less codes */ \
    }                                                                    \
} while (0)
#else
#define OSi_CHECK( exp )                                                 \
    do                                                                       \
{                                                                        \
    if ( !(exp) )                                                        \
    {                                                                    \
        goto exit_OS_CheckHeap; /* goto is not beautiful, but less codes */ \
    }                                                                    \
} while (0)
#endif

s32 OS_CheckHeap(OSArenaId id, OSHeapHandle heap)
{
    OSHeapInfo *heapInfo;
    HeapDesc *hd;
    Cell   *cell;
    long    total = 0;
    long    free = 0;
    long    retValue = -1;
    OSIntrMode enabled = OS_DisableInterrupts();

    //---- check arena id
    SDK_ASSERTMSG(id < OS_ARENA_MAX, OS_ERR_CHECKHEAP_INVID);
    SDK_ASSERTMSG(OSiHeapInfo[id], OS_ERR_CHECKHEAP_NOINFO);
    heapInfo = OSiHeapInfo[id];

    //---- heap<0  means current heap
    if (heap == OS_CURRENT_HEAP_HANDLE)
    {
        heap = heapInfo->currentHeap;
    }
    SDK_ASSERT(heap >= 0);

    OSi_CHECK(heapInfo->heapArray);
    OSi_CHECK(0 <= heap && heap < heapInfo->numHeaps);

    hd = &heapInfo->heapArray[heap];
    OSi_CHECK(0 <= hd->size);

    OSi_CHECK(hd->allocated == NULL || hd->allocated->prev == NULL);
    for (cell = hd->allocated; cell; cell = cell->next)
    {
        OSi_CHECK(InRange(cell, heapInfo->arenaStart, heapInfo->arenaEnd));
        OSi_CHECK(OFFSET(cell, ALIGNMENT) == 0);
        OSi_CHECK(cell->next == NULL || cell->next->prev == cell);
        OSi_CHECK(MINOBJSIZE <= cell->size);
        OSi_CHECK(OFFSET(cell->size, ALIGNMENT) == 0);

        total += cell->size;
        OSi_CHECK(0 < total && total <= hd->size);

#ifdef  SDK_DEBUG
        OSi_CHECK(cell->hd == hd);
        OSi_CHECK(HEADERSIZE + cell->requested <= cell->size);
#endif // SDK_DEBUG
    }

    OSi_CHECK(hd->free == NULL || hd->free->prev == NULL);
    for (cell = hd->free; cell; cell = cell->next)
    {
        OSi_CHECK(InRange(cell, heapInfo->arenaStart, heapInfo->arenaEnd));
        OSi_CHECK(OFFSET(cell, ALIGNMENT) == 0);
        OSi_CHECK(cell->next == NULL || cell->next->prev == cell);
        OSi_CHECK(MINOBJSIZE <= cell->size);
        OSi_CHECK(OFFSET(cell->size, ALIGNMENT) == 0);
        OSi_CHECK(cell->next == NULL || (char *)cell + cell->size < (char *)cell->next);

        total += cell->size;
        free += cell->size - HEADERSIZE;
        OSi_CHECK(0 < total && total <= hd->size);

#ifdef  SDK_DEBUG
        OSi_CHECK(cell->hd == NULL);
#endif // SDK_DEBUG
    }

    OSi_CHECK(total == hd->size);
    retValue = free;

  exit_OS_CheckHeap:
    (void)OS_RestoreInterrupts(enabled);
    return retValue;
}

/*---------------------------------------------------------------------------*
  Name:         OS_ReferentSize

  Description:  Returns size of payload

  Arguments:    id  : arena ID
                ptr : pointer to object previously returned from
                      OS_Alloc() or OSAllocFromHeap().

  Returns:      size of payload
 *---------------------------------------------------------------------------*/
u32 OS_ReferentSize(OSArenaId id, void *ptr)
{
    OSHeapInfo *heapInfo;
    Cell   *cell;
    OSIntrMode enabled = OS_DisableInterrupts();

    //---- check arena id
    SDK_ASSERTMSG(id < OS_ARENA_MAX, OS_ERR_REFERENTSIZE_INVID);
    SDK_ASSERTMSG(OSiHeapInfo[id], OS_ERR_REFERENTSIZE_NOINFO);
    heapInfo = OSiHeapInfo[id];

    SDK_ASSERTMSG(heapInfo->heapArray, OS_ERR_REFERENT_NOHEAP);
    SDK_ASSERTMSG(InRange
                  (ptr, (char *)heapInfo->arenaStart + HEADERSIZE, (char *)heapInfo->arenaEnd),
                  OS_ERR_REFERENT_INVPTR);
    SDK_ASSERTMSG(OFFSET(ptr, ALIGNMENT) == 0, OS_ERR_REFERENT_INVPTR);

    cell = (Cell *) ((char *)ptr - HEADERSIZE);

    SDK_ASSERTMSG(cell->hd, OS_ERR_REFERENT_INVPTR);
    SDK_ASSERTMSG(((char *)cell->hd - (char *)heapInfo->heapArray) % sizeof(HeapDesc) == 0,
                  OS_ERR_REFERENT_INVPTR);
    SDK_ASSERTMSG(heapInfo->heapArray <= cell->hd
                  && cell->hd < &heapInfo->heapArray[heapInfo->numHeaps], OS_ERR_REFERENT_INVPTR);
    SDK_ASSERTMSG(0 <= cell->hd->size, OS_ERR_REFERENT_INVPTR);
    SDK_ASSERTMSG(DLLookup(cell->hd->allocated, cell), OS_ERR_REFERENT_INVPTR);

    (void)OS_RestoreInterrupts(enabled);
    return (u32)(cell->size - HEADERSIZE);
}

/*---------------------------------------------------------------------------*
  Name:         OS_DumpHeap

  Description:  Dumps statistics and elements of a heap

  Arguments:    id   :  arena ID
                heap :  handle to a heap.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OS_DumpHeap(OSArenaId id, OSHeapHandle heap)
{
#ifndef SDK_FINALROM
    OSHeapInfo *heapInfo;
    HeapDesc *hd;
    Cell   *cell;
    OSIntrMode enabled = OS_DisableInterrupts();

    OS_Printf("\nOS_DumpHeap(%d):\n", heap);

    //---- check arena id
    SDK_ASSERTMSG(id < OS_ARENA_MAX, OS_ERR_DUMPHEAP_INVID);
    SDK_ASSERTMSG(OSiHeapInfo[id], OS_ERR_DUMPHEAP_NOINFO);
    heapInfo = OSiHeapInfo[id];

    SDK_ASSERTMSG(heapInfo && heapInfo->heapArray, OS_ERR_DUMPHEAP_NOHEAP);

    //---- heap<0  means current heap
    if (heap < 0)
    {
        heap = heapInfo->currentHeap;
    }
    SDK_ASSERTMSG(0 <= heap && heap < heapInfo->numHeaps, OS_ERR_DUMPHEAP_INVHEAP);

    hd = &heapInfo->heapArray[heap];
    if (hd->size < 0)
    {
        OS_Printf("--------Inactive\n");
        return;
    }

    SDK_ASSERTMSG(0 <= OS_CheckHeap(id, heap), OS_ERR_DUMPHEAP_BROKENHEAP);

#ifdef  SDK_DEBUG
    OS_Printf("padding %d/(%f%%) header %d/(%f%%) payload %d/(%f%%)\n",
              hd->paddingBytes, 100.0 * hd->paddingBytes / hd->size,
              hd->headerBytes, 100.0 * hd->headerBytes / hd->size,
              hd->payloadBytes, 100.0 * hd->payloadBytes / hd->size);
#endif // SDK_DEBUG

    OS_Printf("addr\tsize\tend\tprev\tnext\n");
    OS_Printf("--------Allocated\n");
    SDK_ASSERTMSG(hd->allocated == NULL || hd->allocated->prev == NULL, OS_ERR_DUMPHEAP_BROKENHEAP);
    for (cell = hd->allocated; cell; cell = cell->next)
    {
        OS_Printf("%x\t%d\t%x\t%x\t%x\n",
                  cell, cell->size, (char *)cell + cell->size, cell->prev, cell->next);
    }

    OS_Printf("--------Free\n");
    for (cell = hd->free; cell; cell = cell->next)
    {
        OS_Printf("%x\t%d\t%x\t%x\t%x\n",
                  cell, cell->size, (char *)cell + cell->size, cell->prev, cell->next);
    }

    (void)OS_RestoreInterrupts(enabled);
#else  // ifndef SDK_FINALROM

#pragma unused( id, heap )

#endif // ifndef SDK_FINALROM
}


/*---------------------------------------------------------------------------*
  Name:         OS_VisitAllocated

  Description:  Visits every element of every allocated block of memory,
                calling a routine on each one.

  Arguments:    id      : arena ID
                visitor : function to be called on each cell

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OS_VisitAllocated(OSArenaId id, OSAllocVisitor visitor)
{
    OSHeapInfo *heapInfo;
    u32     heap;
    Cell   *cell;

    OSIntrMode enabled = OS_DisableInterrupts();

    //---- check arena id
    SDK_ASSERTMSG(id < OS_ARENA_MAX, OS_ERR_VISITALLOCATED_INVID);
    SDK_ASSERTMSG(OSiHeapInfo[id], OS_ERR_VISITALLOCATED_NOINFO);
    heapInfo = OSiHeapInfo[id];

    for (heap = 0; heap < heapInfo->numHeaps; heap++)
    {
        if (heapInfo->heapArray[heap].size >= 0)
        {
            for (cell = heapInfo->heapArray[heap].allocated; cell; cell = cell->next)
            {
                visitor((void *)((u8 *)cell + HEADERSIZE), (u32)(cell->size - HEADERSIZE));
            }
        }
    }

    (void)OS_RestoreInterrupts(enabled);
}

//================================================================================
//     Get information about heap
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         OSi_GetTotalAllocSize

  Description:  Get sum of allocated block size.
				Subroutine for OS_GetTotalAllocSize and OS_GetTotalOccupiedSize.

  Arguments:    id            : arena ID
                heap          : handle to a heap.
				isHeadInclude : whether if including block header.

  Returns:      sum of allocated block size
 *---------------------------------------------------------------------------*/
u32 OSi_GetTotalAllocSize(OSArenaId id, OSHeapHandle heap, BOOL isHeadInclude)
{
    OSHeapInfo *heapInfo;
    Cell   *cell;
    u32     sum = 0;
    OSIntrMode enabled = OS_DisableInterrupts();

    SDK_ASSERT(id < OS_ARENA_MAX);
    heapInfo = OSiHeapInfo[id];
    SDK_ASSERT(heapInfo);
    SDK_ASSERT(heap < heapInfo->numHeaps);

    //---- heap<0  means current heap
    if (heap < 0)
    {
        heap = heapInfo->currentHeap;
    }

    if (isHeadInclude)
    {
        for (cell = heapInfo->heapArray[heap].allocated; cell; cell = cell->next)
        {
            sum += (u32)(cell->size);
        }
    }
    else
    {
        for (cell = heapInfo->heapArray[heap].allocated; cell; cell = cell->next)
        {
            sum += (u32)(cell->size - HEADERSIZE);
        }
    }

    (void)OS_RestoreInterrupts(enabled);

    return sum;
}

/*---------------------------------------------------------------------------*
  Name:         OS_GetTotalFreeSize

  Description:  Get sum of free block size,
                not includeing of block header.

  Arguments:    id   :  arena ID
                heap :  handle to a heap.

  Returns:      sum of free block size
 *---------------------------------------------------------------------------*/
u32 OS_GetTotalFreeSize(OSArenaId id, OSHeapHandle heap)
{
    OSHeapInfo *heapInfo;
    Cell   *cell;
    u32     sum = 0;
    OSIntrMode enabled = OS_DisableInterrupts();

    SDK_ASSERT(id < OS_ARENA_MAX);
    heapInfo = OSiHeapInfo[id];
    SDK_ASSERT(heapInfo);
    SDK_ASSERT(heap < heapInfo->numHeaps);

    //---- heap<0  means current heap
    if (heap < 0)
    {
        heap = heapInfo->currentHeap;
    }

    for (cell = heapInfo->heapArray[heap].free; cell; cell = cell->next)
    {
        sum += (u32)(cell->size - HEADERSIZE);
    }

    (void)OS_RestoreInterrupts(enabled);

    return sum;
}

/*---------------------------------------------------------------------------*
  Name:         OS_GetMaxFreeSize

  Description:  Get maximun free block size

  Arguments:    id   :  arena ID
                heap :  handle to a heap.

  Returns:      maximum free block size.
 *---------------------------------------------------------------------------*/
u32 OS_GetMaxFreeSize(OSArenaId id, OSHeapHandle heap)
{
    OSHeapInfo *heapInfo;
    Cell   *cell;
    u32     candidate = 0;
    OSIntrMode enabled = OS_DisableInterrupts();

    SDK_ASSERT(id < OS_ARENA_MAX);
    heapInfo = OSiHeapInfo[id];
    SDK_ASSERT(heapInfo);
    SDK_ASSERT(heap < heapInfo->numHeaps);

    //---- heap<0  means current heap
    if (heap < 0)
    {
        heap = heapInfo->currentHeap;
    }

    for (cell = heapInfo->heapArray[heap].free; cell; cell = cell->next)
    {
        u32     size = (u32)(cell->size - HEADERSIZE);
        if (size > candidate)
        {
            candidate = size;
        }
    }

    (void)OS_RestoreInterrupts(enabled);

    return candidate;
}

//================================================================================
//     re-initialize heap.
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_ClearHeap

  Description:  re-initialize heap.

  Arguments:    id   :  arena ID
                heap :  handle to a heap.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OS_ClearHeap(OSArenaId id, OSHeapHandle heap, void *start, void *end)
{
    OSHeapInfo *heapInfo;
    HeapDesc *hd;
    Cell   *cell;

    OSIntrMode enabled = OS_DisableInterrupts();

    //---- check arena id
    SDK_ASSERTMSG(id < OS_ARENA_MAX, "invalid id");

    heapInfo = OSiHeapInfo[id];

    SDK_ASSERTMSG(heapInfo && heapInfo->heapArray, "heap not initialized");
    SDK_ASSERTMSG(start < end, "invalid range");
    start = (void *)ROUND(start, ALIGNMENT);
    end = (void *)TRUNC(end, ALIGNMENT);
    SDK_ASSERTMSG(start < end, "invalid range");
    SDK_ASSERTMSG(RangeSubset(start, end, heapInfo->arenaStart, heapInfo->arenaEnd),
                  "invalid range");
    SDK_ASSERTMSG(MINOBJSIZE <= (char *)end - (char *)start, "too small range");

    //---- heap<0  means current heap
    if (heap < 0)
    {
        heap = heapInfo->currentHeap;
    }

    //---- Search for free descriptor
    hd = &heapInfo->heapArray[heap];
    hd->size = (char *)end - (char *)start;

    cell = (Cell *) start;
    cell->prev = NULL;
    cell->next = NULL;
    cell->size = hd->size;
#ifdef  SDK_DEBUG
    cell->hd = NULL;
#endif // SDK_DEBUG

    hd->free = cell;
    hd->allocated = 0;
#ifdef  SDK_DEBUG
    hd->paddingBytes = hd->headerBytes = hd->payloadBytes = 0;
#endif // SDK_DEBUG

    (void)OS_RestoreInterrupts(enabled);
}
