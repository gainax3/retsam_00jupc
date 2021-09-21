/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g3d
  File:     glbstate_inline.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.15 $
 *---------------------------------------------------------------------------*/

//
// AUTHOR: Kenji Nishida
//

#ifndef NNSG3D_GLBSTATE_INLINE_H_
#define NNSG3D_GLBSTATE_INLINE_H_
//
// DO NOT INCLUDE THIS FILE DIRECTLY
//

#ifdef __cplusplus
extern "C" {
#endif


/*---------------------------------------------------------------------------*
    NNS_G3dGlbLookAt

 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dGlbLookAt(const VecFx32 * camPos,
                 const VecFx32 * camUp,
                 const VecFx32 * target)
{
    NNS_G3dGlb.camPos = *camPos;
    NNS_G3dGlb.camUp  = *camUp;
    NNS_G3dGlb.camTarget = *target;
    MTX_LookAt(camPos, camUp, target, &NNS_G3dGlb.cameraMtx);
    NNS_G3dGlb.flag &= ~(NNS_G3D_GLB_FLAG_INVCAMERA_UPTODATE     |
                         NNS_G3D_GLB_FLAG_BASECAMERA_UPTODATE    |
                         NNS_G3D_GLB_FLAG_INVBASECAMERA_UPTODATE |
                         NNS_G3D_GLB_FLAG_INVCAMERAPROJ_UPTODATE);


}


/*---------------------------------------------------------------------------*
    NNS_G3dGlbFrustum

 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dGlbFrustum(fx32 t,
               fx32 b,
               fx32 l,
               fx32 r,
               fx32 n,
               fx32 f)
{
    MTX_Frustum(t,
                b,
                l,
                r,
                n,
                f,
                &NNS_G3dGlb.projMtx);

    NNS_G3dGlb.flag &= ~(NNS_G3D_GLB_FLAG_INVPROJ_UPTODATE |
                         NNS_G3D_GLB_FLAG_INVCAMERAPROJ_UPTODATE);
}

/*---------------------------------------------------------------------------*
    NNS_G3dGlbFrustumW

 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dGlbFrustumW(fx32 t,
                   fx32 b,
                   fx32 l,
                   fx32 r,
                   fx32 n,
                   fx32 f,
                   fx32 scaleW)
{
    MTX_FrustumW(t,
                 b,
                 l,
                 r,
                 n,
                 f,
                 scaleW,
                 &NNS_G3dGlb.projMtx);
    NNS_G3dGlb.flag &= ~(NNS_G3D_GLB_FLAG_INVPROJ_UPTODATE |
                         NNS_G3D_GLB_FLAG_INVCAMERAPROJ_UPTODATE);
}



/*---------------------------------------------------------------------------*
    NNS_G3dGlbPerspective

 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dGlbPerspective(fx32 fovySin,
                      fx32 fovyCos,
                      fx32 aspect,
                      fx32 n,
                      fx32 f)
{
    MTX_Perspective(fovySin,
                    fovyCos,
                    aspect,
                    n,
                    f,
                    &NNS_G3dGlb.projMtx);
    NNS_G3dGlb.flag &= ~(NNS_G3D_GLB_FLAG_INVPROJ_UPTODATE |
                         NNS_G3D_GLB_FLAG_INVCAMERAPROJ_UPTODATE);
}


/*---------------------------------------------------------------------------*
    NNS_G3dGlbPerspectiveW

 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dGlbPerspectiveW(fx32 fovySin,
                       fx32 fovyCos,
                       fx32 aspect,
                       fx32 n,
                       fx32 f,
                       fx32 scaleW)
{
    MTX_PerspectiveW(fovySin,
                     fovyCos,
                     aspect,
                     n,
                     f,
                     scaleW,
                     &NNS_G3dGlb.projMtx);
    NNS_G3dGlb.flag &= ~(NNS_G3D_GLB_FLAG_INVPROJ_UPTODATE |
                         NNS_G3D_GLB_FLAG_INVCAMERAPROJ_UPTODATE);
}


/*---------------------------------------------------------------------------*
    NNS_G3dGlbOrtho

 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dGlbOrtho(fx32 t,
                fx32 b,
                fx32 l,
                fx32 r,
                fx32 n,
                fx32 f)
{
    MTX_Ortho(t,
              b,
              l,
              r,
              n,
              f,
              &NNS_G3dGlb.projMtx);
    NNS_G3dGlb.flag &= ~(NNS_G3D_GLB_FLAG_INVPROJ_UPTODATE |
                         NNS_G3D_GLB_FLAG_INVCAMERAPROJ_UPTODATE);
}


/*---------------------------------------------------------------------------*
    NNS_G3dGlbOrthoW

 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dGlbOrthoW(fx32 t,
                 fx32 b,
                 fx32 l,
                 fx32 r,
                 fx32 n,
                 fx32 f,
                 fx32 scaleW)
{
    MTX_OrthoW(t,
               b,
               l,
               r,
               n,
               f,
               scaleW,
               &NNS_G3dGlb.projMtx);
    NNS_G3dGlb.flag &= ~(NNS_G3D_GLB_FLAG_INVPROJ_UPTODATE |
                         NNS_G3D_GLB_FLAG_INVCAMERAPROJ_UPTODATE);
}


/*---------------------------------------------------------------------------*
    NNS_G3dGlbSetProjectionMtx

 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dGlbSetProjectionMtx(const MtxFx44* m)
{
    NNS_G3D_NULL_ASSERT(m);
    MI_CpuCopyFast(m, &NNS_G3dGlb.projMtx, sizeof(MtxFx44));
    NNS_G3dGlb.flag &= ~(NNS_G3D_GLB_FLAG_INVPROJ_UPTODATE |
                         NNS_G3D_GLB_FLAG_INVCAMERAPROJ_UPTODATE);
}




/*---------------------------------------------------------------------------*
    NNS_G3dGlbSetBaseRot

 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dGlbSetBaseRot(const MtxFx33* pRot)
{
    NNS_G3D_NULL_ASSERT(pRot);

    MTX_Copy33(pRot, &NNS_G3dGlb.prmBaseRot);
    NNS_G3dGlb.flag &= ~(NNS_G3D_GLB_FLAG_BASECAMERA_UPTODATE |
                         NNS_G3D_GLB_FLAG_INVBASE_UPTODATE   |
                         NNS_G3D_GLB_FLAG_INVBASECAMERA_UPTODATE);
}


/*---------------------------------------------------------------------------*
    NNS_G3dGlbSetViewPort

 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dGlbSetViewPort(int x1, int y1, int x2, int y2)
{
    GX_VIEWPORT_ASSERT(x1, y1, x2, y2);

    NNS_G3dGlb.prmViewPort = GX_PACK_VIEWPORT_PARAM(x1, y1, x2, y2);
}

/*---------------------------------------------------------------------------*
    NNS_G3dGlbGetCameraMtx

 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE const MtxFx43*
NNS_G3dGlbGetCameraMtx(void)
{
    return &NNS_G3dGlb.cameraMtx;
}


/*---------------------------------------------------------------------------*
    NNS_G3dGlbGetProjectionMtx

 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE const MtxFx44*
NNS_G3dGlbGetProjectionMtx(void)
{
    return &NNS_G3dGlb.projMtx;
}


/*---------------------------------------------------------------------------*
    NNS_G3dGlbGetBaseRot

 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE const MtxFx33*
NNS_G3dGlbGetBaseRot(void)
{
    return &NNS_G3dGlb.prmBaseRot;
}


/*---------------------------------------------------------------------------*
    NNS_G3dGlbGetBaseScale

 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE const VecFx32*
NNS_G3dGlbGetBaseScale(void)
{
    return &NNS_G3dGlb.prmBaseScale;
}


/*---------------------------------------------------------------------------*
    NNS_G3dGlbGetBaseTrans

 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE const VecFx32*
NNS_G3dGlbGetBaseTrans(void)
{
    return &NNS_G3dGlb.prmBaseTrans;
}


/*---------------------------------------------------------------------------*
    NNS_G3dGlbGetCameraPos

 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE const VecFx32*
NNS_G3dGlbGetCameraPos(void)
{
    return &NNS_G3dGlb.camPos;
}


/*---------------------------------------------------------------------------*
    NNS_G3dGlbGetCameraUp

 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE const VecFx32*
NNS_G3dGlbGetCameraUp(void)
{
    return &NNS_G3dGlb.camUp;
}


/*---------------------------------------------------------------------------*
    NNS_G3dGlbGetCameraTarget

 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE const VecFx32*
NNS_G3dGlbGetCameraTarget(void)
{
    return &NNS_G3dGlb.camTarget;
}







/*---------------------------------------------------------------------------*
    NNS_G3dGlbFlush

 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dGlbFlush(void)
{
    NNS_G3dGlbFlushP();
}



/*---------------------------------------------------------------------------*
    NNS_G3dGlbFlushAlt

 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dGlbFlushAlt(void)
{
    NNS_G3dGlbFlushWVP();
}


/*---------------------------------------------------------------------------*
    NNS_G3dGlbGetInvCameraMtx

 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE const MtxFx43*
NNS_G3dGlbGetInvCameraMtx(void)
{
    return NNS_G3dGlbGetInvV();
}


/*---------------------------------------------------------------------------*
    NNS_G3dGlbGetSrtCameraMtx

 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE const MtxFx43*
NNS_G3dGlbGetSrtCameraMtx(void)
{
    return NNS_G3dGlbGetWV();
}


/*---------------------------------------------------------------------------*
    NNS_G3dGlbGetInvSrtCameraMtx

 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE const MtxFx43*
NNS_G3dGlbGetInvSrtCameraMtx(void)
{
    return NNS_G3dGlbGetInvWV();
}




#ifdef __cplusplus
}
#endif

#endif
