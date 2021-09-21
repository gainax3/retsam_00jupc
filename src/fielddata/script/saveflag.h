//==============================================================================================
/**
 * @file	saveflag.h
 * @brief	ゲーム内セーブフラグ定義(スクリプトからのみアクセスされるフラグ)
 * @author	Satoshi Nohara
 * @date	05.10.22
 *
 * 05.11.10 Tomomichi Ohta
 */
//==============================================================================================
#ifndef	__SAVEFLAG_H__
#define	__SAVEFLAG_H__


//----------------------------------------------------------------------
//	汎用フラグ
//	(    1         -     99        )
//----------------------------------------------------------------------
//#define FH_00							(0)	//無効ナンバーなので使用不可
#define FH_01							(1)
#define FH_02							(2)
#define FH_03							(3)
#define FH_04							(4)
#define FH_05							(5)
#define FH_06							(6)
#define FH_07							(7)
#define FH_08							(8)
#define FH_09							(9)
#define FH_10							(10)
#define FH_11							(11)
#define FH_12							(12)
#define FH_13							(13)
#define FH_14							(14)
#define FH_15							(15)
#define FH_16							(16)
#define FH_17							(17)
#define FH_18							(18)
#define FH_19							(19)
#define FH_20							(20)
#define FH_21							(21)
#define FH_22							(22)
#define FH_23							(23)
#define FH_24							(24)
#define FH_25							(25)
#define FH_26							(26)
#define FH_27							(27)
#define FH_28							(28)
#define FH_29							(29)
#define FH_30							(30)
//#define FH_31							(31)
#define FH_31_REPORT					(31)			//07.07.24 common_scr レポート使用

#define FH_OBJ00						(32)
#define FH_OBJ01						(33)
#define FH_OBJ02						(34)
#define FH_OBJ03						(35)
#define FH_OBJ04						(36)
#define FH_OBJ05						(37)
#define FH_OBJ06						(38)
#define FH_OBJ07						(39)
#define FH_OBJ08						(40)
#define FH_OBJ09						(41)
#define FH_OBJ10						(42)
#define FH_OBJ11						(43)
#define FH_OBJ12						(44)
#define FH_OBJ13						(45)
#define FH_OBJ14						(46)
#define FH_OBJ15						(47)
#define FH_OBJ16						(48)
#define FH_OBJ17						(49)
#define FH_OBJ18						(50)
#define FH_OBJ19						(51)
#define FH_OBJ20						(52)
#define FH_OBJ21						(53)
#define FH_OBJ22						(54)
#define FH_OBJ23						(55)
#define FH_OBJ24						(56)
#define FH_OBJ25						(57)
#define FH_OBJ26						(58)
#define FH_OBJ27						(59)
#define FH_OBJ28						(60)
#define FH_OBJ29						(61)
#define FH_OBJ30						(62)
#define FH_OBJ31						(63)
//MAX = evwkdef.h LOCAL_FLAG_MAX

