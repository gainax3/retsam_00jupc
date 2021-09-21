/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - g3d
  File:     kernel.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.35 $
 *---------------------------------------------------------------------------*/

//
// AUTHOR: Kenji Nishida
//

#include <nnsys/g3d/kernel.h>
#include <nnsys/g3d/anm.h>
#include <nnsys/g3d/binres/res_struct.h>
#include <nnsys/g3d/binres/res_struct_accessor.h>


/*---------------------------------------------------------------------------*
    NNS_G3dAnmObjCalcSizeRequired

    アニメーションリソース、モデルリソースに対応するNNSG3dAnmObjに必要な
    メモリサイズを返します。
 *---------------------------------------------------------------------------*/
u32
NNS_G3dAnmObjCalcSizeRequired(const void* pAnm,
                              const NNSG3dResMdl* pMdl)
{
    const NNSG3dResAnmHeader* hdr;
    NNS_G3D_NULL_ASSERT(pAnm);
    NNS_G3D_NULL_ASSERT(pMdl);
    if (!pAnm || !pMdl)
        return 0;

    hdr = (const NNSG3dResAnmHeader*)pAnm;
    switch(hdr->category0)
    {
    case 'M': // material animation
        return NNS_G3D_ANMOBJ_SIZE_MATANM(pMdl);
        break;
    case 'J': // character(joint) animation
    case 'V': // visibility animation
        return NNS_G3D_ANMOBJ_SIZE_JNTANM(pMdl);
        break;
    default:
        NNS_G3D_ASSERT(0);
        return 0;
        break;
    }
}


/*---------------------------------------------------------------------------*
    NNS_G3dInitAnmObj

    pAnmObjをpResAnm, pResMdl用に初期化します。

    NOTICE:
    pAnmObjは最低、NNS_G3dAnmObjCalcSizeRequired(pResAnm, pResMdl)のサイズを
    持っている必要がある。
 *---------------------------------------------------------------------------*/
void
NNS_G3dAnmObjInit(NNSG3dAnmObj* pAnmObj,
                  void* pResAnm,
                  const NNSG3dResMdl* pResMdl,
                  const NNSG3dResTex* pResTex)
{
    // プログラマが定義するアニメーションの場合リソースがない場合もあるが、
    // その場合はヘッダ部分だけを偽装して入れてしまえばよい。
    const NNSG3dResAnmHeader* hdr;
    u32 i;
    NNS_G3D_NULL_ASSERT(pAnmObj);
    NNS_G3D_NULL_ASSERT(pResAnm);
    NNS_G3D_NULL_ASSERT(pResMdl);
    // resTexはテクスチャパターンアニメーション以外ではNULLでもよい。
    NNS_G3D_ASSERT(NNS_G3dAnmFmtNum <= NNS_G3D_ANMFMT_MAX);

    pAnmObj->frame = 0;
    pAnmObj->resAnm = (void*)pResAnm;

    // アニメーション関数の設定がいる。
    // これはcategory1によって決定される
    pAnmObj->next = NULL;
    pAnmObj->priority = 127;
    pAnmObj->ratio = FX32_ONE;
    pAnmObj->resTex = pResTex;
    pAnmObj->numMapData = 0; // アニメーション無効マクロ(NNS_G3D_NS***_DISABLE)対応用
    pAnmObj->funcAnm = NULL; // アニメーション無効マクロ(NNS_G3D_NS***_DISABLE)対応用

    hdr = (const NNSG3dResAnmHeader*)pResAnm;

    for (i = 0; i < NNS_G3dAnmFmtNum; ++i)
    {
        if (NNS_G3dAnmObjInitFuncArray[i].category0 == hdr->category0 &&
            NNS_G3dAnmObjInitFuncArray[i].category1 == hdr->category1)
        {
            if (NNS_G3dAnmObjInitFuncArray[i].func)
            {
                (*NNS_G3dAnmObjInitFuncArray[i].func)(pAnmObj, pResAnm, pResMdl);
            }
            break;
        }
    }
    NNS_G3D_ASSERT(i != NNS_G3dAnmFmtNum);
}


/*---------------------------------------------------------------------------
    NNS_G3dAnmObjEnableID

    JntID,又はMatIDに対応したアニメーションを再生するようにします。
    (デフォルト)
 *---------------------------------------------------------------------------*/
void
NNS_G3dAnmObjEnableID(NNSG3dAnmObj* pAnmObj, int id)
{
    NNS_G3D_NULL_ASSERT(pAnmObj);

    if (id >= 0 && id < pAnmObj->numMapData &&
        (pAnmObj->mapData[id] & NNS_G3D_ANMOBJ_MAPDATA_EXIST))
        pAnmObj->mapData[id] &= ~NNS_G3D_ANMOBJ_MAPDATA_DISABLED;
}


/*---------------------------------------------------------------------------
    NNS_G3dAnmObjEnableID

    JntID,又はMatIDに対応したアニメーションを再生しないようにします。
 *---------------------------------------------------------------------------*/
void
NNS_G3dAnmObjDisableID(NNSG3dAnmObj* pAnmObj, int id)
{
    NNS_G3D_NULL_ASSERT(pAnmObj);

    if (id >= 0 && id < pAnmObj->numMapData &&
        (pAnmObj->mapData[id] & NNS_G3D_ANMOBJ_MAPDATA_EXIST))
        pAnmObj->mapData[id] |= NNS_G3D_ANMOBJ_MAPDATA_DISABLED;
}



/*---------------------------------------------------------------------------*
    NNS_G3dRenderObjInit

    NNSG3dRenderObjを初期化します。pResMdlはNULLでもOKです。
 *---------------------------------------------------------------------------*/
void
NNS_G3dRenderObjInit(NNSG3dRenderObj* pRenderObj,
                     NNSG3dResMdl* pResMdl)
{
    NNS_G3D_NULL_ASSERT(pRenderObj);
    // pResMdl can be NULL

    MI_CpuClear32(pRenderObj, sizeof(NNSG3dRenderObj));

    // ブレンドのデフォルトハンドラを設定
    pRenderObj->funcBlendMat = NNS_G3dFuncBlendMatDefault;
    pRenderObj->funcBlendJnt = NNS_G3dFuncBlendJntDefault;
    pRenderObj->funcBlendVis = NNS_G3dFuncBlendVisDefault;

    pRenderObj->resMdl = pResMdl;
}


