/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - mcs
  File:     base_win32.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.3 $
 *---------------------------------------------------------------------------*/

#ifndef NNS_MCS_BASE_WIN32_H_
#define NNS_MCS_BASE_WIN32_H_

#ifdef __cplusplus
extern "C" {
#endif


/* ========================================================================
    マクロ定義
   ======================================================================== */

#ifdef NNS_MCS_EXPORTS
    #define NNS_MCS_IMPORT __declspec(dllexport)
#else
    #define NNS_MCS_IMPORT __declspec(dllimport)
#endif

#define	NNS_MCS_MODULE_NAME         _T("nnsmcs.dll")

#define	NNS_MCS_API_OPENSTREAM      "NNS_McsOpenStream"
#define	NNS_MCS_API_OPENSTREAMEX    "NNS_McsOpenStreamEx"


/* ========================================================================
    定数定義
   ======================================================================== */

enum
{
    NNS_MCS_DEVICE_TYPE_UNKNOWN,
    // NNS_MCS_DEVICE_TYPE_CGB_SCSI,
    // NNS_MCS_DEVICE_TYPE_CGB_USB,
    NNS_MCS_DEVICE_TYPE_NITRO_DEBUGGER,
    NNS_MCS_DEVICE_TYPE_NITRO_UIC,
    NNS_MCS_DEVICE_TYPE_ENSATA
};


/* =======================================================================
    型定義
   ======================================================================== */

struct NNSMcsStreamInfo
{
    DWORD   structBytes;
    DWORD   deviceType;
};

typedef HANDLE      (WINAPI *NNSMcsPFOpenStream)(
                        USHORT      channel,
                        DWORD       flags);


typedef HANDLE      (WINAPI *NNSMcsPFOpenStreamEx)(
                        USHORT              channel,
                        DWORD               flags,
                        NNSMcsStreamInfo*   pStreamInfo);


/* ========================================================================
    関数プロトタイプ
   ======================================================================== */

NNS_MCS_IMPORT HANDLE WINAPI    NNS_McsOpenStream(
                                    USHORT      channel,
                                    DWORD       flags);

NNS_MCS_IMPORT HANDLE WINAPI    NNS_McsOpenStreamEx(
                                    USHORT              channel,
                                    DWORD               flags,
                                    NNSMcsStreamInfo*   pStreamInfo);


#ifdef __cplusplus
} /* extern "C" */
#endif

/* NNS_MCS_BASE_WIN32_H_ */
#endif
