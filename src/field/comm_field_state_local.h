//==============================================================================
//	型宣言
//==============================================================================
// コールバック関数の書式
typedef void (*PTRStateFunc)(void);

#define _TRAINER_CARD_NUM_MAX (4)
#define UGSTATE_MOVE (0)

//==============================================================================
// ワーク
//==============================================================================

typedef struct{
    TR_CARD_DATA* pTRCard[_TRAINER_CARD_NUM_MAX];  // トレーナーカードを受信するバッファのポインタ
    u8 bTRCard[_TRAINER_CARD_NUM_MAX];
    COMM_MESSAGE* pCMess;
    FIELDSYS_WORK* pFSys;
    MATHRandContext32 sRand; ///< 親子機ネゴシエーション用乱数キー
    PTRStateFunc state;
    TCB_PTR pTcb;
    u16 timer;
    u8 connectIndex;   // 子機が接続する親機のindex番号
    u8 bStalth;
    u8 bStateNoChange;
    u8 bUGOverlay;
    u8 bReturnBattle; // バトルから戻ってきた場合は１ 入ってきた場合は０
    u8 bBattleMoveRoom; // 開始位置についたことを受信

    POKEPARTY*  party;	// ミックスバトル用パーティ

#ifdef PM_DEBUG		// Debug ROM
    u16 debugTimer;
#endif
} _COMM_FSTATE_WORK;


extern _COMM_FSTATE_WORK* _getCommFieldStateWork(void);
extern void _commStateInitialize(FIELDSYS_WORK* pFSys);
extern void _stateFinalize(void);

