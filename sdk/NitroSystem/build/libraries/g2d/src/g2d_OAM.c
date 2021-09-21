/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - g2d
  File:     g2d_OAM.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.44 $
 *---------------------------------------------------------------------------*/

#include <nnsys/g2d/g2d_OAM.h>
#include <nnsys/g2d/g2d_Softsprite.h>

#include <nnsys/g2d/g2d_OamSoftwareSpriteDraw.h>

#include <nnsys/g2d/fmt/g2d_Cell_data.h>

#include "g2d_Internal.h"
#include "g2di_Dma.h"


//
// g2d_Oam_data.h で宣言されている、テーブルの実体を定義します。
// 場所はここが適当ではないかもしれません。
// いづれべつのファイルへと移動される可能性があります。
//
NNS_G2D_DEFINE_NNSI_OBJSIZEWTBL;
NNS_G2D_DEFINE_NNSI_OBJSIZEHTBL;



//------------------------------------------------------------------------------
#define OAM_SETTING_INVISIBLE   192     // OAM Attribute 不可視設定


#define NUM_HW_OAM_ATTR         128     // OAM Attribute 数
#define NUM_HW_OAM_AFFINE       32      // Affine paramater 数
#define NUM_OAM_TYPES           3       // OAM の種類

// OAM 領域が誰にも使用されていない(使用予約テーブルの規定値)
#define OAM_NOT_USED            0xFFFF  

#define GX_AFFINE_SIZE          sizeof( GXOamAffine )
#define GX_OAMATTR_SIZE         sizeof( GXOamAttr )
#define OAMATTR_SIZE            sizeof( u16 ) * 3

//
// OBJ ロード時に使用される関数
//
typedef void (OBJLoadFunction)( const void *pSrc, u32 offset, u32 szByte );






//------------------------------------------------------------------------------
// OAMマネージャ の OAM への操作のキャッシュ
// 
typedef struct OamAttributeCache
{
    u16             reservationTable[NUM_HW_OAM_ATTR]; // 使用予約テーブル
    GXOamAttr       oamBuffer       [NUM_HW_OAM_ATTR]; // OAM Attribute のバッファ
    
}OamAttributeCache;

//------------------------------------------------------------------------------
// Affine paramaters への操作キャシュ
// OamAttributeCache と oamBuffer を 共有します。
// 
typedef struct OamAffineCache
{
    u16             reservationTable[NUM_HW_OAM_AFFINE]; // AffineParamater 使用予約テーブル
    
}OamAffineCache;

//------------------------------------------------------------------------------
// OAMマネージャ の OAM への操作のキャッシュ
// 
// マネージャ の OAM に対する操作は本構造によって一旦キャッシュされます。
// ユーザ は NNS_G2dApplyToHWXXX() の呼び出しによって、キャッシュの内容を
// HW に反映する必要があります。
// 
typedef struct OamCache
{
    OamAttributeCache           attributeCache; // OamAttr 
    OamAffineCache              affineCache;    // Affine params
    
}OamCache;




//------------------------------------------------------------------------------
// 唯一の OamCache 実体 
// NNS_G2dInitOamManagerModule() によって初期化されます。
// 
static OamCache                        oamCache_[NUM_OAM_TYPES];
static u16                             numRegisterdInstance_ = 0x0;











//------------------------------------------------------------------------------
// 管理領域が有効か判定します
static NNS_G2D_INLINE BOOL IsManageAreaValid_( const NNSG2dOAMManageArea* pArea )
{
    NNS_G2D_NULL_ASSERT( pArea );
    
    return (BOOL)( ( pArea->currentIdx <= pArea->toIdx + 1 ) &&
                   ( pArea->fromIdx <= pArea->toIdx ) );
}

//------------------------------------------------------------------------------
// 指定タイプの OamCache へのポインタを取得します。
// 
static NNS_G2D_INLINE OamCache* GetOamCachePtr_( NNSG2dOamType type )
{
    ASSERT_OAMTYPE_VALID( type );
    return &oamCache_[type];
}



//------------------------------------------------------------------------------
// 指定タイプの 管理領域予約テーブルへのポインタを取得します。( OamAttr 用)
static NNS_G2D_INLINE u16* GetOamReservationTblPtr_( NNSG2dOamType type )
{
    ASSERT_OAMTYPE_VALID( type );
    {        
        OamCache* pOamCache = GetOamCachePtr_( type );
        NNS_G2D_NULL_ASSERT( pOamCache );
    
        return pOamCache->attributeCache.reservationTable;
    }
}
//------------------------------------------------------------------------------
// 指定タイプの 管理領域予約テーブルへのポインタを取得します。( affine Paramater 用)
static NNS_G2D_INLINE u16* GetAffineReservationTblPtr_( NNSG2dOamType type )
{
    ASSERT_OAMTYPE_VALID( type );
    {
        OamCache* pOamCache = GetOamCachePtr_( type );
        NNS_G2D_NULL_ASSERT( pOamCache );
        
        return pOamCache->affineCache.reservationTable;
    }
}

//------------------------------------------------------------------------------
// OamAttr バッファの index 番目 へのポインタ を取得します。
static NNS_G2D_INLINE GXOamAttr* GetOamBufferPtr_( NNSG2dOamType type, u16 index )
{
    ASSERT_OAMTYPE_VALID( type );
    {
        OamCache* pOamCache = GetOamCachePtr_( type );
        NNS_G2D_NULL_ASSERT( pOamCache );
        
        return pOamCache->attributeCache.oamBuffer + index;
    }
}

//------------------------------------------------------------------------------
// Oam Affine paramater バッファの index 番目 へのポインタ を取得します。
// 内部で GetOamBufferPtr_() を 呼び出しています.
static NNS_G2D_INLINE GXOamAffine* GetAffineBufferPtr_( NNSG2dOamType type, u16 index )
{
    ASSERT_OAMTYPE_VALID( type );
    NNS_G2D_MINMAX_ASSERT   ( index, 0 , NUM_HW_OAM_AFFINE );
    
    {
        GXOamAffine* pAff =  (GXOamAffine*)GetOamBufferPtr_( type, 0 );
        NNS_G2D_NULL_ASSERT( pAff );
        
        return pAff + index;
    }
}

