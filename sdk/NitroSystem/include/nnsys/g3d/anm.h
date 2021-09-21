/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g3d
  File:     anm.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.16 $
 *---------------------------------------------------------------------------*/

//
// AUTHOR: Kenji Nishida
//

#ifndef NNSG3D_ANM_H_
#define NNSG3D_ANM_H_

#include <nnsys/g3d/config.h>
#include <nnsys/g3d/binres/res_struct.h>
#include <nnsys/g3d/kernel.h>

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*
    アニメーションの動作概略:

    matID, nodeIDに関してアニメーション処理を行う場合、
    SBCインタプリタからNNSG3dRenderObjに登録されているBlend関数が呼ばれる。
    NNSG3d[Mat|Jnt]AnmResult*には、結果(内容にはBlendMatが責任を負う)が
    格納されることになり、NNSG3dAnmObj*はNNSG3dRenderObjのデータメンバである、
    アニメーションオブジェクトのリストである。
 *---------------------------------------------------------------------------*/

////////////////////////////////////////////////////////////////////////////////
//
// 構造体定義及びtypedef
//


struct NNSG3dResMdl_;
/*---------------------------------------------------------------------------*
    NNSG3dAnimInitFunc

    NNSG3dAnmObjを初期化する関数の型。void*はアニメーションリソースへのポインタ。
    アニメーションリソースの種類によってNNSG3dAnmObjの初期化方法は異なるため。
 *---------------------------------------------------------------------------*/
typedef void (*NNSG3dAnimInitFunc)(NNSG3dAnmObj*,
                                   void*,
                                   const NNSG3dResMdl*);






/*---------------------------------------------------------------------------*
    NNSG3dAnmObjInitFunc

    category0とcategory1はNNSG3dResAnmHeaderのものと同じである。
 *---------------------------------------------------------------------------*/
typedef struct
{
    u8                  category0;
    u8                  dummy_;
    u16                 category1;
    NNSG3dAnimInitFunc  func;
}
NNSG3dAnmObjInitFunc;


