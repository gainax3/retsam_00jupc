/*---------------------------------------------------------------------------*
  Project:  NitroSDK - screenshot test - EXT
  File:     ext_scrntest.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: ext_scrntest.c,v $
  Revision 1.11  2006/01/18 02:11:20  kitase_hirotake
  do-indent

  Revision 1.10  2005/02/28 05:26:07  yosizaki
  do-indent.

  Revision 1.9  2004/11/04 02:22:51  takano_makoto
  fix typo.

  Revision 1.8  2004/05/27 09:15:16  takano_makoto
  Add EXT_Printf(), EXT_CompPrint()

  Revision 1.7  2004/03/17 07:21:01  yasu
  do sdk_indent

  Revision 1.6  2004/02/25 06:02:57  kitani_toshikazu
  add comments.

  Revision 1.5  2004/02/06 12:40:58  yasu
  delete SDK_AUTOTEST off

  Revision 1.4  2004/02/06 09:15:21  kitani_toshikazu
  Change check sum calc function.

  Revision 1.3  2004/02/05 10:55:38  kitani_toshikazu
  Added EXT_TestSetVRAMForScreenShot().

  Revision 1.2  2004/02/05 07:09:02  yasu
  change SDK prefix iris -> nitro

  Revision 1.1  2004/02/04 23:40:53  kitani_toshikazu
  Initial check in.

  Revision 1.1  2004/02/04 12:37:55  kitani_toshikazu
  Initial check in.

  
  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>

//------------------------------------------------------------------------------
// These constants will be replaeced, when regular accessor functions are released.
typedef u16 CapturedPixel;

#define MASK_CAPTURED_PIX_R         0x001f      //  0000 0000 0001 1111
#define SHIFT_CAPTURED_PIX_R        0

#define MASK_CAPTURED_PIX_G         0x03e0      //  0000 0011 1110 0000
#define SHIFT_CAPTURED_PIX_G        5

#define MASK_CAPTURED_PIX_B         0x7C00      //  0111 1100 0000 0000
#define SHIFT_CAPTURED_PIX_B        10

#define MASK_CAPTURED_PIX_ALPHA     0x8000      //  1000 0000 0000 0000
#define SHIFT_CAPTURED_PIX_ALPHA    15





const static u16 EXT_SCRN_W_ = 256;
const static u16 EXT_SCRN_H_ = 192;
const static u32 COUNTER_MAX_ = 0xFFFFFFFE;     // = 0xFFFFFFFF - 0x1

//------------------------------------------------------------------------------
// private func.
static u32 calcScreenShotCheckSum_();

//------------------------------------------------------------------------------
static void startCapture_();
static CapturedPixel *getCapturedPixel_(u16 x, u16 y);
static CapturedPixel *getCapturedBufferBase_();
static u8 getCapturedPixR_(const CapturedPixel * pPix);
static u8 getCapturedPixG_(const CapturedPixel * pPix);
static u8 getCapturedPixB_(const CapturedPixel * pPix);
static BOOL getCapturedPixAlpha_(const CapturedPixel * pPix);

static GXDispMode getDispMode_();
static GXCaptureMode getCaptureMode_(GXDispMode mode);
static GXCaptureSrcB getCaptureSrcB_(GXDispMode mode);
static GXCaptureDest getCaptureDest_();




//------------------------------------------------------------------------------
// private variable
static u32 frameCounter = 0x0;
static GXVRamLCDC vramForCapture_ = GX_VRAM_LCDC_C;     // default VRAM for cap. is VRAM_C

//------------------------------------------------------------------------------

/*---------------------------------------------------------------------------*
  Name:         EXT_Printf

  Description:  print formatted strings for auto test.

  Arguments:    fmt : format string
                ...   and parameters

  Returns:      None.
 *---------------------------------------------------------------------------*/
void EXT_Printf(const char *fmt, ...)
{
    va_list vlist;

    OS_Printf("<AUTO-TEST> ");
    va_start(vlist, fmt);
    OS_VPrintf(fmt, vlist);
    va_end(vlist);
}


