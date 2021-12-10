/*---------------------------------------------------------------------------*
  Project:  NitroSDK - PM
  File:     pm.c

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: pm.c,v $
  Revision 1.60  2007/10/26 09:24:50  yada
  enable TIMER0 interrupt for tick system while PM_GoSleepMode()

  Revision 1.59  2007/10/03 00:48:03  okubata_ryoma
  small fix

  Revision 1.58  2007/10/02 07:25:49  okubata_ryoma
  LCD 制御時のマイク電源制御変更

  Revision 1.57  2007/06/08 07:09:39  okubata_ryoma
  LCD OFF 期間が 100ms 未満にならないために wait を変更

  Revision 1.56  2007/02/20 00:28:10  kitase_hirotake
  indent source

  Revision 1.55  2007/01/16 02:38:39  okubata_ryoma
  LCD off 期間の調整

  Revision 1.54  2006/09/13 04:03:25  okubata_ryoma
  WaitBusyでIRQモードでなくても割り込み禁止ならPXIからデータを取得するように変更

  Revision 1.53  2006/05/22 07:01:22  okubata_ryoma
  割り込み禁止中でもPMの同期関数が動作するように修正

  Revision 1.52  2006/03/28 08:33:30  okubata_ryoma
  LCD-OFFになるまで待ち続けるように修正

  Revision 1.51  2006/03/28 08:02:05  okubata_ryoma
  SpinWait150msの追加

  Revision 1.50  2006/03/23 00:29:42  okubata_ryoma
  前回の仕様変更による不具合修正

  Revision 1.49  2006/03/13 01:13:18  okubata_ryoma
  常にLCD-ONで電源を切るように変更

  Revision 1.48  2006/01/18 02:12:27  kitase_hirotake
  do-indent

  Revision 1.47  2005/12/14 02:05:56  yada
  fix the allowed range of command result from ARM7 via PXI for PMIC2

  Revision 1.46  2005/07/28 08:34:39  yada
  Fix PM_GoSleepMode().
  Sleep in LCD off and wakeup, GX status is like that GX_DispOff(). This is fixed.

  Revision 1.45  2005/05/27 02:00:28  yada
  refer accurately only 1 bit in checking battery

  Revision 1.44  2005/02/28 05:26:25  yosizaki
  do-indent.

  Revision 1.43  2005/02/18 07:30:00  yasu
  OS_DisableInterrupts 返値の型修正

  Revision 1.42  2005/02/14 01:03:22  yada
  just fix copyright year

  Revision 1.41  2005/02/10 12:38:17  yada
  fix rule in recovering from sleep mode.

  Revision 1.40  2004/11/02 09:46:58  yada
  just fix comment

  Revision 1.39  2004/10/21 07:56:44  yada
  fix recovery process after sleep mode.
  switch ON/OFF by GX_IsDispOn().

  Revision 1.38  2004/10/08 10:54:24  yada
  fix recovery process after sleep mode.
  that is, add GX_DispOn() and GXS_DispOn()

  Revision 1.37  2004/10/06 08:27:35  yada
  change around sleep callback

  Revision 1.36  2004/10/05 11:30:59  yada
  in sleeping, wait more one frame after turning back light off

  Revision 1.35  2004/10/05 04:13:39  yada
  support pre/post sleep mode callback

  Revision 1.34  2004/10/04 11:22:27  yada
  treat backlight status in sleeping, and recover

  Revision 1.33  2004/09/30 11:10:45  yada
  not change LED status in switching LCD power

  Revision 1.32  2004/09/27 13:45:45  yada
  consider for multi boot child

  Revision 1.31  2004/09/16 09:28:27  yada
  on power off, not turned on LCD after sleep

  Revision 1.30  2004/09/15 09:51:45  yada
  pulling out card in sleep causes power off if specified

  Revision 1.29  2004/09/14 09:16:13  yada
  fix a little

  Revision 1.28  2004/09/13 11:09:45  yada
  consider for cartridge interrupt

  Revision 1.27  2004/09/09 09:49:01  yada
  make LED not blink in power save mode

  Revision 1.26  2004/08/26 08:20:59  yada
  add PM_GetLEDPattern*

  Revision 1.25  2004/08/24 05:43:36  yada
  only change comment

  Revision 1.24  2004/08/23 08:06:16  yada
  add LED self blank

  Revision 1.23  2004/08/16 04:16:10  yada
  change argument of PMi_SetLCDPower().
  let PM_SetLCDPower() be sync ver.

  Revision 1.22  2004/08/10 10:53:23  yada
  only arrange spacing

  Revision 1.21  2004/08/10 10:50:48  yada
  LCD power interval become to be controled with VBlank counter

  Revision 1.20  2004/08/10 01:05:55  yada
  GX_PowerLCD -> GXi_PowerLCD

  Revision 1.19  2004/08/09 09:48:40  yada
  delete PM_SetSoundPower/SoundVolume, PM_GetSoundPower/SoundVolume
  (PM -> PMi)

  Revision 1.18  2004/08/09 05:00:42  yada
  add PM_GetSoundPower/SoundVolume/Amp/AmpGain

  Revision 1.17  2004/08/09 01:57:09  yada
  delete async function from PM_Get*

  Revision 1.16  2004/08/07 07:25:29  yada
  PM_CheckBattery -> PM_GetBattery

  Revision 1.15  2004/08/07 07:14:47  yada
  fix a little

  Revision 1.14  2004/08/07 07:01:45  yada
  PM_SetForcedPower -> PM_ForcedPowerOff

  Revision 1.13  2004/08/06 12:49:18  yada
  fix argument of PM_SetLCDPower() calling

  Revision 1.12  2004/08/06 12:39:50  yada
  delete power save mode proc

  Revision 1.11  2004/08/05 05:57:30  yada
  only arrange spacing

  Revision 1.10  2004/08/05 05:53:34  yada
  create functions about powerSaveMode

  Revision 1.9  2004/08/04 00:43:55  yada
  PM_SetBackLightTop/Bottom -> PM_SetBackLight

  Revision 1.8  2004/08/03 08:50:08  yada
  remove debug print

  Revision 1.7  2004/08/03 08:38:42  yada
  consider for turning LCD off

  Revision 1.6  2004/08/02 06:14:02  yada
  PM_SetLED, PM_WriteRegister, PM_ReadRegister : PM->PMi

  Revision 1.5  2004/08/02 02:30:51  yada
  comment out OS_Printf() line

  Revision 1.4  2004/08/02 02:23:48  yada
  stop->sleep

  Revision 1.3  2004/08/02 00:35:10  yada
  change some argument in pm api.

  Revision 1.2  2004/07/31 01:30:05  yada
  add PM_SetAmp, PM_SetAmpGain, PM_SetVDETVoltage

  Revision 1.1  2004/07/29 08:58:27  yada
  initial release

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro/spi/ARM9/pm.h>
#include <nitro/pxi.h>
#include <nitro/gx.h>
#include <nitro/spi/common/config.h>
#include <nitro/ctrdg.h>
#include <nitro/mb.h>

//----------------
typedef struct
{
    BOOL    lock;
    PMCallback callback;
    void   *callbackArg;

    void   *work;

}
PMiWork;


#define PMi_LCD_POWER_WAIT_MSEC  150
#define PMi_LCD_POWER_WAIT_TICK  (OS_MilliSecondsToTicks( PMi_LCD_POWER_WAIT_MSEC ) * (64 * 2))
#define PMi_LCD_SLEEP_WAIT_MSEC  110
#define PMi_LCD_SLEEP_WAIT_TICK  (OS_MilliSecondsToTicks( PMi_LCD_SLEEP_WAIT_MSEC ) * (64 * 2))

//---- battery flag in the register PMIC_STAT
#define PMi_STAT_BATTERY_MASK	 1

//----------------
#define PMi_SetCallback( callback, arg )     \
    do{                                      \
        PMi_Work.callback    = (callback);    \
        PMi_Work.callbackArg = (arg);         \
    } while(0)

//----------------
inline u32 PMi_MakeData1(u32 bit, u32 seq, u32 command, u32 data)
{
    return (bit) | ((seq) << SPI_PXI_INDEX_SHIFT) | ((command) << 8) | ((data) & 0xff);
}

//----------------
inline u32 PMi_MakeData2(u32 bit, u32 seq, u32 data)
{
    return (bit) | ((seq) << SPI_PXI_INDEX_SHIFT) | ((data) & 0xffff);
}

//---------------- lock
BOOL    PMi_Lock(void);

//---------------- wait unlock
void    PMi_WaitBusy(void);

//---------------- callback
void    PMi_DummyCallback(u32 result, void *arg);

//---------------- callback list
void    PMi_PrependList(PMSleepCallbackInfo **listp, PMSleepCallbackInfo *info);
void    PMi_AppendList(PMSleepCallbackInfo **listp, PMSleepCallbackInfo *info);
void    PMi_DeleteList(PMSleepCallbackInfo **listp, PMSleepCallbackInfo *info);
void    PMi_ExecuteList(PMSleepCallbackInfo *listp);

static PMCallback PMi_Callback;
static u16 PMi_IsInit = FALSE;
static PMiWork PMi_Work;
static PMData16 PMi_RegisterBuffer[PMIC_REG_NUMS];
static volatile BOOL PMi_SyncFlag;
static volatile BOOL PMi_SleepEndFlag;

static OSMutex PMi_Mutex;

//---- VBlank count LCD turned off
static u32 PMi_LCDCount;

//---- sleep callback list
static PMSleepCallbackInfo *PMi_PreSleepCallbackList = NULL;
static PMSleepCallbackInfo *PMi_PostSleepCallbackList = NULL;

//---- mic power config
static u32 PMi_SetAmp(PMAmpSwitch status);
static PMAmpSwitch sAmpSwitch = PM_AMP_OFF;

//================================================================
//      INTERNAL FUNCTIONS
//================================================================
//----------------------------------------------------------------
//  PMi_Lock
//       return FALSE if locked.
//       else lock and return TRUE.
//
static BOOL PMi_Lock(void)
{
    //---- check whether locked
    OSIntrMode enabled = OS_DisableInterrupts();
    if (PMi_Work.lock)
    {
        (void)OS_RestoreInterrupts(enabled);
        return FALSE;
    }
    PMi_Work.lock = TRUE;
    (void)OS_RestoreInterrupts(enabled);

    return TRUE;
}


extern void PXIi_HandlerRecvFifoNotEmpty(void);
//----------------------------------------------------------------
//  PMi_WaitBusy
//       wait while locked
//
static void PMi_WaitBusy(void)
{
    volatile BOOL *p = &PMi_Work.lock;

    while (*p)
    {
        if (OS_GetCpsrIrq() == OS_INTRMODE_IRQ_DISABLE)
        {
            PXIi_HandlerRecvFifoNotEmpty();
        }
    }
}

//----------------------------------------------------------------
//  PMi_DummyCallback
//
static void PMi_DummyCallback(u32 result, void *arg)
{
    *(u32 *)arg = result;
}

//----------------------------------------------------------------
//   PMi_CallCallbackAndUnlock
//
static void PMi_CallCallbackAndUnlock(u32 result)
{
    PMCallback callback;
    void   *arg;

    callback = PMi_Work.callback;
    arg = PMi_Work.callbackArg;

    if (PMi_Work.lock)
    {
        PMi_Work.lock = FALSE;
    }

    if (callback)
    {
        PMi_Work.callback = NULL;
        (callback) (result, arg);
    }
}


//================================================================
//      INIT
//================================================================
/*---------------------------------------------------------------------------*
  Name:         PM_Init

  Description:  initialize PM

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void PM_Init(void)
{
    int     i;

    if (PMi_IsInit)
    {
        return;
    }
    PMi_IsInit = TRUE;

    //---- initialize work area
    PMi_Work.lock = FALSE;
    PMi_Work.callback = NULL;

    //---- wait for till ARM7 PXI library start
    PXI_Init();
    while (!PXI_IsCallbackReady(PXI_FIFO_TAG_PM, PXI_PROC_ARM7))
    {
    }

    //---- set receive callback
    PXI_SetFifoRecvCallback(PXI_FIFO_TAG_PM, PMi_CommonCallback);

    //---- clear register buffer
    for (i = 0; i < PMIC_REG_NUMS; i++)
    {
        PMi_RegisterBuffer[i].flag = FALSE;
    }

    //---- init mutex
    OS_InitMutex(&PMi_Mutex);

    //---- init LCD count
    PMi_LCDCount = OS_GetVBlankCount();
}

//================================================================
//      PXI CALLBACK
//================================================================
/*---------------------------------------------------------------------------*
  Name:         PMi_CommonCallback

  Description:  callback to receive data from PXI

  Arguments:    tag  : tag from PXI (unused)
                data : data from PXI
                err  : error bit

  Returns:      None
 *---------------------------------------------------------------------------*/
