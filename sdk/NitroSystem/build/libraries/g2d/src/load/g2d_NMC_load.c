/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - g2d - src - load
  File:     g2d_NMC_load.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.13 $
 *---------------------------------------------------------------------------*/
 
#include <nitro.h>
#include <nnsys/g2d/load/g2d_NMC_load.h>
#include <nnsys/g2d/g2d_Load.h>
#include "g2di_Debug.h"


#ifndef SDK_FINALROM
// NNSG2dMCAnimationPlayMode の文字列
static const char* s_playModeStrTbl[] = 
{
    "NNS_G2D_MCANIM_PLAYMODE_RESET",
    "NNS_G2D_MCANIM_PLAYMODE_CONTINUE",
    "NNS_G2D_MCANIM_PLAYMODE_PAUSE",
    "NNS_G2D_MCANIM_PLAYMODE_MAX",
};
#endif // SDK_FINALROM

//------------------------------------------------------------------------------
BOOL NNS_G2dGetUnpackedMultiCellBank
( 
    void*                       pNmcrFile, 
    NNSG2dMultiCellDataBank**   ppMCBank 
)
{
    NNS_G2D_NULL_ASSERT( pNmcrFile );
    NNS_G2D_NULL_ASSERT( ppMCBank );
    
    NNS_G2D_ASSERTMSG( NNSi_G2dIsBinFileSignatureValid( pNmcrFile, 
                                                        NNS_G2D_BINFILE_SIG_MULTICELL ),
                       "Input file signature is invalid for this method." );
                     
    NNS_G2D_ASSERTMSG( NNSi_G2dIsBinFileVersionValid( pNmcrFile, 
                                                      BIN_FILE_VERSION( NMCR ) ), 
                       "Input file is obsolete. Please use the new g2dcvtr.exe." );
                                                     
    {
        NNSG2dBinaryFileHeader*     pBinFile    = (NNSG2dBinaryFileHeader*)pNmcrFile;
        {
            NNSG2dMultiCellDataBankBlock* pBinBlk   =
                (NNSG2dMultiCellDataBankBlock*)NNS_G2dFindBinaryBlock( pBinFile, 
                                                                       NNS_G2D_BLKSIG_MULTICELLBANK );
            if( pBinBlk )
            {
                NNS_G2dUnpackNMC( (void*)&pBinBlk->multiCellDataBank );
                //
                // OK 
                //
                *ppMCBank = &pBinBlk->multiCellDataBank;
                return TRUE;
            }else{
                //
                // NG
                //
                *ppMCBank = NULL;
                return FALSE;
                
            }
        }
    }
}

//------------------------------------------------------------------------------
// 拡張データの復元を行います。
// 拡張データは複数のブロックから構成される可能性がありますが、
// 現在はユーザ拡張アトリビュートブロックのみが格納されています。
static void UnpackExtendedData_( void* pExData )
{
    NNS_G2D_NULL_ASSERT( pExData );
    
    //------- ユーザ拡張アトリビュートブロック の復元
    {    
        NNSG2dUserExDataBlock*    pBlk          = (NNSG2dUserExDataBlock*)pExData;
        NNSG2dUserExCellAttrBank* pCellAttrBank = (NNSG2dUserExCellAttrBank*)(pBlk + 1);
    
        NNSi_G2dUnpackUserExCellAttrBank( pCellAttrBank );
    }
}

//------------------------------------------------------------------------------
void NNS_G2dUnpackNMC( NNSG2dMultiCellDataBank* pMCellData )
{
    NNS_G2D_NULL_ASSERT( pMCellData );
    
    {
        //
        // Unpack offset pointer to the MultiCell data. 
        //
        pMCellData->pMultiCellDataArray 
            = NNS_G2D_UNPACK_OFFSET_PTR( pMCellData->pMultiCellDataArray, pMCellData );
        
        pMCellData->pHierarchyDataArray 
            = NNS_G2D_UNPACK_OFFSET_PTR( pMCellData->pHierarchyDataArray, pMCellData );
        //
        // Unpack offset pointer to the NNSG2dMultiCellHierarchyData data. 
        //
        {
            u16 i;
            NNSG2dMultiCellData* pCell = pMCellData->pMultiCellDataArray;
            for( i = 0; i < pMCellData->numMultiCellData; i++ )
            {
                pCell[i].pHierDataArray 
                    = NNS_G2D_UNPACK_OFFSET_PTR( pCell[i].pHierDataArray, 
                                                 pMCellData->pHierarchyDataArray );
            }
        }
        //
        // 拡張情報ポインタを復元します。(存在すれば...) 
        //
        if( pMCellData->pExtendedData != NULL )
        {
            pMCellData->pExtendedData
               = NNS_G2D_UNPACK_OFFSET_PTR( pMCellData->pExtendedData, pMCellData );        
            UnpackExtendedData_( pMCellData->pExtendedData );
        }
    }
    
    NNSI_G2D_DEBUGMSG0("Unpacking NMCR file is successful.\n" );
}

