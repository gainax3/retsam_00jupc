/*---------------------------------------------------------------------------*
  Project:  NitroDWC - base - init
  File:     dwc_init.c

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc_init.c,v $
  Revision 1.11  2007/02/16 08:24:00  takayama
  DWC_GetAuthenticatedUserId()を追加。

  Revision 1.10  2007/02/13 07:42:45  nakata
  DWCisconsoleidreadableフラグを消去。

  Revision 1.9  2007/02/13 02:52:32  nakata
  DWC_GetConsoleId関数のための本体ID読み出し可能フラグを追加。

  Revision 1.8  2005/12/15 05:35:16  arakit
  デバッグ用のNVRAMクリア関数DWC_Debug_ClearConsoleWiFiInfo()を削除し、
  DWC_Init()の全ての戻り値を試せるようにNVRAMを破壊するデバッグ関数
  DWC_Debug_DWCInitError()を追加した。

  Revision 1.7  2005/11/01 10:38:48  arakit
  enumの綴りを修正した。

  Revision 1.6  2005/10/06 09:42:37  sasakit
  versionを埋め込むようにした。

  Revision 1.5  2005/09/21 06:56:02  sasakit
  デバッグ用の本体Wi-Fiユーザ情報を削除する関数を追加。

  Revision 1.4  2005/09/02 10:14:23  sasakit
  OS_Init()他のNitroSDK初期化関数を呼ぶのをやめて、リファレンスに明記するようにした。

  Revision 1.3  2005/08/31 09:47:01  sasakit
  dwc_initの戻り値を設定した。

  Revision 1.2  2005/08/31 04:12:30  sasakit
  util_wifiidtool.hの移動に対応。

  Revision 1.1  2005/08/31 02:29:44  sasakit
  初期化用のコードがoverlayで分けられるように、モジュールを分けた。

  Revision 1.4  2005/08/23 05:40:05  nakata
  Fixed the problem that is related to BM initialize.
  Before : Pass 32 BIT  alignmented memory space to BM as work memory.
  After  : Pass 32 BYTE alignmented memory space to BM as work memory.

  Revision 1.3  2005/08/22 12:06:57  sasakit
  WiFiIDがないときに作成するように戻した。

  Revision 1.2  2005/08/20 06:59:56  sasakit
  ヘッダインクルードガードの書式を統一した。
  bm/dwc_init.h -> bm/dwc_bm_init.hに変更
  ヘッダにCopyrightをつけた。
  構造体の名前空間をできるだけルールにちかづけた。
  util_wifiidtool.hとdwc_backup.hの関係を若干修正。

  Revision 1.1  2005/08/19 12:12:10  sasakit
  リリースに向けて修正

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>

#include <bm/dwc_bm_init.h>
#include <bm/util_wifiidtool.h>
#include <base/dwc_report.h>
#include <base/dwc_init.h>

#include <dwc_version.h>

#include <nitro/version_begin.h>
static char id_string [] = SDK_MIDDLEWARE_STRING("NINTENDO", DWC_VERSION_STRING);
#include <nitro/version_end.h>

extern BOOL DWCi_AUTH_MakeWiFiID( void* work );
int
DWC_Init( void* work )
{
    int ret;
    BOOL created = FALSE;

    SDK_USING_MIDDLEWARE(id_string);

    SDK_ASSERTMSG( ( (u32)work & 0x1f ) == 0, "work must be 32bytes alignment - %p\n", work );

    ret = DWC_BM_Init( work );
    
    if ( DWC_Auth_CheckWiFiIDNeedCreate() )
    {
        DWC_Printf( DWC_REPORTFLAG_INFO, "Create UserID\n" );
        DWCi_AUTH_MakeWiFiID( work );

        created = TRUE;
    }
    else
    {
        DWC_Printf( DWC_REPORTFLAG_INFO, "Read UserID from DS\n" );
    }

    if ( ret < 0 )
    {
        // NVRAMの内容が破壊されている。
        if ( created )
        {
            return DWC_INIT_RESULT_DESTROY_USERID;
        }
        else
        {
            return DWC_INIT_RESULT_DESTROY_OTHER_SETTING;
        }
    }
    else if ( created )
    {
        // NVRAMは問題ないけど、なぜか作られました。
        return DWC_INIT_RESULT_CREATE_USERID;
    }
    
    return DWC_INIT_RESULT_NOERROR;
}

/*---------------------------------------------------------------------------*
  Name:         DWC_GetAuthenticatedUserId

  Description:  DS本体の認証済みユーザIDを取得する

  Arguments:    なし

  Returns:      認証済みユーザID（認証済みでない場合は 0 )
 *---------------------------------------------------------------------------*/
