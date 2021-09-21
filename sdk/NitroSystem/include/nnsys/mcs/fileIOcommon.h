/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - mcs
  File:     fileIOcommon.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.10 $
 *---------------------------------------------------------------------------*/

#ifndef NNS_MCS_FILEIOCOMMON_H_
#define NNS_MCS_FILEIOCOMMON_H_

#ifdef __cplusplus
extern "C" {
#endif


/* ========================================================================
    定数定義
   ======================================================================== */

enum
{
    NNS_MCS_FILEIO_ERROR_SUCCESS        = 0,
    NNS_MCS_FILEIO_ERROR_COMERROR,
    NNS_MCS_FILEIO_ERROR_NOTCONNECT,
    NNS_MCS_FILEIO_ERROR_SERVERERROR,
    NNS_MCS_FILEIO_ERROR_NOMOREFILES,

    NNSi_MCS_FILEIO_RESULT_READING      = 0x80
};


enum
{
    NNS_MCS_FILEIO_FLAG_READ      = 1 << 0,
    NNS_MCS_FILEIO_FLAG_WRITE     = 1 << 1,
    // NNS_MCS_FILEIO_FLAG_OVERWRITE = 1 << 2,
    NNS_MCS_FILEIO_FLAG_FORCE     = 1 << 3,
    NNS_MCS_FILEIO_FLAG_INCENVVAR = 1 << 4,
    NNS_MCS_FILEIO_FLAG_CREATEDIR = 1 << 5
};

//  windef.h の MAX_PATH と同じ
#define NNS_MCS_FILEIO_PATH_MAX 260

enum
{
    NNS_MCS_FILEIO_SEEK_BEGIN,
    NNS_MCS_FILEIO_SEEK_CURRENT,
    NNS_MCS_FILEIO_SEEK_END
};

// winnt.h で定義されているファイル属性の値と同じ
enum
{
    NNS_MCS_FILEIO_ATTRIBUTE_READONLY             = 0x00000001,
    NNS_MCS_FILEIO_ATTRIBUTE_HIDDEN               = 0x00000002,
    NNS_MCS_FILEIO_ATTRIBUTE_SYSTEM               = 0x00000004,
    NNS_MCS_FILEIO_ATTRIBUTE_DIRECTORY            = 0x00000010,
    NNS_MCS_FILEIO_ATTRIBUTE_ARCHIVE              = 0x00000020,
    NNS_MCS_FILEIO_ATTRIBUTE_DEVICE               = 0x00000040,
    NNS_MCS_FILEIO_ATTRIBUTE_NORMAL               = 0x00000080,
    NNS_MCS_FILEIO_ATTRIBUTE_TEMPORARY            = 0x00000100,
    NNS_MCS_FILEIO_ATTRIBUTE_SPARSE_FILE          = 0x00000200,
    NNS_MCS_FILEIO_ATTRIBUTE_REPARSE_POINT        = 0x00000400,
    NNS_MCS_FILEIO_ATTRIBUTE_COMPRESSED           = 0x00000800,
    NNS_MCS_FILEIO_ATTRIBUTE_OFFLINE              = 0x00001000,
    NNS_MCS_FILEIO_ATTRIBUTE_NOT_CONTENT_INDEXED  = 0x00002000,
    NNS_MCS_FILEIO_ATTRIBUTE_ENCRYPTED            = 0x00004000,

    NNS_MCS_FILEIO_ATTRIBUTE_LAST
};


#ifdef __cplusplus
} /* extern "C" */
#endif

/* NNS_MCS_FILEIOCOMMON_H_ */
#endif