//------------------------------------------------------------------------------
// affine paramater を 取得します
static NNS_G2D_INLINE void GetAffineParams_( NNSG2dOamType type, u16 idx, MtxFx22* pMtx )
{
    ASSERT_OAMTYPE_VALID( type );
    NNS_G2D_MINMAX_ASSERT( idx, 0, NUM_HW_OAM_AFFINE );
    NNS_G2D_NULL_ASSERT( pMtx );
    
    {    
        GXOamAffine* pAff = GetAffineBufferPtr_( type, idx );
        NNS_G2D_NULL_ASSERT( pAff );
        
        // Affine transform parmeters(PA,PB,PC,PD) are in s7.8 format.
        // Since fx32 is in s19.12 format, shift them rightward by 4 bits.
        pMtx->_00 = (s16)(pAff->PA << 4);
        pMtx->_01 = (s16)(pAff->PB << 4);
        pMtx->_10 = (s16)(pAff->PC << 4);
        pMtx->_11 = (s16)(pAff->PD << 4);
    }
}

//------------------------------------------------------------------------------
// Oam管理領域が他の実体によって使用されていないか調査します。
static NNS_G2D_INLINE BOOL IsOamNotUsed_( u16* pResvTblHead, u16 from, u16 to )
{
    const u16*    pCursor   = pResvTblHead + from;
    const u16*    pEnd      = pResvTblHead + to;
    
    while( pCursor <= pEnd )
    {
        if( *pCursor != OAM_NOT_USED )
        {
            return FALSE;
        }
        pCursor++;
    }
    return TRUE;
}

//------------------------------------------------------------------------------
// 新たなGUIDを取得します.
static NNS_G2D_INLINE u16 GetNewGUID_()
{
    return numRegisterdInstance_++;
}

//------------------------------------------------------------------------------
// 領域を ownerGUID の管理する領域として、予約します.
// 
// 対象となる領域 は 他のマネージャによって使用されていない必要があります。
// IsOamNotUsed_() を使用して調べてください.
// 条件を満たさない場合 アサートに失敗します.
// 
static NNS_G2D_INLINE void DoReserveArea_( u16* pResvTblHead, u16 from, u16 to, u16 ownerGUID )
{
    NNS_G2D_NULL_ASSERT( pResvTblHead );
    NNS_G2D_ASSERT( from <= to );// MUST BE !    
    // Check if they aren't going to Reserve others area.
    NNS_G2D_ASSERT( IsOamNotUsed_( pResvTblHead, from, to ) );
    
    
    
    
    NNSI_G2D_DEBUGMSG0( "Oam Reservation occur... from %d to %d by GUID %d \n", 
                         from, 
                         to, 
                         ownerGUID );
               
               
    
    MI_CpuFill16( pResvTblHead + from, ownerGUID, sizeof(u16)*(u32)(to - from + 1) );
}

//------------------------------------------------------------------------------
// 予約領域を強制的に未使用状態へと設定します。
static NNS_G2D_INLINE void DoRestoreArea_( u16* pResvTblHead, u16 from, u16 to )
{
    NNS_G2D_NULL_ASSERT( pResvTblHead );
    NNS_G2D_ASSERT( from <= to );// MUST BE !    
    
    NNSI_G2D_DEBUGMSG0( "Oam Restoration occur... from %d to %d \n" , from, to );
    
    MI_CpuFill16( pResvTblHead + from, OAM_NOT_USED, sizeof(u16)*(u32)(to - from + 1) );
}
    
//------------------------------------------------------------------------------
// 管理領域の余分を取得します．
static NNS_G2D_INLINE u16 GetCapacity_( const NNSG2dOAMManageArea* pArea )
{
    NNS_G2D_NULL_ASSERT( pArea );
//  NNS_G2D_ASSERT( IsManageAreaValid_( pArea ) );
    
    if( IsManageAreaValid_( pArea ) )
    {
        return (u16)( (int)pArea->toIdx - pArea->currentIdx + 1);
    }else{ 
        return 0;
    }
}

//------------------------------------------------------------------------------
// 管理領域の使用数を取得します．
static NNS_G2D_INLINE u16 GetNumOamUsed_( const NNSG2dOAMManageArea* pArea )
{
    NNS_G2D_NULL_ASSERT( pArea );
//  NNS_G2D_ASSERT( IsManageAreaValid_( pArea ) );

    if( IsManageAreaValid_( pArea ) )
    {
        return (u16)( (int)pArea->currentIdx - pArea->fromIdx);
    }else{ 
        return 0;
    }
}

//------------------------------------------------------------------------------
// 管理領域が十分な余分を持つか調査します．
static NNS_G2D_INLINE BOOL HasEnoughCapacity_( const NNSG2dOAMManageArea* pArea, u16 num )
{
    NNS_G2D_NULL_ASSERT( pArea );
    NNS_G2D_ASSERT( num != 0 );
    
    return (BOOL)( GetCapacity_( pArea ) >= num );
}


//------------------------------------------------------------------------------
// Oam バッファの現在の位置へのポインタを取得します.
static NNS_G2D_INLINE GXOamAttr* GetOamCurrentPtr_( NNSG2dOamManagerInstance* pMan )
{
    NNS_G2D_NULL_ASSERT( pMan );
    NNS_G2D_ASSERT( IsManageAreaValid_( &pMan->managedAttrArea ) );
    NNS_G2D_ASSERT( pMan->managedAttrArea.toIdx < NUM_HW_OAM_ATTR );
    {            
            
        GXOamAttr* pret = GetOamBufferPtr_( pMan->type, 
                                            pMan->managedAttrArea.currentIdx );
                                            
        NNS_G2D_NULL_ASSERT( pret );
        
        return pret;
    }
}

//------------------------------------------------------------------------------
// Oam バッファの管理領域先頭の位置へのポインタを取得します.
static NNS_G2D_INLINE GXOamAttr* GetOamFromPtr_( NNSG2dOamManagerInstance* pMan )
{
    NNS_G2D_NULL_ASSERT( pMan );
    {
        GXOamAttr* pFrom = GetOamBufferPtr_( pMan->type, 
                                             pMan->managedAttrArea.fromIdx );
        NNS_G2D_NULL_ASSERT( pFrom );
        
        
        return pFrom;
    }
}

//------------------------------------------------------------------------------
// 管理領域のバイトサイズを所得します．
static NNS_G2D_INLINE u32 GetSizeOfManageArea_( const NNSG2dOAMManageArea* pArea )
{
    NNS_G2D_NULL_ASSERT( pArea );
//  NNS_G2D_ASSERT( IsManageAreaValid_( pArea ) );
    
    if( IsManageAreaValid_( pArea ) )
    {
        return (u32)(GX_OAMATTR_SIZE * (u16)(pArea->toIdx - pArea->fromIdx + 1));
    }else{
        return 0;
    }
        
}

//------------------------------------------------------------------------------
// affine paramater を 設定します
static NNS_G2D_INLINE void SetAffineParams_( NNSG2dOamType type, const MtxFx22* mtx, u16 idx )
{
    ASSERT_OAMTYPE_VALID( type );
    NNS_G2D_NULL_ASSERT( mtx );
    NNS_G2D_MINMAX_ASSERT( idx, 0, NUM_HW_OAM_AFFINE );
    
    
    {    
        GXOamAffine* pAff = GetAffineBufferPtr_( type, idx );
        NNS_G2D_NULL_ASSERT( pAff );
        
        G2_SetOBJAffine( pAff, mtx );
    }
    
}



