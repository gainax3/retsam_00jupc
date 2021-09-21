/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g2d
  File:     g2d_OamSoftwareSpriteDraw.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.8 $
 *---------------------------------------------------------------------------*/
 
#ifndef NNS_G2D_OAMSOFTWARESPRITEEMU_H_
#define NNS_G2D_OAMSOFTWARESPRITEEMU_H_

#include <nitro.h>
#include <nnsys/g2d/g2d_Image.h> // TODO : この依存はOKか？要検討

#ifdef __cplusplus
extern "C" {
#endif


/*---------------------------------------------------------------------------*
  Name:         NNS_G2dUVFlipCorrectFunc

  Description:  Oamソフトウエアスプライトエミュレーション 
                で使用される UV値補正関数ポインタ
                
 *---------------------------------------------------------------------------*/
typedef void (*NNS_G2dOamSoftEmuUVFlipCorrectFunc)( fx32*   pRetU0,  
                                                    fx32*   pRetV0, 
                                                    fx32*   pRetU1,  
                                                    fx32*   pRetV1,
                                                    BOOL   bFilpH,
                                                    BOOL   bFilpV );

//------------------------------------------------------------------------------
void    NNS_G2dDrawOneOam3DDirectFast
( 
    const GXOamAttr*                pOam, 
    const NNSG2dImageAttr*          pTexImageAttr,
    u32                             texBaseAddr,
    u32                             pltBaseAddr
);



//------------------------------------------------------------------------------
void    NNS_G2dDrawOneOam3DDirectWithPosFast
( 
    s16                             posX,
    s16                             posY,
    s16                             posZ,
    const GXOamAttr*                pOam, 
    const NNSG2dImageAttr*          pTexImageAttr,
    u32                             texBaseAddr,
    u32                             pltBaseAddr
);



//------------------------------------------------------------------------------
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
);

//------------------------------------------------------------------------------
void NNS_G2dSetOamSoftEmuSpriteParamCache
( 
    const GXOamAttr*                pOam, 
    const NNSG2dImageAttr*          pTexImageAttr,
    u32                             texBaseAddr,
    u32                             pltBaseAddr
);

//------------------------------------------------------------------------------
// NNS_G2dDrawOneOam3DDirectUsingParamCache() は用意されません。
void NNS_G2dDrawOneOam3DDirectUsingParamCacheFast
( 
    s16                             posX,
    s16                             posY,
    s16                             posZ,
    const GXOamAttr*                pOam
);

//------------------------------------------------------------------------------
// UV 値補正コールバック関連
void    NNS_G2dSetOamSoftEmuUVFlipCorrectFunc( NNS_G2dOamSoftEmuUVFlipCorrectFunc pFunc );
void    NNS_G2dResetOamSoftEmuUVFlipCorrectFunc();


//------------------------------------------------------------------------------
// 自動Z値オフセット機能関連

void NNSi_G2dSetOamSoftEmuAutoZOffsetFlag   ( BOOL flag );
fx32 NNSi_G2dGetOamSoftEmuAutoZOffset       ( void );
void NNSi_G2dResetOamSoftEmuAutoZOffset     ( void );
void NNSi_G2dSetOamSoftEmuAutoZOffsetStep   ( fx32 step );
fx32 NNSi_G2dGetOamSoftEmuAutoZOffsetStep   ( void );



//------------------------------------------------------------------------------
// inline 関数
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
NNS_G2D_INLINE void    NNS_G2dDrawOneOam3DDirect
( 
    const GXOamAttr*                pOam, 
    const NNSG2dImageAttr*          pTexImageAttr,
    u32                             texBaseAddr,
    u32                             pltBaseAddr
)
{
    G3_PushMtx();
       NNS_G2dDrawOneOam3DDirectFast
       ( 
           pOam, pTexImageAttr, 
           texBaseAddr, pltBaseAddr
       );
    G3_PopMtx(1);
}
//------------------------------------------------------------------------------
NNS_G2D_INLINE void    NNS_G2dDrawOneOam3DDirectWithPos
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
    G3_PushMtx();
       NNS_G2dDrawOneOam3DDirectWithPosFast
       ( 
           posX, posY, posZ, 
           pOam, pTexImageAttr, 
           texBaseAddr, pltBaseAddr
       );
    G3_PopMtx(1);
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE void    NNS_G2dDrawOneOam3DDirectWithPosAffine
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
    G3_PushMtx();
       NNS_G2dDrawOneOam3DDirectWithPosAffineFast
       ( 
           posX, posY, posZ, 
           pOam, pTexImageAttr, 
           texBaseAddr, pltBaseAddr, pMtx 
       );
    G3_PopMtx(1);
}


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // NNS_G2D_OAMSOFTWARESPRITEEMU_H_
