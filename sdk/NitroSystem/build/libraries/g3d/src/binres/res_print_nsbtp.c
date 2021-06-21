/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - g3d - src - binres
  File:     res_print_nsbtp.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.13 $
 *---------------------------------------------------------------------------*/

//
// AUTHOR: Kenji Nishida
//

#include "./res_print_internal.h"

#include <nnsys/g3d/binres/res_struct.h>
#include <nnsys/g3d/binres/res_print.h>
#include <nnsys/g3d/binres/res_struct_accessor.h>

#if defined(SDK_DEBUG) || defined(NNS_FROM_TOOL)


//---------------------------------------------------------------------
void 
NNS_G3dPrintTexPatAnm( const NNSG3dResTexPatAnm* pPatAnm )
{
    NNS_G3D_NULL_ASSERT( pPatAnm );
    
    {
        u32 i, j;
        const u32 numEntries = pPatAnm->dict.numEntry;
        tabPlus_();
        for( i = 0; i < numEntries; i++ )
        {
            const NNSG3dResDictTexPatAnmData* pAnmData = 
                NNSi_G3dGetTexPatAnmDataByIdx( pPatAnm, i );
            {
                const NNSG3dResName* name =
                    NNS_G3dGetResNameByIdx(&pPatAnm->dict, i);
                chkDict_(&pPatAnm->dict, name, i);
                tabPrint_(); RES_PRINTF("Target material name, ");
                NNS_G3dPrintResName(name); RES_PRINTF("\n");
            }

            tabPrint_(); RES_PRINTF("# of FV keys, %d\n", pAnmData->numFV);

            {
                const NNSG3dResTexPatAnmFV* pfvArray =
                    (const NNSG3dResTexPatAnmFV*)((u8*)pPatAnm + pAnmData->offset);
                
                tabPlus_();
                for( j = 0; j < pAnmData->numFV; j++ )
                {
                    const NNSG3dResTexPatAnmFV* pTexPatAnm =
                        NNSi_G3dGetTexPatAnmFVByFVIndex( pPatAnm, i, j );
                    tabPrint_();
                    RES_PRINTF("frame, %d, ", pTexPatAnm->idxFrame);
                    {
                        const NNSG3dResName* name =
                            (const NNSG3dResName*)((u8*)pPatAnm + pPatAnm->ofsTexName);
                        RES_PRINTF("texture, ");
                        NNS_G3dPrintResName(&name[pTexPatAnm->idTex]);
                    }
                    {
                        const NNSG3dResName* name =
                            (const NNSG3dResName*)((u8*)pPatAnm + pPatAnm->ofsPlttName);
                        RES_PRINTF(", palette, ");
                        NNS_G3dPrintResName(&name[pTexPatAnm->idPltt]);
                    }
                    RES_PRINTF("\n");
                }
                tabMinus_();
            }
        }
        tabMinus_();
    }
}


/*---------------------------------------------------------------------------*
  Name:         NNS_G3dPrintTexPatAnmSet

  Description:  NNSG3dResTexPatAnmSetの内容を デバック出力に表示します。
                
  Arguments:    pAnmSet:     テクスチャパターンアニメーションのセットを束ねる構造体
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void
NNS_G3dPrintTexPatAnmSet( const NNSG3dResTexPatAnmSet* pAnmSet )
{
    NNS_G3D_NULL_ASSERT( pAnmSet );
    NNS_G3D_ASSERT( pAnmSet->header.kind == NNS_G3D_DATABLK_TEXPAT_ANM );
    
    {
        u8 i;
        const u8 numEntries = pAnmSet->dict.numEntry;

        tabPrint_(); RES_PRINTF("# of animations, %d\n", numEntries);
        tabPlus_();
        for( i = 0; i < numEntries; i++ )
        {
            const NNSG3dResName* name =
                NNS_G3dGetResNameByIdx(&pAnmSet->dict, i);
            chkDict_(&pAnmSet->dict, name, i);
            tabPrint_(); RES_PRINTF("Texture pattern animation, ");
            NNS_G3dPrintResName(name); RES_PRINTF("\n");

            NNS_G3dPrintTexPatAnm( NNS_G3dGetTexPatAnmByIdx( pAnmSet, i ) );
        }
        tabMinus_();
    }
}


/*---------------------------------------------------------------------------*
  Name:         NNS_G3dPrintNSBTP

  Description:  NSBTP ファイル の内容を デバック出力に表示します。
                
  Arguments:    binFile:     NSBTPファイルデータの先頭を指すポインタ
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void
NNS_G3dPrintNSBTP(const u8* binFile)
{
    u32                          i;
    const NNSG3dResFileHeader*   header;
    u32                          numBlocks;
    NNS_G3D_NULL_ASSERT(binFile);
    
    header     = (const NNSG3dResFileHeader*)&binFile[0];
    NNS_G3D_ASSERT( header->sigVal == NNS_G3D_SIGNATURE_NSBTP );
    NNS_G3dPrintFileHeader(header);

    //
    // すべてのブロックについて...
    //
    numBlocks  = header->dataBlocks;
    for ( i = 0; i < numBlocks; ++i )
    {
        const NNSG3dResDataBlockHeader* blk;
        blk = NNS_G3dGetDataBlockHeaderByIdx(header, i);
        NNS_G3D_NULL_ASSERT( blk );
        NNS_G3dPrintDataBlockHeader(blk);

        //
        // 具体的な種類ごとにサマリのデバック出力を実行します
        //
        tabPlus_();
        {
            switch( blk->kind )
            {
            case NNS_G3D_DATABLK_TEXPAT_ANM:
                NNS_G3dPrintTexPatAnmSet((NNSG3dResTexPatAnmSet*)blk);
                break;
            default:
                tabPlus_();
                tabPrint_(); RES_PRINTF("cannot display this block\n");
                tabMinus_();
            break;
            }
        }
        tabMinus_();
    }    
}

#endif
