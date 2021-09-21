/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - g2d - src - load
  File:     g2d_NEN_load.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.11 $
 *---------------------------------------------------------------------------*/
#include <nitro.h>
#include <nnsys/g2d/load/g2d_NEN_load.h>
#include <nnsys/g2d/g2d_Load.h>
#include "g2di_Debug.h"


#ifndef SDK_FINALROM
static const char* s_entityTypeStrTbl[] = 
{
    "NNS_G2D_ENTITYTYPE_INVALID",
    "NNS_G2D_ENTITYTYPE_CELL",
    "NNS_G2D_ENTITYTYPE_MULTICELL",
    "NNS_G2D_ENTITYTYPE_MAX",
};
#endif // SDK_FINALROM

//------------------------------------------------------------------------------
BOOL NNS_G2dGetUnpackedEntityBank( void* pNenrFile, NNSG2dEntityDataBank** ppEntityBank )
{
    NNS_G2D_NULL_ASSERT( pNenrFile );
    NNS_G2D_NULL_ASSERT( ppEntityBank );
     
    NNS_G2D_ASSERTMSG( NNSi_G2dIsBinFileSignatureValid( pNenrFile, 
                                                        NNS_G2D_BINFILE_SIG_ENTITY ),
                       "Input file signature is invalid for this method." );
                     
    NNS_G2D_ASSERTMSG( NNSi_G2dIsBinFileVersionValid( pNenrFile, 
                                                      BIN_FILE_VERSION( NENR ) ), 
                       "Input file is obsolete. Please use the new g2dcvtr.exe." );                                        
    {
        NNSG2dBinaryFileHeader*     pBinFile    = (NNSG2dBinaryFileHeader*)pNenrFile;
        {
            NNSG2dEntityDataBankBlock* pBinBlk   =
                (NNSG2dEntityDataBankBlock*)NNS_G2dFindBinaryBlock( pBinFile, 
                                                                    NNS_G2D_BLKSIG_ENTITYBANK );
            if( pBinBlk )
            {
                NNS_G2dUnpackNEN( (void*)&pBinBlk->entityDataBank );
                //
                // OK 
                //
                *ppEntityBank = &pBinBlk->entityDataBank;
                return TRUE;
            }else{
                //
                // NG
                //
                *ppEntityBank = NULL;
                return FALSE;
                
            }
        }
    }
}

//------------------------------------------------------------------------------
void NNS_G2dUnpackNEN( NNSG2dEntityDataBank* pDataBank )
{
    NNS_G2D_NULL_ASSERT( pDataBank );
    {
        //
        // Unpack offset pointer to the NNSG2dEntityData data. 
        //
        pDataBank->pEntityDataArray 
            = NNS_G2D_UNPACK_OFFSET_PTR( pDataBank->pEntityDataArray, pDataBank );
            
        pDataBank->pAnimSequenceIdxArray 
            = NNS_G2D_UNPACK_OFFSET_PTR( pDataBank->pAnimSequenceIdxArray, pDataBank );
        
        
        {
            u16 i;
            NNSG2dEntityData* pEntity = pDataBank->pEntityDataArray;
            for( i = 0; i < pDataBank->numEntityDatas; i++ )
            {
                pEntity[i].animData.pAnimSequenceIdxArray = 
                    NNS_G2D_UNPACK_OFFSET_PTR( pEntity[i].animData.pAnimSequenceIdxArray, 
                                               pDataBank->pAnimSequenceIdxArray );
            }
        }
    }
    NNSI_G2D_DEBUGMSG0("Unpacking NENR file is successful.\n" );
}

//------------------------------------------------------------------------------
const NNSG2dEntityData* NNS_G2dGetEntityDataByIdx
( 
    const NNSG2dEntityDataBank* pDataBank, 
    u16 idx 
)
{
    NNS_G2D_NULL_ASSERT( pDataBank );
        
    if( idx < pDataBank->numEntityDatas )
    {
        return &pDataBank->pEntityDataArray[idx];
    }else{
        return NULL;
    }
}


#ifndef SDK_FINALROM 

//------------------------------------------------------------------------------
void NNS_G2dPrintEntityData        ( const NNSG2dEntityData* pEntity )
{
    u16 i;
    NNS_G2D_NULL_ASSERT( pEntity );
    
    OS_Printf( "type            = %s\n", s_entityTypeStrTbl[pEntity->type] );
    OS_Printf( "numAnimSequence = %d\n", pEntity->animData.numAnimSequence );
    for( i  = 0; i < pEntity->animData.numAnimSequence; i ++ )
    {
        OS_Printf( "anim %d ==> %d \n", i, pEntity->animData.pAnimSequenceIdxArray[i] );
    }
}

//------------------------------------------------------------------------------
void NNS_G2dPrintEntityDataBank    ( const NNSG2dEntityDataBank* pDataBank )
{
    u16         i;
    const NNSG2dEntityData*   pE;
    NNS_G2D_NULL_ASSERT( pDataBank );
    
    OS_Printf( "---------------------------------------------\n" );
    OS_Printf( "numEntityDatas = %d\n", pDataBank->numEntityDatas );
    for( i = 0; i < pDataBank->numEntityDatas; i++ )
    {
        OS_Printf( "Entity Idx = %d\n", i );
        pE = NNS_G2dGetEntityDataByIdx( pDataBank, i );
        NNS_G2dPrintEntityData( pE );
    }
    OS_Printf( "---------------------------------------------\n" );
}

#endif // SDK_FINALROM 
