/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g2d
  File:     g2d_Load.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.12 $
 *---------------------------------------------------------------------------*/
#ifndef NNS_G2D_LOAD_H_
#define NNS_G2D_LOAD_H_

#include <nitro.h>


#ifdef __cplusplus
extern "C" {
#endif



//------------------------------------------------------------------------------
// for loading binary file.
#include <nnsys/g2d/load/g2d_NCE_load.h>
#include <nnsys/g2d/load/g2d_NAN_load.h>
#include <nnsys/g2d/load/g2d_NEN_load.h>
#include <nnsys/g2d/load/g2d_NMC_load.h>
#include <nnsys/g2d/load/g2d_NCG_load.h>
#include <nnsys/g2d/load/g2d_NCL_load.h>
#include <nnsys/g2d/load/g2d_NSC_load.h>
#include <nnsys/g2d/load/g2d_NFT_load.h>

//------------------------------------------------------------------------------
#define BIN_FILE_VERSION( EXT ) NNS_G2dMakeVersionData( NNS_G2D_##EXT##_MAJOR_VER, NNS_G2D_##EXT##_MINOR_VER )




NNSG2dBinaryBlockHeader* NNS_G2dFindBinaryBlock
( 
    NNSG2dBinaryFileHeader* pBinFileHeader, 
    u32                     signature 
);

//------------------------------------------------------------------------------
void NNSi_G2dUnpackUserExCellAttrBank( NNSG2dUserExCellAttrBank* pCellAttrBank );

//------------------------------------------------------------------------------
// デバック出力関数
// FINAL_ROM ではコード生成されない
#ifdef __SNC__
NNS_G2D_DEBUG_FUNC_DECL_BEGIN void NNSi_G2dPrintUserExCellAttrBank( const NNSG2dUserExCellAttrBank* pCellAttrBank ) NNS_G2D_DEBUG_FUNC_DECL_END
#else//__SNC__
NNS_G2D_DEBUG_FUNC_DECL_BEGIN void NNSi_G2dPrintUserExCellAttrBank( const NNSG2dUserExCellAttrBank*  ) NNS_G2D_DEBUG_FUNC_DECL_END
#endif//__SNC__

//
// インライン関数
//
//------------------------------------------------------------------------------
NNS_G2D_INLINE BOOL NNSi_G2dIsBinFileSignatureValid
( 
    const NNSG2dBinaryFileHeader*   pBinFile, 
    u32                             binFileSig 
)
{
    if( pBinFile != NULL )
    {
        // ファイル識別子が正しいか
        if( ( pBinFile->signature == binFileSig  ) )
        {
            return TRUE;                
        }        
    }
    return FALSE;
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE BOOL NNSi_G2dIsBinFileVersionValid
( 
    const NNSG2dBinaryFileHeader*   pBinFile, 
    u16                             version 
)
{
    if( pBinFile != NULL )
    {
        // バイナリファイルのバージョンは指定したバージョンより新しいか？
        if( pBinFile->version >= version )
        {
            return TRUE;                
        }        
    }
    return FALSE;
}





//------------------------------------------------------------------------------
NNS_G2D_INLINE BOOL NNS_G2dIsBinFileValid
( 
    const NNSG2dBinaryFileHeader*   pBinFile, 
    u32                             binFileSig, 
    u16                             version 
)
{
    if( pBinFile != NULL )
    {
        // バイトオーダーは正しいか？
        //
        // 古いバージョンコンバータの出力データでは、正しく設定されていません(ゼロ値が代入されています)。
        // 実質的に使用されていないので、チェックは行いません。
        //
        // if( pBinFile->byteOrder == NNS_G2D_LITTLEENDIAN_BITMARK )
        {            
            return NNSi_G2dIsBinFileSignatureValid( pBinFile, binFileSig ) && 
                   NNSi_G2dIsBinFileVersionValid( pBinFile, version );   
        }    
    }
    return FALSE;
}







#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // NNS_G2D_LOAD_H_
