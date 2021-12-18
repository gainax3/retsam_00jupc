/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g2d - fmt
  File:     g2d_Anim_data.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.17 $
 *---------------------------------------------------------------------------*/
#ifndef NNS_G2D_ANIM_DATA_H_
#define NNS_G2D_ANIM_DATA_H_





#include <nitro/types.h>
#include <nitro/fx/fx.h>


#include <nnsys/g2d/g2d_config.h>
#include <nnsys/g2d/fmt/g2d_Common_data.h>

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
// バイナリファイル 識別子 関連
#define NNS_G2D_BINFILE_SIG_CELLANIM          (u32)'NANR'
#define NNS_G2D_BINFILE_SIG_MULTICELLANIM     (u32)'NMAR'

#define NNS_G2D_BLKSIG_ANIMBANK               (u32)'ABNK'
#define NNS_G2D_USEREXBLK_ANMATTR             (u32)'UAAT'

//------------------------------------------------------------------------------
// バイナリファイル 拡張子
#define NNS_G2D_BINFILE_EXT_CELLANIM          "NANR"
#define NNS_G2D_BINFILE_EXT_MULTICELLANIM     "NMAR"

//
// バージョン情報
// Ver         変更点
// -------------------------------------
// 1.0         初版
//
#define NNS_G2D_NANR_MAJOR_VER   1
#define NNS_G2D_NANR_MINOR_VER   0


// NNSG2dAnimSequenceData メンバアクセスに利用
#define NNS_G2D_ANIMTYPE_SHIFT      16
#define NNS_G2D_ANIMTYPE_MASK       0xFF00
#define NNS_G2D_ANIMELEM_MASK       0x00FF


/*---------------------------------------------------------------------------*
  Name:         NNSG2dAnimationType

  Description:  Animation の 種類
                NNSG2dAnimSequence が保持します
                （つまり、ひとつのSequence内では同一NNSG2dAnimationTypeが保障されます）
                
 *---------------------------------------------------------------------------*/
typedef enum NNSG2dAnimationType
{
    NNS_G2D_ANIMATIONTYPE_INVALID           = 0x0, // 不正な種類
    NNS_G2D_ANIMATIONTYPE_CELL                   , // セル
    NNS_G2D_ANIMATIONTYPE_MULTICELLLOCATION      , // マルチセル
    NNS_G2D_ANIMATIONTYPE_MAX
}
NNSG2dAnimationType;

/*---------------------------------------------------------------------------*
  Name:         NNSG2dAnimationElement

  Description:  Animation 要素 の 種類
                NNSG2dAnimSequence が保持します
                （つまり、ひとつのSequence内では同一NNSG2dAnimationElementが保障されます）
                
 *---------------------------------------------------------------------------*/
typedef enum NNSG2dAnimationElement
{
    NNS_G2D_ANIMELEMENT_INDEX           = 0x0, // Index のみ
    NNS_G2D_ANIMELEMENT_INDEX_SRT            , // Index + SRT 
    NNS_G2D_ANIMELEMENT_INDEX_T              , // Index + T 
    NNS_G2D_ANIMELEMENT_MAX
}
NNSG2dAnimationElement;

/*---------------------------------------------------------------------------*
  Name:         NNSG2dAnimationPlayMode

  Description:  アニメーションシーケンスの再生方法
                NNSG2dAnimSequence が保持します
                
                
 *---------------------------------------------------------------------------*/
typedef enum NNSG2dAnimationPlayMode
{
    NNS_G2D_ANIMATIONPLAYMODE_INVALID = 0x0,    // 無効
    NNS_G2D_ANIMATIONPLAYMODE_FORWARD,          // ワンタイム再生(順方向)
    NNS_G2D_ANIMATIONPLAYMODE_FORWARD_LOOP,     // リピート再生(順方向ループ)
    NNS_G2D_ANIMATIONPLAYMODE_REVERSE,          // 往復再生(リバース（順＋逆方向）
    NNS_G2D_ANIMATIONPLAYMODE_REVERSE_LOOP,     // 往復再生リピート（リバース（順＋逆順方向） ループ）
    NNS_G2D_ANIMATIONPLAYMODE_MAX               
}
NNSG2dAnimationPlayMode;