u64 DWC_GetAuthenticatedUserId( void )
{
    DWCAuthWiFiId wifiid;
    
    DWC_Auth_GetId( &wifiid );
    return wifiid.uId;
}

extern BOOL  DWCi_BACKUPlInit(void* work);
extern BOOL  DWCi_BACKUPlRead(void* mem);
extern BOOL  DWCi_BACKUPlWritePage(const void* data, const BOOL* page, void* work);
static    char s_work[0x400] ATTRIBUTE_ALIGN(32);

// NVRAMの特定の箇所を破壊してDWC_Init()のエラー返り値を試すためのデバッグ関数
void DWC_Debug_DWCInitError( void* work, int dwc_init_error )
{
    BOOL needCrc = FALSE;
    int  crcPage;

    if ( dwc_init_error == DWC_INIT_RESULT_NOERROR ) return;

    DWCi_BACKUPlInit(work);  // NVRAMアクセス初期化

    if ( dwc_init_error == DWC_INIT_RESULT_CREATE_USERID )
    {
        DWCi_BACKUPlRead( s_work );  // NVRAM読み込み

        // ページ0のWiFi情報のメンバnotAttestedId, attestedUserIdのみクリア
        MI_CpuClear8( &s_work[0xf0], 10 );
        s_work[0xf0+0x0a] &= ~0x3f;

        needCrc = TRUE;
        crcPage = 0;     // ページ0のデータを書き換える
    }
    else if ( dwc_init_error == DWC_INIT_RESULT_DESTROY_USERID )
    {
        DWCi_BACKUPlRead( s_work );  // NVRAM読み込み

        // ページ1のWiFi情報のメンバnotAttestedId, attestedUserIdのみクリア
        MI_CpuClear8( &s_work[0x100+0xf0], 10 );
        s_work[0x100+0xf0+0x0a] &= ~0x3f;

        // この後ページ0のデータを破壊するので、ページ0にコピーされることになる
        // ページ1のap.stateに、ページ0にとって正しい値をセットする
        s_work[0x100+0xef] = 1;

        needCrc = TRUE;
        crcPage = 1;     // ページ1のデータを書き換える
    }

    // WiFi情報を書き換えた場合は、CRCの再計算をしてNVRAMを書き換える
    if ( needCrc )
    {
        u16 hash;
        BOOL page[4] = { FALSE, FALSE, FALSE, FALSE };
        MATHCRC16Table crc16_tbl;
        
        // CRC 初期化
    	MATH_CRC16InitTable( &crc16_tbl );

        // CRC 再計算
  		hash = MATH_CalcCRC16( &crc16_tbl, (u16 *)&s_work[crcPage*0x100], 0xFE );
        *(u16 *)(&s_work[crcPage*0x100+0xFE]) = hash;

        // 指定のページのみNVRAM書き換え
        page[crcPage] = TRUE;
        DWCi_BACKUPlWritePage( s_work, page, work );
    }

    // DWC_BM_Init()をDWC_INIT_OK以外で終了させるためにページ0のみクリアする
    if ( dwc_init_error == DWC_INIT_RESULT_DESTROY_USERID ||
         dwc_init_error == DWC_INIT_RESULT_DESTROY_OTHER_SETTING )
    {
        BOOL page[4] = { TRUE, FALSE, FALSE, FALSE };
        
        MI_CpuClear8( s_work, sizeof(s_work) );
        DWCi_BACKUPlWritePage( s_work, page, work );
    }
}