//----------------------------------------------------------------------
//	イベント進行制御フラグ
//
//	(FE_FLAG_START - FV_FLAG_START)
//	(    100       -    399        )	//古い
//	(    100       -    369        )	//08.01.18変更
//----------------------------------------------------------------------
//#define FE_TEST_01						(FE_FLAG_START+0)
//#define FE_TEST_02						(FE_FLAG_START+1)
//#define FE_TEST_03						(FE_FLAG_START+2)
//#define FE_TEST_04						(FE_FLAG_START+3)
//#define FE_TANE_GET						(FE_FLAG_START+4)
#define FE_PC_GOLD						(FE_FLAG_START+5)
#define FE_PC_POKERUS					(FE_FLAG_START+6)
#define FE_SUGOITURIZAO_GET				(FE_FLAG_START+7)
#define FE_R201_KIZUGUSURI_GET			(FE_FLAG_START+8)
#define FE_R201_TALK_PAPA				(FE_FLAG_START+9)
#define FE_T01R0101_RIVALMAMA_TALK		(FE_FLAG_START+10)	//ライバルママと会話した
//#define FE_GURASIDEANOHANA_TALK			(FE_FLAG_START+11)	//グラシデアのはなの話きいた
#define FE_UNION_CHARACTER_SELECT		(FE_FLAG_START+12)
#define FE_C04R0201_POLICE				(FE_FLAG_START+13)	//国際警察
#define FE_SCRATCH_TALK					(FE_FLAG_START+14)	//スクラッチ初回会話
#define FE_SODATE_TAMAGO				(FE_FLAG_START+15)	//そだてやのたまご
#define FE_C04_GYM_ITEM_GET				(FE_FLAG_START+16)	//C04ジムの技マシン入手フラグ
#define FE_C03_GYM_ITEM_GET				(FE_FLAG_START+17)	//C03ジムの技マシン入手フラグ
#define FE_C04R0304_ITEM_GET			(FE_FLAG_START+18)	//ワザ04入手
#define FE_ROTOMU_FORM_FIRST			(FE_FLAG_START+19)	//ロトムのフォルムチェンジ初回
#define FE_C06R0301_BOY3				(FE_FLAG_START+20)	//ミツハニーイベント会話した
#define FE_TANKENSETTO_GET				(FE_FLAG_START+21)	//たんけんセット入手
#define FE_D01R0102_LEADER_TALK			(FE_FLAG_START+22)	//リーダーと会話した
#define FE_R209R0105_KIRI_USE			(FE_FLAG_START+23)	//ロストタワーできりばらい使用した
#define FE_C03R0103_ITEM_GET			(FE_FLAG_START+24)	//かたい石入手
#define FE_C05_GYM_ITEM_GET				(FE_FLAG_START+25)	//C05ジムの技マシン入手フラグ
#define FE_C01R0504_ITEM_GET			(FE_FLAG_START+26)	//”きあいのはちまち”入手
#define FE_R205A_GINGAM_TALK			(FE_FLAG_START+27)	//ギンガ団と会話した
#define FE_FLOWER_ITEM_GET				(FE_FLAG_START+28)	//じょうろ入手
#define FE_C04R0204_WIN					(FE_FLAG_START+29)	//アジトイベント終了
#define FE_CYCLE_GET					(FE_FLAG_START+30)	//自転車入手
#define FE_T03R0301_ITEM_GET			(FE_FLAG_START+31)	//ワザ２入手
#define FE_BORONOTURIZAO_GET			(FE_FLAG_START+32)	//ボロのつりざお
#define FE_C03R0201_POKE_CHG			(FE_FLAG_START+33)	//ポケモン交換をした
#define FE_C04R0301_POKE_CHG			(FE_FLAG_START+34)	//ポケモン交換をした
#define FE_T01R0201_MAMA_TALK			(FE_FLAG_START+35)	//ママと会話した
#define FE_IMAGE_CLIP					(FE_FLAG_START+36)	//イメージクリップ遊んだ
#define FE_MEMORY_PRINT_SCORE_GET		(FE_FLAG_START+37)	//フロンティア制覇のスコア取得した
#define FE_C03_RIVAL_TALK				(FE_FLAG_START+38)	//ライバルと会話した
#define FE_D03R0101_SEVEN1_TALK			(FE_FLAG_START+39)	//5人衆と会話した
#define FE_C05R0102_ITEM_GET			(FE_FLAG_START+40)	//ワザマシン４５入手
#define FE_C05R0201_ITEM_GET			(FE_FLAG_START+41)	//ポルトケース入手
#define FE_INIT_CHANGE_OBJ_DEL			(FE_FLAG_START+42)	//INIT_CHANGEでOBJを消す
#define FE_BOUKENNOOTO_GET				(FE_FLAG_START+43)	//冒険ノートゲット
#define FE_ZUKAN_GET					(FE_FLAG_START+44)	//図鑑ゲット(イベント用)
#define FE_R205A_BABYGIRL1_TALK			(FE_FLAG_START+45)	//会話した
#define FE_C02_GYM_ITEM_GET				(FE_FLAG_START+46)	//C02ジムの技マシン入手フラグ
#define FE_HIDEN_01_GET					(FE_FLAG_START+47)	//ひでんワザ01入手
//#define FE_HIDEN_02_GET					(FE_FLAG_START+48)	//ひでんワザ02入手
//#define FE_HIDEN_03_GET					(FE_FLAG_START+49)	//ひでんワザ03入手
//#define FE_HIDEN_04_GET					(FE_FLAG_START+50)	//ひでんワザ04入手
#define FE_C07R0701_POKEMON_GET			(FE_FLAG_START+51)	//ポリゴン入手
#define FE_C04R0601_MOVE_POKE_ALL_GET	(FE_FLAG_START+52)	//移動ポケモン3匹捕まえた
#define FE_C04R0601_TALK				(FE_FLAG_START+53)	//オーキドと会話した
#define FE_HIDEN_08_GET					(FE_FLAG_START+54)	//ひでんワザ08入手
#define FE_C07_POLICE					(FE_FLAG_START+55)	//国際警察　倉庫に行く「はい」
#define FE_C06_GYM_ITEM_GET				(FE_FLAG_START+56)	//C06ジムの技マシン入手フラグ
#define FE_C07_GYM_ITEM_GET				(FE_FLAG_START+57)	//C07ジムの技マシン入手フラグ
#define FE_C09_GYM_ITEM_GET				(FE_FLAG_START+58)	//C09ジムの技マシン入手フラグ
#define FE_D13R0101_ITEM				(FE_FLAG_START+59)	//発電所のカギ
#define FE_D13R0101_AMAIMITU_GET		(FE_FLAG_START+60)	//あまいみつ入手
#define FE_R209R0105_ITEM_A_GET			(FE_FLAG_START+61)	//アイテム入手
#define FE_IITURIZAO_GET				(FE_FLAG_START+62)	//いいつりざお入手
#define FE_C06R0101_SOFT_GET			(FE_FLAG_START+63)	//ポケッチもらった
#define FE_C06R0101_COWGIRL				(FE_FLAG_START+64)	//会話した
#define FE_D01R0101_MYTH				(FE_FLAG_START+65)	//アイテムいっぱい会話した
#define FE_T05_OLDWOMAN_TALK			(FE_FLAG_START+66)	//長老と会話した
#define FE_FRESCO_SEE					(FE_FLAG_START+67)	//壁画見た
#define FE_L02_NEWS						(FE_FLAG_START+68)	//L02の事件中フラグ(resetするので注意！)
#define FE_D29R0101_RIVAL_TALK			(FE_FLAG_START+69)	//ライバルと会話した
#define FE_CONNECT_TSIGN_TALK			(FE_FLAG_START+70)	//サイン受付と会話した
#define FE_RECONGNIZE_SHINOU			(FE_FLAG_START+71)	//シンオウ図鑑”ひょうしょう”済み
#define FE_RECONGNIZE_NATIONAL			(FE_FLAG_START+72)	//全国図鑑”ひょうしょう”済み
#define FE_D26R0107_GKANBU_WIN			(FE_FLAG_START+73)	//幹部に勝利
#define FE_D26R0107_MACHINE				(FE_FLAG_START+74)	//機械の開放
#define FE_C10R0101_VETERAN				(FE_FLAG_START+75)	//門番どいた
#define FE_BIGFOUR1_WIN					(FE_FLAG_START+76)	//四天王に勝利
#define FE_BIGFOUR2_WIN					(FE_FLAG_START+77)	//四天王に勝利
#define FE_BIGFOUR3_WIN					(FE_FLAG_START+78)	//四天王に勝利
#define FE_BIGFOUR4_WIN					(FE_FLAG_START+79)	//四天王に勝利
#define FE_C10R0111_CHAMP_WIN			(FE_FLAG_START+80)	//チャンピオンに勝利
#define FE_C07R0701_TALK				(FE_FLAG_START+81)	//ポリゴンくれる人と話した
#define FE_C08_GYM_ITEM_GET				(FE_FLAG_START+82)	//C08ジムの技マシン入手フラグ
#define FE_L02_CHAMP					(FE_FLAG_START+83)	//L02チャンピオンイベント
#define FE_D27R0101_DOCTOR_TALK			(FE_FLAG_START+84)	//博士と会話した
#define FE_C07R0207_DOCTOR_TALK			(FE_FLAG_START+85)	//博士と会話した(★FH代用可能)
#define FE_D27R0102_GKANBU_WIN			(FE_FLAG_START+86)	//幹部に勝利
#define FE_CONNECT_UNION_GUIDE_TALK		(FE_FLAG_START+87)	//ユニオンガイドと会話した
#define FE_COIN_CASE_GET				(FE_FLAG_START+88)	//コインケース入手後
#define FE_C04R0801_ASSISTANTM_TALK		(FE_FLAG_START+89)	//アシスタント会話したか
#define	FE_D32R0401_BONUS_TALK			(FE_FLAG_START+90)	//ステージボーナス初回会話
#define FE_C01R0501_ITEM_GET			(FE_FLAG_START+91)	//せんせいのツメ入手
#define FE_C03R0102_ITEM_GET			(FE_FLAG_START+92)	//かいがらのずず入手
#define FE_C03R0202_ITEM_GET			(FE_FLAG_START+93)	//ヒートボール入手
#define FE_C04R0302_ITEM_GET			(FE_FLAG_START+94)	//わざましん６７入手
#define FE_GTC_GUIDE_TALK				(FE_FLAG_START+95)	//受付の説明受けた
#define FE_C07R0401_MAN2_TALK			(FE_FLAG_START+96)	//めざめるパワー会話した
#define FE_WAZAMASIN78_GET				(FE_FLAG_START+97)	//技マシン入手
#define FE_WAZAMASIN77_GET				(FE_FLAG_START+98)	//技マシン入手
#define FE_WAZAMASIN51_GET				(FE_FLAG_START+99)	//技マシン入手
#define FE_T06R0101_IN					(FE_FLAG_START+100)	//再戦施設に入ったか
#define FE_WAZAMASIN48_GET				(FE_FLAG_START+101)	//技マシン入手
#define FE_WAZAMASIN92_GET				(FE_FLAG_START+102)	//技マシン入手
#define FE_WAZAMASIN42_GET				(FE_FLAG_START+103)	//技マシン入手
#define FE_WAZAMASIN63_GET				(FE_FLAG_START+104)	//技マシン入手
#define FE_WAZAMASIN66_GET				(FE_FLAG_START+105)	//技マシン入手
#define FE_WAZAMASIN56_GET				(FE_FLAG_START+106)	//技マシン入手
#define FE_BACKGROUND_ACCE_GET			(FE_FLAG_START+107)	//背景お姉さんから全てもらった*/
#define FE_D05R0118_D_GET				(FE_FLAG_START+108)	//テンガン山ディアルガ入手
#define FE_D05R0119_P_GET				(FE_FLAG_START+109)	//テンガン山パルキア入手
#define FE_D07R0105_MOUNT_ITEM_GET		(FE_FLAG_START+110)	//アイテム入手
#define FE_P_PARK_ADMIN					(FE_FLAG_START+111)	//ポケパーク受付と会話したか
#define FE_HIIDORAN_EVENT				(FE_FLAG_START+112)	//ヒードランイベント中フラグ
#define FE_C11FS0101_ITEM_GET			(FE_FLAG_START+113)	//ピントレンズ入手
#define FE_KAZANNOOKIISI_GET			(FE_FLAG_START+114)	//かぜんのおきいし入手
#define FE_HIIDORAN_SEE					(FE_FLAG_START+115)	//ヒードランと遭遇しているか
#define FE_T04R0301_SUNGLASSES_TALK		(FE_FLAG_START+116)	//編集長の会話した
#define FE_R225R0101_ITEM_GET			(FE_FLAG_START+117)	//おいしいみず入手
#define FE_R229_ITEM_GET				(FE_FLAG_START+118)	//きんのたま入手
#define FE_D04R0101_MIDDLEMAN2_TALK		(FE_FLAG_START+119)	//ほしのかけらの話しをした
#define FE_T04R0301_ITEM_DEPOSIT		(FE_FLAG_START+120)	//編集長にアイテムを預けている
#define FE_D16R0102_SEVEN5_TALK			(FE_FLAG_START+121)	//５人衆と会話した
#define FE_R217R0101_ITEM_GET			(FE_FLAG_START+122)	//つららのプレート入手
#define FE_D09R0104_SEVEN4_TALK			(FE_FLAG_START+123)	//５人衆と会話した
#define FE_D21R0101_SEVEN2_TALK			(FE_FLAG_START+124)	//５人衆と会話した
#define FE_D24R0105_SEVEN3_TALK			(FE_FLAG_START+125)	//５人衆と会話した
#define FE_D24R0105_SEVEN3_POKE_MAX		(FE_FLAG_START+126)	//ルカがもらえなかった
#define FE_SEVEN1_CLEAR					(FE_FLAG_START+127)	//５人衆イベントをクリアーした
#define FE_SEVEN2_CLEAR					(FE_FLAG_START+128)	//５人衆イベントをクリアーした
#define FE_SEVEN3_CLEAR					(FE_FLAG_START+129)	//５人衆イベントをクリアーした
#define FE_SEVEN4_CLEAR					(FE_FLAG_START+130)	//５人衆イベントをクリアーした
#define FE_SEVEN5_CLEAR					(FE_FLAG_START+131)	//５人衆イベントをクリアーした
#define FE_D07R0105_MOUNT				(FE_FLAG_START+132)	//既に話しているか
#define FE_C06R0401_BIGMAN_TALK			(FE_FLAG_START+133)	//既に話しているか
#define FE_T01R0102_RIVAL_TALK			(FE_FLAG_START+134)	//２階のライバルと会話した
#define FE_C09_GYMLEADER_DEL			(FE_FLAG_START+135)	//ジムリーダーを消すフラグ
//#define FE_GTC_OPEN					(FE_FLAG_START+136)	//GTCオープン
#define FE_C01_CROWN1_TICKET			(FE_FLAG_START+137)	//ひきかえけん１
#define FE_C01_CROWN2_TICKET			(FE_FLAG_START+138)	//ひきかえけん２
#define FE_C01_CROWN3_TICKET			(FE_FLAG_START+139)	//ひきかえけん３
#define FE_GENERATE_START				(FE_FLAG_START+140)	//大量発生開始している
#define FE_TOWNMAP_GET					(FE_FLAG_START+141)	//タウンマップ入手
#define FE_ACCE_CASE_GET				(FE_FLAG_START+142)	//アクセサリーケース入手
#define FE_POKETCH_GET					(FE_FLAG_START+143)	//ポケッチ入手
#define FE_C09R0101_POKE_CHG			(FE_FLAG_START+144)	//ポケモン交換をした
#define FE_W226R0101_POKE_CHG			(FE_FLAG_START+145)	//ポケモン交換をした
#define FE_W226R0101_ZUKAN				(FE_FLAG_START+146)	//図鑑パワーアップ（海外テキスト）
#define FE_C08R0201_WEEK_TALK			(FE_FLAG_START+147)	//お洒落姉さんと会話した
#define FE_T01R0201_MAMA_TALK2			(FE_FLAG_START+148)	//ママと会話した2
#define FE_R213R0201_FOOTMARK			(FE_FLAG_START+149)	//フトマキと会話した
#define FE_T07R0101_MEMBER				(FE_FLAG_START+150)	//会員制の建物の会員
#define FE_R212AR0103_POKE_SEE			(FE_FLAG_START+151)	//マナフィーを見たフラグ
#define FE_C08R0401_GOODS				(FE_FLAG_START+152)	//地下おじさんにアイテム預け中
#define FE_R213R0301_POLICE				(FE_FLAG_START+153)	//国際警察と会話した
#define FE_SODATEYA_AZUKE				(FE_FLAG_START+154)	//育て屋に預けたことがある
#define	FE_D31R0201_JUDGE_TALK			(FE_FLAG_START+155)	//個体判定の人と会話したか
#define	FE_R210BR0101_DRAGON_TALK		(FE_FLAG_START+156)	//最強ワザ教え
#define	FE_C07_SUPPORT					(FE_FLAG_START+157)	//サポートイベント終了
#define	FE_C06_GINGAM					(FE_FLAG_START+158)	//ギンガ団一回逃げた
#define	FE_C06_GINGAM_2					(FE_FLAG_START+159)	//ギンガ団二回逃げた
#define	FE_R213_GINGAM					(FE_FLAG_START+160)	//ギンガ団逃げた
#define	FE_C07R0205_ITEM_GET			(FE_FLAG_START+161)	//くっつきバリ入手
#define	FE_L02_GINGAM					(FE_FLAG_START+162)	//ギンガ団
#define	FE_R210A_CLEAR					(FE_FLAG_START+163)	//ストッパーイベントクリア
#define	FE_R228R0201_OLDMAN1_TALK		(FE_FLAG_START+164)	//究極わざ教えじいさん
#define	FE_C03_ITEM_GET					(FE_FLAG_START+165)	//アイテム入手
#define	FE_C03R0602_ITEM_GET			(FE_FLAG_START+166)	//アイテム入手
#define	FE_GAKUSYUUSOUCHI_GET			(FE_FLAG_START+167)	//学習装置入手
#define	FE_C01R0701_BOY_WIN				(FE_FLAG_START+168)	//勝利した
#define	FE_C01R0701_GIRL_WIN			(FE_FLAG_START+169)	//勝利した
#define	FE_C07R0301_OPEN				(FE_FLAG_START+170)	//そうこのカギ使って扉開けた
#define	FE_D02_OPEN						(FE_FLAG_START+171)	//はつでんしょキー使って扉開けた
#define	FE_PC_SHINOU_ZUKAN_COMP			(FE_FLAG_START+172)	//PCで完成メッセージをきいた
#define	FE_UG_MISSION_TALK				(FE_FLAG_START+173)	//各ミッションの説明を聞いた
#define	FE_C01R0701_ITEM_GET			(FE_FLAG_START+174)	//技マシン入手したか
#define	FE_UG_MISSION_OK				(FE_FLAG_START+175)	//ミッションへのチャレンジ承諾
#define	FE_KUJI_ACCE_GET				(FE_FLAG_START+176)	//ポケモンくじでアクセサリーを貰った
#define	FE_R202_SUPPORT_TALK			(FE_FLAG_START+177)	//サポートと会話した
#define	FE_R209R0105_ITEM_B_GET			(FE_FLAG_START+178)	//アイテム入手したか
#define	FE_SIIRUIRE_GET					(FE_FLAG_START+179)	//シールいれ入手
#define	FE_R213_GINGAM_TALK				(FE_FLAG_START+180)	//ギンガ団を一度捕まえた
#define	FE_C04R0601_ITEM_GET			(FE_FLAG_START+181)	//アイテム入手した
#define	FE_D20R0106_LEGEND_UNSEAL		(FE_FLAG_START+182)	//レジキング解放済み
#define	FE_D20R0106_LEGEND_CLEAR		(FE_FLAG_START+183)	//レジキング入手
#define	FE_C06R0301_ITEM_GET			(FE_FLAG_START+184)	//ミノムッチのアイテム入手
#define	FE_D24R0201_ITEM_GET			(FE_FLAG_START+185)	//アイテム入手
#define	FE_D05R0116_LEGEND_CLEAR		(FE_FLAG_START+186)	//アウス入手
#define	FE_D15R0101_LEGEND_CLEAR		(FE_FLAG_START+187)	//ムーバス入手
#define	FE_D16R0103_LEGEND_CLEAR		(FE_FLAG_START+188)	//ヒードラン入手
#define	FE_D17R0104_LEGEND_CLEAR		(FE_FLAG_START+189)	//キマイラン入手
#define	FE_D18_LEGEND_CLEAR				(FE_FLAG_START+191)	//エウリス入手
#define	FE_T05R0501_CHAMP				(FE_FLAG_START+192)	//壁画前のシロナ
#define	FE_D16R0103_LEGEND_UNSEAL		(FE_FLAG_START+193)	//ヒードラン解放済み
#define	FE_D28R0103_LEGEND_CLEAR		(FE_FLAG_START+194)	//ハイ入手
#define	FE_D29R0103_LEGEND_CLEAR		(FE_FLAG_START+195)	//レイ入手
#define	FE_C06_ACCE_GET					(FE_FLAG_START+196)	//アクセサリー入手
#define	FE_C07R0201_ACCE_GET			(FE_FLAG_START+197)	//アクセサリー入手
#define	FE_D03_ACCE_GET					(FE_FLAG_START+198)	//アクセサリー入手
#define	FE_R206R0101_ACCE_GET			(FE_FLAG_START+199)	//アクセサリー入手
#define	FE_C02R0501_EVENT_CLEAR			(FE_FLAG_START+200)	//クレセリア(ムーバス)少年助けた
#define	FE_R224_EVENT_CLEAR				(FE_FLAG_START+201)	//石碑名入力イベントクリア
#define	FE_C07R0101_MIDDLEMAN1_COIN_GET	(FE_FLAG_START+202)	//コイン
#define	FE_C07R0101_BIGMAN1_COIN_GET	(FE_FLAG_START+203)	//コイン
#define	FE_C07R0101_ITEM_GET			(FE_FLAG_START+204)	//１０レンチャンごほうび
#define	FE_C05R0901_POKEMON_GET			(FE_FLAG_START+205)	//ポケモンを貰った
#define	FE_YABURETA_SEKAI				(FE_FLAG_START+206)	//やぶれたせかい
#define	FE_D15_UNSEAL					(FE_FLAG_START+207)	//クレセリア(ムーバス)少年イベント開始==D15解禁
#define	FE_T05R0201_MEGANE_01_GET		(FE_FLAG_START+208)	//メガネ1ゲット
#define	FE_T05R0201_MEGANE_02_GET		(FE_FLAG_START+209)	//メガネ2ゲット
#define	FE_T05R0201_MEGANE_03_GET		(FE_FLAG_START+210)	//メガネ3ゲット
#define	FE_R221R0201_ITEM_KEEP			(FE_FLAG_START+211)	//アイテムを預かっている状態
#define	FE_D04R0101_ITEM_GET			(FE_FLAG_START+212)	//アイテム入手
#define	FE_R212AR0102_ITEM_GET			(FE_FLAG_START+213)	//アイテム入手
#define	FE_L02R0201_ITEM_GET			(FE_FLAG_START+214)	//アイテム入手
#define	FE_R217R0201_ITEM_GET			(FE_FLAG_START+215)	//アイテム入手
#define	FE_C02R0401_START				(FE_FLAG_START+216)	//ダークイベント始めた
#define	FE_C01R0202_ACCE_GET			(FE_FLAG_START+217)	//アクセサリー入手
#define	FE_L02_GINGA_WIN				(FE_FLAG_START+218)	//L02ギンガ団に勝利
#define FE_R208_ITEM_GET				(FE_FLAG_START+219)	//アイテム入手
#define FE_C06R0601_ITEM_GET_STYLE		(FE_FLAG_START+220)	//アイテム入手
#define FE_C06R0601_ITEM_GET_BEAUTIFUL	(FE_FLAG_START+221)	//アイテム入手
#define FE_C06R0601_ITEM_GET_CUTE		(FE_FLAG_START+222)	//アイテム入手
#define FE_C06R0601_ITEM_GET_CLEVER		(FE_FLAG_START+223)	//アイテム入手
#define FE_C06R0601_ITEM_GET_STRONG		(FE_FLAG_START+224)	//アイテム入手
#define	FE_T05R0101_OLDWOMAN1			(FE_FLAG_START+225)	//カンナギ長老イベント終了
#define	FE_SUPPORT_POKETORE_1			(FE_FLAG_START+226)	//サポートのポケトレ会話
#define	FE_SUPPORT_POKETORE_2			(FE_FLAG_START+227)	//サポートのポケトレ会話
#define	FE_SUPPORT_POKETORE_3			(FE_FLAG_START+228)	//サポートのポケトレ会話
#define FE_D25R0106_SPPOKE_CLEAR		(FE_FLAG_START+229)	//d25r0106プラズマ捕獲
#define	FE_C05_GOODS_01					(FE_FLAG_START+230)	//グッズ1入手済み
#define	FE_C05_GOODS_02					(FE_FLAG_START+231)	//グッズ2入手済み
#define	FE_C05_GOODS_03					(FE_FLAG_START+232)	//グッズ3入手済み
#define	FE_C05_GOODS_04					(FE_FLAG_START+233)	//グッズ4入手済み
#define	FE_C05_GOODS_05					(FE_FLAG_START+234)	//グッズ5入手済み
#define	FE_C05_GOODS_06					(FE_FLAG_START+235)	//グッズ6入手済み
#define	FE_C05_GOODS_07					(FE_FLAG_START+236)	//グッズ7入手済み
#define	FE_C05_GOODS_08					(FE_FLAG_START+237)	//グッズ8入手済み
#define	FE_C05_GOODS_09					(FE_FLAG_START+238)	//グッズ9入手済み
#define	FE_C05_GOODS_10					(FE_FLAG_START+239)	//グッズ10入手済み
#define	FE_C05_GOODS_11					(FE_FLAG_START+240)	//グッズ11入手済み
#define	FE_C07_POLICE_WAIT				(FE_FLAG_START+241)	//国際警察　ギンガトバリビルの前で待ち
#define	FE_C06_POKE						(FE_FLAG_START+242)	//グレッグルイベント許可=0,不許可=1
#define	FE_C09_CHAMP					(FE_FLAG_START+243)	//チャンピオンイベント
#define FE_D30R0101_LEGEND_CLEAR		(FE_FLAG_START+244)	//d30r0101ダーク捕獲
#define FE_C05R1101_POFIN_INFOMATION	(FE_FLAG_START+245)	//ポフィン入手：受付前の会話
#define FE_C05R1101_POFIN_GET			(FE_FLAG_START+246)	//ポフィン入手：ポフィン入手済み
#define	FE_C05_ALL_GOODS_GET			(FE_FLAG_START+247)	//ここでの全てのグッズ入手済み
#define FE_T01R0201_GAMECLEAR_KISSAKI	(FE_FLAG_START+248)	//ゲームクリア後のママとの会話
#define FE_T01R0201_GAMECLEAR_ZUKAN		(FE_FLAG_START+249)	//ゲームクリア後のママとの会話
#define	FE_D05R0114_CLEAR				(FE_FLAG_START+250)	//テンガンザン山頂クリア
//#define	FE_D05R0114_SPPOKE_GET			(FE_FLAG_START+251)	//イア（エア）捕獲した
#define	FE_D05R0102_ITEM_GET			(FE_FLAG_START+252)	//ゴールドスプレー入手
#define	FE_R213R0501_MYSTERY_TALK		(FE_FLAG_START+253)	//ミステリーと会話
#define	FE_C08R0801_TALK				(FE_FLAG_START+254)	//灯台でジムリーダーと会話した
#define	FE_R212AR0103_RICH_TALK			(FE_FLAG_START+255)	//金持ちおじさんと会話（特定）
#define	FE_R212AR0103_BUTLER_TALK		(FE_FLAG_START+256)	//執事と会話（特定）
#define	FE_C07R0101_ITEM_RETRY			(FE_FLAG_START+257)	//だいばくはつリトライ
#define	FE_R212AR0101_MAID_TALK			(FE_FLAG_START+258)	//メイド会話したか

