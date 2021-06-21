/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - g2d
  File:     g2d_OamSoftwareSpriteDraw.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.25 $
 *---------------------------------------------------------------------------*/
 
#include <nnsys/g2d/g2d_OAM.h>
#include <nnsys/g2d/g2d_Softsprite.h>

#include <nnsys/g2d/g2d_OamSoftwareSpriteDraw.h>
#include <nnsys/g2d/fmt/g2d_Cell_data.h>


#include "g2d_Internal.h"

#define NNSI_G2D_SHIFT_SIZEOF_256PLTT   9   // means ( sizeof( u16 ) * 256 )
#define NNSI_G2D_SHIFT_SIZEOF_16PLTT    5   // means ( sizeof( u16 ) * 16 )

typedef struct SoftwareSpriteParamCache
{
    fx32       u0;
    fx32       v0;
    fx32       u1;
    fx32       v1;
    
}SoftwareSpriteParamCache;

typedef struct SpriteParams
{
    fx32   u0;
    fx32   v0;
    fx32   u1;
    fx32   v1;
    
    int    sx;
    int    sy;
    
}SpriteParams;



//
// パラメータキャッシュ
// まったく同じテクスチャ設定のスプライトを大量に描画する際に使用します。
static SoftwareSpriteParamCache         softwareSpreiteParamCache_ =
{
    0,
    0,  
    0,
    0
};




// Oamソフトウエアスプライトエミュレーション で使用される UV値補正関数ポインタ
static NNS_G2dOamSoftEmuUVFlipCorrectFunc        s_pUVFlipCorrectFunc   = NULL;



// GXTexFmt キャラクターサイズテーブル
static const u32 characterShiftSize_[] = 
{
    0,// GX_TEXFMT_NONE   
    0,// GX_TEXFMT_A3I5   
    0,// GX_TEXFMT_PLTT4  
    5,// GX_TEXFMT_PLTT16 
    6,// GX_TEXFMT_PLTT256
    0,// GX_TEXFMT_COMP4x4
    0,// GX_TEXFMT_A5I3  
    0 // GX_TEXFMT_DIRECT
};
// GXOamShape から GXTexSizeS を テーブル引きします
static const GXTexSizeS gxTexSizeSTbl [3][4] = 
{
           {
           GX_TEXSIZE_S8,
           GX_TEXSIZE_S16,
           GX_TEXSIZE_S32,
           GX_TEXSIZE_S64
           },
           {
           GX_TEXSIZE_S16,
           GX_TEXSIZE_S32,
           GX_TEXSIZE_S32,
           GX_TEXSIZE_S64
           },
           {
           GX_TEXSIZE_S8,
           GX_TEXSIZE_S8,
           GX_TEXSIZE_S16,
           GX_TEXSIZE_S32
           }
};    

// GXOamShape から GXTexSizeT を テーブル引きします
static const GXTexSizeT gxTexSizeTTbl [3][4] = 
{
           {
           GX_TEXSIZE_T8,
           GX_TEXSIZE_T16,
           GX_TEXSIZE_T32,
           GX_TEXSIZE_T64
           },
           {
           GX_TEXSIZE_T8,
           GX_TEXSIZE_T8,
           GX_TEXSIZE_T16,
           GX_TEXSIZE_T32
           },
           {
           GX_TEXSIZE_T16,
           GX_TEXSIZE_T32,
           GX_TEXSIZE_T32,
           GX_TEXSIZE_T64        
           }
};

static BOOL         bAutoZOffsetAdd_    = FALSE;    // スプライトを描画するたびにZオフセットを自動加算するかどうか
static fx32         zOffset_            = 0;        // スプライトに与えられる、Zオフセット
static fx32         zOffsetStep_        = -FX32_ONE; // スプライトに与えられる、Zオフセットの増加分






//------------------------------------------------------------------------------
// GXTexSizeSからテクスチャサイズ を u32 値で 取得します
static NNS_G2D_INLINE u32 GetNumTexChar_( GXTexSizeS texSize )
{
    static const u32 texSize_ [] = 
    {
        1,
        2,
        4,
        8,
        16,
        32,
        64,
        128   
    };
    
    GX_TEXSIZE_S_ASSERT( texSize );
    
    return texSize_[ texSize ];
}

//------------------------------------------------------------------------------
// 自動Zオフセットのオフセット値を更新する
static NNS_G2D_INLINE void IncreaseAutoZOffset_()
{
    if( bAutoZOffsetAdd_ )
    {
        zOffset_ += zOffsetStep_;
    }
}

