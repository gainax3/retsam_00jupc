/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g3d
  File:     sbc.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.24 $
 *---------------------------------------------------------------------------*/

//
// AUTHOR: Kenji Nishida
//

#ifndef NNSG3D_SBC_H_
#define NNSG3D_SBC_H_

#include <nnsys/g3d/config.h>
#include <nnsys/g3d/binres/res_struct.h>
#include <nnsys/g3d/kernel.h>
#include <nnsys/g3d/anm.h>
#include <nnsys/g3d/cgtool.h>

#ifdef __cplusplus
extern "C" {
#endif


////////////////////////////////////////////////////////////////////////////////
//
// 構造体定義及びtypedef
//

/*---------------------------------------------------------------------------*
    NNSG3dRSFlag

    NNSG3dRS内にある1ビットフラグの集合。
    NNS_G3dDrawやSBCコマンド関数内でセットされたり参照されたりする。

    NNS_G3D_RSFLAG_SKIPをコールバック内で設定すると、そのSBC命令の最後まで、
    コールバックの判定以外の動作をスキップするようになっている。
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_RSFLAG_NODE_VISIBLE           = 0x00000001,    // NODEコマンドでON/OFFされる
    NNS_G3D_RSFLAG_MAT_TRANSPARENT        = 0x00000002,    // MATコマンドでON/OFFされる
    NNS_G3D_RSFLAG_CURRENT_NODE_VALID     = 0x00000004,    // NODEコマンドでONされる
    NNS_G3D_RSFLAG_CURRENT_MAT_VALID      = 0x00000008,    // MATコマンドでONされる
    NNS_G3D_RSFLAG_CURRENT_NODEDESC_VALID = 0x00000010,
    NNS_G3D_RSFLAG_RETURN                 = 0x00000020,    // RETコマンドでONされる
    NNS_G3D_RSFLAG_SKIP                   = 0x00000040,    // コールバック用ユーザーフラグ(コマンドの内の次のコールバックまでスキップ)

    // 以下は実行前にNNSG3dRenderObjによって決定される値
    NNS_G3D_RSFLAG_OPT_RECORD             = 0x00000080,    // NNSG3dRenderObjのrecJntAnm, recMatAnmに計算結果を記録します。
    NNS_G3D_RSFLAG_OPT_NOGECMD            = 0x00000100,    // ジオメトリコマンドを送信しません。
    NNS_G3D_RSFLAG_OPT_SKIP_SBCDRAW       = 0x00000200,    // Draw系SBCコマンドをスキップします。コールバックもスキップします。
    NNS_G3D_RSFLAG_OPT_SKIP_SBCMTXCALC    = 0x00000400     // MtxCalc系SBCコマンドをスキップします。コールバックもスキップします。

    // Draw系SBCコマンド一覧
    // NODE         --- NNSi_G3dFuncSbc_NODE
    // MTX          --- NNSi_G3dFuncSbc_MTX
    // MAT          --- NNSi_G3dFuncSbc_MAT
    // SHP          --- NNSi_G3dFuncSbc_SHP
    // NODEDESC_BB  --- NNSi_G3dFuncSbc_BB
    // NODEDESC_BBY --- NNSi_G3dFuncSbc_BBY
    // POSSCALE     --- NNSi_G3dFuncSbc_POSSCALE

    // MtxCalc系SBCコマンド一覧
    // NODEDESC     --- NNSi_G3dFuncSbc_NODEDESC
}
NNSG3dRSFlag;


/*---------------------------------------------------------------------------*
    NNSG3dRS

    描画エンジンの状態を格納するのに使用。通常はDTCMを使用するためスタックに
    確保されることに注意。描画中にやりとりする変数とかを格納。NNS_G3dRSにポインタが
    格納されている。
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dRS_
{
    // 基本情報
    u8*                               c;                   // 現在参照しているSBC命令へのポインタ
    NNSG3dRenderObj*                  pRenderObj;
    u32                               flag;                // NNSG3dRSFlag 

    // コールバック関数ベクタ
    NNSG3dSbcCallBackFunc             cbVecFunc[NNS_G3D_SBC_COMMAND_NUM];   // コールバック関数のベクタ
    u8                                cbVecTiming[NNS_G3D_SBC_COMMAND_NUM]; // NNSG3dSbcCallBackTimingのベクタ

    // flagのNNS_G3D_RSFLAG_CURRENT_NODE_VALIDがONのとき、直前に呼ばれた(現在実行中の)
    // NODEコマンドのnodeIDが格納されている。
    u8                                currentNode;

    // flagのNNS_G3D_RSFLAG_CURRENT_MAT_VALIDがONのとき、直前に呼ばれた(現在実行中の)
    // MATコマンドのマテリアルIDが格納されている。
    // MATコマンドで指定されているマテリアルIDが同じ場合、マテリアル情報の
    // 送信を省略するので、コールバック等でマテリアルデータを変更した場合は、
    // NNS_G3D_RSFLAG_CURRENT_MAT_VALIDをOFFにする必要がある。
    u8                                currentMat;

    // flagのNNS_G3D_RSFLAG_CURRENT_NODEDESC_VALIDがONのとき、直前に呼ばれた(現在実行中の)
    // NODEDESCコマンドのノードIDが格納されている。
    u8                                currentNodeDesc;

    u8                                dummy_;
    // NNSG3dMatAnmResult,NNSG3dJntAnmResult,NNSG3dVisAnmResultが計算中である場合には
    // このメンバにポインタが格納されている。
    // コールバックで計算結果を変更することができるようになっている。

    // 最初のMATコマンドが呼び出されるまではNULL
    // その後は直前に実行されたMATコマンドによる計算結果を格納
    NNSG3dMatAnmResult*               pMatAnmResult;

    // 最初のNODEDESCコマンドが呼び出されるまではNULL
    // その後は直前に実行されたNODEDESCコマンドによる計算結果を格納
    NNSG3dJntAnmResult*               pJntAnmResult;

    // 最初のNODEコマンドが呼び出されるまではNULL
    // その後は直前に実行されたNODEコマンドによる計算結果を格納
    NNSG3dVisAnmResult*               pVisAnmResult;

    // マテリアルデータが既にキャッシュされている場合はビットが立っている。
    // データ本体はNNSG3dRSOnHeapのmatCacheに格納されている。
    u32                               isMatCached[NNS_G3D_SIZE_MAT_MAX / 32];

    // スケール関連データを格納する場合、データが(1.0, 1.0, 1.0)である場合は、
    // データを格納する代わりに対応するビットをONにしておくことで代用する。
    u32                               isScaleCacheOne[NNS_G3D_SIZE_JNT_MAX / 32];

#if (NNS_G3D_USE_EVPCACHE)
    u32                               isEvpCached[NNS_G3D_SIZE_JNT_MAX / 32];
#endif

    // resMdlの中で繰り返し使う情報をキャッシュしておく
    // ユーザーが触らなければいけないことはない。
    const NNSG3dResNodeInfo*          pResNodeInfo;
    const NNSG3dResMat*               pResMat;
    const NNSG3dResShp*               pResShp;
    fx32                              posScale;
    fx32                              invPosScale;
    NNSG3dGetJointScale               funcJntScale;
    NNSG3dSendJointSRT                funcJntMtx;
    NNSG3dSendTexSRT                  funcTexMtx;

    NNSG3dMatAnmResult                tmpMatAnmResult;
    NNSG3dJntAnmResult                tmpJntAnmResult;
    NNSG3dVisAnmResult                tmpVisAnmResult;
}
NNSG3dRS;


/*---------------------------------------------------------------------------*
    NNSG3dRSOnGlb

    描画エンジンの状態のうち、サイズが大きいためスタックに入れるのには適さない
    ものを格納。グローバル変数のNNS_G3dRSOnGlbを使用する。
    終始メインメモリに確保されたままの状態である。
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dRSOnGlb_
{
    //
    // SbcのMAT命令によりキャッシングされたマテリアル情報
    // NNSG3dRSのisMatChachedの対応ビットがONである場合にデータは有効である。
    //
    struct NNSG3dMatAnmResult_ matCache[NNS_G3D_SIZE_MAT_MAX];

    //
    // MayaのSegment Scale Compensate, SI3DのClassic Scale Offの場合に
    // ジョイントに対応したスケール関連データを格納するために存在する。
    // SbcのNODEDESC命令により書き込まれる。
    // MayaのSSCの場合:
    // NODEDESCのフラグ引数で、NNS_G3D_SBC_NODEDESC_FLAG_MAYASSC_PARENT
    // がONのとき、スケールの逆数が入る。
    //
    // SI3DのClassic Scale Offの場合:
    // NNS_G3D_SCALINGRULE_SI3Dが指定されている
    // 場合に親までのスケールの累乗が格納されている。
    // 
    // いずれも、NNSG3dRSのisScaleCacheOneの対応するビットがOFFの場合
    // (スケールが1.0でないとき)のみ値が格納されている
    //
    struct
    {
        VecFx32 s;
        VecFx32 inv;
    }
    scaleCache[NNS_G3D_SIZE_JNT_MAX];

#if (NNS_G3D_USE_EVPCACHE)
    struct
    {
        MtxFx44 M;
        MtxFx33 N;
    }
    evpCache[NNS_G3D_SIZE_JNT_MAX];
#endif
}
NNSG3dRSOnGlb;


/*---------------------------------------------------------------------------*
    NNSG3dFuncSbc

    Sbcコマンド関数のポインタ型
 *---------------------------------------------------------------------------*/
typedef void (*NNSG3dFuncSbc)(NNSG3dRS*, u32);


/*---------------------------------------------------------------------------*
    NNSG3dFuncSbc_[Mat|Shp]Internal

    SbcのMAT/SHPコマンド内部関数のポインタ型
 *---------------------------------------------------------------------------*/
typedef void (*NNSG3dFuncSbc_MatInternal)(NNSG3dRS*, u32, const NNSG3dResMatData*, u32);
typedef void (*NNSG3dFuncSbc_ShpInternal)(NNSG3dRS*, u32, const NNSG3dResShpData*, u32);


/*---------------------------------------------------------------------------*
    g3dcvtrが生成するコードで使用しない行列スタックインデックスの定義
    NNS_G3D_MTXSTACK_SYSはG3D内部で予約されていて、
    NNS_G3D_MTXSTACK_USERはユーザーが使用することができる。
 *---------------------------------------------------------------------------*/

#define NNS_G3D_MTXSTACK_SYS  (30)
#define NNS_G3D_MTXSTACK_USER (29)


////////////////////////////////////////////////////////////////////////////////
//
// 関数の宣言
//

//
// NNS_G3dRSに対するアクセサ
//
NNS_G3D_INLINE void NNS_G3dRSSetCallBack(NNSG3dRS* rs, NNSG3dSbcCallBackFunc func, u8 cmd, NNSG3dSbcCallBackTiming timing);
NNS_G3D_INLINE void NNS_G3dRSResetCallBack(NNSG3dRS* rs, u8 cmd);
NNS_G3D_INLINE NNSG3dRenderObj* NNS_G3dRSGetRenderObj(NNSG3dRS* rs);
NNS_G3D_INLINE NNSG3dMatAnmResult* NNS_G3dRSGetMatAnmResult(NNSG3dRS* rs);
NNS_G3D_INLINE NNSG3dJntAnmResult* NNS_G3dRSGetJntAnmResult(NNSG3dRS* rs);
NNS_G3D_INLINE NNSG3dVisAnmResult* NNS_G3dRSGetVisAnmResult(NNSG3dRS* rs);
NNS_G3D_INLINE u8* NNS_G3dRSGetSbcPtr(NNSG3dRS* rs);
NNS_G3D_INLINE void NNS_G3dRSSetFlag(NNSG3dRS* rs, NNSG3dRSFlag flag);
NNS_G3D_INLINE void NNS_G3dRSResetFlag(NNSG3dRS* rs, NNSG3dRSFlag flag);
NNS_G3D_INLINE int NNS_G3dRSGetCurrentMatID(const NNSG3dRS* rs);
NNS_G3D_INLINE int NNS_G3dRSGetCurrentNodeID(const NNSG3dRS* rs);
NNS_G3D_INLINE int NNS_G3dRSGetCurrentNodeDescID(const NNSG3dRS* rs);
NNS_G3D_INLINE fx32 NNS_G3dRSGetPosScale(const NNSG3dRS* rs);
NNS_G3D_INLINE fx32 NNS_G3dRSGetInvPosScale(const NNSG3dRS* rs);



//
// 描画
//
void NNS_G3dDraw(struct NNSG3dRenderObj_* pRenderObj);

//
// SBC instructions
//
void NNSi_G3dFuncSbc_NOP(NNSG3dRS*, u32);
void NNSi_G3dFuncSbc_RET(NNSG3dRS*, u32);
void NNSi_G3dFuncSbc_NODE(NNSG3dRS*, u32);
void NNSi_G3dFuncSbc_MTX(NNSG3dRS*, u32);
void NNSi_G3dFuncSbc_MAT(NNSG3dRS*, u32);
void NNSi_G3dFuncSbc_SHP(NNSG3dRS*, u32);
void NNSi_G3dFuncSbc_NODEDESC(NNSG3dRS*, u32);
void NNSi_G3dFuncSbc_BB(NNSG3dRS*, u32);
void NNSi_G3dFuncSbc_BBY(NNSG3dRS*, u32);
void NNSi_G3dFuncSbc_NODEMIX(NNSG3dRS*, u32);
void NNSi_G3dFuncSbc_CALLDL(NNSG3dRS*, u32);
void NNSi_G3dFuncSbc_POSSCALE(NNSG3dRS*, u32);
void NNSi_G3dFuncSbc_ENVMAP(NNSG3dRS*, u32);
void NNSi_G3dFuncSbc_PRJMAP(NNSG3dRS*, u32);


// MAT/SHPのitemTagが0のときの動作
void NNSi_G3dFuncSbc_SHP_InternalDefault(NNSG3dRS* rs,
                                        u32 opt,
                                        const NNSG3dResShpData* shp,
                                        u32 idxShp);

void NNSi_G3dFuncSbc_MAT_InternalDefault(NNSG3dRS* rs,
                                        u32 opt,
                                        const NNSG3dResMatData* mat,
                                        u32 idxMat);

//
// コールバックのチェック 
//
NNS_G3D_INLINE BOOL NNSi_G3dCallBackCheck_A(NNSG3dRS* rs, u8 cmd, NNSG3dSbcCallBackTiming* pTiming);
NNS_G3D_INLINE BOOL NNSi_G3dCallBackCheck_B(NNSG3dRS* rs, u8 cmd, NNSG3dSbcCallBackTiming* pTiming);
NNS_G3D_INLINE BOOL NNSi_G3dCallBackCheck_C(NNSG3dRS* rs, u8 cmd, NNSG3dSbcCallBackTiming timing);
NNS_G3D_INLINE NNSG3dSbcCallBackTiming NNSi_CheckPossibilityOfCallBack(NNSG3dRS* rs, u8 cmd);

////////////////////////////////////////////////////////////////////////////////
//
// グローバル変数
//
extern NNSG3dFuncSbc NNS_G3dFuncSbcTable[NNS_G3D_SBC_COMMAND_NUM];
extern NNSG3dFuncSbc_MatInternal NNS_G3dFuncSbcMatTable[NNS_G3D_SIZE_MAT_VTBL_NUM];
extern NNSG3dFuncSbc_ShpInternal NNS_G3dFuncSbcShpTable[NNS_G3D_SIZE_SHP_VTBL_NUM];
extern NNSG3dRS* NNS_G3dRS;
extern NNSG3dRSOnGlb NNS_G3dRSOnGlb;



#ifdef __cplusplus
}/* extern "C" */
#endif

#include <nnsys/g3d/sbc_inline.h>

#endif
