/*---------------------------------------------------------------------------*
  Project:  NitroSDK - library - init
  File:     crt0.c

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: crt0.c,v $
  Revision 1.52  2007/10/03 01:05:49  okubata_ryoma
  Copyright fix

  Revision 1.51  2007/08/22 05:23:11  yosizaki
  fix about code32.h

  Revision 1.50  2006/08/09 06:11:26  yosizaki
  change to wait vcount.

  Revision 1.49  2006/04/10 05:46:48  yada
  align the irq mode stack for 8byte-alignment

  Revision 1.48  2006/04/04 07:41:56  yada
  stack address in entering into NitroMain() is set to 8byte-alignment

  Revision 1.47  2006/01/18 02:11:20  kitase_hirotake
  do-indent

  Revision 1.46  2005/07/19 06:08:16  yada
  add irq stack check. 4byte is taken for stack check from irq stack

  Revision 1.45  2005/07/04 02:02:46  yosizaki
  add flush operation after bss initialization.

  Revision 1.44  2005/06/14 08:24:11  yosizaki
  fix do_autoload. (insert wait-flush)

  Revision 1.43  2005/06/07 11:04:58  yasu
  Autoload 時のキャッシュフラッシュ追加

  Revision 1.42  2005/04/08 09:29:54  yosizaki
  add OSi_ReferSymbol().

  Revision 1.41  2005/03/29 00:59:28  yasu
  VERSION_PATCH フィールドの廃止

  Revision 1.40  2005/03/24 00:55:15  yasu
  SDK_NOCOMPRESS サポートの中止

  Revision 1.39  2005/03/04 11:09:27  yasu
  __declspec(weak) を SDK_WEAK_SYMBOL に置き換え

  Revision 1.38  2005/02/28 05:26:03  yosizaki
  do-indent.

  Revision 1.37  2004/11/18 01:55:56  yada
  Rename MIi_CpuClear32 to INITi_CpuClear32, and let it static function.

  Revision 1.36  2004/11/05 06:15:17  yasu
  do clearing bss after calling autoload callback

  Revision 1.35  2004/10/29 08:05:38  yasu
  MIi_CpuClear32 moves into crt0.c
  Change position of DTCM clearing

  Revision 1.34  2004/10/12 08:01:47  yada
  clear pointer of buffer of OS_InitPrintServer()

  Revision 1.33  2004/10/04 06:07:18  yada
  protection region for MainRAM  is set to 8MB in default

  Revision 1.32  2004/09/29 05:41:49  yasu
  fix comments

  Revision 1.31  2004/09/24 13:59:28  yasu
  Set checker to find VERSION_ID

  Revision 1.30  2004/09/17 11:25:34  yasu
  hold version informatin

  Revision 1.29  2004/09/09 12:47:00  yasu
  Archive footer size 12 -> 8bytes

  Revision 1.28  2004/09/09 11:45:22  yasu
  Support compstatic in default

  Revision 1.27  2004/09/06 10:44:12  yasu
  Check NULL in MIi_UncompressBackward

  Revision 1.26  2004/09/04 12:39:26  yasu
  Change interface between compstatic tool

  Revision 1.25  2004/09/02 09:48:10  yasu
  rename do_compress -> MIi_UncompressModule

  Revision 1.24  2004/09/01 07:28:47  yasu
  Support NITRO_COMPRESS

  Revision 1.23  2004/08/02 12:24:24  yasu
  Support autoload table for paramters at _start_AutoloadDoneCallback

  Revision 1.22  2004/08/02 10:40:41  yasu
  Add autoload-done callback.

  Revision 1.21  2004/07/23 12:10:50  yada
  for changing HW_DTCM policy

  Revision 1.20  2004/07/08 10:51:32  yada
  only arrange comment

  Revision 1.19  2004/07/01 09:42:36  yasu
  support autoload

  Revision 1.1  2004/06/30 06:55:51  yada
  initial release

  Revision 1.18  2004/04/29 09:30:29  yada
  change initial sp in 4byte for stack check

  Revision 1.17  2004/04/19 10:52:09  takano_makoto
  add check to move DTCM address.

  Revision 1.16  2004/03/30 06:09:29  yada
  ldconst や lda を ldr = を使用する記述にした。

  Revision 1.15  2004/03/24 00:36:36  yada
  SDK_BB 廃止につき修正

  Revision 1.14  2004/03/23 08:40:25  yasu
  _start -> weak symbol

  Revision 1.13  2004/03/22 02:02:05  takano_makoto
  Make HW_MAIN_MEM_SUB_SIZE to variable.

  Revision 1.12  2004/02/25 09:16:57  yasu
  fix comments

  Revision 1.11  2004/02/18 02:37:37  yada
  C++用設定追加

  Revision 1.10  2004/02/16 09:37:27  yada
  スタックサイズをLCFから決定できるようにした。
  DTCM、パレット、OAMをクリアするようにした。

  Revision 1.9  2004/02/12 11:44:10  yada
  ARM9/ARM7 振り分け作業

  Revision 1.8  2004/02/09 11:17:05  yasu
  include code32.h

  Revision 1.7  2004/02/05 07:09:02  yasu
  change SDK prefix iris -> nitro

  Revision 1.6  2004/01/15 08:25:23  yada
  プロテクションユニット５の設定(ITCM)を、0からではなく
  HW_ITCM_IMAGE(0x1000000) からにした。
  NULL Pointer を例外で拾うため、0は外したかった。

  Revision 1.5  2004/01/13 04:15:56  yada
  命令キャッシュ、データキャッシュ無効化部分修正

  Revision 1.4  2003/12/25 07:31:43  yada
  型ルール統一による変更

  Revision 1.3  2003/12/12 04:40:19  yada
  プロセッサモードの定義変更に伴う修正

  Revision 1.2  2003/12/12 01:27:14  yada
  includeの名称変更(os/system.h と hw/armArch.h) による修正

  Revision 1.1  2003/12/09 08:28:41  yasu
  crt0.s -> crt0.c へ変更 NitroSDK でのコンパイル対応

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include        <nitro.h>
#include        <nitro/code32.h>

extern void NitroMain(void);
extern void OS_IrqHandler(void);
static void do_autoload(void);
static void init_cp15(void);
void    _start(void);
static void INITi_CpuClear32(register u32 data, register void *destp, register u32 size);
extern void *const _start_ModuleParams[];
void    _start_AutoloadDoneCallback(void *argv[]);

extern void __call_static_initializers(void);
extern void _fp_init(void);

// from LCF
extern void SDK_IRQ_STACKSIZE(void);
extern void SDK_AUTOLOAD_START(void);  // autoload data will start from here
extern void SDK_AUTOLOAD_LIST(void);   // start pointer to autoload information
extern void SDK_AUTOLOAD_LIST_END(void);        // end pointer to autoload information
extern void SDK_STATIC_BSS_START(void); // static bss start address
extern void SDK_STATIC_BSS_END(void);  // static bss end address

#include        <nitro/version.h>
#define         SDK_VERSION_ID   ((u32)SDK_VERSION_MAJOR<<24|\
                                  (u32)SDK_VERSION_MINOR<<16|\
                                  (u32)SDK_VERSION_RELSTEP)

#define     SDK_NITROCODE_LE    0x2106c0de
#define     SDK_NITROCODE_BE    0xdec00621


/*---------------------------------------------------------------------------*
  Name:         _start

  Description:  Start up

  Arguments:    None

  Returns:      None.
 *---------------------------------------------------------------------------*/