/*---------------------------------------------------------------------------*
  Name:         EXT_CompPrint

  Description:  compare string to print formatted strings for auto test.

  Arguments:    src1 : compared src1
                fmt  : format string of compared src2
                ...    and parameters

  Returns:      None.
 *---------------------------------------------------------------------------*/
void EXT_CompPrint(const char *src1, const char *src2_fmt, ...)
{
    va_list vlist;
    char    src2[256];
    s32     i;

    va_start(vlist, src2_fmt);
    (void)OS_VSPrintf(src2, src2_fmt, vlist);
    va_end(vlist);

    for (i = 0;; i++)
    {
        if (src1[i] != src2[i])
        {
            EXT_Printf("PrintString = \"%s\" : \"%s\"\n", src1, src2);
            EXT_Printf("PrintCompare Test [Fail]\n");
            return;
        }

        if (src1[i] == '\0' && src2[i] == '\0')
        {
            break;
        }
    }

    EXT_Printf("PrintCompare Test [Success]\n");
}


/*---------------------------------------------------------------------------*
  Name:         EXT_TestScreenShot

  Description:  capturing the screen shot and calculate the check sum of it 
                at specified frame count.
                
                If the result is different from that you expected, 
                out error message to debug console.

  Arguments:    testFrame    frame count when the screen shot will be captured.
                checkSum     expected check sum value of captured screen shot.
  
  Returns:      none
 *---------------------------------------------------------------------------*/
void EXT_TestScreenShot(u32 testFrame, u32 checkSum)
{
    const u32 prevTestFrame = testFrame - 1;
    const u32 nextTestFrame = testFrame + 1;

    SDK_ASSERTMSG((testFrame > 0)
                  && (testFrame < (u32)COUNTER_MAX_),
                  "illegal input value for numFrames in EXT_TestScreenShot");



    // Request for starting capture on the previous frame of checking.
    if (frameCounter == prevTestFrame)
    {
        startCapture_();
    }

    // Calculate the checkSum of the frame on the next frame of capturing.
    // We have to wait one frame for the guarantee of successful capturing.
    {
        if (frameCounter == nextTestFrame)
        {
            u32     currentSum = 0x0;

            EXT_Printf("ScreenShot Test at frameCounter = %d\n", testFrame);

            currentSum = calcScreenShotCheckSum_();

            EXT_Printf("CheckSum = %X\n", currentSum);
            if (checkSum == currentSum)
            {
                EXT_Printf("ScreenShot Test [Success]\n");
            }
            else
            {
                EXT_Printf("ScreenShot Test [Fail]\n");
            }

            //
            // Currently, I don't terminate the app here.
            //
        }
    }

}

/*---------------------------------------------------------------------------*
  Name:         EXT_TestTickCounter

  Description:  advance the local frame counter for this library.

  Arguments:    none
  
  Returns:      none
 *---------------------------------------------------------------------------*/
void EXT_TestTickCounter()
{
    frameCounter++;
}

/*---------------------------------------------------------------------------*
  Name:         EXT_TestResetCounter

  Description:  reset the local frame counter for this library.

  Arguments:    none
  
  Returns:      none
 *---------------------------------------------------------------------------*/
void EXT_TestResetCounter()
{
    frameCounter = 0;
}

/*---------------------------------------------------------------------------*
  Name:         EXT_TestSetVRAMForScreenShot

  Description:  Set the VRAM for screen shot capturing.

  Arguments:    vram    a VRAM used for screen shot capturing.
  
  Returns:      none
 *---------------------------------------------------------------------------*/
void EXT_TestSetVRAMForScreenShot(GXVRamLCDC vram)
{
    // TODO: If I could know more about vram usage, I could check the param more strictly.
    SDK_ASSERTMSG(vram == GX_VRAM_LCDC_A || vram == GX_VRAM_LCDC_B || vram == GX_VRAM_LCDC_C
                  || vram == GX_VRAM_LCDC_D, "Currentry VRAM_A B C D are supported for capturing.");

    vramForCapture_ = vram;
}

