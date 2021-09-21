/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g2d - fmt
  File:     g2d_Cell_data.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.26 $
 *---------------------------------------------------------------------------*/
#ifndef NNS_G2D_CELL_DATA_H_
#define NNS_G2D_CELL_DATA_H_

#include <nitro/types.h>
#include <nnsys/g2d/fmt/g2d_Character_data.h>// セル 定義はキャラクタマッピング情報に依存する
#include <nnsys/g2d/fmt/g2d_Common_data.h>
#include <nnsys/g2d/g2d_config.h>

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
// バイナリファイル 識別子 関連
#define NNS_G2D_BINFILE_SIG_CELL          (u32)'NCER'
#define NNS_G2D_BLKSIG_CELLBANK           (u32)'CEBK'

#define NNS_G2D_USEREXBLK_CELLATTR        (u32)'UCAT'


//------------------------------------------------------------------------------
// バイナリファイル 拡張子
#define NNS_G2D_BINFILE_EXT_CELL          "NCER"

//------------------------------------------------------------------------------



// NNSG2dCellData.bUseFlipHV 内を参照するための シフト量
#define NNS_G2D_CELL_FLIP_H_SHIFT           (0 + 8) 
#define NNS_G2D_CELL_FLIP_V_SHIFT           (1 + 8)
#define NNS_G2D_CELL_FLIP_HV_SHIFT          (2 + 8)

#define NNS_G2D_CELL_HAS_BR_SHIFT           (3 + 8)

#define NNS_G2D_CELL_BS_R_MASK              0x3F        // 6 bit 
#define NNS_G2D_CELL_BS_R_OFFSET            (2)
#define NNS_G2D_CELL_BS_R_SHIFT             (0)

#ifndef NNS_G2D_LIMIT_CELL_X_128
    #define NNS_G2D_CELL_MAX_X                  255
#else // NNS_G2D_LIMIT_CELL_X_128
    #define NNS_G2D_CELL_MAX_X                  127
#endif // NNS_G2D_LIMIT_CELL_X_128

#define NNS_G2D_CELL_MAX_Y                  127

//------------------------------------------------------------------------------
// NNSG2dCellDataBank.cellBankAttr 内のビット情報定義
#define NNS_G2D_CELLBK_ATTR_CELLWITHBR       0x1 // NNSG2dCellDataBank.cellBankAttrと & をとる
                                                 // セルデータが 
                                                 // NNSG2dCellDataWithBR なら 1 
                                                 // NNSG2dCellData なら 0

//
// バージョン情報
// Ver         変更点
// -------------------------------------
// 1.0         Vram転送アニメ機能の拡張
//
#define NNS_G2D_NCER_MAJOR_VER              1
#define NNS_G2D_NCER_MINOR_VER              0       



//------------------------------------------------------------------------------
// 関数改名：互換性のため、古い名称を別名として定義します
#define NNS_G2dGetSizeReqiredVramTransferCellDataBank   NNS_G2dGetSizeRequiredVramTransferCellDataBank   

/*---------------------------------------------------------------------------*
  Name:         NNSG2dCellVramTransferData

  Description:  1つのセルを表示するのに必要なVram転送に関する情報
                
 *---------------------------------------------------------------------------*/
typedef struct NNSG2dCellVramTransferData
{
    u32        srcDataOffset;
    u32        szByte;
    
}NNSG2dCellVramTransferData;

/*---------------------------------------------------------------------------*
  Name:         NNSG2dVramTransferData

  Description:  セルを表示するのに必要なVram転送に関する情報
                
 *---------------------------------------------------------------------------*/
typedef struct NNSG2dVramTransferData
{
    u32                                 szByteMax;             // すべてのVRAM転送中の最大バイト数
    NNSG2dCellVramTransferData*         pCellTransferDataArray;// 保持される NNSG2dCellDataBank 
                                                               // の numCells 分の長さです
    
}NNSG2dVramTransferData;




/*---------------------------------------------------------------------------*
  Name:         NNSG2dCellOAMAttrData

  Description:  セルを定義する OAMアトリビュート
                
 *---------------------------------------------------------------------------*/
