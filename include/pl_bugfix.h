//==============================================================================
/**
 * @file	pl_bugfix.h
 * @brief	プラチナ：バグ対処用定義
 * @author	matsuda
 * @date	2008.07.06(日)
 *
 * 7月7日のノータッチデバッグ以降で修正されたバグの有効・無効定義
 * 定義の有効・無効でなく定義値が０の場合は無効、１の場合は有効として定義すること
 */
//==============================================================================
#ifndef	__PL_BUGFIX_H__
#define	__PL_BUGFIX_H__


//--------------------------------------------------------------
//	バトル		PL_B0000_080707_FIX		BTS番号：修正した日付
//	シナリオ	PL_S0000_080707_FIX		BTS番号：修正した日付
//	通信		PL_T0000_080707_FIX		BTS番号：修正した日付
//	社内		PL_G0000_080707_FIX		BTS番号：修正した日付
//--------------------------------------------------------------

/* 不思議な贈り物で余計なメッセージ待ちがある(3sec) */
// MatchComment: enable this bugfix
#define	PL_T0852_080709_FIX		( 1 )	///< comm_mystery_gift.c


/*
 * T0849	リザルト画面でタッチペン型の各プレイヤーの表示がちらつく
 * 場所	src/application/wifi_lobby/minigame_tool.c
 */
#define PL_T0849_080709_FIX	(1)


/*
 * T0834	タッチトイ操作が他プレイヤーのタッチトイ操作に影響を与える
 * 場所	src/application/wifi_lobby/wflby_ev_def.c
 */
#define PL_T0834_080709_FIX	(0)


/*
 * S0789	初期ポケモンをもらう直前にセーブを行い、一度電源を落とした後にも
 *			らおうとするとその際に表示されるテキストが早送りできません
 * 場所	src/demo/ev_poke_select.c
 */
#define PL_S0789_080710_FIX	(1) // MatchComment: enable this bugfix


/*
 * T0855	DLバトルタワーで成績が反映されない
 * 場所	src/frontier/fssc_tower.c
 */
#define PL_T0855_080710_FIX	(0)

/*
 * T0856	参加人数とRingBuffの人数が合致していない場合の対処としてRing書き込みにNULL検査を追加
 * 場所	src/communication/comm_ring_buff.c
 */
#define PL_T0856_080710_FIX  (0)

/*
 * G0197 	DPからPLに「こうかん」を申し込むととまる
 *　BTS_T0856と、BTS_G0197は、
 *	受信バッファを DWC_SetRecvBufferでＤＷＣライブラリに設定するタイミングの
 *	問題で発生するバグです。
 *
 *	メモリが不足しているため、 DWCMatchedSCCallbackでマッチングが完了したことを確認した後に、
 *	受信バッファのメモリを確保し、ライブラリに設定するように処理を組んであります。
 *	この不具合が発生するタイミングでは、ＤＷＣライブラリがデータ受信中の状態で、バッファを設定しようとして、
 *	バッファを設定できず、ＤＷＣライブラリが以前の受信バッファのまま、その後も動作してしまいます。
 *	また、マッチング完了までにもライブラリ側ではコマンドを受信しているため、その部分でもメモリ破壊がおきている可能性があります。
 *
 *	この問題を解決するには、マッチングを開始する前に、受信バッファを設定するように修正しました。
 *	その為取得するメモリサイズが0x1000byte増えています。
 *
 * 場所	src/wifi/dwc_rap.c
 */
#define PL_G0197_080710_FIX	(0)

/*
 * T0857	DPの募集に対し、申し込みを行った時にアイコンは募集内容のものが表示されているのに
 *			ボイスチャットが始まってしまう
 *
 * T0858	DPとの接続時、募集側（PL）が特定のタイミングでボイスチャットの
 *			ON/OFFを切り替えると実際の設定と異なる形での挙動になってしまう
 *
 *	場所 src/application/wifi_p2pmatch/wifi_p2pmatch.c
 */
#define PL_T0857_080711_FIX	(0)


/*
 * S0793	あまいみつ購入メッセージで、余計な送りカーソルが表示されている
 * 場所	src/fielddata/script/d13r0101.ev
 */