/*---------------------------------------------------------------------------*
    addLink_

    itemが既にリストになっている場合にも対応しているが、そのリストがソート
    されていない場合は正しくない順番になる場合がある
 *---------------------------------------------------------------------------*/
static void
addLink_(NNSG3dAnmObj** l, NNSG3dAnmObj* item)
{
#ifdef NITRO_DEBUG
    // itemがl内にすでにある場合はエラー
    {
        NNSG3dAnmObj* p = *l;
        while(p)
        {
            NNS_G3D_ASSERT(p != item);
            p = p->next;
        }
    }
#endif
    if (!(*l))
    {
        // リストが存在しなければ最初の要素になる。
        *l = item;
    }
    else if (!((*l)->next))
    {
        // リストの要素が１つの場合
        if ((*l)->priority > item->priority)
        {
            // itemの末尾にリストを繋げる
            NNSG3dAnmObj* p = item;
            while(p->next)
            {
                p = p->next;
            }
            p->next = (*l);
            *l = item;
        }
        else
        {
            // 既存リスト(ただし要素は１つ)の末尾に繋げる
            (*l)->next = item;
        }
    }
    else
    {
        NNSG3dAnmObj* p = *l;
        NNSG3dAnmObj* x = (*l)->next;

        while(x)
        {
            if (x->priority >= item->priority)
            {
                // 既存リストに挿入
                NNSG3dAnmObj* pp = item;
                while(pp->next)
                {
                    pp = pp->next;
                }
                p->next = item;
                pp->next = x;
                return;
            }
            p = x;
            x = x->next;
        }
        // 既存リストの末尾に繋げる
        p->next = item;
    }
}


/*---------------------------------------------------------------------------*
    updateHintVec_

    pAnmObjで登録されているリソースIDに対応するビットをONにしていく
 *---------------------------------------------------------------------------*/
static void
updateHintVec_(u32* pVec, const NNSG3dAnmObj* pAnmObj)
{
    const NNSG3dAnmObj* p = pAnmObj;
    while(p)
    {
        int i;
        for (i = 0; i < p->numMapData; ++i)
        {
            if (p->mapData[i] & NNS_G3D_ANMOBJ_MAPDATA_EXIST)
            {
                pVec[i >> 5] |= 1 << (i & 31);
            }
        }
        p = p->next;
    }
}


/*---------------------------------------------------------------------------*
    NNS_G3dRenderObjAddAnm

    pRenderObjにpAnmObjを追加する。pAnmObjのヘッダ情報を見て、
    適切なリストに追加する。
 *---------------------------------------------------------------------------*/
void
NNS_G3dRenderObjAddAnmObj(NNSG3dRenderObj* pRenderObj,
                          NNSG3dAnmObj* pAnmObj)
{
    const NNSG3dResAnmHeader* hdr;
    NNS_G3D_NULL_ASSERT(pRenderObj);
    NNS_G3D_NULL_ASSERT(pAnmObj);
    NNS_G3D_NULL_ASSERT(pAnmObj->resAnm);

    if (pAnmObj && pAnmObj->resAnm)
    {
        hdr = (const NNSG3dResAnmHeader*)pAnmObj->resAnm;

        switch(hdr->category0)
        {
        case 'M': // material animation
            updateHintVec_(&pRenderObj->hintMatAnmExist[0], pAnmObj);
            addLink_(&pRenderObj->anmMat, pAnmObj);
            break;
        case 'J': // character(joint) animation
            updateHintVec_(&pRenderObj->hintJntAnmExist[0], pAnmObj);
            addLink_(&pRenderObj->anmJnt, pAnmObj);
            break;
        case 'V': // visibility animation
            updateHintVec_(&pRenderObj->hintVisAnmExist[0], pAnmObj);
            addLink_(&pRenderObj->anmVis, pAnmObj);
            break;
        default:
            NNS_G3D_ASSERT(0);
            break;
        }
    }
}


/*---------------------------------------------------------------------------*
    removeLink_

    itemをリストから取り除きます。リスト内にitemがあった場合にはTRUE
    なかった場合にはFALSEを返します。
 *---------------------------------------------------------------------------*/
static BOOL
removeLink_(NNSG3dAnmObj** l, NNSG3dAnmObj* item)
{
    NNSG3dAnmObj* p;
    NNSG3dAnmObj* pp;

    if (!*l)
    {
        // AnmObjが一つもない場合
        return FALSE;
    }
    
    if (*l == item)
    {
        // 最初のAnmObjが取り除かれる場合
        *l = (*l)->next;
        item->next = NULL;
        return TRUE;
    }

    // リストを検索
    p = (*l)->next;
    pp = (*l);
    while(p)
    {
        if (p == item)
        {
            pp->next = p->next;
            p->next = NULL;

            return TRUE;
        }
        pp = p;
        p = p->next;
    }
    return FALSE;
}


/*---------------------------------------------------------------------------*
    NNS_G3dRenderObjRemoveAnmObj

    pRenderObjからpAnmObjを削除します。
    ただし、ヒント用のビットベクトルは変更されません。
    更新は、NNS_G3dDraw内でNNS_G3D_RENDEROBJ_FLAG_HINT_OBSOLETE
    がチェックされている場合に行われます。
 *---------------------------------------------------------------------------*/
void
NNS_G3dRenderObjRemoveAnmObj(NNSG3dRenderObj* pRenderObj,
                             NNSG3dAnmObj* pAnmObj)
{
    NNS_G3D_NULL_ASSERT(pRenderObj);
    NNS_G3D_NULL_ASSERT(pAnmObj);

    if (removeLink_(&pRenderObj->anmMat, pAnmObj) ||
        removeLink_(&pRenderObj->anmJnt, pAnmObj) ||
        removeLink_(&pRenderObj->anmVis, pAnmObj))
    {
        NNS_G3dRenderObjSetFlag(pRenderObj, NNS_G3D_RENDEROBJ_FLAG_HINT_OBSOLETE);
        return;
    }
    
    // pAnmObjはpRenderObjに繋がっていなかった
    NNS_G3D_WARNING(0, "An AnmObj was not removed in NNS_G3dRenderObjRemoveAnmObj");
}