typedef struct NNSG2dCellOAMAttrData
{
    u16         attr0;
    u16         attr1;
    u16         attr2;

}NNSG2dCellOAMAttrData;

/*---------------------------------------------------------------------------*
  Name:         NNSG2dCellData

  Description:  セルの境界情報(矩形)を定義する
                
 *---------------------------------------------------------------------------*/
typedef struct NNSG2dCellBoundingRectS16
{
    s16                       maxX;
    s16                       maxY;
    s16                       minX;
    s16                       minY;
    
}NNSG2dCellBoundingRectS16; 

/*---------------------------------------------------------------------------*
  Name:         NNSG2dCellData

  Description:  セルを定義する 概念
                
 *---------------------------------------------------------------------------*/
typedef struct NNSG2dCellData
{
    u16                       numOAMAttrs;      // OAM アトリビュート数
    u16                       cellAttr;         // セルの種類に関する情報
                                                // フリップ に関する 情報 (上位8ビット)
                                                // 境界情報に関する情報
                                                // 
                                                // 描画最適化に関する 情報 (下位8ビット)
                                                //      (6Bit => 境界球の半径 >> 2 )
                                                
    
    NNSG2dCellOAMAttrData*    pOamAttrArray;    // NNSG2dCellOAMAttrData 配列へのポインタ

}NNSG2dCellData;

/*---------------------------------------------------------------------------*
  Name:         NNSG2dCellDataWithBR
  
  Description:  セルを定義する 概念(境界情報としてNNSG2dCellBoundingRectを持つ)
                
 *---------------------------------------------------------------------------*/
typedef struct NNSG2dCellDataWithBR
{
    NNSG2dCellData              cellData;
    NNSG2dCellBoundingRectS16   boundingRect;
    
}NNSG2dCellDataWithBR;

/*---------------------------------------------------------------------------*
  Name:         NNSG2dCellDataBank

  Description:  複数 セルデータをまとめる 概念
                通常は NNSG2dCellDataBank == バイナリファイル( NCER ) です
                
 *---------------------------------------------------------------------------*/
typedef struct NNSG2dCellDataBank
{
    u16                              numCells;               // セル総数
    u16                              cellBankAttr;           // 属性
    NNSG2dCellData*                  pCellDataArrayHead;     // offset addres 
    NNSG2dCharacterDataMapingType    mappingMode;            // 参照キャラクタのマッピングモード
    
    NNSG2dVramTransferData*          pVramTransferData;      // Vram 転送 アニメ に関する情報
    
    void*                            pStringBank;            // 文字バンク
    void*                            pExtendedData;          // offset addr (if it exists.)
    
    
}NNSG2dCellDataBank;

/*---------------------------------------------------------------------------*
  Name:         NNSG2dCellDataBankBlock 

  Description:  NNSG2dCellDataBankを格納するバイナリブロック
  
 *---------------------------------------------------------------------------*/
typedef struct NNSG2dCellDataBankBlock
{
    NNSG2dBinaryBlockHeader     blockHeader;
    NNSG2dCellDataBank          cellDataBank;
    
}NNSG2dCellDataBankBlock;


//------------------------------------------------------------------------------
typedef struct NNSG2dUserExCellAttr
{
    u32*           pAttr;
    
}NNSG2dUserExCellAttr;

typedef struct NNSG2dUserExCellAttrBank
{
    u16                      numCells; // 
    u16                      numAttribute; // アトリビュート数：現在は1固定
    NNSG2dUserExCellAttr*    pCellAttrArray;   
    
}NNSG2dUserExCellAttrBank;



//------------------------------------------------------------------------------
// インライン関数
//------------------------------------------------------------------------------