//------------------------------------------------------------------------------
const NNSG2dMultiCellData* 
NNS_G2dGetMultiCellDataByIdx
( 
    const NNSG2dMultiCellDataBank* pDataBank, 
    u16 idx 
)
{
    NNS_G2D_NULL_ASSERT( pDataBank );
        
    if( idx < pDataBank->numMultiCellData )
    {
        return &pDataBank->pMultiCellDataArray[idx];
    }else{
        return NULL;
    }
}

#ifndef SDK_FINALROM 


//------------------------------------------------------------------------------
static const char* GetPlayModeStr_( playType )
{
    if( (NNS_G2D_MCANIM_PLAYMODE_RESET <= playType) &&
        (NNS_G2D_MCANIM_PLAYMODE_MAX >= playType) )
    {
        return s_playModeStrTbl[playType];
    }else{
        return "INVALID_DATA_TYPE";
    }
}

//------------------------------------------------------------------------------
static void PrintNodeAttr_( const NNSG2dMultiCellHierarchyData* pNode )
{
    NNS_G2D_NULL_ASSERT( pNode );
    
    OS_Printf( "nodeAttr--- = %x\n", pNode->nodeAttr );
    
    OS_Printf( "visibilityFlag = %d\n", 
                NNSi_G2dIsMultiCellNodeVisible( pNode ) );
    
    OS_Printf( "playMode = %s\n", 
                GetPlayModeStr_( NNSi_G2dGetMultiCellNodePlayMode( pNode) ) );
}



//------------------------------------------------------------------------------
// 拡張情報を表示します
static void PrintCellExtendedData_( const void* pExData )
{
    NNS_G2D_NULL_ASSERT( pExData ); 
    {    
        NNSG2dUserExDataBlock*    pBlk          = (NNSG2dUserExDataBlock*)pExData;
        NNSG2dUserExCellAttrBank* pCellAttrBank = (NNSG2dUserExCellAttrBank*)(pBlk + 1);
        
        NNSi_G2dPrintUserExCellAttrBank( pCellAttrBank );        
    }
}

//------------------------------------------------------------------------------
void NNS_G2dPrintNodeHierData( const NNSG2dMultiCellHierarchyData* pNode )
{
    NNS_G2D_NULL_ASSERT( pNode );
    
    OS_Printf( "animSequenceIdx = %d\n", pNode->animSequenceIdx);
    OS_Printf( "posX            = %d\n", pNode->posX);
    OS_Printf( "posY            = %d\n", pNode->posY);
    
    PrintNodeAttr_( pNode );
}

//------------------------------------------------------------------------------
void NNS_G2dPrintMultiCellInfo  ( const NNSG2dMultiCellData* pMCell )
{
    u16         i;
    NNS_G2D_NULL_ASSERT( pMCell );
    OS_Printf( "-------------------\n" );
    {
        const NNSG2dMultiCellHierarchyData*    pHierData = pMCell->pHierDataArray;
        
        OS_Printf( "numNodes = %d\n", pMCell->numNodes );
        
        for( i = 0; i < pMCell->numNodes; i++ )
        {
            OS_Printf( "---node Idx = %d\n", i );
            NNS_G2dPrintNodeHierData( &pHierData[i] );    
        }
    }
    OS_Printf( "-------------------\n" );
}

//------------------------------------------------------------------------------
void NNS_G2dPrintMultiCellBank  ( const NNSG2dMultiCellDataBank* pDataBank )
{
    u16         i;
    const NNSG2dMultiCellData*   pMCell;
    OS_Printf( "---------------------------------------------\n" );
    OS_Printf( "numMultiCell = %d\n", pDataBank->numMultiCellData );
    for( i = 0; i < pDataBank->numMultiCellData; i++ )
    {
        OS_Printf( "MultiCell Idx = %d\n", i );
        pMCell = NNS_G2dGetMultiCellDataByIdx( pDataBank, i );
        NNS_G2dPrintMultiCellInfo( pMCell );
    }
    //
    // 拡張情報の表示（もし存在すれば...）
    //
    if( pDataBank->pExtendedData != NULL )
    {
        PrintCellExtendedData_( pDataBank->pExtendedData );
    }
    
    OS_Printf( "---------------------------------------------\n" );
}

#endif // SDK_FINALROM 
