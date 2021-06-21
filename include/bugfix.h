//=============================================================================
/**
 * @file	bugfix.h
 * @date	2006.08.15
 * @brief	バグ対処用定義
 *
 * 定義の有効・無効でなく定義値が０の場合は無効、１の場合は有効として定義すること
 */
//=============================================================================

#ifndef	__BUGFIX_H__
#define	__BUGFIX_H__

/* タイプ変化する技が特性に引き寄せられない不具合 */
#define	B1366_060815_FIX	(1)		//1にすると修正が有効
									//修正ソース：sever_tool.c

/* マジックガード＋とんぼ返り→シャボのみへの不具合 */
#define	B1368_060815_FIX	(1)		//1にすると修正が有効
									//修正ソース：sever_tool.c

/* 複数選択技を受けた時のオウムがえしの挙動の不具合 */
#define	B1370_060815_FIX	(1)		//1にすると修正が有効
									//修正ソース：sever.c

/* さわぐの効果が消えない */
#define	B1371_060815_FIX	(1)		//1にすると修正が有効
									//修正ソース：sever_tool.c
									//
/* タッグバトルで、相手トレーナーが「かいふくのくすり」をつかっても効果がない */
#define	B1372_060816_FIX	(1)		//1にすると修正が有効
									//修正ソース：fight_tool.c

/* 装備アイテム「くろいてっきゅう」が、まきびし系、ありじごくの効果をうけない */
#define	B1373_060816_FIX	(1)		//1にすると修正が有効
									//修正ソース：server_tool.c tr_ai.c src/battle/skill/sub_099.s

/* トリックルーム中に天候回復系の技を使用すると回復量が1/4になる不具合 */
#define	B1374_060817_FIX	(1)		//1にすると修正が有効
									//修正ソース：skill_intp.c
				
/* ねむり、こんらんのターン数が１多い不具合 */
#define	B1375_060817_FIX	(1)		//1にすると修正が有効
									//修正ソース：sub_018.s sub_037.s
				
/* かいふくふうじで、吸い取る系を防いだ後、ブリンクと効果SEが鳴らない不具合 */
#define	B1376_060818_FIX	(1)		//1にすると修正が有効
									//修正ソース：battle_server.h

/* 複数選択技とマジックコートの挙動の不具合 */
#define	GB001_060815_FIX	(1)		//1にすると修正が有効
									//修正ソース：sever_tool.c

/* 気絶後のポケモンのテクスチャー2の挙動の不具合 */
#define	GB002_060815_FIX	(1)		//1にすると修正が有効
									//修正ソース：sever_tool.c

/* 気絶後のポケモンのメトロノームの挙動の不具合 */
#define	GB003_060815_FIX	(1)		//1にすると修正が有効
									//修正ソース：sever_tool.c

/* タウンマップ　サブ画面の説明文チラつき不具合解消 iwasawa*/
#define	S2410_060815_FIX	(1)		//1にすると修正が有効
									//修正ソース：tmap_nrm.c

/* 違う年の同じ日に、ゲーム開始時に冒険ノートが表示されない不具合 */
#define	S2423_060817_FIX	(1)		//1にすると修正が有効
									//修正ソース：fnote_mem.c

/* 通信エラー時に冒険ノートの開始データがセットされない不具合 */
#define	S2424_060818_FIX	(1)		//1にすると修正が有効
									//修正ソース：ev_mapchange.c

/* ボックス画面：「どうぐせいり」でどうぐアイコンが化けてしまう不具合の修正 */
#define	S2426_060818_FIX	(1)		//1にすると修正が有効
									//修正ソース：boxv_itemicon.c

/* 図鑑　見れないはずの大きさ画面が見れてしまうバグ */
#define S2418_060817_FIX	(1)		//1にすると修正が有効
									//修正ソース：zkn_zukan_common.c

/* へんしん状態のポケモンが瀕死になったときに所持している技がそのままになっている不具合 */
#define AFTER_MASTER_061019_FIX	(1)	//1にすると修正が有効
									//修正ソース：scio.c