/*---------------------------------------------------------------------------*
  Name:         startCapture_

  Description:  start screen capturing( and out messages to debug console).
                

  Arguments:    none
  
  Returns:      none
 *---------------------------------------------------------------------------*/
static void startCapture_()
{
    const GXDispMode dispMode = getDispMode_();

    GX_SetCapture(GX_CAPTURE_SIZE_256x192,
                  getCaptureMode_(dispMode), GX_CAPTURE_SRCA_2D3D, getCaptureSrcB_(dispMode),
                  getCaptureDest_(), 16, 0);
    // Out Msg
    {
        char    vram = '*';
        switch (vramForCapture_)
        {
        case GX_VRAM_LCDC_A:
            vram = 'A';
            break;
        case GX_VRAM_LCDC_B:
            vram = 'B';
            break;
        case GX_VRAM_LCDC_C:
            vram = 'C';
            break;
        case GX_VRAM_LCDC_D:
            vram = 'D';
            break;
        default:
            SDK_INTERNAL_ERROR("UnExpected VRAM type in startCapture_()");
            break;
        }
        EXT_Printf("Capture to VRAM %c for ScreenShot Test \n", vram);
    }
}

/*---------------------------------------------------------------------------*
  Name:         getDispMode_

  Description:  get the current display mode from the DISPCNT register.
                

  Arguments:    none
  
  Returns:      current display mode
                ( see GXDispMode for more detail )
 *---------------------------------------------------------------------------*/
static GXDispMode getDispMode_()
{
    const GXDispMode ret =
        (GXDispMode)((reg_GX_DISPCNT & REG_GX_DISPCNT_MODE_MASK) >> REG_GX_DISPCNT_MODE_SHIFT);
    GX_DISPMODE_ASSERT(ret);

    return ret;
}


/*---------------------------------------------------------------------------*
  Name:         getCaptureMode_

  Description:  get the type of current capture mode.

  Arguments:    mode        current display mode
  
  Returns:      type of current capture mode
                ( see GXCaptureMode for more detail )
 *---------------------------------------------------------------------------*/
static GXCaptureMode getCaptureMode_(GXDispMode mode)
{
    if (mode == GX_DISPMODE_GRAPHICS)
    {
        return GX_CAPTURE_MODE_A;
    }
    else
    {
        return GX_CAPTURE_MODE_B;
    }
}

/*---------------------------------------------------------------------------*
  Name:         getCaptureSrcB_

  Description:  get the type of current capture mode.
                

  Arguments:    mode        current display mode
  
  Returns:      type of current capture mode.
                
 *---------------------------------------------------------------------------*/
static GXCaptureSrcB getCaptureSrcB_(GXDispMode mode)
{
    if (mode == GX_DISPMODE_GRAPHICS)
    {
        // return meanless value, because srcB isn't used.
        return (GXCaptureSrcB)0;
    }
    else
    {
        if (mode == GX_DISPMODE_MMEM)
        {
            // capture from main mem
            return GX_CAPTURE_SRCB_MRAM;
        }
        else
        {
            // capture from VRAM
            return GX_CAPTURE_SRCB_VRAM_0x00000;
        }
    }
}


/*---------------------------------------------------------------------------*
  Name:         getCaptureDest_

  Description:  get the captured data's destination.

  Arguments:    none
  
  Returns:      type of destination.
                
 *---------------------------------------------------------------------------*/
static GXCaptureDest getCaptureDest_()
{
    switch (vramForCapture_)
    {
    case GX_VRAM_LCDC_A:
        return GX_CAPTURE_DEST_VRAM_A_0x00000;
    case GX_VRAM_LCDC_B:
        return GX_CAPTURE_DEST_VRAM_B_0x00000;
    case GX_VRAM_LCDC_C:
        return GX_CAPTURE_DEST_VRAM_C_0x00000;
    case GX_VRAM_LCDC_D:
        return GX_CAPTURE_DEST_VRAM_D_0x00000;

    default:
        SDK_INTERNAL_ERROR("UnExpected VRAM type in getCaptureDest_()");
        return GX_CAPTURE_DEST_VRAM_C_0x00000;  // Dummy
    }
}




