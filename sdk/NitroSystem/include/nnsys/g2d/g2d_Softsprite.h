/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g2d
  File:     g2d_Softsprite.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.16 $

 *---------------------------------------------------------------------------*/
#ifndef NNS_G2D_SOFTWARESPRITE_H_
#define NNS_G2D_SOFTWARESPRITE_H_

#include <nitro.h>
#include <nnsys/g2d/g2d_config.h>
#include <nnsys/g2d/fmt/g2d_Vec_data.h>
#include <nnsys/g2d/g2d_Image.h>


#ifdef __cplusplus
extern "C" {
#endif

//
// 改名を行った関数の別名
// 互換性維持のため別名として以前の関数を宣言します。
// 
#define NNS_G2dSetCurrentPolygonID      NNS_G2dSetSpriteCurrentPolygonID
#define NNS_G2dSetDefaultAttr           NNS_G2dSetSpriteDefaultAttr
#define NNS_G2dGetDefaultAttr           NNS_G2dGetSpriteDefaultAttr

#define NNS_G2D_SPRITEATTR_TEXTUE       NNS_G2D_SPRITEATTR_TEXTURE


/*---------------------------------------------------------------------------*
  Name:         NNSG2dSpriteAttr

  Description:  スプライト属性
 *---------------------------------------------------------------------------*/
typedef enum NNSG2dSpriteAttr
{
    NNS_G2D_SPRITEATTR_ALPHA   = 0x1,
    NNS_G2D_SPRITEATTR_TEXTURE = 0x2,
    NNS_G2D_SPRITEATTR_COLOR   = 0x4,
    NNS_G2D_SPRITEATTR_UV      = 0x8,
    NNS_G2D_SPRITEATTR_FLIP    = 0x10,
    NNS_G2D_SPRITEATTR_ROTO    = 0x20,
    NNS_G2D_SPRITEATTR_ROTXY   = 0x40

}NNSG2dSpriteAttr;

/*---------------------------------------------------------------------------*
  Name:         NNSG2dSimpleSprite

  Description:  スプライト実体
 *---------------------------------------------------------------------------*/
typedef struct NNSG2dSimpleSprite
{
    NNSG2dSVec2        pos;        // 位置   
    NNSG2dSVec2        size;       // サイズ
    
    u16                rotZ;       // 回転角( unit = 360 / 65535 )
    u8                 priority;   // 表示優先度
    u8                 alpha;      // α( 0 - 31 )
    
}
NNSG2dSimpleSprite;

//------------------------------------------------------------------------------
typedef struct NNSG2dBasicSprite
{
    NNSG2dSimpleSprite        simpleParams;   // NNSG2dSimpleSprite
    NNSG2dImageAttr*          pTextureAttr;   // Texture 属性
    u32                       texAddr;        // テクスチャアドレス
    u32                       plttAddr;       // パレットアドレス
    u16                       plttOffset;     // パレットテクスチャ使用時のみ
    GXRgb                     color;          // カラー
}
NNSG2dBasicSprite;

//------------------------------------------------------------------------------
typedef struct NNSG2dExtendedSprite
{
    NNSG2dBasicSprite         basicParams;// NNSG2dBasicSprite
    NNSG2dFVec2               uvUL;       // UV (四角：左上)
    NNSG2dFVec2               uvLR;       // UV (四角：右下)
    BOOL                      flipH;      // 水平フリップ           
    BOOL                      flipV;      // 垂直フリップ
    NNSG2dSVec2               rotO;       // 回転中心
    u16                       rotX;       // X回転
    u16                       rotY;       // Y回転
}
NNSG2dExtendedSprite;




//------------------------------------------------------------------------------
// 初期化
void NNS_G2dSetupSoftwareSpriteCamera();
void NNS_G2dSetupSoftwareSpriteMaterial();

//------------------------------------------------------------------------------
// 属性設定
void NNS_G2dSetSpriteAttrEnable( u32 attr );
BOOL NNS_G2dIsSpriteAttrEnable( NNSG2dSpriteAttr attr );
void NNS_G2dSetSpriteCurrentPolygonID( u16 id );

void                          NNS_G2dSetSpriteDefaultAttr( const NNSG2dExtendedSprite* pE );
const NNSG2dExtendedSprite*   NNS_G2dGetSpriteDefaultAttr( );

//------------------------------------------------------------------------------
// 描画
void NNS_G2dDrawSpriteSimpleFast( const NNSG2dSimpleSprite* pS );
void NNS_G2dDrawSpriteBasicFast( const NNSG2dBasicSprite* pB );
void NNS_G2dDrawSpriteExtendedFast( const NNSG2dExtendedSprite* pE );


//------------------------------------------------------------------------------
//
// 直接パラメータを代入するAPI
//
//------------------------------------------------------------------------------
void NNS_G2dDrawSpriteFast
( 
    s16 px, s16 py, int pz, 
    int sx, int sy, 
    int U0, int V0, int U1, int V1 
);

//------------------------------------------------------------------------------
// インライン関数
//------------------------------------------------------------------------------
/*---------------------------------------------------------------------------*
  Name:         NNS_G2dDrawSpriteSimple ... NNS_G2dDrawSprite

  Description:  スプライトを描画します
                関数呼び出しの前後でジオメトリエンジンのカレント行列の内容が保存されます。
                
                
  Arguments:    省略
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE void NNS_G2dDrawSpriteSimple( const NNSG2dSimpleSprite* pS )
{
    G3_PushMtx();
       NNS_G2dDrawSpriteSimpleFast( pS );
    G3_PopMtx(1);
}

NNS_G2D_INLINE void NNS_G2dDrawSpriteBasic( const NNSG2dBasicSprite* pB )
{
    G3_PushMtx();
       NNS_G2dDrawSpriteBasicFast( pB );
    G3_PopMtx(1);
}
NNS_G2D_INLINE void NNS_G2dDrawSpriteExtended( const NNSG2dExtendedSprite* pE )
{
    G3_PushMtx();
       NNS_G2dDrawSpriteExtendedFast( pE );
    G3_PopMtx(1);
}

NNS_G2D_INLINE void NNS_G2dDrawSprite
( 
    s16 px, s16 py, int pz, 
    int sx, int sy, 
    int U0, int V0, int U1, int V1 
)
{
    G3_PushMtx();
       NNS_G2dDrawSpriteFast( px, py, pz, sx, sy, U0, V0, U1, V1 );
    G3_PopMtx(1);
}


//------------------------------------------------------------------------------
// ライブラリ内部公開
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// NNS_G2dDrawSpriteFast と比較して、Z値、UV値の型が異なる点に注意
void NNSi_G2dDrawSpriteFast
( 
    s16 px, s16 py, fx32 pz, 
    int sx, int sy, 
    fx32 U0, fx32 V0, fx32 U1, fx32 V1 
);

//------------------------------------------------------------------------------
void NNSi_G2dDrawSpriteWithMtxFast
( 
    s16 px, s16 py, fx32 pz, 
    int sx, int sy, 
    const MtxFx22* baseMtx, 
    fx32 U0, fx32 V0, fx32 U1, fx32 V1 
);

//------------------------------------------------------------------------------
void NNSi_G2dDrawSpriteWithMtxDoubleAffineFast
( 
    s16 px, s16 py, fx32 pz, 
    int sx, int sy, 
    const MtxFx22* baseMtx, 
    fx32 U0, fx32 V0, fx32 U1, fx32 V1 
);




#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // NNS_G2D_SOFTWARESPRITE_H_
