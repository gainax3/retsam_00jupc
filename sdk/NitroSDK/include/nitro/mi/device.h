/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MI - include
  File:     device.h

  Copyright 2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/
#ifndef	NITRO_MI_DEVICE_H_
#define	NITRO_MI_DEVICE_H_


#include <nitro/misc.h>
#include <nitro/types.h>
#include <nitro/platform.h>


#ifdef __cplusplus
extern  "C"
{
#endif


/*---------------------------------------------------------------------------*/
/* declarations */

/* デバイスアクセス関数プロトタイプ */
typedef int (*MIDeviceReadFunction)(void *userdata, void *buffer, u32 offset, u32 length);
typedef int (*MIDeviceWriteFunction)(void *userdata, const void *buffer, u32 offset, u32 length);

/* デバイスアクセス構造体 */
typedef struct MIDevice
{
    void                   *userdata;
    MIDeviceReadFunction    Read;
    MIDeviceWriteFunction   Write;
}
MIDevice;


/*---------------------------------------------------------------------------*/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         MI_InitDevice

  Description:  デバイスを初期化.

  Arguments:    device           初期化するMIDevice構造体.
                userdata         任意のユーザ定義引数.
                read             デバイス読み出し関数のポインタ.
                write            デバイス書き込み関数のポインタ.

  Returns:      None.
 *---------------------------------------------------------------------------*/
PLATFORM_ATTRIBUTE_INLINE
void MI_InitDevice(MIDevice *device, void *userdata,
                   MIDeviceReadFunction read,
                   MIDeviceWriteFunction write)
{
    device->userdata = userdata;
    device->Read = read;
    device->Write = write;
}

/*---------------------------------------------------------------------------*
  Name:         MI_ReadDevice

  Description:  デバイスから読み込み.

  Arguments:    device           MIDevice構造体.
                buffer           転送先メモリ.
                offset           転送元オフセット.
                length           転送サイズ.

  Returns:      成功すれば0以上の値, 失敗すれば負の値.
 *---------------------------------------------------------------------------*/
PLATFORM_ATTRIBUTE_INLINE
int MI_ReadDevice(MIDevice *device, void *buffer, u32 offset, u32 length)
{
    return device->Read(device->userdata, buffer, offset, length);
}

/*---------------------------------------------------------------------------*
  Name:         MI_WriteDevice

  Description:  デバイスへ書き込み.

  Arguments:    device           MIDevice構造体.
                buffer           転送元メモリ.
                offset           転送先オフセット.
                length           転送サイズ.

  Returns:      成功すれば0以上の値, 失敗すれば負の値.
 *---------------------------------------------------------------------------*/
PLATFORM_ATTRIBUTE_INLINE
int MI_WriteDevice(MIDevice *device, const void *buffer, u32 offset, u32 length)
{
    return device->Write(device->userdata, buffer, offset, length);
}


/*---------------------------------------------------------------------------*/


#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /* NITRO_MI_DEVICE_H_ */
/*---------------------------------------------------------------------------*
  $Log: device.h,v $
  Revision 1.1  2007/04/11 08:04:56  yosizaki
  initial upload.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