//------------------------------------------------------------------------------
// fx32 型 の Z 値 を取得する、自動Zオフセットを考慮している
static NNS_G2D_INLINE fx32 GetFx32DepthValue_( int z )
{
    if( bAutoZOffsetAdd_ )
    {
        return ( z << FX32_SHIFT ) + zOffset_;
    }else{
        return ( z << FX32_SHIFT );
    }
}

//------------------------------------------------------------------------------
// NNS_G2dDrawOneOam3DDirect() 内でのUV値計算を行います
static NNS_G2D_INLINE void DoFlip_
(
    BOOL bFlipH, BOOL bFlipV,
    fx32*   pRetU0,  fx32* pRetU1,
    fx32*   pRetV0,  fx32* pRetV1
)
{
    NNS_G2D_NULL_ASSERT( pRetU0 );
    NNS_G2D_NULL_ASSERT( pRetU1 );
    NNS_G2D_NULL_ASSERT( pRetV0 );
    NNS_G2D_NULL_ASSERT( pRetV1 );
    
    // UVフリップ処理
    {
        fx32 temp;
        if( bFlipH )
        {
            temp = *pRetU0;
            *pRetU0 = *pRetU1;
            *pRetU1 = temp;
        }
        
        if( bFlipV )
        {
            temp = *pRetV0;
            *pRetV0 = *pRetV1;
            *pRetV1 = temp;
        }
       
        // UV 値補正関数が設定されているならば呼び出します。
        if( s_pUVFlipCorrectFunc )
        {
            (*s_pUVFlipCorrectFunc)( pRetU0, pRetV0, pRetU1, pRetV1, bFlipH, bFlipV );
        }
    }
}

//------------------------------------------------------------------------------
// NNS_G2dDrawOneOam3DDirect() 内でのUV値計算を行います
static NNS_G2D_INLINE void CalcUVFor3DDirect2DMap_
( 
    const NNSG2dImageAttr*          pTexImageAttr, 
    u16                             charName,
    fx32*   pRetU0,  
    fx32*   pRetV0
)
{
    NNS_G2D_NULL_ASSERT( pRetU0 );
    NNS_G2D_NULL_ASSERT( pRetV0 );
    
    GX_OBJVRAMMODE_CHAR_ASSERT( pTexImageAttr->mappingType );
    
    //
    // 注意：2D グラフィックエンジンの仕様です。
    // 実際 の 8*8 テクセルのキャラクタ数は charName/2 です。
    //
    if( pTexImageAttr->fmt == GX_TEXFMT_PLTT256 )
    {
        // charName /= 2;
        charName >>= 1;
    }
    
    {
        // GX_OBJVRAMMODE_CHAR_2Dの場合
        {
            const u32 numCharPerOneLine = GetNumTexChar_( pTexImageAttr->sizeS );
            
            //U = ((charName) % numCharPerOneLine ) * 8;
        	*pRetU0 = (fx32)( (( charName & ( numCharPerOneLine - 1 ) ) << 3) << FX32_SHIFT ); 
            
            //
            // 注意：高速化のため。（GXTexSizeSの値に依存しています。）
            //
            //V = ((charName) / numCharPerOneLine ) * 8;
            *pRetV0 = (( charName >> pTexImageAttr->sizeS ) << 3) << FX32_SHIFT;
        }
    }
}

//------------------------------------------------------------------------------
// NNS_G2dDrawOneOam3DDirect() 内でのUV値計算を行います
static NNS_G2D_INLINE void CalcUVFor3DDirect1DMap_
( 
    fx32*   pRetU0,  
    fx32*   pRetV0
)
{
    NNS_G2D_NULL_ASSERT( pRetU0 );
    NNS_G2D_NULL_ASSERT( pRetV0 );
    
    
    {
        // GX_OBJVRAMMODE_CHAR_1D_32K:
        // GX_OBJVRAMMODE_CHAR_1D_64K:
        // GX_OBJVRAMMODE_CHAR_1D_128K:
        // GX_OBJVRAMMODE_CHAR_1D_256K:の場合
        //
        // baseOffset で 対応する
        // NNS_G2dDrawOneOam3DDirect() を 参照されたい
        //
        *pRetU0 = 0; 
        *pRetV0 = 0;
    }
}


                                          


