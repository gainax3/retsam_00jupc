//=============================================================================
/**
 * @file	comm_player_local.h
 * @brief	プレイヤーデータのローカル定義
 * @author	Katsumi Ohno
 * @date    2008.01.24
 */
//=============================================================================

#ifndef __COMM_PLAYER_LOCAL_H__
#define __COMM_PLAYER_LOCAL_H__

//==============================================================================
// 定義
//==============================================================================


/// 無効なフィールドマップアクター番号
#define _NULL_FIELD_MAP_ACT_ID (-1)
/// ネットプレーヤーはタスクは後方でいい
#define _TASK_PRI_NET_PLAYER  (100)
/// ネットプレーヤー生成はさらに後
#define _TASK_PRI_NET_PLAYER_CREATE  (_TASK_PRI_NET_PLAYER+100)
/// BLACT_HEADER workの　親子機IDを入れておく場所
/// ０と１は本物のヒーローアニメで使用しているので２番になっている
#define _WORK_MACHINE_ID (2)

#define _NAME_DISP (0)   ///< プレーヤーに名前を表示する

#define _NO_PLACE_INITIALIZE (0xff)  ///< 移動の初期化が終わっていない場合

#define _PLACEDATA_SEND_SIZE (5)  ///< 場所データを送信する場合のサイズ
#define _IDPLACEDATA_SEND_SIZE (4)  ///< IDつき場所データを送信する場合のサイズ

//#define _BLOW_STOP_COUNT (15)  // 最初に動かない数
#define _BLOW_COUNTER_NUM_MAX   (27)  // ふき飛ばされるフレーム数
#define _BIGBLOW_COUNTER_NUM_MAX   (45)  // ふき飛ばされるフレーム数
#define _BLOW_COUNTER_STOP   (0xff)  // 止めておく区間の値

#define _TALK_TARGET_INDEX_WORK (0)  ///< 会話データ　話相手のIDの位置
#define _TALK_MY_INDEX_WORK (1)  ///< 会話データ　話しているIDの位置

#define _FLAG_TVTOPIC_NUM (5)  ///TVに流れる場合の旗の数

// メニュー＆ウインドウをタスクに登録していく際のプライオリティー
enum menuTaskPri_e {
    _TALK_WINDOW_PRI = 10001,
};


enum _player_speed {
    _SPEED_2,
    _SPEED_4,
    _SPEED_8,
    _SPEED_16,
    _SPEED_HITWALL,
    _SPEED_MAX,
};


enum hedFieldEffect {
    _HFE_NONE,
    _HFE_FLAG,
    _HFE_EXCLAMATION,
    _HFE_OK,
};


#define _LOCAL_WIN_PX (2)   // GF_BGL_BmpAddで使用する 文字ウインドウのPX値
#define _LOCAL_WIN_PY (19)   // GF_BGL_BmpAddで使用する 文字ウインドウのPY値
#define _LOCAL_WIN_SX (28)   // GF_BGL_BmpAddで使用する 文字ウインドウのSX値
#define _LOCAL_WIN_SY (4)   // GF_BGL_BmpAddで使用する 文字ウインドウのSY値

#define _DASH_BIT (0x80)
#define _DEBUG_DASH_BIT (0x40)

#define _MSG_MAX  (50*GLOBAL_MSGLEN)    ///<

#define _KEEP_FLAG_MAX (5)  // 記憶しておくID

//==============================================================================
//	型宣言
//==============================================================================

typedef struct{
    int timer;
    int targetID;
} _FREEZE_PLAYER;



typedef struct{
    u8 mystatus[COMM_SEND_MYSTATUS_SIZE];
    u16 netID;
} _SEND_FLAG;


typedef struct{
    u16 xpos;
    u16 zpos;
    s8 dir;
    u8 speed;
    u8 speedKey;
    u8 bHit;
} _PlayerPlace;

typedef struct{
    u16 xpos;
    u16 zpos;
} _Grid;

