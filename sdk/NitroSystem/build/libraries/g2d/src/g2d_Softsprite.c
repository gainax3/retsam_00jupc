/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - g2d
  File:     g2d_Softsprite.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.31 $

 *---------------------------------------------------------------------------*/
#include <nitro.h>
#include <nnsys/g2d/g2d_Softsprite.h>


static  NNSG2dExtendedSprite              defaultSpriteAttr_;
static  u32                         defaultAttrEnable_  = 0x0;
static  u16                         currentPolygonID_   = 0x0;



static void DrawSprite_( const NNSG2dSimpleSprite* pS, const NNSG2dBasicSprite* pB, const NNSG2dExtendedSprite* pE );

//---------------------------------------------------------------------------
// カレント行列を保存しないように、仕様が変更になりました。
// 本関数を利用する関数はすべて同様の特性をもつこととなります。
static void DrawSprite_( const NNSG2dSimpleSprite* pS, const NNSG2dBasicSprite* pB, const NNSG2dExtendedSprite* pE )
{
    const fx32      size = FX32_HALF;
    
    fx32 U0 = 0; 
    fx32 U1 = pS->size.x << FX32_SHIFT; 
    
    fx32 V0 = 0; 
    fx32 V1 = pS->size.y << FX32_SHIFT; 
    
    // Overwrite the UV values.
    if( NNS_G2dIsSpriteAttrEnable( NNS_G2D_SPRITEATTR_UV ) )
    {
    
        U0 = pE->uvUL.x;
        U1 = pE->uvLR.x;
        V0 = pE->uvUL.y;
        V1 = pE->uvLR.y;
    }
    
    // Flip 
    if( NNS_G2dIsSpriteAttrEnable( NNS_G2D_SPRITEATTR_FLIP ) )
    {
        fx32    temp;
        // H
        if( pE->flipH )
        {
            temp = U0;
            U0 = U1;
            U1 = temp;
        }
        // V
        if( pE->flipV )
        {
            temp = V0;
            V0 = V1;
            V1 = temp;
        }
    }
    
    {
        //
        // SRT
        //
        G3_Translate( ( pS->pos.x << FX32_SHIFT ) + ((pS->size.x >> 1) << FX32_SHIFT ), 
                      ( pS->pos.y << FX32_SHIFT ) + ((pS->size.y >> 1) << FX32_SHIFT ), 
                      ( (int)pS->priority << FX32_SHIFT ) );
                      
        // RotO
        if( NNS_G2dIsSpriteAttrEnable( NNS_G2D_SPRITEATTR_ROTO ) )
        {
            G3_Translate( pE->rotO.x << FX32_SHIFT , 
                          pE->rotO.y << FX32_SHIFT , 
                          0 );            
        }
        // RotXY
        if( NNS_G2dIsSpriteAttrEnable( NNS_G2D_SPRITEATTR_ROTXY ) )
        {
            G3_RotX( FX_SinIdx( pE->rotX ), FX_CosIdx( pE->rotX ) );
            G3_RotY( FX_SinIdx( pE->rotY ), FX_CosIdx( pE->rotY ) );
        }
        G3_RotZ( FX_SinIdx( pS->rotZ ), FX_CosIdx( pS->rotZ ) );
        
        
        // RotO
        if( NNS_G2dIsSpriteAttrEnable( NNS_G2D_SPRITEATTR_ROTO ) )
        {
            G3_Translate( -pE->rotO.x << FX32_SHIFT , 
                          -pE->rotO.y << FX32_SHIFT , 
                          0 );
        }
        G3_Scale( pS->size.x << FX32_SHIFT, 
                  pS->size.y << FX32_SHIFT, 
                  FX32_ONE );
        
        
        //
        //
        //
        if( NNS_G2dIsSpriteAttrEnable( NNS_G2D_SPRITEATTR_TEXTURE ) )
        {
            // Texture setting.
            if( pB->pTextureAttr )
            {
                const NNSG2dImageAttr* pAttr = pB->pTextureAttr;
                
                G3_TexImageParam( pAttr->fmt,        // use alpha texture
                         GX_TEXGEN_TEXCOORD,         // use texcoord
                         pAttr->sizeS,               // 16 pixels
                         pAttr->sizeT,               // 16 pixels
                         GX_TEXREPEAT_NONE,          // no repeat
                         GX_TEXFLIP_NONE,            // no flip
                         pAttr->plttUse,             // use color 0 of the palette
                         pB->texAddr                 // the offset of the texture image
                         );
                
                //
                // パレットフォーマットならば...
                //                         
                if( NNS_G2dIsPaletteImageFmt( pAttr ) )
                {
                    G3_TexPlttBase( (u32)(pB->plttAddr + 32 * pB->plttOffset), pAttr->fmt );                                
                }
                
            }else{
                OS_Warning("The Texture Attribute of software sprite must be specified.");
            }
        }
        
        //
        // alpha blending
        //
        if( NNS_G2dIsSpriteAttrEnable( NNS_G2D_SPRITEATTR_ALPHA ) )
        {
            //
            // 注意 : alpha = 0 ならば描画を行いません 
            //
            if( pS->alpha == 0 )
            {
                return;
            }
            
            //
            // 注意：G2Dは内部で Graphics Engine のレジスタ設定を勝手に変更します。
            //       また設定の復元も行いません。
            //
            G3_PolygonAttr(GX_LIGHTMASK_NONE,              // disable lights
                   GX_POLYGONMODE_MODULATE,                // modulation mode
                   GX_CULL_NONE,                           // cull back
                   currentPolygonID_,                      // polygon ID(0 - 63)
                   pS->alpha,                              // alpha(0 - 31)
                   0                                       // OR of GXPolygonAttrMisc's value
                   );
        }
        //
        // VertexColor
        //
        if( NNS_G2dIsSpriteAttrEnable( NNS_G2D_SPRITEATTR_COLOR ) )
        {
            G3_Color( pB->color );
        }
        
        //
        // Draw a quad polygon.(描画平面の方向を変更しました)
        //
        G3_Begin( GX_BEGIN_QUADS );
            
            G3_TexCoord( U0, V1 );
            G3_Vtx10( 0 - size,0 + size, 0 ); 
            
            G3_TexCoord( U1, V1 );
            G3_Vtx10( 0 + size, 0 + size, 0 ); 
            
            G3_TexCoord( U1, V0 );
            G3_Vtx10( 0 + size, -size, 0 ); 
            
            G3_TexCoord( U0, V0 );
            G3_Vtx10( 0 - size, -size, 0 );
            
        G3_End( );
    }
}

