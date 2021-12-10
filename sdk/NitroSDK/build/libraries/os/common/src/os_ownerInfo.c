/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS
  File:     ownerInfo.c

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: os_ownerInfo.c,v $
  Revision 1.14  2007/02/20 00:28:10  kitase_hirotake
  indent source

  Revision 1.13  2006/10/25 04:47:27  kitase_hirotake
  OSOwnerInfo の nickName, comment に終端コードを含めるように変更

  Revision 1.12  2006/07/05 09:15:31  yosizaki
  add systemWork header.

  Revision 1.11  2006/01/18 02:11:30  kitase_hirotake
  do-indent

  Revision 1.10  2005/04/26 07:59:56  terui
  Fix comment

  Revision 1.9  2005/04/18 12:27:31  terui
  OSi_CheckOwnerInfo関数を削除

  Revision 1.8  2005/04/12 10:49:15  terui
  中国語対応の為に改造

  Revision 1.7  2005/03/01 01:57:00  yosizaki
  copyright の年を修正.

  Revision 1.6  2005/02/28 05:26:29  yosizaki
  do-indent.

  Revision 1.5  2004/10/07 10:19:27  yosizaki
  add favorite color API.

  Revision 1.4  2004/09/13 05:49:56  yasu
  merged with NitroSDK_2_00pr6_branch

  Revision 1.3.2.1  2004/09/13 05:43:02  yasu
  Support SDK_NVRAM_FORMAT

  Revision 1.3  2004/09/03 08:01:20  terui
  OS_GetOwnerRtcOffset関数を追加。

  Revision 1.2  2004/09/03 04:29:15  terui
  NVRAM内ユーザー情報に関する構造体定義整理に伴う修正。

  Revision 1.1  2004/09/02 06:21:20  terui
  Initial upload

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include    <nitro/os/common/ownerInfo.h>
#include    <nitro/os/common/systemWork.h>


/*---------------------------------------------------------------------------*
  Name:         OS_GetMacAddress

  Description:  無線ハードウェアのMACアドレスを取得する。

  Arguments:    macAddress  -   MACアドレスを取得するバッファへのポインタ。
                                このバッファに6バイトのデータが書き込まれる。

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OS_GetMacAddress(u8 *macAddress)
{
    u8     *src;

    src =
        (u8 *)((u32)(OS_GetSystemWork()->nvramUserInfo) +
               ((sizeof(NVRAMConfig) + 3) & ~0x00000003));
    MI_CpuCopy8(src, macAddress, 6);
}

#ifdef  SDK_TS
#if ( SDK_TS_VERSION >= 200 || SDK_NVRAM_FORMAT >= 100 )

/*---------------------------------------------------------------------------*
  Name:         OS_GetOwnerInfo

  Description:  オーナー情報を取得する。

  Arguments:    info    -   オーナー情報を取得するバッファへのポインタ。
                            このバッファにデータが書き込まれる。

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OS_GetOwnerInfo(OSOwnerInfo *info)
{
    NVRAMConfig *src;

    src = (NVRAMConfig *)(OS_GetSystemWork()->nvramUserInfo);
    info->language = (u8)(src->ncd.option.language);
    info->favoriteColor = (u8)(src->ncd.owner.favoriteColor);
    info->birthday.month = (u8)(src->ncd.owner.birthday.month);
    info->birthday.day = (u8)(src->ncd.owner.birthday.day);
    info->nickNameLength = (u16)(src->ncd.owner.nickname.length);
    info->commentLength = (u16)(src->ncd.owner.comment.length);
    MI_CpuCopy16(src->ncd.owner.nickname.str,
                 info->nickName, OS_OWNERINFO_NICKNAME_MAX * sizeof(u16));
    MI_CpuCopy16(src->ncd.owner.comment.str, info->comment, OS_OWNERINFO_COMMENT_MAX * sizeof(u16));
    info->nickName[OS_OWNERINFO_NICKNAME_MAX] = 0;
    info->comment[OS_OWNERINFO_COMMENT_MAX] = 0;
}

#endif

/*---------------------------------------------------------------------------*
  Name:         OS_GetOwnerRtcOffset

  Description:  オーナーがRTC設定を変更する度にその値に応じて増減するオフセット
                値(単位は秒)を取得する。
                再設定直前のRTC時刻から未来に進められた場合は進めた時間が加算
                される。過去に戻された場合は戻した時間が減算される。

  Arguments:    None.

  Returns:      s64 -   RTC設定オフセット値を返す。
 *---------------------------------------------------------------------------*/
s64 OS_GetOwnerRtcOffset(void)
{
    NVRAMConfig *src = (NVRAMConfig *)(OS_GetSystemWork()->nvramUserInfo);

#if ( SDK_TS_VERSION >= 200 || SDK_NVRAM_FORMAT >= 100 )
    return src->ncd.option.rtcOffset;
#else
    return src->ncd.rtcOffset;
#endif
}

#endif

/*---------------------------------------------------------------------------*
  Name:         OS_GetFavoriteColorTable

  Description:  OSOwnerInfo 構造体のメンバ favoriteColor の番号に対応する
                「お気に入りの色」の配列へのポインタを取得します。

  Arguments:    None.

  Returns:      対応する色値を格納した GXRgb 型の定数配列へのポインタ.
 *---------------------------------------------------------------------------*/
const GXRgb *OS_GetFavoriteColorTable(void)
{
    static const GXRgb col_table[OS_FAVORITE_COLOR_MAX] = {
        OS_FAVORITE_COLOR_VALUE_GRAY,
        OS_FAVORITE_COLOR_VALUE_BROWN,
        OS_FAVORITE_COLOR_VALUE_RED,
        OS_FAVORITE_COLOR_VALUE_PINK,
        OS_FAVORITE_COLOR_VALUE_ORANGE,
        OS_FAVORITE_COLOR_VALUE_YELLOW,
        OS_FAVORITE_COLOR_VALUE_LIME_GREEN,
        OS_FAVORITE_COLOR_VALUE_GREEN,
        OS_FAVORITE_COLOR_VALUE_DARK_GREEN,
        OS_FAVORITE_COLOR_VALUE_SEA_GREEN,
        OS_FAVORITE_COLOR_VALUE_TURQUOISE,
        OS_FAVORITE_COLOR_VALUE_BLUE,
        OS_FAVORITE_COLOR_VALUE_DARK_BLUE,
        OS_FAVORITE_COLOR_VALUE_PURPLE,
        OS_FAVORITE_COLOR_VALUE_VIOLET,
        OS_FAVORITE_COLOR_VALUE_MAGENTA,
    };
    return col_table;
}

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