//------------------------------------------------------------------------------
static NNS_G2D_INLINE GXTexSizeS GetTexS_( GXOamShape shape )
{
    GX_OAM_SHAPE_ASSERT( shape );
    {

    
    const u16 shapeBit = (u16)(( shape & GX_OAM_ATTR01_SHAPE_MASK ) >> GX_OAM_ATTR01_SHAPE_SHIFT);
    const u16 sizeBit = (u16)(( shape & GX_OAM_ATTR01_SIZE_MASK ) >> GX_OAM_ATTR01_SIZE_SHIFT);
                              
    return gxTexSizeSTbl[shapeBit][sizeBit];
    }
}

//------------------------------------------------------------------------------
// GXOamShape から GXTexSizeT を テーブル引きします
static NNS_G2D_INLINE GXTexSizeT GetTexT_( GXOamShape shape )
{
    GX_OAM_SHAPE_ASSERT( shape );
    {

    const u16 shapeBit = (u16)(( shape & GX_OAM_ATTR01_SHAPE_MASK ) >> GX_OAM_ATTR01_SHAPE_SHIFT);
    const u16 sizeBit = (u16)(( shape & GX_OAM_ATTR01_SIZE_MASK ) >> GX_OAM_ATTR01_SIZE_SHIFT);
                              
    return gxTexSizeTTbl[shapeBit][sizeBit];
    }
}    



//------------------------------------------------------------------------------
// パレット番号が示す分のオフセットバイト数を計算します。
static NNS_G2D_INLINE u32 GetOffsetByteSizeOfPlt_( GXTexFmt pltFmt, BOOL bExtendedPlt, u16 pltNo )
{
    NNS_G2D_ASSERT( pltFmt == GX_TEXFMT_PLTT16 || pltFmt == GX_TEXFMT_PLTT256 );
    
    if( bExtendedPlt )
    {
        // 拡張パレット 256 * 16 
        NNS_G2D_ASSERT( pltFmt == GX_TEXFMT_PLTT256 );
        
        //return pltNo * (sizeof( u16 ) * 256 );
    	return (u32)( pltNo << NNSI_G2D_SHIFT_SIZEOF_256PLTT );
    }else{
        
        if( pltFmt == GX_TEXFMT_PLTT256 )
        {
            // GX_TEXFMT_PLTT256 では パレット番号は無視する必要があります。
            //return pltNo * 0;
            return 0;
        }else{
            // return pltNo * (sizeof( u16 ) * 16);
        	return (u32)( pltNo << NNSI_G2D_SHIFT_SIZEOF_16PLTT );
        }
    }
    
}

//------------------------------------------------------------------------------
static NNS_G2D_INLINE int GetCharacterNameShiftBit_( GXOBJVRamModeChar objMappingMode )
{
    
    /*
    switch( objMappingMode)
    {
    case GX_OBJVRAMMODE_CHAR_1D_64K:
        return 1;
    case GX_OBJVRAMMODE_CHAR_1D_128K:
        return 2;
    case GX_OBJVRAMMODE_CHAR_1D_256K:
        return 3;
    
    default:
    case GX_OBJVRAMMODE_CHAR_2D:
    case GX_OBJVRAMMODE_CHAR_1D_32K:
        return 0;    
    }
    */
    //
    // 注意：enum 定義に依存する処理です。
    //
    /*
    GX_OBJVRAMMODE_CHAR_2D      = (0 << REG_GX_DISPCNT_OBJMAP_SHIFT) | (0 << REG_GX_DISPCNT_EXOBJ_SHIFT),
    GX_OBJVRAMMODE_CHAR_1D_32K  = (1 << REG_GX_DISPCNT_OBJMAP_SHIFT) | (0 << REG_GX_DISPCNT_EXOBJ_SHIFT),
    GX_OBJVRAMMODE_CHAR_1D_64K  = (1 << REG_GX_DISPCNT_OBJMAP_SHIFT) | (1 << REG_GX_DISPCNT_EXOBJ_SHIFT),
    GX_OBJVRAMMODE_CHAR_1D_128K = (1 << REG_GX_DISPCNT_OBJMAP_SHIFT) | (2 << REG_GX_DISPCNT_EXOBJ_SHIFT),
    GX_OBJVRAMMODE_CHAR_1D_256K = (1 << REG_GX_DISPCNT_OBJMAP_SHIFT) | (3 << REG_GX_DISPCNT_EXOBJ_SHIFT)
    */
    return (objMappingMode & REG_GX_DISPCNT_EXOBJ_MASK ) >> REG_GX_DISPCNT_EXOBJ_SHIFT;
}


