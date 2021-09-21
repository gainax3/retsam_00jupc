/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - g2d - src - load
  File:     g2d_NSC_load.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.5 $
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include <nnsys/g2d/load/g2d_NSC_load.h>
#include <nnsys/g2d/g2d_Load.h>
#include "g2di_Debug.h"


#ifndef SDK_FINALROM
const static char* sColorModeString[] =
{
	"NNS_G2D_SCRCOLORMODE_16x16",
	"NNS_G2D_SCRCOLORMODE_256x1",
	"NNS_G2D_SCRCOLORMODE_256x16"
};

const static char* sScreenFormatString[] =
{
    "NNS_G2D_SCREENFORMAT_TEXT",
    "NNS_G2D_SCREENFORMAT_AFFINE",
    "NNS_G2D_SCREENFORMAT_AFFINEEXT",
    "NNS_G2D_SCREENFORMAT_PLTBMP",
    "NNS_G2D_SCREENFORMAT_DCBMP"
};
#endif // SDK_FINALROM

//------------------------------------------------------------------------------
BOOL NNS_G2dGetUnpackedScreenData( void* pNscrFile, NNSG2dScreenData** ppScrData )
{
    NNS_G2D_NULL_ASSERT( pNscrFile );
    NNS_G2D_NULL_ASSERT( ppScrData );
    NNS_G2D_ASSERTMSG( NNS_G2dIsBinFileValid( pNscrFile, NNS_G2D_BINFILE_SIG_SCRDATA, 
                                                     BIN_FILE_VERSION( NSCR ) ), "Input file is obsolete. Please use the new g2dcvtr.exe." );
                                                     
    {
        NNSG2dBinaryFileHeader*     pBinFile    = (NNSG2dBinaryFileHeader*)pNscrFile;
        {
            NNSG2dScreenDataBlock* pBinBlk   =
                (NNSG2dScreenDataBlock*)NNS_G2dFindBinaryBlock( pBinFile, 
                                                                   NNS_G2D_BINBLK_SIG_SCRDATA );
            if( pBinBlk )
            {
//                NNS_G2dUnpackNSC( (void*)&pBinBlk->screenData );
                //
                // OK 
                //
            	*ppScrData = &(pBinBlk->screenData);
                NNS_G2D_NULL_ASSERT( (*ppScrData) );
            	NNS_G2D_ASSERT( (*ppScrData)->screenWidth > 0  );
            	NNS_G2D_ASSERT( (*ppScrData)->screenHeight > 0 );
            	NNS_G2D_ASSERT( ((*ppScrData)->screenWidth & 0x7) == 0 );
            	NNS_G2D_ASSERT( ((*ppScrData)->screenHeight & 0x7) == 0 );
                NNS_G2D_MINMAX_ASSERT( (*ppScrData)->colorMode, NNS_G2D_SCREENCOLORMODE_16x16, NNS_G2D_SCREENCOLORMODE_256x16 );
                NNS_G2D_MINMAX_ASSERT( (*ppScrData)->screenFormat, NNS_G2D_SCREENFORMAT_TEXT, NNS_G2D_SCREENFORMAT_AFFINEEXT );
            	NNS_G2D_ASSERT(
            		((*ppScrData)->screenFormat != NNS_G2D_SCREENFORMAT_AFFINE)
            		    || ((*ppScrData)->colorMode == NNS_G2D_SCREENCOLORMODE_256x1) );
            	NNS_G2D_ASSERT(
            		((*ppScrData)->screenFormat != NNS_G2D_SCREENFORMAT_AFFINEEXT)
            		    || ((*ppScrData)->colorMode == NNS_G2D_SCREENCOLORMODE_256x16) );

            	return TRUE;
            }else{
                //
                // NG
                //
                *ppScrData = NULL;
                return FALSE;
                
            }
        }
    }
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#ifndef SDK_FINALROM 

void NNS_G2dPrintScreenData( const NNSG2dScreenData* pScrData )
{
    NNS_G2D_NULL_ASSERT( pScrData );
    {
        OS_Printf( "---------------------------------------------\n" );
        OS_Printf( " Screen Data (NSC)\n" );
    	OS_Printf( " screenWidth   = %d\n", pScrData->screenWidth );
    	OS_Printf( " screenHeight  = %d\n", pScrData->screenHeight );
    	OS_Printf( " colorMode     = %s\n", sColorModeString[ pScrData->colorMode ] );
    	OS_Printf( " screenFormat  = %s\n", sScreenFormatString[ pScrData->screenFormat ] );
    	OS_Printf( " szByte        = %d\n", pScrData->szByte );
        OS_Printf( "---------------------------------------------\n" );
    }
}

#endif // SDK_FINALROM 