typedef struct{
    u8 netID:4;      // 話している人
    u8 targetID:4;   // 話しかけられている人
    u8 bItemMenu:1;      //  アイテムメニューを開きにいっている
    u8 bPersonalInformation:1;      // 個人情報を開いている
} _talkMenuInfo;

typedef struct{
    u32 holdBit;
    PLAYER_CONDITION_MGR_PTR pCondMgr;
    PLAYER_STATE_PTR pPlayer[COMM_MACHINE_MAX];   // プレーヤーのポインタ
    EOA_PTR pMark[COMM_MACHINE_MAX];  //まんぷ
    u8 bActive[COMM_MACHINE_MAX];     // プレーヤーが存在するかどうか
    TCB_PTR pTask;          // 処理タスクのポインタ
    FIELDSYS_WORK* pFSys;
    _talkMenuInfo talkMenuInfo;   // 自分がどういう会話状態にあるかどうか記憶 サーバには無い
    u8 handCount[COMM_MACHINE_MAX];
    _PlayerPlace sPlayerPlaceServer[ COMM_MACHINE_MAX ];  // サーバーのプレーヤーの位置情報
    _PlayerPlace sPlayerPlace[ COMM_MACHINE_MAX ];  // プレーヤーの位置情報
    u8 bMoveControl[ COMM_MACHINE_MAX ]; ///< 機のコントロール状態
    u8 bMoveControlFlag[ COMM_MACHINE_MAX ]; ///< 機のコントロール状態 ハタ専用
    u8 bMoveControlVS[ COMM_MACHINE_MAX ];
    u8 hedFE[ COMM_MACHINE_MAX ]; ///< 頭の上のフィールドエフェクト管理
    s8 blowDir[COMM_MACHINE_MAX];
    u8 blowCounterS[COMM_MACHINE_MAX];   // 吹き飛ばしカウンタ
    u8 blowDirS[COMM_MACHINE_MAX];
    u8 giddyCounter[COMM_MACHINE_MAX];
    u8 countHole[COMM_MACHINE_MAX];
    u8 bBlowBigS[COMM_MACHINE_MAX];
    u8 bMoveChange[COMM_MACHINE_MAX]; // 移動したので送信するためのフラグ
    u8 walkCount[COMM_MACHINE_MAX];   // 歩き速度
    u8 walkCountClient[COMM_MACHINE_MAX];   // 歩き速度 クライアント
    _SEND_FLAG sendFlagData[COMM_MACHINE_MAX+1];
  //  u8 sendFlagDataID[COMM_MACHINE_MAX];
    MYSTATUS* pKeepFlag[_KEEP_FLAG_MAX];  // 5つ分ありこの時に重複しなければ記録
    MYSTATUS* pNowFlag[COMM_MACHINE_MAX];  //みんなから送信されたものはここに入る
//    MYSTATUS* pMyFlag;   //自分が持っているはた これを送信しあう
    u16 oldPad;
    u16 myDeliveryFlagNum;
    u8 bStartLineChk[4]; //
    u8 bTalkMenuDisp;   ///<< 会話ウインドウ表示中TRUE
//    u8 msgIndex;      ///< メッセージインデックス メッセージ終了待ちに使用
    u8 bFlagDataRecv;
    u8 moveSync;     ///< 移動位置同期
    u8 bPosAllSend;  ///< 全員の位置を再送信するフラグ
    u8 bMyMoveControl; ///< 自機のコントロール
    u8 bResetting;   ///< リセット中の場合勝手に動かないように
    u8 bUnder;    // 地下かどうか
    u8 bFirstMoveEnable;  // 親機がいなくなった場合、親機に接続した場合 開始を送信するためのFLG
    u8 forceDirFlg;  //強制角度調整
    u8 bMoveControlVSRoom; //対戦部屋での移動
    u8 bDataSendFlag; // 旗の動き制御
  u8 bNotRecvMoveData;  //自分の移動データをサーバから受け取らない
} CommPlayerWork;


extern CommPlayerWork* CommPlayerGetInstance(void);

#endif //__COMM_PLAYER_LOCAL_H__