/* 「どくびし」を「こうそくスピン」で吹き飛ばすと「どろばくだん」と表示される不具合 */
#define AFTER_MASTER_061024_FIX	(1)	//1にすると修正が有効
									//修正ソース：skill_intp.c
									//
/*	ダブルバトルにおいてへんしんしたポケモンからどろぼうするとアイテム増殖になる不具合の修正 */
#define AFTER_MASTER_061026_FIX	(1)	//1にすると修正が有効
									//修正ソース：client.c

/*	さわぐを残りPP１において使用した場合、次ターンで、PPがないので発動しない不具合の修正 */
#define AFTER_MASTER_061113_1_FIX	(1)	//1にすると修正が有効
										//修正ソース：server.c
										//
/*	シンクロの挙動の不具合の修正 */
#define AFTER_MASTER_061113_2_FIX	(1)	//1にすると修正が有効
										//修正ソース：server_tool.c

/*	てだすけの挙動の不具合の修正 */
#define AFTER_MASTER_070117_FIX		(1)	//1にすると修正が有効
										//修正ソース：server.c

/*	とんぼがえり入れ替えで出てきたポケモンのシンクロの挙動の不具合の修正 */
#define AFTER_MASTER_070122_BT1_FIX	(1)	//1にすると修正が有効
										//修正ソース：sub_175.s

/*	おいうち発動時の最終ターンで「まもる」「みきり」が発動してしまう不具合の修正 */
#define AFTER_MASTER_070122_BT2_FIX	(1)	//1にすると修正が有効
										//修正ソース：skill_intp.c

/*	みがわりを出している相手へのおいうちの挙動の不具合の修正 */
#define AFTER_MASTER_070123_BT1_FIX	(1)	//1にすると修正が有効
										//修正ソース：sub_153.s

/*	そらをとぶ中のポケモンへのてだすけの挙動の不具合の修正 */
#define AFTER_MASTER_070123_BT2_FIX	(1)	//1にすると修正が有効
										//修正ソース：be_176.s

/*	おいうちのきあいのタスキへの挙動の不具合の修正 */
#define AFTER_MASTER_070123_BT3_FIX	(1)	//1にすると修正が有効
										//修正ソース：sub_153.s

/*	まもる、みきりのいびき、あくむへの挙動の不具合の修正 */
#define AFTER_MASTER_070123_BT4_FIX	(0)	//有効にするのは禁止
										//修正ソース：server.c

/*	とんぼがえり、メロメロの挙動の不具合の修正 */
#define AFTER_MASTER_070123_BT5_FIX	(1)	//1にすると修正が有効
										//修正ソース：sub_175.s

/*	ソーラービームの挙動の不具合の修正 */
#define AFTER_MASTER_070125_BT1_FIX	(1)	//1にすると修正が有効
										//修正ソース：be_151.s

/*	ダブルバトルで入れ替えおいうちを2体共発動させた時に両方共に気絶すると1体のこってしまう不具合の修正 */
#define AFTER_MASTER_070202_BT1_FIX	(1)	//1にすると修正が有効
										//修正ソース：server.c

/*	おいうちの挙動の不具合の修正（かいがらのすず、こだわり～、いのちのたま）*/
#define AFTER_MASTER_070202_BT2_FIX	(1)	//1にすると修正が有効
										//修正ソース：skill_intp.c sub_153.s battle_server.h

/*	まもる、みきりのいびき、あくむ、なげつける、しぜんのめぐみへの挙動の不具合の修正 */
#define AFTER_MASTER_070202_BT3_FIX	(0)	//有効にするのは禁止
										//修正ソース：server.c be_107.s

/*	入れ替えおいうちをみがわりで受けた時にみがわりが消えてもメッセージが出ない不具合の修正 */
#define AFTER_MASTER_070202_BT4_FIX	(1)	//1にすると修正が有効（AFTER_MASTER_070202_BT2_FIXも有効にしないと反映されません）
										//修正ソース：sub_153.s

/*	みちづれ、おいうちの挙動の不具合の修正 */
#define AFTER_MASTER_070202_BT5_FIX	(1)	//1にすると修正が有効
										//修正ソース：sub_009.s

/*	こだわり～系ととんぼがえりの挙動の不具合の修正 */
#define AFTER_MASTER_070213_BT_FIX	(1)	//1にすると修正が有効
										//修正ソース：server.c

