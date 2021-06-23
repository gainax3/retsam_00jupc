/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS
  File:     os_reset.c

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: os_reset.c,v $
  Revision 1.25  2007/08/22 05:23:11  yosizaki
  fix about code32.h

  Revision 1.24  2007/02/27 10:32:13  okubata_ryoma
  リセット時のspをdtcmに退避するように変更

  Revision 1.23  2006/08/10 00:04:00  okubata_ryoma
  カートリッジの活線挿抜に関する不具合修正(リセット時のクリア領域の変更)

  Revision 1.22  2006/01/18 02:11:30  kitase_hirotake
  do-indent

  Revision 1.21  2006/01/10 06:08:10  okubata_ryoma
  small fix

  Revision 1.20  2005/12/08 07:06:09  okubata_ryoma
  small fix

  Revision 1.19  2005/04/01 09:38:11  yosizaki
  fix around OS_ResetSystem().

  Revision 1.18  2005/02/28 05:26:29  yosizaki
  do-indent.

  Revision 1.17  2005/02/18 06:33:16  yasu
  asm{} 節内 indent 修正

  Revision 1.16  2005/02/18 06:22:33  yasu
  Signed/Unsigned 変換警告抑制

  Revision 1.15  2005/02/14 01:01:55  yada
  just fix copyright year

  Revision 1.14  2005/02/10 11:06:45  yada
  insert DC_WaitWriteBufferEmpty() after DC_FlushAll()

  Revision 1.13  2004/12/08 01:13:46  yosizaki
  change OSi_ReloadRomData to use HW_ROM_BASE_OFFSET_BUF.

  Revision 1.12  2004/11/18 01:46:37  yada
  recover OSi_CpuClear32

  Revision 1.11  2004/11/02 09:52:48  yada
  just fix comment

  Revision 1.10  2004/10/29 08:46:20  yada
  remove OSi_CpuClear32

  Revision 1.9  2004/10/29 05:54:15  yada
  move OSi_CpuClear32 to itcm

  Revision 1.8  2004/10/28 13:02:22  yada
  change how to check card locked

  Revision 1.7  2004/10/28 06:33:32  yada
  in reset, stop DMA, check card bus, arrange interrupt mask

  Revision 1.6  2004/10/04 02:33:44  yada
  SND_BeginSleep() and SND_EndSleep() -> SND_Shutdown()

  Revision 1.5  2004/10/04 02:08:29  yada
  stop sound clock before reset system

  Revision 1.4  2004/09/22 12:44:37  yada
  for DEBUGGER , save exception vector when reset

  Revision 1.3  2004/09/09 12:17:12  yada
  On multiboot-child mode, OS_ResetSystem() causes OS_Panic().

  Revision 1.2  2004/09/01 08:37:31  yada
  OS_Reset -> OS_ResetSystem

  Revision 1.1  2004/09/01 04:21:59  yada
  initial release

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include        <nitro.h>
#include        <nitro/mb.h>
#include        <nitro/snd/common/global.h>
#include        <nitro/card.h>

#include        <nitro/code32.h>

//---- IRQ stack size from LCF
extern void SDK_IRQ_STACKSIZE(void);

//---- PXI command
#define OS_PXI_COMMAND_RESET      0x10

//---- PXI command field
#define OS_PXI_COMMAND_MASK       0x00007f00
#define OS_PXI_COMMAND_SHIFT      8
#define OS_PXI_DATA_MASK          0x000000ff
#define OS_PXI_DATA_SHIFT         0

//---- autoload start
#ifdef SDK_ARM9
#define OSi_HW_DTCM               SDK_AUTOLOAD_DTCM_START
#endif

//---------------- local functions
#ifdef SDK_ARM9
static void OSi_CpuClear32(register u32 data, register void *destp, register u32 size);
static void OSi_ReloadRomData(void);
static void OSi_ReadCardRom32(u32 src, void *dst, int len);
#endif // ifdef SDK_ARM9

static void OSi_CommonCallback(PXIFifoTag tag, u32 data, BOOL err);
static void OSi_DoResetSystem(void);
static void OSi_DoBoot(void);
static void OSi_SendToPxi(u16 data);

//---------------- local variables
static u16 OSi_IsInitReset = FALSE;
static vu16 OSi_IsResetOccurred = FALSE;

