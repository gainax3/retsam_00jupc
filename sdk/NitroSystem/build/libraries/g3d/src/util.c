/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - g3d
  File:     util.c

  Copyright 2004-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.26 $
 *---------------------------------------------------------------------------*/

//
// AUTHOR: Kenji Nishida
//

#include <nnsys/g3d/util.h>
#include <nnsys/g3d/gecom.h>
#include <nnsys/g3d/binres/res_struct_accessor.h>
#include <nnsys/g3d/anm.h>
#include <nnsys/g3d/glbstate.h>


/*---------------------------------------------------------------------------*
    NNS_G3dGetCurrentMtx

    カレント位置座標行列とカレント方向ベクトル行列を取得します。
    引数がNULLの場合はその行列の取得を省略します。
    実行時に射影行列の行列スタックを破壊します。
 *---------------------------------------------------------------------------*/
void
NNS_G3dGetCurrentMtx(MtxFx43* m, MtxFx33* n)
{
    NNS_G3dGeFlushBuffer();

    // Projection行列をIdentityにして
    // クリップ行列とベクタ行列を取り出すと、
    // カレント位置座標行列とカレント方向ベクトル行列を取り出せる。
    G3_MtxMode(GX_MTXMODE_PROJECTION);
    G3_PushMtx();
    G3_Identity();

    if (m)
    {
        MtxFx44 tmp;
        while(G3X_GetClipMtx(&tmp))
            ;
        MTX_Copy44To43(&tmp, m);
    }

    if (n)
    {
        while(G3X_GetVectorMtx(n))
            ;
    }

    // 元のProjection行列を復帰し、Position/Vector行列モードにする
    G3_PopMtx(1);
    G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
}


/*---------------------------------------------------------------------------*
    NNS_G3dGetResultMtx

    NNS_G3dDrawの実行後にnodeIDに対応する行列が行列スタックに残っている場合は、
    その行列を行列スタックから取り出します。
    行列が取り出せた場合にはTRUE, 取り出せなかった場合にはFALSEが返ります。
    TRUEの場合、行列モードはPosition/Vector行列モードに変更されています。
    pos, nrmはNULLでない場合にセットされます。
    なお、得られる行列にはpos_scaleはかかっていないことにご注意ください。
    (.imd中間ファイルの<model_info>で指定される属性で、頂点座標をスケーリング
     している。NNSG3dResMdlInfo::posScaleにfx32型で格納されている。)

    使用法と注意:
    g3cvtrで-sオプションをつけてコンバートした場合、ジョイント行列は全て
    行列スタックにストアされています。
    また、NNSG3dRenderObj::flagのNNS_G3D_RENDEROBJ_FLAG_SKIP_SBC_DRAWを設定して
    NNS_G3dDrawをコールした場合、描画をスキップすることができ、
    NNS_G3D_RENDEROBJ_FLAG_SKIP_SBC_MTXCALCを設定してNNS_G3dDrawをコールした
    場合、行列計算をスキップすることができます。

    この機能を利用すると、
    ・最初に行列を行列スタックにセットしておく
    ・行列スタックにセットされた行列を加工
    ・加工された行列を用いて描画
    といったプログラミングが可能になります。
 *---------------------------------------------------------------------------*/