//------------------------------------------------------------------------------
// GXOamAttr を 3DGraphicsEngine_を利用して描画します。
// 
// Affine Transform を利用する場合、Scale パラメータの 扱いについて注意が必要です。
// 
static void DrawBy3DGraphicsEngine_
( 
    const GXOamAttr*                pOam, 
    u16                             numOam, 
    const NNSG2dImageAttr*          pTexImageAttr,
    u32                             texBaseAddr,
    u32                             pltBaseAddr
)
{
    MtxFx22                 mtx;
    u16                     affineIdx;
    s16 posX;
    s16 posY;
    s16 posZ;
    
    NNS_G2D_NULL_ASSERT( pOam );
    NNS_G2D_NULL_ASSERT( pTexImageAttr );
    NNS_G2D_MINMAX_ASSERT( numOam, 0, NUM_HW_OAM_ATTR );
    
    G3_PushMtx();
    {
        int i = 0; 
        for( i = 0; i < numOam; i++ )
        {
            posX = NNSi_G2dRepeatXinScreenArea( NNSi_G2dGetOamX( &pOam[i] ) );
            posY = NNSi_G2dRepeatYinScreenArea( NNSi_G2dGetOamY( &pOam[i] ) );
            posZ = -1;
            
            // 
            // 注意:
            // ソフトウェアスプライト描画関数はカレント行列の影響を受ける
            // また、描画後のカレント行列は変更される（保存されない）。
            // 
            G3_Identity();
            
            if( NNSi_G2dIsRSEnable( &pOam[i] ) )
            {
                //
                // affine パラメータを取得する
                // 
                affineIdx = NNSi_G2dGetAffineIdx( &pOam[i] );
                NNS_G2D_MINMAX_ASSERT( affineIdx, 0, NUM_HW_OAM_AFFINE );
                GetAffineParams_( NNS_G2D_OAMTYPE_SOFTWAREEMULATION, affineIdx, &mtx );
                
                NNS_G2dDrawOneOam3DDirectWithPosAffineFast( posX, posY, posZ, &pOam[i], pTexImageAttr, texBaseAddr, pltBaseAddr, &mtx );
            }else{
                NNS_G2dDrawOneOam3DDirectWithPosFast( posX, posY, posZ, &pOam[i], pTexImageAttr, texBaseAddr, pltBaseAddr );
            }
        }
    }    
    G3_PopMtx( 1 );
}
    


//------------------------------------------------------------------------------
// Oam バッファをデフォルト値で初期化します。
static NNS_G2D_INLINE void ClearOamByDefaultValue_( NNSG2dOamType type )
{
    ASSERT_OAMTYPE_VALID( type );
    
    MI_CpuFill16( GetOamBufferPtr_( type, 0 ),                        
                  OAM_SETTING_INVISIBLE, 
                  GX_OAMATTR_SIZE * NUM_HW_OAM_ATTR );
}

//------------------------------------------------------------------------------
// Oam 管理領域予約テーブルを未使用として初期化します。
static NNS_G2D_INLINE void SetOamReservationTblNotUsed_( NNSG2dOamType type )
{
    ASSERT_OAMTYPE_VALID( type );
    
    DoRestoreArea_( GetOamReservationTblPtr_( type ),             
                    0, 
                    NUM_HW_OAM_ATTR - 1 );
}

//------------------------------------------------------------------------------
// Affine paramater 管理領域予約テーブルを未使用として初期化します。
static NNS_G2D_INLINE void SetAffineReservationTblNotUsed_( NNSG2dOamType type )
{
    ASSERT_OAMTYPE_VALID( type );
    
    DoRestoreArea_( GetAffineReservationTblPtr_( type ),             
                    0, 
                    NUM_HW_OAM_AFFINE - 1);
}
        
//------------------------------------------------------------------------------
// CpuCopy による ＯＡＭアトリビュートの転送( for 2D Graphics Engine Main )
// 小さなブロックで分割転送を行う必要が発生したため作成
static NNS_G2D_INLINE void CpuLoadOAMMain_(
    const void *pSrc,
    u32 offset,
    u32 szByte)
{
    NNS_G2D_NULL_ASSERT(pSrc);
    NNS_G2D_ASSERT(offset + szByte <= HW_OAM_SIZE);
    
    MI_CpuCopy16( pSrc, (void *)(HW_OAM + offset), szByte);
}

//------------------------------------------------------------------------------
// CpuCopy による ＯＡＭアトリビュートの転送( for 2D Graphics Engine Sub )
// 小さなブロックで分割転送を行う必要が発生したため作成
static NNS_G2D_INLINE void CpuLoadOAMSub_(
    const void *pSrc,
    u32 offset,
    u32 szByte)
{
    NNS_G2D_NULL_ASSERT(pSrc);
    NNS_G2D_ASSERT(offset + szByte <= HW_OAM_SIZE);

    MI_CpuCopy16( pSrc, (void *)(HW_DB_OAM + offset), szByte);
}

//------------------------------------------------------------------------------
// Oam の種類から適切な 転送関数ポインタを取得します
static NNS_G2D_INLINE OBJLoadFunction* GetOBJLoadFunction_( NNSG2dOamType type )
{
    static OBJLoadFunction*       funcTbl[] =
    {
        CpuLoadOAMMain_,    // for NNS_G2D_OAMTYPE_MAIN
        CpuLoadOAMSub_,     // for NNS_G2D_OAMTYPE_SUB
        NULL,               // for NNS_G2D_OAMTYPE_SOFTWAREEMULATION
        NULL,               // for NNS_G2D_OAMTYPE_INVALID
        NULL,               // for NNS_G2D_OAMTYPE_MAX
    };
    
    return funcTbl[type];
}

//------------------------------------------------------------------------------
// affine パラメータ を 2D Graphics Engine OAM  にロードします
// affine パラメータ のみを複数の転送に分割して転送します
// 
static NNS_G2D_INLINE void LoadOneAffine_( const GXOamAffine* pAff, u32 offset, OBJLoadFunction* pOBJLoadFunc )
{
    offset += OAMATTR_SIZE;
    
    (*pOBJLoadFunc)( &pAff->PA, offset + GX_OAMATTR_SIZE*0, sizeof(u16) );
    (*pOBJLoadFunc)( &pAff->PB, offset + GX_OAMATTR_SIZE*1, sizeof(u16) );
    (*pOBJLoadFunc)( &pAff->PC, offset + GX_OAMATTR_SIZE*2, sizeof(u16) );
    (*pOBJLoadFunc)( &pAff->PD, offset + GX_OAMATTR_SIZE*3, sizeof(u16) );
}

