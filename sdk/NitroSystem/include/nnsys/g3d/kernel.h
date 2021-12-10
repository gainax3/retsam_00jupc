/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g3d
  File:     kernel.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.36 $
 *---------------------------------------------------------------------------*/

//
// AUTHOR: Kenji Nishida
//

#ifndef NNSG3D_KERNEL_H_
#define NNSG3D_KERNEL_H_

#include <nnsys/g3d/config.h>
#include <nnsys/g3d/binres/res_struct.h>
#include <nnsys/g3d/binres/res_struct_accessor_anm.h>

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////
//
// 構造体定義及びtypedef
//

#define NNS_G3D_SIZE_MAT_MAX_MAX 256
#define NNS_G3D_SIZE_JNT_MAX_MAX 256
#define NNS_G3D_SIZE_SHP_MAX_MAX 256

#if (NNS_G3D_SIZE_MAT_MAX <= 0 || NNS_G3D_SIZE_MAT_MAX > NNS_G3D_SIZE_MAT_MAX_MAX)
#error NNS_G3D_SIZE_MAT_MAX range error.
#endif

#if (NNS_G3D_SIZE_JNT_MAX <= 0 || NNS_G3D_SIZE_JNT_MAX > NNS_G3D_SIZE_JNT_MAX_MAX)
#error NNS_G3D_SIZE_JNT_MAX range error.
#endif

#if (NNS_G3D_SIZE_SHP_MAX <= 0 || NNS_G3D_SIZE_SHP_MAX > NNS_G3D_SIZE_SHP_MAX_MAX)
#error NNS_G3D_SIZE_SHP_MAX range error.
#endif

#if (NNS_G3D_SIZE_MAT_MAX % 32 != 0)
#error NNS_G3D_SIZE_MAT_MAX must be a multiple of 32.
#endif

#if (NNS_G3D_SIZE_JNT_MAX % 32 != 0)
#error NNS_G3D_SIZE_JNT_MAX must be a multiple of 32.
#endif

#if (NNS_G3D_SIZE_SHP_MAX % 32 != 0)
#error NNS_G3D_SIZE_SHP_MAX must be a multiple of 32.
#endif

typedef u32 NNSG3dTexKey;    // compatible with NNSGfdTexKey
typedef u32 NNSG3dPlttKey;   // compatible with NNSGfdPlttKey