//------------------------------------------------------------------------------
static void SetAffine43( MtxFx43* pDst, const MtxFx22* pSubMtx, fx32 x, fx32 y, fx32 z )
{
    NNS_G2D_NULL_ASSERT(pDst);
    NNS_G2D_NULL_ASSERT(pSubMtx);
    
    
    pDst->_00 = pSubMtx->_00;
    pDst->_01 = pSubMtx->_01;
    pDst->_02 = 0;
    pDst->_10 = pSubMtx->_10;
    pDst->_11 = pSubMtx->_11;
    pDst->_12 = 0;
    pDst->_20 = 0;
    pDst->_21 = 0;
    pDst->_22 = FX32_ONE;
    pDst->_30 = x;
    pDst->_31 = y;
    pDst->_32 = z;
}
//------------------------------------------------------------------------------
static void SetBaseMtx_( const MtxFx22* baseMtx )
{
    //
    // RTS
    //
    MtxFx43       mtx;
    SetAffine43( &mtx, baseMtx, 0, 0, 0 );
    
    // base
    G3_MultMtx43( &mtx );
    
}
//------------------------------------------------------------------------------
static void DrawOneQuad_( fx32 U0, fx32 U1, fx32 V0, fx32 V1 )
{
    const fx32      size = FX32_ONE;
            
    {
        //
        // Draw a quad polygon.
        //
        G3_Begin( GX_BEGIN_QUADS);
            
            G3_TexCoord( U0, V1 );
            G3_Vtx( 0 ,0 + size, 0 ); 
            
            G3_TexCoord( U1, V1 );
            G3_Vtx( 0 + size, 0 + size, 0 ); 
            
            G3_TexCoord( U1, V0 );
            G3_Vtx( 0 + size, 0 , 0 ); 
            
            G3_TexCoord( U0, V0 );
            G3_Vtx( 0 , 0, 0 );
            
            
        G3_End( );
    }
}

