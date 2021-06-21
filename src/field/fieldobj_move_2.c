//******************************************************************************
/**
 *
 * @file	fieldobj_move_2.c
 * @brief	フィールドOBJ　基本動作系その2
 * @author	kagaya
 * @data	05.07.21
 *
 */
//******************************************************************************
#include "common.h"
#include "fieldsys.h"
#include "fieldobj.h"

#include "ev_trainer.h"
#include "field_effect.h"

//==============================================================================
//	define
//==============================================================================
///トレーナーペア　親移動停止
#define PAIR_TR_OYA_STA_BIT_STOP \
	(FLDOBJ_STA_BIT_ATTR_GET_ERROR		| \
	 FLDOBJ_STA_BIT_HEIGHT_GET_ERROR	| \
	 FLDOBJ_STA_BIT_PAUSE_MOVE)

//--------------------------------------------------------------
///	コピー動作番号
//--------------------------------------------------------------
enum
{
	SEQNO_COPYMOVE_FIRST_INIT,
	SEQNO_COPYMOVE_FIRST_WAIT,
	SEQNO_COPYMOVE_INIT,
	SEQNO_COPYMOVE_CMD_SET,
	SEQNO_COPYMOVE_CMD_WAIT,
	SEQNO_COPYMOVE_MAX,
};

//--------------------------------------------------------------
///	壁移動　利き手
//--------------------------------------------------------------
typedef enum
{
	ALONG_L = 0,		///<左
	ALONG_R,			///<右
	ALONG_LR,			///<左右
}ALONG_DIR;

//==============================================================================
//	typedef
//==============================================================================
//--------------------------------------------------------------
//	MV_PAIR_WORK構造体
//--------------------------------------------------------------
typedef struct
{
	u8 seq_no;												///<動作番号
	u8 jiki_init;											///<自機情報初期化完了
	s16 jiki_gx;											///<自機グリッド座標X
	s16 jiki_gz;											///<自機グリッド座標Z
	u16 jiki_ac;											///<自機アニメーションコード
}MV_PAIR_WORK;

#define MV_PAIR_WORK_SIZE (sizeof(MV_PAIR_WORK))

//--------------------------------------------------------------
//	MV_TR_PAIR_WORK構造体
//--------------------------------------------------------------
typedef struct
{
	u8 seq_no;											///<動作番号
	u8 oya_init;										///<親情報初期化完了
	s16 oya_gx;											///<親グリッド座標X
	s16 oya_gz;											///<親グリッド座標Z
	u16 oya_ac;											///<自機アニメーションコード
	FIELD_OBJ_PTR oyaobj;								///<親となるOBJ
}MV_TR_PAIR_WORK;

#define MV_TR_PAIR_WORK_SIZE (sizeof(MV_TR_PAIR_WORK))

//--------------------------------------------------------------
///	MV_HIDE_WORK構造体
//--------------------------------------------------------------
typedef struct
{
	u8 seq_no;
	u8 hide_type;
	u8 pulloff_flag;
	u8 dmy;
	EOA_PTR eoa_hide;
}MV_HIDE_WORK;

#define MV_HIDE_WORK_SIZE (sizeof(MV_HIDE_WORK))

//--------------------------------------------------------------
///	MV_COPY_WORK構造体
//--------------------------------------------------------------
typedef struct
{
	u8 seq_no;
	s8 dir_jiki;
	u8 lgrass_on;
	u8 dmy;
}MV_COPY_WORK;

#define MV_COPY_WORK_SIZE (sizeof(MV_COPY_WORK))

//--------------------------------------------------------------
///	MV_ALONGW_WORK構造体
//--------------------------------------------------------------
typedef struct
{
	u32 seq_no;
	ALONG_DIR dir_hand_init;
	ALONG_DIR dir_hand;
}MV_ALONGW_WORK;

#define MV_ALONGW_WORK_SIZE (sizeof(MV_ALONGW_WORK))

//==============================================================================
//	プロトタイプ
//==============================================================================
int (* const DATA_PairMoveTbl[])( FIELD_OBJ_PTR fldobj, MV_PAIR_WORK *work );

static int Pair_WorkSetJikiSearch( FIELD_OBJ_PTR fldobj, MV_PAIR_WORK *work );
static void Pair_WorkInit( FIELD_OBJ_PTR fldobj, MV_PAIR_WORK *work );
static int Pair_JikiPosUpdateCheck( FIELD_OBJ_PTR fldobj, MV_PAIR_WORK *work );
static void Pair_JikiPosSet( FIELD_OBJ_PTR fldobj, MV_PAIR_WORK *work );
static u32 Pair_JikiAcmdCodeGet( FIELD_OBJ_PTR fldobj );
static int Pair_JikiCheckAcmdSet( FIELD_OBJ_PTR fldobj );

int (* const DATA_PairTrMoveTbl[])( FIELD_OBJ_PTR fldobj, MV_TR_PAIR_WORK *work );

static int PairTr_WorkSetOyaSearch( FIELD_OBJ_PTR fldobj, MV_TR_PAIR_WORK *work );
static void PairTr_WorkInit( FIELD_OBJ_PTR fldobj, MV_TR_PAIR_WORK *work, FIELD_OBJ_PTR oyaobj);
static int PairTr_OyaPosUpdateCheck( FIELD_OBJ_PTR fldobj, MV_TR_PAIR_WORK *work );
static void PairTr_OyaPosSet( FIELD_OBJ_PTR fldobj, MV_TR_PAIR_WORK *work );
static int PairTr_OyaCheckAcmdSet( FIELD_OBJ_PTR fldobj, MV_TR_PAIR_WORK *work );

int (* const DATA_HideMoveTbl[])( FIELD_OBJ_PTR fldobj, MV_HIDE_WORK *work );

//==============================================================================
//	MV_PAIR	自機連れ歩き
//==============================================================================
//--------------------------------------------------------------
/**
 * MV_PAIR　初期化
 * @param	fldobj	FIELD_OBJ_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
void FieldOBJ_MovePair_Init( FIELD_OBJ_PTR fldobj )
{
	MV_PAIR_WORK *work = FieldOBJ_MoveProcWorkInit( fldobj, MV_PAIR_WORK_SIZE );
	Pair_WorkSetJikiSearch( fldobj, work );
	FieldOBJ_DrawStatusSet( fldobj, DRAW_STA_STOP );
	FieldOBJ_StatusBitOFF_Move( fldobj );
	FieldOBJ_StatusBitSet_FellowHit( fldobj, FALSE );
}

//--------------------------------------------------------------
/**
 * MV_PAIR　動作
 * @param	fldobj	FIELD_OBJ_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
void FieldOBJ_MovePair_Move( FIELD_OBJ_PTR fldobj )
{
	MV_PAIR_WORK *work = FieldOBJ_MoveProcWorkGet( fldobj );
	
	if( Pair_WorkSetJikiSearch(fldobj,work) == FALSE ){
		return;
	}
	
	FieldOBJ_StatusBitSet_FellowHit( fldobj, FALSE );
	
	while( DATA_PairMoveTbl[work->seq_no](fldobj,work) == TRUE ){};
}

//--------------------------------------------------------------
/**
 * MV_PAIR 削除
 * @param	fldobj	FIELD_OBJ_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
void FieldOBJ_MovePair_Delete( FIELD_OBJ_PTR fldobj )
{
}

//==============================================================================
//	MV_PAIR 動作
//==============================================================================
//--------------------------------------------------------------
/**
 * Pair 0
 * @param	fldobj	FIELD_OBJ_PTR
 * @param	work	MV_PAIR_WORK
 * @retval	int		TRUE=再帰要求
 */