void PMi_CommonCallback(PXIFifoTag tag, u32 data, BOOL err)
{
#pragma unused( tag )

    u16     command;
    u16     pxiResult;

    //---- check PXI communication error
    if (err)
    {
        PMi_CallCallbackAndUnlock(PM_RESULT_ERROR);
        return;
    }

    command = (u16)((data & SPI_PXI_RESULT_COMMAND_MASK) >> SPI_PXI_RESULT_COMMAND_SHIFT);
    pxiResult = (u16)((data & SPI_PXI_RESULT_DATA_MASK) >> SPI_PXI_RESULT_DATA_SHIFT);

    //---- dispose for each command
    // read register
    if (SPI_PXI_COMMAND_PM_REG0VALUE <= command && command <= SPI_PXI_COMMAND_PM_REG4VALUE)
    {
        int     num = (int)(command - SPI_PXI_COMMAND_PM_REG0VALUE);
        u16     value = (u16)(pxiResult & 0xff);
        u16    *buffer = PMi_RegisterBuffer[num].buffer;

        if (buffer)
        {
            *buffer = value;
        }
        PMi_RegisterBuffer[num].flag = TRUE;
        pxiResult = PM_RESULT_SUCCESS;
    }

    // sync with arm7
    else if (command == SPI_PXI_COMMAND_PM_SYNC)
    {
        PMi_SyncFlag = TRUE;
    }

    // end of sleep
    else if (command == SPI_PXI_COMMAND_PM_SLEEP_END)
    {
        PMi_SleepEndFlag = TRUE;
    }

    // get current blink pattern
    else if (command == SPI_PXI_COMMAND_PM_GET_BLINK)
    {
        if (PMi_Work.work)
        {
            *(u32 *)PMi_Work.work = pxiResult;
        }
        pxiResult = PM_RESULT_SUCCESS;
    }

    PMi_CallCallbackAndUnlock(pxiResult);
}

