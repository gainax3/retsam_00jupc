/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g3d - binres
  File:     res_struct.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.37 $
 *---------------------------------------------------------------------------*/

//
// AUTHOR: Kenji Nishida
//

#ifndef NNSG3D_BINRES_RES_STRUCT_H_
#define NNSG3D_BINRES_RES_STRUCT_H_

#include <nnsys/g3d/config.h>
#define NNS_G3D_RES_INLINE NNS_G3D_INLINE

#ifdef NNS_FROM_TOOL
// VC++
typedef u32 NNSGfdTexKey;
#else
#include <nnsys/gfd.h>
SDK_COMPILER_ASSERT(sizeof(NNSGfdTexKey) == sizeof(u32));
#endif


#ifdef __cplusplus
extern "C" {
#endif


/*---------------------------------------------------------------------------*
    An overview of relation between NNS_G3D_DATABLK_MDLSET structs
    (NNSG3dResMdlSet as a root)
    
    .... -- 1:N relation(referenced by name/index, using NNSG3dResDict)
    ---> -- 1:1 relation(own/refer by offset)

    NNSG3dResMdlSet .... NNSG3dResMdl ---> NNSG3dResMdlInfo
                                      ---> NNSG3dResNodeInfo
                                      ---> Structure Byte Code
                                      ---> NNSG3dResMat
                                      ---> NNSG3dResShp

    NNSG3dResMdlInfo ---> NNSG3dSbcType/NNSG3dScalingRule/NNSG3dTexMtxMode etc.
    
    NNSG3dResNodeInfo .... NNSG3dResNodeData ---> NNSG3dSRTFlag etc.

    NNSG3dResMat .... NNSG3dResMatData

    NNSG3dResShp .... NNSG3dResShpData ---> Display List
 *---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*
    Signatures for G3D's binary files

 *---------------------------------------------------------------------------*/
#define NNS_G3D_SIGNATURE_NSBMD  '0DMB'
#define NNS_G3D_SIGNATURE_NSBTX  '0XTB'
#define NNS_G3D_SIGNATURE_NSBCA  '0ACB'
#define NNS_G3D_SIGNATURE_NSBVA  '0AVB'
#define NNS_G3D_SIGNATURE_NSBMA  '0AMB'
#define NNS_G3D_SIGNATURE_NSBTP  '0PTB'
#define NNS_G3D_SIGNATURE_NSBTA  '0ATB'


/*---------------------------------------------------------------------------*
    Signatures for G3D data blocks

 *---------------------------------------------------------------------------*/
#define NNS_G3D_DATABLK_MDLSET     '0LDM'
#define NNS_G3D_DATABLK_TEX        '0XET'
#define NNS_G3D_DATABLK_JNT_ANM    '0TNJ'
#define NNS_G3D_DATABLK_VIS_ANM    '0SIV'
#define NNS_G3D_DATABLK_MATC_ANM   '0TAM'
#define NNS_G3D_DATABLK_TEXPAT_ANM '0TAP'
#define NNS_G3D_DATABLK_TEXSRT_ANM '0TRS'


/*---------------------------------------------------------------------------*
    マテリアル・シェイプの種別インデックス

    マテリアル・シェイプについてはデータフォーマットが複数存在することが可能なように
    してある。頂点アニメーションを行うため頂点データをインデックス持ちするようなシェイプが
    必要だ、といった場合の拡張を想定している。

    NOTICE:
        関数テーブルへのインデックスになっているので、
        利用可能な値は関数テーブルのサイズによる。
 *---------------------------------------------------------------------------*/
typedef u16 NNSG3dItemTag;
#define NNS_G3D_ITEMTAG_MAT_STANDARD  0
#define NNS_G3D_ITEMTAG_SHP_STANDARD  0

/////////////////////////////////////////////////////////////////////////////////

/*---------------------------------------------------------------------------*
    Name:        NNSG3dResFileHeader

    Description: A header of a resouce file

                 signature  = (sigVal = NNS_G3D_SIGNATURE_XXXX)
                 byteOrder  = 0xfeff
                 version    = described in u8.u8 style
                 fileSize   = size of a data file
                 headerSize = always 16
                 dataBlocks = the number of blocks
                              (the number of datablock header)
                 Following 'dataBlocks' words are the indices to the blocks.
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResFileHeader_
{
    union 
    {
        char signature[4];
        u32  sigVal;
    };
    u16  byteOrder;
    u16  version;
    u32  fileSize;
    u16  headerSize;
    u16  dataBlocks;
}
NNSG3dResFileHeader;


/*---------------------------------------------------------------------------*
   Name:        NNSG3dResDataBlockHeader:

   Description: A header of a data block
                
                kind = the kind of a data block
                size = size of a data block
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResDataBlockHeader_
{
    union
    {
        u32 kind;
        char chr[4];
    };
    u32 size;
}
NNSG3dResDataBlockHeader;


/////////////////////////////////////////////////////////////////////////////////

/*---------------------------------------------------------------------------*
   Name:        NNSG3dResDictTreeNode:

   Description: A node of a patricia tree

                refBit   = an index to a referred bit(0-127)
                idxLeft  = go left if the bit is zero
                idxRight = go right if the bit is one
                idxEntry = an index to a data entry
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResDictTreeNode_
{
    u8  refBit;
    u8  idxLeft;
    u8  idxRight;
    u8  idxEntry;
}
NNSG3dResDictTreeNode;


/*---------------------------------------------------------------------------*
   Name:        NNSG3dResDict:

   Description: 名前検索用の辞書ヘッダ
                インデックスは&NNSG3dResDictからのオフセット
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResDict_
{
    u8  revision;      // revision of dictionary
    u8  numEntry;      // num of entry(not that of tree node)
    u16 sizeDictBlk;   // size of this block
    u16 dummy_;
    u16 ofsEntry;      // (NNSG3dResDictEntryHeader*)   ((u8*)this + idxEntry)

    // patricia tree and its data entry
    NNSG3dResDictTreeNode node[1];  // variable size
}
NNSG3dResDict;


/*---------------------------------------------------------------------------*
   Name:        NNSG3dResName

   Description: A structure for a resouce name
                '\0' must be filled if the length of string is less than 16.
 *---------------------------------------------------------------------------*/
#define NNS_G3D_RESNAME_SIZE    (16)
#define NNS_G3D_RESNAME_VALSIZE (NNS_G3D_RESNAME_SIZE / sizeof(u32))

typedef union NNSG3dResName_
{
    char name[NNS_G3D_RESNAME_SIZE];
    u32  val[NNS_G3D_RESNAME_VALSIZE];
}
NNSG3dResName;


/*---------------------------------------------------------------------------*
   Name:        NNSG3dResDictEntryHeader

   Description: 名前検索用の辞書エントリヘッダ
                名前の後にsizeUnit-16バイトのデータがある
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResDictEntryHeader_
{
    u16 sizeUnit;      // データエントリ1つ分のサイズ（バイト）
    u16 ofsName;       // 
    u8  data[4];       // NNSG3dResDictEntryItemの並び(1つあたりの大きさはsizeUnit)
}
NNSG3dResDictEntryHeader;


/////////////////////////////////////////////////////////////////////////////////
//
//
// Texture Image
//
//


/*---------------------------------------------------------------------------*
    NNSG3dTexImageParam

    テクスチャブロック内のテクスチャイメージ情報
    モデルのマテリアルと関連付けを行うときにここの情報がコピーされる
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_TEXIMAGE_PARAM_TEX_ADDR_MASK  = 0x0000ffff,
    NNS_G3D_TEXIMAGE_PARAM_S_SIZE_MASK    = 0x00700000,
    NNS_G3D_TEXIMAGE_PARAM_T_SIZE_MASK    = 0x03800000,
    NNS_G3D_TEXIMAGE_PARAM_TEXFMT_MASK    = 0x1c000000,
    NNS_G3D_TEXIMAGE_PARAM_TR_MASK        = 0x20000000,

    NNS_G3D_TEXIMAGE_PARAM_TEX_ADDR_SHIFT = 0,
    NNS_G3D_TEXIMAGE_PARAM_S_SIZE_SHIFT   = 20,
    NNS_G3D_TEXIMAGE_PARAM_T_SIZE_SHIFT   = 23,
    NNS_G3D_TEXIMAGE_PARAM_TEXFMT_SHIFT   = 26,
    NNS_G3D_TEXIMAGE_PARAM_TR_SHIFT       = 29
}
NNSG3dTexImageParam;


/*---------------------------------------------------------------------------*
    NNSG3dTexImageParamEx

    ORIGWとORIGHはテクスチャ行列の計算において必要
    NNSG3dTexImageParamの値と同じであれば計算を高速化できるので、
    フラグ(WHSAME)も持っておく
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_TEXIMAGE_PARAMEX_ORIGW_MASK  = 0x000007ff,
    NNS_G3D_TEXIMAGE_PARAMEX_ORIGH_MASK  = 0x003ff800,
    NNS_G3D_TEXIMAGE_PARAMEX_WHSAME_MASK = 0x80000000,

    NNS_G3D_TEXIMAGE_PARAMEX_ORIGW_SHIFT  = 0,
    NNS_G3D_TEXIMAGE_PARAMEX_ORIGH_SHIFT  = 11,
    NNS_G3D_TEXIMAGE_PARAMEX_WHSAME_SHIFT = 31
}
NNSG3dTexImageParamEx;


/*---------------------------------------------------------------------------*
    NNSG3dResDictTexData

    テクスチャ名辞書のデータエントリ
    通常、辞書のデータには可変長データへのオフセットが入っていることが多いが、
    固定長の場合は辞書のエントリとして入れ込むことができる。
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResDictTexData_
{
    // 31 30 29     28  26 25 23 22 20 19   16 15                       0
    //       Pltt0  TexFmt TSize SSize         3bit右シフトされたオフセット
    u32           texImageParam;  // NNSG3dTexImageParam

    // 31      30           22 21     11 10     0
    // SameBit                   OrigH     OrigW
    u32           extraParam;     // NNSG3dTexImageParamEx
}
NNSG3dResDictTexData;


/*---------------------------------------------------------------------------*
    NNSG3dResTexFlag

    テクスチャイメージ(4x4フォーマット以外)をテクスチャイメージスロットにロードしたときに、
    NNS_G3D_RESTEX_LOADEDがセットされる。アンロードしたときにはリセットされる。
    NNSG3dResTexInfo::flag用の値である。
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_RESTEX_LOADED   = 0x0001
}
NNSG3dResTexFlag;


/*---------------------------------------------------------------------------*
    NNSG3dResTexInfo

    NNSG3dResTexが所有する構造体。
    4x4COMPフォーマット以外のテクスチャイメージに関する情報を保持している。
    nsbmd/nsbtxファイル毎にテクスチャイメージデータは１つにまとめられていて、
    まとまりごとにロード／アンロードするようになっている。
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResTexInfo_
{
    NNSGfdTexKey vramKey;         // gfdライブラリ仕様のVRAMキーを格納する。
    u16          sizeTex;         // テクスチャデータのサイズを右に3bitシフトしたものが予め入っている
    u16          ofsDict;
    u16          flag;            // NNSG3dResTexFlag
    u16          dummy_;
    u32          ofsTex;          // テクスチャデータへのオフセット(&NNSG3dResTexImageからのオフセット)
}
NNSG3dResTexInfo;


/*---------------------------------------------------------------------------*
    NNSG3dResTex4x4Flag

    4x4COMPフォーマットのテクスチャイメージをテクスチャイメージスロットにロードしたときに、
    NNS_G3D_RESTEX4x4_LOADEDがセットされる。アンロードしたときにはリセットされる。
    NNSG3dResTex4x4Info::flag用の値である。
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_RESTEX4x4_LOADED   = 0x0001   // VRAMにロードされたかどうか
}
NNSG3dResTex4x4Flag;


/*---------------------------------------------------------------------------*
    NNSG3dResTex4x4Info:

    NNSG3dResTexが所有する構造体。
    4x4COMPフォーマットのテクスチャイメージに関する情報を保持している。
    nsbmd/nsbtxファイル毎にテクスチャイメージデータは１つにまとめられていて、
    まとまりごとにロード／アンロードするようになっている。   
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResTex4x4Info_
{
    NNSGfdTexKey vramKey;         // gfdライブラリ仕様のVRAMキーを格納する。
    u16          sizeTex;         // テクスチャデータのサイズを右に3bitシフトしたもの(パレットインデックスの場合は2bit)
    u16          ofsDict;
    u16          flag;            // 0ビット目はテクスチャがVRAMにロードされたかどうかのフラグ
    u16          dummy_;
    u32          ofsTex;          // テクスチャデータへのオフセット(&NNSG3dResTexImageからのバイト)
    u32          ofsTexPlttIdx;   // テクスチャパレットインデックスデータへのオフセット(&NNSG3dResTexImageからのバイト)
}
NNSG3dResTex4x4Info;


/*---------------------------------------------------------------------------*
    NNSG3dResDictPlttData

    パレット名辞書のデータエントリ
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResDictPlttData_
{
    u16 offset;          // パレットデータの起点に対するオフセットを3bit右シフトしたもの
    u16 flag;            // 0ビット目がTex4Plttかどうかを示すフラグ
}
NNSG3dResDictPlttData;


/*---------------------------------------------------------------------------*
    NNSG3dResPlttFlag

    テクスチャパレットをテクスチャパレットスロットにロードしたときに
    NNS_G3D_RESPLTT_LOADEDがセットされる。アンロードしたときにはリセットされる。
    NNS_G3D_RESPLTT_USEPLTT4は、パレットデータに4色カラーパレットが含まれている
    場合にセットされている。4色カラーパレットのロード先には制限があるからである。
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_RESPLTT_LOADED   = 0x0001,   // VRAMにロードされたかどうか
    NNS_G3D_RESPLTT_USEPLTT4 = 0x8000    // ブロック内に4色パレットが入っているかどうか
}
NNSG3dResPlttFlag;


/*---------------------------------------------------------------------------*
    NNSG3dResPlttInfo

    NNSG3dResTexが所有する構造体。
    テクスチャパレットに関する情報を保持している。
    nsbmd/nsbtxファイル毎にテクスチャパレットデータは１つにまとめられていて、
    まとまりごとにロード／アンロードするようになっている。
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResPlttInfo_
{
    NNSGfdTexKey vramKey;      // gfdライブラリ仕様のVRAMキーを格納する。
    u16          sizePltt;     // テクスチャパレットのデータサイズを右に3bitシフトしたものが入る
    u16          flag;         // NNSG3dResPlttFlag
    u16          ofsDict;
    u16          dummy_;
    u32          ofsPlttData;
}
NNSG3dResPlttInfo;


/*---------------------------------------------------------------------------*
    NNSG3dResTex
   
    格納されている各種オフセットは、VRAM内のスロットに対するオフセットデータ以外は、
    &NNSG3dResTexをベースにしている。
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResTex_
{
    NNSG3dResDataBlockHeader header;
    NNSG3dResTexInfo         texInfo;
    NNSG3dResTex4x4Info      tex4x4Info;
    NNSG3dResPlttInfo        plttInfo;
    NNSG3dResDict            dict;       // テクスチャ用辞書
    // パレット用辞書
    // テクスチャデータ(4x4以外)
    // テクスチャデータ(4x4)
    // テクスチャパレットインデックスデータ
    // テクスチャパレットデータ
    // と続く
}
NNSG3dResTex;


/////////////////////////////////////////////////////////////////////////////////
//
//
// マテリアル
//
//

/*---------------------------------------------------------------------------*
    NNSG3dResDictMatData

    マテリアル名辞書データ。
    マテリアルデータは可変長なのでNNSG3dResMatData(他)へのオフセットを所持している。
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResDictMatData_
{
    u32           offset;       // &NNSG3dResMatからのオフセットデータ
}
NNSG3dResDictMatData;


/*---------------------------------------------------------------------------*
    NNSG3dResDictTexToMatIdxData

    テクスチャ名->マテリアルインデックス列辞書データ
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResDictTexToMatIdxData_
{
    u16           offset;       // NNSG3dResMatからのオフセット
    u8            numIdx;       // テクスチャ名に対応するマテリアルＩＤの数
    u8            flag;         // テクスチャ<->マテリアルのバインドがされたかどうかのフラグ
}
NNSG3dResDictTexToMatIdxData;


/*---------------------------------------------------------------------------*
    NNSG3dResDictPlttToMatIdxData

    パレット名->マテリアルインデックス列辞書データ
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResDictPlttToMatIdxData_
{
    u16           offset;       // NNSG3dResMatからのオフセット
    u8            numIdx;       // パレット名に対応するマテリアルＩＤの数
    u8            flag;         // パレット<->マテリアルのバインドがされたかどうかのフラグ
}
NNSG3dResDictPlttToMatIdxData;


/*---------------------------------------------------------------------------*
    NNSG3dMatFlag

    マテリアルに関するフラグ情報
    ビットがセットされていれば、Xを使用する、といったフラグになっている。
    初期化時に、テクスチャデータと関連付けるときに操作されるフラグもある。
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_MATFLAG_TEXMTX_USE       = 0x0001, // テクスチャ行列を使用するかどうか
    NNS_G3D_MATFLAG_TEXMTX_SCALEONE  = 0x0002, // スケールが1.0ならON(テクスチャ行列使用時)
    NNS_G3D_MATFLAG_TEXMTX_ROTZERO   = 0x0004, // 回転しないならON(テクスチャ行列使用時)
    NNS_G3D_MATFLAG_TEXMTX_TRANSZERO = 0x0008, // 平行移動しないならON(テクスチャ行列使用時)
    NNS_G3D_MATFLAG_ORIGWH_SAME      = 0x0010, // テクスチャのWidth/Heightがシステムと同じ場合セットされる
    NNS_G3D_MATFLAG_WIREFRAME        = 0x0020, // ワイヤーフレーム表示ならON
    NNS_G3D_MATFLAG_DIFFUSE          = 0x0040, // マテリアルでdiffuseを指定するならON
    NNS_G3D_MATFLAG_AMBIENT          = 0x0080, // マテリアルでambientを指定するならON
    NNS_G3D_MATFLAG_VTXCOLOR         = 0x0100, // マテリアルでvtxcolorフラグを指定するならON
    NNS_G3D_MATFLAG_SPECULAR         = 0x0200, // マテリアルでspecularを指定するならON
    NNS_G3D_MATFLAG_EMISSION         = 0x0400, // マテリアルでemissionを指定するならON
    NNS_G3D_MATFLAG_SHININESS        = 0x0800, // マテリアルでshininessフラグを指定するならON
    NNS_G3D_MATFLAG_TEXPLTTBASE      = 0x1000, // テクスチャパレットベースアドレスを指定するならON
    NNS_G3D_MATFLAG_EFFECTMTX        = 0x2000  // effect_mtxが存在するならON
}
NNSG3dMatFlag;


/*---------------------------------------------------------------------------*
    NNSG3dResMatData

    個々のマテリアルのデータ。
    データを拡張する場合は、itemTagの値を変えて、構造体の後方にデータを配置する
    ようにする。
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResMatData_
{
    NNSG3dItemTag itemTag;                // マテリアル種別認識タグ(この構造体の場合はNNS_G3D_ITEMTAG_MAT_STANDARDである必要がある)
    u16           size;                   // サイズ
    u32           diffAmb;                // MaterialColor0コマンドへのパラメタ
    u32           specEmi;                // MaterialColor1コマンドへのパラメタ
    u32           polyAttr;               // PolygonAttrコマンドへのパラメタ
    u32           polyAttrMask;           // マテリアルで指定するビットは1になっている
    u32           texImageParam;          // TexImageParamコマンドへのパラメタ
    u32           texImageParamMask;      // マテリアルで指定するビットは1になっている
    u16           texPlttBase;            // TexPlttBaseコマンドへのパラメタ
    u16           flag;                   // NNSG3dMatFlagへキャスト
    u16           origWidth, origHeight;  // 初期化時にテクスチャイメージから転送される
    fx32          magW;                   // バインドされたテクスチャの幅/origWidth
    fx32          magH;                   // バインドされたテクスチャの高さ/origHeight
//    fx32 scaleS, scaleT;                // exists if NNS_G3D_MATFLAG_TEXMTX_SCALEONE is off
//    fx16 rotSin, rotCos;                // exists if NNS_G3D_MATFLAG_TEXMTX_ROTZERO is off
//    fx32 transS, transT;                // exists if NNS_G3D_MATFLAG_TEXMTX_TRANSZERO is off
//    fx32 effectMtx[16];                 // exists if NNS_G3D_MATFLAG_ENV_EFFECTMTX is on
}
NNSG3dResMatData;


/*---------------------------------------------------------------------------*
    NNSG3dResMat

    １つのモデルが持つマテリアルの集合を束ねる構造体
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResMat_
{
    u16              ofsDictTexToMatList;  // テクスチャ->マテリアルID列辞書へのオフセット
    u16              ofsDictPlttToMatList; // パレット->マテリアルID列辞書へのオフセット
    NNSG3dResDict    dict;                 // NNSG3dResDictMatDataへの辞書
//  NNSG3dResDict    dictTexToMatList;     // (u8*)this + idxDictTexToMatList
//  NNSG3dResDict    dictPlttToMatList;    // (u8*)this + idxDictPlttToMatList
//  u8[]             matIdxData;           // (u8*)this + idxMatIdxData
//  seq of NNSG3dResMatData matData        // (u8*)this + idxMatData
}
NNSG3dResMat;


/////////////////////////////////////////////////////////////////////////////////
//
//
// シェイプ
//
//

/*---------------------------------------------------------------------------*
    NNSG3dResDictShpData

    シェイプ名辞書データ。
    シェイプデータは可変長なのでNNSG3dResShpData(他)へのオフセットを所持している
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResDictShpData_
{
    u32           offset;  // (u8*)&NNSG3DResShp + offset -> NNSG3dResShpData&
}
NNSG3dResDictShpData;


/*---------------------------------------------------------------------------*
    NNSG3dShpFlag

    シェイプのディスプレイリストの特色を表すフラグです。
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_SHPFLAG_USE_NORMAL     = 0x00000001,    // DL内にNormalコマンドが存在します。
    NNS_G3D_SHPFLAG_USE_COLOR      = 0x00000002,    // DL内にColorコマンドが存在します。
    NNS_G3D_SHPFLAG_USE_TEXCOORD   = 0x00000004,    // DL内にTexCoordコマンドが存在します。
    NNS_G3D_SHPFLAG_USE_RESTOREMTX = 0x00000008     // DL内にRestoreMtxコマンドが存在します。
}
NNSG3dShpFlag;


/*---------------------------------------------------------------------------*
    NNSG3dResShpData

    個々のシェイプデータ。
    異なるデータ構造でシェイプを記述する場合でも、itemTagとsizeだけは所持する必要がある。
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResShpData_
{
    NNSG3dItemTag itemTag;          // シェイプ種別認識タグ(この構造体の場合はNNS_G3D_ITEMTAG_SHP_STANDARDである必要がある)
    u16           size;             // サイズ
    u32           flag;             // NNSG3dShpFlag
    u32           ofsDL;            // ディスプレイリストへのオフセット
    u32           sizeDL;           // ディスプレイリストのサイズ
}
NNSG3dResShpData;


/*---------------------------------------------------------------------------*
    NNSG3dResShp

    １つのモデルが持つシェイプの集合を束ねる構造体
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResShp_
{
    NNSG3dResDict dict;             // NNSG3dResDictShpDataへの辞書
    // NNSG3dResShpData[] shpData;
    // u32[]              DL;
}
NNSG3dResShp;


typedef struct NNSG3dResEvpMtx_
{
    MtxFx43 invM;
    MtxFx33 invN;
}
NNSG3dResEvpMtx;


/////////////////////////////////////////////////////////////////////////////////
//
//
// ModelInfo
//
//


/*---------------------------------------------------------------------------*
    NNSG3dSbcType

    モデルが所持するStructure Byte Codeのタイプを指定する
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_SBCTYPE_NORMAL       = 0        // 現在のところ単一のタイプしかない
}
NNSG3dSbcType;


/*---------------------------------------------------------------------------*
    NNSG3dScalingRule

    モデルのスケーリングルールを指定する
    中間ファイルフォーマットの<model_info>::scaling_ruleに対応する。
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_SCALINGRULE_STANDARD = 0,   // 一般的な行列計算でよい
    NNS_G3D_SCALINGRULE_MAYA     = 1,   // MayaのSegment Scaling Compensateの考慮を行わなければならないノードが存在する
    NNS_G3D_SCALINGRULE_SI3D     = 2    // Softimage3Dのスケーリング計算を行わなくてはならない
}
NNSG3dScalingRule;


/*---------------------------------------------------------------------------*
    NNSG3dTexMtxMode

    テクスチャ行列の計算方法を指定する
    中間ファイルフォーマットの<model_info>::tex_mtx_modeに対応する。
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_TEXMTXMODE_MAYA      = 0x00, // Mayaの計算方法を適用する。
    NNS_G3D_TEXMTXMODE_SI3D      = 0x01, // Softimage3Dの計算方法を適用する。
    NNS_G3D_TEXMTXMODE_3DSMAX    = 0x02, // 3dsMaxの計算方法を適用する。
    NNS_G3D_TEXMTXMODE_XSI       = 0x03  // XSIの計算方法を適用する。
}
NNSG3dTexMtxMode;


/*---------------------------------------------------------------------------*
   Name:        NNSG3dResMdlInfo

   Description: モデルの付随情報
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResMdlInfo_
{
    u8         sbcType;       // Structure Byte Codeタイプ識別子(NNSG3dSbcType)
    u8         scalingRule;   // スケーリングルール識別子(NNSG3dScalingRule)
    u8         texMtxMode;    // テクスチャ行列計算方式識別子(NNSG3dTexMtxMode)
    u8         numNode;

    u8         numMat;
    u8         numShp;
    u8         firstUnusedMtxStackID;
    u8         dummy_;

    // 頂点位置座標にかけるスケール値とその逆数
    // FX32_ONE以外だった場合、ジオメトリエンジンのスケールコマンドを利用して
    // 頂点位置座標を加工する。シェイプのディスプレイリスト内で行列がリストアされている
    // (エンベロープ利用)場合、リストア後、posScaleでスケーリングするようになっている
    fx32       posScale;      
    fx32       invPosScale;

    u16        numVertex;    // IMDファイル<output_info>::vertex_sizeの値
    u16        numPolygon;   // IMDファイル<output_info>::polygon_sizeの値
    u16        numTriangle;  // IMDファイル<output_info>::triangle_sizeの値
    u16        numQuad;      // IMDファイル<output_info>::quad_sizeの値

    fx16       boxX, boxY, boxZ; // boxPosScaleでかけると実際の座標になる
    fx16       boxW, boxH, boxD; // boxPosScaleでかけると実際の座標になる

    fx32       boxPosScale;
    fx32       boxInvPosScale;
}
NNSG3dResMdlInfo;


/////////////////////////////////////////////////////////////////////////////////
//
//
// ノード
//
//

/*---------------------------------------------------------------------------*
    NNSG3dResDictNodeData

    ノード名辞書データ。
    NNSG3dResNodeDataへのオフセットを所持している。
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResDictNodeData_
{
    u32       offset;       // &NNSG3dResNodeInfoからのオフセットデータ
}
NNSG3dResDictNodeData;


/*---------------------------------------------------------------------------*
    NNSG3dSRTFlag

    ノードのSRTデータに関するフラグ情報
    このフラグによって後続にどのようなデータが存在するかが決定する。
    データ量の削減と余分な計算の省略を目的としている。
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_SRTFLAG_TRANS_ZERO       = 0x0001, // Trans=(0,0,0), 移動データの出力が省略される。
    NNS_G3D_SRTFLAG_ROT_ZERO         = 0x0002, // Rot=Identity, 回転行列データの出力が省略される。
    NNS_G3D_SRTFLAG_SCALE_ONE        = 0x0004, // Scale=(1,1,1), スケールデータの出力が省略される。
    NNS_G3D_SRTFLAG_PIVOT_EXIST      = 0x0008, // 回転行列内に+1か-1の要素(Pivot)が存在する。
    NNS_G3D_SRTFLAG_IDXPIVOT_MASK    = 0x00f0, // +1,-1要素(Pivot)の場所
    NNS_G3D_SRTFLAG_PIVOT_MINUS      = 0x0100, // ONならばPivotは-1, OFFならば+1
    NNS_G3D_SRTFLAG_SIGN_REVC        = 0x0200, // セットされていればCはBの反対の符号
    NNS_G3D_SRTFLAG_SIGN_REVD        = 0x0400, // セットされていればDはAの反対の符号
    NNS_G3D_SRTFLAG_IDXMTXSTACK_MASK = 0xf800, // 0-30だとスタックにある,31だとスタックにない

    NNS_G3D_SRTFLAG_IDENTITY         = NNS_G3D_SRTFLAG_TRANS_ZERO |
                                       NNS_G3D_SRTFLAG_ROT_ZERO   |
                                       NNS_G3D_SRTFLAG_SCALE_ONE,

    NNS_G3D_SRTFLAG_IDXPIVOT_SHIFT    = 4,
    NNS_G3D_SRTFLAG_IDXMTXSTACK_SHIFT = 11
}
NNSG3dSRTFlag;


/*---------------------------------------------------------------------------*
    NNSG3dResNodeData

    flag(NNSG3dSRTFlag)の値によって後続にどのようなデータがくるかが変化する。
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResNodeData_
{
    u16 flag;       // NNSG3dSRTFlag
    fx16 _00;
    // exists if !NNS_G3D_SRT_FLAG_TRANS_ZERO
    //      fx32 Tx, Ty, Tz;
    // exists if !NNS_G3D_SRT_FLAG_ROT_ZERO && !NNS_G3D_SRT_FLAG_PIVOT_EXIST
    //      fx16 _01, _02;
    //      fx16 _10, _11;
    //      fx16 _12, _20;
    //      fx16 _21, _22;
    // exists if !NNS_G3D_SRT_FLAG_ROT_ZERO && NNS_G3D_SRT_FLAG_PIVOT_EXIST
    //      fx16 A, B;
    // exists if !NNS_G3D_SCALE_ONE
    //      fx32 Sx, Sy, Sz;
    //      fx32 InvSx, InvSy, InvSz;
}
NNSG3dResNodeData;


/*---------------------------------------------------------------------------*
    NNSG3dResNodeInfo

    １つのモデルが持つノードＳＲＴ情報の集合を束ねる構造体
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResNodeInfo_
{
    NNSG3dResDict  dict;
    // a seq of NNSG3dResNodeSRTInfo
}
NNSG3dResNodeInfo;


/////////////////////////////////////////////////////////////////////////////////
//
//
// モデル
//
//

/*---------------------------------------------------------------------------*
    NNSG3dResMdl

    １つのモデルに関する（テクスチャ以外の）全ての情報を束ねる構造体 
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResMdl_
{
    u32                size;        // モデルのサイズ
    u32                ofsSbc;      // &NNSG3dResMdlを起点としたSbcへのオフセット
    u32                ofsMat;      // &NNSG3dResMdlを起点としたNNSG3dResMatへのオフセット
    u32                ofsShp;      // &NNSG3dResMdlを起点としたNNSG3dResShpへのオフセット
    u32                ofsEvpMtx;   // &NNSG3dResMdlを起点としたNNSG3dEvpMtxへのオフセット
    NNSG3dResMdlInfo   info;        // モデル情報(固定長)
    NNSG3dResNodeInfo  nodeInfo;    // ノード情報
    // SBC
    // Materials
    // Shapes
    // EvpMtxs
}
NNSG3dResMdl;


/////////////////////////////////////////////////////////////////////////////////
//
//
// モデルセット
//
//

/*---------------------------------------------------------------------------*
    NNSG3dResDictMdlSetData

    個々のモデルへのオフセット(&NNSG3dResMdlSetを起点)を格納している
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResDictMdlSetData_
{
    u32 offset;
}
NNSG3dResDictMdlSetData;


/*---------------------------------------------------------------------------*
    NNSG3dResMdlSet

    モデルセットブロックの構造体。データブロックヘッダを含む構造体であることに注意
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResMdlSet_
{
    NNSG3dResDataBlockHeader header;
    NNSG3dResDict            dict;
    // a seq of NNSG3dResMdl follows
}
NNSG3dResMdlSet;



/////////////////////////////////////////////////////////////////////////////////
//
//
// Structure Byte Code(Sbc)
//
//

/*---------------------------------------------------------------------------*
    SBC命令セット

 *---------------------------------------------------------------------------*/
#define NNS_G3D_SBC_NOP           0x00     // NOP
#define NNS_G3D_SBC_RET           0x01     // RET
#define NNS_G3D_SBC_NODE          0x02     // NODE      idxNode, visibility
#define NNS_G3D_SBC_MTX           0x03     // MTX       stackIdx
#define NNS_G3D_SBC_MAT           0x04     // MAT       idxMat
#define NNS_G3D_SBC_SHP           0x05     // SHP       idxShp
#define NNS_G3D_SBC_NODEDESC      0x06     // NODEDESC  idxNode, idxMtxDest, idxMtxParent
#define NNS_G3D_SBC_BB            0x07
#define NNS_G3D_SBC_BBY           0x08
#define NNS_G3D_SBC_NODEMIX       0x09     // NODEMIX   idxNode, idxMtxDest, numMtx, idxMtx1, ratio1, ..., idxMtxN, ratioN
#define NNS_G3D_SBC_CALLDL        0x0a     // CALLDL    u32(relative), u32(size)
#define NNS_G3D_SBC_POSSCALE      0x0b     // POSSCALE
#define NNS_G3D_SBC_ENVMAP        0x0c
#define NNS_G3D_SBC_PRJMAP        0x0d


#define NNS_G3D_SBC_NOCMD         0x1f     // not exist in compiled structure byte code mnemonic

#define NNS_G3D_SBC_COMMAND_NUM   0x20

#define NNS_G3D_SBCFLG_000        0x00
#define NNS_G3D_SBCFLG_001        0x20
#define NNS_G3D_SBCFLG_010        0x40
#define NNS_G3D_SBCFLG_011        0x60
#define NNS_G3D_SBCFLG_100        0x80
#define NNS_G3D_SBCFLG_101        0xa0
#define NNS_G3D_SBCFLG_110        0xc0
#define NNS_G3D_SBCFLG_111        0xe0

#define NNS_G3D_SBCCMD_MASK       0x1f
#define NNS_G3D_SBCFLG_MASK       0xe0


typedef enum
{
    NNS_G3D_SBC_NODEDESC_FLAG_MAYASSC_APPLY  = 0x01,
    NNS_G3D_SBC_NODEDESC_FLAG_MAYASSC_PARENT = 0x02
}
NNSG3dSbcNodeDescFlag;

#define NNS_G3D_GET_SBCCMD(x) ((x) & NNS_G3D_SBCCMD_MASK)
#define NNS_G3D_GET_SBCFLG(x) ((u32)((x) & NNS_G3D_SBCFLG_MASK))


/////////////////////////////////////////////////////////////////////////////////
//
//
// アニメーション
//
//

//
// REQUIREMENT:
// アニメーションファイルのブロックは１つで、NNSG3dResAnmSetからディクショナリから、
// NNSG3dResDictAnmSetDataを通してNNSG3dResAnmHeaderを先頭に持つ各アニメーションの
// 構造体に達することができなくてはならない。
//

/*---------------------------------------------------------------------------*
    NNSG3dResAnmHeader
    
    各アニメーションリソースの先頭にくるヘッダ情報
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResAnmHeader_
{
    // 'M' for material animation
    // 'J' for joint animation
    // 'V' for visibility animation
    u8  category0;
    u8  revision;
    // 'CA' for character(joint) animation
    // 'VA' for visibility animation
    // 'MA' for material color animation
    // 'TP' for texture pattern animation
    // 'TA' for texture SRT animation
    u16  category1; 
}
NNSG3dResAnmHeader;

/*---------------------------------------------------------------------------*
    NNSG3dResAnmCommon

    全てのアニメーションはヘッダとnumFrameを持たなくてはならない。
 *---------------------------------------------------------------------------*/
typedef struct
{
    NNSG3dResAnmHeader anmHeader;
    u16                numFrame;
    u16                dummy_;
}
NNSG3dResAnmCommon;


/*---------------------------------------------------------------------------*
    NNSG3dResDictAnmSetData

    (u8*)&NNSG3dResAnmSet+offsetで個々のアニメーションに達することができる。
 *---------------------------------------------------------------------------*/
typedef struct
{
    u32 offset;                      // 個々のアニメーションに達するためのオフセットが格納されている。
}
NNSG3dResDictAnmSetData;


/*---------------------------------------------------------------------------*
    NNSG3dResAnmSet

    アニメーションの集合を束ねるための構造体。
 *---------------------------------------------------------------------------*/
typedef struct
{
    NNSG3dResDataBlockHeader header;
    NNSG3dResDict            dict;   // ファイル名->各AnmSetData
}
NNSG3dResAnmSet;



/////////////////////////////////////////////////////////////////////////////////
//
//
// ビジビリティアニメーション
//
//

/*---------------------------------------------------------------------------*
    NNSG3dResVisAnm

    ビジビリティアニメーション1つ分のデータを束ねる構造体
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResVisAnm_
{
    NNSG3dResAnmHeader anmHeader;
    u16                numFrame;
    u16                numNode;
    u16                size;
    u16                dummy_;
    // variable length
    u32                visData[1];
}
NNSG3dResVisAnm;

/*---------------------------------------------------------------------------*
    NNSG3dResDictVisAnmSetData

    個々のビジビリティアニメーション(NNSG3dResVisAnm)のデータへのオフセットが格納されている。
    &NNSG3dResVisAnmSetを起点とする。
 *---------------------------------------------------------------------------*/
typedef NNSG3dResDictAnmSetData NNSG3dResDictVisAnmSetData;

/*---------------------------------------------------------------------------*
    NNSG3dResVisAnmSet

    ビジビリティアニメーションのセットを束ねる構造体
    後方にはNNSG3dResVisAnmが並んでいる。
    辞書はNNSG3dResDictVisAnmSetData経由でNNSG3dResVisAnmを参照する辞書
    kind==NNS_G3D_SIGNATURE_NSBVAでなくてはならない。
 *---------------------------------------------------------------------------*/
typedef NNSG3dResAnmSet NNSG3dResVisAnmSet;


/////////////////////////////////////////////////////////////////////////////////
//
//
// マテリアルカラーアニメーション
//
//

/*---------------------------------------------------------------------------*
    NNSG3dMatCElem

    NNSG3dResDictMatCAnmDataの各データメンバの値
    NNS_G3D_MATCANM_ELEM_CONSTがセットされていれば、下位16bitは定数カラー値、
    セットされていなければ、カラーデータ列へのオフセット、となる。
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_MATCANM_ELEM_CONST                 = 0x20000000, // データがアニメーションしない場合にON
    NNS_G3D_MATCANM_ELEM_STEP_1                = 0x00000000, // データが毎フレームあるとき
    NNS_G3D_MATCANM_ELEM_STEP_2                = 0x40000000, // データが2フレーム毎にあるとき
    NNS_G3D_MATCANM_ELEM_STEP_4                = 0x80000000, // データが4フレーム毎にあるとき

    NNS_G3D_MATCANM_ELEM_STEP_MASK             = 0xc0000000,
    NNS_G3D_MATCANM_ELEM_LAST_INTERP_MASK      = 0x1fff0000,
    NNS_G3D_MATCANM_ELEM_OFFSET_CONSTANT_MASK  = 0x0000ffff,

    NNS_G3D_MATCANM_ELEM_OFFSET_CONSTANT_SHIFT = 0,
    NNS_G3D_MATCANM_ELEM_LAST_INTERP_SHIFT     = 16
}
NNSG3dMatCElem;


/*---------------------------------------------------------------------------*
    NNSG3dResDictMatCAnmData

    マテリアルカラーアニメーションの各要素についての情報
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResDictMatCAnmData_
{
    // NNSG3dMatCElem:
    // 31  30 29      28         16 15               0
    // frmStp isConst iFrame_interp offset/constantCol
    u32 diffuse;
    u32 ambient;
    u32 specular;
    u32 emission;
    u32 polygon_alpha;
}
NNSG3dResDictMatCAnmData;


/*---------------------------------------------------------------------------*
    NNSG3dMatCAnmOption

    マテリアルカラーアニメーションのオプション(中間ファイルで指定)
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_MATCANM_OPTION_INTERPOLATION              = 0x0001, // <mat_color_info>::interpolationに対応
    NNS_G3D_MATCANM_OPTION_END_TO_START_INTERPOLATION = 0x0002  // <mat_color_info>::interp_end_to_startに対応
}
NNSG3dMatCAnmOption;


/*---------------------------------------------------------------------------*
    NNSG3dResMatCAnm

    マテリアルカラーアニメーション1つ分のデータを束ねる構造体
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResMatCAnm_
{
    NNSG3dResAnmHeader anmHeader;
    u16                numFrame;
    u16                flag;     // NNSG3dMatCAnmOption
    NNSG3dResDict      dict;
}
NNSG3dResMatCAnm;


/*---------------------------------------------------------------------------*
    NNSG3dResDictMatCAnmSetData

    個々のマテリアルカラーアニメーションのデータへのオフセットが格納されている。
    &NNSG3dResMatCAnmSetを起点とする。
 *---------------------------------------------------------------------------*/
typedef NNSG3dResDictAnmSetData NNSG3dResDictMatCAnmSetData;


/*---------------------------------------------------------------------------*
    NNSG3dResMatCAnmSet

    マテリアルカラーアニメーションのセットを束ねる構造体
    後方にはNNSG3dResMatCAnmが並んでいる。
    辞書はNNSG3dResDictMatCAnmSetData経由でNNSG3dResMatCAnmを参照する辞書
    kind = NNS_G3D_DATABLK_MATC_ANMでなくてはならない。
 *---------------------------------------------------------------------------*/
typedef NNSG3dResAnmSet NNSG3dResMatCAnmSet;


/////////////////////////////////////////////////////////////////////////////////
//
//
// テクスチャパターンアニメーション
//
//


/*---------------------------------------------------------------------------*
    NNSG3dResTexPatAnmFV

    フレーム->(テクスチャ, パレット)データです。
    このデータの並びがテクスチャパタンアニメーションを構成します。
    idxFrameからテクスチャはidTexに,パレットはidPlttに切り替わる、
    という意味になります。
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResTexPatAnmFV_
{
    u16 idxFrame;
    u8  idTex;
    u8  idPltt;
}
NNSG3dResTexPatAnmFV;


/*---------------------------------------------------------------------------*
    NNSG3dTexPatAnmOption

    テクスチャパターンアニメーションのオプションです。
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_TEXPATANM_OPTION_NOPLTT = 0x0001    // パレットはアニメーションしない場合にON
}
NNSG3dTexPatAnmOption;


/*---------------------------------------------------------------------------*
    NNSG3dResDictTexPatAnmData

    各マテリアル毎に持つテクスチャパターンアニメーションのデータです。
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResDictTexPatAnmData_
{
    u16  numFV;           // NNSG3dResTexPatAnmFVの数
    u16  flag;            // NNSG3dTexPatAnmOption (現在は使用されていません)
    fx16 ratioDataFrame;  // numFV / numFrameです。データ探索用のヒントです。
    u16  offset;          // &NNSG3dResTexPatAnmを起点としたFVデータへのオフセット
}
NNSG3dResDictTexPatAnmData;


/*---------------------------------------------------------------------------*
    NNSG3dResTexPatAnm

    テクスチャパターンアニメーション1つ分のデータを束ねる構造体
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResTexPatAnm_
{
    NNSG3dResAnmHeader anmHeader;
    u16                numFrame;
    u8                 numTex;       // 使用テクスチャの数
    u8                 numPltt;      // 使用パレットの数
    u16                ofsTexName;   // テクスチャ名配列へのオフセット(NNSG3dResNameがnumTex個並んでいる)
    u16                ofsPlttName;  // パレット名配列へのオフセット(NNSG3dResNameがnumPltt個並んでいる)
    NNSG3dResDict      dict;         // マテリアル名からNNSG3dResDictTexPatAnmDataをひく辞書
    // Frame-Value data
    // テクスチャ名配列
    // パレット名配列
}
NNSG3dResTexPatAnm;


/*---------------------------------------------------------------------------*
    NNSG3dResDictTexPatAnmSetData

    個々のテクスチャパターンアニメーションのデータへのオフセットが格納されている。
    &NNSG3dResTexPatAnmSetを起点とする。
 *---------------------------------------------------------------------------*/
typedef NNSG3dResDictAnmSetData NNSG3dResDictTexPatAnmSetData;


/*---------------------------------------------------------------------------*
    NNSG3dResTexPatAnmSet

    テクスチャパターンアニメーションのセットを束ねる構造体
    後方にはNNSG3dResTexPatAnmが並んでいる。
    辞書はNNSG3dResDictTexPatAnmSetData経由でNNSG3dResTexPatAnmを参照する辞書
    kind = NNS_G3D_DATABLK_TEXPAT_ANMでなくてはならない
 *---------------------------------------------------------------------------*/
typedef NNSG3dResAnmSet NNSG3dResTexPatAnmSet;


/////////////////////////////////////////////////////////////////////////////////
//
//
// テクスチャSRTアニメーション
//
//

/*---------------------------------------------------------------------------*
    NNSG3dTexSRTElem

    テクスチャSRTアニメーションの各種フラグ。
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_TEXSRTANM_ELEM_FX16   = 0x10000000, // データをfx16のベクタで持つ場合はON(rotのときは常にOFF)
    NNS_G3D_TEXSRTANM_ELEM_CONST  = 0x20000000, // データがアニメーションしない場合にON
    NNS_G3D_TEXSRTANM_ELEM_STEP_1 = 0x00000000, // データが毎フレームあるとき
    NNS_G3D_TEXSRTANM_ELEM_STEP_2 = 0x40000000, // データが2フレーム毎にあるとき
    NNS_G3D_TEXSRTANM_ELEM_STEP_4 = 0x80000000, // データが4フレーム毎にあるとき

    NNS_G3D_TEXSRTANM_ELEM_STEP_MASK         = 0xc0000000,
    NNS_G3D_TEXSRTANM_ELEM_LAST_INTERP_MASK  = 0x0000ffff,

    NNS_G3D_TEXSRTANM_ELEM_LAST_INTERP_SHIFT = 0
}
NNSG3dTexSRTElem;


/*---------------------------------------------------------------------------*
    NNSG3dResDictTexSRTAnmData

    各マテリアル毎のテクスチャＳＲＴアニメーションの情報
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResDictTexSRTAnmData_
{
    // word #0(NNSG3dTexSRTElem)
    // 31  30 29      28     15          0
    // frmStp isConst isFx16 iFrame_interp 
    // word #1
    // 31               0
    // offset/constantVal
    u32 scaleS;             // NNSG3dTexSRTElem
    u32 scaleSEx;           // NNS_G3D_TEXSRTANM_ELEM_CONSTのときは定数値、それ以外の場合はデータ列へのオフセット

    u32 scaleT;             // NNSG3dTexSRTElem
    u32 scaleTEx;           // NNS_G3D_TEXSRTANM_ELEM_CONSTのときは定数値、それ以外の場合はデータ列へのオフセット

    u32 rot;                // NNSG3dTexSRTElem
    u32 rotEx;              // NNS_G3D_TEXSRTANM_ELEM_CONSTのときは定数値、それ以外の場合はデータ列へのオフセット
                            //
                            // rot は cos 値 sin 値がペアとなって格納されている
                            // 31          15          0
                            // cos in fx16   sin in fx16
                            
                            
    u32 transS;             // NNSG3dTexSRTElem
    u32 transSEx;           // NNS_G3D_TEXSRTANM_ELEM_CONSTのときは定数値、それ以外の場合はデータ列へのオフセット

    u32 transT;             // NNSG3dTexSRTElem
    u32 transTEx;           // NNS_G3D_TEXSRTANM_ELEM_CONSTのときは定数値、それ以外の場合はデータ列へのオフセット
}
NNSG3dResDictTexSRTAnmData;


/*---------------------------------------------------------------------------*
    NNSG3dTexSRTAnmOption

    テクスチャSRTアニメーションのオプション(中間ファイルで指定)
 *---------------------------------------------------------------------------*/
typedef enum NNSG3dTexSRTAnmOption_
{
    NNS_G3D_TEXSRTANM_OPTION_INTERPOLATION              = 0x01, // <tex_srt_info>::interpolationに対応
    NNS_G3D_TEXSRTANM_OPTION_END_TO_START_INTERPOLATION = 0x02  // <tex_srt_info>::interp_end_to_startに対応
}
NNSG3dTexSRTAnmOption;


/*---------------------------------------------------------------------------*
    NNSG3dResTexSRTAnm

    テクスチャSRTアニメーション1つ分のデータを束ねる構造体
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResTexSRTAnm_
{
    NNSG3dResAnmHeader anmHeader;
    u16                numFrame;
    u8                 flag;             // NNSG3dTexSRTAnmOption
    u8                 texMtxMode;       // NNSG3dTexMtxMode
    NNSG3dResDict      dict;             // マテリアル名->NNSG3dResDictTexSRTAnmDataの辞書
}
NNSG3dResTexSRTAnm;


/*---------------------------------------------------------------------------*
    NNSG3dResDictTexSRTAnmSetData

    個々のテクスチャＳＲＴアニメーションのデータへのオフセットが格納されている。
    &NNSG3dResTexSRTAnmSetを起点とする。
 *---------------------------------------------------------------------------*/
typedef NNSG3dResDictAnmSetData NNSG3dResDictTexSRTAnmSetData;


/*---------------------------------------------------------------------------*
    NNSG3dResTexSRTAnmSet

    テクスチャSRTアニメーションのセットを束ねる構造体
    後方にはNNSG3dResTexSRTAnmが並んでいる。
    辞書はNNSG3dResDictTexSRTAnmSetData経由でNNSG3dResTexSRTAnmを参照する辞書
    kind == NNS_G3D_SIGNATURE_NSBTAでなくてはならない。
 *---------------------------------------------------------------------------*/
typedef NNSG3dResAnmSet NNSG3dResTexSRTAnmSet;


/////////////////////////////////////////////////////////////////////////////////
//
//
// ジョイントアニメーション
//
//

/*---------------------------------------------------------------------------*
    NNSG3dResJntAnmSRTとその仲間たち

    ジョイント１つのSRTアニメーション情報を記述する
    infoのどのビットがONかによってこの後のメモリレイアウトが決定する。
    後続データはTRSの順番に並んでいる。
    
    NNS_G3D_JNTANM_SRTINFO_IDENTITY
        --> 以降のデータは存在しない。
        NNS_G3D_JNTANM_SRTINFO_IDENTITY_T -- Trans = (0, 0, 0)
        NNS_G3D_JNTANM_SRTINFO_BASE_T     -- モデルのTransを参照
            --> Translationのデータは存在しない。
            NNS_G3D_JNTANM_SRTINFO_CONST_TX -- TransのX成分がconst
                --> 1wd分のTxデータを取得 / otherwise NNSG3dJntAnmTInfo(2wd)
            NNS_G3D_JNTANM_SRTINFO_CONST_TY -- TransのY成分がconst
                --> 1wd分のTyデータを取得 / otherwise NNSG3dJntAnmTInfo(2wd)
            NNS_G3D_JNTANM_SRTINFO_CONST_TZ -- TransのZ成分がconst
                --> 1wd分のTzデータを取得 / otherwise NNSG3dJntAnmTInfo(2wd)
        NNS_G3D_JNTANM_SRTINFO_IDENTITY_R -- Rot = I
        NNS_G3D_JNTANM_SRTINFO_BASE_R     -- モデルのRotを参照
            --> Rotationのデータは存在しない
            NNS_G3D_JNTANM_SRTINFO_CONST_R -- Rotがconst
                --> 1wd分のRIdxへのインデックスデータ / otherwise NNSG3dJntAnmRInfo(2wd)
        NNS_G3D_JNTANM_SRTINFO_IDENTITY_S -- Scale = (1, 1, 1)
        NNS_G3D_JNTANM_SRTINFO_BASE_S     -- モデルのScaleを参照
            NNS_G3D_JNTANM_SRTINFO_CONST_SX -- ScaleのX成分がconst
                --> 2wd分のSx,InvSxデータを取得 / otherwise NNSG3dJntAnmSInfo(2wd)
            NNS_G3D_JNTANM_SRTINFO_CONST_SY -- ScaleのY成分がconst
                --> 2wd分のSy,InvSyデータを取得 / otherwise NNSG3dJntAnmSInfo(2wd)
            NNS_G3D_JNTANM_SRTINFO_CONST_SZ -- ScaleのZ成分がconst
                --> 2wd分のSz,InvSzデータを取得 / otherwise NNSG3dJntAnmSInfo(2wd)

    以下ではオフセットは、SRTデータ領域からのオフセットとなる。
    NNSG3dJntAnmTInfo
        NNS_G3D_JNTANM_TINFO_FX16ARRAYならば、
        オフセットで参照されているのはfx16型の配列である

    NNSG3dJntAnmRInfo
        オフセットで参照されているのはRotIdx配列である。
        RotIdxから実データにアクセスすることになる。

    NNSG3dJntAnmSInfo

    RotIdx配列
        16bitの値の並びで、0x8000がONならばRot3配列(圧縮3x3行列)へのインデックス
        OFFならばRot9配列へのインデックス
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
    NNSG3dJntAnmSRTTag

    SRTデータの性質が記述されている
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_JNTANM_SRTINFO_IDENTITY    = 0x00000001,    // SRTに何も変更がないときON
    NNS_G3D_JNTANM_SRTINFO_IDENTITY_T  = 0x00000002,    // Transが(0,0,0)のままのときON
    NNS_G3D_JNTANM_SRTINFO_BASE_T      = 0x00000004,    // Transにモデルの値を使うときON
    NNS_G3D_JNTANM_SRTINFO_CONST_TX    = 0x00000008,    // Txが定数の場合ON
    NNS_G3D_JNTANM_SRTINFO_CONST_TY    = 0x00000010,    // Tyが定数の場合ON
    NNS_G3D_JNTANM_SRTINFO_CONST_TZ    = 0x00000020,    // Tzが定数の場合ON
    NNS_G3D_JNTANM_SRTINFO_IDENTITY_R  = 0x00000040,    // Rotがない場合ON
    NNS_G3D_JNTANM_SRTINFO_BASE_R      = 0x00000080,    // Rotにモデルの値を使うときON
    NNS_G3D_JNTANM_SRTINFO_CONST_R     = 0x00000100,    // Rotが定数の場合ON
    NNS_G3D_JNTANM_SRTINFO_IDENTITY_S  = 0x00000200,    // Scaleがかからない場合ON
    NNS_G3D_JNTANM_SRTINFO_BASE_S      = 0x00000400,    // Scaleにモデルの値を使う場合ON
    NNS_G3D_JNTANM_SRTINFO_CONST_SX    = 0x00000800,    // Sxが定数の場合ON
    NNS_G3D_JNTANM_SRTINFO_CONST_SY    = 0x00001000,    // Syが定数の場合ON
    NNS_G3D_JNTANM_SRTINFO_CONST_SZ    = 0x00002000,    // Szが定数の場合ON
    
    NNS_G3D_JNTANM_SRTINFO_NODE_MASK   = 0xff000000,    // ノードIDが入る。全てのノードで定義されていなくてもいいようにしておく
    NNS_G3D_JNTANM_SRTINFO_NODE_SHIFT  = 24
}
NNSG3dJntAnmSRTTag;


/*---------------------------------------------------------------------------*
    NNSG3dJntAnmTInfo

    Transデータについての情報
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_JNTANM_TINFO_STEP_1            = 0x00000000,    // データが毎フレームあるとき
    NNS_G3D_JNTANM_TINFO_STEP_2            = 0x40000000,    // データが2フレーム毎にあるとき
    NNS_G3D_JNTANM_TINFO_STEP_4            = 0x80000000,    // データが4フレーム毎にあるとき
    NNS_G3D_JNTANM_TINFO_FX16ARRAY         = 0x20000000,    // アニメーションデータがfx16の配列の場合ON

    NNS_G3D_JNTANM_TINFO_LAST_INTERP_MASK  = 0x1fff0000,
    NNS_G3D_JNTANM_TINFO_STEP_MASK         = 0xc0000000,

    NNS_G3D_JNTANM_TINFO_LAST_INTERP_SHIFT = 16,
    NNS_G3D_JNTANM_TINFO_STEP_SHIFT        = 30
}
NNSG3dJntAnmTInfo;


/*---------------------------------------------------------------------------*
    NNSG3dJntAnmRInfo

    回転データについての情報
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_JNTANM_RINFO_STEP_1            = 0x00000000,    // データが毎フレームあるとき
    NNS_G3D_JNTANM_RINFO_STEP_2            = 0x40000000,    // データが2フレーム毎にあるとき
    NNS_G3D_JNTANM_RINFO_STEP_4            = 0x80000000,    // データが4フレーム毎にあるとき

    NNS_G3D_JNTANM_RINFO_LAST_INTERP_MASK  = 0x1fff0000,
    NNS_G3D_JNTANM_RINFO_STEP_MASK         = 0xc0000000,

    NNS_G3D_JNTANM_RINFO_LAST_INTERP_SHIFT = 16,
    NNS_G3D_JNTANM_RINFO_STEP_SHIFT        = 30
}
NNSG3dJntAnmRInfo;


/*---------------------------------------------------------------------------*
    NNSG3dJntAnmPivotInfo

    圧縮された回転行列についての情報
    +1/-1になっている要素の行・列を取り除くと、2x2行列になって、それらを
    A B
    C D
    のように名づける。C=+B/-B D=+A/-Aとなるので、
    NNS_G3D_JNTANM_PIVOTINFO_SIGN_REVC
    NNS_G3D_JNTANM_PIVOTINFO_SIGN_REVD
    に符号の情報を持っておく。
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_JNTANM_PIVOTINFO_IDXPIVOT_MASK = 0x000f,    // +1/-1になっている要素の場所
    NNS_G3D_JNTANM_PIVOTINFO_MINUS         = 0x0010,    // ONなら-1/ OFFなら+1
    NNS_G3D_JNTANM_PIVOTINFO_SIGN_REVC     = 0x0020,    // C=-BならON
    NNS_G3D_JNTANM_PIVOTINFO_SIGN_REVD     = 0x0040,    // D=-AならON

    NNS_G3D_JNTANM_PIVOT_INFO_IDXPIVOT_SHIFT = 0
}
NNSG3dJntAnmPivotInfo;


/*---------------------------------------------------------------------------*
    NNSG3dJntAnmRIdx

    回転行列データへのインデックス
    NNS_G3D_JNTANM_RIDX_PIVOTがONのときは、pivot圧縮行列へのインデックス
    OFFの場合は、通常の回転行列データへのインデックスとなる。
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_JNTANM_RIDX_PIVOT         = 0x8000,
    NNS_G3D_JNTANM_RIDX_IDXDATA_MASK  = 0x7fff,
    NNS_G3D_JNTANM_RIDX_IDXDATA_SHIFT = 0
}
NNSG3dJntAnmRIdx;


/*---------------------------------------------------------------------------*
    NNSG3dJntAnmSInfo


 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_JNTANM_SINFO_STEP_1            = 0x00000000,    // データが毎フレームあるとき
    NNS_G3D_JNTANM_SINFO_STEP_2            = 0x40000000,    // データが2フレーム毎にあるとき
    NNS_G3D_JNTANM_SINFO_STEP_4            = 0x80000000,    // データが4フレーム毎にあるとき
    NNS_G3D_JNTANM_SINFO_FX16ARRAY         = 0x20000000,    // データ値がfx16型のとき

    NNS_G3D_JNTANM_SINFO_LAST_INTERP_MASK  = 0x1fff0000,
    NNS_G3D_JNTANM_SINFO_STEP_MASK         = 0xc0000000,

    NNS_G3D_JNTANM_SINFO_LAST_INTERP_SHIFT = 16,
    NNS_G3D_JNTANM_SINFO_STEP_SHIFT        = 30
}
NNSG3dJntAnmSInfo;


/*---------------------------------------------------------------------------*
    NNSG3dResJntAnmSRTInfo


 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResJntAnmSRTInfo_
{
    u32 info;    // NNSG3dJntAnmTInfo か NNSG3dJntAnmRInfo か NNSG3dJntAnmSInfo
    u32 offset;
}
NNSG3dResJntAnmSRTInfo;


/*---------------------------------------------------------------------------*
    NNSG3dResJntAnmSRTTag


 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResJntAnmSRTTag_
{
    u32 tag;    // NNSG3dJntAnmSRTTag;
}
NNSG3dResJntAnmSRTTag;


/*---------------------------------------------------------------------------*
    NNSG3dJntAnmOption

    ジョイントアニメーションのオプション(中間ファイルで指定)
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_JNTANM_OPTION_INTERPOLATION              = 0x01, // <node_anm_info>::interpolation
    NNS_G3D_JNTANM_OPTION_END_TO_START_INTERPOLATION = 0x02  // <node_anm_info>::interp_end_to_start
}
NNSG3dJntAnmOption;


typedef struct NNSG3dResJntAnm_
{
    NNSG3dResAnmHeader anmHeader;
    u16                numFrame;
    u16                numNode;
    u32                flag;
    u32                ofsRot3;
    u32                ofsRot5;
    // u16[numNode] ofsTag
    // vector of NNSG3dResJntAnmSRTTag
}
NNSG3dResJntAnm;


/*---------------------------------------------------------------------------*
    NNSG3dResDictJntAnmSetData

    個々のジョイントアニメーション(NNSG3dResJntAnm)のデータへのオフセットが格納されている。
    &NNSG3dResJntAnmSetを起点とする。
 *---------------------------------------------------------------------------*/
typedef NNSG3dResDictAnmSetData NNSG3dResDictJntAnmSetData;


/*---------------------------------------------------------------------------*
    NNSG3dResJntAnmSet

    ジョイントアニメーションのセットを束ねる構造体
    後方にはNNSG3dResJntAnmが並んでいる。
    辞書はNNSG3dResDictJntAnmSetData経由でNNSG3dResJntAnmを参照する辞書
    kind==NNS_G3D_SIGNATURE_NSBCAでなくてはならない。
 *---------------------------------------------------------------------------*/
typedef NNSG3dResAnmSet NNSG3dResJntAnmSet;


#ifdef __cplusplus
}/* extern "C" */
#endif

#endif // NNSG3D_BINRES_RES_STRUCT_H_
