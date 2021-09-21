/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WFS - libraries
  File:     wfs_format.c

  Copyright 2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/


#include <nitro/wfs/format.h>


/*---------------------------------------------------------------------------*/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         WFS_LoadTable

  Description:  デバイスからNTRバイナリのFATを読み込み.

  Arguments:    archive          初期化するWFSTableFormat構造体.
                allocator        内部でメモリ確保に使用するアロケータ.
                device           FATの読み込み対象が格納されたデバイス.
                origin_a         元になるFATを読み込むROMのデバイス内オフセット.
                origin_b         オーバーレイをマージするROMのデバイス内オフセット.

  Returns:      FATを正しくロードできればTRUE.
 *---------------------------------------------------------------------------*/
BOOL WFS_LoadTable(WFSTableFormat *archive, MIAllocator *allocator,
                   MIDevice *device, u32 origin_a, u32 origin_b)
{
    BOOL            retval = FALSE;
    const u32       diff = origin_b - origin_a;
    const BOOL      mixed = (diff != 0);
    u32             mixed_overlay_count = 0;
    u32             origin_b_fat = 0;
    CARDRomRegion   mem_region[WFS_TABLE_REGION_MAX];
    int             i;

    /* 構造体を初期化 */
    archive->origin = origin_a;
    archive->buffer = NULL;
    archive->length = 0;

    /* Aのヘッダを読み込んでfat/fntに記録 */
    {
        u8                      buf[0x60];
        const CARDRomHeader    *header = (const CARDRomHeader *)buf;
        (void)MI_ReadDevice(device, buf, archive->origin, sizeof(buf));
        archive->region[WFS_TABLE_REGION_FAT].offset = MI_LoadLE32(&header->fat.offset);
        archive->region[WFS_TABLE_REGION_FAT].length = MI_LoadLE32(&header->fat.length);
        archive->region[WFS_TABLE_REGION_FNT].offset = MI_LoadLE32(&header->fnt.offset);
        archive->region[WFS_TABLE_REGION_FNT].length = MI_LoadLE32(&header->fnt.length);
    }
    /* Bのヘッダを読み込んでov9/ov7に記録 */
    {
        u8                      buf[0x60];
        const CARDRomHeader    *header = (const CARDRomHeader *)buf;
        (void)MI_ReadDevice(device, buf, origin_b, sizeof(buf));
        archive->region[WFS_TABLE_REGION_OV9].offset = MI_LoadLE32(&header->main_ovt.offset) + diff;
        archive->region[WFS_TABLE_REGION_OV9].length = MI_LoadLE32(&header->main_ovt.length);
        archive->region[WFS_TABLE_REGION_OV7].offset = MI_LoadLE32(&header->sub_ovt.offset) + diff;
        archive->region[WFS_TABLE_REGION_OV7].length = MI_LoadLE32(&header->sub_ovt.length);
        origin_b_fat = header->fat.offset + diff;
    }
    /* mixedなら fat += (ov9 + ov7) */
    if (mixed)
    {
        mixed_overlay_count = (archive->region[WFS_TABLE_REGION_OV9].length +
                               archive->region[WFS_TABLE_REGION_OV7].length)
                              / sizeof(WFSOVLFormat);
    }
    /* lengthを計算 */
    {
        u32 offset = 0;
        offset += sizeof(u32);
        for (i = 0; i < WFS_TABLE_REGION_MAX; ++i)
        {
            mem_region[i].offset = offset;
            mem_region[i].length = archive->region[i].length;
            if (i == WFS_TABLE_REGION_FAT)
            {
                mem_region[i].length += mixed_overlay_count * sizeof(WFSFATFormat);
            }
            mem_region[i].length = MATH_ROUNDUP(mem_region[i].length, 32);
            offset += sizeof(u32) + mem_region[i].length;
        }
        archive->length = offset;
    }
    /* bufferを確保 */
    if (allocator)
    {
        archive->buffer = MI_CallAlloc(allocator, archive->length, sizeof(u32));
        if (archive->buffer)
        {
            /* AまたはBのfat/fnt/ov9/ov7を読み込み ※originは調整済み */
            {
                u8     *dst = archive->buffer;
                MI_StoreLE32(dst, archive->origin);
                dst += sizeof(u32);
                for (i = 0; i < WFS_TABLE_REGION_MAX; ++i)
                {
                    const u32 length = archive->region[i].length;
                    const u32 padding = mem_region[i].length - length;
                    MI_StoreLE32(dst, length);
                    dst += sizeof(u32);
                    (void)MI_ReadDevice(device, dst, archive->origin + archive->region[i].offset, length);
                    dst += length;
                    MI_CpuFill8(dst, 0x00, padding);
                    dst += padding;
                }
            }
            /* mixedなら fat += (ov9 + ov7) */
            if (mixed)
            {
                u32     count = archive->region[WFS_TABLE_REGION_FAT].length / sizeof(WFSFATFormat);
                u32     total = count + mixed_overlay_count;
                u8     *dst = &archive->buffer[mem_region[WFS_TABLE_REGION_FAT].offset];
                WFSFATFormat *fat = (WFSFATFormat*)(dst + sizeof(u32));
                MI_StoreLE32(dst, total * sizeof(WFSFATFormat));
                for (i = WFS_TABLE_REGION_OV9; i < WFS_TABLE_REGION_MAX; ++i)
                {
                    WFSOVLFormat *ovl = (WFSOVLFormat *)&archive->buffer[mem_region[i].offset + sizeof(u32)];
                    u32     num = archive->region[i].length / sizeof(WFSOVLFormat);
                    u32     j;
                    for (j = 0; j < num; ++j)
                    {
                        if (count >= total)
                        {
                            OS_TWarning("exceeded FAT iterator! (internal logic error)");
                        }
                        else
                        {
                            WFSFATFormat entry;
                            (void)MI_ReadDevice(device, &entry,
                                                archive->origin + origin_b_fat + ovl[j].file_id * sizeof(WFSFATFormat),
                                                sizeof(entry));
                            MI_StoreLE32(&fat[count].top, MI_LEToH32(entry.top) + diff);
                            MI_StoreLE32(&fat[count].bottom, MI_LEToH32(entry.bottom) + diff);
                            MI_StoreLE32(&ovl[j].file_id, count);
                            ++count;
                        }
                    }

                }
            }
        }
    }
    return (archive->buffer != NULL);
}

/*---------------------------------------------------------------------------*
  Name:         WFS_ParseTable

  Description:  NTRバイナリのFATイメージを読み込んだメモリを解析.

  Arguments:    archive          初期化するWFSTableFormat構造体.
                                 メンバbufferとlength には前もって
                                 FATイメージが設定されている必要がある.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WFS_ParseTable(WFSTableFormat *archive)
{
    const u8   *src = archive->buffer;
    u32         pos = 0;
    int         i;
    /* ベースオフセットを取得 */
    archive->origin = MI_LoadLE32(&src[pos]);
    pos += sizeof(u32);
    /* 与えられたバッファから各領域情報を抽出する */
    for (i = 0; i < WFS_TABLE_REGION_MAX; ++i)
    {
        u32     len = MI_LoadLE32(&src[pos]);
        pos += sizeof(u32);
        archive->region[i].offset = pos;
        archive->region[i].length = len;
        pos += MATH_ROUNDUP(len, 32);
        if (pos > archive->length)
        {
            OS_TPanic("specified invalid archive buffer!");
        }
    }
}


/*---------------------------------------------------------------------------*
  $Log: wfs_format.c,v $
  Revision 1.1  2007/04/13 04:12:37  yosizaki
  initial upload.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
