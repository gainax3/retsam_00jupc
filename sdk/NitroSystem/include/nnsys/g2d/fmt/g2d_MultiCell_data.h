/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g2d - fmt
  File:     g2d_MultiCell_data.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.14 $
 *---------------------------------------------------------------------------*/
#ifndef NNS_G2D_MULTICELL_DATA_H_
#define NNS_G2D_MULTICELL_DATA_H_

#include <nitro/types.h>
#include <nnsys/g2d/fmt/g2d_Common_data.h>
#include <nnsys/g2d/fmt/g2d_Anim_data.h>
#include <nnsys/g2d/fmt/g2d_Cell_data.h>
#include <nnsys/g2d/g2d_config.h>


#ifdef __cplusplus
extern "C" {
#endif

#define NNS_G2D_BINFILE_EXT_MULTICELL     "NMCR"

#define NNS_G2D_BINFILE_SIG_MULTICELL          (u32)'NMCR'
#define NNS_G2D_BLKSIG_MULTICELLBANK           (u32)'MCBK'

//
// バージョン情報
// Ver         変更点
// -------------------------------------
// 1.0         初版
//
#define NNS_G2D_NMCR_MAJOR_VER             (u8)1
#define NNS_G2D_NMCR_MINOR_VER             (u8)0



#define NNS_G2D_MCNODE_PLAYMODE_MASK        0x0F
#define NNS_G2D_MCNODE_PLAYMODE_SHIFT       0
#define NNS_G2D_MCNODE_VISIBILITY_SHIFT     5
#define NNS_G2D_MCNODE_CELLANIMIDX_SHIFT    8
#define NNS_G2D_MCNODE_CELLANIMIDX_MASK     0xFF00

typedef enum NNSG2dMCAnimationPlayMode
{
    NNS_G2D_MCANIM_PLAYMODE_RESET = 0,
    NNS_G2D_MCANIM_PLAYMODE_CONTINUE  = 1,
    NNS_G2D_MCANIM_PLAYMODE_PAUSE = 2,
    NNS_G2D_MCANIM_PLAYMODE_MAX
    
}NNSG2dMCAnimationPlayMode;

//------------------------------------------------------------------------------
typedef struct NNSG2dMultiCellHierarchyData
{
    u16         animSequenceIdx;        // NNSG2dCellAnimation が 再生するシーケンス番号
    s16         posX;                   // MultiCell ローカル系での NNSG2dCellAnimation の 位置
    s16         posY;                   // MultiCell ローカル系での NNSG2dCellAnimation の 位置
                                        
                                        
    u16         nodeAttr;               // Node属性 ( NNSG2dMCAnimationPlayMode など 
                                        // 16 ............ 8 ... 5 .....4 .........................0
                                        //   セルアニメ番号   予約    可視   NNSG2dMCAnimationPlayMode
                                        //
    
    
}NNSG2dMultiCellHierarchyData;


//------------------------------------------------------------------------------
// NNSG2dMultiCellData 定義データ
// NNSG2dMultiCellAnimation 定義ファイルに格納されるデータ
// NNSG2dMultiCellInstance によって参照される
//
typedef struct NNSG2dMultiCellData
{
    u16                             numNodes;
//  u16                             numTotalOams;     // MultiCellの描画に必要な総Oam数(未実装：使用不可)
    u16                             numCellAnim;      // 必要になるセルアニメ実体の数
    NNSG2dMultiCellHierarchyData*   pHierDataArray;   // 再生シーケンス、位置、など(numNodes 分 )

}NNSG2dMultiCellData;


//------------------------------------------------------------------------------
typedef struct NNSG2dMultiCellDataBank
{
    u16                             numMultiCellData;
    u16                             pad16;
    NNSG2dMultiCellData*            pMultiCellDataArray;
    NNSG2dMultiCellHierarchyData*   pHierarchyDataArray;
    void*                           pStringBank;
    void*                           pExtendedData;        // offset addr (if it exists.)
    // MultiCellを構成する NNSG2dCellAnimationが定義されている NNSG2dAnimBankData
    // const NNSG2dAnimBankData*      pAnimDataBank;// 関連するAnimBank (ランタイムにてUnpack時に設定されます)
    //
    // 変更：この情報はNNSG2dMultiCellInstance(実体側)で保持することとする
    // 
    // NNSG2dMultiCellInstance(ランタイム実体)での使用ケースを考えると、 
    // pAnimDataBank情報 は NNSG2dMultiCellData で保持したい情報だが、NNSG2dMultiCellDataで保持すると
    //  ・データ量増加が大きい( ∵ NNSG2dMultiCellData は 数量がとても多い場合が考えられる )
    //  ・pAnimDataBank データは重複する場合が多い
    // 
    // それならば、NNSG2dMultiCellData を もつ NNSG2dMultiCellInstance で データを持つこととする。
    // 上記の欠点をまったく回避できないが、ランタイム側で関連付ける分だけ柔軟性は高い。
    // (特に 複数の NNSG2dMultiCellInstance が NNSG2dMultiCellData を共有するケースで自由度が高い。)
    // 
    
    
}NNSG2dMultiCellDataBank;

//------------------------------------------------------------------------------
typedef struct NNSG2dMultiCellDataBankBlock
{
    NNSG2dBinaryBlockHeader     blockHeader;
    NNSG2dMultiCellDataBank     multiCellDataBank;
    
}NNSG2dMultiCellDataBankBlock;


//------------------------------------------------------------------------------
typedef struct NNSG2dUserExMultiCellAttr
{
    u32*           pAttr;
    
}NNSG2dUserExMultiCellAttr;

typedef struct NNSG2dUserExMultiCellAttrBank
{
    u16                          numMultiCells; // 
    u16                          numAttribute;  // アトリビュート数：現在は1固定
    NNSG2dUserExMultiCellAttr*   pMCAttrArray;   
    
}NNSG2dUserExMultiCellAttrBank;



//------------------------------------------------------------------------------
// inline functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// マルチセルノードアトリビュートを設定します
// コンバータが使用する関数です。
// 
NNS_G2D_INLINE void 
NNSi_G2dSetMultiCellNodeAttribute
( 
    NNSG2dMCAnimationPlayMode    mode, 
    int                          bVisibility,
    u16*                         pDstAttr
)
{
    *pDstAttr = (u16)( ( ( mode & NNS_G2D_MCNODE_PLAYMODE_MASK ) << NNS_G2D_MCNODE_PLAYMODE_SHIFT ) | 
                       ( ( bVisibility & 0x1 ) << NNS_G2D_MCNODE_VISIBILITY_SHIFT ) );
}

//------------------------------------------------------------------------------
// Nodeの可視状態を取得します
NNS_G2D_INLINE BOOL 
NNSi_G2dIsMultiCellNodeVisible
( 
    const NNSG2dMultiCellHierarchyData* pNode 
)
{
    return (BOOL)(( pNode->nodeAttr >> NNS_G2D_MCNODE_VISIBILITY_SHIFT ) & 0x1);
}

//------------------------------------------------------------------------------
// Nodeにバインドされるセルアニメーションの再生方式を取得します
NNS_G2D_INLINE NNSG2dMCAnimationPlayMode 
NNSi_G2dGetMultiCellNodePlayMode
( 
    const NNSG2dMultiCellHierarchyData* pNode 
)
{
    const NNSG2dMCAnimationPlayMode mode 
        = (NNSG2dMCAnimationPlayMode) ( ( pNode->nodeAttr >> NNS_G2D_MCNODE_PLAYMODE_SHIFT ) & 
                                          NNS_G2D_MCNODE_PLAYMODE_MASK );
    // TODO:ASSERT
                                                
    return mode;
}
//------------------------------------------------------------------------------
// Nodeが参照するセルアニメの番号を設定します
// コンバータから利用される関数です。
NNS_G2D_INLINE void NNSi_G2dSetMC2NodeCellAinmIdx
( 
    NNSG2dMultiCellHierarchyData*  pNodeData, 
    u8                             idx 
)
{
    pNodeData->nodeAttr &= ~NNS_G2D_MCNODE_CELLANIMIDX_MASK;
    pNodeData->nodeAttr |= idx << NNS_G2D_MCNODE_CELLANIMIDX_SHIFT;
}

//------------------------------------------------------------------------------
// Nodeが参照するセルアニメの番号を取得します
NNS_G2D_INLINE u16 
NNSi_G2dGetMC2NodeCellAinmIdx
( 
    const NNSG2dMultiCellHierarchyData*  pNodeData
)
{
    return (u16)((NNS_G2D_MCNODE_CELLANIMIDX_MASK & pNodeData->nodeAttr ) >> NNS_G2D_MCNODE_CELLANIMIDX_SHIFT);
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetUserExCellAttrBankFromMCBank

  Description:  マルチセルバンクからセル拡張アトリビュートバンクを取得します。
                
                
  Arguments:    pMCBank   :           マルチセルバンク
                
                
  Returns:      セル拡張アトリビュートバンク
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE const NNSG2dUserExCellAttrBank* 
NNS_G2dGetUserExCellAttrBankFromMCBank( const NNSG2dMultiCellDataBank* pMCBank )
{
    // ブロックを取得します
    const NNSG2dUserExDataBlock* pBlk 
        = NNSi_G2dGetUserExDataBlkByID( pMCBank->pExtendedData,
                                        NNS_G2D_USEREXBLK_CELLATTR );
    // ブロックの取得に成功したら...
    if( pBlk != NULL )
    {
        return (const NNSG2dUserExCellAttrBank*)(pBlk + 1);
    }else{
        return NULL;                                
    }
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // NNS_G2D_MULTICELL_DATA_H_