/*	サファリゾーンで装備効果が発動してしまう不具合の修正 */
#define AFTER_MASTER_070219_BT_FIX	(1)	//1にすると修正が有効
										//修正ソース：server_tool.c

/*	アンコールのターン数が1ターン多い不具合の修正 */
#define AFTER_MASTER_070221_BT_FIX	(0)	//1にすると修正が有効(※北米、欧州版では修正しないのが仕様)
										//修正ソース：skill_intp.c	プラチナでも修正しないのが仕様

/*	技派生系（ゆびをふる、ねごとなど）で発生した技が特性で引き寄せられる系のとき効果が発動しない不具合の修正 */
#define AFTER_MASTER_070330_BT_FIX	(1)	//1にすると修正が有効
										//修正ソース：skill_intp.c

/*	まもる、みきりのいびき、あくむ、なげつける、しぜんのめぐみへの挙動の不具合の修正 */
#define AFTER_MASTER_070405_BT_FIX	(1)	//1にすると修正が有効
										//修正ソース：be_107.s sub_007.s

#if 0	//囲ってないけど反映済み
/*	2vs2時に場に3体以下しかポケモンがいないときのせんせいのつめの挙動の不具合の修正 */
#define AFTER_MASTER_070719_BT1_FIX	(1)	//1にすると修正が有効
										//修正ソース：sub_278.s

/*	まもる、みきりのとっておき、サイコシフト、ねこだましへの挙動の不具合の修正 */
#define AFTER_MASTER_070719_BT2_FIX	(1)	//1にすると修正が有効（ただしAFTER_MASTER_070405_BT_FIXで囲われた中にあるので、
										//そちらも有効にしないとダメです）
										//修正ソース：sub_007.s
#endif

//-------------通信関連 TXXXX 関連   Txxxx マリオクラブ  GTxxx マリオクラブ東京 GFTxxxx ゲームフリーク

/* ポケステータス　次のLvまでの経験値表示不具合 */
#define	T1653_060815_FIX	(1)		//1にすると修正が有効
									//修正ソース：p_status.c, pst_bmp.c, b_plist_main.c, b_plist_bmp.c


/* 地下でハタを奪われた際に本来ならば相手がウインドウを閉じるまで話しかけられないのに
   話しかけられてしまう不具合への対処 */
// 場所 field/comm_player.c
#define T1656_060815_FIX (1)
/* 親機がSINGLEモードへ移行している際に子機と接続してしまい、state変更ができないで
   止まってしまい、結果子機も動く許可を得られないまま止まってしまう不具合への対処*/
// 場所 field/comm_field_state.c
#define T1645_060815_FIX (1)
/* パソコンにハタを登録しレベルアップする際に他の親と接続した場合、
   タスクが強制終了されずに残ってしまう不具合への対処 */
/* パソコンが秘密基地に入ろうとしたフィールド移動時に読み込まれてしまい
   最悪ファイルアクセスに失敗して止まる不具合への対処 */
// 場所 field/underground/ug_secretbase.c
#define GT384_060815_FIX (1)
/* 壁の中を進む時に移動座標が無い場合進まないようにしたつもりが、
   違う戻り値を返したがためにそのまますすんでしまうバグ修正 */
// 場所 field/comm_player.c
#define T1662_060815_FIX  (1)
/* wh.c内で この修正を入れないと、接続する相手が見つからなくてタイムアウトした場合に、
必ず 関係の無い親に接続しに行ってしまう不具合への対処 */
// 場所 communication/wh.c
#define GFT0001_060816_FIX  (1)
/* 親機が子機と切断し ALONEモードへの移行の時に 罠装置解除がはたらき
   さらにCLIENT移動補正を呼ぶので、その場でもう一度装置にかかってしまう */
// 場所 field/comm_field_state.c
#define T1644_060816_FIX    (1)
/*  人数制限ビットを消す場所をLOG位置に入れていため人数が減ってしまうバグ

    子機にハタ受付完了が帰らなくて何度も再起動してしまうバグ 

 */