//================================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_InitReset

  Description:  init reset system

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OS_InitReset(void)
{
    if (OSi_IsInitReset)
    {
        return;
    }
    OSi_IsInitReset = TRUE;

    //---------------- ARM9
#ifdef SDK_ARM9
    //---- wait ARM7 PXI library start
    PXI_Init();
    while (!PXI_IsCallbackReady(PXI_FIFO_TAG_OS, PXI_PROC_ARM7))
    {
    }
#endif

    //---- set PXI receive callback
    PXI_SetFifoRecvCallback(PXI_FIFO_TAG_OS, OSi_CommonCallback);
}

/*---------------------------------------------------------------------------*
  Name:         OS_IsResetOccurred

  Description:  get status whethre reset event occurred

  Arguments:    None.

  Returns:      TRUE  : if reset event occurred
                FALSE : if not
 *---------------------------------------------------------------------------*/
BOOL OS_IsResetOccurred(void)
{
    return OSi_IsResetOccurred;
}

/*---------------------------------------------------------------------------*
  Name:         OSi_CommonCallback

  Description:  PXI callback for OS tag

  Arguments:    tag  : not used
                data : data sent from another processor
                err  : not used

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void OSi_CommonCallback(PXIFifoTag tag, u32 data, BOOL err)
{
#pragma unused( tag, err )
    u16     command;
    u16     commandArg;

    command = (u16)((data & OS_PXI_COMMAND_MASK) >> OS_PXI_COMMAND_SHIFT);
    commandArg = (u16)((data & OS_PXI_DATA_MASK) >> OS_PXI_DATA_SHIFT);

    //---------------- ARM9
#ifdef SDK_ARM9
    //---- reveice reset command (mean ARM7 is prepared to do reset)
    if (command == OS_PXI_COMMAND_RESET)
    {
        OSi_IsResetOccurred = TRUE;
    }
    //---- unknown command
    else
    {
#ifndef SDK_FINALROM
        OS_Panic("unknown command");
#else
        OS_Panic("");
#endif
    }

    //---------------- ARM7
#else
    //---- reveice reset command (mean ARM9 request for ARM7 to do reset)
    if (command == OS_PXI_COMMAND_RESET)
    {
        OSi_IsResetOccurred = TRUE;
    }
    //---- unknown command
    else
    {
#ifndef SDK_FINALROM
        OS_Panic("unknown command");
#else
        OS_Panic("");
#endif
    }
#endif
}

/*---------------------------------------------------------------------------*
  Name:         OSi_SendToPxi

  Description:  send data to PXI interface

  Arguments:    data : data to be sent

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void OSi_SendToPxi(u16 data)
{
    u32     pxi_send_data;

    pxi_send_data = ((u32)data) << OS_PXI_COMMAND_SHIFT;
    while (PXI_SendWordByFifo(PXI_FIFO_TAG_OS, pxi_send_data, FALSE) != PXI_FIFO_SUCCESS)
    {
    }
}

/*---------------------------------------------------------------------------*
  Name:         OS_ResetSystem

  Description:  Start reset sequence.
                To decrease code in itcm or wram,
                separate this function from OSi_DoResetSystem()

  Arguments:    (ARM9) parameter : reset parameter
                (ARM7) None

  Returns:      None
 *---------------------------------------------------------------------------*/
#define RESET_HW_DTCM_IRQ_STACK_END SDK_AUTOLOAD_DTCM_START + 0x00003fc0 - HW_SVC_STACK_SIZE

//---------------- ARM9
#ifdef SDK_ARM9
void OS_ResetSystem(u32 parameter)
{
    //---- stop if reset from multiboot child
    if (MB_IsMultiBootChild())
    {
#ifndef SDK_FINALROM
        OS_Panic("cannot reset from MB child");
#else
        OS_Panic("");
#endif
    }

    //---- wait while card is locked
    {
        u16     id = (u16)OS_GetLockID();
        CARD_LockRom(id);
    }

    //---- stop all dma
    MI_StopDma(0);
    MI_StopDma(1);
    MI_StopDma(2);
    MI_StopDma(3);

    //---- set only pxi interrupt available
    (void)OS_SetIrqMask(OS_IE_FIFO_RECV);
    (void)OS_ResetRequestIrqMask(0xffffffff /*all request */ );

    //---- save parameter
    *(u32 *)HW_RESET_PARAMETER_BUF = parameter;

    //---- send reset command to ARM7
    OSi_SendToPxi(OS_PXI_COMMAND_RESET);

    asm
    {
        //---- save sp to dtcm
        ldr             r0, =RESET_HW_DTCM_IRQ_STACK_END;
        ldr             r1, =SDK_IRQ_STACKSIZE;
        sub             r0, r0, r1;
        mov             sp, r0;
        
        //---- reset
        bl              OSi_DoResetSystem;
    }
    // don't reach
}