/*---------------------------------------------------------------------------*
    NNS_G3dRenderObjSetCallBack

    描画処理中に行うコールバック処理を登録します。
    cmdのコマンドが実行されるときで、
    timingで指定できる(3種類選べる)場所で、コールバックが呼ばれます。
 *---------------------------------------------------------------------------*/
void
NNS_G3dRenderObjSetCallBack(NNSG3dRenderObj* pRenderObj,
                            NNSG3dSbcCallBackFunc func,
                            u8*,          // NOTICE:コールバックのアドレス指定は廃止になりました
                            u8 cmd,
                            NNSG3dSbcCallBackTiming timing)
{
    NNS_G3D_NULL_ASSERT(pRenderObj);

    pRenderObj->cbFunc = func;
    pRenderObj->cbCmd = cmd;
    pRenderObj->cbTiming = (u8)timing;
}



/*---------------------------------------------------------------------------*
    NNS_G3dRenderObjResetCallBack

    描画処理中に行うコールバック処理を消去します。
 *---------------------------------------------------------------------------*/
void
NNS_G3dRenderObjResetCallBack(NNSG3dRenderObj* pRenderObj)
{
    NNS_G3D_NULL_ASSERT(pRenderObj);

    pRenderObj->cbFunc = NULL;
    pRenderObj->cbCmd = 0;
    pRenderObj->cbTiming = 0;
}


/*---------------------------------------------------------------------------*
    NNS_G3dRenderObjSetInitFunc

    描画処理直前に呼ばれるコールバックを設定します。
 *---------------------------------------------------------------------------*/
void
NNS_G3dRenderObjSetInitFunc(NNSG3dRenderObj* pRenderObj,
                            NNSG3dSbcCallBackFunc func)
{
    NNS_G3D_NULL_ASSERT(pRenderObj);
    pRenderObj->cbInitFunc = func;
}


////////////////////////////////////////////////////////////////////////////////
//
// モデル<->テクスチャ間コード
//
//

//
// Texture / 4x4Texture / Plttのライフサイクル
//
// 1 GetRequiredSizeでサイズを求める
// 2 texKeyまたはplttKeyを取得(VRAMマネージャのコードは呼び出さない)
// 3 SetTexKeyまたはSetPlttKeyでキーをセット
// 4 テクスチャ／パレットデータをVRAMにロード
// 5 描画したいモデルリソースにテクスチャ／パレットデータをバインド
// 6 モデルリソースを使って描画
//(7 モデルリソースが別のテクスチャセットを利用する場合はアンバインド)
// 8 どのモデルからも使用しないのならリリース(VRAMマネージャのコードは呼び出さない)
//
// 4,5は順不同
//

/*---------------------------------------------------------------------------*
    NNS_G3dTexGetRequiredSize

    テクスチャブロックが持つ4x4圧縮テクスチャ以外のテクスチャのサイズを返します
 *---------------------------------------------------------------------------*/
u32
NNS_G3dTexGetRequiredSize(const NNSG3dResTex* pTex)
{
    NNS_G3D_NULL_ASSERT(pTex);

    if (pTex)
    {
        return (u32)(pTex->texInfo.sizeTex << 3);
    }
    else
    {
        return 0;
    }
}


/*---------------------------------------------------------------------------*
    NNS_G3dTex4x4GetRequiredSize

    テクスチャブロックが持つ4x4圧縮テクスチャのサイズを返します
 *---------------------------------------------------------------------------*/
u32
NNS_G3dTex4x4GetRequiredSize(const NNSG3dResTex* pTex)
{
    NNS_G3D_NULL_ASSERT(pTex);

    if (pTex)
    {
        return (u32)(pTex->tex4x4Info.sizeTex << 3);
    }
    else
    {
        return 0;
    }
}


/*---------------------------------------------------------------------------*
    NNS_G3dTexSetTexKey

    テクスチャブロック内のテクスチャへテクスチャキーの割り当てを行います
 *---------------------------------------------------------------------------*/
void
NNS_G3dTexSetTexKey(NNSG3dResTex* pTex,
                    NNSG3dTexKey texKey,
                    NNSG3dTexKey tex4x4Key)
{
    if (texKey > 0)
    {
        NNS_G3D_NULL_ASSERT(pTex);
        NNS_G3D_ASSERTMSG(!(pTex->texInfo.flag & NNS_G3D_RESTEX_LOADED),
                        "NNS_G3dTexSetTexKey: Tex already loaded.");
        NNS_G3D_ASSERTMSG((pTex->texInfo.sizeTex << 3) <= NNS_GfdGetTexKeySize(texKey),
                        "NNS_G3dTexSetTexKey: texKey size not enough, "
                        "0x%05x bytes required.", pTex->texInfo.sizeTex << 3);
        pTex->texInfo.vramKey = texKey;
    }

    if (tex4x4Key > 0)
    {
        NNS_G3D_ASSERTMSG(!(pTex->tex4x4Info.flag & NNS_G3D_RESTEX4x4_LOADED),
                        "NNS_G3dTexSetTexKey(4x4): Tex already loaded.");
        NNS_G3D_ASSERTMSG(tex4x4Key & 0x80000000,
                        "NNS_G3dTexSetTexKey(4x4): texKey is not for 4x4comp textures");
        NNS_G3D_ASSERTMSG((pTex->tex4x4Info.sizeTex << 3) <= NNS_GfdGetTexKeySize(tex4x4Key),
                        "NNS_G3dTexSetTexKey(4x4): texKey size not enough, "
                        "0x%05x bytes required.", pTex->tex4x4Info.sizeTex << 3);
        pTex->tex4x4Info.vramKey = tex4x4Key;
    }
}