//------------------------------------------------------------------------------
// NNS_G2dDrawOneOam3DDirect() 内でのTexture設定を行います
static NNS_G2D_INLINE void SetTextureParamsFor3DDirect1DMap_
(
    const NNSG2dImageAttr*          pTexImageAttr,
    u32                             texBaseAddr,
    GXOamShape                      shape,
    u16                             charName
)
{
    NNS_G2D_NULL_ASSERT( pTexImageAttr );
    
    // 
    // テクスチャサイズを設定する
    // 
    {
        const u16 shapeBit   = (u16)(( shape & GX_OAM_ATTR01_SHAPE_MASK ) >> GX_OAM_ATTR01_SHAPE_SHIFT);
        const u16 sizeBit    = (u16)(( shape & GX_OAM_ATTR01_SIZE_MASK ) >> GX_OAM_ATTR01_SIZE_SHIFT);
        //
        // フォーマットに応じて、本来のキャラクタブロック数をシフト計算して求める
        //
        // 5 は * 32(8*8テクセルのバイトサイズ) の意
        // 本来は pTexImageAttr->fmt == GX_TEXFMT_PLTT256 の場合は
        //     ( (charName / 2) * ( 32 * 2 ) ) << shiftBit とすべきですが、
        //     これは、まとめると 結局 charName * 32 << shiftBit となります。
        //
        const int shiftBit   = ( 5 + GetCharacterNameShiftBit_( pTexImageAttr->mappingType ) );
                          
        NNS_G2D_ASSERT( pTexImageAttr->mappingType != GX_OBJVRAMMODE_CHAR_2D );

        
        // 1D マッピングなら、テクスチャサイズ == OBJ サイズである 
        //
        // G3_TexImageParam を使用すると コンパイラが
        // SetTextureParamsFor3DDirect_のインライン展開に失敗するので...        
        reg_G3_TEXIMAGE_PARAM 
           = GX_PACK_TEXIMAGE_PARAM( pTexImageAttr->fmt,   
                                     GX_TEXGEN_TEXCOORD, 
                                     gxTexSizeSTbl[shapeBit][sizeBit],         
                                     gxTexSizeTTbl[shapeBit][sizeBit],         
                                     GX_TEXREPEAT_NONE,  
                                     GX_TEXFLIP_NONE,    
                                     pTexImageAttr->plttUse, 
                                     texBaseAddr + ( charName << shiftBit ) ); 
        
    }
}

//------------------------------------------------------------------------------
// NNS_G2dDrawOneOam3DDirect() 内でのTexture設定を行います
static NNS_G2D_INLINE void SetTextureParamsFor3DDirect2DMap_
(
    const NNSG2dImageAttr*          pTexImageAttr,
    u32                             texBaseAddr
)
{
    NNS_G2D_NULL_ASSERT( pTexImageAttr );        
    {        
        // 2D マッピングなら、テクスチャサイズ は キャラクタデータのサイズ
        reg_G3_TEXIMAGE_PARAM 
           = GX_PACK_TEXIMAGE_PARAM( pTexImageAttr->fmt,   
                                     GX_TEXGEN_TEXCOORD, 
                                     pTexImageAttr->sizeS,         
                                     pTexImageAttr->sizeT,         
                                     GX_TEXREPEAT_NONE,  
                                     GX_TEXFLIP_NONE,    
                                     pTexImageAttr->plttUse, 
                                     texBaseAddr );
    }
}


//------------------------------------------------------------------------------
// NNS_G2dDrawOneOam3DDirect() 内でのTexture設定を行います
static NNS_G2D_INLINE void SetPaletteParamsFor3DDirect_
(
    const NNSG2dImageAttr*          pTexImageAttr,
    u32                             pltBaseAddr,
    const GXOamAttr*                pOam
)
{
    NNS_G2D_NULL_ASSERT( pTexImageAttr );
    NNS_G2D_NULL_ASSERT( pOam );
    
    //
    // パレット設定を行う
    //
    {
        // const GXTexFmt pltFmt = ( pOam->colorMode ) ? GX_TEXFMT_PLTT256 : GX_TEXFMT_PLTT16;
        const static GXTexFmt pltFmtTbl[] = 
        {
           GX_TEXFMT_PLTT16,
           GX_TEXFMT_PLTT256
        };
        const GXTexFmt pltFmt = pltFmtTbl[pOam->colorMode];
        
        const u32 pltOffs 
               = GetOffsetByteSizeOfPlt_( pltFmt, 
                                          pTexImageAttr->bExtendedPlt, 
                                          (u16)pOam->cParam );
                                        
        G3_TexPlttBase( pltBaseAddr + pltOffs, pltFmt );
    }    
}