//---------------- ARM7
#else  // ifdef SDK_ARM9
void OS_ResetSystem(void)
{
    //---- stop all dma
    MI_StopDma(0);
    MI_StopDma(1);
    MI_StopDma(2);
    MI_StopDma(3);

    //---- set only pxi interrupt available
    (void)OS_SetIrqMask(OS_IE_FIFO_RECV);
    (void)OS_ResetRequestIrqMask(0xffffffff /*all request */ );

    //---- sound stop
    SND_Shutdown();

    //---- send reset command to ARM9
    OSi_SendToPxi(OS_PXI_COMMAND_RESET);

    //---- reset
    OSi_DoResetSystem();
}
#endif // ifdef SDK_ARM9


/*---------------------------------------------------------------------------*
  Name:         OSi_DoResetSystem

  Description:  Start reset sequence.
                Subroutine of OS_ResetSystem().
                This part is put in itcm/wram because rom data will be
                loaded newly.

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
//---------------- ARM9
#ifdef SDK_ARM9
#include    <nitro/itcm_begin.h>
static void OSi_DoResetSystem(void)
{
    //---- wait to receive reset command from ARM7
    while (!(vu16)OSi_IsResetOccurred)
    {
    }

    //---- set IME = 0
    reg_OS_IME = 0;

    //---- reload ROM data
    OSi_ReloadRomData();

    //---- do boot
    OSi_DoBoot();
}

#include    <nitro/itcm_end.h>

//---------------- ARM7
#else  // ifdef SDK_ARM9
#include    <nitro/wram_begin.h>
static void OSi_DoResetSystem(void)
{
    //---- set IME = 0
    reg_OS_IME = 0;

    //---- do boot
    OSi_DoBoot();
}

#include    <nitro/wram_end.h>
#endif // ifdef SDK_ARM9

/*---------------------------------------------------------------------------*
  Name:         OSi_DoBoot

  Description:  do boot

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
//---------------- ARM9
#ifdef SDK_ARM9
#include    <nitro/itcm_begin.h>
asm void OSi_DoBoot( void )
{
    //---- set IME = 0
    mov         r12, #HW_REG_BASE
    str         r12, [r12, #REG_IME_OFFSET]

    //---- clear interrupt vector just to make sure
    ldr         r1, =OSi_HW_DTCM
    add         r1, r1, #0x3fc0
    add         r1, r1, #HW_DTCM_SYSRV_OFS_INTR_VECTOR
    mov         r0, #0
    str         r0, [r1, #0]
    
    //---- synchronousness with ARM7
    ldr         r1, =REG_SUBPINTF_ADDR
@1:
    ldrh        r0, [r1]
    and         r0, r0, #0x000f
    cmp         r0, #0x0001
    bne         @1                              // wait till subp status == 1
    mov         r0, #0x0100
    strh        r0, [r1]                        // mainp status == 1

    //---- clear some memory
    mov         r0, #0
    ldr         r3, =HW_EXCP_VECTOR_MAIN        // save exception vector
    ldr         r4, [r3]
    
    ldr         r1, =HW_BIOS_EXCP_STACK_MAIN    // clear system area. (exception, arena, DMA clear buf...)
    mov         r2, #0x80
    bl          OSi_CpuClear32
    
    str         r4, [r3]                        // restore exception vector
    
    ldr         r1, =HW_PXI_SIGNAL_PARAM_ARM9   // clear system area. (wm, thread, lock, intr...)
    mov         r2, #0x18
    bl          OSi_CpuClear32
    
    ldr         r1, =HW_WM_RSSI_POOL
    strh        r0, [r1]
    
    ldr         r1, =HW_COMPONENT_PARAM         // clear system area. (wm, thread, lock, intr...)
    mov         r2, #0x64
    bl          OSi_CpuClear32
    
    //---- syncronousness with ARM7
    ldr         r1, =REG_SUBPINTF_ADDR
@2:
    ldrh        r0, [r1]
    and         r0, r0, #0x000f
    cmp         r0, #0x0001                     // wait till subp status == 0
    beq         @2
    mov         r0, #0
    strh        r0, [r1]                        // mainp status == 0
    
    //---- get boot address
    ldr         r3, =HW_ROM_HEADER_BUF          // r3 = ROM header
    ldr         r12, [r3, #0x24]                // lr = r12 = ARM9 entry address
    mov         lr, r12
    
    //---- clear registers r0-r11
    ldr         r11, =HW_PXI_SIGNAL_PARAM_ARM9  // clear r0-r11
    ldmia       r11, {r0-r10}
    mov         r11, #0

    //---- boot game
    bx          r12                             // jump to r12
}
#include <nitro/itcm_end.h>

//---------------- ARM7
#else  //ifdef SDK_ARM9
#include <nitro/wram_begin.h>
asm void OSi_DoBoot( void )
{
    //---- set IME = 0
    mov         r12, #HW_REG_BASE
    str         r12, [r12, #REG_IME_OFFSET]

    //---- clear interrupt vector just to make sure
    ldr         r1, =HW_INTR_VECTOR_BUF
    mov         r0, #0
    str         r0, [r1, #0]
    
    //---- syncronousness with ARM9
    ldr         r1, =REG_MAINPINTF_ADDR
    mov         r0, #0x0100
    strh        r0, [r1]                        // subp status = 1
@1:
    ldrh        r0, [r1]
    and         r0, r0, #0x000f
    cmp         r0, #0x0001                     // wait till mainp status == 1
    bne         @1

    //---- syncronousness with ARM9
    ldr         r1, =REG_MAINPINTF_ADDR
    mov         r0, #0                          // subp status = 0
    strh        r0, [r1]
@2:
    ldrh        r0, [r1]
    cmp         r0, #0x0001                     // wait till mainp status == 0
    beq         @2

    //---- get boot address
    ldr         r3, =HW_ROM_HEADER_BUF          // r3 = ROM header
    ldr         r12, [r3, #0x34]                // lr = r12 = ARM7 entry address
    mov         lr, r12
    
    //---- clear registers r0-r3
    mov         r0, #0
    mov         r1, #0
    mov         r2, #0
    mov         r3, #0
    
    //---- boot game
    bx          r12
}
#include <nitro/wram_end.h>
#endif //ifdef SDK_ARM9


#ifdef SDK_ARM9
#include    <nitro/itcm_begin.h>
/*---------------------------------------------------------------------------*
  Name:         OSi_CpuClear32

  Description:  fill memory with specified data.
                32bit version

  Arguments:    data  : fill data
                destp : destination address
                size  : size (byte)

  Returns:      None
 *---------------------------------------------------------------------------*/