/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dSetCellBankHasBR

  Description:  セルが境界矩形を持つセルバンクかどうかを取得します
                << コンバータが使用する関数です >>
                
  Arguments:    pCellBank:           セルバンク情報
                
                
  Returns:      セルバンクが境界矩形を持つかどうか
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE void NNSi_G2dSetCellBankHasBR(  NNSG2dCellDataBank* pCellBank )
{
    NNS_G2D_NULL_ASSERT( pCellBank );
    pCellBank->cellBankAttr |= NNS_G2D_CELLBK_ATTR_CELLWITHBR;
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dCellBankHasBR

  Description:  セルが境界矩形を持つセルバンクかどうかを取得します
                
  Arguments:    pCellBank:           セルバンク情報
                
                
  Returns:      セルバンクが境界矩形を持つかどうか
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE BOOL NNSi_G2dCellBankHasBR(  const NNSG2dCellDataBank* pCellBank )
{
    return (BOOL)( pCellBank->cellBankAttr & NNS_G2D_CELLBK_ATTR_CELLWITHBR );
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dSetCellAttrFlipFlag

  Description:  フリップ利用設定情報を格納した u16 変数 を設定し返します 
                
  Arguments:    bH:         Hフリップ
                bV:         Vフリップ
                bHV:        HVフリップ
                
  Returns:      情報を格納した u16 変数 
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE u16 NNSi_G2dSetCellAttrFlipFlag( BOOL bH, BOOL bV, BOOL bHV )
{
    return (u16)((bH  << NNS_G2D_CELL_FLIP_H_SHIFT) |
                 (bV  << NNS_G2D_CELL_FLIP_V_SHIFT) |
                 (bHV << NNS_G2D_CELL_FLIP_HV_SHIFT)  );
}
/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dSetCellAttrHasBR

  Description:  セルが境界矩形を持つかどうかを設定した u16 変数 を設定し返します 
                << コンバータが使用する関数です >>
                
  Arguments:    bHasBR:         セルが境界矩形を持つかどうか
                
                
  Returns:      情報を格納した u16 変数 
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE u16 NNSi_G2dSetCellAttrHasBR( BOOL bHasBR )
{
    return (u16)( bHasBR << NNS_G2D_CELL_HAS_BR_SHIFT );
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dIsCellFlipH

  Description:  Hフリップ を使用するか調査します
                
  Arguments:    pCell:           セルデータ 
                
  Returns:      使用するかどうか
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE BOOL NNSi_G2dIsCellFlipH( const NNSG2dCellData* pCell )
{
    
    return (BOOL)( ( pCell->cellAttr >> NNS_G2D_CELL_FLIP_H_SHIFT) & 0x1);
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dIsCellFlipV

  Description:  Vフリップ を使用するか調査します
                
  Arguments:    pCell:           セルデータ 
                
  Returns:      使用するかどうか
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE BOOL NNSi_G2dIsCellFlipV( const NNSG2dCellData* pCell )
{
    return (BOOL)( (pCell->cellAttr >> NNS_G2D_CELL_FLIP_V_SHIFT) & 0x1);
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dIsCellFlipHV

  Description:  HVフリップ を使用するか調査します
                
  Arguments:    pCell:           セルデータ 
                
  Returns:      使用するかどうか
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE BOOL NNSi_G2dIsCellFlipHV( const NNSG2dCellData* pCell )
{
    return (BOOL)( (pCell->cellAttr >> NNS_G2D_CELL_FLIP_HV_SHIFT) & 0x1);
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dCellHasBR

  Description:  セルが境界矩形を持つかどうかを取得します
                
  Arguments:    pCell:           セルデータ
                
                
  Returns:      セルが境界矩形を持つかどうか
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE BOOL NNS_G2dCellHasBR(  const NNSG2dCellData* pCell )
{
    return (BOOL)( (pCell->cellAttr >> NNS_G2D_CELL_HAS_BR_SHIFT) & 0x1);
}

NNS_G2D_INLINE BOOL NNSi_G2dCellHasBR(  const NNSG2dCellData* pCell )
{
    return NNS_G2dCellHasBR( pCell );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dCellBoundingRect

  Description:  セルの境界矩形を取得します。
                内部ではセルが境界矩形を持つか判定しません。
                判定はユーザコードで行ってください。
                
  Arguments:    pCell:           セルデータ
                
                
  Returns:      セルが境界矩形
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE const NNSG2dCellBoundingRectS16* 
NNS_G2dGetCellBoundingRect( const NNSG2dCellData* pCell )
{
    // NNS_G2D_ASSERT( NNSi_G2dCellHasBR( pCell ) )
    const NNSG2dCellDataWithBR* pBR = ( const NNSG2dCellDataWithBR* )pCell;
    return &pBR->boundingRect;
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dSetCellBoundingSphereR

  Description:  セルの境界球情報を設定します。(本関数はコンバータにより使用される関数です)
                8Bit境界球半径情報を2ビット右シフトして、pCell->cellAttrに格納します。

                半径情報の下位2ビットは切捨て前に、切り上げ処理をおこないます。
                
                << コンバータが使用する関数です >>
                
                
  Arguments:    pCell:           セルデータ
                R    :           境界球半径
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE void NNSi_G2dSetCellBoundingSphereR( NNSG2dCellData* pCell, u8 R )
{
    //
    // 切り上げ処理
    //
    if( (R & 0x3) != 0 )
    {
        R += 0x1 << NNS_G2D_CELL_BS_R_OFFSET;
        R &= ~0x3;
    }

    R = (u8)(R >> NNS_G2D_CELL_BS_R_OFFSET);
    pCell->cellAttr |= (( R & NNS_G2D_CELL_BS_R_MASK ) << NNS_G2D_CELL_BS_R_SHIFT);
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetCellBoundingSphereR    
                NNSi_G2dGetCellBoundingSphereR

  Description:  セルの境界球情報を取得します。
                
  Arguments:    pCell:           セルデータ
                
                
  Returns:      境界球半径
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE u8 NNS_G2dGetCellBoundingSphereR( const NNSG2dCellData* pCell )
{
    u8 R =  (u8)( (pCell->cellAttr >> NNS_G2D_CELL_BS_R_SHIFT) & NNS_G2D_CELL_BS_R_MASK );
    return (u8)(R << NNS_G2D_CELL_BS_R_OFFSET);
}

NNS_G2D_INLINE u8 NNSi_G2dGetCellBoundingSphereR( const NNSG2dCellData* pCell )
{
    // TODO:警告を表示する
    return NNS_G2dGetCellBoundingSphereR( pCell );
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dCellDataBankHasVramTransferData

  Description:  セルバンクがVRAM転送情報を持っているか調査します。
                
  Arguments:    pCellBank:           セルデータバンク
                
                
  Returns:      もっていればTRUE
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE BOOL NNS_G2dCellDataBankHasVramTransferData( const NNSG2dCellDataBank* pCellBank )
{
    // NNS_G2D_NULL_ASSERT( pCellBank );
    return (BOOL)( pCellBank->pVramTransferData != NULL );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetSizeRequiredVramTransferCellDataBank

  Description:  VRAM転送セルバンクが必要とするVRAM領域の大きさ（バイト）を取得します。
                VRAM転送情報を持っていない場合はゼロを返します。
                
  Arguments:    pCellBank:           セルデータバンク
                
                
  Returns:      必要とするVRAM領域の大きさ（バイト）
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE u32 NNS_G2dGetSizeRequiredVramTransferCellDataBank( const NNSG2dCellDataBank* pCellBank )
{
    // NNS_G2D_NULL_ASSERT( pCellBank );
    if( NNS_G2dCellDataBankHasVramTransferData( pCellBank ) )
    {
        const NNSG2dVramTransferData* pData = pCellBank->pVramTransferData;
        
        return pData->szByteMax;
    }
    
    return 0;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetCellVramTransferData

  Description:  指定されたセル番号のVRAM転送セルデータを取得します
                
                セル数以上のセル番号が指定された場合、アサートに失敗します
                VRAM転送情報を持たないセルデータバンクが指定された場合、アサートに失敗します
                
  Arguments:    pCellBank   :           セルデータバンク
                cellIdx     :           セル番号
                
  Returns:      必要とするVRAM領域の大きさ（バイト）
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE const NNSG2dCellVramTransferData* 
NNSi_G2dGetCellVramTransferData( const NNSG2dCellDataBank* pCellBank, u16 cellIdx )
{
    //( pCellBank );
    //( cellIdx < pCellBank->numCells );
    // ( NNS_G2dCellDataBankHasVramTransferData( pCellBank )
    const NNSG2dVramTransferData*       pTransferData = pCellBank->pVramTransferData;
        
    return &pTransferData->pCellTransferDataArray[cellIdx];
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetUserExCellAttrBankFromCellBank

  Description:  セルバンクからセル拡張アトリビュートバンクを取得します。
                
                
  Arguments:    pCellBank   :           セルデータバンク
                
                
  Returns:      セル拡張アトリビュートバンク
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE const NNSG2dUserExCellAttrBank* 
NNS_G2dGetUserExCellAttrBankFromCellBank( const NNSG2dCellDataBank* pCellBank )
{
    // ブロックを取得します
    const NNSG2dUserExDataBlock* pBlk 
        = NNSi_G2dGetUserExDataBlkByID( pCellBank->pExtendedData,
                                        NNS_G2D_USEREXBLK_CELLATTR );
    // ブロックの取得に成功したら...
    if( pBlk != NULL )
    {
        return (const NNSG2dUserExCellAttrBank*)(pBlk + 1);
    }else{
        return NULL;                                
    }
}
/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetUserExCellAttr

  Description:  セル拡張アトリビュートバンクからセル拡張アトリビュート
                をセル番号を指定して取得します。
                
                
  Arguments:    pCellAttrBank   :           セル拡張アトリビュートバンク
                idx             :           セル番号                
                
  Returns:      セル拡張アトリビュート
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE const NNSG2dUserExCellAttr* 
NNS_G2dGetUserExCellAttr
( 
    const NNSG2dUserExCellAttrBank* pCellAttrBank, 
    u16                             idx
)
{
    if( idx < pCellAttrBank->numCells )
    {
        return &pCellAttrBank->pCellAttrArray[idx];
    }else{
        return NULL;    
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetUserExCellAttrValue

  Description:  セル拡張アトリビュートからアトリビュート値を取得します
                
                
  Arguments:    pCellAttrBank   :           セル拡張アトリビュート
                
                
  Returns:      アトリビュート値
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE const u32
NNS_G2dGetUserExCellAttrValue
( 
    const NNSG2dUserExCellAttr* pCellAttr 
)
{
    return pCellAttr->pAttr[0];
}


/*---------------------------------------------------------------------------*
  Name:         NNS_G2dCopyCellAsOamAttr

  Description:  セルデータ内のOamアトリビュート情報をGXOamAttrにコピーします。
                
  Arguments:    pCell       :       セルデータ
                idx         :       OAMアトリビュート番号
                pDst        :       コピー先 GXOamAttr
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE void NNS_G2dCopyCellAsOamAttr( const NNSG2dCellData* pCell, u16 idx, GXOamAttr* pDst )
{
    //NNS_G2D_NULL_ASSERT( pCell );
    //NNS_G2D_NULL_ASSERT( pDst );
    //NNS_G2D_MINMAX_ASSERT( idx, 0, pCell->numOAMAttrs );
    {
        const NNSG2dCellOAMAttrData* pSrc = pCell->pOamAttrArray + idx;
        
        pDst->attr0 = pSrc->attr0;
        pDst->attr1 = pSrc->attr1;
        pDst->attr2 = pSrc->attr2;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dRepeatXinCellSpace

  Description:  セル内のOBJのリピート（座標回り込み）処理を行います
                結果 s16 値は -128 - 127 へと変換されます。
                
  Arguments:    srcX       :       元の値
                
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE  s16 NNS_G2dRepeatXinCellSpace( s16 srcX )
{
    if( srcX > NNS_G2D_CELL_MAX_X )
    { 
        srcX |= 0xFF00; 
    }
    return srcX;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dRepeatYinCellSpace

  Description:  セル内のOBJのリピート（座標回り込み）処理を行います
                結果 s16 値は -128 - 127 へと変換されます。
                
  Arguments:    srcY       :       元の値
                
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE  s16 NNS_G2dRepeatYinCellSpace( s16 srcY )
{   
    if( srcY > NNS_G2D_CELL_MAX_Y )
    { 
        srcY |= 0xFF00; 
    }
    return srcY;
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // NNS_G2D_CELL_DATA_H_