/*---------------------------------------------------------------------------*
  Name:         NNSG2dAnimDataSRT

  Description:  アニメーション結果
                NNSG2dAnimFrameData.pContent の先がこのデータになっています。
                将来的には、アニメーション結果 NNSG2dAnimDataSRT 以外の 
                複数のフォーマットのサポートが予定されています。
                                
 *---------------------------------------------------------------------------*/
typedef       u16       NNSG2dAnimData; // index のみ

typedef struct NNSG2dAnimDataSRT        // index + SRT 情報
{
    u16         index;      // インデックス
    
    u16         rotZ;       // 回転
    
    fx32        sx;         // スケールX
    fx32        sy;         // スケールY
    
    s16         px;         // 位置X
    s16         py;         // 位置Y
    
}NNSG2dAnimDataSRT;

typedef struct NNSG2dAnimDataT        // index + T 情報
{
    u16         index;      // インデックス    
    u16         pad_;       // 回転

    s16         px;         // 位置X
    s16         py;         // 位置Y
    
}NNSG2dAnimDataT;



/*---------------------------------------------------------------------------*
  Name:         NNSG2dAnimFrameData

  Description:  アニメーションフレーム
                アニメーションを構成する単位要素
                アニメーションフレームが複数連なって アニメーションシーケンスを
                形成します。
                
 *---------------------------------------------------------------------------*/
typedef struct NNSG2dAnimFrameData
{
    void*           pContent;   // アニメーション結果へのポインタ
    u16             frames;     // アニメフレーム持続時間( 単位 : ビデオフレーム )
    u16             pad16;      // pading 
    
}NNSG2dAnimFrameData;





/*---------------------------------------------------------------------------*
  Name:         NNSG2dAnimFrameData

  Description:  アニメーションシーケンス
                一連のアニメーションフレームの集合
                いわゆるアニメーションに対応する概念です。
                
 *---------------------------------------------------------------------------*/
typedef struct NNSG2dAnimSequenceData
{
    u16                     numFrames;         // シーケンスを構成するアニメーションフレーム数
    u16                     loopStartFrameIdx; // ループ開始アニメーションフフレーム番号
    
    u32                     animType;          // アニメーションの種類(上位16 bit)|
                                               //  アニメーションの要素(下位16 bit)
                                               // （NNSG2dAnimFrameData.pContentの先が変化します）
                                                      
    NNSG2dAnimationPlayMode playMode;          // アニメーションシーケンスの再生方法
    NNSG2dAnimFrameData*    pAnmFrameArray;    // offset form the head of pFrameArray.
    
}NNSG2dAnimSequenceData;





/*---------------------------------------------------------------------------*
  Name:         NNSG2dAnimBankData

  Description:  アニメーションバンク
                複数のアニメーションシーケンスをまとめる概念です。
                通常 アニメーションバンク == アニメーションファイルです。
                
                
 *---------------------------------------------------------------------------*/
typedef struct NNSG2dAnimBankData
{
    
    u16                       numSequences;         // アニメーションシーケンス数
    u16                       numTotalFrames;       // 総アニメーションフレーム数
    NNSG2dAnimSequenceData*   pSequenceArrayHead;   // アニメーションシーケンス配列へのポインタ
    NNSG2dAnimFrameData*      pFrameArrayHead;      // アニメーションフレーム配列へのポインタ
    void*                     pAnimContents;        // アニメーション結果配列へのポインタ
    void*                     pStringBank;          // ストリングバンクへのポインタ（実行時に設定）
    void*                     pExtendedData;        // ライブラリ拡張領域へのポインタ(未使用)

}NNSG2dAnimBankData;