/*---------------------------------------------------------------------------*
    NNSG3dAnmObj

    NNSG3dRenderObjに参照される構造体。メモリの確保・解放はユーザーが自ら行う
    必要があります。初期化はNNS_G3dAnmObjInit関数で行います。
    この構造体の役割は、
    ・アニメーションリソースとそれを処理する関数のペアを指定し、
    ・アニメーションリソースとモデルリソースの関連付けを行い、
    ・アニメーションの現在のフレームを保持することです。

    frame:      どのフレームを再生するか指定する。
    ratio:      アニメーションブレンド関数が使用する。
    resAnm:     個々のアニメーションリソースへのポインタ
    funcAnm:    resAnmのframe番目のフレームを再生する関数へのポインタ
    next:       NNS_G3dRenderObjBindAnmObj, NNS_G3dRenderObjReleaseAnmObjで
                設定される。
    resTex:     テクスチャリソースへのポインタ。アニメーションが必要とする場合のみ
                設定される。
    priority:   NNSG3dRenderObjに登録される際の優先順位
    numMapData: モデルリソースとアニメリソースのインデックスの対応をとるテーブル
                のエントリの数
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dAnmObj_
{
    fx32                  frame;
    fx32                  ratio;
    void*                 resAnm;     // リソースファイル内アニメーションデータブロックへのポインタ
    void*                 funcAnm;    // 各アニメーションの関数ポインタにキャストされる。デフォルトが入るが変更できる
    struct NNSG3dAnmObj_* next;
    const NNSG3dResTex*   resTex;     // テクスチャブロックの情報が必要な場合(テクスチャパターンアニメーションのみ)
    u8                    priority;
    u8                    numMapData;
    u16                   mapData[1]; // numMapData個の配列になる(NNSG3dAnmObjMapData)
}
NNSG3dAnmObj;

//
// NNSG3dAnmObjに必要なメモリのサイズはモデルリソースとアニメーションのカテゴリから決定される。
// マテリアルアニメーションの場合は、以下のマクロを使用する。
// 本当はsizeof(NNSG3dAnmObj) + sizeof(u16) * (pMdl->info.numMat - 1)
// だが4バイト境界にする
//
#define NNS_G3D_ANMOBJ_SIZE_MATANM(pMdl) ((sizeof(NNSG3dAnmObj) + sizeof(u16) * pMdl->info.numMat) & ~3)

//
// NNSG3dAnmObjに必要なメモリのサイズはモデルリソースとアニメーションのカテゴリから決定される。
// ジョイントアニメーションとビジビリティアニメーションの場合は、以下のマクロを使用する。
// 本当はsizeof(NNSG3dAnmObj) + sizeof(u16) * (pMdl->info.numNode - 1)
// だが4バイト境界にする
//
#define NNS_G3D_ANMOBJ_SIZE_JNTANM(pMdl) ((sizeof(NNSG3dAnmObj) + sizeof(u16) * pMdl->info.numNode) & ~3)
#define NNS_G3D_ANMOBJ_SIZE_VISANM(pMdl) ((sizeof(NNSG3dAnmObj) + sizeof(u16) * pMdl->info.numNode) & ~3)

/*---------------------------------------------------------------------------*
    NNSG3dAnmObjMapData

    NNSG3dAnmObjのmapData配列に格納されるデータのためのユーティリティ的な
    列挙型。
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_ANMOBJ_MAPDATA_EXIST     = 0x0100,
    NNS_G3D_ANMOBJ_MAPDATA_DISABLED  = 0x0200,
    NNS_G3D_ANMOBJ_MAPDATA_DATAFIELD = 0x00ff
}
NNSG3dAnmObjMapData;


/*---------------------------------------------------------------------------*
    アニメーションブレンド関数のtypedef
 *---------------------------------------------------------------------------*/
// anm.hで定義
struct NNSG3dMatAnmResult_;
struct NNSG3dJntAnmResult_;
struct NNSG3dVisAnmResult_;


// マテリアルアニメーションブレンド関数
typedef BOOL (*NNSG3dFuncAnmBlendMat)(struct NNSG3dMatAnmResult_*,
                                      const NNSG3dAnmObj*,
                                      u32);

// ジョイントアニメーションブレンド関数
typedef BOOL (*NNSG3dFuncAnmBlendJnt)(struct NNSG3dJntAnmResult_*,
                                      const NNSG3dAnmObj*,
                                      u32);

// ビジビリティアニメーションブレンド関数
typedef BOOL (*NNSG3dFuncAnmBlendVis)(struct NNSG3dVisAnmResult_*,
                                      const NNSG3dAnmObj*,
                                      u32);