//--------------------------------------------------------------
static int PairMove_Init( FIELD_OBJ_PTR fldobj, MV_PAIR_WORK *work )
{
	FieldOBJ_StatusBitOFF_Move( fldobj );
	FieldOBJ_StatusBitOFF_MoveEnd( fldobj );
		
	if( Pair_JikiPosUpdateCheck(fldobj,work) == TRUE ){
		Pair_JikiPosSet( fldobj, work );
		
		if( Pair_JikiCheckAcmdSet(fldobj) == TRUE ){
			FieldOBJ_StatusBitON_Move( fldobj );
			work->seq_no++;
			return( TRUE );
		}
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * Pair 1
 * @param	fldobj	FIELD_OBJ_PTR
 * @param	work	MV_PAIR_WORK
 * @retval	int		TRUE=再帰要求
 */
//--------------------------------------------------------------
static int PairMove_Move( FIELD_OBJ_PTR fldobj, MV_PAIR_WORK *work )
{
	if( FieldOBJ_CmdAction(fldobj) == TRUE ){
		FieldOBJ_StatusBitOFF_Move( fldobj );
		work->seq_no = 0;
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
///	つれあるき動作テーブル
//--------------------------------------------------------------
static int (* const DATA_PairMoveTbl[])( FIELD_OBJ_PTR fldobj, MV_PAIR_WORK *work ) =
{
	PairMove_Init,
	PairMove_Move,
};

//==============================================================================
//	MV_PAIR　パーツ
//==============================================================================
//--------------------------------------------------------------
/**
 * 自機が存在するかチェック　存在するのであれば情報初期化 
 * @param	fldobj	FIELD_OBJ_PTR
 * @param	work	MV_PAIR_WORK
 * @retval	int		FALSE=存在していない
 */
//--------------------------------------------------------------
static int Pair_WorkSetJikiSearch( FIELD_OBJ_PTR fldobj, MV_PAIR_WORK *work )
{
	CONST_FIELD_OBJ_SYS_PTR fos = FieldOBJ_FieldOBJSysGet( fldobj );
	FIELD_OBJ_PTR jikiobj = Player_FieldOBJSearch( fos );
	
	if( jikiobj == NULL ){
		work->jiki_init = FALSE;
		return( FALSE );
	}
	
	if( work->jiki_init == FALSE ){
		Pair_WorkInit( fldobj, work );
	}
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * MV_PAIR_WORK初期化　自機がいる事前提
 * @param	fldobj	FIELD_OBJ_PTR	
 * @param	work	MV_PAIR_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Pair_WorkInit( FIELD_OBJ_PTR fldobj, MV_PAIR_WORK *work )
{
	FIELDSYS_WORK *fsys = FieldOBJ_FieldSysWorkGet( fldobj );
	PLAYER_STATE_PTR jiki = Player_FieldSysWorkPlayerGet( fsys );
	
	work->jiki_init = TRUE;
	work->jiki_gx = Player_NowGPosXGet( jiki );
	work->jiki_gz = Player_NowGPosZGet( jiki );
	work->jiki_ac = ACMD_NOT;
}

//--------------------------------------------------------------
/**
 * 自機座標更新チェック
 * @param	fldobj	FIELD_OBJ_PTR
 * @param	work	MV_PAIR_WORK
 * @retval	int		TRUE=更新した
 */
//--------------------------------------------------------------
static int Pair_JikiPosUpdateCheck( FIELD_OBJ_PTR fldobj, MV_PAIR_WORK *work )
{
	FIELDSYS_WORK *fsys = FieldOBJ_FieldSysWorkGet( fldobj );
	PLAYER_STATE_PTR jiki = Player_FieldSysWorkPlayerGet( fsys );
	
	if( jiki != NULL ){
		int gx = Player_NowGPosXGet( jiki );
		int gz = Player_NowGPosZGet( jiki );
		
		if( gx != work->jiki_gx || gz != work->jiki_gz ){
			return( TRUE );
		}
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * 自機座標セット
 * @param	fldobj	FIELD_OBJ_PTR
 * @param	work	MV_PAIR_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Pair_JikiPosSet( FIELD_OBJ_PTR fldobj, MV_PAIR_WORK *work )
{
	FIELDSYS_WORK *fsys = FieldOBJ_FieldSysWorkGet( fldobj );
	PLAYER_STATE_PTR jiki = Player_FieldSysWorkPlayerGet( fsys );
	
	work->jiki_gx = Player_NowGPosXGet( jiki );
	work->jiki_gz = Player_NowGPosZGet( jiki );
}

//--------------------------------------------------------------
/**
 * 自機アニメコード取得
 * @param	fldobj	FIELD_OBJ_PTR
 * @retval	u32		アニメーションコード
 */
//--------------------------------------------------------------
static u32 Pair_JikiAcmdCodeGet( FIELD_OBJ_PTR fldobj )
{
	u32 code;
	FIELDSYS_WORK *fsys = FieldOBJ_FieldSysWorkGet( fldobj );
	PLAYER_STATE_PTR jiki = Player_FieldSysWorkPlayerGet( fsys );
	code = Player_AcmdCodeGet( jiki );
	
	switch( code ){
	case AC_DASH_U_4F: code = AC_WALK_U_4F; break;
	case AC_DASH_D_4F: code = AC_WALK_D_4F; break;
	case AC_DASH_L_4F: code = AC_WALK_L_4F; break;
	case AC_DASH_R_4F: code = AC_WALK_R_4F; break;
	}
	
	return( code );
}

//--------------------------------------------------------------
/**
 * 自機アニメコード、方向を取得しアニメーションコマンドをセット
 * @param	fldobj	FIELD_OBJ_PTR
 * @retval	int		TRUE=セットできた FALSE=重なりによりセットできない
 */
//--------------------------------------------------------------
static int Pair_JikiCheckAcmdSet( FIELD_OBJ_PTR fldobj )
{
	FIELDSYS_WORK *fsys = FieldOBJ_FieldSysWorkGet( fldobj );
	PLAYER_STATE_PTR jiki = Player_FieldSysWorkPlayerGet( fsys );
	int gx = FieldOBJ_NowPosGX_Get( fldobj );
	int gz = FieldOBJ_NowPosGZ_Get( fldobj );
	int jx = Player_OldGPosXGet( jiki );
	int jz = Player_OldGPosZGet( jiki );
	
	if( gx != jx || gz != jz ){
		u32 code = Pair_JikiAcmdCodeGet( fldobj );
		int dir = FieldOBJTool_DirRange( gx, gz, jx, jz );
		code = FieldOBJ_AcmdCodeDirChange( dir, code );
		FieldOBJ_CmdSet( fldobj, code );
		
		return( TRUE );
	}
	
	return( FALSE );
}

//==============================================================================
//	MV_TR_PAIR トレーナー連れ歩き
//==============================================================================
//--------------------------------------------------------------
/**
 * MV_TR_PAIR　初期化
 * @param	fldobj	FIELD_OBJ_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
void FieldOBJ_MovePairTr_Init( FIELD_OBJ_PTR fldobj )
{
	MV_TR_PAIR_WORK *work = FieldOBJ_MoveProcWorkInit( fldobj, MV_TR_PAIR_WORK_SIZE );
	PairTr_WorkSetOyaSearch( fldobj, work );
	FieldOBJ_DrawStatusSet( fldobj, DRAW_STA_STOP );
	FieldOBJ_StatusBitOFF_Move( fldobj );
	
//	FieldOBJ_StatusBitSet_FellowHit( fldobj, FALSE );
	work->oya_init = FALSE;
}

//--------------------------------------------------------------
/**
 * MV_TR_PAIR　動作
 * @param	fldobj	FIELD_OBJ_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
void FieldOBJ_MovePairTr_Move( FIELD_OBJ_PTR fldobj )
{
	MV_TR_PAIR_WORK *work = FieldOBJ_MoveProcWorkGet( fldobj );
	
	if( PairTr_WorkSetOyaSearch(fldobj,work) == FALSE ){
		return;
	}
	
//	FieldOBJ_StatusBitSet_FellowHit( fldobj, FALSE );
	while( DATA_PairTrMoveTbl[work->seq_no](fldobj,work) == TRUE ){};
}

//--------------------------------------------------------------
/**
 * MV_TR_PAIR 削除
 * @param	fldobj	FIELD_OBJ_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
void FieldOBJ_MovePairTr_Delete( FIELD_OBJ_PTR fldobj )
{
}

//--------------------------------------------------------------
/**
 * MV_TR_PAIR　復帰
 * @param	fldobj	FIELD_OBJ_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
void FieldOBJ_MovePairTr_Return( FIELD_OBJ_PTR fldobj )
{
	MV_TR_PAIR_WORK *work = FieldOBJ_MoveProcWorkGet( fldobj );
	work->oya_init = 0;
//	PairTr_WorkSetOyaSearch( fldobj, work );
}

//==============================================================================
//	MV_TR_PAIR 動作
//==============================================================================
//--------------------------------------------------------------
/**
 * Pair 0
 * @param	fldobj	FIELD_OBJ_PTR
 * @param	work	MV_TR_PAIR_WORK
 * @retval	int		TRUE=再帰要求
 */
//--------------------------------------------------------------
static int PairTrMove_Init( FIELD_OBJ_PTR fldobj, MV_TR_PAIR_WORK *work )
{
	FieldOBJ_StatusBitOFF_Move( fldobj );
	FieldOBJ_StatusBitOFF_MoveEnd( fldobj );
		
	if( PairTr_OyaPosUpdateCheck(fldobj,work) == TRUE ){
		if( PairTr_OyaCheckAcmdSet(fldobj,work) == TRUE ){
//			PairTr_OyaPosSet( fldobj, work );
			FieldOBJ_StatusBitON_Move( fldobj );
			work->seq_no++;
			return( TRUE );
		}
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * Pair 1
 * @param	fldobj	FIELD_OBJ_PTR
 * @param	work	MV_PAIR_WORK
 * @retval	int		TRUE=再帰要求
 */
//--------------------------------------------------------------
static int PairTrMove_Move( FIELD_OBJ_PTR fldobj, MV_TR_PAIR_WORK *work )
{
	if( FieldOBJ_CmdAction(fldobj) == FALSE ){
		return( FALSE );
	}
	
	FieldOBJ_StatusBitOFF_Move( fldobj );
	work->seq_no = 0;
	return( FALSE );
}

//--------------------------------------------------------------
///	つれあるき動作テーブル
//--------------------------------------------------------------
static int (* const DATA_PairTrMoveTbl[])( FIELD_OBJ_PTR fldobj, MV_TR_PAIR_WORK *work ) =
{
	PairTrMove_Init,
	PairTrMove_Move,
};

//==============================================================================
//	MV_TR_PAIR　パーツ
//==============================================================================
//--------------------------------------------------------------
/**
 * 対象がペア動作かどうか。ペア動作であれば相方を探す
 * @param	fldobj	FIELD_OBJ_PTR
 * @retval	FIELD_OBJ_PTR　相方のFIELD_OBJ_PTR、NULL=ペア動作ではない
 */
//--------------------------------------------------------------
FIELD_OBJ_PTR FieldOBJ_MovePairSearch( FIELD_OBJ_PTR fldobj )
{
	int no = 0;
	int type = FieldOBJ_EventTypeGet( fldobj );
	int zone_id = FieldOBJ_ZoneIDGet( fldobj );
	u32 trid = EvTrainerFldOBJTrainerIDGet( fldobj );
	CONST_FIELD_OBJ_SYS_PTR fos = FieldOBJ_FieldOBJSysGet( fldobj );
	FIELD_OBJ_PTR pair;
	
	switch( type ){
	case EV_TYPE_TRAINER:
	case EV_TYPE_TRAINER_EYEALL:
	case EV_TYPE_ITEM:
	case EV_TYPE_TRAINER_KYORO:
	case EV_TYPE_TRAINER_SPIN_STOP_L:
	case EV_TYPE_TRAINER_SPIN_STOP_R:
	case EV_TYPE_TRAINER_SPIN_MOVE_L:
	case EV_TYPE_TRAINER_SPIN_MOVE_R:
		while( FieldOBJSys_FieldOBJSearch(fos,&pair,&no,FLDOBJ_STA_BIT_USE) == TRUE ){
			if( fldobj != pair && FieldOBJ_ZoneIDGet(pair) == zone_id ){
				if( EvTrainerFldOBJTrainerIDGet(pair) == trid ){
					return( pair );
				}
			}
		}
	}
	
	return( NULL );
}

//--------------------------------------------------------------
/**
 * 親が存在するかチェック　存在するのであれば情報初期化 
 * @param	fldobj	FIELD_OBJ_PTR
 * @param	work	MV_TR_PAIR_WORK
 * @retval	int		FALSE=存在していない
 */
//--------------------------------------------------------------
static int PairTr_WorkSetOyaSearch( FIELD_OBJ_PTR fldobj, MV_TR_PAIR_WORK *work )
{
	int no,zone;
	u32 trid;
	FIELD_OBJ_PTR oyaobj;
	CONST_FIELD_OBJ_SYS_PTR fos = FieldOBJ_FieldOBJSysGet( fldobj );
	
	no = 0;
	zone = FieldOBJ_ZoneIDGet( fldobj );
	trid = EvTrainerFldOBJTrainerIDGet( fldobj );
	
	while( FieldOBJSys_FieldOBJSearch(fos,&oyaobj,&no,FLDOBJ_STA_BIT_USE) == TRUE ){
		if( fldobj != oyaobj &&
			FieldOBJ_ZoneIDGet(oyaobj) == zone &&
			EvTrainerFldOBJTrainerIDGet(oyaobj) == trid ){
			
			if( work->oya_init == FALSE ){
				PairTr_WorkInit( fldobj, work, oyaobj );
			}
			
			return( TRUE );
		}
	}
	
	work->oya_init = FALSE;
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * MV_TR_PAIR_WORK初期化　親がいる事前提
 * @param	fldobj	FIELD_OBJ_PTR	
 * @param	work	MV_TR_PAIR_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void PairTr_WorkInit( FIELD_OBJ_PTR fldobj, MV_TR_PAIR_WORK *work, FIELD_OBJ_PTR oyaobj )
{
	work->oya_init = TRUE;
	work->oya_gx = FieldOBJ_NowPosGX_Get( oyaobj );
	work->oya_gz = FieldOBJ_NowPosGZ_Get( oyaobj );
	work->oya_ac = ACMD_NOT;
	work->oyaobj = oyaobj;
}

//--------------------------------------------------------------
/**
 * 親座標更新チェック
 * @param	fldobj	FIELD_OBJ_PTR
 * @param	work	MV_PAIR_WORK
 * @retval	int		TRUE=更新した
 */
//--------------------------------------------------------------
static int PairTr_OyaPosUpdateCheck( FIELD_OBJ_PTR fldobj, MV_TR_PAIR_WORK *work )
{
	FIELD_OBJ_PTR oyaobj = work->oyaobj;
	int gx = FieldOBJ_NowPosGX_Get( fldobj );
	int gz = FieldOBJ_NowPosGZ_Get( fldobj );
	int ngx = FieldOBJ_OldPosGX_Get( oyaobj );
	int ngz = FieldOBJ_OldPosGZ_Get( oyaobj );
	
	if( (gx != ngx || gz != ngz) &&
		(FieldOBJ_StatusBitCheck_Move(oyaobj) == TRUE ||
		FieldOBJ_StatusBit_Check(oyaobj,PAIR_TR_OYA_STA_BIT_STOP) == 0) ){
		return( TRUE );
	}
	
	return( FALSE );
}

#if 0	//相方の高さを考慮していない
static int PairTr_OyaPosUpdateCheck( FIELD_OBJ_PTR fldobj, MV_TR_PAIR_WORK *work )
{
	FIELD_OBJ_PTR oyaobj = work->oyaobj;
	int gx = FieldOBJ_NowPosGX_Get( fldobj );
	int gz = FieldOBJ_NowPosGZ_Get( fldobj );
	int ngx = FieldOBJ_OldPosGX_Get( oyaobj );
	int ngz = FieldOBJ_OldPosGZ_Get( oyaobj );
	
	if( (gx != ngx || gz != ngz) &&
		FieldOBJ_StatusBit_CheckEasy(oyaobj,FLDOBJ_STA_BIT_ATTR_GET_ERROR) == FALSE &&
		FieldOBJ_StatusBit_CheckEasy(oyaobj,FLDOBJ_STA_BIT_PAUSE_MOVE) == FALSE ){
		return( TRUE );
	}
	
	return( FALSE );
}
#endif

#if 0
static int PairTr_OyaPosUpdateCheck( FIELD_OBJ_PTR fldobj, MV_TR_PAIR_WORK *work )
{
	FIELD_OBJ_PTR oyaobj = work->oyaobj;
	int gx = FieldOBJ_NowPosGX_Get( oyaobj );
	int gz = FieldOBJ_NowPosGZ_Get( oyaobj );
	
	if( (gx != work->oya_gx || gz != work->oya_gz) &&
		FieldOBJ_StatusBit_CheckEasy(oyaobj,FLDOBJ_STA_BIT_ATTR_GET_ERROR) == FALSE &&
		FieldOBJ_StatusBit_CheckEasy(oyaobj,FLDOBJ_STA_BIT_PAUSE_MOVE) == FALSE ){
		return( TRUE );
	}
	
	return( FALSE );
}
#endif

//--------------------------------------------------------------
/**
 * 親座標セット
 * @param	fldobj	FIELD_OBJ_PTR
 * @param	work	MV_PAIR_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void PairTr_OyaPosSet( FIELD_OBJ_PTR fldobj, MV_TR_PAIR_WORK *work )
{
	work->oya_gx = FieldOBJ_NowPosGX_Get( work->oyaobj );
	work->oya_gz = FieldOBJ_NowPosGZ_Get( work->oyaobj );
}

//--------------------------------------------------------------
/**
 * 親アニメコード、方向を取得しアニメーションコマンドをセット
 * @param	fldobj	FIELD_OBJ_PTR
 * @retval	int		TRUE=セットできた FALSE=重なりによりセットできない
 */
//--------------------------------------------------------------
static int PairTr_OyaCheckAcmdSet( FIELD_OBJ_PTR fldobj, MV_TR_PAIR_WORK *work )
{
	int gx = FieldOBJ_NowPosGX_Get( fldobj );
	int gz = FieldOBJ_NowPosGZ_Get( fldobj );
	int ngx = FieldOBJ_NowPosGX_Get( work->oyaobj );
	int ngz = FieldOBJ_NowPosGZ_Get( work->oyaobj );
	int ogx = FieldOBJ_OldPosGX_Get( work->oyaobj );
	int ogz = FieldOBJ_OldPosGZ_Get( work->oyaobj );
	int dir;
	
	if( gx == ngx && gz == ngz ){
		return( FALSE );
	}
	
	dir = FieldOBJTool_DirRange( gx, gz, ogx, ogz );
	gx += FieldOBJ_DirAddValueGX( dir );
	gz += FieldOBJ_DirAddValueGZ( dir );
		
	if( gx != ngx || gz != ngz ){
		u32 code = AC_WALK_U_8F;
		code = FieldOBJ_AcmdCodeDirChange( dir, code );
		FieldOBJ_CmdSet( fldobj, code );
		return( TRUE );
	}
	
	return( FALSE );
}

#if 0
static int PairTr_OyaCheckAcmdSet( FIELD_OBJ_PTR fldobj, MV_TR_PAIR_WORK *work )
{
	int gx = FieldOBJ_NowPosGX_Get( fldobj );
	int gz = FieldOBJ_NowPosGZ_Get( fldobj );
	int ngx = FieldOBJ_NowPosGX_Get( work->oyaobj );
	int ngz = FieldOBJ_NowPosGZ_Get( work->oyaobj );
	int ogx = FieldOBJ_OldPosGX_Get( work->oyaobj );
	int ogz = FieldOBJ_OldPosGZ_Get( work->oyaobj );
	int sx,sz;
	
	if( gx == ngx && gz == ngz ){
		return( FALSE );
	}
	
	sx = gx - ogx;
	if( sx < 0 ){ sx = -sx; }
	sz = gz - ogz;
	if( sz < 0 ){ sz = -sz; }
	
	if( sx || sz ){
		int dir = FieldOBJTool_DirRange( gx, gz, ogx, ogz );
		gx += FieldOBJ_DirAddValueGX( dir );
		gz += FieldOBJ_DirAddValueGZ( dir );
		
		if( gx != ngx && gz != ngz ){
			u32 code = AC_WALK_U_8F;
			code = FieldOBJ_AcmdCodeDirChange( dir, code );
			FieldOBJ_CmdSet( fldobj, code );
			return( TRUE );
		}
	}
	
	return( FALSE );
}
#endif

//==============================================================================
//	MV_HIDE_SNOW　隠れ蓑　雪
//==============================================================================
//--------------------------------------------------------------
/**
 * MV_HIDE 初期化
 * @param	fldobj	FIELD_OBJ_PTR
 * @param	type	HIDETYPE
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldOBJ_MoveHide_Init( FIELD_OBJ_PTR fldobj, HIDETYPE type )
{
	MV_HIDE_WORK *work = FieldOBJ_MoveProcWorkInit( fldobj, MV_HIDE_WORK_SIZE );
	work->hide_type = type;
	FieldOBJ_DrawStatusSet( fldobj, DRAW_STA_STOP );
	FieldOBJ_StatusBitOFF_Move( fldobj );
	FieldOBJ_StatusBit_ON( fldobj, FLDOBJ_STA_BIT_SHADOW_VANISH );
	
	{															//高さ落とす
		VecFx32 offs = { 0, NUM_FX32(-32), 0 };
		FieldOBJ_VecDrawOffsSet( fldobj, &offs );
	}
}

//--------------------------------------------------------------
/**
 * MV_HIDE_SNOW 初期化
 * @param	fldobj	FIELD_OBJ_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
void FieldOBJ_MoveHideSnow_Init( FIELD_OBJ_PTR fldobj )
{
	FldOBJ_MoveHide_Init( fldobj, HIDE_SNOW );
}

//--------------------------------------------------------------
/**
 * MV_HIDE_SAND 初期化
 * @param	fldobj	FIELD_OBJ_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
void FieldOBJ_MoveHideSand_Init( FIELD_OBJ_PTR fldobj )
{
	FldOBJ_MoveHide_Init( fldobj, HIDE_SAND );
}

//--------------------------------------------------------------
/**
 * MV_HIDE_GRND 初期化
 * @param	fldobj	FIELD_OBJ_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
void FieldOBJ_MoveHideGround_Init( FIELD_OBJ_PTR fldobj )
{
	FldOBJ_MoveHide_Init( fldobj, HIDE_GROUND );
}

//--------------------------------------------------------------
/**
 * MV_HIDE_KUSA 初期化
 * @param	fldobj	FIELD_OBJ_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
void FieldOBJ_MoveHideKusa_Init( FIELD_OBJ_PTR fldobj )
{
	FldOBJ_MoveHide_Init( fldobj, HIDE_GRASS );
}

//--------------------------------------------------------------
/**
 * MV_HIDE 動作
 * @param	fldobj	FIELD_OBJ_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
void FieldOBJ_MoveHide_Move( FIELD_OBJ_PTR fldobj )
{
	MV_HIDE_WORK *work = FieldOBJ_MoveProcWorkGet( fldobj );
	while( DATA_HideMoveTbl[work->seq_no](fldobj,work) == TRUE ){};
}

//--------------------------------------------------------------
/**
 * MV_HIDE 削除
 * @param	fldobj	FIELD_OBJ_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
void FieldOBJ_MoveHide_Delete( FIELD_OBJ_PTR fldobj )
{
	EOA_PTR eoa = FieldOBJ_MoveHideEoaPtrGet( fldobj );
	if( eoa != NULL ){ FE_EoaDelete( eoa ); }
}

//--------------------------------------------------------------
/**
 * MV_HIDE 復帰
 * @param	fldobj	FIELD_OBJ_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
void FieldOBJ_MoveHide_Return( FIELD_OBJ_PTR fldobj )
{
	MV_HIDE_WORK *work = FieldOBJ_MoveProcWorkGet( fldobj );
	
	work->seq_no = 0;
	
	FieldOBJ_MoveHideEoaPtrSet( fldobj, NULL );
	
	if( work->pulloff_flag == FALSE ){
		VecFx32 offs = { 0, NUM_FX32(-32), 0 };
		FieldOBJ_VecDrawOffsSet( fldobj, &offs );
		//add pl 
		FieldOBJ_StatusBit_ON( fldobj, FLDOBJ_STA_BIT_SHADOW_VANISH );
	}
}

//==============================================================================
//	MV_HIDE 動作
//==============================================================================
//--------------------------------------------------------------
/**
 * hide 0
 * @param	fldobj	FIELD_OBJ_PTR
 * @param	work	MV_HIDE_WORK
 * @retval	int		TRUE=再帰要求
 */
//--------------------------------------------------------------
static int HideMove_Init( FIELD_OBJ_PTR fldobj, MV_HIDE_WORK *work )
{
	if( work->pulloff_flag == FALSE ){
		EOA_PTR eoa = FE_FldOBJHide_Add( fldobj, work->hide_type );
		FieldOBJ_MoveHideEoaPtrSet( fldobj, eoa );
	}
	
	FieldOBJ_StatusBitOFF_Move( fldobj );
	FieldOBJ_StatusBitOFF_MoveEnd( fldobj );
	
	work->seq_no++;
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * hide 1
 * @param	fldobj	FIELD_OBJ_PTR
 * @param	work	MV_HIDE_WORK
 * @retval	int		TRUE=再帰要求
 */
//--------------------------------------------------------------
static int HideMove_Move( FIELD_OBJ_PTR fldobj, MV_HIDE_WORK *work )
{
	if( work->pulloff_flag == FALSE ){
		EOA_PTR eoa = FieldOBJ_MoveHideEoaPtrGet( fldobj );
	
		if( eoa == NULL ){ 
			if( FieldOBJ_StatusBit_DrawInitCompCheck(fldobj) == TRUE ){
				eoa = FE_FldOBJHide_Add( fldobj, work->hide_type );
				FieldOBJ_MoveHideEoaPtrSet( fldobj, eoa );
			}
		}
		
		//add pl 常に影を消す
		FieldOBJ_StatusBit_ON( fldobj, FLDOBJ_STA_BIT_SHADOW_VANISH );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
///	MV_HIDE 動作テーブル
//--------------------------------------------------------------
static int (* const DATA_HideMoveTbl[])( FIELD_OBJ_PTR fldobj, MV_HIDE_WORK *work ) =
{
	HideMove_Init,
	HideMove_Move,
};

//==============================================================================
//	MV_HIDE パーツ
//==============================================================================
//--------------------------------------------------------------
/**
 * MV_HIDE 隠れ蓑EOA_PTRセット
 * @param	fldobj	FIELD_OBJ_PTR
 * @param	eoa		EOA_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
void FieldOBJ_MoveHideEoaPtrSet( FIELD_OBJ_PTR fldobj, EOA_PTR eoa )
{
	MV_HIDE_WORK *work = FieldOBJ_MoveProcWorkGet( fldobj );
	work->eoa_hide = eoa;
}

//--------------------------------------------------------------
/**
 * MV_HIDE 隠れ蓑EOA_PTR取得
 * @param	fldobj	FIELD_OBJ_PTR
 * @param	eoa		EOA_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
EOA_PTR FieldOBJ_MoveHideEoaPtrGet( FIELD_OBJ_PTR fldobj )
{
	MV_HIDE_WORK *work = FieldOBJ_MoveProcWorkGet( fldobj );
	return( work->eoa_hide );
}

//--------------------------------------------------------------
/**
 * MV_HIDE 隠れ蓑、脱いだ状態に
 * @param	fldobj	FIELD_OBJ_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
void FieldOBJ_MoveHidePullOffFlagSet( FIELD_OBJ_PTR fldobj )
{
	MV_HIDE_WORK *work = FieldOBJ_MoveProcWorkGet( fldobj );
	work->pulloff_flag = TRUE;
}

//==============================================================================
//	MV_COPYU
//==============================================================================
static int (* const DATA_CopyMoveTbl[SEQNO_COPYMOVE_MAX])( FIELD_OBJ_PTR, MV_COPY_WORK * );

//--------------------------------------------------------------
/**
 * ものまね　ワーク初期化
 * @param	fldobj	FIELD_OBJ_PTR
 * @param	dir		初期方向
 * @param	lgrass	長い草限定ものまね FALSE=Not
 * @retval
 */
//--------------------------------------------------------------
static void MoveCopy_WorkInit( FIELD_OBJ_PTR fldobj, int dir, u32 lgrass )
{
	MV_COPY_WORK *work = FieldOBJ_MoveProcWorkInit( fldobj, MV_COPY_WORK_SIZE );
	work->dir_jiki = DIR_NOT;
	work->lgrass_on = lgrass;
	FieldOBJ_DirDispCheckSet( fldobj, DIR_UP );
}

//--------------------------------------------------------------
/**
 * MV_COPYU 初期化
 * @param	fldobj	FIELD_OBJ_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
void FieldOBJ_MoveCopyU_Init( FIELD_OBJ_PTR fldobj )
{
	MoveCopy_WorkInit( fldobj, DIR_UP, FALSE );
}

//--------------------------------------------------------------
/**
 * MV_COPYD 初期化
 * @param	fldobj	FIELD_OBJ_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
void FieldOBJ_MoveCopyD_Init( FIELD_OBJ_PTR fldobj )
{
	MoveCopy_WorkInit( fldobj, DIR_DOWN, FALSE );
}

//--------------------------------------------------------------
/**
 * MV_COPYL 初期化
 * @param	fldobj	FIELD_OBJ_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
void FieldOBJ_MoveCopyL_Init( FIELD_OBJ_PTR fldobj )
{
	MoveCopy_WorkInit( fldobj, DIR_LEFT, FALSE );
}

//--------------------------------------------------------------
/**
 * MV_COPYR 初期化
 * @param	fldobj	FIELD_OBJ_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
void FieldOBJ_MoveCopyR_Init( FIELD_OBJ_PTR fldobj )
{
	MoveCopy_WorkInit( fldobj, DIR_RIGHT, FALSE );
}

//--------------------------------------------------------------
/**
 * MV_COPYLGRASSU 初期化
 * @param	fldobj	FIELD_OBJ_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
void FieldOBJ_MoveCopyLGrassU_Init( FIELD_OBJ_PTR fldobj )
{
	MoveCopy_WorkInit( fldobj, DIR_UP, TRUE );
}

//--------------------------------------------------------------
/**
 * MV_COPYLGRASSD 初期化
 * @param	fldobj	FIELD_OBJ_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
void FieldOBJ_MoveCopyLGrassD_Init( FIELD_OBJ_PTR fldobj )
{
	MoveCopy_WorkInit( fldobj, DIR_DOWN, TRUE );
}

//--------------------------------------------------------------
/**
 * MV_COPYLGRASSL 初期化
 * @param	fldobj	FIELD_OBJ_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
void FieldOBJ_MoveCopyLGrassL_Init( FIELD_OBJ_PTR fldobj )
{
	MoveCopy_WorkInit( fldobj, DIR_LEFT, TRUE );
}

//--------------------------------------------------------------
/**
 * MV_COPYLGRASSR 初期化
 * @param	fldobj	FIELD_OBJ_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
void FieldOBJ_MoveCopyLGrassR_Init( FIELD_OBJ_PTR fldobj )
{
	MoveCopy_WorkInit( fldobj, DIR_RIGHT, TRUE );
}

//--------------------------------------------------------------
/**
 * MV_COPY 動作
 * @param
 * @retval
 */
//--------------------------------------------------------------
void FieldOBJ_MoveCopy_Move( FIELD_OBJ_PTR fldobj )
{
	MV_COPY_WORK *work = FieldOBJ_MoveProcWorkGet( fldobj );
	while( DATA_CopyMoveTbl[work->seq_no](fldobj,work) == TRUE ){};
}

//--------------------------------------------------------------
/**
 * MV_COPY 動作　初回方向　その0
 * @param	fldobj	FIELD_OBJ_PTR
 * @param	work	MV_COPY_WORK
 * @retval	int		TRUE=再帰
 */
//--------------------------------------------------------------
static int CopyMove_FirstInit( FIELD_OBJ_PTR fldobj, MV_COPY_WORK *work )
{
	int ret = FieldOBJ_DirDispGet( fldobj );
	ret = FieldOBJ_AcmdCodeDirChange( ret, AC_DIR_U );
	FieldOBJ_CmdSet( fldobj, ret );
	FieldOBJ_StatusBitOFF_Move( fldobj );
	FieldOBJ_StatusBitOFF_MoveEnd( fldobj );
	work->seq_no = SEQNO_COPYMOVE_FIRST_WAIT;
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * MV_COPYU 動作　初回方向　その1
 * @param	fldobj	FIELD_OBJ_PTR
 * @param	work	MV_COPY_WORK
 * @retval	int		TRUE=再帰
 */
//--------------------------------------------------------------
static int CopyMove_FirstWait( FIELD_OBJ_PTR fldobj, MV_COPY_WORK *work )
{
	if( FieldOBJ_CmdAction(fldobj) == TRUE ){
		work->seq_no = SEQNO_COPYMOVE_INIT;
		return( TRUE );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * MV_COPY 動作 0
 * @param	fldobj	FIELD_OBJ_PTR
 * @param	work	MV_COPY_WORK
 * @retval	int		TRUE=再帰
 */
//--------------------------------------------------------------
static int CopyMove_Init( FIELD_OBJ_PTR fldobj, MV_COPY_WORK *work )
{
	if( work->dir_jiki == DIR_NOT ){
		FIELDSYS_WORK *fsys = FieldOBJ_FieldSysWorkGet( fldobj );
		work->dir_jiki = Player_DirGet( fsys->player );
	}
	
	FieldOBJ_StatusBitOFF_Move( fldobj );
	FieldOBJ_StatusBitOFF_MoveEnd( fldobj );
	work->seq_no = SEQNO_COPYMOVE_CMD_SET;
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * ものまね草移動チェック
 * @param	fldobj	FIELD_OBJ_PTR
 * @param	dir		移動方向
 * @retval	u32		hit bit	
 */
//--------------------------------------------------------------
static u32 CopyMove_LongGrassHitCheck( FIELD_OBJ_PTR fldobj, int dir )
{
	u32 ret = FieldOBJ_NextDirAttrGet( fldobj, dir );
	
	if( MATR_IsLongGrass(ret) == FALSE ){
		ret = FLDOBJ_MOVE_HIT_BIT_ATTR;
	}
	
	ret |= FieldOBJ_MoveHitCheckDir( fldobj, dir );
	return( ret );
}

//--------------------------------------------------------------
/**
 * コピー動作 移動セット
 * @param
 * @retval
 */
//--------------------------------------------------------------
static void CopyMove_MoveSet( FIELD_OBJ_PTR fldobj, int dir, int ac, u32 lgrass_on )
{
	u32 ret;
	
	if( lgrass_on == FALSE ){
		ret = FieldOBJ_MoveHitCheckDir( fldobj, dir );
	}else{
		ret = CopyMove_LongGrassHitCheck( fldobj, dir );
	}
	
	if( ret != FLDOBJ_MOVE_HIT_BIT_NON ){
		ac = FieldOBJ_AcmdCodeDirChange( dir, AC_DIR_U );
	}else{
		ac = FieldOBJ_AcmdCodeDirChange( dir, ac );
		FieldOBJ_StatusBitON_Move( fldobj );
	}
	
	FieldOBJ_CmdSet( fldobj, ac );
}

//--------------------------------------------------------------
/**
 * MV_COPY 動作 1
 * @param	fldobj	FIELD_OBJ_PTR
 * @param	work	MV_COPY_WORK
 * @retval	int		TRUE=再帰
 */
//--------------------------------------------------------------
static int CopyMove_CmdSet( FIELD_OBJ_PTR fldobj, MV_COPY_WORK *work )
{
	int ret;
	FIELDSYS_WORK *fsys = FieldOBJ_FieldSysWorkGet( fldobj );
	int dir = Player_DirGet( fsys->player );
	u32 type = Player_AcmdTypeGet( fsys->player );
	
	switch( type ){
	case HEROACTYPE_NON:
	case HEROACTYPE_STOP:
		ret = FieldOBJ_AcmdCodeDirChange( dir, AC_DIR_U );
		FieldOBJ_CmdSet( fldobj, ret );
		break;
	case HEROACTYPE_WALK_32F:
		CopyMove_MoveSet( fldobj, dir, AC_WALK_U_32F, work->lgrass_on );
		break;
	case HEROACTYPE_WALK_16F:
		CopyMove_MoveSet( fldobj, dir, AC_WALK_U_16F, work->lgrass_on );
		break;
	case HEROACTYPE_WALK_8F:
		CopyMove_MoveSet( fldobj, dir, AC_WALK_U_8F, work->lgrass_on );
		break;
	case HEROACTYPE_WALK_4F:
		CopyMove_MoveSet( fldobj, dir, AC_WALK_U_4F, work->lgrass_on );
		break;
	case HEROACTYPE_WALK_2F:
		CopyMove_MoveSet( fldobj, dir, AC_WALK_U_2F, work->lgrass_on );
		break;
	}
	
	work->seq_no = SEQNO_COPYMOVE_CMD_WAIT;
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * MV_COPY 動作 2
 * @param	fldobj	FIELD_OBJ_PTR
 * @param	work	MV_COPY_WORK
 * @retval	int		TRUE=再帰
 */
//--------------------------------------------------------------
static int CopyMove_MoveWait( FIELD_OBJ_PTR fldobj, MV_COPY_WORK *work )
{
	if( FieldOBJ_CmdAction(fldobj) == FALSE ){
		return( FALSE );
	}
	
	FieldOBJ_StatusBitOFF_Move( fldobj );
	FieldOBJ_StatusBitOFF_MoveEnd( fldobj );
	work->seq_no = SEQNO_COPYMOVE_INIT;
	return( FALSE );
}

//--------------------------------------------------------------
///	コピー動作テーブル
//--------------------------------------------------------------
static int (* const DATA_CopyMoveTbl[SEQNO_COPYMOVE_MAX])( FIELD_OBJ_PTR, MV_COPY_WORK * ) =
{
	CopyMove_FirstInit,
	CopyMove_FirstWait,
	CopyMove_Init,
	CopyMove_CmdSet,
	CopyMove_MoveWait,
};

//==============================================================================
//	壁沿い移動
//==============================================================================
static int AlongWall_WallMove( FIELD_OBJ_PTR fldobj, MV_ALONGW_WORK *work, int ac );
static int (* const DATA_AlongMoveTbl[3])( FIELD_OBJ_PTR, MV_ALONGW_WORK * );

//--------------------------------------------------------------
/**
 * 壁沿い移動　ワーク初期化
 * @param	fldobj		FIELD_OBJ_PTR
 * @param	dir_h_init	初期利き手
 * @param	dir_h		利き手
 * @retval	nothing
 */
//--------------------------------------------------------------
static void AlongWallWorkInit( FIELD_OBJ_PTR fldobj, ALONG_DIR dir_h_init, ALONG_DIR dir_h )
{
	MV_ALONGW_WORK *work = FieldOBJ_MoveProcWorkInit( fldobj, MV_ALONGW_WORK_SIZE );
	work->dir_hand_init = dir_h_init;
	work->dir_hand = dir_h;
}

//--------------------------------------------------------------
/**
 * 壁沿い移動　左手　初期化
 * @param	fldobj	FIELD_OBJ_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
void FieldOBJ_AlongWallL_Init( FIELD_OBJ_PTR fldobj )
{
	AlongWallWorkInit( fldobj, ALONG_L, ALONG_L );
}

//--------------------------------------------------------------
/**
 * 壁沿い移動　右手　初期化
 * @param	fldobj	FIELD_OBJ_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
void FieldOBJ_AlongWallR_Init( FIELD_OBJ_PTR fldobj )
{
	AlongWallWorkInit( fldobj, ALONG_R, ALONG_R );
}

//--------------------------------------------------------------
/**
 * 壁沿い移動　両手左　初期化
 * @param	fldobj	FIELD_OBJ_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
void FieldOBJ_AlongWallLRL_Init( FIELD_OBJ_PTR fldobj )
{
	AlongWallWorkInit( fldobj, ALONG_LR, ALONG_L );
}

//--------------------------------------------------------------
/**
 * 壁沿い移動　両手右　初期化
 * @param	fldobj	FIELD_OBJ_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
void FieldOBJ_AlongWallLRR_Init( FIELD_OBJ_PTR fldobj )
{
	AlongWallWorkInit( fldobj, ALONG_LR, ALONG_R );
}

//--------------------------------------------------------------
/**
 * 壁沿い移動　動作
 * @param	fldobj	FIELD_OBJ_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
void FieldOBJ_AlongWall_Move( FIELD_OBJ_PTR fldobj )
{
	MV_ALONGW_WORK *work = FieldOBJ_MoveProcWorkGet( fldobj );
	while( DATA_AlongMoveTbl[work->seq_no](fldobj,work) == TRUE ){};
}

//--------------------------------------------------------------
/**
 * 動作 初期化
 * @param	fldobj	FIELD_OBJ_PTR
 * @param	work	MV_ALONGW_WORK
 * @retval	int		TRUE=再帰
 */
//--------------------------------------------------------------
static int AlongWallMove_Init( FIELD_OBJ_PTR fldobj, MV_ALONGW_WORK *work )
{
	FieldOBJ_StatusBitOFF_Move( fldobj );
	work->seq_no++;
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * 動作 初期化
 * @param	fldobj	FIELD_OBJ_PTR
 * @param	work	MV_ALONGW_WORK
 * @retval	int		TRUE=再帰
 */
//--------------------------------------------------------------
static int AlongWallMove_CmdSet( FIELD_OBJ_PTR fldobj, MV_ALONGW_WORK *work )
{
	AlongWall_WallMove( fldobj, work, AC_WALK_U_8F );
	work->seq_no++;
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * 動作 コマンド終了待ち
 * @param	fldobj	FIELD_OBJ_PTR
 * @param	work	MV_ALONGW_WORK
 * @retval	int		TRUE=再帰
 */
//--------------------------------------------------------------
static int AlongWallMove_CmdWait( FIELD_OBJ_PTR fldobj, MV_ALONGW_WORK *work )
{
	if( FieldOBJ_CmdAction(fldobj) == TRUE ){
		work->seq_no = 0;
		return( TRUE );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
//	壁伝い移動　動作テーブル
//--------------------------------------------------------------
static int (* const DATA_AlongMoveTbl[3])( FIELD_OBJ_PTR, MV_ALONGW_WORK * ) =
{
	AlongWallMove_Init,
	AlongWallMove_CmdSet,
	AlongWallMove_CmdWait,
};

//--------------------------------------------------------------
///	手の位置
//--------------------------------------------------------------
static const int DATA_DirHandPosX[DIR_4_MAX][2] = 
{
	{ -1, 1 },	//up,left,right
	{ 1, -1 },	//down,left,right
	{ 0, 0 },	//left,left,right
	{ 0, 0 },	//right,left,right
};

static const int DATA_DirHandPosZ[DIR_4_MAX][2] =
{
	{ 0, 0 },	//up,left,right
	{ 0, 0 },	//down,left,right
	{ 1, -1 },	//left,left,right
	{ -1, 1 },	//right,left,right
};

//--------------------------------------------------------------
///	見失った際の判定位置
//--------------------------------------------------------------
static const int DATA_DirHandLostPosX[DIR_4_MAX][2] =
{
	{ -1, 1 },	//up,left,right
	{ 1, -1 },	//down,left,right
	{ 1, 1  },	//left,left,right
	{ -1, -1 },	//right,left,right
};

static const int DATA_DirHandLostPosZ[DIR_4_MAX][2] =
{
	{ 1, 1 },	//up,left,right
	{ -1, -1 },	//down,left,right
	{ 1, -1  },	//left,left,right
	{ -1, 1 },	//right,left,right
};

//--------------------------------------------------------------
///	見失った際の方向切り替え
//--------------------------------------------------------------
static const int DATA_DirHandLostDir[DIR_4_MAX][2] =
{
	{ DIR_LEFT, DIR_RIGHT },	//up,left,right
	{ DIR_RIGHT, DIR_LEFT },	//down,left,right
	{ DIR_DOWN, DIR_UP  },	//left,left,right
	{ DIR_UP, DIR_DOWN },	//right,left,right
};

//--------------------------------------------------------------
///	壁衝突時の方向切り替え
//--------------------------------------------------------------
static const int DATA_DirHandAttrHitDir[DIR_4_MAX][2] =
{
	{ DIR_RIGHT, DIR_LEFT },	//up,left,right
	{ DIR_LEFT, DIR_RIGHT },	//down,left,right
	{ DIR_UP, DIR_DOWN  },	//left,left,right
	{ DIR_DOWN, DIR_UP },	//right,left,right
};

//--------------------------------------------------------------
///	手のむき反転
//--------------------------------------------------------------
static const ALONG_DIR DATA_DirHandFlip[ALONG_LR] =
{ ALONG_R, ALONG_L };
	
//--------------------------------------------------------------
/**
 * 壁沿い　指定方向の壁を拾う
 * @param	fsys	FIELDSYS_WORK
 * @param	gx		グリッドX
 * @param	gz		グリッドZ
 * @param	dir		向き
 * @param	hdir	利き手
 * @retval	BOOL	TRUE=壁あり
 */
//--------------------------------------------------------------
static BOOL AlongWall_HandHitGet( FIELDSYS_WORK *fsys, int gx, int gz, int dir, ALONG_DIR hdir )
{
	BOOL hit;
	gx += DATA_DirHandPosX[dir][hdir];
	gz += DATA_DirHandPosZ[dir][hdir];
	hit = GetHitAttr( fsys, gx, gz );
	return( hit );
}

//--------------------------------------------------------------
/**
 * 壁沿い　壁消失時の壁検索
 * @param	fsys	FIELDSYS_WORK
 * @param	gx		グリッドX
 * @param	gz		グリッドZ
 * @param	dir		向き
 * @param	hdir	利き手
 * @retval	BOOL	TRUE=壁あり
 */
//--------------------------------------------------------------
static BOOL AlongWall_HandLostHitGet(
		FIELDSYS_WORK *fsys, int gx, int gz, int dir, ALONG_DIR hdir )
{
	BOOL hit;
	gx += DATA_DirHandLostPosX[dir][hdir];
	gz += DATA_DirHandLostPosZ[dir][hdir];
	hit = GetHitAttr( fsys, gx, gz );
	return( hit );
}

//--------------------------------------------------------------
/**
 * 壁沿い移動　手の位置に壁はあるか
 * @param	fldobj	FIELD_OBJ_PTR
 * @param	dir_hand	ALONG_DIR
 * @retval	int		TRUE=壁アリ
 */
//--------------------------------------------------------------
static int AlongWall_HandWallCheck( FIELD_OBJ_PTR fldobj, int dir, ALONG_DIR dir_hand )
{
	FIELDSYS_WORK *fsys = FieldOBJ_FieldSysWorkGet( fldobj );
	int gx = FieldOBJ_NowPosGX_Get( fldobj );
	int gz = FieldOBJ_NowPosGZ_Get( fldobj );
	BOOL hit = AlongWall_HandHitGet( fsys, gx, gz, dir, dir_hand );
	return( hit );
}

//--------------------------------------------------------------
/**
 * 壁沿い移動　壁手探り
 * @param	fldobj	FIELD_OBJ_PTR
 * @param	dir_hand	ALONG_DIR
 * @retval	int		TRUE=壁アリ
 */
//--------------------------------------------------------------
static int AlongWall_HandLostWallCheck( FIELD_OBJ_PTR fldobj, int dir, ALONG_DIR dir_hand )
{
	FIELDSYS_WORK *fsys = FieldOBJ_FieldSysWorkGet( fldobj );
	int gx = FieldOBJ_NowPosGX_Get( fldobj );
	int gz = FieldOBJ_NowPosGZ_Get( fldobj );
	BOOL hit = AlongWall_HandLostHitGet( fsys, gx, gz, dir, dir_hand );
	return( hit );
}

//--------------------------------------------------------------
/**
 * 壁移動チェック
 * @param	fldobj	FIELD_OBJ_PTR
 * @param	dir_hand	ALONG_DIR
 * @retval	int		移動するべき方向
 */
//--------------------------------------------------------------
static int AlongWall_MoveHitCheck( FIELD_OBJ_PTR fldobj, int dir_move, ALONG_DIR dir_hand )
{
	if( AlongWall_HandWallCheck(fldobj,dir_move,dir_hand) == FALSE ){	//壁が無い
		if( AlongWall_HandLostWallCheck(fldobj,dir_move,dir_hand) == FALSE ){
			return( DIR_NOT );									//手探りでも壁が無い
		}
		
		dir_move = DATA_DirHandLostDir[dir_move][dir_hand];		//壁発見 方向切り替え
	}
	
	return( dir_move );
}

//--------------------------------------------------------------
/**
 * 壁伝い移動チェック
 * @param	fldobj	FIELD_OBJ_PTR
 * @param	dir_move	移動方向 DIR_NOT=移動不可
 * @param	dir_hand	壁側の手の向き
 * @retval	u32		ヒットビット
 */
//--------------------------------------------------------------
static u32 AlongWall_WallMoveCheck( FIELD_OBJ_PTR fldobj, int *dir_move, ALONG_DIR dir_hand )
{
	u32 ret;
	
	*dir_move = AlongWall_MoveHitCheck( fldobj, *dir_move, dir_hand );
	
	if( *dir_move != DIR_NOT ){
		ret = FieldOBJ_MoveHitCheckDir( fldobj, *dir_move );
		return( ret );
	}
	
	return( 0 );
}

//--------------------------------------------------------------
/**
 * 壁伝い移動
 * @param	fldobj	FIELD_OBJ_PTR
 * @param	work	MV_ALONGW_WORK
 * @retval	int		TRUE=移動した
 */
//--------------------------------------------------------------
static int AlongWall_WallMove( FIELD_OBJ_PTR fldobj, MV_ALONGW_WORK *work, int ac )
{
	u32 ret;
	int dir_hand = work->dir_hand;
	int dir_move = FieldOBJ_DirDispGet( fldobj );
	
	ret = AlongWall_WallMoveCheck( fldobj, &dir_move, dir_hand );
	
	if( dir_move == DIR_NOT ){					//壁がない
		dir_move = FieldOBJ_DirDispGet( fldobj );
		ac = FieldOBJ_AcmdCodeDirChange( dir_move, AC_STAY_WALK_U_16F );
		FieldOBJ_CmdSet( fldobj, ac );
		return( FALSE );
	}
	
	if( ret == FLDOBJ_MOVE_HIT_BIT_NON ){		//移動可能
		ac = FieldOBJ_AcmdCodeDirChange( dir_move, ac );
		FieldOBJ_StatusBitON_Move( fldobj );
		FieldOBJ_CmdSet( fldobj, ac );
		return( TRUE );
	}
	
	//移動制限ヒット&両手利き　反転を試みる
	if( (ret & FLDOBJ_MOVE_HIT_BIT_LIM) && work->dir_hand_init == ALONG_LR ){
		dir_move = FieldOBJTool_DirFlip( FieldOBJ_DirDispGet(fldobj) );
		dir_hand = DATA_DirHandFlip[dir_hand];
		work->dir_hand = dir_hand;
		
		ret = AlongWall_WallMoveCheck( fldobj, &dir_move, dir_hand );
		
		if( dir_move == DIR_NOT ){					//壁がない
			dir_move = FieldOBJ_DirDispGet( fldobj );
			ac = FieldOBJ_AcmdCodeDirChange( dir_move, AC_STAY_WALK_U_16F );
			FieldOBJ_CmdSet( fldobj, ac );
			return( FALSE );
		}
		
		if( ret == FLDOBJ_MOVE_HIT_BIT_NON ){		//移動可能
			ac = FieldOBJ_AcmdCodeDirChange( dir_move, ac );
			FieldOBJ_StatusBitON_Move( fldobj );
			FieldOBJ_CmdSet( fldobj, ac );
			return( TRUE );
		}
	}
	
	//壁ヒット　移動方向変更
	if( (ret & FLDOBJ_MOVE_HIT_BIT_ATTR) ){
		dir_move = DATA_DirHandAttrHitDir[dir_move][dir_hand];
		
		ret = AlongWall_WallMoveCheck( fldobj, &dir_move, dir_hand );
		
		if( dir_move == DIR_NOT ){					//壁が無い
			dir_move = FieldOBJ_DirDispGet( fldobj );
			ac = FieldOBJ_AcmdCodeDirChange( dir_move, AC_STAY_WALK_U_16F );
			FieldOBJ_CmdSet( fldobj, ac );
			return( FALSE );
		}
		
		if( ret == FLDOBJ_MOVE_HIT_BIT_NON ){		//移動可能
			ac = FieldOBJ_AcmdCodeDirChange( dir_move, ac );
			FieldOBJ_StatusBitON_Move( fldobj );
			FieldOBJ_CmdSet( fldobj, ac );
			return( TRUE );
		}
	}
	
	//移動不可
	dir_move = FieldOBJ_DirDispGet( fldobj );	//向きを戻す
	ac = FieldOBJ_AcmdCodeDirChange( dir_move, AC_STAY_WALK_U_16F );
	FieldOBJ_CmdSet( fldobj, ac );
	return( FALSE );
}

