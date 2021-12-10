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

    $replaced |= $intext =~ s/(\W)G2_WINDOW/${1}GX_WND_PLANEMASK/sg;
    $replaced |= $intext =~ s/(\W)G2_BLENDPLANE/${1}GX_BLEND_PLANEMASK/sg;
    
    #
    # g2oam.h
    #

    $replaced |= $intext =~ s/(\W)G2_OAM_SHAPE/${1}GX_OAM_SHAPE/sg;
    $replaced |= $intext =~ s/(\W)G2_OAM_MOSAIC/${1}GX_OAM_MOSAIC/sg;
    $replaced |= $intext =~ s/(\W)G2_OAM_COLOR/${1}GX_OAM_COLOR/sg;
    $replaced |= $intext =~ s/(\W)G2_OAM_EFFECT/${1}GX_OAM_EFFECT/sg;
    $replaced |= $intext =~ s/(\W)G2_OAM_MODE/${1}GX_OAM_MODE/sg;
    $replaced |= $intext =~ s/(\W)G2_OAM_PRIORITY_ASSERT/${1}GX_OAM_PRIORTY_ASSERT/sg;
    
    
    #
    # g3.h and its family
    #

    $replaced |= $intext =~ s/(\W)G3_BEGIN/${1}GX_BEGIN/sg;
    
    $replaced |= $intext =~ s/(\W)G3_MTX_MODE_PROJECTION/${1}GX_MTXMODE_PROJECTION/sg;
    $replaced |= $intext =~ s/(\W)G3_MTX_MODE_POSITION_VECTOR/${1}GX_MTXMODE_POSITION_VECTOR/sg;
    $replaced |= $intext =~ s/(\W)G3_MTX_MODE_POSITION/${1}GX_MTXMODE_POSITION/sg;
    $replaced |= $intext =~ s/(\W)G3_MTX_MODE_TEXTURE/${1}GX_MTXMODE_TEXTURE/sg;
    $replaced |= $intext =~ s/(\W)G3_MTX_MODE/${1}GX_MTXMODE/sg;

    $replaced |= $intext =~ s/(\W)G3_SWAP_BUFFERS_AUTO/${1}GX_SORTMODE_AUTO/sg;
    $replaced |= $intext =~ s/(\W)G3_SWAP_BUFFERS_MANUAL/${1}GX_SORTMODE_MANUAL/sg;
    $replaced |= $intext =~ s/(\W)G3_SWAP_BUFFERS_AM/${1}GX_SORTMODE/sg;

    $replaced |= $intext =~ s/(\W)G3_SWAP_BUFFERS_ZW/${1}GX_BUFFERMODE/sg;
    $replaced |= $intext =~ s/(\W)G3_SWAP_BUFFERS_Z/${1}GX_BUFFERMODE_Z/sg;
    $replaced |= $intext =~ s/(\W)G3_SWAP_BUFFERS_W/${1}GX_BUFFERMODE_W/sg;

    $replaced |= $intext =~ s/(\W)G3_POLYGON_ATTR_L([0-9N])/${1}GX_LIGHTMASK_${2}/sg;
    $replaced |= $intext =~ s/(\W)G3_POLYGON_ATTR_L([\W_])/${1}GX_LIGHTMASK${2}/sg;

    $replaced |= $intext =~ s/(\W)G3_POLYGON_ATTR_POLYMODE_MODULATE/${1}GX_POLYGONMODE_MODULATE/sg;
    $replaced |= $intext =~ s/(\W)G3_POLYGON_ATTR_POLYMODE_DECAL/${1}GX_POLYGONMODE_DECAL/sg;
    $replaced |= $intext =~ s/(\W)G3_POLYGON_ATTR_POLYMODE_TOON/${1}GX_POLYGONMODE_TOON/sg;
    $replaced |= $intext =~ s/(\W)G3_POLYGON_ATTR_POLYMODE_SHADOW/${1}GX_POLYGONMODE_SHADOW/sg;
    $replaced |= $intext =~ s/(\W)G3_POLYGON_ATTR_POLYMODE/${1}GX_POLYGONMODE/sg;

    $replaced |= $intext =~ s/(\W)G3_POLYGON_ATTR_CULL_ALL/${1}GX_CULL_ALL/sg;
    $replaced |= $intext =~ s/(\W)G3_POLYGON_ATTR_CULL_FRONT/${1}GX_CULL_FRONT/sg;
    $replaced |= $intext =~ s/(\W)G3_POLYGON_ATTR_CULL_BACK/${1}GX_CULL_BACK/sg;
    $replaced |= $intext =~ s/(\W)G3_POLYGON_ATTR_CULL_NONE/${1}GX_CULL_NONE/sg;
    $replaced |= $intext =~ s/(\W)G3_POLYGON_ATTR_CULL/${1}GX_CULL/sg;

    $replaced |= $intext =~ s/(\W)G3_POLYGON_ATTR_MISC/${1}GX_POLYGON_ATTR_MISC/sg;

    $replaced |= $intext =~ s/(\W)G3_TEXIMAGE_PARAM_REPEAT_NONE/${1}GX_TEXREPEAT_NONE/sg;
    $replaced |= $intext =~ s/(\W)G3_TEXIMAGE_PARAM_REPEAT_ST/${1}GX_TEXREPEAT_ST/sg;
    $replaced |= $intext =~ s/(\W)G3_TEXIMAGE_PARAM_REPEAT_S/${1}GX_TEXREPEAT_S/sg;
    $replaced |= $intext =~ s/(\W)G3_TEXIMAGE_PARAM_REPEAT_T/${1}GX_TEXREPEAT_T/sg;
    $replaced |= $intext =~ s/(\W)G3_TEXIMAGE_PARAM_REPEAT/${1}GX_TEXREPEAT/sg;

    $replaced |= $intext =~ s/(\W)G3_TEXIMAGE_PARAM_FLIP_NONE/${1}GX_TEXFLIP_NONE/sg;
    $replaced |= $intext =~ s/(\W)G3_TEXIMAGE_PARAM_FLIP_ST/${1}GX_TEXFLIP_ST/sg;
    $replaced |= $intext =~ s/(\W)G3_TEXIMAGE_PARAM_FLIP_S/${1}GX_TEXFLIP_S/sg;
    $replaced |= $intext =~ s/(\W)G3_TEXIMAGE_PARAM_FLIP_T/${1}GX_TEXFLIP_T/sg;
    $replaced |= $intext =~ s/(\W)G3_TEXIMAGE_PARAM_FLIP/${1}GX_TEXFLIP/sg;

    $replaced |= $intext =~ s/(\W)G3_TEXIMAGE_PARAM_SIZE_S8/${1}GX_TEXSIZE_S8/sg;
    $replaced |= $intext =~ s/(\W)G3_TEXIMAGE_PARAM_SIZE_S16/${1}GX_TEXSIZE_S16/sg;
    $replaced |= $intext =~ s/(\W)G3_TEXIMAGE_PARAM_SIZE_S32/${1}GX_TEXSIZE_S32/sg;
    $replaced |= $intext =~ s/(\W)G3_TEXIMAGE_PARAM_SIZE_S64/${1}GX_TEXSIZE_S64/sg;
    $replaced |= $intext =~ s/(\W)G3_TEXIMAGE_PARAM_SIZE_S128/${1}GX_TEXSIZE_S128/sg;
    $replaced |= $intext =~ s/(\W)G3_TEXIMAGE_PARAM_SIZE_S256/${1}GX_TEXSIZE_S256/sg;
    $replaced |= $intext =~ s/(\W)G3_TEXIMAGE_PARAM_SIZE_S512/${1}GX_TEXSIZE_S512/sg;
    $replaced |= $intext =~ s/(\W)G3_TEXIMAGE_PARAM_SIZE_S1024/${1}GX_TEXSIZE_S1024/sg;
    $replaced |= $intext =~ s/(\W)G3_TEXIMAGE_PARAM_SIZE_S/${1}GX_TEXSIZE_S/sg;

    $replaced |= $intext =~ s/(\W)G3_TEXIMAGE_PARAM_SIZE_T8/${1}GX_TEXSIZE_T8/sg;
    $replaced |= $intext =~ s/(\W)G3_TEXIMAGE_PARAM_SIZE_T16/${1}GX_TEXSIZE_T16/sg;
    $replaced |= $intext =~ s/(\W)G3_TEXIMAGE_PARAM_SIZE_T32/${1}GX_TEXSIZE_T32/sg;
    $replaced |= $intext =~ s/(\W)G3_TEXIMAGE_PARAM_SIZE_T64/${1}GX_TEXSIZE_T64/sg;
    $replaced |= $intext =~ s/(\W)G3_TEXIMAGE_PARAM_SIZE_T128/${1}GX_TEXSIZE_T128/sg;
    $replaced |= $intext =~ s/(\W)G3_TEXIMAGE_PARAM_SIZE_T256/${1}GX_TEXSIZE_T256/sg;
    $replaced |= $intext =~ s/(\W)G3_TEXIMAGE_PARAM_SIZE_T512/${1}GX_TEXSIZE_T512/sg;
    $replaced |= $intext =~ s/(\W)G3_TEXIMAGE_PARAM_SIZE_T1024/${1}GX_TEXSIZE_T1024/sg;
    $replaced |= $intext =~ s/(\W)G3_TEXIMAGE_PARAM_SIZE_T/${1}GX_TEXSIZE_T/sg;

    $replaced |= $intext =~ s/(\W)G3_TEXIMAGE_PARAM_TEXFMT_NONE/${1}GX_TEXFMT_NONE/sg;
    $replaced |= $intext =~ s/(\W)G3_TEXIMAGE_PARAM_TEXFMT_PLTT2/${1}GX_TEXFMT_PLTT2/sg;
    $replaced |= $intext =~ s/(\W)G3_TEXIMAGE_PARAM_TEXFMT_PLTT4/${1}GX_TEXFMT_PLTT4/sg;
    $replaced |= $intext =~ s/(\W)G3_TEXIMAGE_PARAM_TEXFMT_PLTT16/${1}GX_TEXFMT_PLTT16/sg;
    $replaced |= $intext =~ s/(\W)G3_TEXIMAGE_PARAM_TEXFMT_PLTT256/${1}GX_TEXFMT_PLTT256/sg;
    $replaced |= $intext =~ s/(\W)G3_TEXIMAGE_PARAM_TEXFMT_COMP4x4/${1}GX_TEXFMT_COMP4x4/sg;
    $replaced |= $intext =~ s/(\W)G3_TEXIMAGE_PARAM_TEXFMT_ALPHA/${1}GX_TEXFMT_ALPHA/sg;
    $replaced |= $intext =~ s/(\W)G3_TEXIMAGE_PARAM_TEXFMT_DIRECT/${1}GX_TEXFMT_DIRECT/sg;
    $replaced |= $intext =~ s/(\W)G3_TEXIMAGE_PARAM_TEXFMT/${1}GX_TEXFMT/sg;

    $replaced |= $intext =~ s/(\W)G3_TEXIMAGE_PARAM_PLTT0_USE/${1}GX_TEXPLTTCOLOR0_USE/sg;
    $replaced |= $intext =~ s/(\W)G3_TEXIMAGE_PARAM_PLTT0_XLU/${1}GX_TEXPLTTCOLOR0_TRNS/sg;
    $replaced |= $intext =~ s/(\W)G3_TEXIMAGE_PARAM_PLTT0/${1}GX_TEXPLTTCOLOR0/sg;

    $replaced |= $intext =~ s/(\W)G3_TEXIMAGE_PARAM_TEXGEN_NONE/${1}GX_TEXGEN_NONE/sg;
    $replaced |= $intext =~ s/(\W)G3_TEXIMAGE_PARAM_TEXGEN_TEXCOORD/${1}GX_TEXGEN_TEXCOORD/sg;
    $replaced |= $intext =~ s/(\W)G3_TEXIMAGE_PARAM_TEXGEN_NORMAL/${1}GX_TEXGEN_NORMAL/sg;
    $replaced |= $intext =~ s/(\W)G3_TEXIMAGE_PARAM_TEXGEN_VERTEX/${1}GX_TEXGEN_VERTEX/sg;
    $replaced |= $intext =~ s/(\W)G3_TEXIMAGE_PARAM_TEXGEN/${1}GX_TEXGEN/sg;

    $replaced |= $intext =~ s/(\W)G3_LIGHT_0/${1}GX_LIGHTID_0/sg;
    $replaced |= $intext =~ s/(\W)G3_LIGHT_1/${1}GX_LIGHTID_1/sg;
    $replaced |= $intext =~ s/(\W)G3_LIGHT_2/${1}GX_LIGHTID_2/sg;
    $replaced |= $intext =~ s/(\W)G3_LIGHT_3/${1}GX_LIGHTID_3/sg;
    $replaced |= $intext =~ s/(\W)G3_LIGHT/${1}GX_LIGHTID/sg;

    #
    # g3x.h
    #

    $replaced |= $intext =~ s/(\W)G3_DISP3DCNT_SHADING/${1}GX_SHADING/sg;
    $replaced |= $intext =~ s/(\W)G3_DISP3DCNT_FOG_BLEND_PIXELALPHA/${1}GX_FOGBLEND_PIXELALPHA/sg;
    $replaced |= $intext =~ s/(\W)G3_DISP3DCNT_FOG_BLEND_PIXEL/${1}GX_FOGBLEND_PIXEL/sg;
    $replaced |= $intext =~ s/(\W)G3_DISP3DCNT_FOG/${1}GX_FOGBLEND/sg;
    $replaced |= $intext =~ s/(\W)G3_FOGSHIFT/${1}GX_FOGSLOPE/sg;
    $replaced |= $intext =~ s/(\W)G3_FIFOINTR_COND/${1}GX_FIFOINTR_COND/sg;

    #
    # g3xstatus.h
    #
    $replaced |= $intext =~ s/(\W)G3_FIFOSTAT/${1}GX_FIFOSTAT/sg;

    #
    # gx.h
    #
    $replaced |= $intext =~ s/(\W)GX_DISPCNT_BGMODE/${1}GX_BGMODE/sg;
    $replaced |= $intext =~ s/(\W)GX_DISPCNT_BG0_2D3D/${1}GX_BG0_AS/sg;
    $replaced |= $intext =~ s/(\W)GX_DISPCNT_CHOBJMAP/${1}GX_OBJMAP_CHAR/sg;
    $replaced |= $intext =~ s/(\W)GX_DISPCNT_BMOBJMAP/${1}GX_OBJMAP_BMP/sg;
    $replaced |= $intext =~ s/(\W)GX_DISPCNT_PLANE/${1}GX_PLANEMASK/sg;
    $replaced |= $intext =~ s/(\W)GX_DISPCNT_WINDOW/${1}GX_WNDMASK/sg;
    $replaced |= $intext =~ s/(\W)GX_DISPCNT_DISPMODE/${1}GX_DISPMODE/sg;
    $replaced |= $intext =~ s/(\W)GX_DISPCNT_CHOBJVRAM/${1}GX_OBJVRAM_CHAR/sg;
    $replaced |= $intext =~ s/(\W)GX_DISPCNT_BMOBJVRAM/${1}GX_OBJVRAM_BMP/sg;
    $replaced |= $intext =~ s/(\W)GX_DISPCNT_BGCHAROFFSET/${1}GX_BGCHAROFFSET/sg;
    $replaced |= $intext =~ s/(\W)GX_DISPCNT_BGSCREENOFFSET/${1}GX_BGSCROFFSET/sg;

    #
    # gx_bgcnt.h
    #

    #
    # gx_capture.h
    #
    $replaced |= $intext =~ s/(\W)GX_DISPCAPCNT_DEST/${1}GX_CAPTURE_DEST/sg;
    $replaced |= $intext =~ s/(\W)GX_DISPCAPCNT_CAPTURESIZE/${1}GX_CAPTURE_SIZE/sg;
    $replaced |= $intext =~ s/(\W)GX_DISPCAPCNT_SRCA/${1}GX_CAPTURE_SRCA/sg;
    $replaced |= $intext =~ s/(\W)GX_DISPCAPCNT_SRCB/${1}GX_CAPTURE_SRCB/sg;
    $replaced |= $intext =~ s/(\W)GX_DISPCAPCNT_CAPTUREMODE/${1}GX_CAPTURE_MODE/sg;

    #
    # gx_vramcnt.h
    #
    $replaced |= $intext =~ s/(\W)GX_VRAMCNT_LCDC/${1}GX_VRAM_LCDC/sg;
    $replaced |= $intext =~ s/(\W)GX_VRAMCNT_BG/${1}GX_VRAM_BG/sg;
    $replaced |= $intext =~ s/(\W)GX_VRAMCNT_OBJ/${1}GX_VRAM_OBJ/sg;
    $replaced |= $intext =~ s/(\W)GX_VRAMCNT_ARM7/${1}GX_VRAM_ARM7/sg;
    $replaced |= $intext =~ s/(\W)GX_VRAMCNT_TEX/${1}GX_VRAM_TEX/sg;
    $replaced |= $intext =~ s/(\W)GX_VRAMCNT_CLRIMG/${1}GX_VRAM_CLRIMG/sg;
    $replaced |= $intext =~ s/(\W)GX_VRAMCNT_TEXPLTT/${1}GX_VRAM_TEXPLTT/sg;
    $replaced |= $intext =~ s/(\W)GX_VRAMCNT_BGEXTPLTT/${1}GX_VRAM_BGEXTPLTT/sg;
    $replaced |= $intext =~ s/(\W)GX_VRAMCNT_OBJEXTPLTT/${1}GX_VRAM_OBJEXTPLTT/sg;
    $replaced |= $intext =~ s/(\W)GX_VRAMCNT_WRAM/${1}GX_WRAM/sg;

    #
    # macros(must have GX prefix)
    #
    $replaced |= $intext =~ s/(\W)G2_BG0CNT_PRIORITY_ASSERT/${1}GX_BG0_PRIORITY_ASSERT/sg;
    $replaced |= $intext =~ s/(\W)G2_BG1CNT_PRIORITY_ASSERT/${1}GX_BG1_PRIORITY_ASSERT/sg;
    $replaced |= $intext =~ s/(\W)G2_BG2CNT_PRIORITY_ASSERT/${1}GX_BG2_PRIORITY_ASSERT/sg;
    $replaced |= $intext =~ s/(\W)G2_BG3CNT_PRIORITY_ASSERT/${1}GX_BG3_PRIORITY_ASSERT/sg;
    $replaced |= $intext =~ s/(\W)G3_ST/${1}GX_ST/sg;
    $replaced |= $intext =~ s/(\W)G3_VECVTXDIFF/${1}GX_VECVTXDIFF/sg;
    $replaced |= $intext =~ s/(\W)G3_VECVTXSHORT/${1}GX_VECVTXSHORT/sg;
    $replaced |= $intext =~ s/(\W)G3_FX16PAIR/${1}GX_FX16PAIR/sg;
    $replaced |= $intext =~ s/(\W)G3X_RDLINES_COUNT_MAX/${1}GX_RDLINES_COUNT_MAX/sg;
    $replaced |= $intext =~ s/(\W)G3X_LISTRAM_COUNT_MAX/${1}GX_LISTRAM_COUNT_MAX/sg;
    $replaced |= $intext =~ s/(\W)G3X_VTXRAM_COUNT_MAX/${1}GX_VTXRAM_COUNT_MAX/sg;
    $replaced |= $intext =~ s/(\W)G3_POLYGONID_ASSERT/${1}GX_POLYGONID_ASSERT/sg;
    $replaced |= $intext =~ s/(\W)G3_ALPHA_ASSERT/${1}GX_ALPHA_ASSERT/sg;
    $replaced |= $intext =~ s/(\W)G3_DEPTH_ASSERT/${1}GX_DEPTH_ASSERT/sg;
    $replaced |= $intext =~ s/(\W)G2_MOSAICSIZE_ASSERT/${1}GX_MOSAICSIZE_ASSERT/sg;
    $replaced |= $intext =~ s/(\W)G2OAM_ATTR01/${1}GX_OAM_ATTR01/sg;
    $replaced |= $intext =~ s/(\W)G2OAM_ATTR2/${1}GX_OAM_ATTR2/sg;
    $replaced |= $intext =~ s/(\W)G2OAM_OBJNUM_ASSERT/${1}GX_OAM_OBJNUM_ASSERT/sg;
    $replaced |= $intext =~ s/(\W)G2OAM_CPARAM_ASSERT/${1}GX_OAM_CPARAM_ASSERT/sg;
    $replaced |= $intext =~ s/(\W)G2OAM_RSPARAM_ASSERT/${1}GX_OAM_RSPARAM_ASSERT/sg;
    $replaced |= $intext =~ s/(\W)G2OAM_CHARNAME_ASSERT/${1}GX_OAM_CHARNAME_ASSERT/sg;

    $replaced |= $intext =~ s/(\W)G3(\w+)ASSERT/${1}GX${2}ASSERT/sg;
    
    #
    # types(must have GX prefix)
    #
    $replaced |= $intext =~ s/G2OamAttr/GXOamAttr/sg;
    $replaced |= $intext =~ s/G2OamRs/GXOamAffine/sg;
    $replaced |= $intext =~ s/G2OamAffine/GXOamAffine/sg;
    $replaced |= $intext =~ s/G3XMiscStatus/GXMiscStatus/sg;
    $replaced |= $intext =~ s/G3St/GXSt/sg;
    $replaced |= $intext =~ s/G3TexPlttBaseParam/GXTexPlttBaseParam/sg;
    $replaced |= $intext =~ s/G3DLInfo/GXDLInfo/sg;
    $replaced |= $intext =~ s/G3BoxTestParam/GXBoxTestParam/sg;

    #
    # functions
    #
    $replaced |= $intext =~ s/G3X_GetG3XMiscStatus/G3X_GetMiscStatus/sg;
    $replaced |= $intext =~ s/G3X_SetG3XMiscStatus/G3X_SetMiscStatus/sg;

    # g2.h
    $replaced |= $intext =~ s/G2_SetWindow0InsidePlane/G2_SetWnd0InsidePlane/sg;
    $replaced |= $intext =~ s/G2_SetWindow1InsidePlane/G2_SetWnd1InsidePlane/sg;
    $replaced |= $intext =~ s/G2_SetWindowOBJInsidePlane/G2_SetWndOBJInsidePlane/sg;
    $replaced |= $intext =~ s/G2_SetWindowOutsidePlane/G2_SetWndOutsidePlane/sg;
    $replaced |= $intext =~ s/G2_SetWindow0Position/G2_SetWnd0Position/sg;
    $replaced |= $intext =~ s/G2_SetWindow1Position/G2_SetWnd1Position/sg;

    # g2oam.h
    $replaced |= $intext =~ s/G2_SetOBJXY/G2_SetOBJPosition/sg;
    $replaced |= $intext =~ s/G2_SetOBJAttribute/G2_SetOBJAttr/sg;

    # g2util.h
    $replaced |= $intext =~ s/G2Util_LoadBG0Screen/G2Util_LoadBG0Scr/sg;
    $replaced |= $intext =~ s/G2Util_LoadBG1Screen/G2Util_LoadBG1Scr/sg;
    $replaced |= $intext =~ s/G2Util_LoadBG2Screen/G2Util_LoadBG2Scr/sg;
    $replaced |= $intext =~ s/G2Util_LoadBG3Screen/G2Util_LoadBG3Scr/sg;

    # gx.h
    $replaced |= $intext =~ s/GX_BlankScreen/GX_BlankScr/sg;
    $replaced |= $intext =~ s/GX_GetBGScreenOffset/GX_GetBGScrOffset/sg;
    $replaced |= $intext =~ s/GX_SetBGScreenOffset/GX_SetBGScrOffset/sg;
    $replaced |= $intext =~ s/GX_SetVisibleWindow/GX_SetVisibleWnd/sg;
    
    # g3util.h
    $replaced |= $intext =~ s/G3Util_BeginLoadTexture/G3Util_BeginLoadTex/sg;
    $replaced |= $intext =~ s/G3Util_LoadTextureMtx/G3Util_LoadTexMtx/sg;
    $replaced |= $intext =~ s/G3Util_BeginLoadTexturePltt/G3Util_BeginLoadTexPltt/sg;
    $replaced |= $intext =~ s/G3Util_LoadTexturePltt/G3Util_LoadTexPltt/sg;
    $replaced |= $intext =~ s/G3Util_EndLoadTexturePltt/G3Util_EndLoadTexPltt/sg;
    $replaced |= $intext =~ s/G3Util_BeginLoadTexure/G3Util_BeginLoadTex/sg;
    $replaced |= $intext =~ s/G3Util_LoadTexture/G3Util_LoadTex/sg;
    $replaced |= $intext =~ s/G3Util_EndLoadTexture/G3Util_EndLoadTex/sg;

    $replaced |= $intext =~ s/G2_GetBG0ScreenPtr/G2_GetBG0ScrPtr/sg;
    $replaced |= $intext =~ s/G2_GetBG1ScreenPtr/G2_GetBG1ScrPtr/sg;
    $replaced |= $intext =~ s/G2_GetBG2ScreenPtr/G2_GetBG2ScrPtr/sg;
    $replaced |= $intext =~ s/G2_GetBG3ScreenPtr/G2_GetBG3ScrPtr/sg;

    # g3imm.h, g3b.h g3c.h
    $replaced |= $intext =~ s/(G3\w*_)Vertex/${1}Vtx/sg;

    #
    # solve 'set' problem
    #
    
    # g3x.h
    $replaced |= $intext =~ s/(\W)G3X_Fog(\W)/${1}G3X_SetFog${2}/sg;
    $replaced |= $intext =~ s/(\W)G3X_Shading(\W)/${1}G3X_SetShading${2}/sg;

    # gx.h
    $replaced |= $intext =~ s/(\W)GX_Power(\W)/${1}GX_SetPower${2}/sg;

    # gx_bgcnt.h
    $replaced |= $intext =~ s/(\W)G2_BG0Priority(\W)/${1}G2_SetBG0Priority${2}/sg;
    $replaced |= $intext =~ s/(\W)G2_BG1Priority(\W)/${1}G2_SetBG1Priority${2}/sg;
    $replaced |= $intext =~ s/(\W)G2_BG2Priority(\W)/${1}G2_SetBG2Priority${2}/sg;
    $replaced |= $intext =~ s/(\W)G2_BG3Priority(\W)/${1}G2_SetBG3Priority${2}/sg;

    # gx_capture.h
    $replaced |= $intext =~ s/(\W)GX_Capture(\W)/${1}GX_SetCapture${2}/sg;

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