/*---------------------------------------------------------------------------*
  Name:         NNSG2dAnimBankDataBlock

  Description:  アニメーションバンクブロック
                アニメーションバンクを含むバイナリブロックです。
                
                
 *---------------------------------------------------------------------------*/
typedef struct NNSG2dAnimBankDataBlock
{
    NNSG2dBinaryBlockHeader     blockHeader;    // バイナリヘッダ
    NNSG2dAnimBankData          animBankData;   // アニメーションバンク
    
}NNSG2dAnimBankDataBlock;

//------------------------------------------------------------------------------
typedef struct NNSG2dUserExAnimFrameAttr
{
    u32*           pAttr;
    
}NNSG2dUserExAnimFrameAttr;

typedef struct NNSG2dUserExAnimSequenceAttr
{
    u16                          numFrames;
    u16                          pad16;
    u32*                         pAttr;    
    NNSG2dUserExAnimFrameAttr*   pAnmFrmAttrArray;
        
}NNSG2dUserExAnimSequenceAttr;

typedef struct NNSG2dUserExAnimAttrBank
{
    u16                           numSequences; // アニメーションシーケンス数
    u16                           numAttribute; // アトリビュート数：現在は1固定
    NNSG2dUserExAnimSequenceAttr* pAnmSeqAttrArray;   
    
}NNSG2dUserExAnimAttrBank;