static asm void  OSi_CpuClear32( register u32 data, register void *destp, register u32 size )
{
    add     r12, r1, r2             // r12: destEndp = destp + size
@1:
    cmp     r1, r12                 // while (destp < destEndp)
    blt @stmltia1
    b @stmltia2
@stmltia1:
    stmia r1!, {r0}               // *((vu32 *)(destp++)) = data
@stmltia2:
    blt     @1
    bx      lr
}

/*---------------------------------------------------------------------------*
  Name:         OSi_ReloadRomData

  Description:  reload ROM data from card

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void OSi_ReloadRomData(void)
{
    u32     p = (u32)HW_ROM_HEADER_BUF;
    const u32 rom_base = *(u32 *)HW_ROM_BASE_OFFSET_BUF;

    /* if necessary, reload ROM header. */
    if (rom_base >= 0x8000)
        OSi_ReadCardRom32(rom_base, (void *)p, 0x160);

    {
        u32     src_arm9 = *(u32 *)(p + 0x20);
        u32     dst_arm9 = *(u32 *)(p + 0x28);
        u32     len_arm9 = *(u32 *)(p + 0x2c);
        u32     src_arm7 = *(u32 *)(p + 0x30);
        u32     dst_arm7 = *(u32 *)(p + 0x38);
        u32     len_arm7 = *(u32 *)(p + 0x3c);

        {
            OSIntrMode bak_cpsr = OS_DisableInterrupts();
            DC_StoreAll();
            DC_InvalidateAll();
            (void)OS_RestoreInterrupts(bak_cpsr);
        }
        IC_InvalidateAll();
        DC_WaitWriteBufferEmpty();

        //OS_Printf("A9 %x %x %x %x\n", *(u32*)(p+0x20), *(u32*)(p+0x24), *(u32*)(p+0x28), *(u32*)(p+0x2c) );
        //OS_Printf("A7 %x %x %x %x\n", *(u32*)(p+0x30), *(u32*)(p+0x34), *(u32*)(p+0x38), *(u32*)(p+0x3c) );

        /* add base-offset. */
        src_arm9 += rom_base;
        src_arm7 += rom_base;

        //---- ARM9 code
        if (src_arm9 < 0x8000)         // top 32KByte of the rom cannot be read.
        {
            u32     diff = 0x8000 - src_arm9;
            src_arm9 = 0x8000;
            dst_arm9 += diff;
            len_arm9 -= diff;
        }
        OSi_ReadCardRom32(src_arm9, (void *)dst_arm9, (int)len_arm9);

        //---- ARM7 code
        OSi_ReadCardRom32(src_arm7, (void *)dst_arm7, (int)len_arm7);
    }
}