// 場所 field/underground/ug_manager.c
// 場所 field/underground/ug_secretbase.c
#define T1665_060816_FIX    (1)
/*罠装置にかかっている最中に秘密基地に入るとびっくりマークだけ表示させているが
  これを消すトリガーが無いので 秘密基地移動時にびっくりマークを消すようにする
 */
// 場所 field/comm_player.c
#define T1648_060818_FIX    (1)
/* 子機と親機がつながりにくい時があるのを修正
 */
// 場所 communication/comm_mp.c
// 場所 communication/comm_system.c
// 場所 communication/wh.c
#define T1657_060818_FIX    (1)
/* たすけたと解除のメッセージが両方表示されてしまう
 */
// 場所 field/underground/ug_trap.c
#define T1617_060825_FIX    (1)
/*  地下にはいってきたメッセージが２度表示される
 */
// 場所 comm_info.c
#define T1346_060825_FIX    (1)
/*   化石サーチをすばやく２度行うと　２度目のサーチが全部表示されない
  */
// ug_dig_fossil.c
// ug_trap.c
#define T1517_060825_FIX    (1)
/* 子機の親名が親機の親名になってしまうバグ対処 */
// 場所 contest/contest.c
#define T1669_060817_FIX	(1)

/* ボックスで下画面操作を行ってから名前修正を行うと名前入力の上画面が消えてしまうバグ対処 */
// 場所 application/namein/namein.c
#define S2427_060821_FIX	(1)

/* Wifi通信上での交換リスト画面でソフトウェアリセットが効いてしまうバグ対処 */
// 場所 application/tradelist/tradelist.c
#define T1666_060821_FIX	(1)




//-----------以下はマスター提出ロム以後の訂正

/* チャンピオンリーグ2Fのユニオンルームで電源切断時に正常に続行できないバグを対処*/
// 場所 field/zonedata.c
#define	AFTERMASTER_061004_FIX	(1)

/* ディアルガ＆パルキアを「倒す」と復活しないバグを修正*/
// 場所 src/fielddata/script/d05r0114.ev
#define AFTERMASTER_061031_IAEA_EV_FIX	(1)

/* ふしぎなおくりものでポケモンを受け取った際、固体乱数内の性別とPPP内の性別が違うバグ対処 */
// 場所 field/scr_postman.c
#define MYSTERY_POKEMON_SEX_FIX	(1)

/* 地下に最初に入った時に主人公の名前が出ていないバグを修正*/
// 場所 src/field/underground/ug_shop.c
#define AFTERMASTER_061214_UNDERGROUND_OPENING_FIX	(1)

/* WIFIP2Pアプリに入る時に下画面に線が出るバグを修正 */
// 場所 src/application/wifi_p2pmatch/wifi_p2pmatch.c
#define AFTERMASTER_061215_WIFIP2P_FADE_FIX	(1)

/* GTSで自分のポケモン引き取るときに通信切断を行うことで複製できるバグの対策 */
// 場所 application/worldtrade/worldtrade_uploadl.c
#define GTS_DUPLICATE_BUG_FIX	(1)

/* 通信進化する時にカスタムボールがついていると、カスタムボール情報が外れないバグの対処 */
// 場所 application/custom_ball/cb_disp.c
//      application/tradelist/tradelist.c
#define AFTERMASTER_061220_CUSTOMBALL_NODEL_BUG_FIX (1)

/* 特定のセーブデータで「レポートが壊れている」旨の計画が出続けるバグの対処 */
// 場所 src/savedata/saveload_system.c
#define	AFTERMASTER_070112_REPORT_WARNING_FIX		(1)

/* 通信サーチャーその他の表示がポフィンしかカウントできていないバグを修正 */
// 場所 src/communication/comm_mp.c
#define AFTERMASTER_070112_POKETCH_ETC (1)

/* 旗をパソコンから得た時に相手の名前を出すように修正 */
// 場所 src/field/underground/ug_pc.c
#define AFTERMASTER_070112_FLAGGET_MESSAGE (1)

/* wifiバトルタワーデータをアップロード後リロードすると、マップが真っ黒になるバグを修正*/
// 場所 src/field/b_tower_ev.c
#define AFTERMASTER_070112_WIFI_MAP_BUG_FIX (1)

