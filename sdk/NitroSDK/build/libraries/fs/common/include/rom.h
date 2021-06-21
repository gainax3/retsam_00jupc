/*---------------------------------------------------------------------------*
  Project:  NitroSDK - FS - libraries
  File:     rom.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/


#if     !defined(NITRO_FS_ROM_H_)
#define NITRO_FS_ROM_H_

#include <nitro/misc.h>
#include <nitro/types.h>
#include <nitro/fs/file.h>
#include <nitro/fs/archive.h>


/*****************************************************************************/
/* variable */

/* カード / カートリッジのロック ID */
extern s32 fsi_card_lock_id;

/* アタッチされたオーバーレイの情報 */
extern CARDRomRegion fsi_ovt9;
extern CARDRomRegion fsi_ovt7;

#if     defined(FS_IMPLEMENT)
/* ROM アーカイブ */
extern FSArchive fsi_arc_rom;
#endif /* FS_IMPLEMENT */


/*****************************************************************************/
/* function */

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*
  Name:         FSi_InitRom

  Description:  Initialize "rom" archive

  Arguments:    default_dma_no   default DMA channel for ROM
                                 if out of range(0-3),
                                 use CpuCopy instead of DMA.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    FSi_InitRom(u32 default_dma_no);

/*---------------------------------------------------------------------------*
  Name:         FSi_LoadOverlayInfoCore

  Description:  内部関数.
                指定の rom-duplicated アーカイブからオーバーレイ情報を読み出す.
                IPL 実装の利便性のために暗黙公開.

  Arguments:    p_ovi            読み出した情報を格納するポインタ.
                target           オーバーレイターゲット. (ARM9/ARM7)
                id               読み出すオーバーレイ ID.
                p_arc            読み出し先のアーカイブ.
                offset_arm9      アーカイブ内のオーバーレイ情報格納位置先頭.
                len_arm9         アーカイブ内のオーバーレイ情報サイズ.
                offset_arm7      アーカイブ内のオーバーレイ情報格納位置先頭.
                len_arm7         アーカイブ内のオーバーレイ情報サイズ.

  Returns:      オーバーレイが正常に読み出せれば TRUE.
 *---------------------------------------------------------------------------*/
BOOL    FSi_LoadOverlayInfoCore(FSOverlayInfo *p_ovi, MIProcessor target, FSOverlayID id,
                                FSArchive *p_arc,
                                u32 offset_arm9, u32 len_arm9, u32 offset_arm7, u32 len_arm7);


#ifdef __cplusplus
} /* extern "C" */
#endif


/*****************************************************************************/


#endif /* NITRO_FS_ROM_H_ */

/*---------------------------------------------------------------------------*
  $Log: rom.h,v $
  Revision 1.8  2006/01/18 02:12:28  kitase_hirotake
  do-indent

  Revision 1.7  2005/05/30 04:13:56  yosizaki
  add comments.

  Revision 1.6  2005/02/28 05:26:02  yosizaki
  do-indent.

  Revision 1.5  2005/02/18 07:45:31  yasu
  Signed/Unsigned 変換警告抑制

  Revision 1.4  2004/09/04 13:21:04  yosizaki
  add prototype of FSi_LoadOverlayInfoCore.

  Revision 1.3  2004/07/08 13:39:45  yosizaki
  extern rom archive.

  Revision 1.2  2004/05/11 04:27:57  yosizaki
  (none)

  Revision 1.1  2004/05/10 06:28:28  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