/*---------------------------------------------------------------------------*
    NNS_G3dTexLoad

    pTex内のテクスチャをテクスチャキーが示す場所にロードします。
    exec_begin_endをTRUEに指定すると、
    内部でVRAMバンクの切り替えを行います。FALSEの場合は、ユーザーが事前と事後に
    VRAMバンクの切り替えを行う必要があります。
 *---------------------------------------------------------------------------*/
void
NNS_G3dTexLoad(NNSG3dResTex* pTex, BOOL exec_begin_end)
{
    NNS_G3D_NULL_ASSERT(pTex);
    NNS_G3D_WARNING(!(pTex->texInfo.flag & NNS_G3D_RESTEX_LOADED),
                    "NNS_G3dTexLoad: texture already loaded");
    NNS_G3D_WARNING(!(pTex->tex4x4Info.flag & NNS_G3D_RESTEX4x4_LOADED),
                    "NNS_G3dTex4x4Load: texture already loaded");

    if (exec_begin_end)
    {
        // VRAMをバンク切り替えしてメインメモリのアドレス空間に持ってくる。
        GX_BeginLoadTex();
    }

    {
        //
        // 通常のテクスチャをロードする
        //

        u32 sz;
        const void* pData;
        u32 from;

        sz = (u32)pTex->texInfo.sizeTex << 3;
        if (sz > 0) // テクスチャデータが存在しない場合はsz == 0になる。
        {
            NNS_G3D_ASSERT(pTex->texInfo.vramKey != 0);
            NNS_G3D_ASSERTMSG(sz <= NNS_GfdGetTexKeySize(pTex->texInfo.vramKey),
                              "NNS_G3dTexLoad: texKey size not enough, "
                              "0x%05x bytes required.", sz);
            
            pData = NNS_G3dGetTexData(pTex);
            from = NNS_GfdGetTexKeyAddr(pTex->texInfo.vramKey);

            GX_LoadTex(pData, from, sz);
    
            pTex->texInfo.flag |= NNS_G3D_RESTEX_LOADED;
        }
    }

    {
        //
        // 4x4圧縮テクスチャをロードする
        //

        u32 sz;
        const void* pData;
        const void* pDataPlttIdx;
        u32 from;

        sz = (u32)pTex->tex4x4Info.sizeTex << 3;

        if (sz > 0) // テクスチャデータが存在しない場合はsz == 0になる。
        {
            NNS_G3D_ASSERT(pTex->tex4x4Info.sizeTex != 0);
            NNS_G3D_ASSERTMSG(NNS_GfdGetTexKey4x4Flag(pTex->tex4x4Info.vramKey),
                              "NNS_G3dTexLoad(4x4): texKey is not for 4x4comp textures");
            NNS_G3D_ASSERTMSG(sz <= NNS_GfdGetTexKeySize(pTex->tex4x4Info.vramKey),
                              "NNS_G3dTexLoad(4x4): texKey size not enough, "
                              "0x%05x bytes required.", sz);

            pData = NNS_G3dGetTex4x4Data(pTex);
            pDataPlttIdx = NNS_G3dGetTex4x4PlttIdxData(pTex);
            from = NNS_GfdGetTexKeyAddr(pTex->tex4x4Info.vramKey);

            GX_LoadTex(pData, from, sz);
            GX_LoadTex(pDataPlttIdx, GX_COMP4x4_PLTT_IDX(from), sz >> 1);
    
            pTex->tex4x4Info.flag |= NNS_G3D_RESTEX4x4_LOADED;
        }
    }

    if (exec_begin_end)
    {
        // バンク切り替えしたものを元に戻す
        GX_EndLoadTex();
    }
}


/*---------------------------------------------------------------------------*
    NNS_G3dTexReleaseTexKey

    テクスチャブロック内のテクスチャキー割り当てを解除します。
    キーは返り値としてユーザーに渡されます。
    VRAMのテクスチャ領域そのものの解放は行われません。
    ユーザーがキーを利用して解放する必要があります。
 *---------------------------------------------------------------------------*/
void
NNS_G3dTexReleaseTexKey(NNSG3dResTex* pTex,
                        NNSG3dTexKey* texKey,
                        NNSG3dTexKey* tex4x4Key)
{
    NNS_G3D_NULL_ASSERT(pTex);
    NNS_G3D_NULL_ASSERT(texKey);
    NNS_G3D_NULL_ASSERT(tex4x4Key);

    if (texKey)
    {
        pTex->texInfo.flag &= ~NNS_G3D_RESTEX_LOADED;
        *texKey = pTex->texInfo.vramKey;
        pTex->texInfo.vramKey = 0;
    }

    if (tex4x4Key)
    {
        pTex->tex4x4Info.flag &= ~NNS_G3D_RESTEX4x4_LOADED;
        *tex4x4Key = pTex->tex4x4Info.vramKey;
        pTex->tex4x4Info.vramKey = 0;
    }
}


/*---------------------------------------------------------------------------*
    NNS_G3dPlttGetRequiredSize

    テクスチャブロックが持つパレットのサイズを返します。
 *---------------------------------------------------------------------------*/
u32
NNS_G3dPlttGetRequiredSize(const NNSG3dResTex* pTex)
{
    NNS_G3D_NULL_ASSERT(pTex);

    if (pTex)
    {
        return (u32)(pTex->plttInfo.sizePltt << 3);
    }
    else
    {
        return 0;
    }
}


/*---------------------------------------------------------------------------*
    NNS_G3dPlttSetPlttKey

    テクスチャブロック内のパレットへパレットキーの割り当てを行います。
 *---------------------------------------------------------------------------*/
void
NNS_G3dPlttSetPlttKey(NNSG3dResTex* pTex, NNSG3dPlttKey plttKey)
{
    NNS_G3D_NULL_ASSERT(pTex);
    NNS_G3D_ASSERTMSG(!(pTex->plttInfo.flag & NNS_G3D_RESPLTT_LOADED),
                      "NNS_G3dPlttSetPlttKey: Palette already loaded.");
    NNS_G3D_ASSERTMSG(!(pTex->plttInfo.sizePltt > (plttKey >> 16)),
                      "NNS_G3dPlttSetPlttKey: plttKey size not enough, "
                      "0x%05x bytes required.", pTex->plttInfo.sizePltt << 3);

    pTex->plttInfo.vramKey = plttKey;
}