//------------------------------------------------------------------------------
// バッファの内容をDMAを使用して高速にグラフィックスエンジンに転送します
static NNS_G2D_INLINE void LoadOamAndAffineFast_( NNSG2dOamType type, u16 fromIdx, u16 toIdx )
{
    GXOamAttr* pFrom    = GetOamBufferPtr_( type, fromIdx );
    const u16  szByte   = (u16)(GX_OAMATTR_SIZE * (toIdx - fromIdx + 1) );
    u16        offset   = (u16)(GX_OAMATTR_SIZE * fromIdx);
    
    // キャッシュフラッシュ
    DC_FlushRange( pFrom, szByte );

    // DMA 転送
    switch( type )
    {
    case NNS_G2D_OAMTYPE_MAIN:
        GX_LoadOAM( pFrom, offset, szByte );
        break;
    case NNS_G2D_OAMTYPE_SUB:
        GXS_LoadOAM( pFrom, offset, szByte );
        break;
    default:
        NNS_G2D_ASSERT( FALSE );
        break;
    }
}

//------------------------------------------------------------------------------
// OAM アトリビュートを 2D Graphics Engine OAM にロードします
static NNS_G2D_INLINE void LoadOam_( NNSG2dOamType type, u16 fromIdx, u16 toIdx )
{
    GXOamAttr* pFrom    = GetOamBufferPtr_( type, fromIdx );
    const u16  numArea  = (u16)(toIdx - fromIdx + 1);
    u16        offset   = (u16)(GX_OAMATTR_SIZE * fromIdx);
    u16         i;        
    OBJLoadFunction* pOBJLoadFunc = GetOBJLoadFunction_( type );
    
    
    for( i = 0; i < numArea; i++ ) 
    {    
        //
        // OAM アトリビュート部分のみを転送します
        // 
        (*pOBJLoadFunc)( pFrom, offset, OAMATTR_SIZE );
        
        offset += GX_OAMATTR_SIZE;
        pFrom++;
    }
}

//------------------------------------------------------------------------------
// affine パラメータ を 2D Graphics Engine OAM にロードします
static NNS_G2D_INLINE void LoadAffine_( NNSG2dOamType type, u16 fromIdx, u16 toIdx )
{

    GXOamAffine* pFrom      = GetAffineBufferPtr_( type, fromIdx );
    const u16  numArea      = (u16)(toIdx - fromIdx + 1);
    u16        offset       = (u16)(GX_AFFINE_SIZE * fromIdx);
    u16         i;        
    OBJLoadFunction* pOBJLoadFunc = GetOBJLoadFunction_( type );
    
    for( i = 0; i < numArea; i++ ) 
    {    
        //
        // アフィンパラメータ部分のみを転送します
        // 
        LoadOneAffine_( pFrom, offset, pOBJLoadFunc );
       
        offset += GX_AFFINE_SIZE;
        pFrom++;
    }
}
        
//------------------------------------------------------------------------------
// OAM アトリビュートバッファを初期値でリセットします
static NNS_G2D_INLINE void ResetOam_( NNSG2dOamType type, u16 fromIdx, u16 toIdx )
{
    GXOamAttr* pFrom    = GetOamBufferPtr_( type, fromIdx );
    const u16  numArea  = (u16)(toIdx - fromIdx + 1);
    u16         i;        
    
    for( i = 0; i < numArea; i++ ) 
    {    
        //
        // OAM アトリビュート部分のみをデフォルト値でフィルします
        //
        //MI_CpuFill16( pFrom, OAM_SETTING_INVISIBLE, sizeof( u16 ) );
        *((u16*)pFrom) = OAM_SETTING_INVISIBLE;
        pFrom++;
    }
}

//------------------------------------------------------------------------------
// affine パラメータ バッファを初期値でリセットします
static NNS_G2D_INLINE void ResetAffine_( NNSG2dOamType type, u16 fromIdx, u16 toIdx )
{

    GXOamAffine* pFrom      = GetAffineBufferPtr_( type, fromIdx );
    const u16  numArea      = (u16)(toIdx - fromIdx + 1);
    u16         i;        
    
    for( i = 0; i < numArea; i++ ) 
    {    
        //
        // アフィンパラメータ部分のみをデフォルト値でフィルします
        // 
        pFrom->PA = OAM_SETTING_INVISIBLE;
        pFrom->PB = OAM_SETTING_INVISIBLE;
        pFrom->PC = OAM_SETTING_INVISIBLE;
        pFrom->PD = OAM_SETTING_INVISIBLE;
        pFrom++;
    }
}

//------------------------------------------------------------------------------
// ライブラリ内部公開
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void* NNSi_G2dGetOamManagerInternalBufferForDebug( NNSG2dOamType type )
{
    return (void*)GetOamBufferPtr_( type, 0 ); 
}




//------------------------------------------------------------------------------
// 外部公開
//------------------------------------------------------------------------------



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dInitOamManagerModule

  Description:  OamManager モジュール を 初期化する
                すべての OamManager モジュール メソッドの実行より先に呼び出してください。
                
                内部で
                HW Oam のメモリフィル
                Oam予約テーブルの初期化を行います
                
                
  Arguments:    なし
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dInitOamManagerModule()
{  
    // Oamバッファ
    ClearOamByDefaultValue_( NNS_G2D_OAMTYPE_MAIN );
    ClearOamByDefaultValue_( NNS_G2D_OAMTYPE_SUB );
    ClearOamByDefaultValue_( NNS_G2D_OAMTYPE_SOFTWAREEMULATION );
    
    NNSI_G2D_DEBUGMSG0("Initialize OamBuffer ... done.\n");
    
    // OamAttr予約テーブル
    SetOamReservationTblNotUsed_( NNS_G2D_OAMTYPE_MAIN );
    SetOamReservationTblNotUsed_( NNS_G2D_OAMTYPE_SUB );
    SetOamReservationTblNotUsed_( NNS_G2D_OAMTYPE_SOFTWAREEMULATION );
    
    NNSI_G2D_DEBUGMSG0("Initialize Oam reservation table ... done.\n");
    
    // affineパラメータ予約テーブル
    SetAffineReservationTblNotUsed_( NNS_G2D_OAMTYPE_MAIN );
    SetAffineReservationTblNotUsed_( NNS_G2D_OAMTYPE_SUB );
    SetAffineReservationTblNotUsed_( NNS_G2D_OAMTYPE_SOFTWAREEMULATION );
    
    NNSI_G2D_DEBUGMSG0("Initialize Oam affine reservation table ... done.\n");
        
    
    NNSI_G2D_DEBUGMSG0("Initialize OamManager ... done.\n");
}


