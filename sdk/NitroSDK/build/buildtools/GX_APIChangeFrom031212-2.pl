#!/usr/bin/perl

use File::Find;

@dirlist = ('.');

sub replace_content {
    my $file = shift @_;
    my $fullpath = $file;
#  $file =~ s/\/(\w*)/$1/;
    $file =~ s/.*\///g;
    
#  $file =~ s/\/([~/]*)/$1/;
  
    open INFILE, "$file" or die "died $file\n";
    $intext = "";
    while(<INFILE>) {
        $intext .= $_;
    }

    close(INFILE);
    $replaced = 0;

    #
    # g2.h
    #

    $replaced |= $intext =~ s/(\W)GX_WND_PLANEMASK(\W)/${1}GXWndPlaneMask${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_BLEND_PLANEMASK(\W)/${1}GXBlendPlaneMask${2}/sg;
    
    #
    # g2oam.h
    #

    $replaced |= $intext =~ s/(\W)GX_OAM_SHAPE(\W)/${1}GXOamShape${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_OAM_MOSAIC(\W)/${1}GXOamMosaic${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_OAM_COLOR(\W)/${1}GXOamColor${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_OAM_EFFECT(\W)/${1}GXOamEffect${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_OAM_MODE(\W)/${1}GXOamMode${2}/sg;
    
    #
    # g3.h and its family
    #

    $replaced |= $intext =~ s/(\W)GX_BEGIN(\W)/${1}GXBegin${2}/sg;
    
    $replaced |= $intext =~ s/(\W)GX_MTXMODE(\W)/${1}GXMtxMode${2}/sg;

    $replaced |= $intext =~ s/(\W)GX_SORTMODE(\W)/${1}GXSortMode${2}/sg;

    $replaced |= $intext =~ s/(\W)GX_BUFFERMODE(\W)/${1}GXBufferMode${2}/sg;

    $replaced |= $intext =~ s/(\W)GX_LIGHTMASK(\W)/${1}GXLightMask${2}/sg;

    $replaced |= $intext =~ s/(\W)GX_POLYGONMODE(\W)/${1}GXPolygonMode${2}/sg;

    $replaced |= $intext =~ s/(\W)GX_CULL(\W)/${1}GXCull${2}/sg;

    $replaced |= $intext =~ s/(\W)GX_POLYGON_ATTR_MISC(\W)/${1}GXPolygonAttrMisc${2}/sg;

    $replaced |= $intext =~ s/(\W)GX_TEXREPEAT(\W)/${1}GXTexRepeat${2}/sg;

    $replaced |= $intext =~ s/(\W)GX_TEXFLIP(\W)/${1}GXTexFlip${2}/sg;

    $replaced |= $intext =~ s/(\W)GX_TEXSIZE_S(\W)/${1}GXTexSizeS${2}/sg;

    $replaced |= $intext =~ s/(\W)GX_TEXSIZE_T(\W)/${1}GXTexSizeT${2}/sg;

    $replaced |= $intext =~ s/(\W)GX_TEXFMT(\W)/${1}GXTexFmt${2}/sg;

    $replaced |= $intext =~ s/(\W)GX_TEXPLTTCOLOR0(\W)/${1}GXTexPlttColor0${2}/sg;

    $replaced |= $intext =~ s/(\W)GX_TEXGEN(\W)/${1}GXTexGen${2}/sg;

    $replaced |= $intext =~ s/(\W)GX_LIGHTID(\W)/${1}GXLightId${2}/sg;

    #
    # g3x.h
    #

    $replaced |= $intext =~ s/(\W)GX_SHADING(\W)/${1}GXShading${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_FOGBLEND(\W)/${1}GXFogBlend${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_FOGSLOPE(\W)/${1}GXFogSlope${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_FIFOINTR_COND(\W)/${1}GXFifoIntrCond${2}/sg;

    #
    # g3xstatus.h
    #
    $replaced |= $intext =~ s/(\W)GX_FIFOSTAT(\W)/${1}GXFifoStat${2}/sg;

    #
    # gx.h
    #
    $replaced |= $intext =~ s/(\W)GX_BGMODE(\W)/${1}GXBGMode${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_BG0_AS(\W)/${1}GXBG0As${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_OBJMAP_CHAR(\W)/${1}GXObjMapChar${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_OBJMAP_BMP(\W)/${1}GXObjMapBmp${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_PLANEMASK(\W)/${1}GXPlaneMask${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_WNDMASK(\W)/${1}GXWndMask${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_DISPMODE(\W)/${1}GXDispMode${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_OBJVRAM_CHAR(\W)/${1}GXObjVRamChar${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_OBJVRAM_BMP(\W)/${1}GXObjVRamBmp${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_BGCHAROFFSET(\W)/${1}GXBGCharOffset${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_BGSCROFFSET(\W)/${1}GXBGScrOffset${2}/sg;
    
    $replaced |= $intext =~ s/(\W)GX_OBJVRAMMODE_CHAR(\W)/${1}GXOBJVRamModeChar${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_OBJVRAMMODE_BMP(\W)/${1}GXOBJVRamModeBmp${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_POWER(\W)/${1}GXPower${2}/sg;

    #
    # gx_bgcnt.h
    #
    $replaced |= $intext =~ s/(\W)GX_BG_SCRSIZE_TEXT(\W)/${1}GXBGScrSizeText${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_BG_SCRSIZE_AFFINE(\W)/${1}GXBGScrSizeAffine${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_BG_SCRSIZE_256x16PLTT(\W)/${1}GXBGScrSize256x16Pltt${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_BG_SCRSIZE_256BMP(\W)/${1}GXBGScrSize256Bmp${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_BG_SCRSIZE_DCBMP(\W)/${1}GXBGScrSizeDcBmp${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_BG_COLORMODE(\W)/${1}GXBGColorMode${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_BG_AREAOVER(\W)/${1}GXBGAreaOver${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_BG_CHARBASE(\W)/${1}GXBGCharBase${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_BG_SCRBASE(\W)/${1}GXBGScrBase${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_BG_BMPSCRBASE(\W)/${1}GXBGBmpScrBase${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_BG_EXTPLTT(\W)/${1}GXBGExtPltt${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_BG_EXTMODE(\W)/${1}GXBGExtMode${2}/sg;
    

    #
    # gx_capture.h
    #
    $replaced |= $intext =~ s/(\W)GX_CAPTURE_DEST(\W)/${1}GXCaptureDest${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_CAPTURE_SIZE(\W)/${1}GXCaptureSize${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_CAPTURE_SRCA(\W)/${1}GXCaptureSrcA${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_CAPTURE_SRCB(\W)/${1}GXCaptureSrcB${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_CAPTURE_MODE(\W)/${1}GXCaptureMode${2}/sg;

    #
    # gx_vramcnt.h
    #
    $replaced |= $intext =~ s/(\W)GX_VRAM_LCDC(\W)/${1}GXVRamLCDC${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_VRAM_BG(\W)/${1}GXVRamBG${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_VRAM_OBJ(\W)/${1}GXVRamOBJ${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_VRAM_ARM7(\W)/${1}GXVRamARM7${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_VRAM_TEX(\W)/${1}GXVRamTex${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_VRAM_CLRIMG(\W)/${1}GXVRamClearImage${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_VRAM_CLRIMG_NONE(\W)/${1}GX_VRAM_CLEARIMAGE_NONE${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_VRAM_CLRIMG_256_AB(\W)/${1}GX_VRAM_CLEARIMAGE_256_AB${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_VRAM_CLRIMG_256_CD(\W)/${1}GX_VRAM_CLEARIMAGE_256_CD${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_VRAM_TEXPLTT(\W)/${1}GXVRamTexPltt${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_VRAM_BGEXTPLTT(\W)/${1}GXVRamBGExtPltt${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_VRAM_OBJEXTPLTT(\W)/${1}GXVRamOBJExtPltt${2}/sg;

    ################
    # functions
    ################

    #
    # g3util.h
    #
    $replaced |= $intext =~ s/(\W)G3Util_Frustum(\W)/${1}G3_Frustum${2}/sg;
    $replaced |= $intext =~ s/(\W)G3Util_Perspective(\W)/${1}G3_Perspective${2}/sg;
    $replaced |= $intext =~ s/(\W)G3Util_Ortho(\W)/${1}G3_Ortho${2}/sg;
    $replaced |= $intext =~ s/(\W)G3Util_LookAt(\W)/${1}G3_LookAt${2}/sg;
    $replaced |= $intext =~ s/(\W)G3Util_RotX(\W)/${1}G3_RotX${2}/sg;
    $replaced |= $intext =~ s/(\W)G3Util_RotY(\W)/${1}G3_RotY${2}/sg;
    $replaced |= $intext =~ s/(\W)G3Util_RotZ(\W)/${1}G3_RotZ${2}/sg;
    $replaced |= $intext =~ s/(\W)G3Util_LoadTexMtx(\W)/${1}G3_LoadTexMtx${2}/sg;

    $replaced |= $intext =~ s/(\W)G3Util_BeginLoadTex(\W)/${1}GX_BeginLoadTex${2}/sg;
    $replaced |= $intext =~ s/(\W)G3Util_LoadTex(\W)/${1}GX_LoadTex${2}/sg;
    $replaced |= $intext =~ s/(\W)G3Util_EndLoadTex(\W)/${1}GX_EndLoadTex${2}/sg;
    
    $replaced |= $intext =~ s/(\W)G3Util_BeginLoadTexPltt(\W)/${1}GX_BeginLoadTexPltt${2}/sg;
    $replaced |= $intext =~ s/(\W)G3Util_LoadTexPltt(\W)/${1}GX_LoadTexPltt${2}/sg;
    $replaced |= $intext =~ s/(\W)G3Util_EndLoadTexPltt(\W)/${1}GX_EndLoadTexPltt${2}/sg;

    $replaced |= $intext =~ s/(\W)G3Util_BeginLoadClearImage(\W)/${1}GX_BeginLoadClearImage${2}/sg;
    $replaced |= $intext =~ s/(\W)G3Util_LoadClearImageColor(\W)/${1}GX_LoadClearImageColor${2}/sg;
    $replaced |= $intext =~ s/(\W)G3Util_LoadClearImageDepth(\W)/${1}GX_LoadClearImageDepth${2}/sg;
    $replaced |= $intext =~ s/(\W)G3Util_EndLoadClearImage(\W)/${1}GX_EndLoadClearImage${2}/sg;

    #
    # g2util.h
    #
    
    $replaced |= $intext =~ s/(\W)G2Util_LoadBGPltt(\W)/${1}GX_LoadBGPltt${2}/sg;
    $replaced |= $intext =~ s/(\W)G2Util_LoadOBJPltt(\W)/${1}GX_LoadOBJPltt${2}/sg;
    $replaced |= $intext =~ s/(\W)G2Util_LoadOAM(\W)/${1}GX_LoadOAM${2}/sg;
    $replaced |= $intext =~ s/(\W)G2Util_LoadOBJ(\W)/${1}GX_LoadOBJ${2}/sg;
    $replaced |= $intext =~ s/(\W)G2Util_LoadBG0Scr(\W)/${1}GX_LoadBG0Scr${2}/sg;
    $replaced |= $intext =~ s/(\W)G2Util_LoadBG1Scr(\W)/${1}GX_LoadBG1Scr${2}/sg;
    $replaced |= $intext =~ s/(\W)G2Util_LoadBG2Scr(\W)/${1}GX_LoadBG2Scr${2}/sg;
    $replaced |= $intext =~ s/(\W)G2Util_LoadBG3Scr(\W)/${1}GX_LoadBG3Scr${2}/sg;
    $replaced |= $intext =~ s/(\W)G2Util_LoadBG0Char(\W)/${1}GX_LoadBG0Char${2}/sg;
    $replaced |= $intext =~ s/(\W)G2Util_LoadBG1Char(\W)/${1}GX_LoadBG1Char${2}/sg;
    $replaced |= $intext =~ s/(\W)G2Util_LoadBG2Char(\W)/${1}GX_LoadBG2Char${2}/sg;
    $replaced |= $intext =~ s/(\W)G2Util_LoadBG3Char(\W)/${1}GX_LoadBG3Char${2}/sg;
    
    $replaced |= $intext =~ s/(\W)G2Util_BeginLoadBGExtPltt(\W)/${1}GX_BeginLoadBGExtPltt${2}/sg;
    $replaced |= $intext =~ s/(\W)G2Util_LoadBGExtPltt(\W)/${1}GX_LoadBGExtPltt${2}/sg;
    $replaced |= $intext =~ s/(\W)G2Util_EndLoadBGExtPltt(\W)/${1}GX_EndLoadBGExtPltt${2}/sg;
    
    $replaced |= $intext =~ s/(\W)G2Util_BeginLoadOBJExtPltt(\W)/${1}GX_BeginLoadOBJExtPltt${2}/sg;
    $replaced |= $intext =~ s/(\W)G2Util_LoadOBJExtPltt(\W)/${1}GX_LoadOBJExtPltt${2}/sg;
    $replaced |= $intext =~ s/(\W)G2Util_EndLoadOBJExtPltt(\W)/${1}GX_EndLoadOBJExtPltt${2}/sg;

    #
    # gx_vramcnt.h
    #
    $replaced |= $intext =~ s/(\W)GX_SetBankForBGEXTPLTT(\W)/${1}GX_SetBankForBGExtPltt${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_SetBankForOBJEXTPLTT(\W)/${1}GX_SetBankForOBJExtPltt${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_SetBankForTEX(\W)/${1}GX_SetBankForTex${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_SetBankForTEXPLTT(\W)/${1}GX_SetBankForTexPltt${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_SetBankForCLRIMG(\W)/${1}GX_SetBankForClearImage${2}/sg;

    $replaced |= $intext =~ s/(\W)GX_ResetBankForBGEXTPLTT(\W)/${1}GX_ResetBankForBGExtPltt${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_ResetBankForOBJEXTPLTT(\W)/${1}GX_ResetBankForOBJExtPltt${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_ResetBankForTEX(\W)/${1}GX_ResetBankForTex${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_ResetBankForTEXPLTT(\W)/${1}GX_ResetBankForTexPltt${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_ResetBankForCLRIMG(\W)/${1}GX_ResetBankForClearImage${2}/sg;

    $replaced |= $intext =~ s/(\W)GX_DisableBankForBGEXTPLTT(\W)/${1}GX_DisableBankForBGExtPltt${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_DisableBankForOBJEXTPLTT(\W)/${1}GX_DisableBankForOBJExtPltt${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_DisableBankForTEX(\W)/${1}GX_DisableBankForTex${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_DisableBankForTEXPLTT(\W)/${1}GX_DisableBankForTexPltt${2}/sg;
    $replaced |= $intext =~ s/(\W)GX_DisableBankForCLRIMG(\W)/${1}GX_DisableBankForClearImage${2}/sg;

    if ($replaced == 0) {
        next;
    }

    print STDERR "$fullpath replaced\n";
    open OUTFILE, ">$file" or die;
    print OUTFILE $intext;
    close(OUTFILE);
}


sub process_file {
    if ($File::Find::name =~ m/\.h$/ ||
        $File::Find::name =~ m/\.c$/ ||
        $File::Find::name =~ m/\.html$/) {
        replace_content($File::Find::name);
    }
}


find(\&process_file, @dirlist);