/*---------------------------------------------------------------------------*
    NNS_G3dPlttLoad

    pTex内のパレットをパレットキーが示す場所にロードします。
    exec_begin_endにTRUEを指定すると
    内部でVRAMバンクの切り替えを行います。FALSEの場合は、ユーザーが事前と事後に
    VRAMバンクの切り替えを行う必要があります。
 *---------------------------------------------------------------------------*/
void
NNS_G3dPlttLoad(NNSG3dResTex* pTex, BOOL exec_begin_end)
{
    NNS_G3D_NULL_ASSERT(pTex);
    NNS_G3D_WARNING(!(pTex->plttInfo.flag & NNS_G3D_RESPLTT_LOADED),
                    "NNS_G3dPlttLoad: texture already loaded");

    if (exec_begin_end)
    {
        // VRAMをバンク切り替えしてメインメモリのアドレス空間に持ってくる。
        GX_BeginLoadTexPltt();
    }

    {
        u32 sz;
        const void* pData;
        u32 from;

        sz = (u32)pTex->plttInfo.sizePltt << 3;
        pData = NNS_G3dGetPlttData(pTex);
        from = NNS_GfdGetTexKeyAddr(pTex->plttInfo.vramKey);

        GX_LoadTexPltt(pData, from, sz);
    
        pTex->plttInfo.flag |= NNS_G3D_RESPLTT_LOADED;
    }

    if (exec_begin_end)
    {
        // バンク切り替えしたものを元に戻す
        GX_EndLoadTexPltt();
    }
}


/*---------------------------------------------------------------------------*
    NNS_G3dPlttReleasePlttKey

    テクスチャブロック内のパレットブロックへのパレットキー割り当てを解除します。
    キーは返り値としてユーザーに渡されます。
    VRAMのパレット領域そのものの解放は行われません。
    ユーザーがキーを利用して解放する必要があります。
 *---------------------------------------------------------------------------*/
NNSG3dPlttKey
NNS_G3dPlttReleasePlttKey(NNSG3dResTex* pTex)
{
    u32 rval;
    NNS_G3D_NULL_ASSERT(pTex);

    pTex->plttInfo.flag &= ~NNS_G3D_RESPLTT_LOADED;
    
    rval = pTex->plttInfo.vramKey;
    pTex->plttInfo.vramKey = 0;
    return rval;
}


static void
bindMdlTex_Internal_(NNSG3dResMat* pMat,
                     NNSG3dResDictTexToMatIdxData* pBindData,
                     const NNSG3dResTex* pTex,
                     const NNSG3dResDictTexData* pTexData)
{
    u8* base = (u8*)pMat + pBindData->offset;
    u32 vramOffset;
    u32 j;

    NNS_G3D_ASSERTMSG((pTex->texInfo.vramKey != 0 || pTex->texInfo.sizeTex == 0),
                      "No texture key assigned");
    NNS_G3D_ASSERTMSG((pTex->tex4x4Info.vramKey != 0 || pTex->tex4x4Info.sizeTex == 0),
                      "No texture(4x4) key assigned");

    if ((pTexData->texImageParam & REG_G3_TEXIMAGE_PARAM_TEXFMT_MASK) !=
                    (GX_TEXFMT_COMP4x4 << REG_G3_TEXIMAGE_PARAM_TEXFMT_SHIFT))
    {
        // 4x4以外のテクスチャ
        vramOffset = NNS_GfdGetTexKeyAddr(pTex->texInfo.vramKey)
                            >> NNS_GFD_TEXKEY_ADDR_SHIFT;
    }
    else
    {
        // 4x4テクスチャ
        vramOffset = NNS_GfdGetTexKeyAddr(pTex->tex4x4Info.vramKey)
                            >> NNS_GFD_TEXKEY_ADDR_SHIFT;
    }

    for (j = 0; j < pBindData->numIdx; ++j)
    {
        s32 w, h;

        // 各matDataにテクスチャ情報をセットアップしていく。
        NNSG3dResMatData* matData = NNS_G3dGetMatDataByIdx(pMat, *(base + j));

        matData->texImageParam |= (pTexData->texImageParam + vramOffset);
        w = (s32)(((pTexData->extraParam) & NNS_G3D_TEXIMAGE_PARAMEX_ORIGW_MASK) >> NNS_G3D_TEXIMAGE_PARAMEX_ORIGW_SHIFT);
        h = (s32)(((pTexData->extraParam) & NNS_G3D_TEXIMAGE_PARAMEX_ORIGH_MASK) >> NNS_G3D_TEXIMAGE_PARAMEX_ORIGH_SHIFT);

        matData->magW = (w != matData->origWidth) ?
                        FX_Div(w << FX32_SHIFT, matData->origWidth << FX32_SHIFT) :
                        FX32_ONE;
        matData->magH = (h != matData->origHeight) ?
                        FX_Div(h << FX32_SHIFT, matData->origHeight << FX32_SHIFT) :
                        FX32_ONE;
    }

    // バインド状態にする。
    pBindData->flag |= 1;
}

static void
releaseMdlTex_Internal_(NNSG3dResMat* pMat,
                        NNSG3dResDictTexToMatIdxData* pData)
{
    // バインド状態の場合リリースする
    u8* base = (u8*)pMat + pData->offset;
    u32 j;

    for (j = 0; j < pData->numIdx; ++j)
    {
        // 各matDataにテクスチャ情報をセットアップしていく。
        NNSG3dResMatData* matData = NNS_G3dGetMatDataByIdx(pMat, *(base + j));

        // flipとrepeatとtexgenを残しリセット
        matData->texImageParam &= REG_G3_TEXIMAGE_PARAM_TGEN_MASK |
                                  REG_G3_TEXIMAGE_PARAM_FT_MASK | REG_G3_TEXIMAGE_PARAM_FS_MASK |
                                  REG_G3_TEXIMAGE_PARAM_RT_MASK | REG_G3_TEXIMAGE_PARAM_RS_MASK;
        matData->magH = matData->magW = FX32_ONE;
    }

    // アンバインド状態にする
    pData->flag &= ~1;
}