#define PL_S0793_080711_FIX	(0)

/*
 * S0792	
 * カンナギタウンから210ばんどうろに移動した直後に「きりばらい」を
 * 使うとにんじゃごっこケンキチの隠れている場所のグラフィックがなくなってしまう
 * 場所 src/field/fldeff_hide.c
 */
#define PL_S0792_080711_FIX (0)

/*
 * G0198
 * 図鑑に登録されていないタマゴを持って「やぶれたせかい」に入ると、孵化していなにも関わらず
 * 図鑑登録されてしまう。
 * 場所 src/field/scrcmd.c
 */
#define PL_G0198_080711_FIX (1)


/*
 * S0791	アクセサリーいれ入手までに、背景をもらえてしまう
 * 場所	src/fielddata/script/c01r0601.ev
 */
#define PL_S0791_080712_FIX	(1)


/*
 * S0798	揺れているミツの木を調べエンカウントしようとしたところでとまるこ
とがある
 *			とまるのは、天気の分割初期化中に破棄処理が発生するため
 * 
 * 場所 src/field/weather_sys.c
 */
#define PL_S0798_080712_FIX (1)


/*
 * T0861	登録確認テキストをタッチ操作でしか早送りできない
 * 
 * 場所 src/application/wifi_note/pl_wifi_note.c
 */
#define PL_T0861_080712_FIX (0)


/*
 * T0862	話しかけた相手のプロフィールが表示されないことがある
 * 
 * 場所 src/application/wifi_lobby/wflby_room.c
 */
#define PL_T0862_080712_FIX (1) // MatchComment: enable this bugfix


/*
 * T0864	トレーナーランキングに出入りした後、
 *			ジオネットに入ると、メッセージを早送りできない
 * 
 * 場所 src/application/wifi_earth/wifi_earth.c
 */
#define PL_T0864_080714_FIX (1) // MatchComment: enable this bugfix



/*
 * S0800	一定の行動を行った後、四天王と戦うとカットイン時に自キャラ中心に
映していたカメラが下方向に移動してしまう事があった
 * 
 * 場所 src/field/encount_effect.c
 */
#define PL_S0800_080714_FIX (1) // MatchComment: enable this bugfix


/*
 * T0866	取り込み中でキャンセルされた相手に再度話しかけて止まり
 * 
 * 場所 src/application/wifi_lobby/wflby_ev_talk.c
 */
#define PL_T0866_080715_FIX (1)

/*
 * T0867	自キャラと他のプレイヤーが重なった状態で「おしゃべり」が行なわれ
る
 * 
 * 場所 src/application/wifi_lobby/wflby_ev_def.c
 */
#define PL_T0867_080716_FIX (0)

/*
 * T0394	いかりを連続で選択していないときにいかり解除するときに他の状態を解除してしまう
 *			戦闘録画に影響してしまうので、プラチナで反映されない場合は、ずっと反映しないでください
 * 
 * 場所 src/battle/server.c
 */
#define PL_T0394_080716_FIX (0)

/*
 * S0802	マナフィーイベントで図鑑に登録されているのに、図鑑に登録されたメッセージが表示される
 * 
 * 場所 src/fielddata/script/r212ar0103.ev
 *		src/fielddata/script/scr_seq_def.h
 *		src/field/scrcmd.c
 *		src/field/scr_field.c
 *		src/field/scr_field.h
 */
#define PL_S0802_080716_FIX (0)

/*
 * G0200	他人のプロフィールを表示しようとしたフレーム内で、
その人の退室コールバックが呼ばれると、ＮＵＬＬ参照でとまる。 
 * 
 * 場所 src/application/wifi_lobby/wflby_room.c
 */
#define PL_G0200_080716_FIX (0)


/*
 * T0868	交換したかを受信する処理で、親機が子機の交換したかフラグをチェックしていないため、
 *			手放すワーク、受け取るワークが子機の値で上書きされてしまう
 * 
 * 場所 src/frontier/factory/factory.c
 */
#define PL_T0868_080717_FIX (1)


#endif	//__PL_BUGFIX_H__
