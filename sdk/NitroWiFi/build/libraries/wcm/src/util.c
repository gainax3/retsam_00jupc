/*---------------------------------------------------------------------------*
  Project:  NitroWiFi - WCM - libraries
  File:     util.c

  Copyright 2005,2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: util.c,v $
  Revision 1.3  2006/03/10 09:22:19  kitase_hirotake
  INDENT SOURCE

  Revision 1.2  2005/10/05 08:51:45  terui
  電波強度を内部バッファに蓄積する関数の追加。
  WCM_GetLinkLevel関数により取得するリンクレベルを内部バッファのデータを元に計算するように改造。

  Revision 1.1  2005/09/10 03:08:43  terui
  Initial upload.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include "wcm_private.h"

/*---------------------------------------------------------------------------*
    定数定義
 *---------------------------------------------------------------------------*/
#define WCM_NETWORK_CAPABILITY_KIND_MASK        0x0003
#define WCM_NETWORK_CAPABILITY_KIND_ESS         0x0001
#define WCM_NETWORK_CAPABILITY_KIND_IBSS        0x0002
#define WCM_NETWORK_CAPABILITY_PRIVACY          0x0010
#define WCM_NETWORK_CAPABILITY_PREAMBLE_MASK    0x0020
#define WCM_NETWORK_CAPABILITY_PREAMBLE_LONG    0x0000
#define WCM_NETWORK_CAPABILITY_PREAMBLE_SHORT   0x0020

#define WCM_RSSI_COUNT_MAX                      16
#define WCM_RSSI_BORDER_HIGH                    28
#define WCM_RSSI_BORDER_MIDDLE                  22
#define WCM_RSSI_BORDER_LOW                     16

/*---------------------------------------------------------------------------*
    ローカル関数プロトタイプ
 *---------------------------------------------------------------------------*/
static WMLinkLevel  WcmGetLinkLevel(void);

/*---------------------------------------------------------------------------*
    変数定義
 *---------------------------------------------------------------------------*/
static u8       wcmRssi[WCM_RSSI_COUNT_MAX];
static u8       wcmRssiIndex = 0;

/*---------------------------------------------------------------------------*
  Name:         WCM_CompareBssID

  Description:  ２つの BSSID を比較する。同期関数。

  Arguments:    a       -   比較対象の BSSID へのポインタを指定する。
                b       -   比較対象の BSSID へのポインタを指定する。

  Returns:      BOOL    -   ２つの BSSID が同じならば TRUE を返す。
                            異なる BSSID である場合は FALSE を返す。
 *---------------------------------------------------------------------------*/