/*---------------------------------------------------------------------------*
    NNSG3dRenderObjFlag

    NNSG3dRenderObjが持つフラグで、NNS_G3dDrawの動作をカスタマイズできる。
    
    NNS_G3D_RENDEROBJ_FLAG_RECORD
        NNS_G3dDrawの実行時に、recJntAnm, recMatAnmにジョイントやマテリアルの
        計算結果をストアします。実行終了時にこのフラグはリセットされます。
        このフラグがリセットされていて、recJntAnm, recMatAnmがNULLで無い場合は
        recJntAnm, recMatAnm内の計算結果がそのまま使用されます。
    NNS_G3D_RENDEROBJ_FLAG_NOGECMD
        NNS_G3dDrawの実行時に、ジオメトリコマンドを送信しません。
    NNS_G3D_RENDEROBJ_FLAG_SKIP_SBC_DRAW
        NNS_G3dDrawの実行時に、SBCの描画系コマンドの実行をスキップします。
    NNS_G3D_RENDEROBJ_FLAG_SKIP_SBC_MTXCALC
        NNS_G3dDrawの実行時に、SBCの行列計算系コマンドの実行をスキップします。

    NNS_G3D_RENDEROBJ_FLAG_HINT_OBSOLETE
        G3D内部でセット／リセットされるフラグでhintXXXAnmExistが正しくない状態
        の場合セットされます(NNS_G3dRenderObjRemoveAnmObjされたときセット)

    NNS_G3D_RENDEROBJ_FLAG_SKIP_SBC_DRAWとNNS_G3D_RENDEROBJ_FLAG_SKIP_SBC_MTXCALCは
    g3dcvtrで-sオプションをつけてコンバートしたモデルに対して有効です。
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_RENDEROBJ_FLAG_RECORD             = 0x00000001,
    NNS_G3D_RENDEROBJ_FLAG_NOGECMD            = 0x00000002,
    NNS_G3D_RENDEROBJ_FLAG_SKIP_SBC_DRAW      = 0x00000004,
    NNS_G3D_RENDEROBJ_FLAG_SKIP_SBC_MTXCALC   = 0x00000008,
    NNS_G3D_RENDEROBJ_FLAG_HINT_OBSOLETE      = 0x00000010
}
NNSG3dRenderObjFlag;


/*---------------------------------------------------------------------------*
    NNSG3dSbcCallBackFunc

    NNSG3dRS内に格納されるコールバック関数へのポインタ
 *---------------------------------------------------------------------------*/
struct NNSG3dRS_;
typedef void (*NNSG3dSbcCallBackFunc)(struct NNSG3dRS_*);


