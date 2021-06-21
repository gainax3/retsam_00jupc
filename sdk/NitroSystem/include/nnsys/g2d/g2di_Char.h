/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g2d
  File:     g2di_Char.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.2 $
 *---------------------------------------------------------------------------*/
#ifndef G2DI_CHAR_H_
#define G2DI_CHAR_H_

#ifdef __cplusplus
extern "C" {
#endif



//---------------------------------------------------------------------
#ifdef NNS_G2D_UNICODE
    #define NNS_G2D_TRANSCODE(str)  L##str
    typedef wchar_t NNSG2dChar;
#else   // NNS_G2D_UNICODE
    #define NNS_G2D_TRANSCODE(str)  str
    typedef char NNSG2dChar;
#endif  // NNS_G2D_UNICODE
//---------------------------------------------------------------------




#ifdef __cplusplus
}/* extern "C" */
#endif

#endif // G2DI_CHAR_H_

