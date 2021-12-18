//==============================================================================
/**
 * @file	frontier_ex.h
 * @brief	バトルフロンティア外部データ(進行に関係ない記録のみ)のヘッダ
 * @author	matsuda
 * @date	2007.07.18(水)
 */
//==============================================================================
#ifndef	__FRONTIER_EX_H__
#define	__FRONTIER_EX_H__

#include "common.h"
#include "savedata/savedata_def.h"


//==============================================================================
//	定数定義
//==============================================================================

//バトルステージのデータ取得ID
enum{
	FREXID_STAGE_RENSHOU_SINGLE,		///<ステージ：シングル最大連勝数
	FREXID_STAGE_RENSHOU_DOUBLE,		///<ステージ：ダブル最大連勝数
	FREXID_STAGE_RENSHOU_MULTI,			///<ステージ：ワイヤレス最大連勝数
//	FREXID_STAGE_RENSHOU_WIFI,			///<ステージ：WIFI最大連勝数
};

//--------------------------------------------------------------
/**
 *	戦闘録画セーブデータの不完全型構造体宣言
 */
//--------------------------------------------------------------
typedef struct _FRONTIER_EX_SAVEDATA		FRONTIER_EX_SAVEDATA;


//==============================================================================
//	外部関数宣言
//==============================================================================
extern int FrontierEx_GetWorkSize( void );
extern void FrontierEx_Init(FRONTIER_EX_SAVEDATA *work);
extern FRONTIER_EX_SAVEDATA * FrontierEx_Load(SAVEDATA *sv,int heapID,LOAD_RESULT *result);
extern SAVE_RESULT FrontierEx_Save(SAVEDATA *sv, FRONTIER_EX_SAVEDATA *fes);
extern u16 FrontierEx_StageRenshou_Get(SAVEDATA *sv, FRONTIER_EX_SAVEDATA *fes, int id, int monsno);

extern BOOL FrontierEx_StageRenshou_RenshouCopyExtra(SAVEDATA *sv, 
	int record_id, int record_monsno_id, 
	int friend_no, int save_id, int heap_id, LOAD_RESULT *load_result, SAVE_RESULT *save_result);


extern FRONTIER_EX_SAVEDATA * SaveData_Extra_LoadFrontierEx(SAVEDATA * sv, int heap_id, LOAD_RESULT * result);
extern SAVE_RESULT SaveData_Extra_SaveFrontierEx(SAVEDATA * sv, FRONTIER_EX_SAVEDATA * data);

//デバック用
extern u16 Debug_FrontierEx_StageRenshou_Set(FRONTIER_EX_SAVEDATA *fes, int id, int monsno, u16 data);


#endif	/* __FRONTIER_EX_H__ */