/*---------------------------------------------------------------------------*
  Name:         calcScreenShotCheckSum_

  Description:  caluclate the check sum value of captured screen shot.

  Arguments:    none
  
  Returns:      check sum value of captured screen shot.
                
 *---------------------------------------------------------------------------*/
// 
// NOTICE ME! Screen size is hardcoded. (These should be removed...)
//
static u32 calcScreenShotCheckSum_()
{
    u16     i, j;
    u32     sum = 0x0;
    const CapturedPixel *pPx = NULL;
    for (j = 0; j < EXT_SCRN_H_; j++)
    {
        for (i = 0; i < EXT_SCRN_W_; i++)
        {
            pPx = getCapturedPixel_(i, j);
            SDK_NULL_ASSERT(pPx);
            sum += (u32)(*pPx) * (i + j);
        }
    }
    return sum;
}



/*---------------------------------------------------------------------------*
  Name:         getCapturedPixel_

  Description:  accessor for Captured scrrenshot on VRAM.

  Arguments:    none
  
  Returns:      Captured pixel data on VRAM.
                
 *---------------------------------------------------------------------------*/
static CapturedPixel *getCapturedPixel_(u16 x, u16 y)
{
    SDK_MINMAX_ASSERT(x, 0, EXT_SCRN_W_);
    SDK_MINMAX_ASSERT(y, 0, EXT_SCRN_H_);

    return (CapturedPixel *) (getCapturedBufferBase_()) + ((EXT_SCRN_W_ * y) + x);
}

/*---------------------------------------------------------------------------*
  Name:         getCapturedBufferBase_

  Description:  Get the captured data's base adderess.

  Arguments:    none
  
  Returns:      captured data's base adderess.
                
 *---------------------------------------------------------------------------*/
static CapturedPixel *getCapturedBufferBase_()
{
    switch (vramForCapture_)
    {
    case GX_VRAM_LCDC_A:
        return (CapturedPixel *) HW_LCDC_VRAM_A;
    case GX_VRAM_LCDC_B:
        return (CapturedPixel *) HW_LCDC_VRAM_B;
    case GX_VRAM_LCDC_C:
        return (CapturedPixel *) HW_LCDC_VRAM_C;
    case GX_VRAM_LCDC_D:
        return (CapturedPixel *) HW_LCDC_VRAM_D;

    default:
        SDK_INTERNAL_ERROR("UnExpected VRAM type in getCapturedBufferBase_()");
        return (CapturedPixel *) NULL; // Dummy
    }
}

//------------------------------------------------------------------------------
// accessors
//
static u8 getCapturedPixR_(const CapturedPixel * pPix)
{
    SDK_NULL_ASSERT(pPix);
    return (u8)((MASK_CAPTURED_PIX_R & *pPix) >> SHIFT_CAPTURED_PIX_R);
}

static u8 getCapturedPixG_(const CapturedPixel * pPix)
{
    SDK_NULL_ASSERT(pPix);
    return (u8)((MASK_CAPTURED_PIX_G & *pPix) >> SHIFT_CAPTURED_PIX_G);
}

static u8 getCapturedPixB_(const CapturedPixel * pPix)
{
    SDK_NULL_ASSERT(pPix);
    return (u8)((MASK_CAPTURED_PIX_B & *pPix) >> SHIFT_CAPTURED_PIX_B);
}

static BOOL getCapturedPixAlpha_(const CapturedPixel * pPix)
{
    SDK_NULL_ASSERT(pPix);
    return (BOOL)((MASK_CAPTURED_PIX_R & *pPix) >> SHIFT_CAPTURED_PIX_ALPHA);
}