//------------------------------------------------------------------------------
// OAMマネージャにおける、2D エミュレーション時の描画のみに使用されます。
// UV の 設定は 外部で行うこと
static void DrawOneQuadCenter_( fx32 U0, fx32 U1, fx32 V0, fx32 V1 )
{
    const fx32      size = FX32_HALF;
            
    {
        //
        // Draw a quad polygon.
        //
        G3_Begin( GX_BEGIN_QUADS);
            
            G3_TexCoord( U0, V1 );
            G3_Vtx( 0 -size ,0 + size, 0 ); 
            
            G3_TexCoord( U1, V1 );
            G3_Vtx( 0 + size, 0 + size, 0 ); 
            
            G3_TexCoord( U1, V0 );
            G3_Vtx( 0 + size, -size , 0 ); 
            
            G3_TexCoord( U0, V0 );
            G3_Vtx( -size , -size, 0 );
            
        G3_End( );
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetupSoftwareSpriteMaterial

  Description:  スプライト描画のための３Dグラフィックスエンジンのマテリアル
  　　　　　　　設定を行います。
                
                
  Arguments:    なし
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetupSoftwareSpriteMaterial()
{
    // 標準的なマテリアル設定をします。
    {
        G3_MaterialColorDiffAmb(GX_RGB(31, 31, 31),     // diffuse
                                GX_RGB(16, 16, 16),        // ambient
                                TRUE                       // use diffuse as vtx color if TRUE
                                );

        G3_MaterialColorSpecEmi(GX_RGB(16, 16, 16),        // specular
                                GX_RGB(0, 0, 0),           // emission
                                FALSE                      // use shininess table if TRUE
                                );

        G3_PolygonAttr(GX_LIGHTMASK_NONE,                  // no lights
                       GX_POLYGONMODE_MODULATE,            // modulation mode
                       GX_CULL_NONE,                       // cull back
                       0,                                  // polygon ID(0 - 63)
                       31,                                 // alpha(0 - 31)
                       0                                   // OR of GXPolygonAttrMisc's value
                       );
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetupSoftwareSpriteCamera

  Description:  スプライト描画のための３Dグラフィックスエンジンのカメラ設定を行います。
                
                
  Arguments:    なし
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetupSoftwareSpriteCamera()
{
    G3_ViewPort(0, 0, 255, 191);                           // Viewport
    {
        // 投影行列を設定します。
        {
            G3_OrthoW( FX32_ONE * 0,
                      FX32_ONE * 192,
                      FX32_ONE * 0,
                      FX32_ONE * 256,
                      FX32_ONE * -1024,
                      FX32_ONE * 1024,
                      FX32_ONE * 1024,
                      NULL );

            G3_StoreMtx(0);
        }
        
        // カメラ行列を設定します。
        {
            // 単位行列と等価
            VecFx32 Eye    = { 0, 0, 0 };          // Eye position
            VecFx32 vUp    = { 0, FX32_ONE, 0 };  // Up
            VecFx32 at     = { 0, 0, -FX32_ONE }; // Viewpoint

            G3_LookAt(&Eye, &vUp, &at, NULL);
        }

        // G3_LookAtの実行で、行列モードがGX_MTXMODE_POSITION_VECTOR に変更されるので
        // 行列モードを元に戻します
        G3_MtxMode( GX_MTXMODE_POSITION );        
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetSpriteAttrEnable

  Description:  SoftwareSpriteの有効属性を設定します
                
                
  Arguments:    attr:         [IN]  NNSG2dSpriteAttr の 各値もしくはその論理和
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetSpriteAttrEnable( u32 attr )
{
    defaultAttrEnable_ = attr;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dIsSpriteAttrEnable

  Description:  SoftwareSpriteの属性が有効か判定します
                
                
  Arguments:    attr:         [IN]  NNSG2dSpriteAttr の 各値( 注意：論理和はとれません)
                
  Returns:      属性が有効か
  
 *---------------------------------------------------------------------------*/
BOOL NNS_G2dIsSpriteAttrEnable( NNSG2dSpriteAttr attr )
{
    return (BOOL)(defaultAttrEnable_ & (u32)attr);
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetSpriteCurrentPolygonID 

  Description:  SoftwareSprite描画に使用されるポリゴンIDを指定します
                
                
  Arguments:    id:         [IN]  SoftwareSprite描画に使用されるポリゴンID(0-63)
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetSpriteCurrentPolygonID( u16 id )
{
    NNS_G2D_MINMAX_ASSERT( id, 0, 63 ); 
    
    currentPolygonID_ = id; 
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetSpriteDefaultAttr

  Description:  SoftwareSpriteのデフォルト値を設定します
                不足する情報はこの値によって補われます。
                
                
  Arguments:    pE:         [IN]  スプライト
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetSpriteDefaultAttr( const NNSG2dExtendedSprite* pE )
{
    defaultSpriteAttr_ = *pE;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetSpriteDefaultAttr

  Description:  SoftwareSpriteのデフォルト値の参照ポインタを得ます
                
  Arguments:    なし
                
  Returns:      SoftwareSpriteのデフォルト値の参照ポインタ
  
 *---------------------------------------------------------------------------*/
const NNSG2dExtendedSprite* NNS_G2dGetSpriteDefaultAttr( )
{
    return &defaultSpriteAttr_;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dDrawSpriteSimpleFast ... NNS_G2dDrawSpriteExtendedFast

  Description:  SoftwareSpriteを描画します
                不足する情報はデフォルト値によって補われます。
                関数呼び出しの前後でジオメトリエンジンのカレント行列の内容が
                保存されない点にご注意ください。
                
                保存されるバージョンとして、関数名末にFastが付かない
                NNS_G2dDrawSpriteSimple ... NNS_G2dDrawSpriteExtended
                が用意されています。
                
                
  Arguments:    pS,pB,pE:         [IN]  スプライト
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dDrawSpriteSimpleFast( const NNSG2dSimpleSprite* pS )
{
    DrawSprite_( pS, &defaultSpriteAttr_.basicParams, &defaultSpriteAttr_ ); 
}
//---------------------------------------------------------------------------
void NNS_G2dDrawSpriteBasicFast( const NNSG2dBasicSprite* pB )
{
    DrawSprite_( &pB->simpleParams, pB, &defaultSpriteAttr_ ); 
}
//---------------------------------------------------------------------------
void NNS_G2dDrawSpriteExtendedFast( const NNSG2dExtendedSprite* pE )
{
    DrawSprite_( &pE->basicParams.simpleParams, &pE->basicParams, pE ); 
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dDrawSpriteFast

  Description:  SoftwareSpriteの描画
                
                
  Arguments:    px:         [IN]  x
                py:         [IN]  y
                pz:         [IN]  z
                sx:         [IN]  size x
                sy:         [IN]  size y
                U0:         [IN]  UV値(左上U)
                V0:         [IN]  UV値(左上V)
                U1:         [IN]  UV値(右下U)
                V1:         [IN]  UV値(右下V)
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dDrawSpriteFast
( 
    s16 px, s16 py, int pz, 
    int sx, int sy, 
    int U0, int V0, int U1, int V1 
)
{
    // T
    G3_Translate( FX32_ONE * px, FX32_ONE * py, FX32_ONE * pz );    
    // S
    G3_Scale( FX32_ONE * sx, FX32_ONE * sy, FX32_ONE );
       
    DrawOneQuad_( FX32_ONE * U0, FX32_ONE * U1, FX32_ONE * V0, FX32_ONE * V1 );    
}


/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dDrawSpriteWithMtxFast

  Description:  スプライトを描画します
                Z値、UV値の型が NNS_G2dDrawSpriteFast と異なります。
                レンダラコアモジュールから利用されています。             
                
                
  Arguments:    省略
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNSi_G2dDrawSpriteFast
( 
    s16 px, s16 py, fx32 pz, 
    int sx, int sy, 
    fx32 U0, fx32 V0, fx32 U1, fx32 V1 
)
{
    // T
    G3_Translate( px << FX32_SHIFT, py << FX32_SHIFT, pz );    
    // S
    G3_Scale( sx << FX32_SHIFT, sy << FX32_SHIFT, FX32_ONE );
       
    {
       const fx32      size = FX32_ONE;
           
           
       //
       // Draw a quad polygon.
       //
       G3_Begin( GX_BEGIN_QUADS);
           
           G3_TexCoord( U0, V1 );
           G3_Vtx( 0 ,0 + size, 0 ); 
                       
           G3_TexCoord( U1, V1 );
           G3_Vtx( 0 + size, 0 + size, 0 ); 
                   
           G3_TexCoord( U1, V0 );
           G3_Vtx( 0 + size, 0 , 0 ); 
               
           G3_TexCoord( U0, V0 );
           G3_Vtx( 0 , 0, 0 );
          
       G3_End( );
    }
    
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dDrawSpriteWithMtxFast

  Description:  スプライトを描画します
                引数で行列を指定します。
                倍角アフィンモードでのOBJと同じようにスプライトの描画を行います。
                OAMマネージャのソフトウェアスプライト・エミュレーション機能使用時に
                利用される関数です。
                
                
  Arguments:    省略
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNSi_G2dDrawSpriteWithMtxFast
( 
    s16 px, s16 py, fx32 pz, 
    int sx, int sy, 
    const MtxFx22* baseMtx, 
    fx32 U0, fx32 V0, fx32 U1, fx32 V1 
)
{
    // T
    G3_Translate( FX32_ONE * px + FX32_HALF * sx, FX32_ONE * py + FX32_HALF * sy, pz );    
    // R or S
    SetBaseMtx_( baseMtx );
    // S
    G3_Scale( FX32_ONE * sx, FX32_ONE * sy, FX32_ONE );
       
    DrawOneQuadCenter_( U0, U1, V0, V1 );

}




/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dDrawSpriteWithMtxDoubleAffineFast

  Description:  スプライトを描画します。
                引数で行列を指定します。
                倍角アフィンモードでのOBJと同じようにスプライトの描画を行います。
                OAMマネージャのソフトウェアスプライト・エミュレーション機能使用時に
                利用される関数です。
                
                
  Arguments:    省略
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNSi_G2dDrawSpriteWithMtxDoubleAffineFast
( 
    s16 px, s16 py, fx32 pz, 
    int sx, int sy, 
    const MtxFx22* baseMtx, 
    fx32 U0, fx32 V0, fx32 U1, fx32 V1 
)
{
    // T
    // FX32_HALF ではなく FX32_ONE である点に注意：倍角モードの振る舞い
    G3_Translate( FX32_ONE * px + FX32_ONE * sx, FX32_ONE * py + FX32_ONE * sy, pz );    
    // R or S
    SetBaseMtx_( baseMtx );
    // S
    G3_Scale( FX32_ONE * sx, FX32_ONE * sy, FX32_ONE );
       
    DrawOneQuadCenter_( U0, U1, V0, V1 );
}