/* かせき掘り終了時に親と切断すると、真っ黒な画面のままAボタンを押さないと先に進まないので
   タイマーを使用して先に進むように修正 */
// 場所 src/field/b_tower_ev.c
#define AFTER_MASTER_070122_FOSSILMSG_FIX  (1)

/* セーブ失敗時などの警告画面で時計アイコンのアニメがゴミとして表示される不具合の修正 */
//	場所 src/gflib/system.c
//	場所 src/communcation/com_dwc_warning.c
//	場所 src/application/backup/save_error_warning.c
#define	AFTERMASTER_070122_WARNING_DISP_FIX	(1)

/* グッズショップでグッズ選択時にSEが鳴らないバグを修正 */
// 場所 src/field/fd_shop.c
#define	AFTERMASTER_070122_GOODSSHOP_SE		(1)

/* フラワーショップのウィンドウがウィンドウタイプで変わってしまうバグの対処 */
// 場所 src/field/field_accessory_shop.c
#define AFTERMASTER_070122_ACCESSORYSHOP_WND_FIX	(1)

/* アクセサリの個数が７００個を超えるとクリップでフリーズするバグの対処 */
// 場所 src/savedata/imageclip_data.c
#define AFTERMASTER_070112_IMAGECLIP_ACCE_BUG_FIX	(1)

/* ポケモンを探すで、検索完了後に”けんさくちゅう”が一瞬出てしまうバグの対処 */
// 場所 src/application/zukanlist/zkn_sortsearch.c
#define AFTERMASTER_070112_ZUKAN_SORTMSG_BUG_FIX	(1)

/* スタートメニューでAGBカートリッジを抜くとフリーズしてしまうバグの対処 */
// 場所 src/system/startmenu.c, src/system/main.c
#define AFTERMASTER_070123_GBACARTRIDGE_BUG_FIX		(1)

/* パソコンに旗を追加した場合重複をはじく検査を入れる対処 */
// 場所 src/savedata/undergrounddata.c
#define AFTERMASTER_070125_UNDERGROUND_PCFLAGLIST_FIX	(1)

/* Wifiバトルタワーでランクを選んだ後にルームナンバーを初期化していないせいで、
   ランク毎のルーム数が違うとルームナンバーが存在しない場所をしていできてしまうバグを対処 */
// 場所 application/wifi_b_tower/wbtower_enter.c
#define AFTERMASTER_070125_WIFI_BATTLETOWER_BUG_FIX (1)

/* 親機が接続解除後、もう一度接続してきたときに、子機の方のログに不正な名前が入るバグを
 修正するのにあたり、親機との切断のタイミングでログを消すように修正 */
// 場所 field/comm_field_state.c
// 場所 field/underground/ug_dig_fossil.c .h
#define AFTER_MASTER_070202_FOSSILLOG_FIX  (1)

/* ユニオンルームで３０人目のビーコンと３１人目のビーコンを受け取ったときに
   下画面のボタンが聞かなくなるバグを修正 */
// 場所 src/field/union_board.c
#define AFTER_MASTER_070206_UNION_BOARD_FIX (1)

/* パソコンに旗を追加した場合重複をはじく検査を入れる対処+今までの重複検査をはずす対処 */
// 場所 src/field/comm_player.c
#define AFTERMASTER_070206_UNDERGROUND_PCFLAGLIST2ND_FIX (1)

/* ポケモンレンジャーとの通信シーケンスで、一部キー入力をすっ飛ばしていた箇所があったのを対処 */
// 場所 src/fushigi/pokemonranger.c
#define AFTERMASTER_070207_POKEMONRANGER_CONNECTMENU_FIX (1)

/* 地下の玉が成長限界になると、それ以降の配列に登録されているたまが成長しない */
// 場所 src/savedata/undergrounddata.c
#define AFTERMASTER_070207_UNDERGROUND_STONEUPDATE_FIX (1)

/* 道具がいっぱいで受け取れない場合道具名の所がトレーナー名になっている部分を修正 */
// 場所 src/field/scr_postman.c
#define AFTERMASTER_070213_POSTMAN_ITEMFULL_FIX (1)