//------------------------------------------------------------------------------
// スプライトの平行移動値を設定します
// 関数内で、倍角アフィンOBJかどうかを判定し、その結果によって描画位置を
// 変化させています。
static NNS_G2D_INLINE void SetQuadTranslation_
( 
    const GXOamAttr* pOam, 
    const int posX, 
    const int posY, 
    const int posZ 
)
{
    #pragma inline_max_size(20000)
    // 倍角アフィンOBJか判定します
    if( G2_GetOBJEffect( pOam ) == GX_OAM_EFFECT_AFFINE_DOUBLE )
    {        
        const GXOamShape     oamShape = NNS_G2dGetOAMSize( pOam );
        const int           halfW = NNS_G2dGetOamSizeX( &oamShape ) >> 1; // - 1 は / 2 の意
        const int           halfH = NNS_G2dGetOamSizeY( &oamShape ) >> 1; 
           
        G3_Translate
        ( 
           (posX + halfW )<< FX32_SHIFT, 
           (posY + halfH )<< FX32_SHIFT, 
           GetFx32DepthValue_( posZ ) 
        );    
    }else{
        G3_Translate
        ( 
           posX << FX32_SHIFT, 
           posY << FX32_SHIFT, 
           GetFx32DepthValue_( posZ ) 
        );    
    }
}

//------------------------------------------------------------------------------
// スプライトのパラメータを計算する。
static void CalcSpriteParams_
( 
    const GXOamAttr*                pOam, 
    const NNSG2dImageAttr*          pTexImageAttr,
    u32                             texBaseAddr,
    u32                             pltBaseAddr,
    SpriteParams                    *pResult
)
{
    
    const GXOamShape  shapeOam = NNS_G2dGetOAMSize( pOam );
    const u16 charName = (u16)pOam->charNo;
    
    pResult->sx       = NNS_G2dGetOamSizeX( &shapeOam );
    pResult->sy       = NNS_G2dGetOamSizeY( &shapeOam );
   
    if( pTexImageAttr->mappingType == GX_OBJVRAMMODE_CHAR_2D )
    {
        // テクスチャ
        SetTextureParamsFor3DDirect2DMap_( pTexImageAttr, texBaseAddr );
        // UV 0 値を算出する
        CalcUVFor3DDirect2DMap_( pTexImageAttr, charName, &pResult->u0, &pResult->v0 );          
    }else{
        // テクスチャ
        SetTextureParamsFor3DDirect1DMap_( pTexImageAttr, texBaseAddr, shapeOam, charName );
        // UV 0 値を算出する
        CalcUVFor3DDirect1DMap_( &pResult->u0, &pResult->v0 );
    }
       
    // UV 1 値を算出する
    pResult->u1 = pResult->u0 + ( pResult->sx << FX32_SHIFT );
    pResult->v1 = pResult->v0 + ( pResult->sy << FX32_SHIFT );
       
    DoFlip_( NNSi_G2dGetOamFlipH( pOam ), 
             NNSi_G2dGetOamFlipV( pOam ), 
             &pResult->u0, &pResult->u1 , 
             &pResult->v0, &pResult->v1 );
   
    // パレット
    SetPaletteParamsFor3DDirect_( pTexImageAttr, pltBaseAddr, pOam );
}

//------------------------------------------------------------------------------
// 自動Z値オフセット機能を有効にするフラグを設定します。
void NNSi_G2dSetOamSoftEmuAutoZOffsetFlag( BOOL flag )
{
    bAutoZOffsetAdd_ = flag;
}

//------------------------------------------------------------------------------
fx32 NNSi_G2dGetOamSoftEmuAutoZOffset( void )
{
    return zOffset_;
}

//------------------------------------------------------------------------------
void NNSi_G2dResetOamSoftEmuAutoZOffset( void )
{
    zOffset_ = 0;
}

//------------------------------------------------------------------------------
void NNSi_G2dSetOamSoftEmuAutoZOffsetStep( fx32 step )
{
    NNS_G2D_WARNING( step <=  0, "AutoZOffsetStep should be smaller than zero." );
    zOffsetStep_ = step;
}