/*---------------------------------------------------------------------------*
    NNS_G3dBindMdlTex

    モデル内で、テクスチャ名に関連付けられているマテリアルのテクスチャエントリを
    テクスチャブロック内の同名のテクスチャ名を持つテクスチャデータに関連付けます。
 *---------------------------------------------------------------------------*/
BOOL
NNS_G3dBindMdlTex(NNSG3dResMdl* pMdl, const NNSG3dResTex* pTex)
{
    NNSG3dResMat*  mat;
    NNSG3dResDict* dictTex;
    u32 i;
    BOOL result = TRUE;

    NNS_G3D_NULL_ASSERT(pMdl);
    NNS_G3D_NULL_ASSERT(pTex);

    mat     = NNS_G3dGetMat(pMdl);
    dictTex = (NNSG3dResDict*)((u8*)mat + mat->ofsDictTexToMatList);
    
    // モデルリソースのテクスチャ名->マテリアルインデックス列辞書内の
    // それぞれに対してループを回す
    for (i = 0; i < dictTex->numEntry; ++i)
    {
        const NNSG3dResName* name = NNS_G3dGetResNameByIdx(dictTex, i);

        // テクスチャブロック内の同名エントリを検索
        const NNSG3dResDictTexData* texData = NNS_G3dGetTexDataByName(pTex, name);

        if (texData)
        {
            // 存在していればi番目のテクスチャに関するバインドデータを取得
            NNSG3dResDictTexToMatIdxData* data =
                (NNSG3dResDictTexToMatIdxData*) NNS_G3dGetResDataByIdx(dictTex, i);
        
            if (!(data->flag & 1))
            {
                // バインド状態でない場合、バインドする
                bindMdlTex_Internal_(mat, data, pTex, texData);
            }
        }
        else
        {
            result = FALSE;
        }
    }
    return result;
}


/*---------------------------------------------------------------------------*
    NNS_G3dBindMdlTexEx

    モデル内で、pResNameというテクスチャ名に関連付けられているマテリアルの
    テクスチャエントリをテクスチャブロック内の名前pResNameを持つテクスチャ
    に関連付けます。
 *---------------------------------------------------------------------------*/
BOOL
NNS_G3dBindMdlTexEx(NNSG3dResMdl* pMdl,
                    const NNSG3dResTex* pTex,
                    const NNSG3dResName* pResName)
{
    NNSG3dResMat* mat;
    NNSG3dResDict* dictTex;
    const NNSG3dResDictTexData* texData;
    NNSG3dResDictTexToMatIdxData* data;

    NNS_G3D_NULL_ASSERT(pMdl);
    NNS_G3D_NULL_ASSERT(pTex);
    NNS_G3D_NULL_ASSERT(pResName);

    mat     = NNS_G3dGetMat(pMdl);
    dictTex = (NNSG3dResDict*)((u8*)mat + mat->ofsDictTexToMatList);

    // テクスチャブロック内の辞書を検索
    texData = NNS_G3dGetTexDataByName(pTex, pResName);

    if (texData)
    {
        // モデルデータ内のテクスチャ->マテリアルインデックス列の辞書を検索
        data = (NNSG3dResDictTexToMatIdxData*)
                    NNS_G3dGetResDataByName(dictTex, pResName);

        if (data && !(data->flag & 1))
        {
            // モデルとテクスチャの両方にpResNameに対応するデータがあって、
            // バインド状態でない場合、バインドする
            bindMdlTex_Internal_(mat, data, pTex, texData);
            return TRUE;
        }
    }
    return FALSE;
}


/*---------------------------------------------------------------------------*
    NNS_G3dForceBindMdlTex

    モデル内でテクスチャ名->マテリアルリスト辞書のtexToMatListIdx番目のエントリ
    に格納されているマテリアルのテクスチャエントリとテクスチャブロック内の
    texIdx番目のテクスチャに強制的に関連付けます。
 *---------------------------------------------------------------------------*/
BOOL
NNS_G3dForceBindMdlTex(NNSG3dResMdl* pMdl,
                       const NNSG3dResTex* pTex,
                       u32 texToMatListIdx,
                       u32 texIdx)
{
    NNSG3dResMat* mat;
    NNSG3dResDict* dictTex;
    const NNSG3dResDictTexData* texData;
    NNSG3dResDictTexToMatIdxData* data;

    NNS_G3D_NULL_ASSERT(pMdl);
    NNS_G3D_NULL_ASSERT(pTex);

    mat     = NNS_G3dGetMat(pMdl);
    dictTex = (NNSG3dResDict*)((u8*)mat + mat->ofsDictTexToMatList);

    // テクスチャブロック内のテクスチャデータをインデックス引きで取得
    texData = NNS_G3dGetTexDataByIdx(pTex, texIdx);

    // モデルデータ内のテクスチャ->マテリアルインデックス列の辞書を検索
    data = (NNSG3dResDictTexToMatIdxData*)
                NNS_G3dGetResDataByIdx(dictTex, texToMatListIdx);

    if (data)
    {
        // pResNameの名前をもつテクスチャがマテリアルに存在すれば強制的にバインドする。
        bindMdlTex_Internal_(mat, data, pTex, texData);
        return TRUE;
    }
    return FALSE;
}


/*---------------------------------------------------------------------------*
    NNS_G3dReleaseMdlTex

    モデル内で、テクスチャに関連付けられているマテリアルのテクスチャエントリ
    の関連付けを解除します。
 *---------------------------------------------------------------------------*/
void
NNS_G3dReleaseMdlTex(NNSG3dResMdl* pMdl)
{
    NNSG3dResMat*  mat;
    NNSG3dResDict* dictTex;
    u32 i;

    NNS_G3D_NULL_ASSERT(pMdl);

    mat      = NNS_G3dGetMat(pMdl);
    dictTex  = (NNSG3dResDict*)((u8*)mat + mat->ofsDictTexToMatList);
    
    // モデルリソースのテクスチャ名->マテリアルインデックス列辞書内の
    // それぞれに対してループを回す
    for (i = 0; i < dictTex->numEntry; ++i)
    {
        NNSG3dResDictTexToMatIdxData* data =
            (NNSG3dResDictTexToMatIdxData*) NNS_G3dGetResDataByIdx(dictTex, i);

        if (data->flag & 1)
        {
            // バインドされている状態ならばリリース
            releaseMdlTex_Internal_(mat, data);
        }
    }
}