/* 「ワイヤレスでうけとる」か「Wi-Fiでうけとる」を撰んだ後に「ともだちからうけとる」で
   AGBカートリッジからふしぎなおくりものを受け取れなくなるバグを修整 */
// 場所 src/fushigi/comm_mystery_gift.c
#define AFTERMASTER_070213_MISTERY_AGBCARTRIDGE_FIX (1)

/* レーダーの初期化時にBG面がONOFFされているので MASTERBRITNESSで囲んで修正しました */
// 場所 src/field/ug_radar.c
#define AFTERMASTER_070213_RADARINIT_FIX (1)

/* ＧＴＳとWifiバトルタワー接続画面の最初にWIPE_ResetBrightnessを呼んでしまっているために
   バックドロップ面が見えてしまう事があるバグを対処 */
// 場所 src/application/wifi_worldtrade/worldtrade.c
// 場所 src/application/wifi_b_tower/wbtower.c
#define AFTERMASTER_070215_GTS_WBTOWER_INIT_FIX	(1)

/* 通信初期化をした直後にアイコン表示を行うと、アニメが接続もしていないのに
   アンテナ３本の状態になって表示されてしまうバグの修正 */
// 場所 src/communication/wm_icon.c
#define AFTERMASTER_070220_WM_ICON_INIT_FIX	(1)

/* Wifiバトルタワーリーダーダウンロード画面でランクを指定しても
   自分のランクしか取得しなくなっているバグに対処 */
// 場所 src/wifi_b_tower/wbtower_enter.c
#define AFTERMASTER_070222_WIFI_BATTLETOWER_NOWRANK_BUG_FIX	(1)

//-----------以下は欧州版からの修正
//AIマルチで相手のいない状態で味方が繰り出したのろいの挙動の不具合
//修正ソース：tr_ai.c
#define	AFTER_MASTER_070320_BT1_EUR_FIX	(1)

//おんねん＋入れ替えおいうちの挙動の不具合（有効にするのは禁止、AFTER_MASTER_070409_30_EUR_FIXでの修正に変更）
//修正ソース：skill_intp.c
#define	AFTER_MASTER_070320_BT2_EUR_FIX	(0)

//行動が終了しているのに、てだすけが発動する不具合
//修正ソース：skill_intp.c
#define	AFTER_MASTER_070320_BT3_EUR_FIX	(1)

//こらえる＋半減木の実の挙動がおかしい
//修正ソース：sub_021.s
#define	AFTER_MASTER_070320_BT4_EUR_FIX	(1)

//こらえる＋半減木の実の挙動がおかしい（こらえる側がHP1だと半減木の実の効果のあとHPが1回復してしまう）
//修正ソース：server_tool.c
#define	AFTER_MASTER_070320_BT5_EUR_FIX	(1)

//戦闘ポケモンステータス画面のコンディションの簡易５角形表示位置を修正
//修正ソース：b_plist_obj.c
#define	AFTER_MASTER_070409_59_EUR_FIX	(1)

//ゆびをふるから派生で出た連続技に対するおんねんの挙動の不具合
//修正ソース：skill_intp.c
#define	AFTER_MASTER_070409_30_EUR_FIX	(1)

//みがわり中嫌いな木の実を食べたときにしかしうまく決まらなかったと表示される不具合
//修正ソース：sub_037.s
#define	AFTER_MASTER_070409_31_EUR_FIX	(1)

//ダブルバトルで、相手が1体しかいないときにトレースが発動しないときがある不具合
//修正ソース：server_tool.c
#define	AFTER_MASTER_070409_42_EUR_FIX	(1)

//さわぐの起きるチェックが気絶しているポケモンにも発生してしまう不具合
//修正ソース：server.c
#define	AFTER_MASTER_070409_44_EUR_FIX	(1)

//シャドーダイブ中のポケモンにてだすけをすると効果は発動するが外れたメッセージが表示される不具合
//修正ソース：server.c battle_server.h
#define	AFTER_MASTER_070409_49_EUR_FIX	(1)

