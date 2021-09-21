//==============================================================================
/**
 * @file	guru2_comm.h
 * @brief	ぐるぐる交換　通信処理
 * @author	kagaya
 * @data	05.07.13
 */
//==============================================================================
#ifndef GURU2_COMM_FILE
#define GURU2_COMM_FILE

#include "communication/comm_command.h"
#include "../../field/comm_command_field.h"

#include "communication/communication.h"
#include "communication/comm_state.h"
#include "communication/comm_save.h"
#include "communication/wm_icon.h"
#include "field/comm_union_beacon.h"
#include "field/comm_union_view_common.h"

//==============================================================================
//	define
//==============================================================================
#define COMM_GURU2_TIMINGSYNC_NO (202)

//--------------------------------------------------------------
///	通信人数
//--------------------------------------------------------------
enum
{
	G2MEMBER_0,
	G2MEMBER_1,
	G2MEMBER_2,
	G2MEMBER_3,
	G2MEMBER_4,
	G2MEMBER_MAX,
};

#define GURU2_FRIEND_MAX (G2MEMBER_MAX-1)	///<相手最大

//--------------------------------------------------------------
///	通信コマンド
//--------------------------------------------------------------
enum
{
	G2COMM_RC_STOP,			///<乱入により停止
	G2COMM_RC_RESTART,		///<乱入終了、再度開始
	G2COMM_RC_END_CHILD,	///<子機離脱
	G2COMM_RC_END,			///<終了
	G2COMM_RC_CHILD_JOIN,	///<子機乱入開始
	G2COMM_RC_START,		///<開始
	G2COMM_RC_DATA,			///<データ転送
	G2COMM_RC_BAN,			///<離脱禁止・許可
	
	G2COMM_GM_SIGNAL,		///<ゲーム　シグナル
	G2COMM_GM_JOIN,			///<ゲーム　参加
	G2COMM_GM_BTN,			///<ゲーム　ボタン情報
	G2COMM_GM_GAMEDATA,		///<ゲーム　情報
	G2COMM_GM_PLAYNO,		///<ゲーム　プレイ番号
	G2COMM_GM_PLAYMAX,		///<ゲーム　プレイ最大数
	G2COMM_GM_TRADE_POS,	///<ゲーム　交換位置
	G2COMM_GM_GAMERESULT,	///<ゲーム　結果
	G2COMM_GM_EGG_DATA_NG,	///<ゲーム　卵データ異常
	G2COMM_GM_EGG_DATA_OK,	///<ゲーム　卵データ正常
	
	G2COMM_MAX,				///<最大
};

//--------------------------------------------------------------
///	通信ワーク
//--------------------------------------------------------------
#define GURU2_WIDEUSE_SENDWORK_SIZE (24)	///汎用送信データ最大

//--------------------------------------------------------------
///	受付
//--------------------------------------------------------------
enum
{
	CREC_REQ_RIDATU_CHECK,		///<離脱確認
	CREC_REQ_RIDATU_EXE,		///<離脱実行
};

//--------------------------------------------------------------
///	ポケモンセレクト　親終了フラグ
//--------------------------------------------------------------
enum
{
	G2C_OYA_END_FLAG_NON = 0,
	G2C_OYA_END_FLAG_EXE,
	G2C_OYA_END_FLAG_END,
};

//--------------------------------------------------------------
///	ゲームシグナル
//--------------------------------------------------------------
#define G2COMM_GMSBIT_JOIN_CLOSE	(1<<0) ///<ゲーム参加締め切り
#define G2COMM_GMSBIT_CANCEL		(1<<1) ///<ゲームキャンセル
#define G2COMM_GMSBIT_GAME_START	(1<<2) ///<ゲーム開始
#define G2COMM_GMSBIT_EGG_ADD_START (1<<3) ///<タマゴ追加開始

#define G2COMM_GAMEBIT_DISC_STOP	(1<<0) ///<ゲーム　皿停止
#define G2COMM_GAMEBIT_BTN_STOP		(1<<1) ///<ゲーム　ボタン入力禁止

#define GURU2COMM_BAN_NONE		(0)
#define GURU2COMM_BAN_ON		(1)
#define GURU2COMM_BAN_MOST_ON	(2)

#define GURU2_COMM_SYNCRO_0 (201)	///<通信同期 0

#define GURU2COMM_RC_SEND_DATABUF_SIZE	(3000)		///<送信データサイズ

// ポケモンパーティーを送信するときのデータ量
#define POKEPARTY_SEND_ONCE_SIZE (236*6+4*2)	// sizeof(POKEPARTY)

//==============================================================================
//	typedef
//==============================================================================
//--------------------------------------------------------------
//	参照用
//--------------------------------------------------------------
typedef struct _TAG_GURU2PROC_WORK GURU2PROC_WORK;