/*---------------------------------------------------------------------------*
    マテリアルアニメーションの動作について

    マテリアルアニメーションの計算はNNSG3dMatAnmResultに次々と値を上書きしていき、
    最後にジオメトリエンジンにデータを送信することで行われる。
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_MATANM_RESULTFLAG_TEXMTX_SCALEONE  = 0x00000001,
    NNS_G3D_MATANM_RESULTFLAG_TEXMTX_ROTZERO   = 0x00000002,
    NNS_G3D_MATANM_RESULTFLAG_TEXMTX_TRANSZERO = 0x00000004,

    NNS_G3D_MATANM_RESULTFLAG_TEXMTX_SET       = 0x00000008,
    NNS_G3D_MATANM_RESULTFLAG_TEXMTX_MULT      = 0x00000010,
    NNS_G3D_MATANM_RESULTFLAG_WIREFRAME        = 0x00000020
}
NNSG3dMatAnmResultFlag;


/*---------------------------------------------------------------------------*
    NNSG3dMatAnmResult

    計算されたマテリアル情報を格納するための構造体。
    SBCのMATコマンド内部で計算される。
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dMatAnmResult_
{
    NNSG3dMatAnmResultFlag flag;
    u32                    prmMatColor0;
    u32                    prmMatColor1;
    u32                    prmPolygonAttr;
    u32                    prmTexImage;
    u32                    prmTexPltt;

    // flagのSCALEONE/ROTZERO/TRANSZEROの場合は
    // 対応する値はセットされていない。
    fx32                   scaleS, scaleT;
    fx16                   sinR, cosR;
    fx32                   transS, transT;

    u16                    origWidth, origHeight;
    fx32                   magW, magH;
}
NNSG3dMatAnmResult;


/*---------------------------------------------------------------------------*
    ジョイントアニメーションの動作について

    クォータニオンはサポートされていない。
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_JNTANM_RESULTFLAG_SCALE_ONE      = 0x00000001,
    NNS_G3D_JNTANM_RESULTFLAG_ROT_ZERO       = 0x00000002,
    NNS_G3D_JNTANM_RESULTFLAG_TRANS_ZERO     = 0x00000004,
    NNS_G3D_JNTANM_RESULTFLAG_SCALEEX0_ONE   = 0x00000008,
    NNS_G3D_JNTANM_RESULTFLAG_SCALEEX1_ONE   = 0x00000010,
    NNS_G3D_JNTANM_RESULTFLAG_MAYA_SSC       = 0x00000020
//    NNS_G3D_JNTANM_RESULTFLAG_ROT_QUATERNION = 0x00000040
}
NNSG3dJntAnmResultFlag;


/*---------------------------------------------------------------------------*
    NNSG3dJntAnmResult

    計算されたノード情報を格納するための構造体。
    SBCのNODEDESCコマンド内部で計算される。
    scaleEx0とscaleEx1はMayaのSSC,Si3dのClassic Scale offの場合に
    付加的なスケール情報を格納するために使用される。
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dJntAnmResult_
{
    NNSG3dJntAnmResultFlag flag;
    VecFx32                scale;
    VecFx32                scaleEx0;
    VecFx32                scaleEx1;
    MtxFx33                rot;
    VecFx32                trans;
}
NNSG3dJntAnmResult;


/*---------------------------------------------------------------------------*
    ビジビリティアニメーションの動作について

    ジョイントが丸ごと見えたり見えなくなったりします。
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
    NNSG3dVisAnmResult

    計算されたビジビリティ情報を格納するための構造体。
    SBCのNODEコマンド内部で計算される。
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dVisAnmResult_
{
    BOOL isVisible;
}
NNSG3dVisAnmResult;


// マテリアルアニメーション計算関数
typedef void (*NNSG3dFuncAnmMat)(NNSG3dMatAnmResult*,
                                 const NNSG3dAnmObj*,
                                 u32);

// ジョイントアニメーション計算関数
typedef void (*NNSG3dFuncAnmJnt)(NNSG3dJntAnmResult*,
                                 const NNSG3dAnmObj*,
                                 u32);

// ビジビリティアニメーション計算関数
typedef void (*NNSG3dFuncAnmVis)(NNSG3dVisAnmResult*,
                                 const NNSG3dAnmObj*,
                                 u32);


////////////////////////////////////////////////////////////////////////////////
//
// 関数の宣言
//

//
// マテリアルアニメーションブレンド関数のデフォルト
// 単純に、後で計算されたアニメーション結果が優先する。
//
BOOL NNSi_G3dAnmBlendMat(NNSG3dMatAnmResult* pResult,
                         const NNSG3dAnmObj* pAnmObj,
                         u32 matID);

BOOL NNSi_G3dAnmBlendJnt(NNSG3dJntAnmResult*,
                         const NNSG3dAnmObj*,
                         u32);

BOOL NNSi_G3dAnmBlendVis(NNSG3dVisAnmResult*,
                         const NNSG3dAnmObj*,
                         u32);

////////////////////////////////////////////////////////////////////////////////
//
// グローバル変数
//

//
// デフォルトのアニメーションブレンド関数へのポインタ
// NNS_G3dRenderObjInitにおいてNNSG3dRenderObjを初期設定するために使用される。
//
extern NNSG3dFuncAnmBlendMat NNS_G3dFuncBlendMatDefault;
extern NNSG3dFuncAnmBlendJnt NNS_G3dFuncBlendJntDefault;
extern NNSG3dFuncAnmBlendVis NNS_G3dFuncBlendVisDefault;

//
// デフォルトのアニメーション計算関数へのポインタ
// NNS_G3dAnmObjInitにおいてNNSG3dAnmObjを初期設定するために使用される。
//
extern NNSG3dFuncAnmMat NNS_G3dFuncAnmMatNsBmaDefault;
extern NNSG3dFuncAnmMat NNS_G3dFuncAnmMatNsBtpDefault;
extern NNSG3dFuncAnmMat NNS_G3dFuncAnmMatNsBtaDefault;
extern NNSG3dFuncAnmJnt NNS_G3dFuncAnmJntNsBcaDefault;
extern NNSG3dFuncAnmVis NNS_G3dFuncAnmVisNsBvaDefault;

extern u32 NNS_G3dAnmFmtNum;
extern NNSG3dAnmObjInitFunc NNS_G3dAnmObjInitFuncArray[NNS_G3D_ANMFMT_MAX];


#ifdef __cplusplus
}/* extern "C" */
#endif

#endif