//FE_MAX = (FE_FLAG_START(100)+299)		//古い
//FE_MAX = (FE_FLAG_START(100)+269)		//08.01.18変更

//----------------------------------------------------------------------
//	バニッシュフラグ
//
//	(FV_FLAG_START - FH_FLAG_START)
//	(    400       -    729      )		//古い
//	(    370       -    729      )		//08.01.18変更
//----------------------------------------------------------------------
#define FV_R201_RIVAL					(FV_FLAG_START+0)		//ライバル
#define FV_T01R0202_RIVAL				(FV_FLAG_START+1)		//ライバル
#define FV_T01_RIVAL					(FV_FLAG_START+2)		//ライバル
#define FV_D27R0101_DOCTOR				(FV_FLAG_START+3)		//博士
#define FV_D27R0101_AKAGI				(FV_FLAG_START+4)		//アカギ
#define FV_T02_SUPPORT					(FV_FLAG_START+5)		//サポート
#define FV_R201_DOCTOR					(FV_FLAG_START+6)		//博士
#define FV_R201_SUPPORT					(FV_FLAG_START+7)		//サポート
#define FV_C01_SUPPORT					(FV_FLAG_START+8)		//サポート(コトブキ)
#define FV_R203_RIVAL					(FV_FLAG_START+9)		//ライバル(r203)
#define FV_C03_RIVAL					(FV_FLAG_START+10)		//ライバル(クロガネ)
#define FV_R201_BAG						(FV_FLAG_START+11)		//バッグ(R201)
#define	FV_C05R0701_STOPPER				(FV_FLAG_START+12)		//ストッパー
#define FV_R205A_BOY2					(FV_FLAG_START+13)		//少年
#define FV_C04_CHAMP					(FV_FLAG_START+14)		//チャンピオン(ハクタイ)
#define FV_C01_POLICE					(FV_FLAG_START+15)		//国際警察
#define FV_D05R0102_HAHEN				(FV_FLAG_START+16)		//テンガン山壁画の破片
#define FV_DOSA_TR1						(FV_FLAG_START+17)		//ドサ周りのトレーナー1
#define FV_DOSA_TR2						(FV_FLAG_START+18)		//ドサ周りのトレーナー2
#define FV_C07R0207_DOCTOR				(FV_FLAG_START+19)		//博士
#define FV_C02R0101_BIGFOUR4			(FV_FLAG_START+20)		//図書館ゴヨウ
//#define FV_C04_GYM_LEADER				(FV_FLAG_START+21)		//C04ジムのリーダー
#define FV_R202_SUPPORT					(FV_FLAG_START+22)		//サポート
#define FV_C04_AKAGI					(FV_FLAG_START+23)		//アカギ
#define FV_D01R0102_LEADER				(FV_FLAG_START+24)		//ジムリーダー
#define FV_C01_SUNGLASSES				(FV_FLAG_START+25)		//サングラス(アクセサリケース)
#define FV_C04_RIVAL					(FV_FLAG_START+26)		//ライバル
#define FV_C05_SUNGLASSES				(FV_FLAG_START+27)		//サングラス(ヨスガ)
#define FV_D02_GINGAM_A					(FV_FLAG_START+28)		//ギンガ団(d02)
#define FV_D27R0101_POKE				(FV_FLAG_START+29)		//ポケモン
#define FV_C05R1101_LEADER				(FV_FLAG_START+30)		//メリッサ
#define FV_C07_LEADER					(FV_FLAG_START+31)		//マキシマム仮面
#define FV_C04R0101_MIDDLEM				(FV_FLAG_START+32)		//自転車の人
//#define FV_C04R0101_POKE				(FV_FLAG_START+33)		//自転車のポケモン
#define FV_D27R0101_SUPPORT				(FV_FLAG_START+34)		//サポート
#define FV_L01_RIVAL					(FV_FLAG_START+35)		//ライバル
#define FV_D27R0101_RIVAL				(FV_FLAG_START+36)		//ライバル
#define FV_T02_RIVAL					(FV_FLAG_START+37)		//ライバル
#define FV_T02R0101_DOCTOR				(FV_FLAG_START+38)		//博士
#define FV_T02R0101_SUPPORT				(FV_FLAG_START+39)		//サポート
#define FV_C03_BOY1						(FV_FLAG_START+40)		//シューズ少年
#define FV_D03R0101_SEVEN1				(FV_FLAG_START+41)		//5人衆
#define FV_C01_GINGA					(FV_FLAG_START+42)		//ギンガ団
#define FV_C01_DOCTOR					(FV_FLAG_START+43)		//博士
#define FV_D13R0101_ITEM				(FV_FLAG_START+44)		//発電所の鍵
#define FV_D13R0101_GINGAM_A			(FV_FLAG_START+45)		//ギンガ団
#define FV_D13R0101_GINGAM_B			(FV_FLAG_START+46)		//ギンガ団
#define FV_T03_GINGAM					(FV_FLAG_START+47)		//ギンガ団
#define FV_R205A_GINGAM					(FV_FLAG_START+48)		//ギンガ団
#define FV_C07R0101_POLICE				(FV_FLAG_START+49)		//国際警察
#define FV_D02R0101_GINGAM				(FV_FLAG_START+50)		//ギンガ団(POS)
#define FV_D02R0101_GINGALEADER1		(FV_FLAG_START+51)		//ギンガ団リーダー
#define FV_T05R0501_CHAMP				(FV_FLAG_START+52)		//壁画前のシロナ
#define FV_UG_MOUNT						(FV_FLAG_START+53)		//地下山男
#define FV_C07_SUPPORT					(FV_FLAG_START+54)		//サポート
#define FV_C06_RIVAL_STOPPER			(FV_FLAG_START+55)		//ライバル(ジム前ストッパー)
#define FV_L03_RIVAL					(FV_FLAG_START+56)		//ライバル
#define FV_D05R0101_GINGABOSS			(FV_FLAG_START+57)		//ギンガ団ボス
#define FV_T05_OLDWOMAN					(FV_FLAG_START+58)		//長老
#define FV_L02_CHAMP					(FV_FLAG_START+59)		//チャンピオン
#define FV_L02_GINGAM					(FV_FLAG_START+60)		//ギンガ団
#define FV_T05_GINGAM					(FV_FLAG_START+61)		//ギンガ団
#define FV_R210A_POKE					(FV_FLAG_START+62)		//ストッパー
#define FV_R210A_CHAMP					(FV_FLAG_START+63)		//チャンピオン
#define FV_C02_RIVAL					(FV_FLAG_START+64)		//ライバル
#define FV_C02_DOCTOR					(FV_FLAG_START+65)		//博士
#define FV_C02_SEAMAN					(FV_FLAG_START+66)		//一般市民
#define FV_C02_SUPPORT					(FV_FLAG_START+67)		//サポート
#define FV_C02R0103_SUPPORT				(FV_FLAG_START+68)		//サポート
#define FV_C02R0103_DOCTOR				(FV_FLAG_START+69)		//博士
#define FV_C02R0103_RIVAL				(FV_FLAG_START+70)		//ライバル
#define FV_D29R0102_RIVAL				(FV_FLAG_START+71)		//ライバル
#define FV_D29R0102_GKANBU2				(FV_FLAG_START+72)		//ギンガ団幹部
#define FV_C04R0201_POLICE				(FV_FLAG_START+73)		//国際警察
#define FV_D28R0101_GINGA				(FV_FLAG_START+74)		//ギンガ団
#define FV_T05_CHAMP					(FV_FLAG_START+75)		//シロナ
#define FV_D26R0104_GINGABOSS			(FV_FLAG_START+76)		//ギンガボス
#define FV_C07_GINGAM					(FV_FLAG_START+77)		//ギンガ団
#define FV_D27R0102_GKANBU				(FV_FLAG_START+78)		//ギンガ団幹部
#define FV_D28R0101_KOIKING				(FV_FLAG_START+79)		//コイキング
#define FV_C04R0201_POLICE_GINGA		(FV_FLAG_START+80)		//国際警察
#define FV_T05R0101_OLDWOMAN			(FV_FLAG_START+81)		//長老
//#define	FV_D05R0114_SPPOKE_01			(FV_FLAG_START+82)		//テンガンザン山頂：イア（エア）
#define	FV_D05R0114_RIVAL				(FV_FLAG_START+83)		//テンガンザン山頂：ライバル
//#define FV_D05R0114_SUPPORT				(FV_FLAG_START+84)		//テンガンザン山頂：サポート
#define FV_D05R0115_GINGA				(FV_FLAG_START+85)		//テンガンザン山頂：おろおろギンガ団
#define	FV_D05R0114_GINGAM				(FV_FLAG_START+86)		//テンガンザン山頂：ギンガ団ザコ
#define	FV_D05R0114_GINGAKANBU			(FV_FLAG_START+87)		//テンガンザン山頂：ギンガ団幹部
#define	FV_D05R0114_GINGABOSS			(FV_FLAG_START+88)		//テンガンザン山頂：ギンガ団ボス
#define	FV_D31R0205_TRAINER				(FV_FLAG_START+89)		//タワーシングル対戦相手
#define	FV_R207_SUPPORT					(FV_FLAG_START+90)		//サポート
#define	FV_D05R0115_CHAMP				(FV_FLAG_START+91)		//やぶれた世界入り口のシロナ
//#define	FV_D05R0114_POKE_L01			(FV_FLAG_START+92)		//テンガンザン山頂：レイ
//#define	FV_D05R0114_POKE_L02			(FV_FLAG_START+93)		//テンガンザン山頂：アイ
//#define	FV_D05R0114_POKE_L03			(FV_FLAG_START+94)		//テンガンザン山頂：ハイ
#define	FV_T04R0101_SUNGLASSES			(FV_FLAG_START+95)		//そだてやのサングラス
#define	FV_R221R0101_OOKIDO				(FV_FLAG_START+96)		//オーキド
#define	FV_C11_RIVAL					(FV_FLAG_START+97)		//ライバル
#define	FV_C11_SEVEN2					(FV_FLAG_START+98)		//５人衆
#define	FV_L02_RIVAL					(FV_FLAG_START+99)		//ライバル(L02)
#define	FV_T06R0101_SEVEN2				(FV_FLAG_START+100)		//５人衆
#define	FV_R227_RIVAL					(FV_FLAG_START+101)		//ライバル
#define	FV_R227_GYMLEADER				(FV_FLAG_START+102)		//ジムリーダー
#define	FV_T06_RIVAL					(FV_FLAG_START+103)		//ライバル
#define	FV_D16R0102_SEVEN5				(FV_FLAG_START+104)		//５人衆
#define	FV_D16R0103_SEVEN2				(FV_FLAG_START+105)		//５人衆
#define	FV_D16R0103_GKANBU				(FV_FLAG_START+106)		//プルート
#define	FV_D16R0103_HIIDORAN			(FV_FLAG_START+107)		//ヒードラン
#define	FV_D16R0103_KAZANNOOKIISI		(FV_FLAG_START+108)		//かざんのおきいし
#define	FV_D27R0103_AI					(FV_FLAG_START+109)		//アイ
#define	FV_D28R0103_HAI					(FV_FLAG_START+110)		//ハイ
#define	FV_D29R0103_REI					(FV_FLAG_START+111)		//レイ
#define	FV_C11_LEADER					(FV_FLAG_START+112)		//オーバ＆デンジ
#define	FV_C11_TOWERBOSS				(FV_FLAG_START+113)		//タイクーン
#define	FV_D21R0101_SEVEN2				(FV_FLAG_START+114)		//５人衆
#define	FV_D24R0105_SEVEN3				(FV_FLAG_START+115)		//５人衆
#define	FV_D09R0104_SEVEN4				(FV_FLAG_START+116)		//５人衆
#define	FV_D24R0105_GINGAM				(FV_FLAG_START+117)		//鋼鉄島ギンガ団
#define	FV_R213_POLICE					(FV_FLAG_START+118)		//国際警察
#define	FV_D31R0201_PRIZEMAN			(FV_FLAG_START+119)		//タワー受付賞品をくれる人
#define	FV_D31R0206_TRAINER01			(FV_FLAG_START+120)		//タワーマルチ対戦部屋トレーナー1
#define	FV_D31R0206_TRAINER02			(FV_FLAG_START+121)		//タワーマルチ対戦部屋トレーナー2
#define	FV_D31R0207_SEVEN1				(FV_FLAG_START+122)		//タワーサロン五人衆1
#define	FV_D31R0207_SEVEN2				(FV_FLAG_START+123)		//タワーサロン五人衆2
#define	FV_D31R0207_SEVEN3				(FV_FLAG_START+124)		//タワーサロン五人衆3
#define	FV_D31R0207_SEVEN4				(FV_FLAG_START+125)		//タワーサロン五人衆4
#define	FV_D31R0207_SEVEN5				(FV_FLAG_START+126)		//タワーサロン五人衆5
#define	FV_T01R0201_RIVALMAMA			(FV_FLAG_START+127)		//ライバルママ
#define	FV_T01R0102_RIVAL				(FV_FLAG_START+128)		//ライバル
#define	FV_C09_GYMLEADER				(FV_FLAG_START+129)		//ジムリーダー
#define	FV_C01R0701_RIVAL				(FV_FLAG_START+130)		//ライバル
#define	FV_C01_CROWN					(FV_FLAG_START+131)		//ピエロ
#define	FV_C01_POKETCH					(FV_FLAG_START+132)		//ポケッチくれる人
#define	FV_C01R0101_POKETCH				(FV_FLAG_START+133)		//ポケッチくれる人
#define	FV_R205A_BABYGIRL1				(FV_FLAG_START+134)		//発電所付近の少女
#define	FV_D02R0101_BABYGIRL1			(FV_FLAG_START+135)		//発電所付近の少女
#define	FV_D02_POLICE					(FV_FLAG_START+136)		//国際警察
#define	FV_C06_LEADER					(FV_FLAG_START+137)		//マキシ
#define	FV_D03R0101_LEADER				(FV_FLAG_START+138)		//ナタネ
#define	FV_C04_GINGA					(FV_FLAG_START+139)		//ギンガ団
#define	FV_C04PC0101_GONBE				(FV_FLAG_START+140)		//ゴンベ
#define	FV_C04R0204_MIDDLEM				(FV_FLAG_START+141)		//アジトにいるおじさん
#define	FV_C04R0101_PIPPI				(FV_FLAG_START+142)		//ぴっぴ
#define	FV_C05R1102_CON1				(FV_FLAG_START+143)		//コンテスト参加者
#define	FV_C05R1102_CON2				(FV_FLAG_START+144)		//コンテスト参加者
#define	FV_C05R1102_CON3				(FV_FLAG_START+145)		//コンテスト参加者
#define	FV_C05R1102_CON4				(FV_FLAG_START+146)		//コンテスト参加者
#define	FV_C05R1102_CON5				(FV_FLAG_START+147)		//コンテスト参加者
#define	FV_C05_STOPPER					(FV_FLAG_START+148)		//コンテストストッパー
#define	FV_C05R0601_RIVAL				(FV_FLAG_START+149)		//ライバル
#define	FV_C05R1101_MAMA				(FV_FLAG_START+150)		//ママ
#define	FV_C05R1101_JUDGE				(FV_FLAG_START+151)		//ミミィ
#define	FV_MYSTERY_POSTMAN				(FV_FLAG_START+152)		//ふしぎ配達員
#define	FV_D02_FUUSEN					(FV_FLAG_START+153)		//フーセン
#define FV_C06_GINGAM					(FV_FLAG_START+154)		//ギンガ団
#define FV_C07R0301_POLICE				(FV_FLAG_START+155)		//国際警察
#define FV_TV_INTERVIEW_02				(FV_FLAG_START+156)		//TV(インタビュー)バトルタワー
#define FV_C06GYM0101_LEADER			(FV_FLAG_START+157)		//マキシ
#define FV_TV_INTERVIEW_04				(FV_FLAG_START+158)		//TV(インタビュー)あなたのポケモン
#define FV_R213R0301_POLICE				(FV_FLAG_START+159)		//国際警察
#define FV_TV_INTERVIEW_06				(FV_FLAG_START+160)		//TV(インタビュー)ポケッチ
#define FV_TV_INTERVIEW_07				(FV_FLAG_START+161)		//TV(インタビュー)コンテスト
#define FV_D24_SEVEN3					(FV_FLAG_START+162)		//鋼鉄島入り口のゲン
#define FV_TV_INTERVIEW_09				(FV_FLAG_START+163)		//TV(インタビュー)クリップ
#define FV_TV_INTERVIEW_10				(FV_FLAG_START+164)		//TV(インタビュー)性格診断
#define FV_TV_INTERVIEW_11				(FV_FLAG_START+165)		//TV(インタビュー)ポルト
#define FV_C04_LEADER					(FV_FLAG_START+166)		//ナタネ
#define FV_TV_INTERVIEW_13				(FV_FLAG_START+167)		//TV(インタビュー)連れ歩き
#define FV_R213_GINGAM					(FV_FLAG_START+168)		//ギンガ団
#define FV_L02_GINGAM_1					(FV_FLAG_START+169)		//逃げるギンガ団
#define FV_R218R0201_ASSISTANTM			(FV_FLAG_START+170)		//助手（サポーターの親？）
#define FV_C10R0101_RIVAL				(FV_FLAG_START+171)		//ライバル
#define FV_T04_RIVAL					(FV_FLAG_START+172)		//ライバル
#define FV_D07R0105_BABYBOY1			(FV_FLAG_START+173)		//シール少年
#define FV_C05_EGG						(FV_FLAG_START+174)		//タマゴくれる人
#define FV_C04GYM0101_LEADER			(FV_FLAG_START+175)		//ジムリーダー
#define FV_C04R0401_ASSISTANTM			(FV_FLAG_START+176)		//助手
#define FV_C07_GINGAM_3					(FV_FLAG_START+177)		//ギンガ団
#define FV_D26R0101_DOOR				(FV_FLAG_START+178)		//ドア
#define FV_D26R0103_DOOR				(FV_FLAG_START+179)		//ドア
#define FV_D26R0104_DOOR				(FV_FLAG_START+180)		//ドア
#define FV_D26R0106_DOOR				(FV_FLAG_START+181)		//ドア
#define FV_C07R0301_BAR					(FV_FLAG_START+182)		//バー
#define FV_T05R0501_OLDWOMAN			(FV_FLAG_START+183)		//長老
#define FV_T02R0101_OOKIDO				(FV_FLAG_START+184)		//オーキド博士
#define	FV_T06_SEVEN2					(FV_FLAG_START+185)		//5人衆
#define	FV_D16_GINGA					(FV_FLAG_START+186)		//ギンガ団1,2
#define	FV_D16_SEVEN2					(FV_FLAG_START+187)		//5人衆
#define	FV_D16_POLICE					(FV_FLAG_START+188)		//国際警察
#define	FV_D16R0101_GKANBU				(FV_FLAG_START+189)		//マーズ、ジュピター、プルート同じ
#define	FV_D16R0103_POLICE_23			(FV_FLAG_START+190)		//警察2,3
#define	FV_D16R0103_POLICE				(FV_FLAG_START+191)		//国際警察
#define	FV_D16R0103_POKE				(FV_FLAG_START+192)		//グレッグル
#define	FV_D16R0103_STONE				(FV_FLAG_START+193)		//隠れている岩
#define	FV_C09PC0101_LEADER				(FV_FLAG_START+194)		//スモモ
#define	FV_D05R0102_STOPPER				(FV_FLAG_START+195)		//テンガン山ストッパー
#define	FV_D26R0107_POKE				(FV_FLAG_START+196)		//レイアイハイ
#define	FV_D26R0107_GKANBU				(FV_FLAG_START+197)		//幹部
#define	FV_C01_CROWN3					(FV_FLAG_START+198)		//ピエロ(TV局前)
#define	FV_C08_LEADER					(FV_FLAG_START+199)		//四天王
#define	FV_C10R0112_CHAMP				(FV_FLAG_START+200)		//チャンピオン
#define	FV_C10R0112_DOCTOR				(FV_FLAG_START+201)		//ナナカマド
#define FV_C05_JUDGE					(FV_FLAG_START+202)		//ミミィ
#define FV_C05_POKE						(FV_FLAG_START+203)		//ミミット
#define FV_C05_LEADER					(FV_FLAG_START+204)		//ジムリーダー
#define FV_STOPPER_FRIENDBOOK			(FV_FLAG_START+205)		//ともだち手帳ストッパー
#define FV_D30R0101_SPPOKE7				(FV_FLAG_START+206)		//ダーク
#define FV_C02R0401_EVENT				(FV_FLAG_START+207)		//ダークのサングラス
#define FV_C04R0601_OOKIDO				(FV_FLAG_START+208)		//オーキド博士
#define FV_D20R0106_LEGEND				(FV_FLAG_START+209)		//レジキング
#define FV_L02R0101_COUPLE1				(FV_FLAG_START+210)		//レストラン　カップル１
#define FV_L02R0101_COUPLE2				(FV_FLAG_START+211)		//レストラン　カップル２
#define FV_L02R0101_COUPLE3				(FV_FLAG_START+212)		//レストラン　カップル３
#define FV_L02R0101_COUPLE4				(FV_FLAG_START+213)		//レストラン　カップル４
#define FV_L02R0101_COUPLE5				(FV_FLAG_START+214)		//レストラン　カップル５
#define FV_L02R0101_COUPLE6				(FV_FLAG_START+215)		//レストラン　カップル６
#define FV_L02R0101_COUPLE7				(FV_FLAG_START+216)		//レストラン　カップル７
#define FV_L02R0101_COUPLE8				(FV_FLAG_START+217)		//レストラン　カップル８
#define FV_L02R0101_COUPLE9				(FV_FLAG_START+218)		//レストラン　カップル９
#define FV_D24R0201_LEADER				(FV_FLAG_START+219)		//ジムリーダー　トウガン
#define	FV_D05R0116_LEGEND				(FV_FLAG_START+220)	//アウス
#define	FV_D15R0101_LEGEND				(FV_FLAG_START+221)	//ムーバス
#define	FV_D17R0104_LEGEND				(FV_FLAG_START+222)	//キマイラン
#define	FV_D18_LEGEND					(FV_FLAG_START+223)	//エウリス
#define	FV_R224_OOKIDO					(FV_FLAG_START+224)	//r224オーキド
#define	FV_D26R0109_BOSS				(FV_FLAG_START+225)	//アカギ
#define	FV_D26R0109_POLICE				(FV_FLAG_START+226)	//月曜トレーナー
#define	FV_C08_RIVAL					(FV_FLAG_START+227)		//ライバル
#define	FV_C09_CHAMP					(FV_FLAG_START+228)		//チャンピオン
#define	FV_R212AR0101_MAID				(FV_FLAG_START+229)	//メイド
#define	FV_R212AR0101_BOSS				(FV_FLAG_START+230)	//メイドボス
#define	FV_D26R0109_GINGA				(FV_FLAG_START+231)	//ギンガ団
#define	FV_T07R0201_KEEPER				(FV_FLAG_START+232)	//管理人
#define	FV_C02R0501_SEAMAN				(FV_FLAG_START+233)	//c02r0501船乗り
#define	FV_R224_LEGEND					(FV_FLAG_START+234)	//r224エウリス
#define	FV_L02_WOMAN2					(FV_FLAG_START+235)	//お洒落お姉さん
#define	FV_L02R0201_WOMAN2				(FV_FLAG_START+236)	//お洒落お姉さん
#define	FV_R217R0201_WOMAN1				(FV_FLAG_START+237)	//姉さん
#define FV_R212A_POLICE1_NPC			(FV_FLAG_START+238)		//お巡りさん(通常NPC)
#define FV_R212A_POLICE1_TR				(FV_FLAG_START+239)		//お巡りさん(対戦トレーナー)
#define FV_R212A_POLICE2_NPC			(FV_FLAG_START+240)		//お巡りさん(通常NPC)
#define FV_R212A_POLICE2_TR				(FV_FLAG_START+241)		//お巡りさん(対戦トレーナー)
#define FV_R212A_POLICE3_NPC			(FV_FLAG_START+242)		//お巡りさん(通常NPC)
#define FV_R212A_POLICE3_TR				(FV_FLAG_START+243)		//お巡りさん(対戦トレーナー)
#define FV_R212A_POLICE4_NPC			(FV_FLAG_START+244)		//お巡りさん(通常NPC)
#define FV_R212A_POLICE4_TR				(FV_FLAG_START+245)		//お巡りさん(対戦トレーナー)
#define FV_R212B_POLICE5_NPC			(FV_FLAG_START+246)		//お巡りさん(通常NPC)
#define FV_R212B_POLICE5_TR				(FV_FLAG_START+247)		//お巡りさん(対戦トレーナー)
#define FV_R222_POLICE6_NPC				(FV_FLAG_START+248)		//お巡りさん(通常NPC)
#define FV_R222_POLICE6_TR				(FV_FLAG_START+249)		//お巡りさん(対戦トレーナー)
#define FV_R209_JOGGER1_NPC				(FV_FLAG_START+250)		//ジョギング(通常NPC)
#define FV_R209_JOGGER1_TR				(FV_FLAG_START+251)		//ジョギング(対戦トレーナー)
#define FV_R209_JOGGER6_NPC				(FV_FLAG_START+252)		//ジョギング(通常NPC)
#define FV_R209_JOGGER6_TR				(FV_FLAG_START+253)		//ジョギング(対戦トレーナー)
#define FV_R210A_JOGGER4_NPC			(FV_FLAG_START+254)		//ジョギング(通常NPC)
#define FV_R210A_JOGGER4_TR				(FV_FLAG_START+255)		//ジョギング(対戦トレーナー)
#define FV_R215_JOGGER3_NPC				(FV_FLAG_START+256)		//ジョギング(通常NPC)
#define FV_R215_JOGGER3_TR				(FV_FLAG_START+257)		//ジョギング(対戦トレーナー)
#define FV_R215_JOGGER5_NPC				(FV_FLAG_START+258)		//ジョギング(通常NPC)
#define FV_R215_JOGGER5_TR				(FV_FLAG_START+259)		//ジョギング(対戦トレーナー)
#define FV_D31_TOWERBOSS				(FV_FLAG_START+260)		//タワータイクーン
#define FV_D31_RIVAL					(FV_FLAG_START+261)		//ライバル
#define	FV_D17R0104_ITEM				(FV_FLAG_START+262)		//d17r0104アイテムボール
#define	FV_D30_SEAMAN					(FV_FLAG_START+263)		//d30船員
#define	FV_C09_SEAMAN					(FV_FLAG_START+264)		//c09船員
#define	FV_D25R0102_OLDMAN2				(FV_FLAG_START+265)		//d25r0102幽霊じいちゃん
#define	FV_D25R0108_BABYGIRL1			(FV_FLAG_START+266)		//d25r0108幽霊女の子
#define	FV_L02_BIGMAN					(FV_FLAG_START+267)		//l02ストッパー
#define	FV_D09R0101_BIGMAN				(FV_FLAG_START+268)		//d09r0101ストッパー
#define	FV_R221_STOPPER1				(FV_FLAG_START+269)		//R221ストッパー1
#define	FV_R221_STOPPER2				(FV_FLAG_START+270)		//R221ストッパー2
#define	FV_C08R0801_LEADER				(FV_FLAG_START+271)		//ジムリーダー
#define	FV_C10R0103_DOOR_01				(FV_FLAG_START+272)		//ドア(手前)
#define	FV_C10R0105_DOOR_01				(FV_FLAG_START+273)		//ドア
#define	FV_C10R0107_DOOR_01				(FV_FLAG_START+274)		//ドア
#define	FV_C10R0109_DOOR_01				(FV_FLAG_START+275)		//ドア
#define	FV_C10R0103_DOOR_02				(FV_FLAG_START+276)		//ドア(奥)
#define	FV_C10R0105_DOOR_02				(FV_FLAG_START+277)		//ドア
#define	FV_C10R0107_DOOR_02				(FV_FLAG_START+278)		//ドア
#define	FV_C10R0109_DOOR_02				(FV_FLAG_START+279)		//ドア
#define	FV_C07_POLICE					(FV_FLAG_START+280)		//国際警察
#define	FV_R212AR0103_POLICEMAN			(FV_FLAG_START+281)		//警備員
#define FV_D27R0102_DOCTOR				(FV_FLAG_START+282)		//博士
#define FV_D27R0102_SUPPORT				(FV_FLAG_START+283)		//サポート
#define FV_R227_SEVEN2					(FV_FLAG_START+284)		//５人衆
#define FV_D05R0110_GINGAM				(FV_FLAG_START+285)		//ギンガ団したっぱストッパー
#define FV_C01R0206_STOP				(FV_FLAG_START+286)		//働くおじさんストッパー
#define FV_C01R0207_STOP				(FV_FLAG_START+287)		//働くおじさんストッパー
#define FV_D26R0101_GKANBU				(FV_FLAG_START+288)		//幹部
#define FV_D29R0101_RIVAL				(FV_FLAG_START+289)		//ライバル
#define FV_C11_STOP						(FV_FLAG_START+290)		//ストッパー(c11,w230で使用)
#define FV_D05R0112_GINGAW				(FV_FLAG_START+291)		//ギンガ団
#define FV_D27R0103_DOCTOR				(FV_FLAG_START+292)		//博士
#define FV_R217_LEADER					(FV_FLAG_START+293)		//スモモ
#define FV_C07GYM0101_LEADER			(FV_FLAG_START+294)		//スモモ
#define FV_R218_STOPPER					(FV_FLAG_START+295)		//ストッパー
#define FV_D05R0112_GINGAM				(FV_FLAG_START+296)		//ギンガ団ストッパー
#define FV_T05R0501_AKAGI				(FV_FLAG_START+297)		//カンナギ遺跡アカギ
#define FV_C05GYM0104_POLE				(FV_FLAG_START+298)		//メリッサ横のポール
#define FV_C06_POKE						(FV_FLAG_START+299)		//グレッグル
#define	FV_D26R0107_GKANBU2				(FV_FLAG_START+300)		//幹部(プルート)
#define	FV_D05R0102_POLICE				(FV_FLAG_START+301)		//国際警察
#define	FV_D17R0101_CHAMP				(FV_FLAG_START+302)		//シロナ
#define	FV_D11R0101_POKE				(FV_FLAG_START+303)		//ふれあい広場
#define	FV_T06R0101_TR1					(FV_FLAG_START+304)		//再戦施設
#define	FV_T06R0101_TR2					(FV_FLAG_START+305)		//再戦施設
#define	FV_T06R0101_TR3					(FV_FLAG_START+306)		//再戦施設
#define	FV_T06R0101_TR4					(FV_FLAG_START+307)		//再戦施設
#define	FV_T07_KEEPER					(FV_FLAG_START+308)		//別荘管理人
#define	FV_VILLA_IN_RIVAL				(FV_FLAG_START+309)		//別荘来客
#define	FV_VILLA_IN_SUPPORT				(FV_FLAG_START+310)		//別荘来客
#define	FV_VILLA_IN_DOCTOR				(FV_FLAG_START+311)		//別荘来客
#define	FV_VILLA_OUT_DOCTOR				(FV_FLAG_START+312)		//別荘来客
#define	FV_VILLA_IN_MAMA				(FV_FLAG_START+313)		//別荘来客
#define	FV_VILLA_IN_LEADER1				(FV_FLAG_START+314)		//別荘来客
#define	FV_VILLA_OUT_LEADER1			(FV_FLAG_START+315)		//別荘来客
#define	FV_VILLA_IN_LEADER2				(FV_FLAG_START+316)		//別荘来客
#define	FV_VILLA_OUT_LEADER2			(FV_FLAG_START+317)		//別荘来客
#define	FV_VILLA_IN_LEADER3				(FV_FLAG_START+318)		//別荘来客
#define	FV_VILLA_IN_LEADER4				(FV_FLAG_START+319)		//別荘来客
#define	FV_VILLA_OUT_LEADER4			(FV_FLAG_START+320)		//別荘来客
#define	FV_VILLA_IN_LEADER5				(FV_FLAG_START+321)		//別荘来客
#define	FV_VILLA_IN_LEADER6				(FV_FLAG_START+322)		//別荘来客
#define	FV_VILLA_OUT_LEADER6			(FV_FLAG_START+323)		//別荘来客
#define	FV_VILLA_IN_LEADER7				(FV_FLAG_START+324)		//別荘来客
#define	FV_VILLA_IN_LEADER8				(FV_FLAG_START+325)		//別荘来客
#define	FV_VILLA_IN_CHAMP				(FV_FLAG_START+326)		//別荘来客
#define	FV_VILLA_IN_BIGFOUR3			(FV_FLAG_START+327)		//別荘来客
#define	FV_D05R0115_SPPOKE2				(FV_FLAG_START+328)		//エムリット
#define	FV_ROTOMU_HOT					(FV_FLAG_START+329)		//ロトム
#define	FV_ROTOMU_WASH					(FV_FLAG_START+330)		//ロトム
#define	FV_ROTOMU_COLD					(FV_FLAG_START+331)		//ロトム
#define	FV_ROTOMU_FAN					(FV_FLAG_START+332)		//ロトム
#define	FV_ROTOMU_CUT					(FV_FLAG_START+333)		//ロトム
#define	FV_C04R0205_DOCTOR				(FV_FLAG_START+334)		//ナナカマド
#define	FV_TV_INTERVIEW_14				(FV_FLAG_START+335)		//TV(インタビュー)ステージ
#define	FV_TV_INTERVIEW_15				(FV_FLAG_START+336)		//TV(インタビュー)へんなアンケート
#define	FV_TV_INTERVIEW_19				(FV_FLAG_START+337)		//TV(インタビュー)フロンティア
#define	FV_T02_DOCTOR					(FV_FLAG_START+338)		//博士
#define	FV_T07R0201_DELIVERY			(FV_FLAG_START+339)		//別荘のデリバリー
#define	FV_C04R0201_DOOR				(FV_FLAG_START+340)		//ロトムへのドア
#define	FV_T07R0201_BOOK				(FV_FLAG_START+341)		//別荘の本
#define	FV_D01R0101_MYTH				(FV_FLAG_START+342)		//アルセウス山男
#define	FV_C02R0102_MYTH				(FV_FLAG_START+343)		//アルセウス山男
#define	FV_R224_MAI						(FV_FLAG_START+344)		//マイ
#define	FV_D32R0401_OUEN				(FV_FLAG_START+345)		//ステージ応援キャラ
#define	FV_C02_RIVAL_STOPPER			(FV_FLAG_START+346)		//ライバル　図書館ストッパー
#if 0
#define	FV_			(FV_FLAG_START+347)		//dummy
#define	FV_			(FV_FLAG_START+348)		//dummy
#define	FV_			(FV_FLAG_START+349)		//dummy
#define	FV_			(FV_FLAG_START+350)		//dummy
#define	FV_			(FV_FLAG_START+351)		//dummy
#define	FV_			(FV_FLAG_START+352)		//dummy
#define	FV_			(FV_FLAG_START+353)		//dummy
#define	FV_			(FV_FLAG_START+354)		//dummy
#define	FV_			(FV_FLAG_START+355)		//dummy
#define	FV_			(FV_FLAG_START+356)		//dummy
#define	FV_			(FV_FLAG_START+357)		//dummy
#define	FV_			(FV_FLAG_START+358)		//dummy
#endif