/*---------------------------------------------------------------------------*
  Name:         OSi_ReadCardRom32

  Description:  read card data by CPU
                (subroutine of OSi_ReloadRomData)

  Arguments:    src  : source address, need to be 4 byte aligned
                dest : destination address, need to be 4 byte aligned
                len  : size of forwarding

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void OSi_ReadCardRom32(u32 src, void *dst, int len)
{
    //---- I/O registers for CARD
    vu8    *const reg_CARD_MASTERCNT = (vu8 *)(HW_REG_BASE + 0x1a1);
    vu8    *const reg_CARD_CMD = (vu8 *)(HW_REG_BASE + 0x1a8);
    vu32   *const reg_CARD_CNT = (vu32 *)(HW_REG_BASE + 0x1a4);
    vu32   *const reg_CARD_DATA = (vu32 *)(HW_REG_BASE + 0x100010);
    vu32   *const hdr_GAME_BUF = (vu32 *)(HW_ROM_HEADER_BUF + 0x60);


    enum
    {
        CARD_MASTER_SELECT_ROM = 0x00,
        CARD_MASTER_ENABLE = 0x80,
        CARD_CMD_READ_PAGE = 0xb7,
        CARD_CTRL_CMD_MASK = 0x07000000,
        CARD_CTRL_CMD_PAGE = 0x01000000,
        CARD_CTRL_READ = 0x00000000,
        CARD_CTRL_RESET_HI = 0x20000000,
        CARD_CTRL_START = 0x80000000,
        CARD_CTRL_READY = 0x00800000,
        CARD_ENUM_END
    };

    const u32 ctrl_start = (u32)((*hdr_GAME_BUF & ~CARD_CTRL_CMD_MASK)
                                 | (CARD_CTRL_CMD_PAGE | CARD_CTRL_READ
                                    | CARD_CTRL_START | CARD_CTRL_RESET_HI));

    //---- position of page. minus value mean before range.
    int     pos = -(int)(src & (512 - 1));

    //---- wait finishing just to make sure, and turn master enable on
    while ((*reg_CARD_CNT & CARD_CTRL_START) != 0)
    {
    }
    *reg_CARD_MASTERCNT = (u32)(CARD_MASTER_SELECT_ROM | CARD_MASTER_ENABLE);

    for (src = (u32)(src + pos); pos < len; src += 512)
    {
        //---- setting for command of one page reading
        reg_CARD_CMD[0] = (u8)(CARD_CMD_READ_PAGE);
        reg_CARD_CMD[1] = (u8)(src >> 24);
        reg_CARD_CMD[2] = (u8)(src >> 16);
        reg_CARD_CMD[3] = (u8)(src >> 8);
        reg_CARD_CMD[4] = (u8)(src >> 0);
        reg_CARD_CMD[5] = (u8)(0);
        reg_CARD_CMD[6] = (u8)(0);
        reg_CARD_CMD[7] = (u8)(0);

        //---- start forwarding by CPU
        *reg_CARD_CNT = ctrl_start;
        for (;;)
        {
            u32     ctrl = *reg_CARD_CNT;

            //---- check if ok to read word data
            if ((ctrl & CARD_CTRL_READY) != 0)
            {
                //---- read data
                const u32 data = *reg_CARD_DATA;

                //---- store data
                if ((pos >= 0) && (pos < len))
                {
                    *(u32 *)((u32)dst + pos) = data;
                }

                pos += sizeof(u32);
            }

            //---- notice: need to read whole one page data
            if (!(ctrl & CARD_CTRL_START))
            {
                break;
            }
        }
    }
}

#include <nitro/itcm_end.h>
#endif //ifdef SDK_ARM9
