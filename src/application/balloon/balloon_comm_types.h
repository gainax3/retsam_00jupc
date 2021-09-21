//==============================================================================
/**
 * @file	balloon_comm_types.h
 * @brief	風船ミニゲームで通信しあうデータ型の定義
 * @author	matsuda
 * @date	2007.11.26(月)
 */
//==============================================================================
#ifndef __BALLOON_COMM_TYPES_H__
#define __BALLOON_COMM_TYPES_H__


//==============================================================================
//	定数定義
//==============================================================================
///命令コード
enum{
	ORDER_CODE_NULL,		///<空き
	
	ORDER_CODE_AIR,
	ORDER_CODE_EXPLODED,
	ORDER_CODE_APPEAR,

	ORDER_CODE_START,		///<ミニゲーム開始
	ORDER_CODE_FINISH,		///<ミニゲーム終了
	ORDER_CODE_TIMING,		///<同期待ち

	ORDER_CODE_TOUCHPEN_DEMO,	///<タッチペンデモ
	
	ORDER_CODE_MAX,			///<命令コード終端
};

///送受信バッファのチェーン数(貯めておける数)
#define SIO_BUFFER_CHAIN_MAX		(60)


///同期取りリクエストコマンド
enum{
	TIMING_REQ_NULL,				///<リクエストなし
	//----------------------------
	
	TIMING_REQ_NORMAL,				///<即同期取り

	TIMING_REQ_START_AFTER,			///<ゲーム開始演出が終了したら同期取り
	TIMING_REQ_FINISH_AFTER,		///<ゲーム終了演出が終了したら同期取り

	TIMING_REQ_APPEAR_AFTER,		///<風船が登場したら同期取り
	TIMING_REQ_EXPLODED_AFTER,		///<破裂エフェクトが終了したら同期取り
	
	TIMING_REQ_TOUCHPEN_DEMO_AFTER,	///<タッチペンデモが終了したら同期取り
	
	//----------------------------
	TIMING_REQ_WAIT,				///<同期信号は送って、他の人の信号待ち
};


//==============================================================================
//	構造体定義
//==============================================================================

///同期取り
typedef struct{
	u8 timing_req;		///<同期取りの種類
	u8 timing_no;		///<同期取り番号
	
	u8 dummy[2];
}BALLOON_TIMING;

///ゲーム開始
typedef struct{
	u8 timing_req;		///<同期取りの種類
	u8 timing_no;		///<同期取り番号
	
	u8 dummy[2];
}BALLOON_START;

///ゲーム終了
typedef struct{
	u8 timing_req;		///<同期取りの種類
	u8 timing_no;		///<同期取り番号
	
	u8 dummy[2];
}BALLOON_FINISH;

///発生した空気量
typedef struct{
	u8 net_id;			///<通信ID
	u8 booster_type;	///<BOOSTER_TYPE_???
	u16 no;				///<風船番号(ゲームが始まって何個目の風船か)
	s32 air;			///<空気(ブースター、レアがない純粋な空気量)
	s32 last_air;		///<ブースター、レア補正分込みの最終的な空気量
	u8 booster_occ;		///<ブースターの設定有無(TRUE:設定済み)
	u8 padding[3];
}BALLOON_AIR_DATA;

///破裂した風船番号情報
typedef struct{
	u16 no;				///<風船番号(ゲームが始まって何個目の風船か)

	u8 timing_req;
	u8 timing_no;
}BALLOON_EXPLODED;

///新しく登場する風船情報
typedef struct{
	u16 no;				///<風船番号(ゲームが始まって何個目の風船か)
	u8 level;			///<風船レベル(BALLOON_LEVEL_???)
	
	u8 timing_req;
	u8 timing_no;
	
	u8 dummy[2];
}BALLOON_APPEAR;


//--------------------------------------------------------------
/**
 *	ゲーム中の送受信データ構造体
 */
//--------------------------------------------------------------
typedef struct{
	int order_code;		///<命令コード
	
	union{
		BALLOON_TIMING timing;
		BALLOON_TIMING start;
		BALLOON_TIMING finish;
		BALLOON_AIR_DATA air_data;
		BALLOON_EXPLODED exploded;
		BALLOON_APPEAR appear;
	};
}BALLOON_SIO_PLAY_WORK;


//--------------------------------------------------------------
/**
 *	送受信バッファ(チェーンリストとして扱う)
 */
//--------------------------------------------------------------
typedef struct{
	BALLOON_SIO_PLAY_WORK	play_work[SIO_BUFFER_CHAIN_MAX];
}BALLOON_SIO_BUFFER;


#endif	//__BALLOON_COMM_TYPES_H__