//================================================================================
//           SEND COMMAND TO ARM7
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         PMi_SendSleepStart

  Description:  send SLEEP START command to ARM7

  Arguments:    trigger     : factors to wake up | backlight status to recover
                keyIntrData : key pattern and logic in waking up by key interrupt

  Returns:      result of issueing command
                PM_RESULT_BUSY    : busy
                PM_RESULT_SUCCESS : success
 *---------------------------------------------------------------------------*/
u32 PMi_SendSleepStart(u16 trigger, u16 keyIntrData)
{
    u32     pxi_send_data;

    //---- check whether locked
    if (!PMi_Lock())
    {
        return PM_BUSY;
    }

    //---- send SYNC command to ARM7
    PMi_SyncFlag = 0;
    pxi_send_data =
        PMi_MakeData1(SPI_PXI_START_BIT | SPI_PXI_END_BIT, 0, SPI_PXI_COMMAND_PM_SYNC, 0);
    PMi_SendPxiData(pxi_send_data);

    //---- wait
    while (PMi_SyncFlag == 0)
    {
    }
    PMi_SyncFlag = 0;
    PMi_SleepEndFlag = 0;

    //---- turn LCD off
    (void)PMi_SetLCDPower(PM_LCD_POWER_OFF, PM_LED_BLINK_LOW, FALSE, TRUE);

    //---- send SLEEP_START command to ARM7
    pxi_send_data = PMi_MakeData1(SPI_PXI_START_BIT, 0, SPI_PXI_COMMAND_PM_SLEEP_START, trigger);
    PMi_SendPxiData(pxi_send_data);

    pxi_send_data = PMi_MakeData2(SPI_PXI_END_BIT, 1, keyIntrData);
    PMi_SendPxiData(pxi_send_data);

    return PM_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         PM_SendUtilityCommandAsync

  Description:  send utility command to ARM7

  Arguments:    number   : action number
                callback : callback called in finishing proc
                arg      : argument of callback

  Returns:      result of issueing command
                PM_RESULT_BUSY    : busy
                PM_RESULT_SUCCESS : success
 *---------------------------------------------------------------------------*/
u32 PM_SendUtilityCommandAsync(u32 number, PMCallback callback, void *arg)
{
    u32     pxi_send_data;

    //---- check whether locked
    if (!PMi_Lock())
    {
        return PM_BUSY;
    }
    PMi_SetCallback(callback, arg);

    pxi_send_data = PMi_MakeData1(SPI_PXI_START_BIT, 0, SPI_PXI_COMMAND_PM_UTILITY, number >> 16);
    PMi_SendPxiData(pxi_send_data);

    pxi_send_data = PMi_MakeData2(SPI_PXI_END_BIT, 1, number);
    PMi_SendPxiData(pxi_send_data);

    return PM_SUCCESS;
}

//---------------- sync version
u32 PM_SendUtilityCommand(u32 number)
{
    u32     commandResult;
    u32     sendResult = PM_SendUtilityCommandAsync(number, PMi_DummyCallback, &commandResult);
    if (sendResult == PM_SUCCESS)
    {
        PMi_WaitBusy();
        return commandResult;
    }
    return sendResult;
}

/*---------------------------------------------------------------------------*
  Name:         PMi_ReadRegisterAsync

  Description:  send read register command  to ARM7

  Arguments:    registerAddr : PMIC register number (0-3)
                buffer       : buffer to store register value
                callback     : callback function
                arg          : callback argument

  Returns:      result of issueing command
                PM_RESULT_BUSY    : busy
                PM_RESULT_SUCCESS : success
 *---------------------------------------------------------------------------*/
u32 PMi_ReadRegisterAsync(u16 registerAddr, u16 *buffer, PMCallback callback, void *arg)
{
    u32     pxi_send_data;

    //---- check whether locked
    if (!PMi_Lock())
    {
        return PM_BUSY;
    }
    PMi_SetCallback(callback, arg);

    PMi_RegisterBuffer[registerAddr].flag = FALSE;
    PMi_RegisterBuffer[registerAddr].buffer = buffer;

    pxi_send_data =
        PMi_MakeData1(SPI_PXI_START_BIT | SPI_PXI_END_BIT, 0, SPI_PXI_COMMAND_PM_REG_READ,
                      registerAddr);
    PMi_SendPxiData(pxi_send_data);

    return PM_SUCCESS;
}

//---------------- sync version
u32 PMi_ReadRegister(u16 registerAddr, u16 *buffer)
{
    u32     commandResult;
    u32     sendResult =
        PMi_ReadRegisterAsync(registerAddr, buffer, PMi_DummyCallback, &commandResult);
    if (sendResult == PM_SUCCESS)
    {
        PMi_WaitBusy();
        return commandResult;
    }
    return sendResult;
}

/*---------------------------------------------------------------------------*
  Name:         PMi_WriteRegisterAsync

  Description:  send write register command to ARM7

  Arguments:    registerAddr : PMIC register number (0-3)
                data         : data written to PMIC register
                callback     : callback function
                arg          : callback argument

  Returns:      result of issueing command
                PM_RESULT_BUSY    : busy
                PM_RESULT_SUCCESS : success
 *---------------------------------------------------------------------------*/
u32 PMi_WriteRegisterAsync(u16 registerAddr, u16 data, PMCallback callback, void *arg)
{
    u32     pxi_send_data;

    //---- check whether locked
    if (!PMi_Lock())
    {
        return PM_BUSY;
    }
    PMi_SetCallback(callback, arg);

    pxi_send_data = PMi_MakeData1(SPI_PXI_START_BIT, 0, SPI_PXI_COMMAND_PM_REG_WRITE, registerAddr);
    PMi_SendPxiData(pxi_send_data);

    pxi_send_data = PMi_MakeData2(SPI_PXI_END_BIT, 1, data);
    PMi_SendPxiData(pxi_send_data);

    return PM_SUCCESS;
}

//---------------- sync version
u32 PMi_WriteRegister(u16 registerAddr, u16 data)
{
    u32     commandResult;
    u32     sendResult =
        PMi_WriteRegisterAsync(registerAddr, data, PMi_DummyCallback, &commandResult);
    if (sendResult == PM_SUCCESS)
    {
        PMi_WaitBusy();
        return commandResult;
    }
    return sendResult;
}

//================================================================================
//           ACTION ABOUT PMIC
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         PMi_SetLEDAsync / PMi_SetLED

  Description:  change LED status

  Arguments:    status : PM_LED_ON         : on
                         PM_LED_BLINK_HIGH : blinking in high speed
                         PM_LED_BLINK_LOW  : blinking in low speed
                callback : callback function
                arg      : callback argument

  Returns:      result.
                PM_RESULT_SUCCESS  : success to exec command
                PM_INVALID_COMMAND : bad status given
                PM_RESULT_ERROR    : some error occurred in ARM7
                PM_RESULT_BUSY     : busy
 *---------------------------------------------------------------------------*/
//---------------- async version
u32 PMi_SetLEDAsync(PMLEDStatus status, PMCallback callback, void *arg)
{
    u32     command;

    switch (status)
    {
    case PM_LED_ON:
        command = PM_UTIL_LED_ON;
        break;
    case PM_LED_BLINK_HIGH:
        command = PM_UTIL_LED_BLINK_HIGH_SPEED;
        break;
    case PM_LED_BLINK_LOW:
        command = PM_UTIL_LED_BLINK_LOW_SPEED;
        break;
    default:
        command = 0;
    }

    return (command) ? PM_SendUtilityCommandAsync(command, callback, arg) : PM_INVALID_COMMAND;
}

//---------------- sync version
u32 PMi_SetLED(PMLEDStatus status)
{
    u32     commandResult;
    u32     sendResult = PMi_SetLEDAsync(status, PMi_DummyCallback, &commandResult);
    if (sendResult == PM_SUCCESS)
    {
        PMi_WaitBusy();
        return commandResult;
    }
    return sendResult;
}

/*---------------------------------------------------------------------------*
  Name:         PM_SetBackLightAsync / PM_SetBackLight

  Description:  change backlight switch

  Arguments:    targer   : target LCD.
                           PM_LCD_TOP    : top LCD
                           PM_LCD_BOTTOM : bottom LCD
                           PM_LCD_ALL    : top and bottom LCD
                sw       : switch of top LCD.
                           PM_BACKLIGHT_OFF : off
                           PM_BACKLIGHT_ON  : on
                callback : callback function
                arg      : callback argument

  Returns:      result.
                PM_RESULT_SUCCESS  : success to exec command
                PM_INVALID_COMMAND : bad status given
                PM_RESULT_ERROR    : some error occurred in ARM7
                PM_RESULT_BUSY     : busy
 *---------------------------------------------------------------------------*/
//---------------- async version
u32 PM_SetBackLightAsync(PMLCDTarget target, PMBackLightSwitch sw, PMCallback callback, void *arg)
{
    u32     command = 0;

    if (target == PM_LCD_TOP)
    {
        if (sw == PM_BACKLIGHT_ON)
        {
            command = PM_UTIL_LCD2_BACKLIGHT_ON;
        }
        if (sw == PM_BACKLIGHT_OFF)
        {
            command = PM_UTIL_LCD2_BACKLIGHT_OFF;
        }
    }
    else if (target == PM_LCD_BOTTOM)
    {
        if (sw == PM_BACKLIGHT_ON)
        {
            command = PM_UTIL_LCD1_BACKLIGHT_ON;
        }
        if (sw == PM_BACKLIGHT_OFF)
        {
            command = PM_UTIL_LCD1_BACKLIGHT_OFF;
        }
    }
    else if (target == PM_LCD_ALL)
    {
        if (sw == PM_BACKLIGHT_ON)
        {
            command = PM_UTIL_LCD12_BACKLIGHT_ON;
        }
        if (sw == PM_BACKLIGHT_OFF)
        {
            command = PM_UTIL_LCD12_BACKLIGHT_OFF;
        }
    }

    return (command) ? PM_SendUtilityCommandAsync(command, callback, arg) : PM_INVALID_COMMAND;
}

//---------------- sync version
u32 PM_SetBackLight(PMLCDTarget target, PMBackLightSwitch sw)
{
    u32     commandResult;
    u32     sendResult = PM_SetBackLightAsync(target, sw, PMi_DummyCallback, &commandResult);
    if (sendResult == PM_SUCCESS)
    {
        PMi_WaitBusy();
        return commandResult;
    }
    return sendResult;
}

/*---------------------------------------------------------------------------*
  Name:         PMi_SetSoundPowerAsync / PMi_SetSoundPower

  Description:  change sound power switch

  Arguments:    sw       : switch of sound power
                          PM_SOUND_POWER_OFF : off
                          PM_SOUND_POWER_ON  : on
                callback : callback function
                arg      : callback argument

  Returns:      result.
                PM_RESULT_SUCCESS  : success to exec command
                PM_INVALID_COMMAND : bad status given
                PM_RESULT_ERROR    : some error occurred in ARM7
                PM_RESULT_BUSY     : busy
 *---------------------------------------------------------------------------*/
//---------------- async version
u32 PMi_SetSoundPowerAsync(PMSoundPowerSwitch sw, PMCallback callback, void *arg)
{
    u32     command;

    switch (sw)
    {
    case PM_SOUND_POWER_ON:
        command = PM_UTIL_SOUND_POWER_ON;
        break;
    case PM_SOUND_POWER_OFF:
        command = PM_UTIL_SOUND_POWER_OFF;
        break;
    default:
        command = 0;
    }

    return (command) ? PM_SendUtilityCommandAsync(command, callback, arg) : PM_INVALID_COMMAND;
}

//---------------- sync version
u32 PMi_SetSoundPower(PMSoundPowerSwitch sw)
{
    u32     commandResult;
    u32     sendResult = PMi_SetSoundPowerAsync(sw, PMi_DummyCallback, &commandResult);
    if (sendResult == PM_SUCCESS)
    {
        PMi_WaitBusy();
        return commandResult;
    }
    return sendResult;
}

/*---------------------------------------------------------------------------*
  Name:         PMi_SetSoundVolumeAsync / PMi_SetSoundVolume

  Description:  change sound volume control switch

  Arguments:    sw       : switch of sound volume control
                          PM_SOUND_VOLUME_ON  : on
                          PM_SOUND_VOLUME_OFF : off
                callback : callback function
                arg      : callback argument

  Returns:      result.
                PM_RESULT_SUCCESS  : success to exec command
                PM_INVALID_COMMAND : bad status given
                PM_RESULT_ERROR    : some error occurred in ARM7
                PM_RESULT_BUSY     : busy
 *---------------------------------------------------------------------------*/
//---------------- async version
u32 PMi_SetSoundVolumeAsync(PMSoundVolumeSwitch sw, PMCallback callback, void *arg)
{
    u32     command;

    switch (sw)
    {
    case PM_SOUND_VOLUME_ON:
        command = PM_UTIL_SOUND_VOL_CTRL_ON;
        break;
    case PM_SOUND_VOLUME_OFF:
        command = PM_UTIL_SOUND_VOL_CTRL_OFF;
        break;
    default:
        command = 0;
    }

    return (command) ? PM_SendUtilityCommandAsync(command, callback, arg) : PM_INVALID_COMMAND;
}

//---------------- sync version
u32 PMi_SetSoundVolume(PMSoundVolumeSwitch sw)
{
    u32     commandResult;
    u32     sendResult = PMi_SetSoundVolumeAsync(sw, PMi_DummyCallback, &commandResult);
    if (sendResult == PM_SUCCESS)
    {
        PMi_WaitBusy();
        return commandResult;
    }
    return sendResult;
}

/*---------------------------------------------------------------------------*
  Name:         PM_ForceToPowerOffAsync / PM_ForceToPowerOff

  Description:  force to turn off main power

  Arguments:    callback : callback function
                arg      : callback argument

  Returns:      result.
                PM_RESULT_SUCCESS  : success to exec command
                PM_RESULT_ERROR    : some error occurred in ARM7
                PM_RESULT_BUSY     : busy

                If success, maybe you cannot do anything because power if off.
 *---------------------------------------------------------------------------*/
//---------------- async version
u32 PM_ForceToPowerOffAsync(PMCallback callback, void *arg)
{
    PMLCDPower LCDResult;
    PMBackLightSwitch top;
    PMBackLightSwitch bottom;

    /* wait 150ms */
    OS_SpinWait(PMi_LCD_POWER_WAIT_TICK);
    LCDResult = PM_GetLCDPower();
    if (LCDResult != PM_LCD_POWER_ON)
    {
        (void)PM_GetBackLight(&top, &bottom);
        if (top != PM_BACKLIGHT_OFF)
            (void)PM_SetBackLight(PM_LCD_TOP, PM_BACKLIGHT_OFF);
        if (bottom != PM_BACKLIGHT_OFF)
            (void)PM_SetBackLight(PM_LCD_BOTTOM, PM_BACKLIGHT_OFF);

        while (!PM_SetLCDPower(PM_LCD_POWER_ON))
        {
            OS_SpinWait(PMi_LCD_POWER_WAIT_TICK);
        }
    }

    return PM_SendUtilityCommandAsync(PM_UTIL_FORCE_POWER_OFF, callback, arg);
}

//---------------- sync version
u32 PM_ForceToPowerOff(void)
{
    u32     commandResult;
    u32     sendResult = PM_ForceToPowerOffAsync(PMi_DummyCallback, &commandResult);
    if (sendResult == PM_SUCCESS)
    {
        PMi_WaitBusy();
        return commandResult;
    }
    return sendResult;
}

/*---------------------------------------------------------------------------*
  Name:         PM_SetAmpAsync / PM_SetAmp

  Description:  switch amp

  Arguments:    sw       : switch of programable gain amp
                          PM_AMP_ON   : on
                          PM_AMP_OFF  : off
                callback : callback function
                arg      : callback argument

  Returns:      result.
                PM_RESULT_SUCCESS  : success to exec command
                PM_INVALID_COMMAND : bad status given
                PM_RESULT_ERROR    : some error occurred in ARM7
 *---------------------------------------------------------------------------*/
//---------------- async version
u32 PM_SetAmpAsync(PMAmpSwitch status, PMCallback callback, void *arg)
{
    return PMi_WriteRegisterAsync(REG_PMIC_OP_CTL_ADDR, (u16)status, callback, arg);
}

//---------------- sync version
u32 PM_SetAmp(PMAmpSwitch status)
{
    // マイク電源設定保存
    sAmpSwitch = status;
    return PMi_SetAmp(status);
}

static u32 PMi_SetAmp(PMAmpSwitch status)
{
    // LCD-ON 状態なら即変更
    if (PM_GetLCDPower())
    {
        return PMi_WriteRegister(REG_PMIC_OP_CTL_ADDR, (u16)status);
    }
    // LCD-OFF 状態なら LCD-ON になるまで処理遅延
    else
    {
        return PM_RESULT_SUCCESS;
    }
}

/*---------------------------------------------------------------------------*
  Name:         PM_SetAmpGainAsync / PM_SetAmpGain

  Description:  change amp gain

  Arguments:    gain      : gain
                          PM_AMPGAIN_20   : gain=20
                          PM_AMPGAIN_40   : gain=40
                          PM_AMPGAIN_80   : gain=80
                          PM_AMPGAIN_160  : gain=160
                callback : callback function
                arg      : callback argument

  Returns:      result.
                PM_RESULT_SUCCESS  : success to exec command
                PM_INVALID_COMMAND : bad status given
                PM_RESULT_ERROR    : some error occurred in ARM7
 *---------------------------------------------------------------------------*/
//---------------- async version
u32 PM_SetAmpGainAsync(PMAmpGain status, PMCallback callback, void *arg)
{
    return PMi_WriteRegisterAsync(REG_PMIC_PGA_GAIN_ADDR, (u16)status, callback, arg);
}

//---------------- sync version
u32 PM_SetAmpGain(PMAmpGain status)
{
    return PMi_WriteRegister(REG_PMIC_PGA_GAIN_ADDR, (u16)status);
}

//================================================================================
//           GET STATUS FROM ARM7
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         PM_GetBattery

  Description:  get battery status

  Arguments:    batteryBuf  : buffer to store result

  Returns:      result.
                PM_RESULT_SUCCESS  : success to exec command
                PM_RESULT_ERROR    : some error occurred in ARM7
 *---------------------------------------------------------------------------*/
u32 PM_GetBattery(PMBattery *batteryBuf)
{
    u16     reg;
    u32     result;

    if ((result = PMi_ReadRegister(REG_PMIC_STAT_ADDR, &reg)) == PM_SUCCESS)
    {
        if (batteryBuf)
        {
            *batteryBuf =
                (PMBattery)(reg & PMi_STAT_BATTERY_MASK) ? PM_BATTERY_LOW : PM_BATTERY_HIGH;
        }
    }

    return result;
}

/*---------------------------------------------------------------------------*
  Name:         PM_GetBackLight

  Description:  get backlight status

  Arguments:    top    : buffer to set result of top LCD
                bottom : buffer to set result of bottom LCD

  Returns:      result.
                PM_RESULT_SUCCESS  : success to exec command
                PM_RESULT_ERROR    : some error occurred in ARM7
 *---------------------------------------------------------------------------*/
u32 PM_GetBackLight(PMBackLightSwitch *top, PMBackLightSwitch *bottom)
{
    u16     reg;
    u32     result;

    if ((result = PMi_ReadRegister(REG_PMIC_CTL_ADDR, &reg)) == PM_SUCCESS)
    {
        if (top)
        {
            *top = (reg & PMIC_CTL_BKLT2) ? PM_BACKLIGHT_ON : PM_BACKLIGHT_OFF;
        }
        if (bottom)
        {
            *bottom = (reg & PMIC_CTL_BKLT1) ? PM_BACKLIGHT_ON : PM_BACKLIGHT_OFF;
        }
    }

    return result;
}

/*---------------------------------------------------------------------------*
  Name:         PMi_GetSoundPower

  Description:  get status of sound power switch

  Arguments:    swBuf  : buffer to store result

  Returns:      result.
                PM_RESULT_SUCCESS  : success to exec command
                PM_RESULT_ERROR    : some error occurred in ARM7
 *---------------------------------------------------------------------------*/
u32 PMi_GetSoundPower(PMSoundPowerSwitch *swBuf)
{
    u16     reg;
    u32     result;

    if ((result = PMi_ReadRegister(REG_PMIC_CTL_ADDR, &reg)) == PM_SUCCESS)
    {
        if (swBuf)
        {
            *swBuf = (reg & PMIC_CTL_SND_PWR) ? PM_SOUND_POWER_ON : PM_SOUND_POWER_OFF;
        }
    }

    return result;
}

/*---------------------------------------------------------------------------*
  Name:         PMi_GetSoundVolume

  Description:  get status of sound volume control switch

  Arguments:    swBuf : buffer to store result

  Returns:      result.
                PM_RESULT_SUCCESS  : success to exec command
                PM_RESULT_ERROR    : some error occurred in ARM7
 *---------------------------------------------------------------------------*/
u32 PMi_GetSoundVolume(PMSoundVolumeSwitch *swBuf)
{
    u16     reg;
    u32     result;

    if ((result = PMi_ReadRegister(REG_PMIC_CTL_ADDR, &reg)) == PM_SUCCESS)
    {
        if (swBuf)
        {
            *swBuf = (reg & PMIC_CTL_SND_VOLCTRL) ? PM_SOUND_VOLUME_ON : PM_SOUND_VOLUME_OFF;
        }
    }

    return result;
}

/*---------------------------------------------------------------------------*
  Name:         PM_GetAmp

  Description:  get status of amp switch

  Arguments:    swBuf  : buffer to store result

  Returns:      result.
                PM_RESULT_SUCCESS  : success to exec command
                PM_RESULT_ERROR    : some error occurred in ARM7

 *---------------------------------------------------------------------------*/
u32 PM_GetAmp(PMAmpSwitch *swBuf)
{
    u16     reg;
    u32     result;

    if ((result = PMi_ReadRegister(REG_PMIC_OP_CTL_ADDR, &reg)) == PM_SUCCESS)
    {
        if (swBuf)
        {
            *swBuf = (PMAmpSwitch)reg;
        }
    }

    return result;
}

/*---------------------------------------------------------------------------*
  Name:         PM_GetAmpGain

  Description:  get status of amp gain

  Arguments:    gainBuf  : buffer to store result

  Returns:      result.
                PM_RESULT_SUCCESS  : success to exec command
                PM_RESULT_ERROR    : some error occurred in ARM7
 *---------------------------------------------------------------------------*/
u32 PM_GetAmpGain(PMAmpGain *gainBuf)
{
    u16     reg;
    u32     result;

    if ((result = PMi_ReadRegister(REG_PMIC_PGA_GAIN_ADDR, &reg)) == PM_SUCCESS)
    {
        if (gainBuf)
        {
            *gainBuf = (PMAmpGain)reg;
        }
    }

    return result;
}

//================================================================================
//           SEND DATA TO ARM7
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         PMi_SendPxiData

  Description:  send data via PXI

  Arguments:    data : data to send

  Returns:      None
 *---------------------------------------------------------------------------*/
void PMi_SendPxiData(u32 data)
{
    while (PXI_SendWordByFifo(PXI_FIFO_TAG_PM, data, FALSE) != PXI_FIFO_SUCCESS)
    {
        // do nothing
    }
}


//================================================================================
//           SLEEP
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         PM_GoSleepMode

  Description:  go to be in sleep mode

  Arguments:    trigger    : factors to return from being on sleep
                logic      : key logic to key interrupt
                             PM_PAD_LOGIC_AND : occur interrupt at all specified button pushed
                             PM_PAD_LOGIC_OR  : occur interrupt at one of specified buttons pushed
                keyPattern : keys to wakeup

  Returns:      None
 *---------------------------------------------------------------------------*/
void PM_GoSleepMode(PMWakeUpTrigger trigger, PMLogic logic, u16 keyPattern)
{
    BOOL    prepIrq;                   // IME
    OSIntrMode prepIntrMode;           // CPSR-IRQ
    OSIrqMask prepIntrMask;            // IE
    BOOL    powerOffFlag = FALSE;

    PMBackLightSwitch preTop;
    PMBackLightSwitch preBottom;

    u32     preGX;
    u32     preGXS;
    PMLCDPower preLCDPower;

    //---- call pre-callbacks
    PMi_ExecuteList(PMi_PreSleepCallbackList);

    //---- disable all interrupt
    prepIrq = OS_DisableIrq();         // IME       D
    prepIntrMode = OS_DisableInterrupts();      // CPSR-IRQ  D
    prepIntrMask = OS_DisableIrqMask((1 << OS_IRQ_TABLE_MAX) - 1);      // IE        D

    //---- interrupt setting
	{
		// enable PXI from ARM7 and TIMER0(if needed)
		OSIntrMode intr = OS_IE_FIFO_RECV | (OS_IsTickAvailable()? OS_IE_TIMER0: 0 );
		(void)OS_SetIrqMask( intr );
	}
    (void)OS_RestoreInterrupts(prepIntrMode);   // CPSR-IRQ  E
    (void)OS_EnableIrq();              // IME       E

    //---- check card trigger
    if (trigger & PM_TRIGGER_CARD)
    {
        //---- if multi boot child, ignore card check flag
        if (MB_IsMultiBootChild())
        {
            trigger &= ~PM_TRIGGER_CARD;
        }
    }

    //---- check cartridge trigger
    if (trigger & PM_TRIGGER_CARTRIDGE)
    {
        //---- if cartridge not exist, ignore cartridge check flag
        if (!CTRDG_IsExisting())
        {
            trigger &= ~PM_TRIGGER_CARTRIDGE;
        }
    }

    //---- remember gx state
    preGX = reg_GX_DISPCNT;
    preGXS = reg_GXS_DB_DISPCNT;
    preLCDPower = PM_GetLCDPower();

    //---- set backlight off
    (void)PM_GetBackLight(&preTop, &preBottom);
    (void)PM_SetBackLight(PM_LCD_ALL, PM_BACKLIGHT_OFF);

    //---- wait a few frames after backlight off for avoiding appearance of afterimage
    {
        vu32    vcount = OS_GetVBlankCount();
        while (vcount == OS_GetVBlankCount())
        {
        }
        vcount = OS_GetVBlankCount();

        reg_GX_DISPCNT = reg_GX_DISPCNT & ~REG_GX_DISPCNT_MODE_MASK;    // main screen off
        GXS_DispOff();

        while (vcount == OS_GetVBlankCount())
        {
        }
        vcount = OS_GetVBlankCount();
        while (vcount == OS_GetVBlankCount())
        {
        }
    }

    //---- send SLEEP_START command to ARM7
    {
        u16     param;
        param = (u16)(trigger
                      | preTop << PM_BACKLIGHT_RECOVER_TOP_SHIFT
                      | preBottom << PM_BACKLIGHT_RECOVER_BOTTOM_SHIFT);
        while (PMi_SendSleepStart(param, (u16)(logic | keyPattern)) != PM_SUCCESS)
        {
        }
    }

    //==== Halt ================
    OS_Halt();
    //==========================

    //---- check card remove
    if ((trigger & PM_TRIGGER_CARD) && (OS_GetRequestIrqMask() & OS_IE_CARD_IREQ))
    {
        powerOffFlag = TRUE;
    }

    //---- turn LCD on, and restore gx state
    if (!powerOffFlag)
    {
        if (preLCDPower == PM_LCD_POWER_ON)
        {
            (void)PMi_SetLCDPower(PM_LCD_POWER_ON, PM_LED_ON, TRUE, TRUE);
        }
        else
        {
            (void)PMi_SetLED(PM_LED_ON);
        }

        reg_GX_DISPCNT = preGX;
        reg_GXS_DB_DISPCNT = preGXS;
    }
    
    //---- LCD OFF 期間が 100ms 未満にならないための wait 110ms
    OS_SpinWait(PMi_LCD_SLEEP_WAIT_TICK);

    //---- restore all interrupt
    (void)OS_DisableInterrupts();      // CPSR-IRQ  D (IME D in it)
    (void)OS_SetIrqMask(prepIntrMask); // IE        R
    (void)OS_RestoreInterrupts(prepIntrMode);   // CPSR-IRQ  R
    (void)OS_RestoreIrq(prepIrq);      // IME       R

    //---- power off if need
    if (powerOffFlag)
    {
        (void)PM_ForceToPowerOff();
    }

    //---- call post-callbacks
    PMi_ExecuteList(PMi_PostSleepCallbackList);
}

//================================================================================
//             LCD
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         PMi_SetLCDPower

  Description:  turn on/off LCD power

  Arguments:    sw : switch
                   PM_LCD_POWER_ON : on
                   PM_LCD_POWER_ON : off
                led : LED status
                   PM_LED_NONE : not touch
                   PM_LED_ON
                   PM_LED_BLINK_HIGH
                   PM_LED_BLINK_LOW
                skip : whether skip wait
                   TRUE  : skip
                   FALSE : wait
                isSync: whether this function is sync version
                   TRUE  : sync
                   FALSE : async

  Returns:      TRUE   if success
                FALSE  if fail
 *---------------------------------------------------------------------------*/
#define PMi_WAIT_FRAME  7

BOOL PMi_SetLCDPower(PMLCDPower sw, PMLEDStatus led, BOOL skip, BOOL isSync)
{
    switch (sw)
    {
    case PM_LCD_POWER_ON:
        // compare with counter in which LCD power was turned off.
        //     the reason of waiting 100ms: The interval switching LCD power
        //     from OFF to ON must be more than 100ms. if short than that,
        //     PMIC will be shutdowned and never recover oneself.
        if (!skip && OS_GetVBlankCount() - PMi_LCDCount <= PMi_WAIT_FRAME)
        {
            return FALSE;
        }

        //---- LED
        if (led != PM_LED_NONE)
        {
            if (isSync)
            {
                (void)PMi_SetLED(led);
            }
            else
            {
                (void)PMi_SetLEDAsync(led, NULL, NULL);
            }
        }

        (void)GXi_PowerLCD(TRUE);
        // マイク電源状態復帰
        (void)PMi_SetAmp(sAmpSwitch);
        break;

    case PM_LCD_POWER_OFF:
        // マイク電源OFF
        (void)PMi_SetAmp(PM_AMP_OFF);
        (void)GXi_PowerLCD(FALSE);

        //---- remember LCD off timing
        PMi_LCDCount = OS_GetVBlankCount();

        //---- LED
        if (led != PM_LED_NONE)
        {
            if (isSync)
            {
                (void)PMi_SetLED(led);
            }
            else
            {
                (void)PMi_SetLEDAsync(led, NULL, NULL);
            }
        }
        break;

    default:
        // do nothing
        break;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         PM_SetLCDPower

  Description:  turn on/off LCD power.
                when undefined SDK_NO_THREAD (= thread is valid),
                Tick system and Alarm system are needed.

                this function is sync version.

  Arguments:    sw : switch
                   PM_LCD_POWER_ON  : on
                   PM_LCD_POWER_OFF : off

  Returns:      TRUE   if success
                FALSE  if fail. maybe interval of LCD off->on is too short.
 *---------------------------------------------------------------------------*/
BOOL PM_SetLCDPower(PMLCDPower sw)
{
    if (sw != PM_LCD_POWER_ON)
    {
        sw = PM_LCD_POWER_OFF;
    }
    return PMi_SetLCDPower(sw, PM_LED_NONE /*not touch */ , FALSE, TRUE);
}

/*---------------------------------------------------------------------------*
  Name:         PM_GetLCDPower

  Description:  get status of LCD power

  Arguments:    None

  Returns:      status.
                PM_LCD_POWER_ON  : on
                PM_LCD_POWER_OFF : off
 *---------------------------------------------------------------------------*/
PMLCDPower PM_GetLCDPower(void)
{
    return (reg_GX_POWCNT & REG_GX_POWCNT_LCD_MASK) ? PM_LCD_POWER_ON : PM_LCD_POWER_OFF;
}

/*---------------------------------------------------------------------------*
  Name:         PMi_GetLCDOffCount

  Description:  get counter value LCD turned off

  Arguments:    None

  Returns:      counter value
 *---------------------------------------------------------------------------*/
u32 PMi_GetLCDOffCount(void)
{
    return PMi_LCDCount;
}


//================================================================================
//             LED
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         PMi_SendLEDPatternCommandAsync

  Description:  set up LED blink pattern

  Arguments:    pattern  : LED blink pattern
                callback : callback function
                arg      : callback argument

  Returns:      result of issueing command
                PM_RESULT_BUSY    : busy
                PM_RESULT_SUCCESS : success
 *---------------------------------------------------------------------------*/
//---------------- async version
u32 PMi_SendLEDPatternCommandAsync(PMLEDPattern pattern, PMCallback callback, void *arg)
{
    u32     pxi_send_data;

    //---- check whether locked
    if (!PMi_Lock())
    {
        return PM_BUSY;
    }
    PMi_SetCallback(callback, arg);

    pxi_send_data =
        PMi_MakeData1(SPI_PXI_START_BIT | SPI_PXI_END_BIT, 0, SPI_PXI_COMMAND_PM_SELF_BLINK,
                      pattern);
    PMi_SendPxiData(pxi_send_data);

    return PM_SUCCESS;
}

//---------------- sync version
u32 PMi_SendLEDPatternCommand(PMLEDPattern pattern)
{
    u32     commandResult;
    u32     sendResult = PMi_SendLEDPatternCommandAsync(pattern, PMi_DummyCallback, &commandResult);
    if (sendResult == PM_SUCCESS)
    {
        PMi_WaitBusy();
        return commandResult;
    }
    return sendResult;
}

/*---------------------------------------------------------------------------*
  Name:         PMi_GetLEDPattern

  Description:  get current LED blink pattern

  Arguments:    pattern  : LED blink pattern
                callback : callback function
                arg      : callback argument

  Returns:      result of issueing command
                PM_RESULT_BUSY    : busy
                PM_RESULT_SUCCESS : success
 *---------------------------------------------------------------------------*/
//---------------- async version
u32 PM_GetLEDPatternAsync(PMLEDPattern *patternBuf, PMCallback callback, void *arg)
{
    u32     pxi_send_data;

    //---- check whether locked
    if (!PMi_Lock())
    {
        return PM_BUSY;
    }
    PMi_SetCallback(callback, arg);
    PMi_Work.work = (void *)patternBuf;

    pxi_send_data =
        PMi_MakeData1(SPI_PXI_START_BIT | SPI_PXI_END_BIT, 0, SPI_PXI_COMMAND_PM_GET_BLINK, 0);
    PMi_SendPxiData(pxi_send_data);

    return PM_SUCCESS;
}

//---------------- sync version
u32 PM_GetLEDPattern(PMLEDPattern *patternBuf)
{
    u32     commandResult;
    u32     sendResult = PM_GetLEDPatternAsync(patternBuf, PMi_DummyCallback, &commandResult);
    if (sendResult == PM_SUCCESS)
    {
        PMi_WaitBusy();
        return commandResult;
    }
    return sendResult;
}

//================================================================================
//             CALLBACK LIST
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         PMi_PrependList

  Description:  insert callback info to top of callback info list.
                subroutine of PM_Prepend*CallbackInfo().

  Arguments:    listp : callback info list pointer
                info  : callback info to insert

  Returns:      None
 *---------------------------------------------------------------------------*/
void PMi_PrependList(PMSleepCallbackInfo **listp, PMSleepCallbackInfo *info)
{
    if (!listp)
    {
        return;
    }

    info->next = *listp;
    *listp = info;
}

/*---------------------------------------------------------------------------*
  Name:         PMi_AppendList

  Description:  insert callback info to last of callback info list.
                subroutine of PM_Append*CallbackInfo().

  Arguments:    listp : callback info list pointer
                info  : callback info to insert

  Returns:      None
 *---------------------------------------------------------------------------*/
void PMi_AppendList(PMSleepCallbackInfo **listp, PMSleepCallbackInfo *info)
{
    if (!listp)
    {
        return;
    }

    if (!*listp)
    {
        info->next = NULL;
        *listp = info;
    }
    else
    {
        PMSleepCallbackInfo *p = *listp;
        while (p->next)
        {
            p = p->next;
        }

        info->next = p->next;
        p->next = info;
    }
}

/*---------------------------------------------------------------------------*
  Name:         PMi_DeleteList

  Description:  delete callback info from callback info list.
                subroutine of PM_Delete*CallbackInfo().

  Arguments:    listp : callback info list pointer
                info  : callback info to delete

  Returns:      None
 *---------------------------------------------------------------------------*/
void PMi_DeleteList(PMSleepCallbackInfo **listp, PMSleepCallbackInfo *info)
{
    PMSleepCallbackInfo *p = *listp;
    PMSleepCallbackInfo *pre;

    if (!listp)
    {
        return;
    }

    pre = p = *listp;
    while (p)
    {
        //---- one to delete?
        if (p == info)
        {
            if (p == pre)
            {
                *listp = p->next;
            }
            else
            {
                pre->next = p->next;
            }
            break;
        }

        pre = p;
        p = p->next;
    }
}

/*---------------------------------------------------------------------------*
  Name:         PMi_ExecuteList

  Description:  execute each callback registerd to info list

  Arguments:    listp : callback info list pointer

  Returns:      None
 *---------------------------------------------------------------------------*/
void PMi_ExecuteList(PMSleepCallbackInfo *listp)
{
    while (listp)
    {
        (listp->callback) (listp->arg);

        listp = listp->next;
    }
}

/*---------------------------------------------------------------------------*
  Name:         PM_AppendPreSleepCallback

  Description:  append callback info to pre-callback info list

  Arguments:    info : callback info to append

  Returns:      None
 *---------------------------------------------------------------------------*/
void PM_AppendPreSleepCallback(PMSleepCallbackInfo *info)
{
    PMi_AppendList(&PMi_PreSleepCallbackList, info);
}

/*---------------------------------------------------------------------------*
  Name:         PM_PrependPreSleepCallback

  Description:  prepend callback info to pre-callback info list

  Arguments:    info : callback info to prepend

  Returns:      None
 *---------------------------------------------------------------------------*/
void PM_PrependPreSleepCallback(PMSleepCallbackInfo *info)
{
    PMi_PrependList(&PMi_PreSleepCallbackList, info);
}

/*---------------------------------------------------------------------------*
  Name:         PM_AppendPostSleepCallback

  Description:  append callback info to post-callback info list

  Arguments:    info : callback info to append

  Returns:      None
 *---------------------------------------------------------------------------*/
void PM_AppendPostSleepCallback(PMSleepCallbackInfo *info)
{
    PMi_AppendList(&PMi_PostSleepCallbackList, info);
}

/*---------------------------------------------------------------------------*
  Name:         PM_PrependPostSleepCallback

  Description:  prepend callback info to post-callback info list

  Arguments:    info : callback info to prepend

  Returns:      None
 *---------------------------------------------------------------------------*/
void PM_PrependPostSleepCallback(PMSleepCallbackInfo *info)
{
    PMi_PrependList(&PMi_PostSleepCallbackList, info);
}

/*---------------------------------------------------------------------------*
  Name:         PM_DeletePreSleepCallback

  Description:  delete callback info from pre-callback info list

  Arguments:    info : callback info to delete

  Returns:      None
 *---------------------------------------------------------------------------*/
void PM_DeletePreSleepCallback(PMSleepCallbackInfo *info)
{
    PMi_DeleteList(&PMi_PreSleepCallbackList, info);
}

/*---------------------------------------------------------------------------*
  Name:         PM_DeletePostSleepCallback

  Description:  delete callback info from post-callback info list

  Arguments:    info : callback info to delete

  Returns:      None
 *---------------------------------------------------------------------------*/
void PM_DeletePostSleepCallback(PMSleepCallbackInfo *info)
{
    PMi_DeleteList(&PMi_PostSleepCallbackList, info);
}
