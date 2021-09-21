/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g3d
  File:     sbc_inline.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.14 $
 *---------------------------------------------------------------------------*/

//
// AUTHOR: Kenji Nishida
//

#ifndef NNSG3D_SBC_INLINE_H_
#define NNSG3D_SBC_INLINE_H_

#ifdef __cplusplus
extern "C" {
#endif

//
// コールバックチェック
//
#ifndef NNS_G3D_SBC_CALLBACK_TIMING_A_DISABLE
NNS_G3D_INLINE BOOL
NNSi_G3dCallBackCheck_A(NNSG3dRS* rs, u8 cmd, NNSG3dSbcCallBackTiming* pTiming)
{
    // 最初に呼び出されるコールバックなので*pTimingを設定する
    *pTiming = NNSi_CheckPossibilityOfCallBack(rs, cmd);
    if (*pTiming == NNS_G3D_SBC_CALLBACK_TIMING_A)
    {
        rs->flag &= ~NNS_G3D_RSFLAG_SKIP;
        (*rs->cbVecFunc[cmd])(rs);
        // コールバック内部で設定が変更されている場合があるので再チェック
        *pTiming = NNSi_CheckPossibilityOfCallBack(rs, cmd);
        return (BOOL)(rs->flag & NNS_G3D_RSFLAG_SKIP);
    }
    else
    {
        return FALSE;
    }
}
#else
NNS_G3D_INLINE BOOL
NNSi_G3dCallBackCheck_A(NNSG3dRS* rs, u8 cmd, NNSG3dSbcCallBackTiming* pTiming)
{
    // 最初に呼び出されるコールバックなので*pTimingを設定する
    *pTiming = NNSi_CheckPossibilityOfCallBack(rs, cmd);
    return FALSE;
}
#endif


#ifndef NNS_G3D_SBC_CALLBACK_TIMING_B_DISABLE
NNS_G3D_INLINE BOOL
NNSi_G3dCallBackCheck_B(NNSG3dRS* rs, u8 cmd, NNSG3dSbcCallBackTiming* pTiming)
{
    if (*pTiming == NNS_G3D_SBC_CALLBACK_TIMING_B)
    {
        rs->flag &= ~NNS_G3D_RSFLAG_SKIP;
        (*rs->cbVecFunc[cmd])(rs);
        // コールバック内部で設定が変更されている場合があるので再チェック
        *pTiming = NNSi_CheckPossibilityOfCallBack(rs, cmd);
        return (BOOL)(rs->flag & NNS_G3D_RSFLAG_SKIP);
    }
    else
    {
        return FALSE;
    }
}
#else
NNS_G3D_INLINE BOOL
NNSi_G3dCallBackCheck_B(NNSG3dRS*, u8, NNSG3dSbcCallBackTiming*) { return FALSE; }
#endif


#ifndef NNS_G3D_SBC_CALLBACK_TIMING_C_DISABLE
NNS_G3D_INLINE BOOL
NNSi_G3dCallBackCheck_C(NNSG3dRS* rs, u8 cmd, NNSG3dSbcCallBackTiming timing)
{
    if (timing == NNS_G3D_SBC_CALLBACK_TIMING_C)
    {
        rs->flag &= ~NNS_G3D_RSFLAG_SKIP;
        (*rs->cbVecFunc[cmd])(rs);
        // 最後のコールバックなので再チェックの必要はない
        return (BOOL)(rs->flag & NNS_G3D_RSFLAG_SKIP);
    }
    else
    {
        return FALSE;
    }
}
#else
NNS_G3D_INLINE BOOL
NNSi_G3dCallBackCheck_C(NNSG3dRS*, u8, NNSG3dSbcCallBackTiming) { return FALSE; }
#endif

//
//
//
NNS_G3D_INLINE NNSG3dSbcCallBackTiming
NNSi_CheckPossibilityOfCallBack(NNSG3dRS* rs, u8 cmd)
{
    if (rs->cbVecFunc[cmd])
    {
        return (NNSG3dSbcCallBackTiming)rs->cbVecTiming[cmd];        
    }
    else
    {
        return NNS_G3D_SBC_CALLBACK_TIMING_NONE;
    }
}


/*---------------------------------------------------------------------------*
    NNS_G3dRSSetCallBack

    NNSG3dRS構造体でSBCコマンドcmd用に設定できるコールバック関数を設定する。
    NNS_G3dRenderObjSetInitFuncで設定できるイニシャライズ関数内で使用されるのが普通。
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dRSSetCallBack(NNSG3dRS* rs,
                     NNSG3dSbcCallBackFunc func,
                     u8 cmd,
                     NNSG3dSbcCallBackTiming timing)
{
    NNS_G3D_NULL_ASSERT(rs);
    NNS_G3D_SBC_CALLBACK_TIMING_ASSERT(timing);
    NNS_G3D_ASSERT(cmd < NNS_G3D_SBC_COMMAND_NUM);

    rs->cbVecFunc[cmd] = func;
    rs->cbVecTiming[cmd] = timing;
}


/*---------------------------------------------------------------------------*
    NNS_G3dRSResetCallBack

    NNSG3dRS構造体のcmd用コールバック関数を解除する。
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dRSResetCallBack(NNSG3dRS* rs, u8 cmd)
{
    NNS_G3D_NULL_ASSERT(rs);
    NNS_G3D_ASSERT(cmd < NNS_G3D_SBC_COMMAND_NUM);

    rs->cbVecFunc[cmd] = NULL;
    rs->cbVecTiming[cmd] = (u8)NNS_G3D_SBC_CALLBACK_TIMING_NONE;
}


/*---------------------------------------------------------------------------*
    NNS_G3dRSGetRenderObj

    NNSG3dRS構造体が保持するNNSG3dRenderObj構造体へのポインタを返す
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE NNSG3dRenderObj*
NNS_G3dRSGetRenderObj(NNSG3dRS* rs)
{
    NNS_G3D_NULL_ASSERT(rs);
    return rs->pRenderObj;
}


/*---------------------------------------------------------------------------*
    NNS_G3dRSGetMatAnmResult

    NNSG3dRS構造体が保持するNNSG3dMatAnmResult構造体へのポインタを返す
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE NNSG3dMatAnmResult*
NNS_G3dRSGetMatAnmResult(NNSG3dRS* rs)
{
    NNS_G3D_NULL_ASSERT(rs);
    return rs->pMatAnmResult;
}


/*---------------------------------------------------------------------------*
    NNS_G3dRSGetJntAnmResult

    NNSG3dRS構造体が保持するNNSG3dJntAnmResult構造体へのポインタを返す
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE NNSG3dJntAnmResult*
NNS_G3dRSGetJntAnmResult(NNSG3dRS* rs)
{
    NNS_G3D_NULL_ASSERT(rs);
    return rs->pJntAnmResult;
}


/*---------------------------------------------------------------------------*
    NNS_G3dRSGetJntAnmResult

    NNSG3dRS構造体が保持するNNSG3dJntAnmResult構造体へのポインタを返す
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE NNSG3dVisAnmResult*
NNS_G3dRSGetVisAnmResult(NNSG3dRS* rs)
{
    NNS_G3D_NULL_ASSERT(rs);
    return rs->pVisAnmResult;
}


/*---------------------------------------------------------------------------*
    NNS_G3dRSGetSbcPtr

    NNSG3dRS構造体が保持する現在実行中のSBC命令へのポインタを返します。
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE u8*
NNS_G3dRSGetSbcPtr(NNSG3dRS* rs)
{
    NNS_G3D_NULL_ASSERT(rs);
    return rs->c;
}


/*---------------------------------------------------------------------------*
    NNS_G3dRSSetFlag

    NNSG3dRS構造体が保持するフラグをセットします。
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dRSSetFlag(NNSG3dRS* rs, NNSG3dRSFlag flag)
{
    NNS_G3D_NULL_ASSERT(rs);
    rs->flag |= flag;
}


/*---------------------------------------------------------------------------*
    NNS_G3dRSResetFlag

    NNSG3dRS構造体が保持するフラグをリセットします。
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dRSResetFlag(NNSG3dRS* rs, NNSG3dRSFlag flag)
{
    NNS_G3D_NULL_ASSERT(rs);
    rs->flag &= ~flag;
}


/*---------------------------------------------------------------------------*
    NNS_G3dRSGetCurrentMatID

    現在NNSG3dRS構造体に設定されているマテリアルIDを取得します。
    マテリアルIDが設定されていない場合は-1を返します。

    マテリアルIDはSBCのMATコマンドで設定され、次のMATコマンドがオーバーライドする
    か、コールバック関数によって変更されるまで値を保持しつづけます。
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE int
NNS_G3dRSGetCurrentMatID(const NNSG3dRS* rs)
{
    NNS_G3D_NULL_ASSERT(rs);
    if (rs->flag & NNS_G3D_RSFLAG_CURRENT_MAT_VALID)
    {
        return rs->currentMat;
    }
    else
    {
        return -1;
    }
}


/*---------------------------------------------------------------------------*
    NNS_G3dRSGetCurrentNodeID

    現在NNSG3dRS構造体に設定されているノードIDを取得します。
    ノードIDが設定されていない場合は-1を返します。

    ノードIDはSBCのNODEコマンドで設定され、次のNODEコマンドがオーバーライドするか、
    コールバック関数によって変更されるまで値を保持しつづけます。
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE int
NNS_G3dRSGetCurrentNodeID(const NNSG3dRS* rs)
{
    NNS_G3D_NULL_ASSERT(rs);
    if (rs->flag & NNS_G3D_RSFLAG_CURRENT_NODE_VALID)
    {
        return rs->currentNode;
    }
    else
    {
        return -1;
    }
}


/*---------------------------------------------------------------------------*
    NNS_G3dRSGetCurrentNodeDescID

    現在NNSG3dRS構造体に設定されているNodeDescIDを取得します。
    NodeDescIDが設定されていない場合は-1を返します。

    NodeDescIDはSBCのNODEDESCコマンドで設定され、次のNODEDESCコマンドが
    オーバーライドするか、コールバック関数によって変更されるまで値を保持
    しつづけます。
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE int
NNS_G3dRSGetCurrentNodeDescID(const NNSG3dRS* rs)
{
    NNS_G3D_NULL_ASSERT(rs);
    if (rs->flag & NNS_G3D_RSFLAG_CURRENT_NODEDESC_VALID)
    {
        return rs->currentNodeDesc;
    }
    else
    {
        return -1;
    }
}


/*---------------------------------------------------------------------------*
    NNS_G3dRSGetPosScale

    NNSG3dRS構造体にキャッシュされている、頂点座標に対して乗ざれるスケール値を
    取得します。
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE fx32
NNS_G3dRSGetPosScale(const NNSG3dRS* rs)
{
    NNS_G3D_NULL_ASSERT(rs);
    return rs->posScale;
}


/*---------------------------------------------------------------------------*
    NNS_G3dRSGetInvPosScale

    NNSG3dRS構造体にキャッシュされている、頂点座標に対して乗ざれるスケール値の
    逆数を取得します。
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE fx32
NNS_G3dRSGetInvPosScale(const NNSG3dRS* rs)
{
    NNS_G3D_NULL_ASSERT(rs);
    return rs->invPosScale;
}


#ifdef __cplusplus
}/* extern "C" */
#endif

#endif