BOOL WCM_CompareBssID(u8* a, u8* b)
{
    s32 i;

    for (i = 0; i < WCM_BSSID_SIZE; i++)
    {
        if (a[i] != b[i])
        {
            return FALSE;
        }
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         WCM_GetLinkLevel

  Description:  DCF 通信時のリンク強度を取得する。同期関数。

  Arguments:    None.

  Returns:      WMLinkLevel -   4段階に評価したリンク強度を返す。
 *---------------------------------------------------------------------------*/
WMLinkLevel WCM_GetLinkLevel(void)
{
    OSIntrMode  e = OS_DisableInterrupts();
    WCMWork*    w = WCMi_GetSystemWork();
    WMLinkLevel ret = WM_LINK_LEVEL_0;

    // 初期化確認
    if (w != NULL)
    {
        if (w->phase == WCM_PHASE_DCF)
        {
            ret = WcmGetLinkLevel();
        }
    }

    (void)OS_RestoreInterrupts(e);
    return ret;
}

/*---------------------------------------------------------------------------*
  Name:         WCM_IsInfrastructureNetwork

  Description:  AP サーチ結果として得られた情報がインフラストラクチャネットワーク
                のものであるかどうかを調べる。

  Arguments:    bssDesc -   調べる AP 情報 ( ネットワーク情報 )。

  Returns:      BOOL    -   インフラストラクチャネットワークである場合に TRUE を、
                            そうでない場合に FALSE を返す。
 *---------------------------------------------------------------------------*/
BOOL WCM_IsInfrastructureNetwork(WMBssDesc* bssDesc)
{
    if (bssDesc != NULL)
    {
        if ((bssDesc->capaInfo & WCM_NETWORK_CAPABILITY_KIND_MASK) == WCM_NETWORK_CAPABILITY_KIND_ESS)
        {
            return TRUE;
        }
    }

    return FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         WCM_IsAdhocNetwork

  Description:  AP サーチ結果として得られた情報がアドホックネットワークのもの
                であるかどうかを調べる。

  Arguments:    bssDesc -   調べる AP 情報 ( ネットワーク情報 )。

  Returns:      BOOL    -   アドホックネットワークである場合に TRUE を、
                            そうでない場合に FALSE を返す。
 *---------------------------------------------------------------------------*/
BOOL WCM_IsAdhocNetwork(WMBssDesc* bssDesc)
{
    if (bssDesc != NULL)
    {
        if ((bssDesc->capaInfo & WCM_NETWORK_CAPABILITY_KIND_MASK) == WCM_NETWORK_CAPABILITY_KIND_IBSS)
        {
            return TRUE;
        }
    }

    return FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         WCM_IsPrivacyNetwork

  Description:  AP サーチ結果として得られた情報が WEP セキュリティの適用された
                ネットワークのものであるかどうかを調べる。

  Arguments:    bssDesc -   調べる AP 情報 ( ネットワーク情報 )。

  Returns:      BOOL    -   WEP セキュリティの適用されたネットワークである場合に
                            TRUE を、そうでない場合に FALSE を返す。
 *---------------------------------------------------------------------------*/
BOOL WCM_IsPrivacyNetwork(WMBssDesc* bssDesc)
{
    if (bssDesc != NULL)
    {
        if (bssDesc->capaInfo & WCM_NETWORK_CAPABILITY_PRIVACY)
        {
            return TRUE;
        }
    }

    return FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         WCM_IsShortPreambleNetwork

  Description:  AP サーチ結果として得られた情報がショートプリアンブルを使用した
                ネットワークのものであるかどうかを調べる。

  Arguments:    bssDesc -   調べる AP 情報 ( ネットワーク情報 )。

  Returns:      BOOL    -   ショートプリアンブルを使用したネットワークである場合に
                            TRUE を、そうでない場合に FALSE を返す。
 *---------------------------------------------------------------------------*/
BOOL WCM_IsShortPreambleNetwork(WMBssDesc* bssDesc)
{
    if (bssDesc != NULL)
    {
        if ((bssDesc->capaInfo & WCM_NETWORK_CAPABILITY_PREAMBLE_MASK) == WCM_NETWORK_CAPABILITY_PREAMBLE_SHORT)
        {
            return TRUE;
        }
    }

    return FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         WCM_IsLongPreambleNetwork

  Description:  AP サーチ結果として得られた情報がロングプリアンブルを使用した
                ネットワークのものであるかどうかを調べる。

  Arguments:    bssDesc -   調べる AP 情報 ( ネットワーク情報 )。

  Returns:      BOOL    -   ロングプリアンブルを使用したネットワークである場合に
                            TRUE を、そうでない場合に FALSE を返す。
 *---------------------------------------------------------------------------*/
BOOL WCM_IsLongPreambleNetwork(WMBssDesc* bssDesc)
{
    if (bssDesc != NULL)
    {
        if ((bssDesc->capaInfo & WCM_NETWORK_CAPABILITY_PREAMBLE_MASK) == WCM_NETWORK_CAPABILITY_PREAMBLE_LONG)
        {
            return TRUE;
        }
    }

    return FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         WCM_IsEssidHidden

  Description:  AP サーチ結果として得られた情報が ESS-ID を隠蔽しているかどうか
                調べる。

  Arguments:    bssDesc -   調べる AP 情報 ( ネットワーク情報 )。

  Returns:      BOOL    -   ESS-ID が隠蔽されている場合に TRUE を返す。
                            有効な ESS-ID が公開されている場合には FALSE を返す。
 *---------------------------------------------------------------------------*/
BOOL WCM_IsEssidHidden(WMBssDesc* bssDesc)
{
    if (bssDesc != NULL)
    {
        s32 i;

        if (bssDesc->ssidLength == 0)
        {
            return TRUE;    // 長さ 0 の場合ステルス
        }

        for (i = 0; (i < bssDesc->ssidLength) && (i < WM_SIZE_SSID); i++)
        {
            if (bssDesc->ssid[i] != 0x00)
            {
                return FALSE;
            }
        }

        return TRUE;        // 長さぶん全て 0x00 の場合ステルス
    }

    return FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         WCMi_GetRssiAverage

  Description:  内部バッファに蓄積されている受信電波強度の履歴から、平均の
                RSSI 値を取得する。

  Arguments:    None.

  Returns:      u8      -   平均 RSSI 値を返す。履歴の平均なので、接続されて
                            いない場合などでも過去の履歴を元に有効な値を返す
                            ことに注意。
 *---------------------------------------------------------------------------*/
u8 WCMi_GetRssiAverage(void)
{
    s32 sum = 0;
    s32 i;

    if (wcmRssiIndex > WCM_RSSI_COUNT_MAX)
    {
        for (i = 0; i < WCM_RSSI_COUNT_MAX; i++)
        {
            sum += wcmRssi[i];
        }

        sum /= WCM_RSSI_COUNT_MAX;
    }
    else if (wcmRssiIndex > 0)
    {
        for (i = 0; i < wcmRssiIndex; i++)
        {
            sum += wcmRssi[i];
        }

        sum /= wcmRssiIndex;
    }

    return(u8) sum;
}

/*---------------------------------------------------------------------------*
  Name:         WcmGetLinkLevel

  Description:  内部バッファに蓄積されている受信電波強度の履歴を元にしたリンク
                レベルを取得する。

  Arguments:    None.

  Returns:      WMLinkLevel -   リンクレベルを返す。
 *---------------------------------------------------------------------------*/
static WMLinkLevel WcmGetLinkLevel(void)
{
    u8          ave = WCMi_GetRssiAverage();
    WMLinkLevel level = WM_LINK_LEVEL_0;

    // sum から level を計算
    if (ave >= WCM_RSSI_BORDER_HIGH)
    {
        level = WM_LINK_LEVEL_3;
    }
    else if (ave >= WCM_RSSI_BORDER_MIDDLE)
    {
        level = WM_LINK_LEVEL_2;
    }
    else if (ave >= WCM_RSSI_BORDER_LOW)
    {
        level = WM_LINK_LEVEL_1;
    }

    return level;
}

/*---------------------------------------------------------------------------*
  Name:         WCMi_ShelterRssi

  Description:  DCF データを受信した際に、受信電波強度の RSSI 値を内部バッファ
                に蓄積する。

  Arguments:    rssi    -   受信電波強度の RSSI 値を指定する。

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WCMi_ShelterRssi(u8 rssi)
{
    u8  raw;

    if (rssi & 0x02)
    {
        raw = (u8) (rssi >> 2);
    }
    else
    {
        raw = (u8) (25 + (rssi >> 2));
    }

    wcmRssi[wcmRssiIndex % WCM_RSSI_COUNT_MAX] = raw;
    if (wcmRssiIndex >= WCM_RSSI_COUNT_MAX)
    {
        wcmRssiIndex = (u8) (((1 + wcmRssiIndex) % WCM_RSSI_COUNT_MAX) + WCM_RSSI_COUNT_MAX);
    }
    else
    {
        wcmRssiIndex++;
    }
}

/*---------------------------------------------------------------------------*
    End of file
 *---------------------------------------------------------------------------*/