#define INITi_HW_DTCM   SDK_AUTOLOAD_DTCM_START

SDK_WEAK_SYMBOL asm void _start( void )
{
        //---- set IME = 0
        //     ( use that LSB of HW_REG_BASE equal to 0 )
        mov             r12, #HW_REG_BASE
        str             r12, [r12, #REG_IME_OFFSET]

        //---- adjust VCOUNT.
@wait_vcount_0:
        ldrh            r0, [r12, #REG_VCOUNT_OFFSET]
        cmp             r0, #0
        bne             @wait_vcount_0

        //---- initialize cp15
        bl              init_cp15

        //---- initialize stack pointer
        // SVC mode
        mov             r0, #HW_PSR_SVC_MODE
        msr             cpsr_c, r0
        ldr             r0, =INITi_HW_DTCM
        add             r0, r0, #0x3fc0
        mov             sp, r0

        // IRQ mode
        mov             r0, #HW_PSR_IRQ_MODE
        msr             cpsr_c, r0
        ldr             r0, =INITi_HW_DTCM
        add             r0, r0, #0x3fc0
        sub             r0, r0, #HW_SVC_STACK_SIZE
        sub             sp, r0, #4 // 4byte for stack check code
        tst             sp, #4
        subeq           sp, sp, #4 // for 8byte-alignment

        // System mode
        ldr             r1, =SDK_IRQ_STACKSIZE
        sub             r1, r0, r1
        mov             r0, #HW_PSR_SYS_MODE
        msr             cpsr_csfx, r0
        sub             sp, r1, #4 // 4byte for stack check code

        //---- clear memory
        // DTCM (16KB)
        mov             r0, #0
        ldr             r1, =INITi_HW_DTCM
        mov             r2, #HW_DTCM_SIZE
        bl              INITi_CpuClear32

        // BG/OBJ palette (1KB)
        mov             r0, #0
        ldr             r1, =HW_PLTT
        mov             r2, #HW_PLTT_SIZE
        bl              INITi_CpuClear32

        // OAM (1KB)
        mov             r0, #0x0200
        ldr             r1, =HW_OAM
        mov             r2, #HW_OAM_SIZE
        bl              INITi_CpuClear32

        //---- load autoload block and initialize bss
        ldr             r1, =_start_ModuleParams
        ldr             r0, [r1, #20]   // r0 = bottom of compressed data
        bl              MIi_UncompressBackward
        bl              do_autoload
        
        //---- fill static static bss with 0
        ldr             r0, =_start_ModuleParams
        ldr                r1, [r0, #12]   // BSS segment start
        ldr                r2, [r0, #16]   // BSS segment end
        mov                r3, r1          // for next step(flush bss)  
        mov                r0, #0
@1:     cmp                r1, r2
        strcc              r0, [r1], #4
        bcc                @1

        //---- flush static bss region
        //     (r0 == #0, r3 == _start_ModuleParams::BSS_segment_start)
        bic                r1, r3, #HW_CACHE_LINE_SIZE - 1
@cacheflush:
        mcr                p15, 0, r0, c7, c10, 4         // wait writebuffer empty
        mcr                p15, 0, r1, c7,  c5, 1         // ICache
        mcr                p15, 0, r1, c7, c14, 1         // DCache
        add                r1, r1, #HW_CACHE_LINE_SIZE
        cmp                r1, r2
        blt                @cacheflush

        // print buffer (used for ARM7's printing)
        ldr             r1, =HW_COMPONENT_PARAM
        str             r0, [r1, #0]

        //---- set interrupt vector
        ldr             r1, =INITi_HW_DTCM
        add             r1, r1, #0x3fc0
        add             r1, r1, #HW_DTCM_SYSRV_OFS_INTR_VECTOR
        ldr             r0, =OS_IrqHandler
        str             r0, [r1, #0]

#ifndef SDK_NOINIT
        //---- for C++
        bl             _fp_init
        bl             NitroStartUp
        bl             __call_static_initializers
#endif
        //---- start (to 16bit code)
        ldr             r1, =NitroMain
        ldr             lr, =HW_RESET_VECTOR

        tst             sp, #4
        subne           sp, sp, #4 // for 8byte-alignment
        bx              r1
}


/*---------------------------------------------------------------------------*
  Name:         INITi_CpuClear32

  Description:  fill memory with specified data.
                32bit version

  Arguments:    data  : fill data
                destp : destination address
                size  : size (byte)

  Returns:      None
 *---------------------------------------------------------------------------*/
static asm void  INITi_CpuClear32( register u32 data, register void *destp, register u32 size )
{
        add     r12, r1, r2             // r12: destEndp = destp + size
@20:
        cmp     r1, r12                 // while (destp < destEndp)
        stmltia r1!, {r0}               // *((vu32 *)(destp++)) = data
        blt     @20
        bx      lr
}


/*---------------------------------------------------------------------------*
  Name:         _start_ModuleParams

  Description:  autoload/compress/arguments data block

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void   *const _start_ModuleParams[] = {
    (void *)SDK_AUTOLOAD_LIST,
    (void *)SDK_AUTOLOAD_LIST_END,
    (void *)SDK_AUTOLOAD_START,
    (void *)SDK_STATIC_BSS_START,
    (void *)SDK_STATIC_BSS_END,
    (void *)0,                         // CompressedStaticEnd
    (void *)SDK_VERSION_ID,            // SDK version info
    (void *)SDK_NITROCODE_BE,          // Checker 1
    (void *)SDK_NITROCODE_LE,          // Checker 2
};

/*---------------------------------------------------------------------------*
  Name:         MIi_UncompressBackward

  Description:  Uncompress special archive for module compression

  Arguments:    bottom         = Bottom adrs of packed archive + 1
                bottom[-8..-6] = offset for top    of compressed data
                                 inp_top = bottom - bottom[-8..-6]
                bottom[-5]     = offset for bottom of compressed data
                                 inp     = bottom - bottom[-5]
                bottom[-4..-1] = offset for bottom of original data
                                 outp    = bottom + bottom[-4..-1]
  
                typedef struct
                {
                   u32         bufferTop:24;
                   u32         compressBottom:8;
                   u32         originalBottom;
                }  CompFooter;

  Returns:      None.
 *---------------------------------------------------------------------------*/
asm void  MIi_UncompressBackward( register void* bottom )
{
#define data            r0
#define inp_top         r1
#define outp            r2
#define inp             r3
#define outp_save       r4
#define flag            r5
#define count8          r6
#define index           r7
#define len             r12
                cmp     bottom, #0
                beq     @exit
                stmfd   sp!,    {r4-r7}
                ldmdb   bottom, {r1-r2}
                add     outp,    bottom,  outp
                sub     inp,     bottom,  inp_top, LSR #24
                bic     inp_top, inp_top, #0xff000000
                sub     inp_top, bottom,  inp_top
                mov     outp_save, outp
@loop:
                cmp     inp, inp_top            // exit if inp==inp_top
                ble     @end_loop
                ldrb    flag, [inp, #-1]!       // r4 = compress_flag = *--inp
                mov     count8, #8
@loop8:
                subs    count8, count8, #1
                blt     @loop
                tst     flag, #0x80
                bne     @blockcopy
@bytecopy:
                ldrb    data, [inp, #-1]!
#ifdef  SDK_TEG
                sub     outp, outp, #1
                swpb    data, data, [outp]
#else
                strb    data, [outp, #-1]!      // Copy 1 byte
#endif
                b       @joinhere
@blockcopy:
                ldrb    len,   [inp, #-1]!
                ldrb    index, [inp, #-1]!
                orr     index, index, len, LSL #8
                bic     index, index, #0xf000
                add     index, index, #0x0002
                add     len,   len,   #0x0020
@patterncopy:
                ldrb    data,  [outp, index]
#ifdef  SDK_TEG
                sub     outp, outp, #1
                swpb    data, data, [outp]
#else
                strb    data,  [outp, #-1]!
#endif
                subs    len,   len,   #0x0010
                bge     @patterncopy

@joinhere:
                cmp     inp, inp_top
                mov     flag, flag, LSL #1
                bgt     @loop8
@end_loop:
    
                // DC_FlushRange & IC_InvalidateRange
                mov     r0, #0
                bic     inp,  inp_top, #HW_CACHE_LINE_SIZE - 1
@cacheflush:
                mcr     p15, 0, r0, c7, c10, 4          // wait writebuffer empty
                mcr     p15, 0, inp, c7,  c5, 1         // ICache
                mcr     p15, 0, inp, c7, c14, 1         // DCache
                add     inp, inp, #HW_CACHE_LINE_SIZE
                cmp     inp, outp_save
                blt     @cacheflush
                
                ldmfd   sp!, {r4-r7}
@exit           bx      lr
}


/*---------------------------------------------------------------------------*
  Name:         do_autoload

  Description:  put autoload data block according to autoload information,
                and clear static bss by filling with 0.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static asm void do_autoload( void )
{
#define ptable          r0
#define infop           r1
#define infop_end       r2
#define src             r3
#define dest            r4
#define dest_begin      r5
#define dest_end        r6
#define tmp             r7
    
        ldr     ptable, =_start_ModuleParams
        ldr     infop,      [ptable, #0]   // r1 = start pointer to autoload_info
        ldr     infop_end,  [ptable, #4]   // r2 = end pointer to autoload_info
        ldr     src,        [ptable, #8]   // r3 = autoload block

@2:
        cmp     infop, infop_end                // reach to end?
        beq     @skipout

        ldr     dest_begin,[infop], #4          // dest_begin
        ldr     tmp,       [infop], #4          // size
        add     dest_end, dest_begin, tmp       // dest_end
        mov     dest,     dest_begin            // dest working pointer
@1:
        cmp     dest, dest_end
        ldrmi   tmp, [src],  #4                 // [dest++] <- [src++]
        strmi   tmp, [dest], #4
        bmi     @1

        //---- fill bss with 0
        ldr     tmp, [infop], #4                // size
        add     dest_end, dest, tmp             // bss end
        mov     tmp, #0
@3:
        cmp     dest, dest_end
        strcc   tmp, [dest], #4
        bcc     @3

        //---- cache work (DC_FlushRange & IC_InvalidateRange)
        bic     dest, dest_begin, #HW_CACHE_LINE_SIZE - 1
@cacheflush:
        mcr     p15, 0, tmp, c7, c10, 4          /* wait writebuffer empty */
        mcr     p15, 0, dest, c7,  c5, 1         // ICache
        mcr     p15, 0, dest, c7, c14, 1         // DCache
        add     dest, dest, #HW_CACHE_LINE_SIZE
        cmp     dest, dest_end
        blt     @cacheflush
        
        b       @2

@skipout:
        // r0 = _start_ModuleParams
        b       _start_AutoloadDoneCallback   // Jump into the callback
}

/*---------------------------------------------------------------------------*
  Name:         _start_AutoloadDoneCallback

  Description:  hook for end of autoload (This is dummy target for DEBUGGER)
  
  Arguments:    argv: pointer for autoload parameters
                     argv[0] = SDK_AUTOLOAD_LIST
                     argv[1] = SDK_AUTOLOAD_LIST_END
                     argv[2] = SDK_AUTOLOAD_START
                     argv[3] = SDK_STATIC_BSS_START
                     argv[4] = SDK_STATIC_BSS_END

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_WEAK_SYMBOL asm void _start_AutoloadDoneCallback( void* argv[] )
{
        bx      lr
}

//-----------------------------------------------------------------------
//                   システム制御コプロセッサ 初期化
//-----------------------------------------------------------------------
static asm void init_cp15(void)
{
        // プロテクションユニット/キャッシュ/TCM ディセーブル

        mrc     p15, 0, r0, c1, c0, 0
        ldr     r1, =HW_C1_ICACHE_ENABLE  | HW_C1_DCACHE_ENABLE  \
                   | HW_C1_ITCM_ENABLE    | HW_C1_DTCM_ENABLE    \
                   | HW_C1_ITCM_LOAD_MODE | HW_C1_DTCM_LOAD_MODE \
                   | HW_C1_LD_INTERWORK_DISABLE                  \
                   | HW_C1_PROTECT_UNIT_ENABLE
        bic     r0, r0, r1
        mcr     p15, 0, r0, c1, c0, 0

        // キャッシュ無効化
        mov     r0, #0
        mcr     p15, 0, r0, c7, c5, 0       // 命令キャッシュ
        mcr     p15, 0, r0, c7, c6, 0       // データキャッシュ

        // ライトバッファ エンプティ待ち
        mcr     p15, 0, r0, c7, c10, 4

/*
; Region G:    BACK_GROUND: Base = 0x0,        Size = 4GB,   I:NC NB    / D:NC NB,     I:NA / D:NA
; Region 0:    IO_VRAM:     Base = 0x04000000, Size = 64MB,  I:NC NB    / D:NC NB,     I:RW / D:RW
; Region 1Rel: MAIN_MEM:    Base = 0x02000000, Size = 8MB*,  I:Cach Buf / D:Cach Buf,  I:RW / D:RW
; Region 1Dbg: MAIN_MEM:    Base = 0x02000000, Size = 8MB,   I:Cach Buf / D:Cach Buf,  I:RW / D:RW
;                                              (* Size will be arranged in OS_InitArena(). )
; Region 2Rel: SOUND_DATA:  Base = 0x02380000, Size = 512KB, I:NC NB    / D:NC NB,     I:NA / D:NA
; Region 2D4M: SOUND_DATA:  Base = 0x02300000, Size = 1MB,   I:NC NB    / D:NC NB,     I:NA / D:NA
; Region 2D8M: SOUND_DATA:  Base = 0x02600000, Size = 2MB,   I:NC NB    / D:NC NB,     I:NA / D:NA
; Region 3:    CARTRIDGE:   Base = 0x08000000, Size = 128MB, I:NC NB    / D:NC NB,     I:NA / D:RW
; Region 4:    DTCM:        Base = SOUND_DATA, Size = 16KB,  I:NC NB    / D:NC NB,     I:NA / D:RW
; Region 5:    ITCM:        Base = 0x01000000, Size = 16MB,  I:NC NB    / D:NC NB,     I:RW / D:RW

; Region 6:    BIOS:        Base = 0xffff0000, Size = 32KB,  I:Cach NB  / D:Cach NB,   I:RO / D:RO
; Region 7:    SHARE_WORK:  Base = 0x027ff000, Size = 4KB,   I:NC NB    / D:NC NB,     I:NA / D:RW
;(Region 7:    DBG_RESERVE: Base = 0x02700000, Size = 1MB,   I:NC NB    / D:NC NB,     I:RW / D:RW)
*/
#define SET_PROTECTION_A( id, adr, siz )        ldr r0, =(adr|HW_C6_PR_##siz|HW_C6_PR_ENABLE)
#define SET_PROTECTION_B( id, adr, siz )        mcr     p15, 0, r0, c6, id, 0
#define REGION_BIT(a,b,c,d,e,f,g,h)     (((a)<<0)|((b)<<1)|((c)<<2)|((d)<<3)|((e)<<4)|((f)<<5)|((g)<<6)|((h)<<7))
#define REGION_ACC(a,b,c,d,e,f,g,h)     (((a)<<0)|((b)<<4)|((c)<<8)|((d)<<12)|((e)<<16)|((f)<<20)|((g)<<24)|((h)<<28))
#define NA      0
#define RW      1
#define RO      5


        //
        // メモリリージョン初期化
        //
        //---- I/O レジスタ & VRAM 等
        SET_PROTECTION_A( c0, HW_IOREG, 64MB )
        SET_PROTECTION_B( c0, HW_IOREG, 64MB )

        //---- メインメモリ
        SET_PROTECTION_A( c1, HW_MAIN_MEM_MAIN, 8MB )
        SET_PROTECTION_B( c1, HW_MAIN_MEM_MAIN, 8MB )

        //---- サウンドデータ領域
#if     HW_MAIN_MEM_SUB_SIZE+HW_MAIN_MEM_SHARED_SIZE == 0x1000
        SET_PROTECTION_A( c2, HW_MAIN_MEM_SUB, 4KB )
        SET_PROTECTION_B( c2, HW_MAIN_MEM_SUB, 4KB )
#elif   HW_MAIN_MEM_SUB_SIZE+HW_MAIN_MEM_SHARED_SIZE == 0x2000
        SET_PROTECTION_A( c2, HW_MAIN_MEM_SUB, 8KB )
        SET_PROTECTION_B( c2, HW_MAIN_MEM_SUB, 8KB )
#elif   HW_MAIN_MEM_SUB_SIZE+HW_MAIN_MEM_SHARED_SIZE == 0x4000
        SET_PROTECTION_A( c2, HW_MAIN_MEM_SUB, 16KB )
        SET_PROTECTION_B( c2, HW_MAIN_MEM_SUB, 16KB )
#elif   HW_MAIN_MEM_SUB_SIZE+HW_MAIN_MEM_SHARED_SIZE == 0x8000
        SET_PROTECTION_A( c2, HW_MAIN_MEM_SUB, 32KB )
        SET_PROTECTION_B( c2, HW_MAIN_MEM_SUB, 32KB )
#elif   HW_MAIN_MEM_SUB_SIZE+HW_MAIN_MEM_SHARED_SIZE == 0x10000
        SET_PROTECTION_A( c2, HW_MAIN_MEM_SUB, 64KB )
        SET_PROTECTION_B( c2, HW_MAIN_MEM_SUB, 64KB )
#elif   HW_MAIN_MEM_SUB_SIZE+HW_MAIN_MEM_SHARED_SIZE == 0x20000
        SET_PROTECTION_A( c2, HW_MAIN_MEM_SUB, 128KB )
        SET_PROTECTION_B( c2, HW_MAIN_MEM_SUB, 128KB )
#elif   HW_MAIN_MEM_SUB_SIZE+HW_MAIN_MEM_SHARED_SIZE == 0x40000
        SET_PROTECTION_A( c2, HW_MAIN_MEM_SUB, 256KB )
        SET_PROTECTION_B( c2, HW_MAIN_MEM_SUB, 256KB )
#elif   HW_MAIN_MEM_SUB_SIZE+HW_MAIN_MEM_SHARED_SIZE == 0x80000
        SET_PROTECTION_A( c2, HW_MAIN_MEM_SUB, 512KB )
        SET_PROTECTION_B( c2, HW_MAIN_MEM_SUB, 512KB )
#elif   HW_MAIN_MEM_SUB_SIZE+HW_MAIN_MEM_SHARED_SIZE == 0x100000
        SET_PROTECTION_A( c2, HW_MAIN_MEM_SUB, 1MB )
        SET_PROTECTION_B( c2, HW_MAIN_MEM_SUB, 1MB )
#elif   HW_MAIN_MEM_SUB_SIZE+HW_MAIN_MEM_SHARED_SIZE == 0x200000
        SET_PROTECTION_A( c2, HW_MAIN_MEM_SUB, 2MB )
        SET_PROTECTION_B( c2, HW_MAIN_MEM_SUB, 2MB )
#else
#pragma message(ERROR: Size unmatch HW_MAIN_MEM_SUB_SIZE)
#endif

        //---- カートリッジ又は他の用途
        //      CPU 内部ワーク RAM 等
        SET_PROTECTION_A( c3, HW_CTRDG_ROM, 128MB )
        SET_PROTECTION_B( c3, HW_CTRDG_ROM, 128MB )

        //---- データ TCM
        //      + CPU 内部ワーク RAM の場合あり
//#if   (HW_DTCM & 0x3FFF) != 0
//#pragma message(ERROR: HW_DTCM need to be aligned 16KB!)
//#endif

//        SET_PROTECTION_A( c4, HW_DTCM, 16KB )
        ldr     r0, =SDK_AUTOLOAD_DTCM_START
        orr     r0, r0, #HW_C6_PR_16KB
        orr     r0, r0, #HW_C6_PR_ENABLE
        SET_PROTECTION_B( c4, HW_DTCM, 16KB )

        //---- 命令 TCM
        //      データ TCM より優先が高い、メインメモリ領域までのイメージ
        SET_PROTECTION_A( c5, HW_ITCM_IMAGE, 16MB )
        SET_PROTECTION_B( c5, HW_ITCM_IMAGE, 16MB )

        //---- BIOS
        SET_PROTECTION_A( c6, HW_BIOS, 32KB )
        SET_PROTECTION_B( c6, HW_BIOS, 32KB )

        //---- SHARED  CPU 間通信ワーク領域
        SET_PROTECTION_A( c7, HW_MAIN_MEM_SHARED, 4KB )
        SET_PROTECTION_B( c7, HW_MAIN_MEM_SHARED, 4KB )
            
#if     HW_MAIN_MEM_SHARED_SIZE != 0x1000
#pragma message(ERROR: Size unmatch HW_MAIN_MEM_SHARED_SIZE)
#endif
       
        //
        // 命令ＴＣＭ 設定
        //
        mov     r0, #HW_C9_TCMR_32MB
        mcr     p15, 0, r0, c9, c1, 1

        //
        // データＴＣＭ 設定
        //
        ldr     r0, =INITi_HW_DTCM
        orr     r0, r0, #HW_C9_TCMR_16KB
        mcr     p15, 0, r0, c9, c1, 0

        //
        // 命令キャッシュ イネーブル (リージョン設定)
        //      1: MAIN_MEM
        //      6: BIOS
        //
        mov     r0, #REGION_BIT(0,1,0,0,0,0,1,0)
        mcr     p15, 0, r0, c2, c0, 1

        //
        // データキャッシュ イネーブル (リージョン設定)
        //      1: MAIN_MEM
        //      6: BIOS
        //
        mov     r0, #REGION_BIT(0,1,0,0,0,0,1,0)
        mcr     p15, 0, r0, c2, c0, 0

        //
        // ライトバッファ イネーブル(リージョン設定)
        //      1: MAIN_MEM
        //
        mov     r0, #REGION_BIT(0,1,0,0,0,0,0,0)
        mcr     p15, 0, r0, c3, c0, 0

        //
        // 命令アクセス許可 (リージョン設定)
        //  IO_VRAM       : RW
        //  MAIN_MEM_MAIN : RW
        //  MAIN_MEM_SUB  : NA
        //  CTRDG         : NA
        //  DTCM          : NA
        //  ITCM          : RW
        //  BIOS          : RO
        //  SHARED        : NA
        //
        ldr     r0, =REGION_ACC(RW,RW,NA,NA,NA,RW,RO,NA)
        mcr     p15, 0, r0, c5, c0, 3

        //
        // データアクセス許可（リージョン設定）
        //  IO_VRAM       : RW
        //  MAIN_MEM_MAIN : RW
        //  MAIN_MEM_SUB  : NA
        //  CTRDG         : RW
        //  DTCM          : RW
        //  ITCM          : RW
        //  BIOS          : RO
        //  SHARED        : RW
        //
        ldr     r0, =REGION_ACC(RW,RW,NA,RW,RW,RW,RO,RW)
        mcr     p15, 0, r0, c5, c0, 2 

        //
        // システム制御コプロセッサ マスター設定
        //
        mrc     p15, 0, r0, c1, c0, 0
        ldr     r1,=HW_C1_ICACHE_ENABLE | HW_C1_DCACHE_ENABLE | HW_C1_CACHE_ROUND_ROBIN \
                  | HW_C1_ITCM_ENABLE   | HW_C1_DTCM_ENABLE                             \
                  | HW_C1_SB1_BITSET    | HW_C1_EXCEPT_VEC_UPPER                        \
                  | HW_C1_PROTECT_UNIT_ENABLE
        orr     r0, r0, r1
        mcr     p15, 0, r0, c1, c0, 0

        bx      lr
}


/*---------------------------------------------------------------------------*
  Name:         NitroStartUp

  Description:  hook for user start up

  Arguments:    None

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_WEAK_SYMBOL void NitroStartUp(void)
{
}

/*---------------------------------------------------------------------------*
  Name:         OSi_ReferSymbol

  Description:  used by SDK_REFER_SYMBOL macro to avoid dead-strip.

  Arguments:    symbol            unused

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OSi_ReferSymbol(void *symbol)
{
#pragma unused(symbol)
}