/*---------------------------------------------------------------------------*
    NNS_G3dReleaseMdlTexEx

    モデル内で、名前pResNameという名前をもつテクスチャに関連付けられている
    マテリアルのテクスチャエントリの関連付けを解除します。
 *---------------------------------------------------------------------------*/
BOOL NNS_G3dReleaseMdlTexEx(NNSG3dResMdl* pMdl, const NNSG3dResName* pResName)
{
    NNSG3dResMat*  mat;
    NNSG3dResDict* dictTex;
    NNSG3dResDictTexToMatIdxData* data;

    NNS_G3D_NULL_ASSERT(pMdl);
    NNS_G3D_NULL_ASSERT(pResName);

    mat      = NNS_G3dGetMat(pMdl);
    dictTex  = (NNSG3dResDict*)((u8*)mat + mat->ofsDictTexToMatList);
    data     = (NNSG3dResDictTexToMatIdxData*)NNS_G3dGetResDataByName(dictTex, pResName);

    if (data && (data->flag & 1))
    {
        // バインドされている状態ならばリリース
        releaseMdlTex_Internal_(mat, data);
        return TRUE;
    }
    return FALSE;
}


static void
bindMdlPltt_Internal_(NNSG3dResMat* pMat,
                      NNSG3dResDictPlttToMatIdxData* pBindData,
                      const NNSG3dResTex* pTex,
                      const NNSG3dResDictPlttData* pPlttData)
{
    // バインド状態でない場合、バインドする
    u8* base = (u8*)pMat + pBindData->offset;
    u16 plttBase = pPlttData->offset;
    u16 vramOffset = (u16)(NNS_GfdGetTexKeyAddr(pTex->plttInfo.vramKey) >> NNS_GFD_TEXKEY_ADDR_SHIFT);
    u32 j;

    NNS_G3D_ASSERTMSG((pTex->plttInfo.vramKey != 0 || pTex->plttInfo.sizePltt == 0),
                       "No palette key assigned");
    
    // 4colorsならビットが立っている
    if (!(pPlttData->flag & 1))
    {
        // 4colors以外の場合は4bitシフトになる。
        // 4colorsの場合は3bitシフトなのでそのまま
        plttBase >>= 1;
        vramOffset >>= 1;
    }

    for (j = 0; j < pBindData->numIdx; ++j)
    {
        // 各matDataにパレット情報をセットアップしていく。
        NNSG3dResMatData* matData = NNS_G3dGetMatDataByIdx(pMat, *(base + j));
        matData->texPlttBase = (u16)(plttBase + vramOffset);
    }

    // バインド状態にする
    pBindData->flag |= 1;
}


/*---------------------------------------------------------------------------*
    NNS_G3dBindMdlPltt

    モデル内で、パレット名に関連付けられているマテリアルのパレットエントリを
    テクスチャブロック内の同名のパレット名を持つパレットデータに関連付けます。
 *---------------------------------------------------------------------------*/
BOOL
NNS_G3dBindMdlPltt(NNSG3dResMdl* pMdl, const NNSG3dResTex* pTex)
{
    NNSG3dResMat*  mat;
    NNSG3dResDict* dictPltt;
    u32 i;
    BOOL result = TRUE;

    NNS_G3D_NULL_ASSERT(pMdl);
    NNS_G3D_NULL_ASSERT(pTex);

    mat      = NNS_G3dGetMat(pMdl);
    dictPltt = (NNSG3dResDict*)((u8*)mat + mat->ofsDictPlttToMatList);

    for (i = 0; i < dictPltt->numEntry; ++i)
    {
        // モデル内のパレット名->マテリアルインデックス列辞書からパレット名をとる
        const NNSG3dResName* name = NNS_G3dGetResNameByIdx(dictPltt, i);
        
        // テクスチャブロックからパレット名に対応するデータフィールドを得る
        const NNSG3dResDictPlttData* plttData = NNS_G3dGetPlttDataByName(pTex, name);

        if (plttData)
        {
            // テクスチャブロックにパレット名が存在
            NNSG3dResDictPlttToMatIdxData* data;
            data = (NNSG3dResDictPlttToMatIdxData*) NNS_G3dGetResDataByIdx(dictPltt, i);

            if (!(data->flag & 1))
            {
                bindMdlPltt_Internal_(mat, data, pTex, plttData);
            }
        }
        else
        {
            result = FALSE;
        }
    }
    return result;
}


/*---------------------------------------------------------------------------*
    NNS_G3dBindMdlPlttEx

    モデル内で、pResNameというパレット名に関連付けられているマテリアルの
    パレットエントリをテクスチャブロック内の名前pResNameを持つパレット
    に関連付けます。
 *---------------------------------------------------------------------------*/
BOOL
NNS_G3dBindMdlPlttEx(NNSG3dResMdl* pMdl,
                     const NNSG3dResTex* pTex,
                     const NNSG3dResName* pResName)
{
    NNSG3dResMat*  mat;
    NNSG3dResDict* dictPltt;
    const NNSG3dResDictPlttData* plttData;
    NNSG3dResDictPlttToMatIdxData* data;

    NNS_G3D_NULL_ASSERT(pMdl);
    NNS_G3D_NULL_ASSERT(pTex);
    NNS_G3D_NULL_ASSERT(pResName);

    mat      = NNS_G3dGetMat(pMdl);
    dictPltt = (NNSG3dResDict*)((u8*)mat + mat->ofsDictPlttToMatList);

    // テクスチャブロックからパレット名に対応するデータフィールドを得る
    plttData = NNS_G3dGetPlttDataByName(pTex, pResName);

    if (plttData)
    {
        // テクスチャブロックにパレット名が存在
        data = (NNSG3dResDictPlttToMatIdxData*)
                    NNS_G3dGetResDataByName(dictPltt, pResName);
        
        if (data && !(data->flag & 1))
        {
            bindMdlPltt_Internal_(mat, data, pTex, plttData);
            return TRUE;
        }
    }
    return FALSE;
}