//FV_MAX = (FV_FLAG_START(400)+329)		//古い
//FV_MAX = (FV_FLAG_START(370)+359)		//08.01.18変更

//----------------------------------------------------------------------
//	隠しアイテムのフラグ(256+28個)
//
//	(FH_FLAG_START - FB_FLAG_START)
//	(    730       -   986+28=1014)
//----------------------------------------------------------------------
//
//

//----------------------------------------------------------------------
//	フィールド上のアイテムのフラグ(256+72個)
//
//	(FB_FLAG_START - ??_FLAG_START)
//	(986+28=1014   - 1014+256+72=1342)
//----------------------------------------------------------------------
#define FV_FLD_ITEM_0					(FB_FLAG_START+0)
#define FV_FLD_ITEM_1					(FB_FLAG_START+1)
#define FV_FLD_ITEM_2					(FB_FLAG_START+2)
#define FV_FLD_ITEM_3					(FB_FLAG_START+3)
#define FV_FLD_ITEM_4					(FB_FLAG_START+4)
#define FV_FLD_ITEM_5					(FB_FLAG_START+5)
#define FV_FLD_ITEM_6					(FB_FLAG_START+6)
#define FV_FLD_ITEM_7					(FB_FLAG_START+7)
#define FV_FLD_ITEM_8					(FB_FLAG_START+8)
#define FV_FLD_ITEM_9					(FB_FLAG_START+9)
#define FV_FLD_ITEM_10					(FB_FLAG_START+10)
#define FV_FLD_ITEM_11					(FB_FLAG_START+11)
#define FV_FLD_ITEM_12					(FB_FLAG_START+12)
#define FV_FLD_ITEM_13					(FB_FLAG_START+13)
#define FV_FLD_ITEM_14					(FB_FLAG_START+14)
#define FV_FLD_ITEM_15					(FB_FLAG_START+15)
#define FV_FLD_ITEM_16					(FB_FLAG_START+16)
#define FV_FLD_ITEM_17					(FB_FLAG_START+17)
#define FV_FLD_ITEM_18					(FB_FLAG_START+18)
#define FV_FLD_ITEM_19					(FB_FLAG_START+19)
#define FV_FLD_ITEM_20					(FB_FLAG_START+20)
#define FV_FLD_ITEM_21					(FB_FLAG_START+21)
#define FV_FLD_ITEM_22					(FB_FLAG_START+22)
#define FV_FLD_ITEM_23					(FB_FLAG_START+23)
#define FV_FLD_ITEM_24					(FB_FLAG_START+24)
#define FV_FLD_ITEM_25					(FB_FLAG_START+25)
#define FV_FLD_ITEM_26					(FB_FLAG_START+26)
#define FV_FLD_ITEM_27					(FB_FLAG_START+27)
#define FV_FLD_ITEM_28					(FB_FLAG_START+28)
#define FV_FLD_ITEM_29					(FB_FLAG_START+29)
#define FV_FLD_ITEM_30					(FB_FLAG_START+30)
#define FV_FLD_ITEM_31					(FB_FLAG_START+31)
#define FV_FLD_ITEM_32					(FB_FLAG_START+32)
#define FV_FLD_ITEM_33					(FB_FLAG_START+33)
#define FV_FLD_ITEM_34					(FB_FLAG_START+34)
#define FV_FLD_ITEM_35					(FB_FLAG_START+35)
#define FV_FLD_ITEM_36					(FB_FLAG_START+36)
#define FV_FLD_ITEM_37					(FB_FLAG_START+37)
#define FV_FLD_ITEM_38					(FB_FLAG_START+38)
#define FV_FLD_ITEM_39					(FB_FLAG_START+39)
#define FV_FLD_ITEM_40					(FB_FLAG_START+40)
#define FV_FLD_ITEM_41					(FB_FLAG_START+41)
#define FV_FLD_ITEM_42					(FB_FLAG_START+42)
#define FV_FLD_ITEM_43					(FB_FLAG_START+43)
#define FV_FLD_ITEM_44					(FB_FLAG_START+44)
#define FV_FLD_ITEM_45					(FB_FLAG_START+45)
#define FV_FLD_ITEM_46					(FB_FLAG_START+46)
#define FV_FLD_ITEM_47					(FB_FLAG_START+47)
#define FV_FLD_ITEM_48					(FB_FLAG_START+48)
#define FV_FLD_ITEM_49					(FB_FLAG_START+49)
#define FV_FLD_ITEM_50					(FB_FLAG_START+50)
#define FV_FLD_ITEM_51					(FB_FLAG_START+51)
#define FV_FLD_ITEM_52					(FB_FLAG_START+52)
#define FV_FLD_ITEM_53					(FB_FLAG_START+53)
#define FV_FLD_ITEM_54					(FB_FLAG_START+54)
#define FV_FLD_ITEM_55					(FB_FLAG_START+55)
#define FV_FLD_ITEM_56					(FB_FLAG_START+56)
#define FV_FLD_ITEM_57					(FB_FLAG_START+57)
#define FV_FLD_ITEM_58					(FB_FLAG_START+58)
#define FV_FLD_ITEM_59					(FB_FLAG_START+59)
#define FV_FLD_ITEM_60					(FB_FLAG_START+60)
#define FV_FLD_ITEM_61					(FB_FLAG_START+61)
#define FV_FLD_ITEM_62					(FB_FLAG_START+62)
#define FV_FLD_ITEM_63					(FB_FLAG_START+63)
#define FV_FLD_ITEM_64					(FB_FLAG_START+64)
#define FV_FLD_ITEM_65					(FB_FLAG_START+65)
#define FV_FLD_ITEM_66					(FB_FLAG_START+66)
#define FV_FLD_ITEM_67					(FB_FLAG_START+67)
#define FV_FLD_ITEM_68					(FB_FLAG_START+68)
#define FV_FLD_ITEM_69					(FB_FLAG_START+69)
#define FV_FLD_ITEM_70					(FB_FLAG_START+70)
#define FV_FLD_ITEM_71					(FB_FLAG_START+71)
#define FV_FLD_ITEM_72					(FB_FLAG_START+72)
#define FV_FLD_ITEM_73					(FB_FLAG_START+73)
#define FV_FLD_ITEM_74					(FB_FLAG_START+74)
#define FV_FLD_ITEM_75					(FB_FLAG_START+75)
#define FV_FLD_ITEM_76					(FB_FLAG_START+76)
#define FV_FLD_ITEM_77					(FB_FLAG_START+77)
#define FV_FLD_ITEM_78					(FB_FLAG_START+78)
#define FV_FLD_ITEM_79					(FB_FLAG_START+79)
#define FV_FLD_ITEM_80					(FB_FLAG_START+80)
#define FV_FLD_ITEM_81					(FB_FLAG_START+81)
#define FV_FLD_ITEM_82					(FB_FLAG_START+82)
#define FV_FLD_ITEM_83					(FB_FLAG_START+83)
#define FV_FLD_ITEM_84					(FB_FLAG_START+84)
#define FV_FLD_ITEM_85					(FB_FLAG_START+85)
#define FV_FLD_ITEM_86					(FB_FLAG_START+86)
#define FV_FLD_ITEM_87					(FB_FLAG_START+87)
#define FV_FLD_ITEM_88					(FB_FLAG_START+88)
#define FV_FLD_ITEM_89					(FB_FLAG_START+89)
#define FV_FLD_ITEM_90					(FB_FLAG_START+90)
#define FV_FLD_ITEM_91					(FB_FLAG_START+91)
#define FV_FLD_ITEM_92					(FB_FLAG_START+92)
#define FV_FLD_ITEM_93					(FB_FLAG_START+93)
#define FV_FLD_ITEM_94					(FB_FLAG_START+94)
#define FV_FLD_ITEM_95					(FB_FLAG_START+95)
#define FV_FLD_ITEM_96					(FB_FLAG_START+96)
#define FV_FLD_ITEM_97					(FB_FLAG_START+97)
#define FV_FLD_ITEM_98					(FB_FLAG_START+98)
#define FV_FLD_ITEM_99					(FB_FLAG_START+99)
#define FV_FLD_ITEM_100					(FB_FLAG_START+100)
#define FV_FLD_ITEM_101					(FB_FLAG_START+101)
#define FV_FLD_ITEM_102					(FB_FLAG_START+102)
#define FV_FLD_ITEM_103					(FB_FLAG_START+103)
#define FV_FLD_ITEM_104					(FB_FLAG_START+104)
#define FV_FLD_ITEM_105					(FB_FLAG_START+105)
#define FV_FLD_ITEM_106					(FB_FLAG_START+106)
#define FV_FLD_ITEM_107					(FB_FLAG_START+107)
#define FV_FLD_ITEM_108					(FB_FLAG_START+108)
#define FV_FLD_ITEM_109					(FB_FLAG_START+109)
#define FV_FLD_ITEM_110					(FB_FLAG_START+110)
#define FV_FLD_ITEM_111					(FB_FLAG_START+111)
#define FV_FLD_ITEM_112					(FB_FLAG_START+112)
#define FV_FLD_ITEM_113					(FB_FLAG_START+113)
#define FV_FLD_ITEM_114					(FB_FLAG_START+114)
#define FV_FLD_ITEM_115					(FB_FLAG_START+115)
#define FV_FLD_ITEM_116					(FB_FLAG_START+116)
#define FV_FLD_ITEM_117					(FB_FLAG_START+117)
#define FV_FLD_ITEM_118					(FB_FLAG_START+118)
#define FV_FLD_ITEM_119					(FB_FLAG_START+119)
#define FV_FLD_ITEM_120					(FB_FLAG_START+120)
#define FV_FLD_ITEM_121					(FB_FLAG_START+121)
#define FV_FLD_ITEM_122					(FB_FLAG_START+122)
#define FV_FLD_ITEM_123					(FB_FLAG_START+123)
#define FV_FLD_ITEM_124					(FB_FLAG_START+124)
#define FV_FLD_ITEM_125					(FB_FLAG_START+125)
#define FV_FLD_ITEM_126					(FB_FLAG_START+126)
#define FV_FLD_ITEM_127					(FB_FLAG_START+127)
#define FV_FLD_ITEM_128					(FB_FLAG_START+128)
#define FV_FLD_ITEM_129					(FB_FLAG_START+129)
#define FV_FLD_ITEM_130					(FB_FLAG_START+130)
#define FV_FLD_ITEM_131					(FB_FLAG_START+131)
#define FV_FLD_ITEM_132					(FB_FLAG_START+132)
#define FV_FLD_ITEM_133					(FB_FLAG_START+133)
#define FV_FLD_ITEM_134					(FB_FLAG_START+134)
#define FV_FLD_ITEM_135					(FB_FLAG_START+135)
#define FV_FLD_ITEM_136					(FB_FLAG_START+136)
#define FV_FLD_ITEM_137					(FB_FLAG_START+137)
#define FV_FLD_ITEM_138					(FB_FLAG_START+138)
#define FV_FLD_ITEM_139					(FB_FLAG_START+139)
#define FV_FLD_ITEM_140					(FB_FLAG_START+140)
#define FV_FLD_ITEM_141					(FB_FLAG_START+141)
#define FV_FLD_ITEM_142					(FB_FLAG_START+142)
#define FV_FLD_ITEM_143					(FB_FLAG_START+143)
#define FV_FLD_ITEM_144					(FB_FLAG_START+144)
#define FV_FLD_ITEM_145					(FB_FLAG_START+145)
#define FV_FLD_ITEM_146					(FB_FLAG_START+146)
#define FV_FLD_ITEM_147					(FB_FLAG_START+147)
#define FV_FLD_ITEM_148					(FB_FLAG_START+148)
#define FV_FLD_ITEM_149					(FB_FLAG_START+149)
#define FV_FLD_ITEM_150					(FB_FLAG_START+150)
#define FV_FLD_ITEM_151					(FB_FLAG_START+151)
#define FV_FLD_ITEM_152					(FB_FLAG_START+152)
#define FV_FLD_ITEM_153					(FB_FLAG_START+153)
#define FV_FLD_ITEM_154					(FB_FLAG_START+154)
#define FV_FLD_ITEM_155					(FB_FLAG_START+155)
#define FV_FLD_ITEM_156					(FB_FLAG_START+156)
#define FV_FLD_ITEM_157					(FB_FLAG_START+157)
#define FV_FLD_ITEM_158					(FB_FLAG_START+158)
#define FV_FLD_ITEM_159					(FB_FLAG_START+159)
#define FV_FLD_ITEM_160					(FB_FLAG_START+160)
#define FV_FLD_ITEM_161					(FB_FLAG_START+161)
#define FV_FLD_ITEM_162					(FB_FLAG_START+162)
#define FV_FLD_ITEM_163					(FB_FLAG_START+163)
#define FV_FLD_ITEM_164					(FB_FLAG_START+164)
#define FV_FLD_ITEM_165					(FB_FLAG_START+165)
#define FV_FLD_ITEM_166					(FB_FLAG_START+166)
#define FV_FLD_ITEM_167					(FB_FLAG_START+167)
#define FV_FLD_ITEM_168					(FB_FLAG_START+168)
#define FV_FLD_ITEM_169					(FB_FLAG_START+169)
#define FV_FLD_ITEM_170					(FB_FLAG_START+170)
#define FV_FLD_ITEM_171					(FB_FLAG_START+171)
#define FV_FLD_ITEM_172					(FB_FLAG_START+172)
#define FV_FLD_ITEM_173					(FB_FLAG_START+173)
#define FV_FLD_ITEM_174					(FB_FLAG_START+174)
#define FV_FLD_ITEM_175					(FB_FLAG_START+175)
#define FV_FLD_ITEM_176					(FB_FLAG_START+176)
#define FV_FLD_ITEM_177					(FB_FLAG_START+177)
#define FV_FLD_ITEM_178					(FB_FLAG_START+178)
#define FV_FLD_ITEM_179					(FB_FLAG_START+179)
#define FV_FLD_ITEM_180					(FB_FLAG_START+180)
#define FV_FLD_ITEM_181					(FB_FLAG_START+181)
#define FV_FLD_ITEM_182					(FB_FLAG_START+182)
#define FV_FLD_ITEM_183					(FB_FLAG_START+183)
#define FV_FLD_ITEM_184					(FB_FLAG_START+184)
#define FV_FLD_ITEM_185					(FB_FLAG_START+185)
#define FV_FLD_ITEM_186					(FB_FLAG_START+186)
#define FV_FLD_ITEM_187					(FB_FLAG_START+187)
#define FV_FLD_ITEM_188					(FB_FLAG_START+188)
#define FV_FLD_ITEM_189					(FB_FLAG_START+189)
#define FV_FLD_ITEM_190					(FB_FLAG_START+190)
#define FV_FLD_ITEM_191					(FB_FLAG_START+191)
#define FV_FLD_ITEM_192					(FB_FLAG_START+192)
#define FV_FLD_ITEM_193					(FB_FLAG_START+193)
#define FV_FLD_ITEM_194					(FB_FLAG_START+194)
#define FV_FLD_ITEM_195					(FB_FLAG_START+195)
#define FV_FLD_ITEM_196					(FB_FLAG_START+196)
#define FV_FLD_ITEM_197					(FB_FLAG_START+197)
#define FV_FLD_ITEM_198					(FB_FLAG_START+198)
#define FV_FLD_ITEM_199					(FB_FLAG_START+199)
#define FV_FLD_ITEM_200					(FB_FLAG_START+200)
#define FV_FLD_ITEM_201					(FB_FLAG_START+201)
#define FV_FLD_ITEM_202					(FB_FLAG_START+202)
#define FV_FLD_ITEM_203					(FB_FLAG_START+203)
#define FV_FLD_ITEM_204					(FB_FLAG_START+204)
#define FV_FLD_ITEM_205					(FB_FLAG_START+205)
#define FV_FLD_ITEM_206					(FB_FLAG_START+206)
#define FV_FLD_ITEM_207					(FB_FLAG_START+207)
#define FV_FLD_ITEM_208					(FB_FLAG_START+208)
#define FV_FLD_ITEM_209					(FB_FLAG_START+209)
#define FV_FLD_ITEM_210					(FB_FLAG_START+210)
#define FV_FLD_ITEM_211					(FB_FLAG_START+211)
#define FV_FLD_ITEM_212					(FB_FLAG_START+212)
#define FV_FLD_ITEM_213					(FB_FLAG_START+213)
#define FV_FLD_ITEM_214					(FB_FLAG_START+214)
#define FV_FLD_ITEM_215					(FB_FLAG_START+215)
#define FV_FLD_ITEM_216					(FB_FLAG_START+216)
#define FV_FLD_ITEM_217					(FB_FLAG_START+217)
#define FV_FLD_ITEM_218					(FB_FLAG_START+218)
#define FV_FLD_ITEM_219					(FB_FLAG_START+219)
#define FV_FLD_ITEM_220					(FB_FLAG_START+220)
#define FV_FLD_ITEM_221					(FB_FLAG_START+221)
#define FV_FLD_ITEM_222					(FB_FLAG_START+222)
#define FV_FLD_ITEM_223					(FB_FLAG_START+223)
#define FV_FLD_ITEM_224					(FB_FLAG_START+224)
#define FV_FLD_ITEM_225					(FB_FLAG_START+225)
#define FV_FLD_ITEM_226					(FB_FLAG_START+226)
#define FV_FLD_ITEM_227					(FB_FLAG_START+227)
#define FV_FLD_ITEM_228					(FB_FLAG_START+228)
#define FV_FLD_ITEM_229					(FB_FLAG_START+229)
#define FV_FLD_ITEM_230					(FB_FLAG_START+230)
#define FV_FLD_ITEM_231					(FB_FLAG_START+231)
#define FV_FLD_ITEM_232					(FB_FLAG_START+232)
#define FV_FLD_ITEM_233					(FB_FLAG_START+233)
#define FV_FLD_ITEM_234					(FB_FLAG_START+234)
#define FV_FLD_ITEM_235					(FB_FLAG_START+235)
#define FV_FLD_ITEM_236					(FB_FLAG_START+236)
#define FV_FLD_ITEM_237					(FB_FLAG_START+237)
#define FV_FLD_ITEM_238					(FB_FLAG_START+238)
#define FV_FLD_ITEM_239					(FB_FLAG_START+239)
#define FV_FLD_ITEM_240					(FB_FLAG_START+240)
#define FV_FLD_ITEM_241					(FB_FLAG_START+241)
#define FV_FLD_ITEM_242					(FB_FLAG_START+242)
#define FV_FLD_ITEM_243					(FB_FLAG_START+243)
#define FV_FLD_ITEM_244					(FB_FLAG_START+244)
#define FV_FLD_ITEM_245					(FB_FLAG_START+245)
#define FV_FLD_ITEM_246					(FB_FLAG_START+246)
#define FV_FLD_ITEM_247					(FB_FLAG_START+247)
#define FV_FLD_ITEM_248					(FB_FLAG_START+248)
#define FV_FLD_ITEM_249					(FB_FLAG_START+249)
#define FV_FLD_ITEM_250					(FB_FLAG_START+250)
#define FV_FLD_ITEM_251					(FB_FLAG_START+251)
#define FV_FLD_ITEM_252					(FB_FLAG_START+252)
#define FV_FLD_ITEM_253					(FB_FLAG_START+253)
#define FV_FLD_ITEM_254					(FB_FLAG_START+254)
#define FV_FLD_ITEM_255					(FB_FLAG_START+255)
#define FV_FLD_ITEM_256					(FB_FLAG_START+256)		//プラチナ追加
#define FV_FLD_ITEM_257					(FB_FLAG_START+257)
#define FV_FLD_ITEM_258					(FB_FLAG_START+258)
#define FV_FLD_ITEM_259					(FB_FLAG_START+259)
#define FV_FLD_ITEM_260					(FB_FLAG_START+260)
#define FV_FLD_ITEM_261					(FB_FLAG_START+261)
#define FV_FLD_ITEM_262					(FB_FLAG_START+262)
#define FV_FLD_ITEM_263					(FB_FLAG_START+263)
#define FV_FLD_ITEM_264					(FB_FLAG_START+264)
#define FV_FLD_ITEM_265					(FB_FLAG_START+265)
#define FV_FLD_ITEM_266					(FB_FLAG_START+266)
#define FV_FLD_ITEM_267					(FB_FLAG_START+267)
#define FV_FLD_ITEM_268					(FB_FLAG_START+268)
#define FV_FLD_ITEM_269					(FB_FLAG_START+269)
#define FV_FLD_ITEM_270					(FB_FLAG_START+270)
#define FV_FLD_ITEM_271					(FB_FLAG_START+271)
#define FV_FLD_ITEM_272					(FB_FLAG_START+272)
#define FV_FLD_ITEM_273					(FB_FLAG_START+273)
#define FV_FLD_ITEM_274					(FB_FLAG_START+274)
#define FV_FLD_ITEM_275					(FB_FLAG_START+275)
#define FV_FLD_ITEM_276					(FB_FLAG_START+276)
#define FV_FLD_ITEM_277					(FB_FLAG_START+277)
#define FV_FLD_ITEM_278					(FB_FLAG_START+278)
#define FV_FLD_ITEM_279					(FB_FLAG_START+279)
#define FV_FLD_ITEM_280					(FB_FLAG_START+280)
#define FV_FLD_ITEM_281					(FB_FLAG_START+281)
#define FV_FLD_ITEM_282					(FB_FLAG_START+282)
#define FV_FLD_ITEM_283					(FB_FLAG_START+283)
#define FV_FLD_ITEM_284					(FB_FLAG_START+284)
#define FV_FLD_ITEM_285					(FB_FLAG_START+285)
#define FV_FLD_ITEM_286					(FB_FLAG_START+286)
#define FV_FLD_ITEM_287					(FB_FLAG_START+287)
#define FV_FLD_ITEM_288					(FB_FLAG_START+288)
#define FV_FLD_ITEM_289					(FB_FLAG_START+289)
#define FV_FLD_ITEM_290					(FB_FLAG_START+290)
#define FV_FLD_ITEM_291					(FB_FLAG_START+291)
#define FV_FLD_ITEM_292					(FB_FLAG_START+292)
#define FV_FLD_ITEM_293					(FB_FLAG_START+293)
#define FV_FLD_ITEM_294					(FB_FLAG_START+294)
#define FV_FLD_ITEM_295					(FB_FLAG_START+295)
#define FV_FLD_ITEM_296					(FB_FLAG_START+296)
#define FV_FLD_ITEM_297					(FB_FLAG_START+297)
#define FV_FLD_ITEM_298					(FB_FLAG_START+298)
#define FV_FLD_ITEM_299					(FB_FLAG_START+299)
#define FV_FLD_ITEM_300					(FB_FLAG_START+300)
#define FV_FLD_ITEM_301					(FB_FLAG_START+301)
#define FV_FLD_ITEM_302					(FB_FLAG_START+302)
#define FV_FLD_ITEM_303					(FB_FLAG_START+303)
#define FV_FLD_ITEM_304					(FB_FLAG_START+304)
#define FV_FLD_ITEM_305					(FB_FLAG_START+305)
#define FV_FLD_ITEM_306					(FB_FLAG_START+306)
#define FV_FLD_ITEM_307					(FB_FLAG_START+307)
#define FV_FLD_ITEM_308					(FB_FLAG_START+308)
#define FV_FLD_ITEM_309					(FB_FLAG_START+309)
#define FV_FLD_ITEM_310					(FB_FLAG_START+310)
#define FV_FLD_ITEM_311					(FB_FLAG_START+311)
#define FV_FLD_ITEM_312					(FB_FLAG_START+312)
#define FV_FLD_ITEM_313					(FB_FLAG_START+313)
#define FV_FLD_ITEM_314					(FB_FLAG_START+314)
#define FV_FLD_ITEM_315					(FB_FLAG_START+315)
#define FV_FLD_ITEM_316					(FB_FLAG_START+316)
#define FV_FLD_ITEM_317					(FB_FLAG_START+317)
#define FV_FLD_ITEM_318					(FB_FLAG_START+318)
#define FV_FLD_ITEM_319					(FB_FLAG_START+319)
#define FV_FLD_ITEM_320					(FB_FLAG_START+320)
#define FV_FLD_ITEM_321					(FB_FLAG_START+321)
#define FV_FLD_ITEM_322					(FB_FLAG_START+322)
#define FV_FLD_ITEM_323					(FB_FLAG_START+323)
#define FV_FLD_ITEM_324					(FB_FLAG_START+324)
#define FV_FLD_ITEM_325					(FB_FLAG_START+325)
#define FV_FLD_ITEM_326					(FB_FLAG_START+326)
#define FV_FLD_ITEM_327					(FB_FLAG_START+327)