/*---------------------------------------------------------------------------*
    NNSG3dSbcCallBackTiming

    SBC命令内で起動するコールバックのタイミングを3種類から指定できる。
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_SBC_CALLBACK_TIMING_NONE = 0x00,
    NNS_G3D_SBC_CALLBACK_TIMING_A    = 0x01,
    NNS_G3D_SBC_CALLBACK_TIMING_B    = 0x02,
    NNS_G3D_SBC_CALLBACK_TIMING_C    = 0x03
}
NNSG3dSbcCallBackTiming;

#define NNS_G3D_SBC_CALLBACK_TIMING_ASSERT(x)               \
    NNS_G3D_ASSERT(x == NNS_G3D_SBC_CALLBACK_TIMING_NONE || \
                   x == NNS_G3D_SBC_CALLBACK_TIMING_A    || \
                   x == NNS_G3D_SBC_CALLBACK_TIMING_B    || \
                   x == NNS_G3D_SBC_CALLBACK_TIMING_C)

/*---------------------------------------------------------------------------*
    NNSG3dRenderObj

    構造体のメンバ等は常に変化する可能性がある
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dRenderObj_
{
    u32 flag; // NNSG3dRenderObjFlag

    // NOTICE:
    // NNS_G3dDraw内ではNNSG3dResMdlの中味を書き換えることはありません。
    // (コールバックを使った場合等は除く)
    NNSG3dResMdl*         resMdl;
    NNSG3dAnmObj*         anmMat;
    NNSG3dFuncAnmBlendMat funcBlendMat;
    NNSG3dAnmObj*         anmJnt;
    NNSG3dFuncAnmBlendJnt funcBlendJnt;
    NNSG3dAnmObj*         anmVis;
    NNSG3dFuncAnmBlendVis funcBlendVis;

    // コールバック用情報
    NNSG3dSbcCallBackFunc cbFunc;              // NULLならばコールバックはない。
    u8                    cbCmd;               // 停止位置をコマンドで指定。NNS_G3D_SBC_*****(res_struct.hで定義)
    u8                    cbTiming;            // NNSG3dSbcCallBackTiming(sbc.hで定義)
    u16                   dummy_;

    // レンダリング開始直前に呼ばれる。一般的にNNS_G3dRSの
    // コールバックベクタを設定するために使われる。
    NNSG3dSbcCallBackFunc cbInitFunc;

    // ユーザーが管理する領域へのポインタ
    // あらかじめポインタを設定しておけばコールバックでの使用が可能.
    void*                 ptrUser;

    //
    // デフォルトではresMdl内に格納されているSBCを使用するが、
    // ptrUserSbcにポインタを格納することによって、ユーザー定義のSBCが
    // 使用されるようになる。
    //
    // Example:
    // パーティクルシステム等で、単純なモデル(1マテリアル1シェイプ)をいろいろな
    // 場所に出したいような場合、次のようなSBCコードを作成してコードへのポインタを
    // ptrUserSbcに代入しておくと効率がよい。
    // 
    // MAT[000] 0
    // MTX 0
    // SHP 0
    // MTX 1
    // SHP 0
    // ....
    // MTX n
    // SHP 0
    // RET
    //
    // あらかじめ行列は行列スタックにセットしておく必要がある。
    // また、マテリアルをシェイプ毎に少しずつ変えたい場合は、
    // コールバックで変更するのがよいだろう。
    //
    u8*                   ptrUserSbc;

    //
    // 計算結果のバッファリング領域へのポインタ
    // ジョイント・マテリアルの計算結果を複数フレーム／複数体のモデル
    // で使いまわしたい場合は、recJntAnm/recMatAnmにバッファをセットする。
    //
    // flagのNNS_G3D_RENDEROBJ_FLAG_RECORDがONのとき、
    // recJntAnm, recMatAnmでNULLでないものについて、
    // それぞれマテリアル・ジョイントの計算結果が記録されていく。
    //
    // flagのNNS_G3D_RENDEROBJ_FLAG_RECORDがOFFのとき、
    // recJntAnm, recMatAnmがNULLでないものについて、
    // それぞれマテリアル・ジョイントの計算結果として利用していく。
    //
    // ユーザーは
    // recJntAnmについては、
    // sizeof(NNSG3dJntAnmResult) * resMdl->info.numNodeバイト
    // recMatAnmについては、
    // sizeof(NNSG3dMatAnmResult) * resMdl->info.numMatバイト
    // の領域を確保しておかなくてはならない。
    //
    struct NNSG3dJntAnmResult_*   recJntAnm;
    struct NNSG3dMatAnmResult_*   recMatAnm;

    //
    // アニメーションが追加されたとき、matID/nodeIDに対して定義されていると
    // bitが1になる。各IDは最大でも256個までなので、8ワードずつで管理できる。
    // ただし、アニメーションが削除された場合にもbitは1のままである。
    // SBCのインタプリタはこのフィールドをチェックして、
    // NNSG3dFuncBlendMatXXXをコールするかどうかを決定する。
    // 注意すべきなのは、bitが0のときは、そのmatID/nodeIDに関係した
    // アニメーションがない、ということだけが確実であることである。
    //
    u32 hintMatAnmExist[NNS_G3D_SIZE_MAT_MAX / 32];
    u32 hintJntAnmExist[NNS_G3D_SIZE_JNT_MAX / 32];
    u32 hintVisAnmExist[NNS_G3D_SIZE_JNT_MAX / 32];
}
NNSG3dRenderObj;

// NNSG3dJntAnmResult計算結果をバッファリングする際に
// 使用するバッファのサイズ計算マクロ
#define NNS_G3D_RENDEROBJ_JNTBUFFER_SIZE(numJnt) \
    ((u32)(sizeof(NNSG3dJntAnmResult) * (numJnt)))

// NNSG3dMatAnmResult計算結果をバッファリングする際に
// 使用するバッファのサイズ計算マクロ
#define NNS_G3D_RENDEROBJ_MATBUFFER_SIZE(numMat) \
    ((u32)(sizeof(NNSG3dMatAnmResult) * (numMat)))





////////////////////////////////////////////////////////////////////////////////
//
// 関数の宣言
//

//
// inline functions for NNSG3dAnmObj
//
NNS_G3D_INLINE void NNS_G3dAnmObjSetFrame(NNSG3dAnmObj* pAnmObj, fx32 frame);
NNS_G3D_INLINE void NNS_G3dAnmObjSetBlendRatio(NNSG3dAnmObj* pAnmObj, fx32 ratio);
NNS_G3D_INLINE fx32 NNS_G3dAnmObjGetNumFrame(const NNSG3dAnmObj* pAnmObj);


//
// non-inline functions for NNSG3dAnmObj
//
u32 NNS_G3dAnmObjCalcSizeRequired(const void* pAnm, const NNSG3dResMdl* pMdl);
void NNS_G3dAnmObjInit(NNSG3dAnmObj* pAnmObj,
                       void* pResAnm,
                       const NNSG3dResMdl* pResMdl,
                       const NNSG3dResTex* pResTex);
void NNS_G3dAnmObjEnableID(NNSG3dAnmObj* pAnmObj, int id);
void NNS_G3dAnmObjDisableID(NNSG3dAnmObj* pAnmObj, int id);


//
// inline functions for NNSG3dRenderObj
//
NNS_G3D_INLINE void NNS_G3dRenderObjSetFlag(NNSG3dRenderObj* pRenderObj, NNSG3dRenderObjFlag flag);
NNS_G3D_INLINE void NNS_G3dRenderObjResetFlag(NNSG3dRenderObj* pRenderObj, NNSG3dRenderObjFlag flag);
NNS_G3D_INLINE BOOL NNS_G3dRenderObjTestFlag(const NNSG3dRenderObj* pRenderObj, NNSG3dRenderObjFlag flag);
NNS_G3D_INLINE u8* NNS_G3dRenderObjSetUserSbc(NNSG3dRenderObj* pRenderObj, u8* sbc);
NNS_G3D_INLINE void NNS_G3dRenderObjSetJntAnmBuffer(NNSG3dRenderObj* pRenderObj, struct NNSG3dJntAnmResult_* buf);
NNS_G3D_INLINE void NNS_G3dRenderObjSetMatAnmBuffer(NNSG3dRenderObj* pRenderObj, struct NNSG3dMatAnmResult_* buf);

NNS_G3D_INLINE struct NNSG3dJntAnmResult_* NNS_G3dRenderObjReleaseJntAnmBuffer(NNSG3dRenderObj* pRenderObj);
NNS_G3D_INLINE struct NNSG3dMatAnmResult_* NNS_G3dRenderObjReleaseMatAnmBuffer(NNSG3dRenderObj* pRenderObj);
NNS_G3D_INLINE void* NNS_G3dRenderObjSetUserPtr(NNSG3dRenderObj* pRenderObj, void* ptr);

NNS_G3D_INLINE NNSG3dResMdl* NNS_G3dRenderObjGetResMdl(NNSG3dRenderObj* pRenderObj);
NNS_G3D_INLINE void NNS_G3dRenderObjSetBlendFuncMat(NNSG3dRenderObj* pRenderObj, NNSG3dFuncAnmBlendMat func);
NNS_G3D_INLINE void NNS_G3dRenderObjSetBlendFuncJnt(NNSG3dRenderObj* pRenderObj, NNSG3dFuncAnmBlendJnt func);
NNS_G3D_INLINE void NNS_G3dRenderObjSetBlendFuncVis(NNSG3dRenderObj* pRenderObj, NNSG3dFuncAnmBlendVis func);


//
// non-inline functions for NNSG3dRenderObj
//
void NNS_G3dRenderObjInit(NNSG3dRenderObj* pRenderObj, NNSG3dResMdl* pResMdl);
void NNS_G3dRenderObjAddAnmObj(NNSG3dRenderObj* pRenderObj, NNSG3dAnmObj* pAnmObj);
void NNS_G3dRenderObjRemoveAnmObj(NNSG3dRenderObj* pRenderObj, NNSG3dAnmObj* pAnmObj);
void NNS_G3dRenderObjSetCallBack(NNSG3dRenderObj* pRenderObj,
                                 NNSG3dSbcCallBackFunc func,
                                 u8*,
                                 u8 cmd,
                                 NNSG3dSbcCallBackTiming timing);
void NNS_G3dRenderObjResetCallBack(NNSG3dRenderObj* pRenderObj);
void NNS_G3dRenderObjSetInitFunc(NNSG3dRenderObj* pRenderObj, NNSG3dSbcCallBackFunc func);


//
// non-inline functions for Model/Texture binding
//

// Texture Key
u32 NNS_G3dTexGetRequiredSize(const NNSG3dResTex* pTex);
u32 NNS_G3dTex4x4GetRequiredSize(const NNSG3dResTex* pTex);
void NNS_G3dTexSetTexKey(NNSG3dResTex* pTex,
                         NNSG3dTexKey texKey,
                         NNSG3dTexKey tex4x4Key);
void NNS_G3dTexLoad(NNSG3dResTex* pTex, BOOL exec_begin_end);
void NNS_G3dTexReleaseTexKey(NNSG3dResTex* pTex,
                             NNSG3dTexKey* texKey,
                             NNSG3dTexKey* tex4x4Key);

// Palette Key
u32 NNS_G3dPlttGetRequiredSize(const NNSG3dResTex* pTex);
void NNS_G3dPlttSetPlttKey(NNSG3dResTex* pTex, NNSG3dPlttKey plttKey);
void NNS_G3dPlttLoad(NNSG3dResTex* pTex, BOOL exec_begin_end);
NNSG3dPlttKey NNS_G3dPlttReleasePlttKey(NNSG3dResTex* pTex);

// Model -> Texture binding
BOOL NNS_G3dBindMdlTex(NNSG3dResMdl* pMdl, const NNSG3dResTex* pTex);
BOOL NNS_G3dBindMdlTexEx(NNSG3dResMdl* pMdl,
                         const NNSG3dResTex* pTex,
                         const NNSG3dResName* pResName);
BOOL NNS_G3dForceBindMdlTex(NNSG3dResMdl* pMdl,
                            const NNSG3dResTex* pTex,
                            u32 texToMatListIdx,
                            u32 texIdx);

void NNS_G3dReleaseMdlTex(NNSG3dResMdl* pMdl);
BOOL NNS_G3dReleaseMdlTexEx(NNSG3dResMdl* pMdl, const NNSG3dResName* pResName);

// Model -> Palette binding
BOOL NNS_G3dBindMdlPltt(NNSG3dResMdl* pMdl, const NNSG3dResTex* pTex);
BOOL NNS_G3dBindMdlPlttEx(NNSG3dResMdl* pMdl,
                          const NNSG3dResTex* pTex,
                          const NNSG3dResName* pResName);
BOOL NNS_G3dForceBindMdlPltt(NNSG3dResMdl* pMdl,
                             const NNSG3dResTex* pTex,
                             u32 plttToMatListIdx,
                             u32 plttIdx);
void NNS_G3dReleaseMdlPltt(NNSG3dResMdl* pMdl);
BOOL NNS_G3dReleaseMdlPlttEx(NNSG3dResMdl* pMdl, const NNSG3dResName* pResName);

BOOL NNS_G3dBindMdlSet(NNSG3dResMdlSet* pMdlSet, const NNSG3dResTex* pTex);
void NNS_G3dReleaseMdlSet(NNSG3dResMdlSet* pMdlSet);


//
// Misc inline functions
//

#ifdef __cplusplus
}
#endif

#include <nnsys/g3d/kernel_inline.h>

#endif
