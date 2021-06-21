/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g2d - fmt
  File:     g2d_Entity_data.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.7 $
 *---------------------------------------------------------------------------*/
#ifndef NNS_G2D_ENTITY_DATA_H_
#define NNS_G2D_ENTITY_DATA_H_

#include <nitro/types.h>
#include <nnsys/g2d/fmt/g2d_Common_data.h>
#include <nnsys/g2d/fmt/g2d_Anim_data.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NNS_G2D_BINFILE_SIG_ENTITY        (u32)'NENR'
#define NNS_G2D_BLKSIG_ENTITYBANK         (u32)'ENBK'

#define NNS_G2D_BINFILE_EXT_ENTITY          "NENR"

//
// バージョン情報
// Ver         変更点
// -------------------------------------
// 1.0         初版
//
#define NNS_G2D_NENR_MAJOR_VER              1
#define NNS_G2D_NENR_MINOR_VER              0


//------------------------------------------------------------------------------
// Entity の 種類
typedef enum NNSG2dEntityType
{
    NNS_G2D_ENTITYTYPE_INVALID      = 0,
    NNS_G2D_ENTITYTYPE_CELL         ,
    NNS_G2D_ENTITYTYPE_MULTICELL    ,
    NNS_G2D_ENTITYTYPE_MAX          

}NNSG2dEntityType;

//------------------------------------------------------------------------------
// Entity が持つアニメーションデータ配列
typedef struct NNSG2dEntityAnimData
{
    u16         numAnimSequence;
    u16         pad16;
    u16*        pAnimSequenceIdxArray;      // NNSG2dAnimSequence番号配列
    
}NNSG2dEntityAnimData;

//------------------------------------------------------------------------------
// Entity 設定情報
typedef struct NNSG2dEntityData
{
    
    NNSG2dEntityAnimData          animData;      // Entity が持つアニメーションデータ配列
    NNSG2dEntityType              type;          // 描画実体の種類
    
    // const NNSG2dAnimBankData*     pAnimDataBank; // 関連するAnimBank (ランタイムにてUnpack時に設定されます)
    // 
    // この情報は実体側で保持することとする
    // 
    // この変更は、NNSG2dEntityData に とっては 最良の選択ではない。
    // （ NNSG2dEntityData の数は少なく、それを共有する実体の数は多い ）
    // NNSG2dMultiCellDataBank との統一感を出すための変更である。
    // ここで、発生するコストに対してはあまりナーバスにはならないというスタンス。
    // 
    // NNSG2dMultiCellDataBank 宣言 の コメント を 参照されたい。
    // 
    
}NNSG2dEntityData;


//------------------------------------------------------------------------------
// Entity 設定情報バンク
typedef struct NNSG2dEntityDataBank
{
    u16                     numEntityDatas;
    u16                     pad16;
    NNSG2dEntityData*       pEntityDataArray;
    u16*                    pAnimSequenceIdxArray;
    void*                   pStringBank;
    void*                   pExtendedData;        // offset addr (if it exists.)
    
}NNSG2dEntityDataBank;


typedef struct NNSG2dEntityDataBankBlock
{
    
    NNSG2dBinaryBlockHeader     blockHeader;
    NNSG2dEntityDataBank        entityDataBank;
    
}NNSG2dEntityDataBankBlock;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // NNS_G2D_ENTITY_DATA_H_

