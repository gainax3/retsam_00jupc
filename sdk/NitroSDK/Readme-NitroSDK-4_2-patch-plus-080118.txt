■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
■                                                                          ■
■  Readme-NitroSDK-4_2-patch-plus-080118.txt                               ■
■                                                                          ■
■  Plus Patch for NITRO-SDK 4.2                                            ■
■                                                                          ■
■  Jan. 18, 2008                                                           ■
■                                                                          ■
■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■


【 はじめに 】

    このパッチは、NITRO-SDK 4.2 のリリース以降に発見された不具合を修正する
    ためのものです。インストールするためには、同梱されているファイルを
    NITRO-SDK 4.2 (071210)がインストールされているディレクトリに上書きコピー
    してください。

【 注意 】

　　　NitroSDK のライブラリをビルドした事がある場合、本パッチを当てただけだと
　　キャッシュの影響でプログラムをビルドするときにエラーが発生する可能性が
　　あります。
　　　もしビルド時にエラーが発生した場合は、NitroSDK のルートディレクトリで
　　「make clobber」を実行することでキャッシュが削除されます。
　　その後、同じ NitroSDK のルートディレクトリで「make」することで、
　　この症状は改善されます。

【 修正内容 】 
    
    4_2-patch-plus-080118 で修正された内容
    --------------------------------------------------------------------------

    ・(OS) OS ライブラリのリファレンスに含まれるスレッド概要の更新

    OS ライブラリのリファレンス中のスレッド概要に、
    「スレッド状態を外部から変化させる場合の注意」を追記しました。

    ・(TP) TP_WaitRawResult、TP_WaitCalibratedResult 関数の修正

    TP_WaitRawResult、TP_WaitCalibratedResult 関数はサンプリング動作の完了を待って
    データを取得する関数ですが、既にサンプリング動作が完了している場合に限って
    割込みを禁止した状態での呼び出しをサポートしています。
    しかしデバッグビルドでは意図通り動作しないという不具合がありました。
    この不具合を修正しました。

    ・(WM) WM_EnableForListening 関数の blink 引数の解釈の修正

    WM_EnableForListening 関数の blink 引数の真偽値を逆に解釈する不具合が
    ありましたので、これを修正しました。

    ・(WM) 送信サイズチェックの不具合修正

    「Note 50-16: (WM) 子機送信容量の制限の緩和」の変更に起因して、
    MP 通信で親機送信容量を510バイト以上に設定する、もしくは
    子機送信容量を512バイトに設定した場合に、場合によってデータを
    送信できなくなる不具合が混入してしまっていました。
    これを修正しました。


【 ファイルリスト 】

■ソースファイル
	/NitroSDK/build/libraries/spi/ARM9/src/tp.c
	/NitroSDK/build/libraries/wm/ARM9/src/wm_standard.c

■ヘッダファイル
	/NitroSDK/include/nitro/version.h

■ライブラリファイル
	/NitroSDK/lib/ARM9-TS/Debug/crt0.o
	/NitroSDK/lib/ARM9-TS/Debug/libspi.a
	/NitroSDK/lib/ARM9-TS/Debug/libspi.thumb.a
	/NitroSDK/lib/ARM9-TS/Debug/libspi_TS0.a
	/NitroSDK/lib/ARM9-TS/Debug/libspi_TS0.thumb.a
	/NitroSDK/lib/ARM9-TS/Debug/libspi_TS100.a
	/NitroSDK/lib/ARM9-TS/Debug/libspi_TS100.thumb.a
	/NitroSDK/lib/ARM9-TS/Debug/libspi_TS200.a
	/NitroSDK/lib/ARM9-TS/Debug/libspi_TS200.thumb.a
	/NitroSDK/lib/ARM9-TS/Debug/libwm.a
	/NitroSDK/lib/ARM9-TS/Debug/libwm.thumb.a
	/NitroSDK/lib/ARM9-TS/Release/crt0.o
	/NitroSDK/lib/ARM9-TS/Release/libspi.a
	/NitroSDK/lib/ARM9-TS/Release/libspi.thumb.a
	/NitroSDK/lib/ARM9-TS/Release/libspi_TS0.a
	/NitroSDK/lib/ARM9-TS/Release/libspi_TS0.thumb.a
	/NitroSDK/lib/ARM9-TS/Release/libspi_TS100.a
	/NitroSDK/lib/ARM9-TS/Release/libspi_TS100.thumb.a
	/NitroSDK/lib/ARM9-TS/Release/libspi_TS200.a
	/NitroSDK/lib/ARM9-TS/Release/libspi_TS200.thumb.a
	/NitroSDK/lib/ARM9-TS/Release/libwm.a
	/NitroSDK/lib/ARM9-TS/Release/libwm.thumb.a
	/NitroSDK/lib/ARM9-TS/Rom/crt0.o
	/NitroSDK/lib/ARM9-TS/Rom/libwm.a
	/NitroSDK/lib/ARM9-TS/Rom/libwm.thumb.a

