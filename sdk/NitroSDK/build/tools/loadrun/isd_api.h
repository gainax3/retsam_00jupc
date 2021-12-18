/*---------------------------------------------------------------------------*
  Project:  NitroSDK - tools - loadrun
  File:     isd_api.h

  Copyright 2005,2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: isd_api.h,v $
  Revision 1.4  2006/01/18 02:11:21  kitase_hirotake
  do-indent

  Revision 1.3  2005/09/05 01:36:43  yasu
  TAB の空白化

  Revision 1.2  2005/08/24 09:53:35  yada
  applied for new ISD library

  Revision 1.1  2005/08/24 07:44:46  yada
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NITRO_LOADRUN_ISD_API_H_
#define NITRO_LOADRUN_ISD_API_H_

#ifdef __cplusplus
extern "C" {
#endif

#if 0
extern NITROTOOLAPI_FINDOPEN NITROToolAPIFindOpen;
extern NITROTOOLAPI_FINDNEXT NITROToolAPIFindNext;
extern NITROTOOLAPI_FINDCLOSE NITROToolAPIFindClose;
extern NITROTOOLAPI_DEVSELECTOR NITROToolAPISelector;

extern NITROTOOLAPI_OPEN NITROToolAPIDeviceOpen;
extern NITROTOOLAPI_CHECKCONNECT NITROToolAPICheckConnect;
extern NITROTOOLAPI_CLOSE NITROToolAPIDeviceClose;

extern NITROTOOLAPI_RESET NITROToolAPIReset;
extern NITROTOOLAPI_READROM NITROToolAPIReadROM;
extern NITROTOOLAPI_WRITEROM NITROToolAPIWriteROM;
extern NITROTOOLAPI_SYNCWRITE NITROToolAPISyncWriteROM;
extern NITROTOOLAPI_SYNCREAD NITROToolAPISyncReadROM;
extern NITROTOOLAPI_ROMSIZE NITROToolAPIGetROMSize;
extern NITROTOOLAPI_SETSYNCTIMEOUT NITROToolAPISetSyncTimeout;
extern NITROTOOLAPI_CHECKPOWER NITROToolAPIGetPowerStatus;
extern NITROTOOLAPI_CARDSLOTPOWER NITROToolAPICardSlotPower;
extern NITROTOOLAPI_CARTRIDGESLOTPOWER NITROToolAPICartridgeSlotPower;

// ストリームベースのデータ送受信
extern NITROTOOLAPI_STREAMWRITE NITROToolAPIStreamWrite;
extern NITROTOOLAPI_STREAMGETWRITABLELEN NITROToolAPIStreamGetWritableLength;
extern NITROTOOLAPI_STREAMSETCB NITROToolAPISetReceiveStreamCallBackFunction;
extern NITROTOOLAPI_STREAMCHKRECV NITROToolAPICheckStreamReceive;

// デバッグプリントの取得系
extern NITROTOOLAPI_GETDEBUGPRINT NITROToolAPIGetDebugPrint;

// エラー取得系
extern NITROTOOLAPI_GETLASTERROR NITROToolAPIGetLastError;
extern NITROTOOLAPI_GETERRORMESSAGE NITROToolAPIGetErrorMessage;
#endif

// public function
extern NITROTOOLAPI_FINDOPEN ISNTD_FindOpen;
extern NITROTOOLAPI_FINDNEXT ISNTD_FindNext;
extern NITROTOOLAPI_FINDCLOSE ISNTD_FindClose;
extern NITROTOOLAPI_DEVSELECTOR ISNTD_Selector;
extern NITROTOOLAPI_OPEN ISNTD_DeviceOpen;
extern NITROTOOLAPI_CHECKCONNECT ISNTD_CheckConnect;
extern NITROTOOLAPI_CLOSE ISNTD_DeviceClose;
extern NITROTOOLAPI_WRITEROM ISNTD_WriteROM;
extern NITROTOOLAPI_READROM ISNTD_ReadROM;
extern NITROTOOLAPI_RESET ISNTD_Reset;
extern NITROTOOLAPI_SYNCWRITE ISNTD_SyncWriteROM;
extern NITROTOOLAPI_SYNCREAD ISNTD_SyncReadROM;
extern NITROTOOLAPI_STREAMWRITE ISNTD_StreamWrite;
extern NITROTOOLAPI_STREAMGETWRITABLELEN ISNTD_StreamGetWritableLength;
extern NITROTOOLAPI_STREAMSETCB ISNTD_SetReceiveStreamCallBackFunction;
extern NITROTOOLAPI_STREAMCHKRECV ISNTD_StreamCheckStreamReceive;
extern NITROTOOLAPI_SETSYNCTIMEOUT ISNTD_SetSyncTimeOut;
extern NITROTOOLAPI_GETDEBUGPRINT ISNTD_GetDebugPrint;
extern NITROTOOLAPI_ROMSIZE ISNTD_GetROMSize;
extern NITROTOOLAPI_CHECKPOWER ISNTD_GetPowerStatus;
extern NITROTOOLAPI_CARDSLOTPOWER ISNTD_CardSlotPower;
extern NITROTOOLAPI_CARTRIDGESLOTPOWER ISNTD_CartridgeSlotPower;
extern NITROTOOLAPI_GETLASTERROR ISNTD_GetLastError;
extern NITROTOOLAPI_GETERRORMESSAGE ISNTD_GetLastErrorMessage;


//---- デバイス種類
#define ISNTD_DEVICE_NONE                   0
#define ISNTD_DEVICE_CGB_EMULATOR_USB       1
#define ISNTD_DEVICE_CGB_EMULATOR_SCSI      2
#define ISNTD_DEVICE_IS_NITRO_EMULATOR      3
#define ISNTD_DEVICE_IS_NITRO_UIC           4
#define ISNTD_DEVICE_UNKNOWN                5

typedef struct ISNTDDevice ISNTDDevice;
struct ISNTDDevice
{
    NITRODEVICEID ntdId;
    int     type;
    int     host;
    int     serial;
};


/*---------------------------------------------------------------------------*
  Name:         ISNTD_InitDll

  Description:  initialize dll

  Arguments:    None

  Returns:      TRUE if success
 *---------------------------------------------------------------------------*/
extern BOOL ISNTD_InitDll(void);

/*---------------------------------------------------------------------------*
  Name:         ISNTD_FreeDll

  Description:  free dll module

  Arguments:    None

  Returns:      TRUE if success
 *---------------------------------------------------------------------------*/
extern BOOL ISNTD_FreeDll(void);


/*---------------------------------------------------------------------------*
  Name:         ISNTD_GetDeviceList

  Description:  get connecting device list

  Arguments:    deviceList : pointer of list
                deviceMax  : max number of list

  Returns:      -1 if FAIL
                >=0 value is num of device which is connected
 *---------------------------------------------------------------------------*/
extern int ISNTD_GetDeviceList(ISNTDDevice * deviceList, int deviceMax);


#ifdef __cplusplus
} /* extern "C" */
#endif

/* NITRO_LOADRUN_ISD_API_H_ */
#endif