/*---------------------------------------------------------------------------*
    NNS_G3dForceBindMdlPltt

    モデル内でパレット名->マテリアルリスト辞書のplttToMatListIdx番目のエントリ
    に格納されているマテリアルのパレットエントリとテクスチャブロック内の
    plttIdx番目のパレットに強制的に関連付けます。
 *---------------------------------------------------------------------------*/
BOOL NNS_G3dForceBindMdlPltt(NNSG3dResMdl* pMdl,
                             const NNSG3dResTex* pTex,
                             u32 plttToMatListIdx,
                             u32 plttIdx)
{
    NNSG3dResMat*  mat;
    NNSG3dResDict* dictPltt;
    const NNSG3dResDictPlttData* plttData;
    NNSG3dResDictPlttToMatIdxData* data;

    NNS_G3D_NULL_ASSERT(pMdl);
    NNS_G3D_NULL_ASSERT(pTex);

    mat      = NNS_G3dGetMat(pMdl);
    dictPltt = (NNSG3dResDict*)((u8*)mat + mat->ofsDictPlttToMatList);
    plttData = NNS_G3dGetPlttDataByIdx(pTex, plttIdx);

    data = (NNSG3dResDictPlttToMatIdxData*)
                NNS_G3dGetResDataByIdx(dictPltt, plttToMatListIdx);
        
    if (data && !(data->flag & 1))
    {
        bindMdlPltt_Internal_(mat, data, pTex, plttData);
        return TRUE;
    }
    return FALSE;
}



/*---------------------------------------------------------------------------*
    NNS_G3dReleaseMdlPltt

    モデル内で、パレットに関連付けられているマテリアルのパレットエントリ
    の関連付けを解除します。
 *---------------------------------------------------------------------------*/
void
NNS_G3dReleaseMdlPltt(NNSG3dResMdl* pMdl)
{
    NNSG3dResMat*  mat;
    NNSG3dResDict* dictPltt;
    u32 i;

    NNS_G3D_NULL_ASSERT(pMdl);

    mat      = NNS_G3dGetMat(pMdl);
    dictPltt = (NNSG3dResDict*)((u8*)mat + mat->ofsDictPlttToMatList);
    for (i = 0; i < dictPltt->numEntry; ++i)
    {
        NNSG3dResDictPlttToMatIdxData* data =
            (NNSG3dResDictPlttToMatIdxData*) NNS_G3dGetResDataByIdx(dictPltt, i);
         
        if (data->flag & 1)
        {
            // バインド状態の場合リリースする
#if 0
            u32 j;
            // フラグ操作だけでＯＫなので実行しない
            u8* base = (u8*)mat + data->offset;

            for (j = 0; j < data->numIdx; ++j)
            {
                // 各matDataをリセットする.
                NNSG3dResMatData* matData = NNS_G3dGetMatDataByIdx(mat, *(base + j));
                matData->texPlttBase = 0;
            }
#endif
            // アンバインドにする。
            data->flag &= ~1;
        }
    }
}


/*---------------------------------------------------------------------------*
    NNS_G3dReleaseMdlPlttEx

    モデル内で、名前pResNameという名前をもつパレットに関連付けられている
    マテリアルのパレットエントリの関連付けを解除します。
 *---------------------------------------------------------------------------*/
BOOL
NNS_G3dReleaseMdlPlttEx(NNSG3dResMdl* pMdl, const NNSG3dResName* pResName)
{
    NNSG3dResMat*  mat;
    NNSG3dResDict* dictPltt;
    NNSG3dResDictPlttToMatIdxData* data;

    NNS_G3D_NULL_ASSERT(pMdl);
    NNS_G3D_NULL_ASSERT(pResName);

    mat      = NNS_G3dGetMat(pMdl);
    dictPltt = (NNSG3dResDict*)((u8*)mat + mat->ofsDictPlttToMatList);
    data = (NNSG3dResDictPlttToMatIdxData*)
                NNS_G3dGetResDataByName(dictPltt, pResName);

    if (data && (data->flag & 1))
    {
        // バインド状態の場合リリースする
        data->flag &= ~1;
        return TRUE;
    }
    return FALSE;
}


/*---------------------------------------------------------------------------*
    NNS_G3dBindMdlSet

    各モデルをテクスチャ／パレットに関連付けます。
    テクスチャはTexKey, PlttKeyがセットされている必要があります。
 *---------------------------------------------------------------------------*/
BOOL
NNS_G3dBindMdlSet(NNSG3dResMdlSet* pMdlSet, const NNSG3dResTex* pTex)
{
    u32 i;
    BOOL result = TRUE;

    NNS_G3D_NULL_ASSERT(pMdlSet);
    NNS_G3D_NULL_ASSERT(pTex);

    for (i = 0; i < pMdlSet->dict.numEntry; ++i)
    {
        NNSG3dResMdl* mdl = NNS_G3dGetMdlByIdx(pMdlSet, i);
        NNS_G3D_NULL_ASSERT(mdl);

        result &= NNS_G3dBindMdlTex(mdl, pTex);
        result &= NNS_G3dBindMdlPltt(mdl, pTex);
    }
    return result;
}


/*---------------------------------------------------------------------------*
    NNS_G3dReleaseMdlSet

    各モデルからテクスチャ／パレットへの関連付けを外します。
 *---------------------------------------------------------------------------*/
void
NNS_G3dReleaseMdlSet(NNSG3dResMdlSet* pMdlSet)
{
    u32 i;
    NNS_G3D_NULL_ASSERT(pMdlSet);

    for (i = 0; i < pMdlSet->dict.numEntry; ++i)
    {
        NNSG3dResMdl* mdl = NNS_G3dGetMdlByIdx(pMdlSet, i);
        NNS_G3D_NULL_ASSERT(mdl);

        NNS_G3dReleaseMdlTex(mdl);
        NNS_G3dReleaseMdlPltt(mdl);
    }
}