//ポケモンを捕獲して戦闘を終了させたときにしぜんかいふくの効果が発動しない不具合
//修正ソース：server.c sub_011.s
#define	AFTER_MASTER_070409_60_EUR_FIX	(1)

//ミルクのみエフェクトを横取りされた際
//よこどりした側にエフェクトが出ない不具合修正
#define	AFTER_MASTER_070410_57_EUR_FIX	(1)		//囲っていないけど反映済み 2008.04.02(水)matsuda

//「いせきマニアのあな」の到着フラグセット漏れの不具合
//修正ソース：d22r0102.ev d22r0103.ev sp_d22r0102.ev sp_d22r0103.ev d22r0102_def.h d22r0103_def.h
#define	AFTER_MASTER_070426_86_EUR_FIX	(1)

//地下わなの解除方法メッセージが遅い場合消えてしまうので 一括表示に置き換え
//場所 src/field/underground/ug_trap.c
#define AFTERMASTER_070410_UNDERGROUND_N58_EUR_FIX (1)

//メッセージ強制削除時にキーだけが残ってしまうので メッセージを自動送りにします
// 同じ仕組みを使用しますのでAFTER_MASTER_070122_FOSSILMSG_FIXに依存します
//場所 src/field/underground/ug_dig_fossil.c
#define AFTERMASTER_070410_UNDERGROUND_N23_EUR_FIX (1)

// 移動しないと、地下レーダーの位置情報を送信していなかったバグを修正
//場所 src/field/underground/ug_dig_fossil.c
//場所 src/field/underground/ug_manager.c
#define AFTER_MASTER_070410_UNDERGROUND_N13_EUR_FIX (1)

// レーダー範囲に何も無い場合　最後の一個が残ったままになる不具合修正
// 場所 src/field/underground/ug_dig_fossil.c
#define AFTER_MASTER_070410_UNDERGROUND_N14_EUR_FIX (1)

// タッチを2回連続で拾ってしまうタイミングがあった箇所を修正しました
// 現状の分岐で以前よりtouchが遅くはなりますが このままにいたします
// 場所 src/field/underground/ug_manager.c
#define AFTER_MASTER_070410_UNDERGROUND_N15_EUR_FIX (1)

//再接続時に自分のステータス文字が残ったままになる不具合を修正
// 場所 src/application/wifi_p2pmatch.c
// 
// 　プラチナでは、自分のステータスが常に上に表示されているように仕様変更されたため、
// 　消してしまうと困るので、無効
// #define AFTER_MASTER_070410_WIFIAPP_N20_EUR_FIX (1)

// 最初起動フラグを セーブ表示後もONしたままだったのでOFFに修正
// 場所 src/application/wifi_p2pmatch.c
#define AFTER_MASTER_070410_WIFIAPP_N22_EUR_FIX (1)

// VBlank関数が入れ替わる関数を呼ばないように修正
// 場所 src/communication/com_dwc_warning.c
#define AFTER_MASTER_070411_COMM_N25_EUR_FIX (1)


// 接続の最初になかなかつながらなく、エラーになってしまう不具合への対処
//  ->親機で通信していないときにSendDataを呼ぶと接続後におかしくなる不具合を修正
// 場所 src/communication/comm_system.c
//      src/communication/wh.c
//      src/communication/comm_state.c
#define AFTER_MASTER_070420_GF_COMM_FIX  (1)

/* レコードコーナーで3人以上の時、０．５秒ぐらいのタイミングで子機が抜けて親を一人にすると
 「つごうがつかないメンバーが…」というメッセージが２重に表示されてウインドウ内で  
  表示が壊れてしまうバグを対処 */
// 場所 application/record/record_corner.c
#define AFTER_MASTER_070424_RECORDCONER_FIX	(1)

// ふしぎ通信でエラー表示後にOBJ面がOFFになっている不具合を修正
// 場所 src/fushigi/comm_mystery_gift.c
#define AFTER_MASTER_070426_WIFIICON_N81_EUR_FIX (1)

// 通信アイコンに半透明がかかるのでウインドウ機能を使用して修正
// 場所 src/field/underground/ug_trap.c
#define AFTER_MASTER_070426_WMICON_N82_EUR_FIX (1)

