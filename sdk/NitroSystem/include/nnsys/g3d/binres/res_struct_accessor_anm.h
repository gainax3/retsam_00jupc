/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g3d - binres
  File:     res_struct_accessor_anm.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.17 $
 *---------------------------------------------------------------------------*/

//
// AUTHOR: Kenji Nishida
//

#ifndef NNSG3D_BINRES_RES_STRUCT_ACCESSOR_ANM_H_
#define NNSG3D_BINRES_RES_STRUCT_ACCESSOR_ANM_H_

#include <nnsys/g3d/binres/res_struct.h>


#ifdef __cplusplus
extern "C" {
#endif

//
// Common
//
const void* 
NNSi_G3dGetBinaryBlockFromFile
( 
    const u8*  pFileHead, 
    u32        fileSignature, 
    u32        blockSignature 
);

BOOL NNSi_G3dIsValidAnmRes(const void* pRes);
void* NNS_G3dGetAnmByIdx(const void* pRes, u32 idx);
void* NNS_G3dGetAnmByName(const void* pRes, const NNSG3dResName* pName);

//
// NCBVA
//
const NNSG3dResVisAnm*
NNS_G3dGetVisAnmByIdx( const NNSG3dResVisAnmSet* pAnmSet, u8 idx  );

const NNSG3dResVisAnmSet*
NNS_G3dGetVisAnmSet( const u8* pFileHead );


//
// NCBTP
//

const NNSG3dResName* 
NNSi_G3dGetTexPatAnmTexNameByIdx( const NNSG3dResTexPatAnm* pPatAnm, u8 texIdx );

const NNSG3dResName* 
NNSi_G3dGetTexPatAnmPlttNameByIdx( const NNSG3dResTexPatAnm* pPatAnm, u8 plttIdx );


const NNSG3dResTexPatAnmFV* 
NNSi_G3dGetTexPatAnmFVByFVIndex( const NNSG3dResTexPatAnm* pPatAnm, u32 idx, u32 fvIdx );

const NNSG3dResTexPatAnmFV* 
NNSi_G3dGetTexPatAnmFV( const NNSG3dResTexPatAnm* pPatAnm, u32 idx, u32 frame );




const NNSG3dResDictTexPatAnmData* 
NNSi_G3dGetTexPatAnmDataByIdx( const NNSG3dResTexPatAnm* pPatAnm, u32 idx );

const NNSG3dResTexPatAnm*
NNS_G3dGetTexPatAnmByIdx( const NNSG3dResTexPatAnmSet* pAnmSet, u8 idx );

const NNSG3dResTexPatAnmSet*
NNS_G3dGetTexPatAnmSet( const u8* pFileHead );


//
// NSBTA
//
const NNSG3dResTexSRTAnm*
NNS_G3dGetTexSRTAnmByIdx( const NNSG3dResTexSRTAnmSet* pTexSRTSet, u8 idx );

const NNSG3dResTexSRTAnmSet*
NNS_G3dGetTexSRTAnmSet( const u8* pFileHead );


//
// NSBMA
//
const NNSG3dResMatCAnm*
NNS_G3dGetMatCAnmByIdx( const NNSG3dResMatCAnmSet* pAnmSet, u8 idx );

const NNSG3dResMatCAnmSet*
NNS_G3dGetMatCAnmSet( const u8* pFileHead );


//
// NSBCA
//
const NNSG3dResJntAnmSRTTag*
NNS_G3dGetJntAnmSRTTag
(
    const NNSG3dResJntAnm*       pJntAnm, 
    int                          nodeIdx
);

const NNSG3dResJntAnm* 
NNS_G3dGetJntAnmByIdx
( 
    const NNSG3dResJntAnmSet*    pJntAnmSet, 
    int                          idx 
);

const NNSG3dResJntAnmSet*
NNS_G3dGetJntAnmSet( const u8* pFileHead );


#ifdef __cplusplus
}/* extern "C" */
#endif

#endif // NNSG3D_BINRES_RES_STRUCT_ACCESSOR_ANM_H_