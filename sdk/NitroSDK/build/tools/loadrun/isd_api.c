/*---------------------------------------------------------------------------*
  Project:  NitroSDK - tools - loadrun
  File:     isd_api.c

  Copyright 2005,2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: isd_api.c,v $
  Revision 1.5  2006/01/18 02:11:21  kitase_hirotake
  do-indent

  Revision 1.4  2005/09/05 01:36:06  yasu
  TAB ‚Ì‹ó”’‰»

  Revision 1.3  2005/08/24 12:56:41  yada
  fix a little

  Revision 1.2  2005/08/24 09:53:35  yada
  applied for new ISD library

  Revision 1.1  2005/08/24 07:44:46  yada
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <windows.h>
#include "ISNITRODLL.h"
#include "isd_api.h"

//---- IS-dll functions
NITROTOOLAPI_FINDOPEN ISNTD_FindOpen;
NITROTOOLAPI_FINDNEXT ISNTD_FindNext;
NITROTOOLAPI_FINDCLOSE ISNTD_FindClose;
NITROTOOLAPI_DEVSELECTOR ISNTD_Selector;
NITROTOOLAPI_OPEN ISNTD_DeviceOpen;
NITROTOOLAPI_CHECKCONNECT ISNTD_CheckConnect;
NITROTOOLAPI_CLOSE ISNTD_DeviceClose;
NITROTOOLAPI_WRITEROM ISNTD_WriteROM;
NITROTOOLAPI_READROM ISNTD_ReadROM;
NITROTOOLAPI_RESET ISNTD_Reset;
NITROTOOLAPI_SYNCWRITE ISNTD_SyncWriteROM;
NITROTOOLAPI_SYNCREAD ISNTD_SyncReadROM;
NITROTOOLAPI_STREAMWRITE ISNTD_StreamWrite;
NITROTOOLAPI_STREAMGETWRITABLELEN ISNTD_StreamGetWritableLength;
NITROTOOLAPI_STREAMSETCB ISNTD_SetReceiveStreamCallBackFunction;
NITROTOOLAPI_STREAMCHKRECV ISNTD_StreamCheckStreamReceive;
NITROTOOLAPI_SETSYNCTIMEOUT ISNTD_SetSyncTimeOut;
NITROTOOLAPI_GETDEBUGPRINT ISNTD_GetDebugPrint;
NITROTOOLAPI_ROMSIZE ISNTD_GetROMSize;
NITROTOOLAPI_CHECKPOWER ISNTD_GetPowerStatus;
NITROTOOLAPI_CARDSLOTPOWER ISNTD_CardSlotPower;
NITROTOOLAPI_CARTRIDGESLOTPOWER ISNTD_CartridgeSlotPower;
NITROTOOLAPI_GETLASTERROR ISNTD_GetLastError;
NITROTOOLAPI_GETERRORMESSAGE ISNTD_GetLastErrorMessage;


//---- flag of initialized already
static BOOL ISNTDi_IsInitialized = FALSE;

//---- dll instance
HINSTANCE ISNTD_DllInstance;


#ifdef _declDllFunc
#undef _declDllFunc
#endif
#define _declDllFunc(type,name) (NITROTOOLAPI_##type)GetProcAddress(ISNTD_DllInstance, NITRO_FUNC_##name)

/*---------------------------------------------------------------------------*
  Name:         ISNTD_InitDll

  Description:  initialize IS-dll functions

  Arguments:    None

  Returns:      TRUE if success. FALSE if called already.
 *---------------------------------------------------------------------------*/
