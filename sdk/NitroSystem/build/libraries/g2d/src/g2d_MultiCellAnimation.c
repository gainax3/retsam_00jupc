/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - g2d
  File:     g2d_MultiCellAnimation.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.45 $
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include <nnsys/g2d/g2d_MultiCellAnimation.h>
#include <nnsys/g2d/g2d_SRTControl.h>

#include <nnsys/g2d/load/g2d_NMC_load.h>
#include <nnsys/g2d/load/g2d_NAN_load.h>


//------------------------------------------------------------------------------
// セルアニメーションにノード情報を設定します。
static NNS_G2D_INLINE void SetNodeDataToCellAnim_( 
    NNSG2dCellAnimation*                  pCellAnim,
    const NNSG2dMultiCellHierarchyData*   pNodeData, 
    const NNSG2dAnimBankData*             pAnimBank,
    u16                                   mcTotalFrame 
)
{
    const NNSG2dAnimSequenceData*  pAnimSeq     = NULL;
    
    NNS_G2D_NULL_ASSERT( pCellAnim );
    NNS_G2D_NULL_ASSERT( pNodeData );
    NNS_G2D_NULL_ASSERT( pAnimBank );
    
    
    // アニメーションの実体を取得
    pAnimSeq = NNS_G2dGetAnimSequenceByIdx( pAnimBank, pNodeData->animSequenceIdx );
    
    //
    // NNSG2dNode設定データを反映する
    // 現在の表示アニメーションフレームはシーケンス先頭へとリセットされる
    //
    NNS_G2dSetCellAnimationSequence( pCellAnim, pAnimSeq );            
    // 再生をスタートします
    NNS_G2dStartAnimCtrl( NNS_G2dGetCellAnimationAnimCtrl( pCellAnim ) );
    
                
    //
    // セルアニメーションの再生方式が
    // 連続再生方式に設定されているか？
    //
    if( NNSi_G2dGetMultiCellNodePlayMode( pNodeData ) == NNS_G2D_MCANIM_PLAYMODE_CONTINUE )
    {
        // セルアニメーションの再生位置は
        // NNS_G2dSetCellAnimationSequence() の実行によって
        // 一旦アニメーションシーケンスの先頭までリセットされます。
        // ここでは、適切な位置まで、セルアニメーションの再生位置を更新します。
        const u32 animSeqLength = NNS_G2dCalcAnimSequenceTotalVideoFrames( pAnimSeq );
        
        // ループ再生が指定されているか？
        // 指定されていない場合は、最初の一回だけ、
        // アニメーションシーケンスを再生した後アニメーションをアニメ終端で停止する必要がある
        // 
        if( NNSi_G2dIsAnimCtrlLoopAnim( NNS_G2dGetCellAnimationAnimCtrl( pCellAnim ) ) )
        {
           const u32 frameToMove   = ( mcTotalFrame % animSeqLength );
           NNS_G2dTickCellAnimation( pCellAnim, (fx32)( frameToMove  << FX32_SHIFT ) );                            
        }else{
           const u32 frameToMove   = ( mcTotalFrame >= animSeqLength ) ? animSeqLength : mcTotalFrame;
           NNS_G2dTickCellAnimation( pCellAnim, (fx32)( frameToMove  << FX32_SHIFT ) );        
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         SetMCDataToMCInstanceImpl_

  Description:  マルチセル実体にマルチセルデータを設定します。
                初期化後に呼ばれる必要があります
                マルチセル実体がマルチセルデータを反映するのに十分なNodeを保有し
                ている必要があります。
                
                注意：
                ･以前のバージョンでは、pMCInst->numNode と pMcData->numNodes を比較し、
                 条件を満たさない場合は処理を行わずにFASLEを返す仕様でした。
                 しかし、そのような仕様は、実用上ではあまり意味がないと判断され、
                 マルチセルアニメーション実体が、必ず前述の条件を満たすものと仮定して、
                 処理を行うように変更しました。
                 このため、ユーザは条件をみたすことを保障する必要があります。
                
                ･マルチセル実体は初期化時に指定される、NNSG2dMultiCellInstance.mcType
                 メンバによって内部構造が大きく異なります。
                 そのため、本関数でも処理が大きく2系統に分岐しています。
                
                
  Arguments:    pMultiCellAnim   :        マルチセルアニメーション実体
                pMcData          :        マルチセルデータ
                mcTotalFrame     :        アニメーション再生総ビデオフレーム長  
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/ 
//
// 本関数はプライベート関数に変更されました。
// 過去のバージョンとの互換性維持のために、旧APIは残されています。
//
static void SetMCDataToMCInstanceImpl_
( 
    NNSG2dMultiCellInstance*       pMCInst, 
    const NNSG2dMultiCellData*     pMcData,
    u16                            mcTotalFrame 
)
{
    u16 i;
    
    NNS_G2D_NULL_ASSERT( pMCInst );
    NNS_G2D_NULL_ASSERT( pMCInst->pAnimDataBank );
    NNS_G2D_NULL_ASSERT( pMcData );
    
    pMCInst->pCurrentMultiCell = pMcData;
    
    //
    // マルチセル実体の種類は?
    //  マルチセル実体は初期化時に指定される、NNSG2dMultiCellInstance.mcType
    //  メンバによって内部構造が大きく異なります。
    //  特に、pCellAnim の取得の仕方が異なります。
    //
    if( pMCInst->mcType == NNS_G2D_MCTYPE_SHARE_CELLANIM )
    {
        
        NNSG2dMCCellAnimation* pCellAnimArray = (NNSG2dMCCellAnimation*)pMCInst->pCellAnimInstasnces;
        
        //
        // 初期化済みフラグをリセットします
        //
        for( i = 0; i < pMcData->numCellAnim; i++ )
        {
            pCellAnimArray[i].bInited = FALSE;
        }
        
        // すべてのNNSG2dNodeに対して...
        for( i = 0; i < pMcData->numNodes; i++ )
        {
            // 
            // TODO:nodeAttrから取得する、コンバータでデータを挿入する
            //
            const NNSG2dMultiCellHierarchyData* pNodeData    = &pMcData->pHierDataArray[i];
            const u16                           cellAnimIdx  = NNSi_G2dGetMC2NodeCellAinmIdx( pNodeData );
            
            // 初期化済みではないか？
            // 複数のノードによって共有されるセルアニメーションが存在する場合に
            // 有効となる、条件判断です。
            if( !pCellAnimArray[cellAnimIdx].bInited )
            {
                NNSG2dCellAnimation*          pCellAnim = &pCellAnimArray[cellAnimIdx].cellAnim;
                
                SetNodeDataToCellAnim_( pCellAnim, 
                                        pNodeData, 
                                        pMCInst->pAnimDataBank,
                                        mcTotalFrame );
                  
                pCellAnimArray[cellAnimIdx].bInited = TRUE;
            }
        }
        
    }else{
       
       NNSG2dNode*   pNodeArray   = (NNSG2dNode*)(pMCInst->pCellAnimInstasnces);
       
       
       // すべてのNNSG2dNodeに対して...
       for( i = 0; i < pMcData->numNodes; i++ )
       {
           const NNSG2dMultiCellHierarchyData* pNodeData    = &pMcData->pHierDataArray[i];
           NNSG2dCellAnimation*  pCellAnim    = (NNSG2dCellAnimation*)pNodeArray[i].pContent;
                  
           SetNodeDataToCellAnim_( pCellAnim, 
                                   pNodeData, 
                                   pMCInst->pAnimDataBank,
                                   mcTotalFrame );
               
           pNodeArray[i].bVisible = TRUE;                          
           NNSi_G2dSrtcSetTrans( &pNodeArray[i].srtCtrl, pNodeData->posX, pNodeData->posY );             
       }
    }
}


//------------------------------------------------------------------------------
static NNS_G2D_INLINE void ApplyCurrentAnimResult_( NNSG2dMultiCellAnimation* pMultiCellAnim )
{
    //
    // 少し乱暴だが、どんなアニメフォーマットであってもNNSG2dAnimDataSRTでアクセスしてしまう
    // (アニメフォーマットがNNS_G2D_ANIMELEMENT_INDEXの場合は SRT 部分には不正なデータが入る事となる）
    // 
    const NNSG2dAnimDataSRT*      pAnimResult  = NULL;
    const NNSG2dMultiCellData*    pData        = NULL;
    
    
    NNS_G2D_NULL_ASSERT( pMultiCellAnim );
    NNS_G2D_NULL_ASSERT( pMultiCellAnim->pMultiCellDataBank );
    
    //
    // 表示時間がゼロである、アニメーションフレームの場合は、更新作業を何も行いません。
    //
    if( pMultiCellAnim->animCtrl.pActiveCurrent->frames == 0 )
    {
        return; 
    }
    pAnimResult = (const NNSG2dAnimDataSRT*)NNS_G2dGetAnimCtrlCurrentElement( &pMultiCellAnim->animCtrl );
    NNS_G2D_NULL_ASSERT( pAnimResult );

    
    pData  
        = NNS_G2dGetMultiCellDataByIdx( pMultiCellAnim->pMultiCellDataBank, 
                                        pAnimResult->index );
    NNS_G2D_NULL_ASSERT( pData );
    
    //
    // SRT を 反映 
    //
    {
       const NNSG2dAnimationElement elemType 
           = NNSi_G2dGetAnimSequenceElementType( pMultiCellAnim->animCtrl.pAnimSequence->animType );
       NNSi_G2dSrtcInitControl  ( &pMultiCellAnim->srtCtrl, NNS_G2D_SRTCONTROLTYPE_SRT );
       
       if( elemType != NNS_G2D_ANIMELEMENT_INDEX )
       {
           if( elemType == NNS_G2D_ANIMELEMENT_INDEX_T )
           {
              const NNSG2dAnimDataT*  pAnmResT = (const NNSG2dAnimDataT*)pAnimResult;
              NNSi_G2dSrtcSetTrans     ( &pMultiCellAnim->srtCtrl, pAnmResT->px, pAnmResT->py );// T
           }else{
              NNSi_G2dSrtcSetSRTScale  ( &pMultiCellAnim->srtCtrl, pAnimResult->sx, pAnimResult->sy );// S    
              NNSi_G2dSrtcSetSRTRotZ   ( &pMultiCellAnim->srtCtrl, pAnimResult->rotZ );// R    
              NNSi_G2dSrtcSetTrans     ( &pMultiCellAnim->srtCtrl, pAnimResult->px, pAnimResult->py );// T    
           }
       }
       
    }
    
    //
    // マルチセルの設定
    //    
    SetMCDataToMCInstanceImpl_( &pMultiCellAnim->multiCellInstance, 
                                pData, 
                                pMultiCellAnim->totalVideoFrame );
    
    NNS_G2D_NULL_ASSERT( pMultiCellAnim->multiCellInstance.pCurrentMultiCell );
}



//------------------------------------------------------------------------------
static NNS_G2D_INLINE void FVecToSVec( const NNSG2dFVec2* pvSrc, NNSG2dSVec2* pvDst )
{
    pvDst->x = (s16)(pvSrc->x >> FX32_SHIFT);
    pvDst->y = (s16)(pvSrc->y >> FX32_SHIFT);
}

//------------------------------------------------------------------------------
static NNS_G2D_INLINE void SVecToFVec( const NNSG2dSVec2* pvSrc, NNSG2dFVec2* pvDst )
{
    pvDst->x = pvSrc->x << FX32_SHIFT;
    pvDst->y = pvSrc->y << FX32_SHIFT;
}

//------------------------------------------------------------------------------
static NNS_G2D_INLINE void AddSVec_( const NNSG2dSVec2* pv1, const NNSG2dSVec2* pv2, NNSG2dSVec2* pvDst )
{
    pvDst->x = (s16)(pv1->x + pv2->x);
    pvDst->y = (s16)(pv1->y + pv2->y);
}

//------------------------------------------------------------------------------
//
static u16 GetMCBankNumCellAnimRequired_
( 
    const NNSG2dMultiCellDataBank*       pMultiCellDataBank 
)
{
    NNS_G2D_NULL_ASSERT( pMultiCellDataBank );    
    {   
        const NNSG2dMultiCellData*  pMCell    = NULL;
        u16                         maxNum    = 0;
        u16                         i;
        
        for( i = 0; i < pMultiCellDataBank->numMultiCellData; i++ )
        {
            pMCell = NNS_G2dGetMultiCellDataByIdx( pMultiCellDataBank, i );
            NNS_G2D_NULL_ASSERT( pMCell );
            
            if( pMCell->numCellAnim > maxNum )
            {
                maxNum = pMCell->numCellAnim;
            }
        }
        return maxNum;
    }
}

//------------------------------------------------------------------------------
static u16 MakeCellAnimToOams_
(
    GXOamAttr*                   pDstOams, 
    u16                          numDstOams,
    const NNSG2dCellAnimation*   pCellAnim,
    const NNSG2dSVec2*           pNodeTrans,
    const MtxFx22*               pMtxSR, 
    const NNSG2dFVec2*           pBaseTrans,
    u16                          affineIndex,
    BOOL                         bDoubleAffine
)
{
    NNSG2dFVec2                       vTransF;
    NNSG2dSVec2                       vTransS;
    const NNSG2dSRTControl*           pContentsSRT;
    
    pContentsSRT = &pCellAnim->srtCtrl;
   
    // trans
    {
        // vTransS = pContentsSRT->srtData.trans + pNode->srtCtrl.srtData.trans
        AddSVec_( &pContentsSRT->srtData.trans, 
                   pNodeTrans, 
                  &vTransS );
        
        
        // pMtxSR が存在すれば Node の Trans を変換する
        if( pMtxSR != NULL )
        {
            vTransF.x = pMtxSR->_00 * vTransS.x + pMtxSR->_10 * vTransS.y;
            vTransF.y = pMtxSR->_01 * vTransS.x + pMtxSR->_11 * vTransS.y;  
            
        }else{
            SVecToFVec( &vTransS, &vTransF );
        }
        
        // pBaseTrans が存在するなら、vNodeTransに加える
        if( pBaseTrans != NULL )
        {
            vTransF.x += pBaseTrans->x;
            vTransF.y += pBaseTrans->y;
        }
    }
    
    //
    // 構成要素 が Affine 変換を持たないか？
    // 
    // TODO:これはNNS_G2D_WARNINGとする
    if( NNSi_G2dSrtcIsAffineEnable_SR( pContentsSRT ) )
    {
        // ！警告
        OS_Warning("invalid affine transformation is found in NNS_G2dInitMCAnimation()");
    }
    
    //
    // Cell => Oam へ 変換
    // 
    return NNS_G2dMakeCellToOams( pDstOams,
                                  numDstOams,
                                  NNS_G2dGetCellAnimationCurrentCell( pCellAnim ), 
                                  pMtxSR, 
                                  &vTransF, 
                                  affineIndex,
                                  bDoubleAffine );
}

//------------------------------------------------------------------------------
static NNS_G2D_INLINE u16 MakeSimpleMultiCellToOams_
( 
    GXOamAttr*                      pDstOams, 
    u16                             numDstOams,
    const NNSG2dMultiCellInstance*  pMCellInst, 
    const MtxFx22*                  pMtxSR, 
    const NNSG2dFVec2*              pBaseTrans,
    u16                             affineIndex,
    BOOL                            bDoubleAffine 
)
{
    u16     i           = 0;
    u16     numOamUsed  = 0;
    int     numRestOams = numDstOams;
    NNSG2dNode* pNode   = NULL;
    
    NNS_G2D_NULL_ASSERT( pDstOams );
    NNS_G2D_NULL_ASSERT( pMCellInst );
    NNS_G2D_NULL_ASSERT( pMCellInst->pCurrentMultiCell );
    NNS_G2D_ASSERT( pMCellInst->mcType == NNS_G2D_MCTYPE_DONOT_SHARE_CELLANIM );
    
    
    //
    // すべてのNodeについて...
    //
    pNode = (NNSG2dNode*)pMCellInst->pCellAnimInstasnces;
    for( i = 0; i < pMCellInst->pCurrentMultiCell->numNodes; i++ )
    {
        // バッファ容量が十分ならば...
        if( numRestOams > 0 )
        {
            //
            // Node を Oam Attrsに変換する
            //
            
            //
            // 注意：
            // pNode->type は NNS_G2D_NODETYPE_CELL であると想定されています。
            // ノードの SR パラメータが変更されている、マルチセルの描画はサポートしていません。
            //
            NNS_G2D_WARNING( pNode->type == NNS_G2D_NODETYPE_CELL &&
                            !NNSi_G2dSrtcIsAffineEnable_SR( &pNode->srtCtrl ),
                            "A SR-Transformation of a multicell-node was ignored." );
                            
            numOamUsed = MakeCellAnimToOams_( pDstOams,
                                              (u16)numRestOams,
                                              pNode[i].pContent,
                                              &pNode[i].srtCtrl.srtData.trans,
                                              pMtxSR,
                                              pBaseTrans,
                                              affineIndex,
                                              bDoubleAffine );
            
            // 書き出しバッファ位置を更新する
            numRestOams -= numOamUsed;
            pDstOams    += numOamUsed;    
        }else{
            // ループ終了
            break;
        }
    }
    
    NNS_G2D_ASSERT( numDstOams >= numRestOams );
    return (u16)( numDstOams - numRestOams );
}

// 
// NNS_G2dMakeSimpleMultiCellToOams() 誕生の経緯
// 
// 問題
// 
// MultiCell の Node or NodeContents が Affine変換を持つ場合, 
// 変換途中で 複数の affine 変換行列 が 発生する場合がありうる。
// 
// ①それらの行列をHWに設定しながら、
// ②Oam Attrs に対して ①の結果に応じた affineIndex の設定を行う必要がある。
// 
// ①②の作業とも、クライアントコードにおいて実装をカスタマイズしたいという要望があると予想される。
// 通常、こういったケースでは 関数ポインタで、①②の処理 を外部関数に委譲することとなるが、
// 単純で簡易な描画インタフェースの提供という、本関数の目的を見失ってしまう。
// 



//------------------------------------------------------------------------------
static NNS_G2D_INLINE u16 MakeSimpleMultiCellToOams_ShareCellAnims_
( 
    GXOamAttr*                      pDstOams, 
    u16                             numDstOams,
    const NNSG2dMultiCellInstance*  pMCellInst, 
    const MtxFx22*                  pMtxSR, 
    const NNSG2dFVec2*              pBaseTrans,
    u16                             affineIndex,
    BOOL                            bDoubleAffine 
)
{
    u16     i           = 0;
    u16     numOamUsed  = 0;
    int     numRestOams = numDstOams;
    const NNSG2dMCCellAnimation* pMCCellAnimArray = NULL;
    NNSG2dSVec2       nodeTrans;
    
    NNS_G2D_NULL_ASSERT( pDstOams );
    NNS_G2D_NULL_ASSERT( pMCellInst );
    NNS_G2D_NULL_ASSERT( pMCellInst->pCurrentMultiCell );
    NNS_G2D_ASSERT( pMCellInst->mcType == NNS_G2D_MCTYPE_SHARE_CELLANIM );
    
    pMCCellAnimArray = (const NNSG2dMCCellAnimation*)pMCellInst->pCellAnimInstasnces;
    
    //
    // すべてのNodeについて...
    //
    for( i = 0; i < pMCellInst->pCurrentMultiCell->numNodes; i++ )
    {
        const NNSG2dMultiCellHierarchyData* pNode 
           = &pMCellInst->pCurrentMultiCell->pHierDataArray[i];
        
        //
        // TODO:
        //
        // pNode->animSequenceIdx
        // pNode->nodeAttr に ノードに関連付けられている セルアニメ番号 格納されている  
        //
        //
        const u16 cellAnimIdx  = NNSi_G2dGetMC2NodeCellAinmIdx( pNode );
        
        
           
           
        // バッファ容量が十分ならば...
        if( numRestOams > 0 )
        {
            //
            // Node を Oam Attrsに変換する
            //
            nodeTrans.x = pNode->posX;
            nodeTrans.y = pNode->posY;
            
            numOamUsed = MakeCellAnimToOams_( pDstOams,
                                              (u16)numRestOams,
                                              &pMCCellAnimArray[cellAnimIdx].cellAnim,
                                              &nodeTrans,
                                              pMtxSR,
                                              pBaseTrans,
                                              affineIndex,
                                              bDoubleAffine );
            
            // 書き出しバッファ位置を更新する
            numRestOams -= numOamUsed;
            pDstOams    += numOamUsed;   
        }else{
            // ループ終了
            break;
        }
    }
    
    NNS_G2D_ASSERT( numDstOams >= numRestOams );
    return (u16)( numDstOams - numRestOams );
}

/*---------------------------------------------------------------------------*
  Name:         InitMCInstance_

  Description:  マルチセル実体 を 初期化します。
                過去のバージョンとの互換性維持のために残されています。
                
                
  Arguments:    pMultiCell      :         マルチセル実体
                pNodeArray      :         ノード実体配列( numNode分 )
                pCellAnim       :         セルアニメーション実体配列( numNode分 )
                numNode         :         ノード数
                pAnimBank       :         セルアニメーション定義バンク
                pCellDataBank   :         セルデータバンク
                
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
static NNS_G2D_INLINE void InitMCInstance_
( 
    NNSG2dMultiCellInstance*           pMultiCell, 
    NNSG2dNode*                        pNodeArray, 
    NNSG2dCellAnimation*               pCellAnim, 
    u16                                numNode, 
    const NNSG2dCellAnimBankData*      pAnimBank,  
    const NNSG2dCellDataBank*          pCellDataBank 
)
{
    NNS_G2D_NULL_ASSERT( pMultiCell );
    NNS_G2D_NULL_ASSERT( pNodeArray );
    NNS_G2D_NULL_ASSERT( pAnimBank );
    NNS_G2D_ASSERTMSG( numNode != 0, "Non zero value is expected." );
    
    
    //
    // 本関数を使用した場合は、NNS_G2D_MCTYPE_DONOT_SHARE_CELLANIM 
    // 型の実体として初期化されることになります。
    // NNS_G2D_MCTYPE_DONOT_SHARE_CELLANIM 型の実体のpCellAnimInstasncesメンバは
    // ノード配列の先頭を指し示すようになっています。
    //
    pMultiCell->mcType              = NNS_G2D_MCTYPE_DONOT_SHARE_CELLANIM;
    pMultiCell->pCurrentMultiCell   = NULL;
    pMultiCell->pAnimDataBank       = pAnimBank;
    pMultiCell->pCellAnimInstasnces = (void*)pNodeArray;
    
    {
        u16 i;
        for( i = 0; i < numNode; i++ ) 
        {
            NNSi_G2dInitializeNode( &pNodeArray[i], NNS_G2D_NODETYPE_CELL );
            // TODO:bind は 関数化する
            // bind
            pNodeArray[i].pContent  = &pCellAnim[i];
            
            
            NNS_G2dInitCellAnimation( 
                &pCellAnim[i], 
                NNS_G2dGetAnimSequenceByIdx( pAnimBank, 0 ) , 
                pCellDataBank );
        }
    }
}

//------------------------------------------------------------------------------
// マルチセルアニメーションを初期化します。
// mcType として NNS_G2D_MCTYPE_DONOT_SHARE_CELLANIM を設定した場合に実行されます。
//
// mcType として、NNS_G2D_MCTYPE_DONOT_SHARE_CELLANIMを指定すると
// 過去のバージョンと同一のデータ構造で実体が初期化され、同じように処理されます。
// プログラマがマルチセルの構成ノードにアクセスし、データ等を書き換えるなどの処理を行わない限り、 
// NNS_G2D_MCTYPE_SHARE_CELLANIMを指定して実体を初期化したほうが、メモリ消費量、処理効率の点から有利です。 
//
static NNS_G2D_INLINE void InitMCAnimation_( 
    NNSG2dMultiCellAnimation*          pMultiCellAnim, 
    NNSG2dNode*                        pNodeArray, 
    NNSG2dCellAnimation*               pCellAnim, 
    u16                                numNode, 
    const NNSG2dCellAnimBankData*      pAnimBank,  
    const NNSG2dCellDataBank*          pCellDataBank,
    const NNSG2dMultiCellDataBank*     pMultiCellDataBank 
)
{
#pragma unused( pMultiCellDataBank )
    NNS_G2D_NULL_ASSERT( pMultiCellAnim );
    
    NNS_G2D_NULL_ASSERT( pNodeArray );
    NNS_G2D_NULL_ASSERT( pCellAnim );
    NNS_G2D_ASSERTMSG( numNode != 0, "TODO: msg" );
    
    NNS_G2D_NULL_ASSERT( pAnimBank );
    NNS_G2D_NULL_ASSERT( pCellDataBank );
    
    NNS_G2D_NULL_ASSERT( pMultiCellDataBank );
    
    //
    // マルチセル実体を初期化する
    //
    InitMCInstance_( &pMultiCellAnim->multiCellInstance, 
                            pNodeArray, 
                            pCellAnim, 
                            numNode, 
                            pAnimBank,  
                            pCellDataBank );
    
    NNS_G2dInitAnimCtrl( &pMultiCellAnim->animCtrl );
    pMultiCellAnim->pMultiCellDataBank = pMultiCellDataBank;
    NNSi_G2dSrtcInitControl( &pMultiCellAnim->srtCtrl, NNS_G2D_SRTCONTROLTYPE_SRT );
    pMultiCellAnim->totalVideoFrame = 0;
}

//------------------------------------------------------------------------------
// マルチセルアニメーションを初期化します。
// mcType として NNS_G2D_MCTYPE_SHARE_CELLANIM を設定した場合に実行されます。
// 
// mcTypeとして、NNS_G2D_MCTYPE_SHARE_CELLANIMが指定された場合、
// マルチセル上の複数のノードで同一のアニメーションシーケンス再生を行うようなデータの入力があった場合に、
// セルアニメーションの実体を複数ノード間で共有します。そのため、メモリ消費量や、アニメーション更新の処理負荷が減少します。
//
// また、NNS_G2D_MCTYPE_SHARE_CELLANIMが指定された場合、マルチセルのランタイム実体はノードに相当する情報を保持しません。
// そのため、ノード単位の位置の変更、アフィン変換、がサポートされません。
// ただし、上記制限によって同一セルアニメーションを再生するノードでは必ず同一のアフィンパラメータを参照することが保障されるため、
// 特に2D グラフィックスエンジンを使用した描画の際に、NNS_G2D_MCTYPE_DONOT_SHARE_CELLANIM を指定した場合に比べて、
// よりハードウェア資源(＝使用するアフィンパラメータ)を節約することが可能です。 
//
static NNS_G2D_INLINE void InitMCAnimation_SharingCellAnim_( 
    NNSG2dMultiCellAnimation*          pMultiCellAnim,  
    void*                              pWork, 
    const NNSG2dCellAnimBankData*      pAnimBank,  
    const NNSG2dCellDataBank*          pCellDataBank,
    const NNSG2dMultiCellDataBank*     pMultiCellDataBank 
)
{
    NNS_G2D_NULL_ASSERT( pMultiCellAnim );
    NNS_G2D_NULL_ASSERT( pWork );    
    NNS_G2D_NULL_ASSERT( pAnimBank );
    NNS_G2D_NULL_ASSERT( pCellDataBank );
    NNS_G2D_NULL_ASSERT( pMultiCellDataBank );
    
    //
    // マルチセル実体を初期化する
    //
    pMultiCellAnim->multiCellInstance.mcType = NNS_G2D_MCTYPE_SHARE_CELLANIM;
    {
       NNSG2dMultiCellInstance* pMCInst = &pMultiCellAnim->multiCellInstance;
       
       pMCInst->pAnimDataBank = pAnimBank;
       // ワークメモリを初期化します
       // ワークメモリをNNSG2dMCCellAnimationとして使用します
       pMCInst->pCellAnimInstasnces = (void*)pWork;

       {
           u16 i;
           const u16 numCellAnim 
              = GetMCBankNumCellAnimRequired_( pMultiCellDataBank );
           NNSG2dMCCellAnimation* pMCCellAnim 
              = (NNSG2dMCCellAnimation*)pMCInst->pCellAnimInstasnces;
            
           //
           // すべてのセルアニメーションについて
           // とりあえず適当なアニメーションを割り当てておく
           //   
           for( i = 0; i < numCellAnim; i++ ) 
           {
              NNS_G2dInitCellAnimation( 
                  &pMCCellAnim[i].cellAnim, 
                  NNS_G2dGetAnimSequenceByIdx( pAnimBank, 0 ) , 
                  pCellDataBank );
                    
              pMCCellAnim[i].bInited = TRUE;
           }
       }
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetAnimSequenceToMCAnimation

  Description:  マルチセルアニメーション実体 に 再生アニメを設定します
                初期化後に呼ばれる必要があります
                
  Arguments:    pMultiCellAnim   :         [OUT] マルチセルアニメーション実体
                pAnimSeq         :         [IN]  再生アニメ
                
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
// TODO:各種リソースが正しい状態かチェックする関数群 それらをASSERT内で実行
void NNS_G2dSetAnimSequenceToMCAnimation
( 
    NNSG2dMultiCellAnimation*         pMultiCellAnim, 
    const NNSG2dAnimSequence*         pAnimSeq
)
{
    NNS_G2D_NULL_ASSERT( pMultiCellAnim );
    NNS_G2D_NULL_ASSERT( pMultiCellAnim->pMultiCellDataBank );
    
    NNS_G2D_NULL_ASSERT( pAnimSeq );
    NNS_G2D_ASSERTMSG( NNS_G2dGetAnimSequenceAnimType( pAnimSeq ) 
                   == NNS_G2D_ANIMATIONTYPE_MULTICELLLOCATION, 
                    "NNSG2dAnimationType must be MultiCellLocation." );
    
    NNS_G2dBindAnimCtrl( &pMultiCellAnim->animCtrl, pAnimSeq );
    
    // マルチセルの再生総フレーム時間はリセットされます。    
    pMultiCellAnim->totalVideoFrame = 0;
    
    ApplyCurrentAnimResult_( pMultiCellAnim );
}




/*---------------------------------------------------------------------------*
  Name:         NNS_G2dInitMCAnimationInstance

  Description:  NNSG2dMC2Animation  実体を初期化します
  
                本関数は  NNS_G2dInitMCAnimation(), 
                         NNS_G2dInitMCInstance(), 
                         NNS_G2dSetMCDataToMCInstance() の代替関数として用意されました。
                以前の関数は、互換性維持のため別名として残されています。

                新規にご利用のかたは、こちらの関数をご利用ください。 
                
  Arguments:    pMultiCellAnim      [OUT] マルチセルアニメーション 
                pWork               [IN]  マルチセル実体の使用するワーク領域
                pAnimBank           [IN]  マルチセルを構成する セルアニメーションが定義されている アニメバンク 
                pCellDataBank       [IN]  セルデータバンク 
                pMultiCellDataBank  [IN]  マルチセルデータバンク 
                mcType              [IN]  マルチセル実体の種類（NNSG2dMCType型）

  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dInitMCAnimationInstance
( 
    NNSG2dMultiCellAnimation*          pMultiCellAnim,  
    void*                              pWork, 
    const NNSG2dCellAnimBankData*      pAnimBank,  
    const NNSG2dCellDataBank*          pCellDataBank,
    const NNSG2dMultiCellDataBank*     pMultiCellDataBank,
    NNSG2dMCType                       mcType
)
{
    NNS_G2D_NULL_ASSERT( pMultiCellAnim );
    NNS_G2D_NULL_ASSERT( pWork );    
    NNS_G2D_NULL_ASSERT( pAnimBank );
    NNS_G2D_NULL_ASSERT( pCellDataBank );
    NNS_G2D_NULL_ASSERT( pMultiCellDataBank );
    
    
    //
    // 実体の種類ごとの初期化
    //
    if( mcType == NNS_G2D_MCTYPE_SHARE_CELLANIM )
    {
       InitMCAnimation_SharingCellAnim_( pMultiCellAnim,
                                         pWork,
                                         pAnimBank,
                                         pCellDataBank,
                                         pMultiCellDataBank );
    }else{
       const u16 numNode 
           = NNS_G2dGetMCBankNumNodesRequired( pMultiCellDataBank );
       NNSG2dNode* pNodeArray 
           = pWork;
       NNSG2dCellAnimation* pCellAnimArray 
           = (NNSG2dCellAnimation*)(pNodeArray + numNode);
                 
       InitMCAnimation_( pMultiCellAnim, 
                         pNodeArray, 
                         pCellAnimArray, 
                         numNode, 
                         pAnimBank,  
                         pCellDataBank,
                         pMultiCellDataBank );
    }
    
    //
    // 共通する部分の初期化
    //
    NNS_G2dInitAnimCtrl( &pMultiCellAnim->animCtrl );
    pMultiCellAnim->pMultiCellDataBank = pMultiCellDataBank;
    NNSi_G2dSrtcInitControl( &pMultiCellAnim->srtCtrl, NNS_G2D_SRTCONTROLTYPE_SRT );
    pMultiCellAnim->totalVideoFrame = 0;
    
}






/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetMCNumNodesRequired

  Description:  NNSG2dMultiCellAnimSequence 中で 必要となる 最大 NNSG2dNode数を取得します
                
                新しい初期化API NNS_G2dInitMCAnimationInstance() が追加されたため、
                現在は使用する必要がなくなっています。
                
  Arguments:    pMultiCellSeq:          [IN]  マルチセルアニメーション
                pMultiCellDataBank:     [IN]  マルチセルデータバンク
                
                
  Returns:      マルチセルアニメーション中で 必要となる 最大 NNSG2dNode数
  
 *---------------------------------------------------------------------------*/
u16 NNS_G2dGetMCNumNodesRequired
( 
    const NNSG2dMultiCellAnimSequence*   pMultiCellSeq, 
    const NNSG2dMultiCellDataBank*       pMultiCellDataBank 
)
{
    NNS_G2D_NULL_ASSERT( pMultiCellSeq );
    NNS_G2D_NULL_ASSERT( pMultiCellDataBank );
    NNS_G2D_ASSERTMSG( NNS_G2dGetAnimSequenceAnimType( pMultiCellSeq )
                   == NNS_G2D_ANIMATIONTYPE_MULTICELLLOCATION, 
                    "NNSG2dAnimationType must be MultiCellLocation." );
    
    {   
        const NNSG2dMultiCellData*  pMCell        = NULL;
        u16                         maxNumNode    = 0;
        
        //
        // アニメーションを構成する一連のマルチセル中から 最大NNSG2dNode数を 探す
        // 
        u16     i;
        const NNSG2dAnimDataSRT*    pAnmFrm;
        for( i = 0; i < pMultiCellSeq->numFrames; i++ )
        {
            pAnmFrm = (const NNSG2dAnimDataSRT*)
               pMultiCellSeq->pAnmFrameArray[i].pContent;
            
            
            pMCell = NNS_G2dGetMultiCellDataByIdx( pMultiCellDataBank, 
                                                   pAnmFrm->index );
            NNS_G2D_NULL_ASSERT( pMCell );
            
            
            if( pMCell->numNodes > maxNumNode )
            {
                maxNumNode = pMCell->numNodes;
            }
        }
        return maxNumNode;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetMCWorkAreaSize

  Description:  マルチセル実体を初期化する際に必要となるワークメモリ領域のサイズを取得します。
                NNS_G2dGetMCNumNodesRequired
                NNS_G2dGetMCBankNumNodesRequired
                NNS_G2dGetMCBankNumCellAnimsRequired
                の代わりとして、ワークサイズが何バイト必要かを取得する関数です。
                
  Arguments:    pMultiCellDataBank:     [IN]  マルチセルデータバンク
                mcType:                 [IN]  マルチセル実体の種類
                
  Returns:      マルチセルデータバンク中で 必要となる 最大 NNSG2dNode数
  
 *---------------------------------------------------------------------------*/
u32 NNS_G2dGetMCWorkAreaSize
(
    const NNSG2dMultiCellDataBank*       pMultiCellDataBank,
    NNSG2dMCType                         mcType
)
{
    if( mcType == NNS_G2D_MCTYPE_SHARE_CELLANIM )
    {
       return sizeof( NNSG2dMCCellAnimation ) 
           * GetMCBankNumCellAnimRequired_( pMultiCellDataBank );
    }else{
       return ( sizeof( NNSG2dNode ) + sizeof( NNSG2dCellAnimation ) ) 
           * NNS_G2dGetMCBankNumNodesRequired( pMultiCellDataBank );
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetMCBankNumNodesRequired

  Description:  NNSG2dMultiCellDataBank 中で 必要となる 最大 NNSG2dNode数を取得します
                
                新しい初期化API NNS_G2dInitMCAnimationInstance() が追加されたため、
                現在は使用する必要がなくなっています。
                
  Arguments:    pMultiCellDataBank:     [IN]  マルチセルデータバンク
                
                
  Returns:      マルチセルデータバンク中で 必要となる 最大 NNSG2dNode数
  
 *---------------------------------------------------------------------------*/
u16 NNS_G2dGetMCBankNumNodesRequired
( 
    const NNSG2dMultiCellDataBank*       pMultiCellDataBank 
)
{
    NNS_G2D_NULL_ASSERT( pMultiCellDataBank );    
    {   
        const NNSG2dMultiCellData*  pMCell        = NULL;
        u16                         maxNumNode    = 0;
        u16                         i;
        
        for( i = 0; i < pMultiCellDataBank->numMultiCellData; i++ )
        {
            pMCell = NNS_G2dGetMultiCellDataByIdx( pMultiCellDataBank, i );
            NNS_G2D_NULL_ASSERT( pMCell );
            
            if( pMCell->numNodes > maxNumNode )
            {
                maxNumNode = pMCell->numNodes;
            }
        }
        return maxNumNode;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTickMCInstance

  Description:  マルチセルを構成する要素(セルアニメ等) の 時間を進めます
                
  Arguments:    pMultiCellAnim:     [OUT] マルチセル実体
                frames:             [IN]  進める時間（フレーム）
                
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dTickMCInstance( NNSG2dMultiCellInstance* pMCellInst, fx32 frames )
{

    if( pMCellInst->mcType == NNS_G2D_MCTYPE_SHARE_CELLANIM )
    {
       u16 i;
       NNSG2dMCCellAnimation* pCellAnimArray = pMCellInst->pCellAnimInstasnces;
       
       for( i = 0; i < pMCellInst->pCurrentMultiCell->numCellAnim; i++ )
       {
           NNS_G2dTickCellAnimation( &pCellAnimArray[i].cellAnim, frames );
       }
    }else{    
       u16 i;
       NNSG2dNode*   pNodeArray = NULL;
        
       NNS_G2D_NULL_ASSERT( pMCellInst );
       NNS_G2D_NULL_ASSERT( pMCellInst->pCurrentMultiCell );
        
       pNodeArray = (NNSG2dNode*)(pMCellInst->pCellAnimInstasnces);
        
       for( i = 0; i < pMCellInst->pCurrentMultiCell->numNodes; i++ )
       {
            
           // 現状では NNS_G2D_NODETYPE_CELL のみ対応
           NNS_G2D_ASSERT( pNodeArray[i].type == NNS_G2D_NODETYPE_CELL );
           NNS_G2dTickCellAnimation( (NNSG2dCellAnimation*)pNodeArray[i].pContent, frames );
       }
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTickMCAnimation

  Description:  NNSG2dMultiCellAnimation の 時間を進めます
                
  Arguments:    pMultiCellAnim:     [OUT] マルチセル実体
                frames:             [IN]  進める時間（フレーム）
                
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dTickMCAnimation( NNSG2dMultiCellAnimation* pMultiCellAnim, fx32 frames )
{
    NNS_G2D_NULL_ASSERT( pMultiCellAnim );
    
    {
        const u16 currentAnimFrameFrames = pMultiCellAnim->animCtrl.pCurrent->frames;
        
        if( NNS_G2dTickAnimCtrl( &pMultiCellAnim->animCtrl, frames ) )
        {
           pMultiCellAnim->totalVideoFrame += currentAnimFrameFrames;
           //
           // Track更新が起きたなら、アニメーション結果を反映する
           //
           ApplyCurrentAnimResult_( pMultiCellAnim );
        }else{
           // マルチセル内のセルアニメーションを更新する
           // マルチセルアニメーションフレームの切り替えが起きたフレームでは
           // 実行されない点に注意！
           NNS_G2dTickMCInstance( &pMultiCellAnim->multiCellInstance, frames );
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetMCAnimationCurrentFrame

  Description:  NNS_G2dSetMCAnimation の 再生アニメフレームを設定します
                
  Arguments:    pMultiCellAnim:     [OUT] マルチセル実体
                frameIndex:         [IN]  設定フレーム番号
                
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetMCAnimationCurrentFrame
( 
    NNSG2dMultiCellAnimation*   pMultiCellAnim, 
    u16                         frameIndex 
)
{
    NNS_G2D_NULL_ASSERT( pMultiCellAnim );
    
    if( NNS_G2dSetAnimCtrlCurrentFrame( &pMultiCellAnim->animCtrl, frameIndex ) )
    {
        //
        // Track更新が起きたなら、アニメーション結果を反映する
        //
        ApplyCurrentAnimResult_( pMultiCellAnim );
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetMCAnimationCellAnimFrame

  Description:  
                マルチセルを構成する、
                各セルアニメーションについてアニメーションフレームを設定します。
                
                
  Arguments:    pMultiCellAnim:         [OUT] マルチセル実体
                caFrameIndex:           [IN]  セルアニメフレーム番号    
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetMCAnimationCellAnimFrame
( 
    NNSG2dMultiCellAnimation*   pMultiCellAnim, 
    u16                         caFrameIndex     
)
{
    u16 i;
    NNSG2dMultiCellInstance* pMCInst = NULL;
        
    NNS_G2D_NULL_ASSERT( pMultiCellAnim );            
    //
    // すべてのセルアニメーションに対して、アニメーションフレームを設定します       
    // 
    pMCInst = &pMultiCellAnim->multiCellInstance;
    
    // 初期化されるときのパラメータによってデータ構造が異なります
    if( pMCInst->mcType == NNS_G2D_MCTYPE_SHARE_CELLANIM )
    {
        NNSG2dMCCellAnimation* pCellAnimArray 
           = (NNSG2dMCCellAnimation*)pMCInst->pCellAnimInstasnces;
        const int numCellAnm = pMCInst->pCurrentMultiCell->numCellAnim;
                      
        for( i = 0; i < numCellAnm; i++ )
        {
           NNSG2dCellAnimation* pCell 
           = &pCellAnimArray[i].cellAnim;
           NNS_G2dSetCellAnimationCurrentFrame( pCell, caFrameIndex );
        }
    }else{
        NNSG2dNode*   pNodeArray   
           = (NNSG2dNode*)(pMCInst->pCellAnimInstasnces);
        const int numCellAnm = pMCInst->pCurrentMultiCell->numNodes;
           
        for( i = 0; i < numCellAnm; i++ )
        {
           NNSG2dCellAnimation* pCell 
           = (NNSG2dCellAnimation*)pNodeArray[i].pContent;
           NNS_G2dSetCellAnimationCurrentFrame( pCell, caFrameIndex );
        }
    }
}




/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetMCAnimationSpeed

  Description:  NNS_G2dSetMCAnimation の 再生アニメスピードを設定します
                
  Arguments:    pMultiCellAnim:     [OUT] マルチセル実体
                speed:              [IN]  アニメスピード
                
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetMCAnimationSpeed
(
    NNSG2dMultiCellAnimation*   pMultiCellAnim, 
    fx32                        speed  
)
{
    NNS_G2D_NULL_ASSERT( pMultiCellAnim );
    {
       NNSG2dMultiCellInstance* pMCellInst     = &pMultiCellAnim->multiCellInstance;
        
       if( pMCellInst->mcType == NNS_G2D_MCTYPE_SHARE_CELLANIM )
       {
           u16 i;
           
           NNSG2dMCCellAnimation*     pCellAnimArray = pMCellInst->pCellAnimInstasnces;
           NNS_G2dSetAnimCtrlSpeed( &pMultiCellAnim->animCtrl, speed );
           
           for( i = 0; i < pMCellInst->pCurrentMultiCell->numCellAnim; i++ )
           {
              NNS_G2dSetCellAnimationSpeed( &pCellAnimArray[i].cellAnim, speed );
           }
           
       }else{    
           u16 i = 0;
           NNSG2dNode*              pNode      = (NNSG2dNode*)pMultiCellAnim->multiCellInstance.pCellAnimInstasnces;
           
           NNS_G2D_NULL_ASSERT( pMultiCellAnim );
            
            
           NNS_G2dSetAnimCtrlSpeed( &pMultiCellAnim->animCtrl, speed );
            
           // Cellアニメのスピードも変更する
           for( i = 0; i < pMCellInst->pCurrentMultiCell->numNodes; i++ )
           {
              // 現状では NNS_G2D_NODETYPE_CELL のみ対応
              NNS_G2D_ASSERT( pNode[i].type == NNS_G2D_NODETYPE_CELL );
              NNS_G2dSetCellAnimationSpeed( (NNSG2dCellAnimation*)pNode[i].pContent, speed );
           }
       }
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dResetMCCellAnimationAll

  Description:  マルチセルアニメーション中のセルアニメーションの再生シーケンスをゼロに設定します。
                再生停止状態になっている、セルアニメーションの再生再開はおこないません。
                
                
  Arguments:    pMultiCellAnim:      [OUT] マルチセル実体
                
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dResetMCCellAnimationAll
(
    NNSG2dMultiCellInstance*    pMCInst 
)
{
    u16 i = 0;
    NNS_G2D_NULL_ASSERT( pMCInst );
    {
       if( pMCInst->mcType == NNS_G2D_MCTYPE_SHARE_CELLANIM )
       {
           NNSG2dMCCellAnimation*     pCellAnimArray = pMCInst->pCellAnimInstasnces;
           
           for( i = 0; i < pMCInst->pCurrentMultiCell->numCellAnim; i++ )
           {
              NNS_G2dSetCellAnimationCurrentFrame( &pCellAnimArray[i].cellAnim, 0 );
           }
           
       }else{    
           
           NNSG2dNode*              pNode      = (NNSG2dNode*)pMCInst->pCellAnimInstasnces;
                 
           
           for( i = 0; i < pMCInst->pCurrentMultiCell->numNodes; i++ )
           {
              // 現状では NNS_G2D_NODETYPE_CELL のみ対応
              NNS_G2D_ASSERT( pNode[i].type == NNS_G2D_NODETYPE_CELL );
              NNS_G2dSetCellAnimationCurrentFrame( (NNSG2dCellAnimation*)pNode[i].pContent, 0 );
           }
       }
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dStartMCCellAnimationAll

  Description:  マルチセルアニメーション中のすべてのセルアニメーションの再生をスタートします。
                
                
  Arguments:    pMultiCellAnim:      [OUT] マルチセル実体
                
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dStartMCCellAnimationAll
(
    NNSG2dMultiCellInstance*    pMCInst 
)
{
    u16 i = 0;
    NNSG2dAnimController*    pAnmCtrl   = NULL;
    NNS_G2D_NULL_ASSERT( pMCInst );
    {
       if( pMCInst->mcType == NNS_G2D_MCTYPE_SHARE_CELLANIM )
       {
           NNSG2dMCCellAnimation*     pCellAnimArray = pMCInst->pCellAnimInstasnces;
           
           for( i = 0; i < pMCInst->pCurrentMultiCell->numCellAnim; i++ )
           {
              pAnmCtrl = NNS_G2dGetCellAnimationAnimCtrl( &pCellAnimArray[i].cellAnim );
              
              NNS_G2dStartAnimCtrl( pAnmCtrl );
           }
           
       }else{    
           
           NNSG2dNode*              pNode      = (NNSG2dNode*)pMCInst->pCellAnimInstasnces;
                 
           
           for( i = 0; i < pMCInst->pCurrentMultiCell->numNodes; i++ )
           {
              // 現状では NNS_G2D_NODETYPE_CELL のみ対応
              NNS_G2D_ASSERT( pNode[i].type == NNS_G2D_NODETYPE_CELL );
              pAnmCtrl = NNS_G2dGetCellAnimationAnimCtrl( (NNSG2dCellAnimation*)pNode[i].pContent );
              
              NNS_G2dStartAnimCtrl( pAnmCtrl );
           }
       }
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dRestartMCAnimation

  Description:  マルチセルアニメーション中のセルアニメーションの再生を
                 シーケンスの先頭から再スタートします。
                 逆再生している場合は、終端から再生します。
                
  Arguments:    pMCInst:      [OUT] マルチセル実体
                
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dRestartMCAnimation
(
    NNSG2dMultiCellAnimation*   pMultiCellAnim
)
{
    u16                     i             = 0;
    NNSG2dMultiCellInstance* pMCInst      = &pMultiCellAnim->multiCellInstance;
    NNSG2dAnimController*    pMCAnmCtrl   = NNS_G2dGetMCAnimAnimCtrl( pMultiCellAnim );
    
    
    // 総ビデオフレームをリセットします。
    pMultiCellAnim->totalVideoFrame = 0;
    
    // マルチセルアニメーションのアニメーションコントローラを再スタートします。
    NNS_G2dSetAnimSequenceToMCAnimation( pMultiCellAnim, 
                                         NNS_G2dGetAnimCtrlCurrentAnimSequence( pMCAnmCtrl ) );    
    NNS_G2dStartAnimCtrl( pMCAnmCtrl );
    
    
    // マルチセルを構成している、セルアニメーションを再スタートします。
    NNS_G2D_NULL_ASSERT( pMCInst );
    {
       if( pMCInst->mcType == NNS_G2D_MCTYPE_SHARE_CELLANIM )
       {
           NNSG2dMCCellAnimation*     pCellAnimArray = pMCInst->pCellAnimInstasnces;
           
           for( i = 0; i < pMCInst->pCurrentMultiCell->numCellAnim; i++ )
           {
              // 再生を再スタートします。
              NNS_G2dRestartCellAnimation( &pCellAnimArray[i].cellAnim );
           }
           
       }else{    
           
           NNSG2dNode*              pNode      = (NNSG2dNode*)pMCInst->pCellAnimInstasnces;
                 
           for( i = 0; i < pMCInst->pCurrentMultiCell->numNodes; i++ )
           {
              // 現状では NNS_G2D_NODETYPE_CELL のみ対応
              NNS_G2D_ASSERT( pNode[i].type == NNS_G2D_NODETYPE_CELL );
              // 再生を再スタートします。
              NNS_G2dRestartCellAnimation( (NNSG2dCellAnimation*)pNode[i].pContent );
           }
       }
    }
}


/*---------------------------------------------------------------------------*
  Name:         NNS_G2dMakeSimpleMultiCellToOams

  Description:  OAM 情報 を 生成します
                MultiCell を 構成 する CellAnimation( MultiCellAnimation ) が
                affine 変換を持たない, 単純な MultiCell 実体に限って 実行可能です。
  
                MultiCell 中で不正な affine 変換 が発見された場合は警告を表示し、
                変換 を無視して 処理を続行します。
                ( したがって、描画結果は意図したものとはなりません ）
                 
                MultiCell全体の Affine変換は可能です。              
                ただしその場合、フリップ効果を適用しているOBJは正しく描画できません。  
                
                
                
  Arguments:    pDstOams    :             [IN]  結果用バッファ
                numDstOams  :             [IN]  結果用バッファ長
                pMCellInst  :             [OUT] マルチセル実体
                pMtxSR      :             [IN]  affine行列( 3D Format )
                pBaseTrans  :             [IN]  平行移動成分
                affineIndex :             [IN]  マルチセル構成Oamが参照する Affine Index
                bDoubleAffine:            [IN]  倍角affineか
                
                
  Returns:      実際に使用された 結果用バッファ の 数
  
 *---------------------------------------------------------------------------*/ 
u16 NNS_G2dMakeSimpleMultiCellToOams
( 
    GXOamAttr*                      pDstOams, 
    u16                             numDstOams,
    const NNSG2dMultiCellInstance*  pMCellInst, 
    const MtxFx22*                  pMtxSR, 
    const NNSG2dFVec2*              pBaseTrans,
    u16                             affineIndex,
    BOOL                            bDoubleAffine 
)
{
    if( pMCellInst->mcType == NNS_G2D_MCTYPE_SHARE_CELLANIM )
    {
        return MakeSimpleMultiCellToOams_ShareCellAnims_( pDstOams, 
                              numDstOams, 
                              pMCellInst, 
                              pMtxSR, 
                              pBaseTrans, 
                              affineIndex, 
                              bDoubleAffine );
    }else{
        return MakeSimpleMultiCellToOams_( pDstOams, 
                                    numDstOams, 
                                    pMCellInst, 
                                    pMtxSR, 
                                    pBaseTrans, 
                                    affineIndex, 
                                    bDoubleAffine );
    }    
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dMakeSimpleMultiCellToOams

  Description:  マルチセルを構成するセルアニメーションについて、
                それらを巡回しながら、各セルアニメーションを引数に
                コールバック関数呼び出しを行います。
                
                
  Arguments:    pMCellInst  :             [IN] マルチセル実体
                pCBFunc     :             [IN] マルチセル中の各セルアニメーション
                                               について呼び出されるコールバック関数
                userParamater :           [IN] コールバック関数の第１引数として渡される、
                                               ユーザが自由に使用するパラメータです。  
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/ 
void NNS_G2dTraverseMCCellAnims
( 
    NNSG2dMultiCellInstance*         pMCellInst,
    NNSG2dMCTraverseCellAnimCallBack pCBFunc,
    u32                              userParamater
)
{
    BOOL                         bContinue = TRUE;
    if( pMCellInst->mcType == NNS_G2D_MCTYPE_SHARE_CELLANIM )
    {
       u16 i;
       NNSG2dMCCellAnimation* pCellAnimArray = pMCellInst->pCellAnimInstasnces;
       
       for( i = 0; i < pMCellInst->pCurrentMultiCell->numCellAnim; i++ )
       {
           bContinue = (*pCBFunc)( userParamater, &pCellAnimArray[i].cellAnim, i );
           
           if( !bContinue )
           {
              break;
           }
       }
    }else{    
       u16 i;
       NNSG2dNode*   pNodeArray = NULL;
        
       NNS_G2D_NULL_ASSERT( pMCellInst );
       NNS_G2D_NULL_ASSERT( pMCellInst->pCurrentMultiCell );
        
       pNodeArray = (NNSG2dNode*)(pMCellInst->pCellAnimInstasnces);
        
       for( i = 0; i < pMCellInst->pCurrentMultiCell->numNodes; i++ )
       { 
           // 現状では NNS_G2D_NODETYPE_CELL のみ対応
           NNS_G2D_ASSERT( pNodeArray[i].type == NNS_G2D_NODETYPE_CELL );
           bContinue = (*pCBFunc)( userParamater, (NNSG2dCellAnimation*)pNodeArray[i].pContent, i );           
           
           if( !bContinue )
           {
              break;
           }
       }
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTraverseMCNodes

  Description:  マルチセルを構成するノードについて、
                それらを巡回しながら、各ノード、セルアニメーション、ノード番号を引数に
                コールバック関数呼び出しを行います。
                
                
  Arguments:    pMCellInst    :             [IN] マルチセル実体
                pCBFunc       :             [IN] マルチセル中の各セルアニメーション
                                                 について呼び出されるコールバック関数
                userParamater :             [IN] コールバック関数の第１引数として渡される、
                                                 ユーザが自由に使用するパラメータです。  
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/ 
void NNS_G2dTraverseMCNodes
( 
    NNSG2dMultiCellInstance*        pMCellInst,
    NNSG2dMCTraverseNodeCallBack    pCBFunc,
    u32                             userParamater
)
{
    NNS_G2D_NULL_ASSERT( pMCellInst );
    NNS_G2D_NULL_ASSERT( pMCellInst->pCurrentMultiCell );    
    {
        u16 i;
        const NNSG2dMultiCellData*   pMCData = pMCellInst->pCurrentMultiCell;
        const u16                    numNode = pMCData->numNodes;
        BOOL                         bContinue = TRUE;
        
        if( pMCellInst->mcType == NNS_G2D_MCTYPE_SHARE_CELLANIM )
        {
           NNSG2dMCCellAnimation* pMCCellAnimArray
               = (NNSG2dMCCellAnimation*)pMCellInst->pCellAnimInstasnces;
           //
           // すべてのNodeについて巡回しながら...
           //
           for( i = 0; i < numNode; i++ )
           {
               const NNSG2dMultiCellHierarchyData* pNodeData = &pMCData->pHierDataArray[i];
               // pNode->nodeAttr に ノードに関連付けられている セルアニメ番号 格納されている  
               const u16 cellAnimIdx  = NNSi_G2dGetMC2NodeCellAinmIdx( pNodeData );
                
               // コールバック呼び出し
               bContinue = (*pCBFunc)( userParamater,
                                       pNodeData,                                 // ノードデータ
                                       &pMCCellAnimArray[cellAnimIdx].cellAnim,   // セルアニメーション
                                       i );                                       // ノード番号                 
               if( !bContinue )
               {
                  break;
               }
           }
        }else{ 
           const NNSG2dNode* pNode 
               = (const NNSG2dNode*)pMCellInst->pCellAnimInstasnces;
           //
           // すべてのNodeについて巡回しながら...
           //
           for( i = 0; i < numNode; i++ )
           {
               const NNSG2dMultiCellHierarchyData* pNodeData = &pMCData->pHierDataArray[i];
               // pNode->animSequenceIdx
               // pNode->nodeAttr に ノードに関連付けられている セルアニメ番号 格納されている  
               //
               //
               // コールバック呼び出し
               bContinue = (*pCBFunc)( userParamater,
                                       pNodeData,           // ノードデータ
                                       pNode[i].pContent,   // セルアニメーション
                                       i );                 // ノード番号            
               
               if( !bContinue )
               {
                  break;
               }
           }
        }
    }
}
//------------------------------------------------------------------------------
//
// 本関数はプライベート関数に変更されました。
// 過去のバージョンとの互換性維持のために、旧APIは残されています。
//
void NNS_G2dInitMCAnimation( 
    NNSG2dMultiCellAnimation*          pMultiCellAnim, 
    NNSG2dNode*                        pNodeArray, 
    NNSG2dCellAnimation*               pCellAnim, 
    u16                                numNode, 
    const NNSG2dCellAnimBankData*      pAnimBank,  
    const NNSG2dCellDataBank*          pCellDataBank,
    const NNSG2dMultiCellDataBank*     pMultiCellDataBank 
)
{
    InitMCAnimation_( pMultiCellAnim, 
                      pNodeArray, 
                      pCellAnim, 
                      numNode, 
                      pAnimBank, 
                      pCellDataBank, 
                      pMultiCellDataBank );
}
//
// 本関数はプライベート関数に変更されました。
// 過去のバージョンとの互換性維持のために、旧APIは残されています。
//
void NNS_G2dInitMCInstance
( 
    NNSG2dMultiCellInstance*           pMultiCell, 
    NNSG2dNode*                        pNodeArray, 
    NNSG2dCellAnimation*               pCellAnim, 
    u16                                numNode, 
    const NNSG2dCellAnimBankData*      pAnimBank,  
    const NNSG2dCellDataBank*          pCellDataBank 
)
{
    InitMCInstance_
    ( 
       pMultiCell, 
       pNodeArray, 
       pCellAnim, 
       numNode, 
       pAnimBank,  
       pCellDataBank 
    );
}


//------------------------------------------------------------------------------
//
// 本関数はプライベート関数に変更されました。
// 過去のバージョンとの互換性維持のために、旧APIは残されています。
//
BOOL NNS_G2dSetMCDataToMCInstance
( 
    NNSG2dMultiCellInstance*       pMCInst, 
    const NNSG2dMultiCellData*     pMcData 
)
{
    SetMCDataToMCInstanceImpl_( pMCInst, pMcData, 0 );

    // API互換性維持のため
    // 実質的な意味はありません。    
    return TRUE;
}



//------------------------------------------------------------------------------