//------------------------------------------------------------------------------
fx32 NNSi_G2dGetOamSoftEmuAutoZOffsetStep( void )
{
    return zOffsetStep_;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dDrawOneOam3DDirectFast

  Description:  Oamアトリビュートの内容を直接 3D Graphics Engine を 使用して描画します。
                
  Arguments:    pOam          :     [IN]  OAM（GXOamAttr）
                pTexImageAttr :     [IN]  テクスチャ属性 
                texBaseAddr   :     [IN]  テクスチャベースアドレス 
                pltBaseAddr   :     [IN]  パレットベースアドレス 

  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dDrawOneOam3DDirectFast
( 
    const GXOamAttr*                pOam, 
    const NNSG2dImageAttr*          pTexImageAttr,
    u32                             texBaseAddr,
    u32                             pltBaseAddr
)
{
    NNS_G2D_NULL_ASSERT( pOam );
    NNS_G2D_NULL_ASSERT( pTexImageAttr );
    
    {
        SpriteParams             spriteParams;
        const s16 posX = NNSi_G2dRepeatXinScreenArea( NNSi_G2dGetOamX( pOam ) );
        const s16 posY = NNSi_G2dRepeatYinScreenArea( NNSi_G2dGetOamY( pOam ) );
        
        CalcSpriteParams_( pOam, pTexImageAttr, texBaseAddr, pltBaseAddr, &spriteParams );
        
        NNSi_G2dDrawSpriteFast   ( posX, posY, GetFx32DepthValue_( -1 ), 
                               spriteParams.sx, spriteParams.sy,       
                               spriteParams.u0, spriteParams.v0, 
                               spriteParams.u1, spriteParams.v1 );
        //
        // 自動Zオフセット値の更新
        //
        IncreaseAutoZOffset_();   
    }
}


/*---------------------------------------------------------------------------*
  Name:         NNS_G2dDrawOneOam3DDirectWithPosFast

  Description:  Oamアトリビュートの内容を位置を指定して 直接 3D Graphics Engine を 使用して描画します。
                
  Arguments:    
                posX          :        [IN]  位置 X
                posY          :        [IN]  位置 Y
                posZ          :        [IN]  位置 Z
                pOam          :        [IN]  OAM（GXOamAttr）
                pTexImageAttr :        [IN]  テクスチャ属性
                texBaseAddr   :        [IN]  VRAM ベースアドレス
                pltBaseAddr   :        [IN]  パレット ベースアドレス
                
                       
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void    NNS_G2dDrawOneOam3DDirectWithPosFast
( 
    s16                             posX,
    s16                             posY,
    s16                             posZ,
    const GXOamAttr*                pOam, 
    const NNSG2dImageAttr*          pTexImageAttr,
    u32                             texBaseAddr,
    u32                             pltBaseAddr
)
{
    NNS_G2D_NULL_ASSERT( pOam );
    NNS_G2D_NULL_ASSERT( pTexImageAttr );
    
    {
        SpriteParams             spriteParams;
        CalcSpriteParams_( pOam, pTexImageAttr, texBaseAddr, pltBaseAddr, &spriteParams );
        //
        // 描画
        //
        //
        // レンダラ描画の最適化のため、ソフトウェアスプライトモジュールの処理を展開する。 
        //
        // T
        SetQuadTranslation_( pOam, posX, posY, posZ );
        // S
        G3_Scale( spriteParams.sx << FX32_SHIFT, spriteParams.sy << FX32_SHIFT, FX32_ONE );        
        {
           const fx32      size = FX32_ONE;
           //
           // Draw a quad polygon.
           //
           G3_Begin( GX_BEGIN_QUADS );
                
               G3_TexCoord( spriteParams.u0, spriteParams.v1 );
               G3_Vtx10( 0 ,size, 0 ); 
                 
               G3_TexCoord( spriteParams.u1, spriteParams.v1 );
               G3_Vtx10( size, size, 0 ); 
                
               G3_TexCoord( spriteParams.u1, spriteParams.v0 );
               G3_Vtx10( size, 0 , 0 ); 
               
               G3_TexCoord( spriteParams.u0, spriteParams.v0 );
               G3_Vtx10( 0 , 0, 0 );
               
                
           G3_End( );
        }
        //
        // 自動Zオフセット値の更新
        //
        IncreaseAutoZOffset_();   
    }
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dDrawOneOam3DDirectWithPosAffine

  Description:  Oamアトリビュートの内容を位置を指定して 直接 3D Graphics Engine を 使用して描画します。
                
  Arguments:    
                posX          :        [IN]  位置 X
                posY          :        [IN]  位置 Y
                posZ          :        [IN]  位置 Z
                pOam          :        [IN]  OAM（GXOamAttr）
                pTexImageAttr :        [IN]  テクスチャ属性
                texBaseAddr   :        [IN]  VRAM ベースアドレス
                pltBaseAddr   :        [IN]  パレット ベースアドレス
                pMtx          :        [IN]  アフィン変換行列
                       
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void    NNS_G2dDrawOneOam3DDirectWithPosAffineFast
( 
    s16                             posX,
    s16                             posY,
    s16                             posZ,
    const GXOamAttr*                pOam, 
    const NNSG2dImageAttr*          pTexImageAttr,
    u32                             texBaseAddr,
    u32                             pltBaseAddr,
    const MtxFx22*                  pMtx
)
{
    NNS_G2D_NULL_ASSERT( pOam );
    NNS_G2D_NULL_ASSERT( pTexImageAttr );
    
    {
        SpriteParams             spriteParams;
        CalcSpriteParams_( pOam, pTexImageAttr, texBaseAddr, pltBaseAddr, &spriteParams );
        
        //
        // 描画
        //
        if( G2_GetOBJEffect( pOam ) == GX_OAM_EFFECT_AFFINE )
        {
            NNSi_G2dDrawSpriteWithMtxFast               
                ( posX, posY, GetFx32DepthValue_( posZ ), 
                  spriteParams.sx, spriteParams.sy, pMtx, 
                  spriteParams.u0, spriteParams.v0, 
                  spriteParams.u1, spriteParams.v1 );
        }else{
            NNSi_G2dDrawSpriteWithMtxDoubleAffineFast   
                ( posX, posY, GetFx32DepthValue_( posZ ), 
                spriteParams.sx, spriteParams.sy, pMtx, 
                spriteParams.u0, spriteParams.v0, 
                spriteParams.u1, spriteParams.v1 );
        }
        
        //
        // 自動Zオフセット値の更新
        //
        IncreaseAutoZOffset_();   
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetOamSoftEmuSpriteParamCache

  Description:  ソフトウエアスプライト描画に使用する、
                パラメータのキャッシュを設定します。
                
                キャッシュを利用した描画は
                NNS_G2dDrawOneOam3DDirectUsingParamCacheFast()関数で行います。
                
                パラメータとはUVパラメータのことなので、同一のテクスチャを参照する
                スプライトを大量に書く際に、UV値の計算、設定を一度にまとめ 
                効率よく処理を行うことが可能となります。
                
                
                NNS_G2dDrawOneOam3DDirectUsingParamCache()の描画
                  
                  処理 = UVパラメータ設定 x 1 + 描画 x N
                  
                通常の描画
                  処理 = ( UVパラメータ設定 + 描画 ) x N
                  
                
                
  Arguments:    
                pOam          :        [IN]  OBJアトリビュート
                pTexImageAttr :        [IN]  テクスチャ属性
                texBaseAddr   :        [IN]  VRAM ベースアドレス
                pltBaseAddr   :        [IN]  パレット ベースアドレス
                       
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void    NNS_G2dSetOamSoftEmuSpriteParamCache
( 
    const GXOamAttr*                pOam, 
    const NNSG2dImageAttr*          pTexImageAttr,
    u32                             texBaseAddr,
    u32                             pltBaseAddr
)
{
    {
        SpriteParams             spriteParams;
        CalcSpriteParams_( pOam, pTexImageAttr, texBaseAddr, pltBaseAddr, &spriteParams );
    
        //
        // キャッシュに格納
        //
        softwareSpreiteParamCache_.u0 = spriteParams.u0;
        softwareSpreiteParamCache_.v0 = spriteParams.v0;
        softwareSpreiteParamCache_.u1 = spriteParams.u1;
        softwareSpreiteParamCache_.v1 = spriteParams.v1;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dDrawOneOam3DDirectUsingParamCacheFast

  Description:  パラメータのキャッシュを使用してOAMをスプライト描画します。
                実行前にキャッシュパラメータの設定を行ってください。
                キャッシュパラメータの設定は
                NNS_G2dSetOamSoftEmuSpriteParamCache()関数で行います。
                
                関数呼び出しの前後で、3D グラフィックスエンジンのカレント行列
                は保存されません。
                
                
                パラメータとはUVパラメータのことなので、同一のテクスチャを参照する
                スプライトを大量に書く際に、UV値の計算、設定を一度にまとめ 
                効率よく処理を行うことが可能となります。
                
                
                クローン描画時のフリップフラグ変更はパフォーマンスへの影響が大きいため、
                サポートしないことにしました。
                したがって、本関数では、OAMのフリップフラグが無視される点にご注意ください。
                (UVパラメータはNNS_G2dSetOamSoftEmuSpriteParamCache()関数実行時に
                決定されてしまいます。)
                フリップしたスプライトを描画する場合、
                NNS_G2dSetOamSoftEmuSpriteParamCache()の引数としてフリップ設定した
                OAM（GXOamAttr）を渡し、キャッシュパラメータを更新する必要があります。
                
                
                NNS_G2dDrawOneOam3DDirectUsingParamCache()の描画
                  
                  処理 = UVパラメータ設定 x 1 + 描画 x N
                  
                通常の描画
                  処理 = ( UVパラメータ設定 + 描画 ) x N
                
  Arguments:    
                posX          :        [IN]  位置 X
                posY          :        [IN]  位置 Y
                posZ          :        [IN]  位置 Z
                pOam          :        [IN]  OAM（GXOamAttr）
                       
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void    NNS_G2dDrawOneOam3DDirectUsingParamCacheFast
( 
    s16                             posX,
    s16                             posY,
    s16                             posZ,
    const GXOamAttr*                pOam
)
{
    {
        const GXOamShape  shapeOam = NNS_G2dGetOAMSize( pOam );
        const int sx       = NNS_G2dGetOamSizeX( &shapeOam );
        const int sy       = NNS_G2dGetOamSizeY( &shapeOam );        
        
        //
        // パラメータキャッシュの値を使用する。
        //
        const fx32   u0 = softwareSpreiteParamCache_.u0, 
                     u1 = softwareSpreiteParamCache_.u1, 
                     v0 = softwareSpreiteParamCache_.v0, 
                     v1 = softwareSpreiteParamCache_.v1;
        //
        // 描画
        //
        //
        // 描画の最適化のため、ソフトウェアスプライトモジュールの処理を展開する。 
        
        //
        // T
        //G3_Translate( posX << FX32_SHIFT, posY << FX32_SHIFT, GetFx32DepthValue_( posZ ) );    
        SetQuadTranslation_( pOam, posX, posY, posZ );
        
        // S
        G3_Scale( sx << FX32_SHIFT, sy << FX32_SHIFT, FX32_ONE );        
        {
           const fx32      size = FX32_ONE;
           //
           // Draw a quad polygon.
           //
           G3_Begin( GX_BEGIN_QUADS );
               
               G3_TexCoord( u0, v1 );
               G3_Vtx10( 0 ,size, 0 ); 
               
               
               G3_TexCoord( u1, v1 );
               G3_Vtx10( size, size, 0 ); 
               
               G3_TexCoord( u1, v0 );
               G3_Vtx10( size, 0 , 0 ); 
               
               G3_TexCoord( u0, v0 );
               G3_Vtx10( 0 , 0, 0 );
               
                    
           G3_End( );
        }
        //
        // 自動Zオフセット値の更新
        //
        IncreaseAutoZOffset_();
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetOamSoftEmuUVFlipCorrectFunc

  Description:  Oamソフトウエアスプライトエミュレーション 
                で使用される UV値補正関数ポインタを設定します。
                
  Arguments:    pFunc          :        [IN]  UV値補正関数ポインタ
                       
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void    NNS_G2dSetOamSoftEmuUVFlipCorrectFunc( NNS_G2dOamSoftEmuUVFlipCorrectFunc pFunc )
{
    NNS_G2D_NULL_ASSERT( pFunc );
    s_pUVFlipCorrectFunc = pFunc;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dResetOamSoftEmuUVFlipCorrectFunc

  Description:  Oamソフトウエアスプライトエミュレーション 
                で使用される UV値補正関数ポインタを設定します。
                
  Arguments:    なし
                       
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void    NNS_G2dResetOamSoftEmuUVFlipCorrectFunc()
{
    s_pUVFlipCorrectFunc = NULL;
}