BOOL ISNTD_InitDll(void)
{
    //---- check initialized already
    if (ISNTDi_IsInitialized)
    {
        return FALSE;
    }
    ISNTDi_IsInitialized = TRUE;

    //---- load dll
    ISNTD_DllInstance = LoadLibrary(NITROEMUDLL_NAME);

    ISNTD_FindOpen = _declDllFunc(FINDOPEN, FINDOPEN);
    ISNTD_FindNext = _declDllFunc(FINDNEXT, FINDNEXT);
    ISNTD_FindClose = _declDllFunc(FINDCLOSE, FINDCLOSE);
    ISNTD_Selector = _declDllFunc(DEVSELECTOR, SELECTOR);
    ISNTD_DeviceOpen = _declDllFunc(OPEN, OPEN);
    ISNTD_CheckConnect = _declDllFunc(CHECKCONNECT, CHECKCONNECT);
    ISNTD_DeviceClose = _declDllFunc(CLOSE, CLOSE);
    ISNTD_WriteROM = _declDllFunc(WRITEROM, WRITEROM);
    ISNTD_ReadROM = _declDllFunc(READROM, READROM);
    ISNTD_Reset = _declDllFunc(RESET, RESET);
    ISNTD_SyncWriteROM = _declDllFunc(SYNCWRITE, SYNC_WRITEROM);
    ISNTD_SyncReadROM = _declDllFunc(SYNCREAD, SYNC_READROM);
    ISNTD_StreamWrite = _declDllFunc(STREAMWRITE, STREAM_WRITE);
    ISNTD_StreamGetWritableLength = _declDllFunc(STREAMGETWRITABLELEN, STREAM_GETWL);
    ISNTD_SetReceiveStreamCallBackFunction = _declDllFunc(STREAMSETCB, STREAM_SETCB);
    ISNTD_StreamCheckStreamReceive = _declDllFunc(STREAMCHKRECV, STREAM_CHECK);
    ISNTD_SetSyncTimeOut = _declDllFunc(SETSYNCTIMEOUT, SET_SYNCTIMEOUT);
    ISNTD_GetDebugPrint = _declDllFunc(GETDEBUGPRINT, DEBUGPRINT);
    ISNTD_GetROMSize = _declDllFunc(ROMSIZE, ROMSIZE);
    ISNTD_GetPowerStatus = _declDllFunc(CHECKPOWER, CHECKPOWER);
    ISNTD_CardSlotPower = _declDllFunc(CARDSLOTPOWER, CARDSLOTPOWER);
    ISNTD_CartridgeSlotPower = _declDllFunc(CARTRIDGESLOTPOWER, CARTSLOTPOWER);
    ISNTD_GetLastError = _declDllFunc(GETLASTERROR, GETLASTERROR);
    ISNTD_GetLastErrorMessage = _declDllFunc(GETERRORMESSAGE, GETERRORMESSAGE);

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         ISNTD_FreeDll

  Description:  free dll module

  Arguments:    None

  Returns:      TRUE if success
 *---------------------------------------------------------------------------*/
BOOL ISNTD_FreeDll(void)
{
    //---- check initialized already
    if (!ISNTDi_IsInitialized)
    {
        return FALSE;
    }

    FreeLibrary(ISNTD_DllInstance);
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         ISNTD_GetDeviceList

  Description:  get connecting device list

  Arguments:    deviceList : pointer of list
                deviceMax  : max number of list

  Returns:      -1 if FAIL
                >=0 value is num of device which is connected
 *---------------------------------------------------------------------------*/
int ISNTD_GetDeviceList(ISNTDDevice * deviceList, int deviceMax)
{
    NITROFINDHANDLE finder;
    int     count = 0;
    NITRODEVICEID id;
    ISNTDDevice *p = deviceList;


    //---- check device list
    if (!deviceList)
    {
        return -1;
    }

    //---- search devices
    finder = ISNTD_FindOpen();
    if (!finder)
    {
        return -1;
    }

    //---- clear
    {
        int     n;
        for (n = 0; n < deviceMax; n++)
        {
            deviceList[n].type = ISNTD_DEVICE_NONE;
        }
    }

    //---- store devices id to list
    while (ISNTD_FindNext(finder, &id) && count < deviceMax)
    {
        p->ntdId = id;

        if (IS_DEVICE_CGBUSB(id))
        {
            p->type = ISNTD_DEVICE_CGB_EMULATOR_USB;
            p->serial = GET_CGBUSB_SERIAL(id);
        }
        else if (IS_DEVICE_CGBSCSI(id))
        {
            p->type = ISNTD_DEVICE_CGB_EMULATOR_SCSI;
            p->host = (int)GET_CGBSCSI_HOST(id);
            p->serial = (int)GET_NITROUSB_SERIAL(id);
        }
        else if (IS_DEVICE_NITROUSB(id))
        {
            p->type = ISNTD_DEVICE_IS_NITRO_EMULATOR;
            p->serial = (int)GET_NITROUSB_SERIAL(id);
        }
        else if (IS_DEVICE_NITROUIC(id))
        {
            p->type = ISNTD_DEVICE_IS_NITRO_UIC;
            p->serial = (int)GET_NITROUIC_SERIAL(id);
        }
        else
        {
            p->type = ISNTD_DEVICE_UNKNOWN;
            p->serial = 0;
        }

        p++;
        count++;
    }

    return count;
}
