//============================================================================================
/**
 * @file	castle_savedata.h
 * @brief	キャッスルセーブデータ用ヘッダー
 * @author	Satoshi Nohara
 * @date	2007.06.27
 */
//============================================================================================
#ifndef	__CASTLE_SAVEDATA_H__
#define	__CASTLE_SAVEDATA_H__


//============================================================================================
//
//	定義
//
//============================================================================================
enum{
	CASTLEDATA_ID_TYPE = 0,
	CASTLEDATA_ID_ROUND,
	//手持ちポケモン
	CASTLEDATA_ID_TEMOTI_HP,
	CASTLEDATA_ID_TEMOTI_PP,
	CASTLEDATA_ID_TEMOTI_CONDITION,
	CASTLEDATA_ID_TEMOTI_ITEM,
	//トレーナー
	CASTLEDATA_ID_TR_INDEX,
	//参加している手持ちポケモン位置情報
	CASTLEDATA_ID_MINE_POKE_POS,
	//敵ポケモンデータが被らないように保存
	CASTLEDATA_ID_ENEMY_POKE_INDEX,

	//////////
	//成績
	//////////
	CASTLESCORE_ID_CLEAR_FLAG,
	CASTLESCORE_ID_GUIDE_FLAG,
};


//---------------------------------------------------------------------------
/**
 * @brief	ステージセーブデータへの不完全型定義
 */
//---------------------------------------------------------------------------
typedef struct _CASTLEDATA CASTLEDATA;
typedef struct _CASTLESCORE CASTLESCORE;


//============================================================================================
//
//
//============================================================================================
extern CASTLEDATA * SaveData_GetCastleData( SAVEDATA* sv );

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
extern void CASTLEDATA_Init( CASTLEDATA* wk );
extern int CASTLEDATA_GetWorkSize( void );
extern CASTLEDATA* CASTLEDATA_AllocWork( int heapID );

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
extern void CASTLEDATA_ClearTypeLevel( CASTLEDATA* wk, u8 type );
extern BOOL CASTLEDATA_GetSaveFlag( CASTLEDATA* wk );
extern void CASTLEDATA_SetSaveFlag( CASTLEDATA* wk, BOOL flag );
extern void CASTLEDATA_PutPlayData( CASTLEDATA* wk, u8 id, u8 param, u8 param2, const void* buf );
extern u32 CASTLEDATA_GetPlayData( CASTLEDATA* wk, u8 id, u8 param, u8 param2, void* buf );
extern u8 Castle_GetTypeLevel( u8 csr_pos, u8* p_rank );
extern void Castle_SetTypeLevel( u8 csr_pos, u8* p_rank, u8 num );
extern void Castle_ClearTypeLevel( u8* p_rank );

//成績
extern void CASTLESCORE_Init( CASTLESCORE* wk );
extern int CASTLESCORE_GetWorkSize( void );
extern CASTLESCORE * SaveData_GetCastleScore( SAVEDATA* sv );
extern void CASTLESCORE_PutScoreData( CASTLESCORE* wk, u8 id, u8 param, u8 param2, const void* buf );
extern u32 CASTLESCORE_GetScoreData( CASTLESCORE* wk, u8 id, u8 param, u8 param2, void* buf );


#endif	__CASTLE_SAVEDATA_H__


