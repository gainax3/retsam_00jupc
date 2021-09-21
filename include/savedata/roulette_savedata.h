//============================================================================================
/**
 * @file	roulette_savedata.h
 * @brief	ルーレットセーブデータ用ヘッダー
 * @author	Satoshi Nohara
 * @date	2007.09.05
 */
//============================================================================================
#ifndef	__ROULETTE_SAVEDATA_H__
#define	__ROULETTE_SAVEDATA_H__


//============================================================================================
//
//	定義
//
//============================================================================================
enum{
	ROULETTEDATA_ID_TYPE = 0,
	ROULETTEDATA_ID_RANDOM_FLAG,
	ROULETTEDATA_ID_ROUND,
	ROULETTEDATA_ID_CSR_SPEED_LEVEL,
	//手持ちポケモン
	ROULETTEDATA_ID_TEMOTI_ITEM,
	//トレーナー
	ROULETTEDATA_ID_TR_INDEX,
	//参加している手持ちポケモン位置情報
	ROULETTEDATA_ID_MINE_POKE_POS,
	//敵ポケモンデータが被らないように保存
	ROULETTEDATA_ID_ENEMY_POKE_INDEX,

	//////////
	//成績
	//////////
	ROULETTESCORE_ID_CLEAR_FLAG,
};


//---------------------------------------------------------------------------
/**
 * @brief	ステージセーブデータへの不完全型定義
 */
//---------------------------------------------------------------------------
typedef struct _ROULETTEDATA ROULETTEDATA;
typedef struct _ROULETTESCORE ROULETTESCORE;


//============================================================================================
//
//
//============================================================================================
extern ROULETTEDATA * SaveData_GetRouletteData( SAVEDATA* sv );

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
extern void ROULETTEDATA_Init( ROULETTEDATA* wk );
extern int ROULETTEDATA_GetWorkSize( void );
extern ROULETTEDATA* ROULETTEDATA_AllocWork( int heapID );

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
extern void ROULETTEDATA_ClearTypeLevel( ROULETTEDATA* wk, u8 type );
extern BOOL ROULETTEDATA_GetSaveFlag( ROULETTEDATA* wk );
extern void ROULETTEDATA_SetSaveFlag( ROULETTEDATA* wk, BOOL flag );
extern void ROULETTEDATA_PutPlayData( ROULETTEDATA* wk, u8 id, u8 param, u8 param2, const void* buf );
extern u32 ROULETTEDATA_GetPlayData( ROULETTEDATA* wk, u8 id, u8 param, u8 param2, void* buf );

//成績
extern void ROULETTESCORE_Init( ROULETTESCORE* wk );
extern int ROULETTESCORE_GetWorkSize( void );
extern ROULETTESCORE * SaveData_GetRouletteScore( SAVEDATA* sv );
extern void ROULETTESCORE_PutScoreData( ROULETTESCORE* wk, u8 id, u8 param, u8 param2, const void* buf );
extern u32 ROULETTESCORE_GetScoreData( ROULETTESCORE* wk, u8 id, u8 param, u8 param2, void* buf );


#endif	__ROULETTE_SAVEDATA_H__