■コンポーネントファイル
	/NitroSDK/components/ichneumon/ARM7-TS/Debug/ichneumon_sub.nef
	/NitroSDK/components/ichneumon/ARM7-TS/Debug/ichneumon_sub_TS0.nef
	/NitroSDK/components/ichneumon/ARM7-TS/Debug/ichneumon_sub_TS100.nef
	/NitroSDK/components/ichneumon/ARM7-TS/Debug/ichneumon_sub_TS200.nef
	/NitroSDK/components/ichneumon/ARM7-TS/Release/ichneumon_sub.nef
	/NitroSDK/components/ichneumon/ARM7-TS/Release/ichneumon_sub_TS0.nef
	/NitroSDK/components/ichneumon/ARM7-TS/Release/ichneumon_sub_TS100.nef
	/NitroSDK/components/ichneumon/ARM7-TS/Release/ichneumon_sub_TS200.nef
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Debug/ichneumon_sub.nef
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Debug/ichneumon_sub_TS0.nef
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Debug/ichneumon_sub_TS100.nef
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Debug/ichneumon_sub_TS200.nef
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Release/ichneumon_sub.nef
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Release/ichneumon_sub_TS0.nef
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Release/ichneumon_sub_TS100.nef
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Release/ichneumon_sub_TS200.nef
	/NitroSDK/components/mongoose/ARM7-TS/Debug/mongoose_sub.nef
	/NitroSDK/components/mongoose/ARM7-TS/Debug/mongoose_sub_TS0.nef
	/NitroSDK/components/mongoose/ARM7-TS/Debug/mongoose_sub_TS100.nef
	/NitroSDK/components/mongoose/ARM7-TS/Debug/mongoose_sub_TS200.nef
	/NitroSDK/components/mongoose/ARM7-TS/Release/mongoose_sub.nef
	/NitroSDK/components/mongoose/ARM7-TS/Release/mongoose_sub_TS0.nef
	/NitroSDK/components/mongoose/ARM7-TS/Release/mongoose_sub_TS100.nef
	/NitroSDK/components/mongoose/ARM7-TS/Release/mongoose_sub_TS200.nef
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Debug/mongoose_sub.nef
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Debug/mongoose_sub_TS0.nef
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Debug/mongoose_sub_TS100.nef
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Debug/mongoose_sub_TS200.nef
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Release/mongoose_sub.nef
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Release/mongoose_sub_TS0.nef
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Release/mongoose_sub_TS100.nef
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Release/mongoose_sub_TS200.nef
	/NitroSDK/components/ichneumon/ARM7-TS/Debug/ichneumon_sub.sbin
	/NitroSDK/components/ichneumon/ARM7-TS/Debug/ichneumon_sub_defs.sbin
	/NitroSDK/components/ichneumon/ARM7-TS/Debug/ichneumon_sub_TS0.sbin
	/NitroSDK/components/ichneumon/ARM7-TS/Debug/ichneumon_sub_TS0_defs.sbin
	/NitroSDK/components/ichneumon/ARM7-TS/Debug/ichneumon_sub_TS100.sbin
	/NitroSDK/components/ichneumon/ARM7-TS/Debug/ichneumon_sub_TS100_defs.sbin
	/NitroSDK/components/ichneumon/ARM7-TS/Debug/ichneumon_sub_TS200.sbin
	/NitroSDK/components/ichneumon/ARM7-TS/Debug/ichneumon_sub_TS200_defs.sbin
	/NitroSDK/components/ichneumon/ARM7-TS/Release/ichneumon_sub.sbin
	/NitroSDK/components/ichneumon/ARM7-TS/Release/ichneumon_sub_defs.sbin
	/NitroSDK/components/ichneumon/ARM7-TS/Release/ichneumon_sub_TS0.sbin
	/NitroSDK/components/ichneumon/ARM7-TS/Release/ichneumon_sub_TS0_defs.sbin
	/NitroSDK/components/ichneumon/ARM7-TS/Release/ichneumon_sub_TS100.sbin
	/NitroSDK/components/ichneumon/ARM7-TS/Release/ichneumon_sub_TS100_defs.sbin
	/NitroSDK/components/ichneumon/ARM7-TS/Release/ichneumon_sub_TS200.sbin
	/NitroSDK/components/ichneumon/ARM7-TS/Release/ichneumon_sub_TS200_defs.sbin
	/NitroSDK/components/ichneumon/ARM7-TS/Rom/ichneumon_sub.sbin
	/NitroSDK/components/ichneumon/ARM7-TS/Rom/ichneumon_sub_TS0.sbin
	/NitroSDK/components/ichneumon/ARM7-TS/Rom/ichneumon_sub_TS100.sbin
	/NitroSDK/components/ichneumon/ARM7-TS/Rom/ichneumon_sub_TS200.sbin
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Debug/ichneumon_sub.sbin
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Debug/ichneumon_sub_defs.sbin
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Debug/ichneumon_sub_TS0.sbin
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Debug/ichneumon_sub_TS0_defs.sbin
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Debug/ichneumon_sub_TS100.sbin
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Debug/ichneumon_sub_TS100_defs.sbin
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Debug/ichneumon_sub_TS200.sbin
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Debug/ichneumon_sub_TS200_defs.sbin
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Release/ichneumon_sub.sbin
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Release/ichneumon_sub_defs.sbin
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Release/ichneumon_sub_TS0.sbin
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Release/ichneumon_sub_TS0_defs.sbin
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Release/ichneumon_sub_TS100.sbin
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Release/ichneumon_sub_TS100_defs.sbin
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Release/ichneumon_sub_TS200.sbin
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Release/ichneumon_sub_TS200_defs.sbin
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Rom/ichneumon_sub.sbin
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Rom/ichneumon_sub_TS0.sbin
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Rom/ichneumon_sub_TS100.sbin
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Rom/ichneumon_sub_TS200.sbin
	/NitroSDK/components/mongoose/ARM7-TS/Debug/mongoose_sub.sbin
	/NitroSDK/components/mongoose/ARM7-TS/Debug/mongoose_sub_defs.sbin
	/NitroSDK/components/mongoose/ARM7-TS/Debug/mongoose_sub_TS0.sbin
	/NitroSDK/components/mongoose/ARM7-TS/Debug/mongoose_sub_TS0_defs.sbin
	/NitroSDK/components/mongoose/ARM7-TS/Debug/mongoose_sub_TS100.sbin
	/NitroSDK/components/mongoose/ARM7-TS/Debug/mongoose_sub_TS100_defs.sbin
	/NitroSDK/components/mongoose/ARM7-TS/Debug/mongoose_sub_TS200.sbin
	/NitroSDK/components/mongoose/ARM7-TS/Debug/mongoose_sub_TS200_defs.sbin
	/NitroSDK/components/mongoose/ARM7-TS/Release/mongoose_sub.sbin
	/NitroSDK/components/mongoose/ARM7-TS/Release/mongoose_sub_defs.sbin
	/NitroSDK/components/mongoose/ARM7-TS/Release/mongoose_sub_TS0.sbin
	/NitroSDK/components/mongoose/ARM7-TS/Release/mongoose_sub_TS0_defs.sbin
	/NitroSDK/components/mongoose/ARM7-TS/Release/mongoose_sub_TS100.sbin
	/NitroSDK/components/mongoose/ARM7-TS/Release/mongoose_sub_TS100_defs.sbin
	/NitroSDK/components/mongoose/ARM7-TS/Release/mongoose_sub_TS200.sbin
	/NitroSDK/components/mongoose/ARM7-TS/Release/mongoose_sub_TS200_defs.sbin
	/NitroSDK/components/mongoose/ARM7-TS/Rom/mongoose_sub.sbin
	/NitroSDK/components/mongoose/ARM7-TS/Rom/mongoose_sub_TS0.sbin
	/NitroSDK/components/mongoose/ARM7-TS/Rom/mongoose_sub_TS100.sbin
	/NitroSDK/components/mongoose/ARM7-TS/Rom/mongoose_sub_TS200.sbin
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Debug/mongoose_sub.sbin
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Debug/mongoose_sub_defs.sbin
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Debug/mongoose_sub_TS0.sbin
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Debug/mongoose_sub_TS0_defs.sbin
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Debug/mongoose_sub_TS100.sbin
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Debug/mongoose_sub_TS100_defs.sbin
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Debug/mongoose_sub_TS200.sbin
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Debug/mongoose_sub_TS200_defs.sbin
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Release/mongoose_sub.sbin
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Release/mongoose_sub_defs.sbin
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Release/mongoose_sub_TS0.sbin
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Release/mongoose_sub_TS0_defs.sbin
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Release/mongoose_sub_TS100.sbin
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Release/mongoose_sub_TS100_defs.sbin
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Release/mongoose_sub_TS200.sbin
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Release/mongoose_sub_TS200_defs.sbin
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Rom/mongoose_sub.sbin
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Rom/mongoose_sub_TS0.sbin
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Rom/mongoose_sub_TS100.sbin
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Rom/mongoose_sub_TS200.sbin

■ドキュメント・その他
	/NitroSDK/man/ja_JP/os/thread/about_Thread.html

以上
