/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g2d
  File:     g2d_RendererCore.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.17 $
 *---------------------------------------------------------------------------*/
#ifndef NNS_G2D_RENDERERCORE_H_
#define NNS_G2D_RENDERERCORE_H_


#include <nitro.h>

#include <nnsys/g2d/g2d_config.h>
#include <nnsys/g2d/fmt/g2d_Vec_data.h>

#include <nnsys/g2d/fmt/g2d_Oam_data.h>
#include <nnsys/g2d/fmt/g2d_Cell_data.h>



#ifdef __cplusplus
extern "C" {
#endif

#define MtxCache_NOT_AVAILABLE                  0xFFFF
#define MtxCache_NOT_AVAILABLE_ForMemFill       0xFFFFFFFF // メモリフィル用
#define NNS_G2D_NUMBER_OF_2DGRAPHICS_ENGINE     2
#define NNS_G2D_RNDCORE_INTERNAL_OAMBUFFER_SIZE 128
#define NNS_G2D_OAMFLIP_PATTERN_NUM             0x04 // (OAM_FLIP_NONE, OAM_FLIP_H, OAM_FLIP_V, OAM_FLIP_HV )
    

/*---------------------------------------------------------------------------*
  Name:         NNSG2dRendererAffineTypeOverwiteMode

  Description:  OAMアトリビュートのアフィン設定部分の上書き属性
 *---------------------------------------------------------------------------*/ 
typedef enum NNSG2dRendererAffineTypeOverwiteMode
{
    NNS_G2D_RND_AFFINE_OVERWRITE_NONE,  // 上書きしない
    NNS_G2D_RND_AFFINE_OVERWRITE_NORMAL,// 通常のアフィン変換方式に設定
    NNS_G2D_RND_AFFINE_OVERWRITE_DOUBLE // 倍角アフィン変換方式に設定
    
}NNSG2dRendererAffineTypeOverwiteMode;

/*---------------------------------------------------------------------------*
  Name:         NNSG2dSurfaceType

  Description:  レンダリング対象
                
 *---------------------------------------------------------------------------*/
// enum 値 は 変更してはならない 
// 内部動作に関連します。
typedef enum NNSG2dSurfaceType
{
    NNS_G2D_SURFACETYPE_MAIN3D = 0x00,  // 3D グラフィックスエンジン   を使用
    NNS_G2D_SURFACETYPE_MAIN2D = 0x01,  // 2D グラフィックスエンジン A を使用
    NNS_G2D_SURFACETYPE_SUB2D  = 0x02,  // 2D グラフィックスエンジン B を使用
    NNS_G2D_SURFACETYPE_MAX    = 0x03
    
}NNSG2dSurfaceType;

/*---------------------------------------------------------------------------*
  Name:         NNSG2dRendererFlip

  Description:  レンダラ、フリップ描画情報
                
 *---------------------------------------------------------------------------*/
typedef enum NNSG2dRendererFlip
{
    NNS_G2D_RENDERERFLIP_NONE   = 0x00,
    NNS_G2D_RENDERERFLIP_H      = 0x01,
    NNS_G2D_RENDERERFLIP_V      = 0x02
    
}NNSG2dRendererFlip;


//------------------------------------------------------------------------------
// 実際の 2D Graphics Enigine への Affine パラメータ登録を キャシュする役割を果たします。
// 重複する 登録は 実行されず過去の登録結果が返されます。
// 
// これによって、同一 NNSG2dRndCore2DMtxCache Index を 指定した場合 
// 同一 Affine パラメータ を参照することとなります。
//
// affineIndex は 規定値で Affine パラメータ登録 がなされていないという意味の 
// MtxCache_NOT_AVAILABLE を持ちます。
// 
typedef struct NNSG2dRndCore2DMtxCache
{
    MtxFx22         m22;
    u16             affineIndex[NNS_G2D_OAMFLIP_PATTERN_NUM][NNS_G2D_NUMBER_OF_2DGRAPHICS_ENGINE];
}NNSG2dRndCore2DMtxCache;


/*---------------------------------------------------------------------------*
  Name:         NNSG2dViewRect

  Description:  可視領域をあらわす矩形
 *---------------------------------------------------------------------------*/ 
typedef struct NNSG2dViewRect
{
    NNSG2dFVec2            posTopLeft;      // 可視領域 左上位置
    NNSG2dFVec2            sizeView;        // ビューのサイズ
    
}NNSG2dViewRect;


 /*---------------------------------------------------------------------------*
  Name:         NNSG2dOamRegisterFunction

  Description:  Oam 登録に使用される関数ポインタ
                
  Arguments:    pOam :                  新しく登録する Oamアトリビュート
                affineIndex :           affine インデックス
                                        ( 32 以上の場合もあります )
                                        (未使用の場合は
                                        NNS_G2D_OAM_AFFINE_IDX_NONEを指定します)
                                        
                bDoubleAffine :         倍角affineモードで描画するか
                                        (現在は使用されていません)
                
  Returns:      登録の成否
  
 *---------------------------------------------------------------------------*/
typedef BOOL (*NNSG2dOamRegisterFunction)     
( 
    const GXOamAttr*    pOam, 
    u16                 affineIndex, 
    BOOL                bDoubleAffine 
);

typedef BOOL (*NNSG2dOamBlockRegisterFunction)     
( 
    const GXOamAttr*    pOam, 
    u16                 num 
);

                         
/*---------------------------------------------------------------------------*
  Name:         NNSG2dAffineRegisterFunction
  
  Description:  affine パラメータ 登録に使用される関数ポインタ
  
  Arguments:    mtx :                  affine変換行列
               
  Returns:      登録されたaffineIndex( 32 以上の場合もあります )
  
 *---------------------------------------------------------------------------*/ 
typedef u16 (*NNSG2dAffineRegisterFunction)   
( 
    const MtxFx22* mtx 
);


struct NNSG2dRndCoreInstance;
struct NNSG2dRndCoreSurface;

typedef void(*NNSG2dRndCoreDrawCellCallBack)
(
    struct NNSG2dRndCoreInstance*   pRend,
    const NNSG2dCellData*           pCell
);

/*---------------------------------------------------------------------------*
  Name:         NNSG2dRndDrawOamCallBack
  
  Description:  セル 内のOamアトリビュート描画時に呼び出されるコールバック
                （描画前と描画後に呼び出される）
  
  Arguments:    pRend   : レンダラへのポインタ
                pSurface: レンダサーフェスへのポインタ
                pCell   : セル
                pMtx    : 変換行列
               
  Returns:      無し
  
 *---------------------------------------------------------------------------*/ 
typedef void(*NNSG2dRndCoreDrawOamCallBack)
(
    struct NNSG2dRndCoreInstance*   pRend,
    const NNSG2dCellData*           pCell,
    u16                             oamIdx
);

/*---------------------------------------------------------------------------*
  Name:         NNSG2dRndCoreOamOverwriteCallBack
  
  Description:  Oam パラメータを上書きするためのコールバック
                本コールバックは Oamアトリビュート描画時 コールバック
                NNSG2dRndDrawOamCallBackと統合され、廃止されました。
                
                NNSG2dRndDrawOamCallBack中で、
                pRend->currentOamに対して同等のパラメータを上書き処理を行ってください。
                
  
  Arguments:    pRend   : レンダラへのポインタ
                pCell   : 一時セル（書き換え可能）
                oamIdx  : セル番号
               
  Returns:      無し
  
 *---------------------------------------------------------------------------*/ 



//------------------------------------------------------------------------------
typedef struct NNSG2dRndCoreSurface
{
    NNSG2dViewRect                    viewRect;                     // 可視領域をあらわす矩形
    BOOL                              bActive;                      // 有効か
    NNSG2dSurfaceType                 type;                         // サーフェスの種類 
    
    
    NNSG2dRndCoreDrawCellCallBack         pBeforeDrawCellBackFunc;  // 可視カリング関数もかねる
    NNSG2dRndCoreDrawCellCallBack         pAfterDrawCellBackFunc;   
    
    NNSG2dRndCoreDrawOamCallBack          pBeforeDrawOamBackFunc;   // Oam パラメータ上書き用コールバック  
                                                                    // NNSG2dRndCoreOamOverwriteCallBack     
                                                                    // pOamOverwriteFunc は本コールバックと統合されました。
    NNSG2dRndCoreDrawOamCallBack          pAfterDrawOamBackFunc;    
}
NNSG2dRndCoreSurface;

struct NNSG2dImageProxy;
struct NNSG2dImagePaletteProxy;

//------------------------------------------------------------------------------
typedef struct NNSG2dRndCoreInstance
{
    NNSG2dRndCoreSurface*                   pCurrentTargetSurface; // 描画対象サーフェス
    NNSG2dRendererAffineTypeOverwiteMode    affineOverwriteMode;   // （規定値：NNS_G2D_RND_AFFINE_OVERWRITE_DOUBLE）
    
    //
    // OBJが参照する 画像データ
    //
    const struct NNSG2dImageProxy*          pImgProxy;             // 画像情報
    const struct NNSG2dImagePaletteProxy*   pPltProxy;             // パレット情報
    u32                                     base2DCharOffset;      // 2D キャラクタベースオフセット
                                                                   // BeginRendering 呼び出し時に呼ばれます。
    u32                                     baseTexAddr3D;         // 3D テクスチャベースオフセット
    u32                                     basePltAddr3D;         // 3D テクスチャパレットベースオフセット

    //
    // 2D 関連登録関数
    // 2D HW描画を行う際には必ず設定されていること
    // pFuncOamRegister と pFuncOamBlockRegister は 排他的に使用されます。
    //
    NNSG2dOamRegisterFunction             pFuncOamRegister;        // Oam 登録関数
    NNSG2dAffineRegisterFunction          pFuncOamAffineRegister;  // Affine 登録関数
    
    
    // NNSG2dRendererFlip 
    // 描画フリップを使用するかどうか
    // 使用する場合はアフィン変換が利用できないので注意
    u32                                     flipFlag;              
    
    // 
    // pCurrentMtxCacheFor2D が 設定されている場合は、2D HW によるOBJ描画の際に 
    // OBJ に対して アフィン変換が施されます。
    // 2Dハードウェアの制約からアフィン変換 と フリップ処理は排他的に使用され無ければならないので、
    // flipFlag != NNS_G2D_RENDERERFLIP_NONE で pCurrentMtxCacheFor2D が設定された場合、
    // レンダラコアモジュールは警告メッセージを表示します。
    //
    NNSG2dRndCore2DMtxCache*                pCurrentMtxCacheFor2D;      // カレント行列のキャッシュ(2D HW 用 )
    // 
    // pCurrentMxt は 3D HW 描画の際に使用されるカレント行列です。
    // 設定されなかった場合には、単位行列が使用されます。
    //
    const MtxFx32*                          pCurrentMxt;           // カレント行列
    
    
    
    BOOL                                    bDrawEnable;           // 描画が有効か？
    
    fx32                                    zFor3DSoftwareSprite;  // Z 値
    
    GXOamAttr                               currentOam;            // 現在の描画処理対象OAM
    
    MtxFx43                                 mtxFor3DGE;            // 3D Graphics Engine 用の行列
                                                                   // pCurrentMxtから生成されます。
    
    
    
                                                                       
}NNSG2dRndCoreInstance;



//------------------------------------------------------------------------------
// 初期化
void NNS_G2dInitRndCore( NNSG2dRndCoreInstance* pRnd );
void NNS_G2dInitRndCoreSurface( NNSG2dRndCoreSurface* pSurface );


//------------------------------------------------------------------------------
// 画像プロキシの設定
struct NNSG2dImageProxy;
struct NNSG2dImagePaletteProxy;

void NNS_G2dSetRndCoreImageProxy
(
    NNSG2dRndCoreInstance*                     pRnd,
    const struct NNSG2dImageProxy*             pImgProxy, 
    const struct NNSG2dImagePaletteProxy*      pPltProxy
);

//------------------------------------------------------------------------------
// 2D HW 登録関数
void NNS_G2dSetRndCoreOamRegisterFunc
(
    NNSG2dRndCoreInstance*          pRnd,
    NNSG2dOamRegisterFunction       pFuncOamRegister,
    NNSG2dAffineRegisterFunction    pFuncOamAffineRegister
);

//------------------------------------------------------------------------------
// 設定
void NNS_G2dSetRndCoreAffineOverwriteMode
(   NNSG2dRndCoreInstance*                  pRnd, 
    NNSG2dRendererAffineTypeOverwiteMode    mode 
);

void NNS_G2dSetRndCore3DSoftSpriteZvalue
( 
    NNSG2dRndCoreInstance* pRnd, fx32 z 
);

void NNS_G2dSetRndCoreSurface
( 
    NNSG2dRndCoreInstance* pRnd, 
    NNSG2dRndCoreSurface* pSurface 
);

//------------------------------------------------------------------------------
// 行列関連
void NNS_G2dSetRndCoreCurrentMtx3D
( 
    const MtxFx32*          pCurrentMxt
);
void NNS_G2dSetRndCoreCurrentMtx2D
( 
    const MtxFx32*                 pMtx,
    NNSG2dRndCore2DMtxCache*       pCurrentMtxCacheFor2D
);


//------------------------------------------------------------------------------
// Flip関連
BOOL NNS_G2dIsRndCoreFlipH( const NNSG2dRndCoreInstance* pRnd );
BOOL NNS_G2dIsRndCoreFlipV( const NNSG2dRndCoreInstance* pRnd );
void NNS_G2dSetRndCoreFlipMode( NNSG2dRndCoreInstance* pRnd, BOOL bFlipH, BOOL bFlipV );



//------------------------------------------------------------------------------
// 描画
void NNS_G2dRndCoreBeginRendering( NNSG2dRndCoreInstance* pRnd );
void NNS_G2dRndCoreEndRendering( void );

void NNS_G2dRndCoreDrawCell( const NNSG2dCellData* pCell );
void NNS_G2dRndCoreDrawCellVramTransfer( const NNSG2dCellData* pCell, u32 cellVramTransferHandle );

void NNS_G2dRndCoreDrawCellFast2D( const NNSG2dCellData* pCell );

void NNS_G2dSetRndCoreCellCloneSource3D ( const NNSG2dCellData* pCell );
void NNS_G2dRndCoreDrawCellClone3D      ( const NNSG2dCellData* pCell );

//------------------------------------------------------------------------------
// inline 関数
//------------------------------------------------------------------------------

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dInitRndCore2DMtxCache

  Description:  行列キャッシュを初期化します。
                行列キャッシュ内部のアフィンインデックス情報を初期化しています。
                
                NNS_G2dSetRndCore2DMtxCacheMtxParams() によって、
                行列キャッシュの内容を設定する際に内部で呼び出されます。
                
  Arguments:    pMtxCache:      行列キャッシュ
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE void NNS_G2dInitRndCore2DMtxCache
( 
    NNSG2dRndCore2DMtxCache* pMtxCache 
)
{
    MI_CpuFillFast( pMtxCache->affineIndex, 
                    MtxCache_NOT_AVAILABLE_ForMemFill, 
                    sizeof( pMtxCache->affineIndex ) );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetRndCore2DMtxCacheMtxParams

  Description:  行列キャッシュに行列を設定します。
                内部で、NNS_G2dInitRndCore2DMtxCache が呼ばれ、アフィンインデックス情報が初期化されます。
                
                
  Arguments:    pMtxCache:      セルデータ
                pM:             アフィン変換行列
                                (2Dグラフィックスエンジンのアフィンパラメータに設定する行列です。
                                 スケール要素の扱いが一般の変換行列と異なる点に注意してください。)
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE void NNS_G2dSetRndCore2DMtxCacheMtxParams
( 
    NNSG2dRndCore2DMtxCache* pMtxCache, 
    MtxFx22*          pM 
)
{
    NNS_G2dInitRndCore2DMtxCache( pMtxCache );
    pMtxCache->m22 = *pM;    
}



#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // NNS_G2D_RENDERERCORE_H_