BOOL
NNS_G3dGetResultMtx(const NNSG3dRenderObj* pRenderObj,
                    MtxFx43* pos,
                    MtxFx33* nrm,
                    u32 nodeID)
{
    const NNSG3dResNodeData* nd;
    u32 stackID;

    // pos, nrmはNULLでもOK
    NNS_G3D_NULL_ASSERT(pRenderObj);

    nd = NNS_G3dGetNodeDataByIdx(&pRenderObj->resMdl->nodeInfo, nodeID);
    stackID = (nd->flag & (u32)NNS_G3D_SRTFLAG_IDXMTXSTACK_MASK)
                                >> NNS_G3D_SRTFLAG_IDXMTXSTACK_SHIFT;

    if (stackID != 31)
    {
        NNS_G3dGeRestoreMtx((int)stackID);
        if (pos || nrm)
        {
            NNS_G3dGetCurrentMtx(pos, nrm);
        }
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


/*---------------------------------------------------------------------------*
    NNS_G3dSetResultMtx

    NNS_G3dDrawの実行後にnodeIDに対応する行列が行列スタックに残るような設定に
    なっている場合は、行列*pos, *nrmをスタックの該当位置に代入します。
    代入できた場合はTRUE,できなかった場合はFALSEが返ります。
    TRUEの場合、行列モードはPosition/Vector行列モードに変更されています。
    nrmはNULLでもかまいませんが、posはNULLであってはいけません。
 *---------------------------------------------------------------------------*/
BOOL
NNS_G3dSetResultMtx(const NNSG3dRenderObj* pRenderObj,
                    const MtxFx43* pos,
                    const MtxFx33* nrm,
                    u32 nodeID)
{
    const NNSG3dResNodeData* nd;
    u32 stackID;

    NNS_G3D_NULL_ASSERT(pRenderObj);
    NNS_G3D_NULL_ASSERT(pos); // nrmはNULLでもよい

    nd = NNS_G3dGetNodeDataByIdx(&pRenderObj->resMdl->nodeInfo, nodeID);
    stackID = (nd->flag & (u32)NNS_G3D_SRTFLAG_IDXMTXSTACK_MASK)
                                >> NNS_G3D_SRTFLAG_IDXMTXSTACK_SHIFT;

    if (stackID != 31)
    {
        // バッファをフラッシュ
        NNS_G3dGeFlushBuffer();

        // 以降はイミディエイトで送信可能.
        G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);

        if (nrm)
        {
            // ロードは4x3行列しかできない
            reg_G3X_GXFIFO = G3OP_MTX_LOAD_4x3;
            MI_CpuSend32(&nrm->_00, &reg_G3X_GXFIFO, sizeof(*nrm));
            reg_G3X_GXFIFO = 0;
            reg_G3X_GXFIFO = 0;
            reg_G3X_GXFIFO = 0;

            G3_MtxMode(GX_MTXMODE_POSITION);
            G3_LoadMtx43(pos);
            G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
        }
        else
        {
            G3_LoadMtx43(pos);
        }

        G3_StoreMtx((int)stackID);

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


/*---------------------------------------------------------------------------*
    NNS_G3dInit

    デフォルトのイニシャライズを行います。
 *---------------------------------------------------------------------------*/
void
NNS_G3dInit(void)
{
    G3X_Init();

    //
    // G3D内部で保持しているグローバル状態(カメラ行列・射影行列等)のイニシャライズ
    //
    NNS_G3dGlbInit();

    //
    // GX_FIFOINTR_COND_DISABLEでなければよいが、
    // GX_FIFOINTR_COND_UNDERHALFよりGX_FIFOINTR_COND_EMPTYのほうがよいようだ
    //
    G3X_SetFifoIntrCond(GX_FIFOINTR_COND_EMPTY);
}


/*---------------------------------------------------------------------------*
    NNS_G3dSbcCmdLen

    SBCコマンドの長さ(バイト数)を表すテーブルです。
    -1の場合は対応するSBCコマンドは存在せず、0の場合はコマンドは可変長です。
 *---------------------------------------------------------------------------*/
const s8 NNS_G3dSbcCmdLen[256] = 
{
    // -1の場合は存在せず
    // 0の場合は不定長
     1,  1,  3,  2,  2,  2,  4,  2,  2,  0,  9,  1,  3,  3, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,

    -1, -1, -1, -1,  2, -1,  5,  3,  3,  0, -1,  1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,

    -1, -1, -1, -1,  2, -1,  5,  3,  3, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,

    -1, -1, -1, -1, -1, -1,  6,  4,  4, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,

    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,

    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,

    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,

    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};


/*---------------------------------------------------------------------------*
    NNS_G3dGetSbcCmdLen

    SBCコマンド*cの長さを返します。
    *cがSBCコマンドでない場合は-1を返します。
 *---------------------------------------------------------------------------*/
int
NNS_G3dGetSbcCmdLen(const u8* c)
{
    int cmdLen;
    NNS_G3D_NULL_ASSERT(c);

    cmdLen = NNS_G3dSbcCmdLen[*c];

    if (cmdLen < 0)
    {
        return -1;
    }
    else if (cmdLen == 0)
    {
        if (*c == NNS_G3D_SBC_NODEMIX)
        {
            return *(c + 2) * 3 + 3;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return cmdLen;
    }
}


/*---------------------------------------------------------------------------*
    NNS_G3dSearchSbcCmd

    SBCコマンド(cmd)をSBCコード列(c)の中から見つけます。
    見つかった場合はSBCコマンド内のポインタを返します。
    見つからなかった場合はNULLを返します。
    SBCコマンド内のフラグ部は無視されます。
 *---------------------------------------------------------------------------*/
const u8*
NNS_G3dSearchSbcCmd(const u8* c, u8 cmd)
{
    int x;
    NNS_G3D_NULL_ASSERT(c);
    NNS_G3D_ASSERTMSG(NNS_G3dSbcCmdLen[cmd] >= 0, "unknown command");

    cmd &= NNS_G3D_SBCCMD_MASK;
    while((x = (*c & NNS_G3D_SBCCMD_MASK)) != NNS_G3D_SBC_RET)
    {
        if (cmd == x)
        {
            return c;
        }
        else
        {
            int cmdLen = NNS_G3dGetSbcCmdLen(c);
            NNS_G3D_ASSERTMSG(cmdLen > 0, "unknown command");
            c += cmdLen;
        }
    }
    return NULL;
}


/*---------------------------------------------------------------------------*
    NNS_G3dGetParentNodeID

    nodeIDの親になるノードのIDを*parentIDに格納します。
    返り値は、nodeIDを設定しているNODEDESC命令へのポインタです。
    見つからなかった場合はNULLを返し、*parentIDも変更されません。
 *---------------------------------------------------------------------------*/
const u8*
NNS_G3dGetParentNodeID(int* parentID, const u8* sbc, u32 nodeID)
{
    const u8* tmp = sbc;

    NNS_G3D_NULL_ASSERT(parentID);
    NNS_G3D_NULL_ASSERT(sbc);
    NNS_G3D_ASSERTMSG(nodeID < NNS_G3D_SIZE_JNT_MAX,
                      "specified nodeID is illegal");

    while((tmp = NNS_G3dSearchSbcCmd(tmp, NNS_G3D_SBC_NODEDESC)) != 0)
    {
        // 自身のノードIDとの一致検査
        if (*(tmp + 1) == nodeID)
        {
            // 親を取得
            // 子に対して親が複数あるような不正なSBCには対応していない
            *parentID = *(tmp + 2);
            return tmp;
        }

        {
            int cmdLen = NNS_G3dGetSbcCmdLen(tmp);
            NNS_G3D_ASSERTMSG(cmdLen > 0, "unknown command");
            tmp += cmdLen;
        }
    }
    return NULL;
}


/*---------------------------------------------------------------------------*
    NNS_G3dGetChildNodeIDList

    nodeIDの子になるノードIDのリストをidListに作成します。
    idListは十分な領域が確保されている必要があります。
    返り値は子ノードの数です。
    みつからなかった場合は0を返し、idListには何も書き込まれません。
 *---------------------------------------------------------------------------*/
int
NNS_G3dGetChildNodeIDList(u8* idList, const u8* sbc, u32 nodeID)
{
    const u8* tmp = sbc;
    int num = 0;

    NNS_G3D_NULL_ASSERT(idList);
    NNS_G3D_ASSERTMSG(nodeID < NNS_G3D_SIZE_JNT_MAX,
                      "specified nodeID is illegal");

    while((tmp = NNS_G3dSearchSbcCmd(tmp, NNS_G3D_SBC_NODEDESC)) != 0)
    {
        // 親ノードIDの一致検査
        if (*(tmp + 2) == nodeID && *(tmp + 2) != *(tmp + 1))
        {
            // 追加
            *(idList + num++) = *(tmp + 1);
        }

        {
            int cmdLen = NNS_G3dGetSbcCmdLen(tmp);
            NNS_G3D_ASSERTMSG(cmdLen > 0, "unknown command");
            tmp += cmdLen;
        }
    }

    return num;
}


/*---------------------------------------------------------------------------*
    NNS_G3dResDefaultSetup

    G3Dのリソース*pDataをセットアップします。データフォーマットにより
    動作がことなります。

    .nsbmdファイルの場合
        ・テクスチャとパレット用の領域を確保
        ・テクスチャとパレットをVRAMにロード
        ・ファイル内の各モデルにテクスチャをバインド

    .nsbtxファイルの場合
        ・テクスチャとパレット用の領域を確保
        ・テクスチャとパレットをVRAMにロード

    その他
        なにもしません。

    テクスチャとパレットをVRAMにロードする際にDMA転送を使用するため、
    この関数を呼ぶ前にテクスチャとパレットのデータキャッシュを
    メインメモリにライトバックしておく必要があります。
 *---------------------------------------------------------------------------*/
BOOL
NNS_G3dResDefaultSetup(void* pResData)
{
    u8* binFile = (u8*)pResData;
    BOOL failed = FALSE;

    NNS_G3D_NULL_ASSERT(pResData);

    switch(*(u32*)&binFile[0])
    {
    case NNS_G3D_SIGNATURE_NSBTX: // BTX0
    case NNS_G3D_SIGNATURE_NSBMD: // BMD0
        {
            NNSG3dResTex* tex;
            u32 szTex, szTex4x4, szPltt;
            BOOL sucTex    = TRUE;
            BOOL sucTex4x4 = TRUE;
            BOOL sucPltt   = TRUE;
            NNSG3dTexKey keyTex;
            NNSG3dTexKey keyTex4x4;
            NNSG3dPlttKey keyPltt;

            tex = NNS_G3dGetTex((NNSG3dResFileHeader*) pResData);
            if (tex)
            {
                // 必要なサイズを取得
                szTex    = NNS_G3dTexGetRequiredSize(tex);
                szTex4x4 = NNS_G3dTex4x4GetRequiredSize(tex);
                szPltt   = NNS_G3dPlttGetRequiredSize(tex);

                if (szTex > 0)
                {
                    // 存在すればテクスチャイメージスロットに確保
                    keyTex = NNS_GfdAllocTexVram(szTex, FALSE, 0);
                    if (keyTex == NNS_GFD_ALLOC_ERROR_TEXKEY)
                    {
                        sucTex = FALSE;
                    }
                }
                else
                {
                    keyTex = 0;
                }

                if (szTex4x4 > 0)
                {
                    // 存在すればテクスチャイメージスロットに確保
                    keyTex4x4 = NNS_GfdAllocTexVram(szTex4x4, TRUE, 0);
                    if (keyTex4x4 == NNS_GFD_ALLOC_ERROR_TEXKEY)
                    {
                        sucTex4x4 = FALSE;
                    }
                }
                else
                {
                    keyTex4x4 = 0;
                }

                if (szPltt > 0)
                {
                    // 存在すればテクスチャパレットスロットに確保
                    keyPltt = 
                        NNS_GfdAllocPlttVram(szPltt,
                                            tex->tex4x4Info.flag & NNS_G3D_RESPLTT_USEPLTT4,
                                            0);
                    if (keyPltt == NNS_GFD_ALLOC_ERROR_PLTTKEY)
                    {
                        sucPltt = FALSE;
                    }
                }
                else
                {
                    keyPltt = 0;
                }

                if (!sucTex || !sucTex4x4 || !sucPltt)
                {
                    // 失敗時のロールバック処理
                    int status;

                    if (sucPltt)
                    {
                        status = NNS_GfdFreePlttVram(keyPltt);
                        NNS_G3D_ASSERTMSG(!status, "NNS_GfdFreePlttVram failed");
                    }

                    if (sucTex4x4)
                    {
                        status = NNS_GfdFreeTexVram(keyTex4x4);
                        NNS_G3D_ASSERTMSG(!status, "NNS_GfdFreeTexVram failed");
                    }

                    if (sucTex)
                    {
                        status = NNS_GfdFreeTexVram(keyTex);
                        NNS_G3D_ASSERTMSG(!status, "NNS_GfdFreeTexVram failed");
                    }

                    return FALSE;
                }

                // キーのアサイン
                NNS_G3dTexSetTexKey(tex, keyTex, keyTex4x4);
                NNS_G3dPlttSetPlttKey(tex, keyPltt);

                // VRAMへのロード
                NNS_G3dTexLoad(tex, TRUE);
                NNS_G3dPlttLoad(tex, TRUE);
            }

            if (*(u32*)&binFile[0] == NNS_G3D_SIGNATURE_NSBMD)
            {
                // NSBMDの場合は全てのモデルをバインドする
                NNSG3dResMdlSet* mdlSet = NNS_G3dGetMdlSet((NNSG3dResFileHeader*) pResData);
                NNS_G3D_NULL_ASSERT(mdlSet);

                if (tex)
                {
                    // モデルセットのバインド
                    (void)NNS_G3dBindMdlSet(mdlSet, tex);
                }
            }
        }
        return TRUE;
        break;
    case NNS_G3D_SIGNATURE_NSBCA: // BCA0
    case NNS_G3D_SIGNATURE_NSBVA: // BVA0
    case NNS_G3D_SIGNATURE_NSBMA: // BMA0
    case NNS_G3D_SIGNATURE_NSBTP: // BTP0
    case NNS_G3D_SIGNATURE_NSBTA: // BTA0
        return TRUE;
        break;
    default:
        NNS_G3D_ASSERTMSG(1==0, "unknown file signature: '%c%c%c%c' found.\n",
                                binFile[0], binFile[1], binFile[2], binFile[3]);
        return FALSE;
        break;
    };
}


/*---------------------------------------------------------------------------*
    NNS_G3dResDefaultRelease

    G3Dのリソース*pDataの領域を開放する前に通常行う操作を実行します。
    データフォーマットにより動作がことなります。

    .nsbmdファイルの場合
        ・テクスチャとパレット用の領域を開放

    .nsbtxファイルの場合
        ・テクスチャとパレット用の領域を開放

    その他
        なにもしません。

    NOTICE:
    pResDataのメモリ領域そのものの解放はユーザーが行う必要があります。
 *---------------------------------------------------------------------------*/
void
NNS_G3dResDefaultRelease(void* pResData)
{
    u8* binFile = (u8*)pResData;
    BOOL failed = FALSE;

    NNS_G3D_NULL_ASSERT(pResData);

    switch(*(u32*)&binFile[0])
    {
    case NNS_G3D_SIGNATURE_NSBMD: // BMD0
        {
            NNSG3dResTex* tex;
            NNSG3dResMdlSet* mdlSet = NNS_G3dGetMdlSet(pResData);
            NNS_G3D_NULL_ASSERT(mdlSet);
            tex = NNS_G3dGetTex((NNSG3dResFileHeader*) pResData);

            if (tex)
            {
                // 全てのモデルをリリース
                NNS_G3dReleaseMdlSet(mdlSet);
            }
        }
        // don't break
    case NNS_G3D_SIGNATURE_NSBTX: // BTX0
        {
            NNSG3dResTex* tex;
            NNSG3dPlttKey plttKey;
            NNSG3dTexKey  texKey, tex4x4Key;
            int status;
            tex = NNS_G3dGetTex((NNSG3dResFileHeader*) pResData);

            if (tex)
            {
                // キーをテクスチャブロックから切り離す
                plttKey   = NNS_G3dPlttReleasePlttKey(tex);
                NNS_G3dTexReleaseTexKey(tex, &texKey, &tex4x4Key);

                if (plttKey > 0)
                {
                    // テクスチャパレットスロット内に確保されていれば解放
                    status = NNS_GfdFreePlttVram(plttKey);
                    NNS_G3D_ASSERTMSG(!status, "NNS_GfdFreePlttVram failed");
                }

                if (tex4x4Key > 0)
                {
                    // テクスチャイメージスロット内に確保されていれば解放
                    status = NNS_GfdFreeTexVram(tex4x4Key);
                    NNS_G3D_ASSERTMSG(!status, "NNS_GfdFreeTexVram failed");
                }

                if (texKey > 0)
                {
                    // テクスチャイメージスロット内に確保されていれば解放
                    status = NNS_GfdFreeTexVram(texKey);
                    NNS_G3D_ASSERTMSG(!status, "NNS_GfdFreeTexVram failed");
                }
            }
        }
        break;
    case NNS_G3D_SIGNATURE_NSBCA: // BCA0
    case NNS_G3D_SIGNATURE_NSBVA: // BVA0
    case NNS_G3D_SIGNATURE_NSBMA: // BMA0
    case NNS_G3D_SIGNATURE_NSBTP: // BTP0
    case NNS_G3D_SIGNATURE_NSBTA: // BTA0
        break;
    default:
        NNS_G3D_ASSERTMSG(1==0, "unknown file signature: '%c%c%c%c' found.\n",
                                binFile[0], binFile[1], binFile[2], binFile[3]);
        break;
    };
}


/*---------------------------------------------------------------------------*
    NNS_G3dLocalOriginToScrPos

    ローカル座標系の原点のスクリーン上の位置を求める。
    カレント位置座標行列とカレント射影行列が適切に設定されている必要があります。

    返り値が0ならば*px, *pyにスクリーン上の座標が格納されています。
    返り値が-1ならば画面外ですが、*px, *pyに方向の目安となる座標が格納されています。
 *---------------------------------------------------------------------------*/
int
NNS_G3dLocalOriginToScrPos(int* px, int* py)
{
    // カレント位置座標行列とカレント射影変換行列で、
    // ローカル座標系からクリップ座標系への変換ができるように
    // 設定してから呼び出す。
    
    VecFx32 vec;
    fx32 w;
    fx64c invW;
    int x1, y1, x2, y2;
    int dx, dy;
    int rval;

    NNS_G3D_NULL_ASSERT(px);
    NNS_G3D_NULL_ASSERT(py);

    // ローカル座標(0, 0, 0)に対するクリップ座標を求める。
    NNS_G3dGePositionTest(0, 0, 0);
    NNS_G3dGeFlushBuffer();
    while(G3X_GetPositionTestResult(&vec, &w))
        ;

    invW = FX_InvFx64c(w);

    // 正規化スクリーン座標系に変換
    vec.x = (FX_Mul32x64c(vec.x, invW) + FX32_ONE) / 2;
    vec.y = (FX_Mul32x64c(vec.y, invW) + FX32_ONE) / 2;

    // ローカル座標の原点が正規化スクリーン座標系に変換されている
    if (vec.x < 0 || vec.y < 0 || vec.x > FX32_ONE || vec.y > FX32_ONE)
    {
        // 画面外の場合
        rval = -1;
    }
    else
    {
        rval = 0;
    }

    NNS_G3dGlbGetViewPort(&x1, &y1, &x2, &y2);
    dx = x2 - x1;
    dy = y2 - y1;

    // 更にBGスクリーン座標系に変換
    // BGの水平移動等をさせている場合は、更に結果を補正する必要がある。
    *px = x1 + ((vec.x * dx + FX32_HALF) >> FX32_SHIFT);
    *py = 191 - y1 - ((vec.y * dy + FX32_HALF) >> FX32_SHIFT);

    
    return rval;
}


/*---------------------------------------------------------------------------*
    NNS_G3dWorldPosToScrPos

    ワールド座標からスクリーン上の位置を求める。
    NNS_G3dGlb構造体にカメラ行列と射影行列が設定されている必要があります。

    返り値が0ならば*px, *pyにスクリーン上の座標が格納されています。
    返り値が-1ならば画面外ですが、*px, *pyに方向の目安となる座標が格納されています。
 *---------------------------------------------------------------------------*/
int
NNS_G3dWorldPosToScrPos(const VecFx32* pWorld, int* px, int* py)
{
    // 入力のワールド座標にビューイング変換と
    // 射影変換をかける

    const MtxFx44* proj;
    const MtxFx43* camera;
    VecFx32 tmp;
    VecFx32 vec;
    fx32 w;
    fx64c invW;
    int x1, y1, x2, y2;
    int dx, dy;
    int rval;

    NNS_G3D_NULL_ASSERT(pWorld);
    NNS_G3D_NULL_ASSERT(px);
    NNS_G3D_NULL_ASSERT(py);

    proj = NNS_G3dGlbGetProjectionMtx();
    camera = NNS_G3dGlbGetCameraMtx();

    MTX_MultVec43(pWorld, camera, &tmp);

    w = (fx32)(((fx64)tmp.x * proj->_03 +
                (fx64)tmp.y * proj->_13 +
                (fx64)tmp.z * proj->_23) >> FX32_SHIFT);
    w += proj->_33;

    FX_InvAsync(w);

    vec.x = (fx32)(((fx64)tmp.x * proj->_00 +
                    (fx64)tmp.y * proj->_10 +
                    (fx64)tmp.z * proj->_20) >> FX32_SHIFT);
    vec.x += proj->_30;

    vec.y = (fx32)(((fx64)tmp.x * proj->_01 +
                    (fx64)tmp.y * proj->_11 +
                    (fx64)tmp.z * proj->_21) >> FX32_SHIFT);
    vec.y += proj->_31;

    invW = FX_GetInvResultFx64c();

    // 正規化スクリーン座標系に変換
    vec.x = (FX_Mul32x64c(vec.x, invW) + FX32_ONE) / 2;
    vec.y = (FX_Mul32x64c(vec.y, invW) + FX32_ONE) / 2;

    // ローカル座標の原点が正規化スクリーン座標系に変換されている
    if (vec.x < 0 || vec.y < 0 || vec.x > FX32_ONE || vec.y > FX32_ONE)
    {
        // 画面外の場合
        rval = -1;
    }
    else
    {
        rval = 0;
    }

    NNS_G3dGlbGetViewPort(&x1, &y1, &x2, &y2);
    dx = x2 - x1;
    dy = y2 - y1;

    // 更にBGスクリーン座標系に変換
    // BGの水平移動等をさせている場合は、更に結果を補正する必要がある。
    *px = x1 + ((vec.x * dx + FX32_HALF) >> FX32_SHIFT);
    *py = 191 - y1 - ((vec.y * dy + FX32_HALF) >> FX32_SHIFT);
    
    return rval;
}


/*---------------------------------------------------------------------------*
    NNS_G3dScrPosToWorldLine

    スクリーン上の位置に対応するNearクリップ面とFarクリップ面上の点を
    ワールド座標系で返します。
    pFarがNULLの場合は、Farクリップ面上の点の計算は省略されます。

    返り値が0ならば、スクリーン上の点はビューポート内です。
    返り値が-1ならば、スクリーン上の点はビューポート外です。
 *---------------------------------------------------------------------------*/
int
NNS_G3dScrPosToWorldLine(int px, int py, VecFx32* pNear, VecFx32* pFar)
{
    int rval;
    int x1, y1, x2, y2;
    int dx, dy;
    fx32 x, y;
    const MtxFx44* m;
    VecFx32 vNear, vFar;
    fx64c invWNear, invWFar;
    fx32 wNear, wFar;

    NNS_G3D_NULL_ASSERT(pNear);

    // ＢＧスクリーン座標系から正規化スクリーン座標系に変換
    NNS_G3dGlbGetViewPort(&x1, &y1, &x2, &y2);
    dx = x2 - x1;
    dy = y2 - y1;

    x = FX_Div((px - x1) << FX32_SHIFT, dx << FX32_SHIFT);
    y = FX_Div((py + y1 - 191) << FX32_SHIFT, -dy << FX32_SHIFT);

    if (x < 0 || y < 0 || x > FX32_ONE || y > FX32_ONE)
    {
        rval = -1;
    }
    else
    {
        rval = 0;
    }

    // +-1の立方体になる。
    x = (x - FX32_HALF) * 2;
    y = (y - FX32_HALF) * 2;

    // 射影行列とカメラ行列をかけた行列の逆行列を取得する。
    m = NNS_G3dGlbGetInvVP();

    // NEAR面上の点は(x, y, -FX32_ONE, FX32_ONE)
    // FAR面上の点は (x, y,  FX32_ONE, FX32_ONE)
    // 逆行列をかけてワールド座標系での点を求める。
    wNear   = m->_33 + (fx32)(((fx64)x * m->_03 + (fx64)y * m->_13) >> FX32_SHIFT);
    FX_InvAsync(wNear - m->_23);

    vNear.x = m->_30 + (fx32)(((fx64)x * m->_00 + (fx64)y * m->_10) >> FX32_SHIFT);
    vNear.y = m->_31 + (fx32)(((fx64)x * m->_01 + (fx64)y * m->_11) >> FX32_SHIFT);
    vNear.z = m->_32 + (fx32)(((fx64)x * m->_02 + (fx64)y * m->_12) >> FX32_SHIFT);

    if (pFar)
    {
        vFar.x = vNear.x + m->_20;
        vFar.y = vNear.y + m->_21;
        vFar.z = vNear.z + m->_22;
        wFar   = wNear + m->_23;
    }

    vNear.x -= m->_20;
    vNear.y -= m->_21;
    vNear.z -= m->_22;

    invWNear = FX_GetInvResultFx64c();
    if (pFar)
        FX_InvAsync(wFar);
    
    pNear->x = FX_Mul32x64c(vNear.x, invWNear);
    pNear->y = FX_Mul32x64c(vNear.y, invWNear);
    pNear->z = FX_Mul32x64c(vNear.z, invWNear);

    if (pFar)
    {
        invWFar = FX_GetInvResultFx64c();

        pFar->x = FX_Mul32x64c(vFar.x, invWFar);
        pFar->y = FX_Mul32x64c(vFar.y, invWFar);
        pFar->z = FX_Mul32x64c(vFar.z, invWFar);
    }

    return rval;
}
