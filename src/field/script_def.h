//============================================================================================
/**
 * @file	script_def.h
 * @bfief	スクリプト定義ファイル
 * @author	Satoshi Nohara
 * @date	05.12.14
 */
//============================================================================================
#ifndef SCRIPT_DEF_H
#define SCRIPT_DEF_H


//============================================================================================
//
//	定義
//
//============================================================================================
//特殊スクリプト定義
#define SP_SCRID_NONE				(0)
#define SP_SCRID_SCENE_CHANGE		(1)
#define SP_SCRID_FLAG_CHANGE		(2)
#define SP_SCRID_OBJ_CHANGE			(3)			//注意！　フィールド初期化で呼ばれる
#define SP_SCRID_INIT_CHANGE		(4)

//エイリアス識別用スクリプトID
#define SP_SCRID_ALIES				(0xffff)

//トレーナータイプ定義
#define SCR_EYE_TR_TYPE_SINGLE		(0)
#define SCR_EYE_TR_TYPE_DOUBLE		(1)
#define SCR_EYE_TR_TYPE_TAG			(2)

//トレーナーの番号定義
#define SCR_EYE_TR_0				(0)
#define SCR_EYE_TR_1				(1)

//fld_menu.c
#define SCR_REPORT_OK				(0)			//セーブした
#define SCR_REPORT_CANCEL			(1)			//セーブしていない

//連れ歩きOBJを取得するOBJID定義
#define SCR_OBJID_MV_PAIR			(0xf2)

//透明ダミーOBJを取得するOBJID定義
#define SCR_OBJID_DUMMY				(0xf1)

//フラグ操作関数のモード指定用
#define	FLAG_MODE_RESET				(0)
#define	FLAG_MODE_SET				(1)
#define	FLAG_MODE_GET				(2)

//エレベータアニメ
#define T07R0103_ELEVATOR_LOOP		(4)
#define C05R0103_ELEVATOR_LOOP		(4)
#define C05R0803_ELEVATOR_LOOP		(4)
#define C07R0206_ELEVATOR_LOOP		(4)
#define C08R0802_ELEVATOR_LOOP		(4)
#define C01R0208_ELEVATOR_LOOP		(4)

//曜日取得関数の戻り値
#define RTC_WEEK_SUNDAY		0
#define RTC_WEEK_MONDAY		1
#define RTC_WEEK_TUESDAY	2
#define RTC_WEEK_WEDNESDAY	3
#define RTC_WEEK_THURSDAY	4
#define RTC_WEEK_FRIDAY		5
#define RTC_WEEK_SATURDAY	6

//図鑑モード
#define SCR_SHINOU_ZUKAN_MODE	0
#define SCR_ZENKOKU_ZUKAN_MODE	1

//パソコンアニメ管理ナンバー
#define SCR_PASO_ANM_NO		90

//別荘イベントナンバー
#define T07R0201_EVENT_RIVAL		0
#define T07R0201_EVENT_SUPPORT		1
#define T07R0201_EVENT_DOCTOR		2
#define T07R0201_EVENT_MAMA			3
#define T07R0201_EVENT_LE1			4
#define T07R0201_EVENT_LE2			5
#define T07R0201_EVENT_LE3			6
#define T07R0201_EVENT_LE4			7
#define T07R0201_EVENT_LE5			8
#define T07R0201_EVENT_LE6			9
#define T07R0201_EVENT_LE7			10
#define T07R0201_EVENT_LE8			11
#define T07R0201_EVENT_CHAMP		12
#define T07R0201_EVENT_BIG_LE		13
#define T07R0201_EVENT_LE_LE		14
#define T07R0201_EVENT_EVENT_MAX	15

//技教え：3人いて、それぞれの色の技しか教えてくれない
#define WAZA_OSHIE_COL_BLUE			(0)
#define WAZA_OSHIE_COL_RED			(1)
#define WAZA_OSHIE_COL_YELLOW		(2)

//技覚え共通ウェイト
#define WAZA_COMMON_WAIT			(30)
#define WAZA_COMMON_WAIT2			(32)
#define WAZA_COMMON_WAIT3			(16)

//レジイベントワークの定義(踏む場所の数以上の適当な値)
//#define REZI_WORK_2_ALL_NUM			(7)					//r0102床の数
//#define REZI_WORK_4_ALL_NUM			(7)					//r0104床の数
//#define REZI_WORK_6_ALL_NUM			(7)					//r0106床の数
#define REZI_WORK_2_ALL_NUM			(127)					//r0102床の数(111 1111)
#define REZI_WORK_4_ALL_NUM			(127)					//r0104床の数(111 1111)
#define REZI_WORK_6_ALL_NUM			(127)					//r0106床の数(111 1111)
#define REZI_WORK_POS_OK			(260)				//全て踏んだ
#define REZI_WORK_TALK_OK			(270)				//一度話しかけて戦闘いける状態
#define REZI_WORK_CAPTURE_NG		(280)				//捕獲失敗
#define REZI_WORK_CAPTURE_OK		(290)				//捕獲成功


#endif	/* SCRIPT_DEF_H */