//--------------------------------------------------------------
///	GURU2COMM_END_CHILD_WORK
///	この構造体の中身を替えたら、必ず通信関数テーブルで
///	使用している箇所の送受信サイズを変更すること
//--------------------------------------------------------------
typedef struct
{
	u8 ridatu_id;			///<離脱者のID
	u8 oya_share_num;		///<親の持つshareNum
	u8 request;				///<命令コード
	u8 ridatu_kyoka;		///<TRUE:離脱OK、FALSE:離脱NG
}GURU2COMM_END_CHILD_WORK;

//--------------------------------------------------------------
///	GURU2COMM_RC_SENDDATA
//--------------------------------------------------------------
typedef struct
{
	u8  data[GURU2COMM_RC_SEND_DATABUF_SIZE];	//レコード交換データ
	u32 _xor;						//データ整合性確認用XOR
	u32 seed;	//親から来たこの数値を元に乱数を発生させてデータを入れ替える
}GURU2COMM_RC_SENDDATA;

//--------------------------------------------------------------
///	GURU2COMM_GAMEDATA
//--------------------------------------------------------------
typedef struct
{
	u16 oya_flag;
	u16 game_time;
	s16 disc_speed;
	u16 disc_angle;
}GURU2COMM_GAMEDATA;

//--------------------------------------------------------------
///	GURU2COMM_GAMERESULT
//--------------------------------------------------------------
typedef struct
{
	u16 disc_angle;					//皿角度
	u16 omake_bit;					//おまけエリアヒットビット
	u16 egg_angle[G2MEMBER_MAX];	//卵角度
	u16 dmy;
}GURU2COMM_GAMERESULT;

//--------------------------------------------------------------
///	GURU2COMM_PLAYNO
//--------------------------------------------------------------
typedef struct
{
	u16 play_no;
	u16 comm_id;
}GURU2COMM_PLAYNO;

//--------------------------------------------------------------
///	PSEL_OYAEND_WORK
//--------------------------------------------------------------
typedef struct
{
	int seq_no;
}GURU2COMM_PSEL_OYAEND_TCB_WORK;

//--------------------------------------------------------------
///	GURU2COMM_KO_CANCEL_WORK
//--------------------------------------------------------------
typedef struct
{
	u8 cancel_id;
	u8 cancel_flag;
	u16 dmy;
}GURU2COMM_CANCEL_ANSWER;

//--------------------------------------------------------------
///	GURU2COMM_WORK
//--------------------------------------------------------------
typedef struct _TAG_GURU2COMM_WORK
{
	GURU2PROC_WORK *g2p;
	u8 wideuse_sendwork[GURU2_WIDEUSE_SENDWORK_SIZE]; //転送用ワーク
	
	int send_stop_flag;		//送信停止
	int recv_count;
	int record_execute;
	int banFlag;			//操作禁止フラグ
	int	shareNum;
	u32	ridatu_bit;			//離脱しようとしている子のbit
	int send_num;
	u16	oya_share_num;		//親が持っているshareNumをもらう
	
	int comm_psel_oya_end_flag;		//ポケモンセレクト中の親終了フラグ
	u16 comm_game_signal_bit;		//ゲームシグナル
	u16 comm_game_join_bit;			//ゲーム参加ビット
	u16 comm_game_cancel_bit;		//ゲームキャンセル
	s16 comm_game_egg_recv_bit;		//タマゴデータ受信ビット
	u16 comm_game_egg_check_error_bit;
	u16 comm_game_egg_check_ok_bit;
	u8 *recv_poke_party_buf;//[G2MEMBER_MAX][POKEPARTY_SEND_ONCE_SIZE]
	u8 *friend_poke_party_buf;//[G2MEMBER_MAX][POKEPARTY_SEND_ONCE_SIZE]
	
	GURU2COMM_PSEL_OYAEND_TCB_WORK pselend_work;
}GURU2COMM_WORK;

//==============================================================================
//	extern
//==============================================================================
extern GURU2COMM_WORK * Guru2Comm_WorkInit(GURU2PROC_WORK *g2p, u32 heap_id);
extern void Guru2Comm_WorkDelete( GURU2COMM_WORK *g2c );
extern void Guru2Comm_CommandInit( GURU2COMM_WORK *g2c );
extern void Guru2Comm_CommUnionRoomChange( GURU2COMM_WORK *g2c );

extern void * Guru2Comm_WideUseSendWorkGet( GURU2COMM_WORK *g2c, u32 size );

extern BOOL Guru2Comm_SendData(
	GURU2COMM_WORK *g2c, u32 code, const void *data, u32 size );

extern int Guru2Comm_OnlineNumGet( void );
extern void * Guru2Comm_FriendPokePartyGet( GURU2COMM_WORK *g2c, int id );

#endif //GURU2_COMM_FILE
