/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g2d - fmt
  File:     g2d_Common_data.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.14 $
 *---------------------------------------------------------------------------*/
#ifndef NNS_G2D_COMMON_DATA_H_
#define NNS_G2D_COMMON_DATA_H_

#include <nitro/types.h>
#include <nnsys/g2d/g2d_config.h>

#ifdef NNS_FROM_TOOL
#ifndef NNS_G2D_ASSERT
    #include <assert.h>
    #define NNS_G2D_ASSERT(x) assert((x))
    #define NNS_G2D_ASSERTMSG(x, y) assert((x))
    #define NNS_G2D_NULL_ASSERT(x) assert(NULL != (x))
    #define NNS_G2D_MINMAX_ASSERT(x,min,max) assert( ( (x) >= (min) )&&( (x) <= (max) ) )
#endif // NNS_G2D_ASSERT

#if (defined(_MSC_VER) && !defined(__cplusplus))
#define inline __inline
#endif

#endif // NNS_FROM_TOOL



#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
#define NNS_G2D_DUMMY_DATA8         (u8)0xCC
#define NNS_G2D_DUMMY_DATA16        (u16)0xBEEF
#define NNS_G2D_DUMMY_DATA32        (u32)0xDEADBEEF
#define NNS_G2D_DUMMY_PTR           (u32)0x00000000

//------------------------------------------------------------------------------
#define NNS_G2D_BLKSIG_NAMELABEL        (u32)('LABL')
#define NNS_G2D_BLKSIG_USEREXT          (u32)('UEXT')

#define NNS_G2D_BIGENDIAN_BITMARK          0xfffe
#define NNS_G2D_LITTLEENDIAN_BITMARK       0xfeff



//------------------------------------------------------------------------------
typedef struct NNSG2dBinaryFileHeader
{
    u32        signature;
    u16        byteOrder;
    u16        version;
    u32        fileSize;
    u16        headerSize;
    u16        dataBlocks;

}NNSG2dBinaryFileHeader;

//------------------------------------------------------------------------------
typedef struct NNSG2dBinaryBlockHeader
{
    u32        kind;
    u32        size;

}NNSG2dBinaryBlockHeader;

//------------------------------------------------------------------------------
// ユーザ拡張領域ブロックヘッダ
typedef struct NNSG2dUserExDataBlock
{
    u32        blkTypeID;
    u32        blkSize;

}NNSG2dUserExDataBlock;




//------------------------------------------------------------------------------
// inline fuctions 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
NNS_G2D_INLINE BOOL NNS_G2dIsPad16Valid( u16 pad16 )
{
    return ( NNS_G2D_DUMMY_DATA16 == pad16 ) ? TRUE : FALSE;
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE BOOL NNS_G2dIsPad32Valid( u32 pad32 )
{
    return ( NNS_G2D_DUMMY_DATA32 == pad32 ) ? TRUE : FALSE;
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE u16 NNS_G2dMakeVersionData( u8 majorVer, u8 minorVer )
{
    return (u16)((u16)majorVer << 8 | ((u16)minorVer & 0x00FF ));
}

//------------------------------------------------------------------------------
// 指定したIDを持つユーザ拡張ブロックを取得する。
// pExtendData        ユーザ拡張データの先頭をさすポインタ
// blkTypeID          ユーザ拡張データブロックID
//
NNS_G2D_INLINE const NNSG2dUserExDataBlock* NNSi_G2dGetUserExDataBlkByID
(
    const void* pExtendData, 
    u32         blkTypeID 
)
{
    if( pExtendData != NULL )
    {
        const NNSG2dUserExDataBlock* pBlk = (const NNSG2dUserExDataBlock*)pExtendData;    
        
        // 指定したIDのブロックが発見された
        if( pBlk->blkTypeID == blkTypeID )
        {
            return pBlk;
        }
        // 次のブロックへ...
        pBlk = (const NNSG2dUserExDataBlock*)((const u8*)pBlk + pBlk->blkSize);        
    }
    return NULL;
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // NNS_G2D_COMMON_DATA_H_