//FVF_MAX = (FB_FLAG_START(1014)+328)

//----------------------------------------------------------------------
//	１日１回クリアされるフラグ(192個)
//
//	※スクリプトのみでセットする＆個数が少ないのでsaveflag.hで同居
// 	(TIMEFLAG_START - )
//----------------------------------------------------------------------
// TIMEFLAG_START はevwkdef.hで定義
#define TMFLG_SEED_PRESENT			(TIMEFLAG_START+0)		/* タネのプレゼント */
#define TMFLG_T05PC0101_ITEM_GET	(TIMEFLAG_START+1)		/* アイテムプレゼント */
#define TMFLG_URANIWA_ENCOUNT		(TIMEFLAG_START+2)		/* 裏庭エンカウント用 */
#define TMFLG_C06R0201_SEED_PRESENT	(TIMEFLAG_START+3)		/* タネのプレゼント */
#define TMFLG_C08R0201_RIBBON_GET	(TIMEFLAG_START+4)		/* リボンをゲット */
#define TMFLG_T07R0101_ESTHTIQUE	(TIMEFLAG_START+5)		/* エステを受けた */
#define TMFLG_C08R0201_WEEK_TALK	(TIMEFLAG_START+6)		/* おねさんと会話した */
#define TMFLG_C08R0201_FIRST_DAY	(TIMEFLAG_START+7)		/* おねさんと会話した1日目 */
#define TMFLG_D02_FUUSEN			(TIMEFLAG_START+8)		/* フーセン */
#define TMFLG_KUJI_DAY				(TIMEFLAG_START+9)		/* くじを今日はやった */
#define TMFLG_T03R0101_SEED_PRESENT	(TIMEFLAG_START+10)		/* タネのプレゼント */
#define TMFLG_L02R0101_INIT			(TIMEFLAG_START+11)		/* レストラン初期化用 */
#define TMFLG_C07R0801_DRESSING		(TIMEFLAG_START+12)		/* お洒落お姉さん */
#define TMFLG_C09R0201_NANKAI		(TIMEFLAG_START+13)		/* 簡易会話：難解言葉教え */
#define TMFLG_R221R0201_ITEM_GET	(TIMEFLAG_START+14)		/* アイテム入手 */
//#define TMFLG_C01R0201_BATTLE_END	(TIMEFLAG_START+15)		/* 曜日毎に変わるトレーナとの対戦終了*/
#define TMFLG_D25R0106_SPPOKE_END	(TIMEFLAG_START+16)		/* d25r0106プラズマ挑戦終了 */
#define TMFLG_T04R0301_NEWS_END		(TIMEFLAG_START+17)		/* 今日の依頼終了 */
#define TMFLG_T06_RIVAL_BTL			(TIMEFLAG_START+18)		/* t06ライバル戦闘 */
#define TMFLG_KUJI_ITEMGET			(TIMEFLAG_START+19)		/* くじで今日のアイテムゲットした */
#define TMFLG_D06R0206_ITEM_GET		(TIMEFLAG_START+20)		/* かけらもらえる */
#define TMFLG_D11R0101_ITEM_GET		(TIMEFLAG_START+21)		/* 公園おじさん */
#define TMFLG_R212AR0101_MAID		(TIMEFLAG_START+22)		/* メイド勝ち抜きのターン数変更 */
#define TMFLG_T06R0101_REMATCH		(TIMEFLAG_START+23)		/* 再戦施設 */
#define TMFLG_T06R0101_WIN_TR1		(TIMEFLAG_START+24)		/* 再戦施設 TR1 勝利したか */
#define TMFLG_T06R0101_WIN_TR2		(TIMEFLAG_START+25)		/* 再戦施設 TR2 勝利したか */
#define TMFLG_T06R0101_WIN_TR3		(TIMEFLAG_START+26)		/* 再戦施設 TR3 勝利したか */
#define TMFLG_T06R0101_WIN_TR4		(TIMEFLAG_START+27)		/* 再戦施設 TR4 勝利したか */
#define TMFLG_DOSA_TR1_WIN			(TIMEFLAG_START+28)		/* ドサ周り TR1 勝利したか */
#define TMFLG_DOSA_TR2_WIN			(TIMEFLAG_START+29)		/* ドサ周り TR2 勝利したか */
#define TMFLG_DOSA_TR3_WIN			(TIMEFLAG_START+30)		/* ドサ周り TR3 勝利したか */
#define TMFLG_DOSA_TR4_WIN			(TIMEFLAG_START+31)		/* ドサ周り TR4 勝利したか */
#define TMFLG_DOSA_TR5_WIN			(TIMEFLAG_START+32)		/* ドサ周り TR5 勝利したか */
#define TMFLG_DOSA_TR6_WIN			(TIMEFLAG_START+33)		/* ドサ周り TR6 勝利したか */
#define TMFLG_DOSA_TR7_WIN			(TIMEFLAG_START+34)		/* ドサ周り TR7 勝利したか */
#define TMFLG_BACKGROUND_ITEMGET	(TIMEFLAG_START+35)		/* 背景姉さん */
#define TMFLG_STAGE_OUEN			(TIMEFLAG_START+36)		/* 応援　選出したか */
#define TMFLG_T06R0101_WIN_TR5		(TIMEFLAG_START+37)		/* 再戦施設 TR5 勝利したか */
#define TMFLG_T06R0101_WIN_TR6		(TIMEFLAG_START+38)		/* 再戦施設 TR6 勝利したか */
#define TMFLG_T06R0101_WIN_TR7		(TIMEFLAG_START+39)		/* 再戦施設 TR7 勝利したか */
#define TMFLG_T06R0101_WIN_TR8		(TIMEFLAG_START+40)		/* 再戦施設 TR8 勝利したか */
#define TMFLG_T06R0101_WIN_TR9		(TIMEFLAG_START+41)		/* 再戦施設 TR9 勝利したか */
#define TMFLG_T06R0101_WIN_TR10		(TIMEFLAG_START+42)		/* 再戦施設 TR10 勝利したか */
#define TMFLG_T06R0101_WIN_TR11		(TIMEFLAG_START+43)		/* 再戦施設 TR11 勝利したか */
#define TMFLG_T06R0101_WIN_TR12		(TIMEFLAG_START+44)		/* 再戦施設 TR12 勝利したか */
#define TMFLG_T06R0101_WIN_TR13		(TIMEFLAG_START+45)		/* 再戦施設 TR13 勝利したか */


//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
#endif	//__SAVEFLAG_H__