/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetNewManagerInstance

  Description:  OamManager の 実体を初期化する
                
                要求領域がすでに他の実体によって使用されている場合は、
                初期化に失敗しFALSEを返します。
                
  
  
  Arguments:    pMan:       [OUT] マネージャ実体
                from:       [IN]  使用するOam（開始番号）
                to:         [IN]  使用するOam（終了番号）
                type:       [IN]  Oamの種類
                
                from <= to を満たす必要があります。
                
  Returns:      初期化の成否
  
 *---------------------------------------------------------------------------*/
BOOL NNS_G2dGetNewManagerInstance
( 
    NNSG2dOamManagerInstance*   pMan, 
    u16                         from, 
    u16                         to, 
    NNSG2dOamType               type 
)
{
    ASSERT_OAMTYPE_VALID( type );
    NNS_G2D_ASSERT( from <= to );// MUST BE !    
    NNS_G2D_ASSERT( to < NUM_HW_OAM_ATTR );
    NNS_G2D_NULL_ASSERT( pMan );
    
    
    {
        u16*        pReserveTbl = GetOamReservationTblPtr_( type );
        NNS_G2D_NULL_ASSERT( pReserveTbl );
        
        //
        // 指定された領域が未使用であれば...
        //
        if( IsOamNotUsed_( pReserveTbl, from, to ) )
        {
            //
            // 実体へのパラメータの代入を行う
            //
            pMan->GUID                          = GetNewGUID_();
            pMan->managedAttrArea.fromIdx       = from;
            pMan->managedAttrArea.toIdx         = to;
            pMan->managedAttrArea.currentIdx    = from;
            //
            // アフィンパラメータを管理しない様に設定
            //
            pMan->managedAffineArea.fromIdx    = NUM_HW_OAM_AFFINE;
            pMan->managedAffineArea.toIdx      = 0;
            
            
            pMan->type                          = type;
            pMan->bFastTransferEnable           = FALSE;
            
            DoReserveArea_( pReserveTbl, from, to, pMan->GUID );
            
            return TRUE;
        }else{
            
            OS_Warning("Failure in NNS_G2dGetNewManagerInstance().\n The manageArea that you specified has been used by someone.");
            return FALSE;
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dInitManagerInstanceAffine

  Description:  OamManager の 実体 Affine 管理部分 を初期化する
                
                要求領域がすでに他の実体によって使用されている場合は、
                初期化に失敗しFALSEを返します。
                
  
  
  Arguments:    pMan:       [OUT] マネージャ実体
                from:       [IN]  使用するAffine（開始番号）
                to:         [IN]  使用するAffine（終了番号）
                
                from <= to を満たす必要があります。
                
  Returns:      初期化の成否
  
 *---------------------------------------------------------------------------*/
BOOL NNS_G2dInitManagerInstanceAffine
( 
    NNSG2dOamManagerInstance*   pMan, 
    u16                         from, 
    u16                         to 
)
{
    NNS_G2D_NULL_ASSERT( pMan );
    NNS_G2D_ASSERT( from <= to );
	NNS_G2D_ASSERT( to < NUM_HW_OAM_AFFINE );
    // 初期化済みか？
    
    {
        u16*        pReserveTbl = GetAffineReservationTblPtr_( pMan->type );
        NNS_G2D_NULL_ASSERT( pReserveTbl );
        
        //
        // 指定された領域が未使用であれば...
        //
        if( IsOamNotUsed_( pReserveTbl, from, to ) )
        {
            //
            // 実体へのパラメータの代入を行う
            //
            pMan->managedAffineArea.fromIdx    = from;
            pMan->managedAffineArea.toIdx      = to;
            pMan->managedAffineArea.currentIdx = from;
            pMan->bFastTransferEnable          = FALSE;
            
            DoReserveArea_( pReserveTbl, from, to, pMan->GUID );
            
            return TRUE;
        }else{
            OS_Warning("Failure in NNS_G2dInitManagerInstanceAffine().\n The manageArea that you specified has been used by someone.");
            return FALSE;
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetNewOamManagerInstance

  Description:  OamManager の 実体を初期化します
                
                OAMマネージャの初期化をする新しいAPIです。
                
  Arguments:    pMan:           [OUT] マネージャ実体
                fromOBJ:        [IN]  使用するOam（開始番号）
                numOBJ:         [IN]  使用するOam個数(非ゼロ値)
                fromAffine:     [IN]  使用するAffine（開始番号）
                numAffine:      [IN]  使用するアフィン個数(ゼロも可)
                type:           [IN]  Oamの種類
                
                
  Returns:      初期化の成否
  
 *---------------------------------------------------------------------------*/
BOOL NNS_G2dGetNewOamManagerInstance
( 
    NNSG2dOamManagerInstance*   pMan, 
    u16                         fromOBJ, 
    u16                         numOBJ, 
    u16                         fromAffine, 
    u16                         numAffine, 
    NNSG2dOamType               type 
)
{
    ASSERT_OAMTYPE_VALID( type );
    NNS_G2D_NON_ZERO_ASSERT( numOBJ );// MUST BE !    
    NNS_G2D_ASSERT( numOBJ <= NUM_HW_OAM_ATTR );
	NNS_G2D_ASSERT( numAffine <= NUM_HW_OAM_AFFINE );
    NNS_G2D_NULL_ASSERT( pMan );
    //
    // OBJ について
    //
    {
        u16*        pReserveTbl = GetOamReservationTblPtr_( type );
        const u16   toOBJ       = (u16)(fromOBJ + (numOBJ - 1));
        
        NNS_G2D_NULL_ASSERT( pReserveTbl );
        //
        // 指定された領域が未使用であれば...
        //
        if( IsOamNotUsed_( pReserveTbl, fromOBJ, toOBJ ) )
        {
            //
            // 実体へのパラメータの代入を行う
            //
            pMan->GUID                          = GetNewGUID_();
            pMan->managedAttrArea.fromIdx       = fromOBJ;
            pMan->managedAttrArea.toIdx         = toOBJ;
            pMan->managedAttrArea.currentIdx    = fromOBJ;
            
            DoReserveArea_( pReserveTbl, fromOBJ, toOBJ, pMan->GUID );
            
        }else{
            
            OS_Warning("Failure in NNS_G2dGetNewManagerInstance().\n The manageArea that you specified has been used by someone.");
            return FALSE;
        }
    }
    
    //
    // アフィンパラメータについて
    //
    {
        u16*        pReserveTbl = GetAffineReservationTblPtr_( type );
        NNS_G2D_NULL_ASSERT( pReserveTbl );
        
        
        if( numAffine == 0 )
        {
            //
            // アフィンパラメータ を使用しない場合、不正なデータを挿入する
            // 不正なデータの挿入は重要である。（モジュール内部では不正な管理領域設定を判定して、処理を行う。）
            //
            pMan->managedAffineArea.fromIdx    = NUM_HW_OAM_AFFINE;
            pMan->managedAffineArea.toIdx      = 0;
            pMan->managedAffineArea.currentIdx = pMan->managedAffineArea.fromIdx;
            
        }else{
            //
            // アフィンパラメータ を使用する場合
            //
            const u16   toAffine       = (u16)(fromAffine + (numAffine - 1));
            //
            // 指定された領域が未使用であれば...
            //
            if( IsOamNotUsed_( pReserveTbl, fromAffine, toAffine ) )
            {
                pMan->managedAffineArea.fromIdx    = fromAffine;
                pMan->managedAffineArea.toIdx      = toAffine;
                pMan->managedAffineArea.currentIdx = fromAffine;
                
                DoReserveArea_( pReserveTbl, fromAffine, toAffine, pMan->GUID );
                
            }else{
                OS_Warning("Failure in NNS_G2dGetNewManagerInstanceNew().\n The manageArea that you specified has been used by someone.");
                return FALSE;
            }
        }
    }
    
    pMan->bFastTransferEnable           = FALSE;
    pMan->type                          = type;
    
    //
    // 滞りなく初期化が完了した
    //
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetNewOamManagerInstanceAsFastTransferMode

  Description:  OamManager の 実体を初期化します
                
                OAMマネージャの初期化をする新しいAPIです。
                
                確保する領域に制限が存在します。
                また、管理アフィンパラメータ領域は管理OAMアトリビュート領域内に含まれる、個数分だけ
                割り当てられます。(例：OAM 0 - 32 番 => アフィンパラメータ 0 - 8 番 )
                
                バッファからグラフィックスエンジンへのデータ転送の際に
                高速転送が可能となっています。
                
  Arguments:    pMan:           [OUT] マネージャ実体
                fromOBJ:        [IN]  使用するOam（開始番号：4の倍数である必要があります）
                numOBJ:         [IN]  使用するOam個数(非ゼロ値：：4の倍数である必要があります)
                type:           [IN]  Oamの種類
                
                
  Returns:      初期化の成否
  
 *---------------------------------------------------------------------------*/
BOOL NNS_G2dGetNewOamManagerInstanceAsFastTransferMode
( 
    NNSG2dOamManagerInstance*   pMan, 
    u16                         fromOBJ, 
    u16                         numOBJ,
    NNSG2dOamType               type 
)
{
    ASSERT_OAMTYPE_VALID( type );
    NNS_G2D_NON_ZERO_ASSERT( numOBJ );// MUST BE !    
    NNS_G2D_ASSERT( numOBJ <= NUM_HW_OAM_ATTR );
    NNS_G2D_NULL_ASSERT( pMan );
    NNS_G2D_ASSERT( fromOBJ % 4 == 0 );
    NNS_G2D_ASSERT( numOBJ % 4 == 0 );
    //
    // OBJ について
    //
    {
        u16*        pReserveTbl = GetOamReservationTblPtr_( type );
        const u16   toOBJ       = (u16)(fromOBJ + (numOBJ - 1));
        
        NNS_G2D_NULL_ASSERT( pReserveTbl );
        //
        // 指定された領域が未使用であれば...
        //
        if( IsOamNotUsed_( pReserveTbl, fromOBJ, toOBJ ) )
        {
            //
            // 実体へのパラメータの代入を行う
            //
            pMan->GUID                          = GetNewGUID_();
            pMan->managedAttrArea.fromIdx       = fromOBJ;
            pMan->managedAttrArea.toIdx         = toOBJ;
            pMan->managedAttrArea.currentIdx    = fromOBJ;
            
            DoReserveArea_( pReserveTbl, fromOBJ, toOBJ, pMan->GUID );
            
        }else{
            
            OS_Warning("Failure in NNS_G2dGetNewOamManagerInstanceAsFastTransferMode().\n The manageArea that you specified has been used by someone.");
            return FALSE;
        }
    }
    
    //
    // アフィンパラメータについて
    //
    {
        const u16   fromAffine     = (u16)(fromOBJ / 4);
        const u16   numAffine      = (u16)(numOBJ / 4);
        const u16   toAffine       = (u16)(fromAffine + (numAffine - 1));
        
        u16*        pReserveTbl = GetAffineReservationTblPtr_( type );
        NNS_G2D_NULL_ASSERT( pReserveTbl );        
        //
        // 指定された領域が未使用であれば...
        //
        if( IsOamNotUsed_( pReserveTbl, fromAffine, toAffine ) )
        {
            pMan->managedAffineArea.fromIdx    = fromAffine;
            pMan->managedAffineArea.toIdx      = toAffine;
            pMan->managedAffineArea.currentIdx = fromAffine;
            
            DoReserveArea_( pReserveTbl, fromAffine, toAffine, pMan->GUID );
            
        }else{
            OS_Warning("Failure in NNS_G2dGetNewManagerInstanceNew().\n The manageArea that you specified has been used by someone.");
            return FALSE;
        }
    }
    
    //
    // 高速転送可能フラグをセットする
    //
    pMan->bFastTransferEnable           = TRUE;
    pMan->type                          = type;
    
    //
    // 滞りなく初期化が完了した
    //
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dEntryOamManagerOam

  Description:  Oam を 登録します。
                要求された登録のための十分なOamを持たない場合は、
                何もせずFALSEを返します。
                
  Arguments:    pMan:       [OUT] マネージャ実体
                pOam:       [IN]  登録 する Oam の先頭ポインタ
                num:        [IN]  登録 する Oam 数
                
  Returns:      登録の成否
  
 *---------------------------------------------------------------------------*/
BOOL NNS_G2dEntryOamManagerOam
( 
    NNSG2dOamManagerInstance*   pMan, 
    const GXOamAttr*            pOam, 
    u16                         num 
)
{
    NNS_G2D_NULL_ASSERT( pMan );
    NNS_G2D_NULL_ASSERT( pOam );
    NNS_G2D_ASSERT( num != 0 );
    
    //
    // 十分な容量があれば...
    //
    if( HasEnoughCapacity_( &pMan->managedAttrArea, num ) )
    {
        //
        // データをバッファにコピーする
        // affine paramater を 上書きしないように、メモリコピー を 分割して行う。
        //     
        int i = 0;
        GXOamAttr* pOamAttr = GetOamCurrentPtr_( pMan );
        for( i = 0; i < num; i ++ )
        {    
            //MI_CpuCopy16( pOam, pOamAttr, OAMATTR_SIZE );
            pOamAttr[i].attr0 = pOam->attr0;
            pOamAttr[i].attr1 = pOam->attr1;
            pOamAttr[i].attr2 = pOam->attr2;
            
            pMan->managedAttrArea.currentIdx++;
            pOam++;
        }
        
        //
        // デバック出力
        //
        NNSI_G2D_DEBUGMSG1( "New Oam entry occur...  from %d to %d for GUID %d\n",
                             pMan->currentPos, 
                             pMan->currentPos + num, 
                             pMan->GUID );
        
        return TRUE;
    }else{
        NNSI_G2D_DEBUGMSG0("The OamBuffer has no capacity enough to store new Oam.");
        return FALSE;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dEntryOamManagerOamWithAffineIdx

  Description:  OAM アトリビュートパラメータ を affine index を指定して 登録します。
                要求された登録のための十分な容量を持たない場合は、
                何もせずFALSEを返します。
                
  Arguments:    pMan:       [OUT] マネージャ実体
                pOam:       [IN]  登録 する Affine Matrix 
                affineIdx:  [IN]  OBJ の 参照する Affine index 
                
  Returns:      登録の成否
  
 *---------------------------------------------------------------------------*/
BOOL NNS_G2dEntryOamManagerOamWithAffineIdx
( 
    NNSG2dOamManagerInstance*   pMan, 
    const GXOamAttr*            pOam, 
    u16                         affineIdx 
)
{
    NNS_G2D_NULL_ASSERT( pMan );
    NNS_G2D_NULL_ASSERT( pOam );
   
    
    //
    // 十分な容量があれば...
    //
    if( HasEnoughCapacity_( &pMan->managedAttrArea, 1 ) )
    {
        GXOamAttr* pOamAttr = GetOamCurrentPtr_( pMan );
        
        
        // MI_CpuCopy16( pOam, pOamAttr, OAMATTR_SIZE );
        pOamAttr->attr0 = pOam->attr0;
        pOamAttr->attr1 = pOam->attr1;
        pOamAttr->attr2 = pOam->attr2;
        
        //
        // affine Index が指定されていたら...
        //
        if( NNS_G2D_OAM_AFFINE_IDX_NONE != affineIdx )
        {
            
            // G2_SetOBJEffect( pOamAttr, GX_OAM_EFFECT_AFFINE_DOUBLE, affineIdx );
            {
                
                // 回転拡大縮小イネーブルフラグのチェック
                // アフィン変換有効フラグが有効に設定されていない場合
                // oamAffinIdx を設定しません 
                // 以前はアサートで強制終了していました。
                //
                // NNS_G2D_ASSERT( pOamAttr->rsMode & 0x1 );
                //
                if( pOamAttr->rsMode & 0x1 )                
                {
                    pOamAttr->rsParam = affineIdx;
                }
            }
        }
        
        pMan->managedAttrArea.currentIdx++;
        return TRUE;
    }else{
        NNSI_G2D_DEBUGMSG0("The OamBuffer has no capacity enough to store new Oam.");
        return FALSE;
    }
}


/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetOamManagerAffine

  Description:  Oam Affine パラメータ を index を指定して 登録します。
                不正なOAM種類、インデックスを指定した場合はアサートに失敗します。
                
                アフィンパラメータ 管理領域 への書き込みがあった場合は、警告を表示します。
                
  Arguments:    type:      [OUT] OAM種類
                mtx:       [IN]  登録 する Affine Matrix 
                idx:       [IN]  Affine を 登録 する index 
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetOamManagerAffine
( 
    NNSG2dOamType               type, 
    const MtxFx22*              mtx, 
    u16                         idx 
)
{
    ASSERT_OAMTYPE_VALID( type );
    NNS_G2D_NULL_ASSERT( mtx );
    NNS_G2D_MINMAX_ASSERT( idx, 0, NUM_HW_OAM_AFFINE );
    
    
    SDK_WARNING( IsOamNotUsed_( GetAffineReservationTblPtr_( type ), idx, idx ),
                "An invalid affine param setting to the managed area is detected. index = %d", idx );

    
    SetAffineParams_( type, mtx, idx );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dEntryOamManagerAffine

  Description:  Oam Affine パラメータ 登録します。
                登録後 AffineパラメータIndexを返します。
                要求された登録のための十分な容量を持たない場合は、
                何もせず NNS_G2D_OAM_AFFINE_IDX_NONE を返します。
                
                初期化時に、アフィンパラメータを割り当てていない場合はアサートに失敗します。
                
  Arguments:    pMan:      [OUT] マネージャ実体
                mtx:       [IN]  登録 する Affine Matrix 
                
  Returns:      AffineパラメータIndex
  
 *---------------------------------------------------------------------------*/
u16 NNS_G2dEntryOamManagerAffine( NNSG2dOamManagerInstance* pMan, const MtxFx22* mtx )
{
    NNS_G2D_NULL_ASSERT( pMan );
    NNS_G2D_NULL_ASSERT( mtx );
    NNS_G2D_ASSERT( IsManageAreaValid_( &pMan->managedAffineArea ) );
    
    //
    // 十分な容量があれば...
    //
    if( HasEnoughCapacity_( &pMan->managedAffineArea, 1 ) )
    {
        const u16 currentAffineIdx = pMan->managedAffineArea.currentIdx;

        SetAffineParams_( pMan->type, mtx, currentAffineIdx );
        
        pMan->managedAffineArea.currentIdx++;
        
        return currentAffineIdx;
    }else{
        NNSI_G2D_DEBUGMSG0("The OamBuffer has no capacity enough to store new Affine Paramater.");
        return NNS_G2D_OAM_AFFINE_IDX_NONE;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dApplyOamManagerToHW

  Description:  OamManager 内部buffer を HW OAM に適用します
                描画中の画面に影響を与えずに反映を完了するためには、
                描画ブランクに実行する必要があります。
                
  Arguments:    pMan:                [OUT] マネージャ実体
                
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/     
void NNS_G2dApplyOamManagerToHW( NNSG2dOamManagerInstance* pMan )
{
    NNS_G2D_NULL_ASSERT( pMan );
    NNS_G2D_ASSERTMSG( pMan->type != NNS_G2D_OAMTYPE_SOFTWAREEMULATION, 
                    " For the NNS_G2D_OAMTYPE_SOFTWAREEMULATION type Manager, Use NNS_G2dApplyOamManagerToHWSprite() instead." );
    {
        //
        // 高速転送(OAMAttrとアフィンパラメータの一括転送)が可能か？
        //
        if( pMan->bFastTransferEnable )
        {
            LoadOamAndAffineFast_( pMan->type,
                                   pMan->managedAttrArea.fromIdx, 
                                   pMan->managedAttrArea.toIdx );
        }else{
            // OAM Attr
            LoadOam_    ( pMan->type, 
                          pMan->managedAttrArea.fromIdx, 
                          pMan->managedAttrArea.toIdx );
            // affine params
            if( IsManageAreaValid_( &pMan->managedAffineArea ) )
            {    
                LoadAffine_ ( pMan->type, 
                              pMan->managedAffineArea.fromIdx, 
                              pMan->managedAffineArea.toIdx );
            }
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dApplyOamManagerToHWSprite 

  Description:  3D Graphics Engine を 使用して マネージャの内容を描画します
                
  Arguments:    pMan          :      [OUT] マネージャ実体
                pTexImageAttr :      [IN]  VRAM テクスチャイメージの属性
                texBaseAddr   :      [IN]  VRAM ベースアドレス
                pltBaseAddr   :      [IN]  パレット ベースアドレス
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/     
void NNS_G2dApplyOamManagerToHWSprite
( 
    NNSG2dOamManagerInstance*       pMan, 
    const NNSG2dImageAttr*          pTexImageAttr,
    u32                             texBaseAddr,
    u32                             pltBaseAddr
)
{
    NNS_G2D_NULL_ASSERT( pMan );
    NNS_G2D_NULL_ASSERT( pTexImageAttr );
    
    
    NNS_G2D_ASSERTMSG( pMan->type == NNS_G2D_OAMTYPE_SOFTWAREEMULATION, 
        " For the NNS_G2D_OAMTYPE_MAIN SUB type Manager, Use NNS_G2dApplyOamManagerToHW() instead." );
    
    if( pMan->spriteZoffsetStep != 0 )
    {
        fx32 step = NNSi_G2dGetOamSoftEmuAutoZOffsetStep();
        NNSi_G2dSetOamSoftEmuAutoZOffsetFlag( TRUE );
        NNSi_G2dSetOamSoftEmuAutoZOffsetStep( pMan->spriteZoffsetStep );
        
        {
            void*       pFrom   = GetOamFromPtr_( pMan );
            const u16   numOam  = GetNumOamUsed_( &pMan->managedAttrArea );
            NNS_G2D_NULL_ASSERT( pFrom );
            
            
            DrawBy3DGraphicsEngine_( pFrom, 
                                     numOam, 
                                     pTexImageAttr,
                                     texBaseAddr,
                                     pltBaseAddr );
        }
        
        NNSi_G2dSetOamSoftEmuAutoZOffsetStep( step );
        NNSi_G2dSetOamSoftEmuAutoZOffsetFlag( FALSE );
        NNSi_G2dResetOamSoftEmuAutoZOffset();
    }else{
        {
           void*       pFrom   = GetOamFromPtr_( pMan );
           const u16   numOam  = GetNumOamUsed_( &pMan->managedAttrArea );
           NNS_G2D_NULL_ASSERT( pFrom );
           
           
           DrawBy3DGraphicsEngine_( pFrom, 
                                    numOam, 
                                    pTexImageAttr,
                                    texBaseAddr,
                                    pltBaseAddr );
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dResetOamManagerBuffer

  Description:  OamManager 内部 buffer を リセットします。
                
  Arguments:    pMan:                [OUT] マネージャ実体
                
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/ 
void NNS_G2dResetOamManagerBuffer( NNSG2dOamManagerInstance* pMan )
{
    NNS_G2D_NULL_ASSERT( pMan );

    // デフォルト値でリセット
    {
        //
        // 高速転送可能状態ならば
        //
        if( pMan->bFastTransferEnable )
        {    
            //
            // DMA によって一気にメモリフィルを行う
            //            
            const u32 szByte = GetSizeOfManageArea_( &pMan->managedAttrArea );
            void* pData = GetOamFromPtr_( pMan );
            NNS_G2D_ASSERT( szByte != 0 );
            
            DC_InvalidateRange( pData, szByte );
            NNSi_G2dDmaFill32( NNS_G2D_DMA_NO, pData, OAM_SETTING_INVISIBLE, szByte );
            
        }else{    
            //
            // CPU コピーによって細切れにメモリフィルを行う
            //
            // OAM Attr
            ResetOam_( pMan->type, 
                       pMan->managedAttrArea.fromIdx, 
                       pMan->managedAttrArea.toIdx );
            
            //
            // アフィンパラメータのリセットは必要ないはずなので、行わない様に変更しました。
            //
            /*
            // affine params
            if( IsManageAreaValid_( &pMan->managedAffineArea ) )
            {
                ResetAffine_( pMan->type, 
                              pMan->managedAffineArea.fromIdx, 
                              pMan->managedAffineArea.toIdx );
            }
            */
        }
    }
    
    // カウンターをリセットする
    {
        pMan->managedAttrArea.currentIdx    = pMan->managedAttrArea.fromIdx;
        pMan->managedAffineArea.currentIdx  = pMan->managedAffineArea.fromIdx;
    }
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dApplyAndResetOamManagerBuffer

  Description:  OamManager 内部 buffer を HW OAM に適用した後、
                OamManager 内部 buffer を リセットします。
                
  Arguments:    pMan:                [OUT] マネージャ実体
                
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/    
void NNS_G2dApplyAndResetOamManagerBuffer( NNSG2dOamManagerInstance* pMan )
{
    NNS_G2D_NULL_ASSERT( pMan );
    
    NNS_G2dApplyOamManagerToHW( pMan );
    NNS_G2dResetOamManagerBuffer( pMan );
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetOamManagerOamCapacity, NNS_G2dGetOamManagerAffineCapacity

  Description:  利用可能なリソースの個数を取得します
                
  Arguments:    pMan:                [IN]  マネージャ実体
                
                
  Returns:      利用可能なリソースの個数
  
 *---------------------------------------------------------------------------*/
u16     NNS_G2dGetOamManagerOamCapacity( NNSG2dOamManagerInstance* pMan )
{
    NNS_G2D_NULL_ASSERT( pMan );
    return GetCapacity_( &pMan->managedAttrArea );
}

//------------------------------------------------------------------------------
u16     NNS_G2dGetOamManagerAffineCapacity( NNSG2dOamManagerInstance* pMan )
{
    NNS_G2D_NULL_ASSERT( pMan );
    return GetCapacity_( &pMan->managedAffineArea );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetOamBuffer

  Description:  OAMマネージャモジュール内部バッファの参照を取得します。
                
  Arguments:    type:                [IN]  OAMバッファの種類
                
                
  Returns:      内部バッファへのポインタ
  
 *---------------------------------------------------------------------------*/
GXOamAttr* NNS_G2dGetOamBuffer( NNSG2dOamType type )
{
    return GetOamBufferPtr_( type, 0 );    
}



