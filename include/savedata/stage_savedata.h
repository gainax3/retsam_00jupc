//============================================================================================
/**
 * @file	stage_savedata.h
 * @brief	ステージセーブデータ用ヘッダー
 * @author	Satoshi Nohara
 * @date	2007.06.07
 */
//============================================================================================
#ifndef	__STAGE_SAVEDATA_H__
#define	__STAGE_SAVEDATA_H__


//============================================================================================
//
//	定義
//
//============================================================================================
enum{
	STAGEDATA_ID_TYPE = 0,
	STAGEDATA_ID_ROUND,
	//トレーナー
	STAGEDATA_ID_TR_INDEX,
	//参加している手持ちポケモン位置情報
	STAGEDATA_ID_MINE_POKE_POS,
	//敵ポケモンデータが被らないように保存
	STAGEDATA_ID_ENEMY_POKE_INDEX,

	//////////
	//成績
	//////////
	STAGESCORE_ID_CLEAR_FLAG,
};


//---------------------------------------------------------------------------
/**
 * @brief	ステージセーブデータへの不完全型定義
 */
//---------------------------------------------------------------------------
typedef struct _STAGEDATA STAGEDATA;
typedef struct _STAGESCORE STAGESCORE;


//============================================================================================
//
//
//============================================================================================
extern STAGEDATA * SaveData_GetStageData( SAVEDATA* sv );

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
extern void STAGEDATA_Init( STAGEDATA* pv );
extern int STAGEDATA_GetWorkSize( void );
extern STAGEDATA* STAGEDATA_AllocWork( int heapID );

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
extern void STAGEDATA_InitPlayData( STAGEDATA* wk );
extern BOOL STAGEDATA_GetSaveFlag( STAGEDATA* wk );
extern void STAGEDATA_SetSaveFlag( STAGEDATA* wk, BOOL flag );
extern void STAGEDATA_PutPlayData( STAGEDATA* wk, u8 id, u8 param, u8 param2, const void* buf );
extern u32 STAGEDATA_GetPlayData( STAGEDATA* wk, u8 id, u8 param, u8 param2, void* buf );
extern u8 Stage_GetTypeLevel( u8 csr_pos, u8* p_rank );
extern void Stage_SetTypeLevel( u8 csr_pos, u8* p_rank, u8 num );
extern void Stage_ClearTypeLevel( u8* p_rank );

//成績
extern void STAGESCORE_Init( STAGESCORE* pv );
extern int STAGESCORE_GetWorkSize( void );
extern void STAGESCORE_ClearTypeLevel( STAGESCORE* wk, u8 type );
extern STAGESCORE * SaveData_GetStageScore( SAVEDATA* sv );
extern void STAGESCORE_PutScoreData( STAGESCORE* wk, u8 id, u8 param, u8 param2, const void* buf );
extern u32 STAGESCORE_GetScoreData( STAGESCORE* wk, u8 id, u8 param, u8 param2, void* buf );


#endif	__STAGE_SAVEDATA_H__


