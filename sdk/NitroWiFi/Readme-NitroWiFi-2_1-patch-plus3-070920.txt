■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
■                                                                          ■
■  Readme-NitroWiFi-2_1-patch-plus3-070920.txt                             ■
■                                                                          ■
■  Plus Patch for NITRO-SDK Wi-Fi ライブラリ 2.1                           ■
■                                                                          ■
■  Sep. 20, 2007                                                           ■
■                                                                          ■
■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■


【 はじめに 】

    このパッチは、NITRO-SDK Wi-Fi ライブラリ 2.1 のリリース以降に
    発見された不具合を修正するためのものです。インストールするためには、
    同梱されているファイルを NITRO-SDK Wi-Fi ライブラリ 2.1 (070710)が
    インストールされているディレクトリに上書きコピーしてください。

    ※ NITRO-SDK Wi-Fi ライブラリ 2.1 をご使用の場合、
       必ずこのplusパッチを適用して下さい。

【 注意 】

　　　NITRO-SDK Wi-Fi ライブラリ 2.1 のライブラリをビルドした事がある場合、
    本パッチを当てただけだとキャッシュの影響でプログラムをビルドするときに
    エラーが発生する可能性があります。
　　　もしビルド時にエラーが発生した場合は、NitroWiFiのルートディレクトリで
　　「make clobber」を実行することでキャッシュが削除されます。
　　その後、同じNitroWiFiのルートディレクトリで「make」することで、
　　この症状は改善されます。

【 修正内容 】 
    
    NitroWiFi-2_1-patch-plus3-070920 で修正された内容
    --------------------------------------------------------------------------
    
    ・(CPS/WCM) TCP 送信におけるデータ化けの修正

    NitroWiFi には大きなサイズの TCP データを送信した場合に、
    データが化けることがあるという不具合があり、これを修正しました。


    NitroWiFi-2_1-patch-plus2-070912 で修正された内容
    --------------------------------------------------------------------------
    
    ・(CPS) 接続時にタイムアウトしなくなる不具合の修正

    接続時に RST パケットを受信した場合に、接続がタイムアウトしなくなる
    という不具合がありましたので、これを修正しました。


    NitroWiFi-2_1-patch-plus-070829-1321 で修正された内容
    --------------------------------------------------------------------------
    
    ・(CPS/SSL) アクセス例外の修正

    通信環境により、稀にアクセス例外が発生することがありましたので、
    これを修正しました。
    この修正には、Warning:mac mismatch と表示されて
    データアクセス例外が発生する問題の修正も含まれます。


【 ファイルリスト 】

    ■ソースファイル
	/NitroWiFi/build/libraries/wcm/src/cpsif.c

    ■ヘッダファイル
	/NitroWiFi/build/libraries/wcm/include/wcm_cpsif.h
	/NitroWiFi/include/nitroWiFi/version.h

    ■ライブラリファイル
	/NitroWiFi/lib/ARM9-TS/Debug/libcps.a
	/NitroWiFi/lib/ARM9-TS/Debug/libcps.thumb.a
	/NitroWiFi/lib/ARM9-TS/Debug/libsoc.a
	/NitroWiFi/lib/ARM9-TS/Debug/libsoc.thumb.a
	/NitroWiFi/lib/ARM9-TS/Debug/libssl.a
	/NitroWiFi/lib/ARM9-TS/Debug/libssl.thumb.a
	/NitroWiFi/lib/ARM9-TS/Debug/libwcm.a
	/NitroWiFi/lib/ARM9-TS/Debug/libwcm.thumb.a
	/NitroWiFi/lib/ARM9-TS/Release/libcps.a
	/NitroWiFi/lib/ARM9-TS/Release/libcps.thumb.a
	/NitroWiFi/lib/ARM9-TS/Release/libsoc.a
	/NitroWiFi/lib/ARM9-TS/Release/libsoc.thumb.a
	/NitroWiFi/lib/ARM9-TS/Release/libssl.a
	/NitroWiFi/lib/ARM9-TS/Release/libssl.thumb.a
	/NitroWiFi/lib/ARM9-TS/Release/libwcm.a
	/NitroWiFi/lib/ARM9-TS/Release/libwcm.thumb.a
	/NitroWiFi/lib/ARM9-TS/Rom/libcps.a
	/NitroWiFi/lib/ARM9-TS/Rom/libcps.thumb.a
	/NitroWiFi/lib/ARM9-TS/Rom/libsoc.a
	/NitroWiFi/lib/ARM9-TS/Rom/libsoc.thumb.a
	/NitroWiFi/lib/ARM9-TS/Rom/libssl.a
	/NitroWiFi/lib/ARM9-TS/Rom/libssl.thumb.a
	/NitroWiFi/lib/ARM9-TS/Rom/libwcm.a
	/NitroWiFi/lib/ARM9-TS/Rom/libwcm.thumb.a

以上