//------------------------------------------------------------------------------
// inline functions.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// アニメーション種類取得関連
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
NNS_G2D_INLINE u32 NNSi_G2dMakeAnimType( NNSG2dAnimationType animType, NNSG2dAnimationElement elmType )
{
    return (u32)animType << NNS_G2D_ANIMTYPE_SHIFT | ((u32)elmType & NNS_G2D_ANIMELEM_MASK );
}
//------------------------------------------------------------------------------
NNS_G2D_INLINE NNSG2dAnimationType 
NNSi_G2dGetAnimSequenceAnimType( u32 animType )
{
    return  (NNSG2dAnimationType)( animType >> NNS_G2D_ANIMTYPE_SHIFT );
}
//------------------------------------------------------------------------------
NNS_G2D_INLINE NNSG2dAnimationElement
NNSi_G2dGetAnimSequenceElementType( u32 animType )
{
    return  (NNSG2dAnimationElement)( animType & NNS_G2D_ANIMELEM_MASK );
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE NNSG2dAnimationType 
NNS_G2dGetAnimSequenceAnimType( const NNSG2dAnimSequenceData* pAnimSeq )
{
//    NNS_G2D_NULL_ASSERT( pAnimSeq );
    
    return  NNSi_G2dGetAnimSequenceAnimType( pAnimSeq->animType );
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE NNSG2dAnimationElement
NNS_G2dGetAnimSequenceElementType( const NNSG2dAnimSequenceData* pAnimSeq )
{
//    NNS_G2D_NULL_ASSERT( pAnimSeq );
    
    return  NNSi_G2dGetAnimSequenceElementType( pAnimSeq->animType );
}

//------------------------------------------------------------------------------
// シーケンスの総ビデオフレーム長を計算します。
NNS_G2D_INLINE u32 NNS_G2dCalcAnimSequenceTotalVideoFrames
( 
     const NNSG2dAnimSequenceData* pAnimSeq 
)
{
    u32    i;
    u32    total = 0;
    
    for( i = 0; i < pAnimSeq->numFrames; i++ )
    {
        total += pAnimSeq->pAnmFrameArray[i].frames;
    }
    return total;
}
//------------------------------------------------------------------------------
// アニメーションバンク中のアニメーションシーケンス番号を取得します
// アニメーションシーケンスがアニメーションバンク中に含まれるものであることを期待しています。
// 
#ifndef NNS_FROM_TOOL
NNS_G2D_INLINE u16 NNS_G2dGetAnimSequenceIndex
( 
    const NNSG2dAnimBankData*     pAnimBank,
    const NNSG2dAnimSequenceData* pAnimSeq 
)
{
    return (u16)(
        ((u32)pAnimSeq - (u32)pAnimBank->pSequenceArrayHead) 
        / (u32)sizeof( NNSG2dAnimSequenceData ) );
}
#else
// NNS_G2dGetAnimSequenceIndex() は VC環境では警告が発生するコードなので、定義しません。
#endif

//------------------------------------------------------------------------------
// NNSG2dAnimBankData から NNSG2dUserExAnimAttrBank を取得します
// 取得に失敗した場合は、NULLが返ります。
// ユーザ拡張アトリビュートを含むデータを出力するためには、g2dcvtr.exe 2.8 以降を
// 利用し、-oua オプションを指定してデータを出力する必要があります。
// 
NNS_G2D_INLINE const NNSG2dUserExAnimAttrBank* 
NNS_G2dGetUserExAnimAttrBank( const NNSG2dAnimBankData* pAnimBank )
{
    // ブロックを取得します
    const NNSG2dUserExDataBlock* pBlk 
        = NNSi_G2dGetUserExDataBlkByID( pAnimBank->pExtendedData,
                                        NNS_G2D_USEREXBLK_ANMATTR );
    // ブロックの取得に成功したら...
    if( pBlk != NULL )
    {
        return (const NNSG2dUserExAnimAttrBank*)(pBlk + 1);
    }else{
        return NULL;                                
    }
}

//------------------------------------------------------------------------------
// 番号を指定してアニメーションシーケンスの拡張アトリビュート情報を取得します
// 不正な番号が指定された場合は、NULLを返します。
//
NNS_G2D_INLINE const NNSG2dUserExAnimSequenceAttr* 
NNS_G2dGetUserExAnimSequenceAttr
( 
    const NNSG2dUserExAnimAttrBank* pAnimAttrBank,
    u16                             idx 
)
{
    NNS_G2D_NULL_ASSERT( pAnimAttrBank );
    
    if( idx < pAnimAttrBank->numSequences )
    {
        return &pAnimAttrBank->pAnmSeqAttrArray[idx];
    }else{
        return NULL;
    }
}

//------------------------------------------------------------------------------
// 番号を指定してアニメーションシーケンスの拡張アトリビュート情報を取得します
// 不正な番号が指定された場合は、NULLを返します。
//
NNS_G2D_INLINE const NNSG2dUserExAnimFrameAttr* 
NNS_G2dGetUserExAnimFrameAttr
( 
    const NNSG2dUserExAnimSequenceAttr* pAnimSeqAttr,
    u16                                 idx 
)
{
    NNS_G2D_NULL_ASSERT( pAnimSeqAttr );
    
    if( pAnimSeqAttr->numFrames )
    {
        return &pAnimSeqAttr->pAnmFrmAttrArray[idx];            
    }else{
        return NULL;
    }   
}
//------------------------------------------------------------------------------
// アニメーションシーケンスの
// アトリビュート値を取得します
NNS_G2D_INLINE u32
NNS_G2dGetUserExAnimSeqAttrValue
( 
    const NNSG2dUserExAnimSequenceAttr* pAnimSeqAttr
)
{
    NNS_G2D_NULL_ASSERT( pAnimSeqAttr );
    return pAnimSeqAttr->pAttr[0];
}

//------------------------------------------------------------------------------
// アニメーションフレームの
// アトリビュート値を取得します
NNS_G2D_INLINE u32
NNS_G2dGetUserExAnimFrmAttrValue
( 
    const NNSG2dUserExAnimFrameAttr* pFrmAttr
)
{
    NNS_G2D_NULL_ASSERT( pFrmAttr );
    return pFrmAttr->pAttr[0];
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // NNS_G2D_ANIM_DATA_H_