// レベルじいさんで、アイテムがもらえないときのメッセージが違う不具合を修正
// 場所 src/fielddata/script/r221r0201.ev
#define AFTER_MASTER_070426_LEVEL_JIISAN_FIX (1)

/* 育て屋のタマゴフラグが乱数の種である説明がなかったため、
   ポケッチとスクリプトがu16やu8で取得して０か１かで判定してしまっているのを
   育て屋処理の方で修正することにした */
// 場所 include/savedata/sodateyadata.h
//       src/savedata/sodateyadata.c
//       src/field/sodateya.c
#define AFTER_MASTER_SODATEYA_EGGFLAG_FIX	(1)

/* GTSで自分でポケモンを預けている時に、自分から検索してポケモンを交換した際
   セーブデータの「ポケモンをGTSに預けているフラグ」を落としてしまっているバグに対処 */
// 場所 src/application/wifi_worldtrade/worldtrade_upload.c
#define  AFTER_MASTER_070510_GTS_MENU_FIX	(1)

/*
 * 欧州版報告、保留不具合（No.104)
 * ディアルガ・パルキア戦で逃げて終了→殿堂入り→アルセウス戦とすると
 * ディアルガ・パルキアのOBJが表示されているが戦闘ができない不具合に対処
 */
// 場所 src/fielddata/script/common_scr.ev
#define	AFTER_MASTER_070719_EURO104_FIX		(1)


/*
	2007.07.19
	ポケッチアプリ切り替えボタンを連打すると、
	ポケッチ液晶の縁のパレットが書き換わってしまうバグの修正。
	修正ソース：src\field\poketch\poketch_v.c
	修正者：田谷
*/
#define  AFTER_MASTER_070719_POKETCH_APPNUM_PAL	(1)

// メガネが貰えないとき時のメッセージが違う不具合を対処
// 場所 src/fielddata/script/t05r0201.ev
#define AFTER_MASTER_070720_GLASSES_FIX (1)

/* 欧州版で文字が上に切れるバグがあったので、１ドットだけ上げる修正が組み込まれた */
// 場所 src/field/union_board.c
#define AFTERMASTER_070719_UNION_BOARD_FONT_BUG_FIX	(1)


// 「そらをとぶ」
// 場所 src/wazaeffect/wsp_tomoya.c
//		src/wazaeffect/we_180.s
#define AFTER_MASTER_070426_85_EUR_FIX	(1)



//
//	PLFIX	DP優先順位低のバグを修正したもの
//
/*
 *	申し込んだ瞬間に、相手がVCHATOFFにすると、音声が聞こえない
 *	場所	src/application/wifi_p2pmatch/wifi_p2pmatch.c
 */
#define PLFIX_T869	(1)

/*
 *	通信エラーで受付に戻されるときの画面切り替えがおかしい
 *	場所	src/system/main.c
 */
#define PLFIX_T1238	(1)

/*
 *	T1370　コロシアムで、対戦後の対戦部屋での動作がおかしい	
 *	場所	src/field/comm_player.c
 */
#define PLFIX_T1370 (1)

/*
 *	T1618　通信受けつけで、メンバーに入るのと同時に抜けたあとの動作がおかしい
 *	場所	src/field/comm_direct_counter.c
 */
#define PLFIX_T1618	(1)

/*
 *	S2414　ポケッチ：デジタル時計のスリープ後の時刻表示について
 *	場所	src/field/poketch/poketch_a00c.c
 */
#define PLFIX_S2414	(1)

/*
 *	T1624　ユニオンおえかきで、パレットを連打すると線が描かれる
 *	場所	src/application/oekaki/oekaki.c
 */
#define PLFIX_T1624	(1)

/*
 *	S2407　再報告：クリップでポケモンを枠一杯まで移動させることができない
 *	場所	src/application/imageClip/imc_poke_obj.c
 *			include/application/imageClip/imc_poke_obj.h
 *
 *	080603 tomoya
 *	BTS:シナリオ304
 *	　DPとPLでコンテストのポケモンの表示位置がずれてしまうため、
 *	　コンテストのときだけは、この修正は反映させないことになりました。
 */
#define PLFIX_S2407	(1)


#endif	__BUGFIX_H__
