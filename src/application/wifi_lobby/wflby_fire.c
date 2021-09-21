//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wflby_fire.c
 *	@brief		WiFi広場花火処理
 *	@author		tomoya takahashi
 *	@data		2008.03.18
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


#include "common.h"

#include "wflby_fire.h"

#include "wflby_snd.h"

//-----------------------------------------------------------------------------
/**
 *					コーディング規約
 *		●関数名
 *				１文字目は大文字それ以降は小文字にする
 *		●変数名
 *				・変数共通
 *						constには c_ を付ける
 *						staticには s_ を付ける
 *						ポインタには p_ を付ける
 *						全て合わさると csp_ となる
 *				・グローバル変数
 *						１文字目は大文字
 *				・関数内変数
 *						小文字と”＿”と数字を使用する 関数の引数もこれと同じ
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	交互動作の定数
//=====================================
#define WFLBY_FIRE_CONT_KOUGO_ONEANM_FRAME	( 14 )
#define WFLBY_FIRE_CONT_KOUGO_ANM_SPEED		( FX32_ONE )
#define WFLBY_FIRE_CONT_KOUGO_ANM_SEQ_NUM	( 4 )
enum{
	WFLBY_FIRE_CONT_KOUGO_ON,
	WFLBY_FIRE_CONT_KOUGO_OFF,
	WFLBY_FIRE_CONT_KOUGO_FLAG_NUM,
};

// 交互用花火インデックス
// 交互花火が一番きれいに見えるように調整したインデックス
// マップ情報が更新されたら整合性が変わるので注意が必要です。
// 汎用性のまったく無いテーブルです。
//
// [花火配置図]
//
//    1			2		0
//	9	 10
//
//	3
// 4	  
//		    15   16
//  11					12
//		5 13 6	7 14 8
//
//			入り口
static const u8 sc_WFLBY_FIRE_CONT_KOUGO_FIRE_IDX[] = {
	6, 13, 5, 11,
	7, 14, 8, 12,
	1,  0, 2, 10,
	16, 9, 4, 3, 
	15, 
};



//-------------------------------------
///	波状発射定数
//=====================================
#define WFLBY_FIRE_CONT_NAMI_ONEANM_FRAME	( 6 )
#define WFLBY_FIRE_CONT_NAMI_SEQ_NUM		( 7 )
enum{
	WFLBY_FIRE_CONT_NAMI_ON,
	WFLBY_FIRE_CONT_NAMI_OFF,
	WFLBY_FIRE_CONT_NAMI_FLAG_NUM,
};
// 波用花火発射データ
// [花火配置図]参照
static const u8 sc_WFLBY_FIRE_CONT_NAMI_00_IDX[] = {
	5,  13, 6, 7, 14, 8,
};
static const u8 sc_WFLBY_FIRE_CONT_NAMI_01_IDX[] = {
	11, 12,
};
static const u8 sc_WFLBY_FIRE_CONT_NAMI_02_IDX[] = {
	15, 16,	
};
static const u8 sc_WFLBY_FIRE_CONT_NAMI_03_IDX[] = {
	4,	
};
static const u8 sc_WFLBY_FIRE_CONT_NAMI_04_IDX[] = {
	3,
};
static const u8 sc_WFLBY_FIRE_CONT_NAMI_05_IDX[] = {
	9, 10,
};
static const u8 sc_WFLBY_FIRE_CONT_NAMI_06_IDX[] = {
	1, 2, 0,
};
// 波シーケンスごとの　発射　停止　フラグ
static const u8 sc_WFLBY_FIRE_CONT_NAMI_ONOFF_FLAG[ WFLBY_FIRE_CONT_NAMI_SEQ_NUM ][ WFLBY_FIRE_CONT_NAMI_FLAG_NUM ] = {
	{ 0, 5 },
	{ 1, 6 },
	{ 2, 0 },
	{ 3, 1 },
	{ 4, 2 },
	{ 5, 3 },
	{ 6, 4 },
};




//-------------------------------------
///	クライマックス
//=====================================
// 再生データ
typedef struct {
	u32		start_count;
	fx32	anm_speed;
} WFLBY_FIRECONT_CLIMAX_ANMDATA;

#define WFLBY_FIRECONT_CLIMAX_ANMNUM	(3)
static const WFLBY_FIRECONT_CLIMAX_ANMDATA sc_WFLBY_FIRECONT_CLIMAX_ANMDATA[WFLBY_FIRECONT_CLIMAX_ANMNUM] = {
	{ 10, FX32_ONE },
	{ 45, FX32_ONE },
	{ 80, FX32_HALF },
};

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	各動作タイプのワーク
//=====================================
typedef union {
	struct{
		u32 seq;
	} kougo;

	struct{
		u32 seq;
	} nami;

	struct{
		u32 anmidx;
	} climax;
} WFLBY_FIRECONT_WK;

//-------------------------------------
///	波テーブルデータ
//=====================================
typedef struct {
	const u8* cp_tbl;
	u32	tblnum;
} WFLBY_FIRECONT_NAMI_TBL;



//-------------------------------------
///	花火管理システム
//=====================================
typedef struct _WFLBY_FIRE_CONT {
	const WFLBY_SYSTEM*		cp_system;
	WFLBY_3DMAPOBJ_CONT*	p_mapobjcont;

	u8	now_move_type;
	u8	fire_se;
	u16 last_hanabi_st;

	WFLBY_FIRECONT_WK move_wk;	// 動作ワーク
} WFLBY_FIRE_CONT;

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
static void WFLBY_FIRE_CONT_Move( WFLBY_FIRE_CONT* p_sys, u32 move_type, u32 count );
static void WFLBY_FIRE_CONT_MoveNone( WFLBY_FIRE_CONT* p_sys, u32 count );
static void WFLBY_FIRE_CONT_MoveNormal( WFLBY_FIRE_CONT* p_sys, u32 count );
static void WFLBY_FIRE_CONT_MoveKougo( WFLBY_FIRE_CONT* p_sys, u32 count );
static void WFLBY_FIRE_CONT_MoveNami( WFLBY_FIRE_CONT* p_sys, u32 count );
static void WFLBY_FIRE_CONT_MoveClimax( WFLBY_FIRE_CONT* p_sys, u32 count );

static void WFLBY_FIRE_CONT_Init( WFLBY_FIRE_CONT* p_sys, u32 move_type, u32 count );
static void WFLBY_FIRE_CONT_InitNone( WFLBY_FIRE_CONT* p_sys, u32 count );
static void WFLBY_FIRE_CONT_InitNormal( WFLBY_FIRE_CONT* p_sys, u32 count );
static void WFLBY_FIRE_CONT_InitKougo( WFLBY_FIRE_CONT* p_sys, u32 count );
static void WFLBY_FIRE_CONT_InitNami( WFLBY_FIRE_CONT* p_sys, u32 count );
static void WFLBY_FIRE_CONT_InitClimax( WFLBY_FIRE_CONT* p_sys, u32 count );



//-------------------------------------
///	波状発射
//=====================================
static void WFLBY_FIRE_CONT_Nami_GetTblData( u32 idx, WFLBY_FIRECONT_NAMI_TBL* p_buff );


//-------------------------------------
///	花火の音管理
//=====================================
static void WFLBY_FIRE_CONT_StartFireSe( WFLBY_FIRE_CONT* p_wk );
static void WFLBY_FIRE_CONT_StopFireSe( WFLBY_FIRE_CONT* p_wk );


//----------------------------------------------------------------------------
/**
 *	@brief	花火イベント管理システム		初期化
 *
 *	@param	cp_system		システム	
 *	@param	p_mapobjcont	配置オブジェ管理システム
 *	@param	heapID			ヒープID
 *
 *	@return	作成したシステム
 */
//-----------------------------------------------------------------------------
WFLBY_FIRE_CONT* WFLBY_FIRECONT_Init( const WFLBY_SYSTEM* cp_system, WFLBY_3DMAPOBJ_CONT* p_mapobjcont, u32 heapID )
{
	WFLBY_FIRE_CONT* p_wk;
	u32 move_type;
	
	p_wk = sys_AllocMemory( heapID, sizeof(WFLBY_FIRE_CONT) );
	memset( p_wk, 0, sizeof(WFLBY_FIRE_CONT) );

	p_wk->cp_system		= cp_system;
	p_wk->p_mapobjcont	= p_mapobjcont;

	// 花火中のときだけ初期化
	move_type				= WFLBY_SYSTEM_FIRE_GetType( cp_system );
	p_wk->last_hanabi_st	= WFLBY_SYSTEM_Event_GetHanabi( cp_system );
	if( (WFLBY_EVENT_HANABI_BEFORE != p_wk->last_hanabi_st) && (move_type != WFLBY_FIRE_END) ){

		u32 count;

		// 銅像から花火を出す
		WFLBY_3DMAPOBJCONT_MAP_SetDouzouFire( p_mapobjcont, TRUE );

		//SE
		WFLBY_FIRE_CONT_StartFireSe( p_wk );

		// 今の状態を見て、初期化する
		count		= WFLBY_SYSTEM_FIRE_GetTypeMoveCount( cp_system );
		WFLBY_FIRE_CONT_Init( p_wk, move_type, count );
	}

	return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief	花火イベント管理システム		破棄
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_FIRECONT_Exit( WFLBY_FIRE_CONT* p_wk )
{
	WFLBY_FIRE_CONT_StopFireSe( p_wk );
	sys_FreeMemoryEz( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	花火イベント管理システム	メイン
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_FIRECONT_Main( WFLBY_FIRE_CONT* p_wk )
{
	u32 hanabi_st;
	u32 move_type;
	u32 count;

	move_type	= WFLBY_SYSTEM_FIRE_GetType( p_wk->cp_system );
	count		= WFLBY_SYSTEM_FIRE_GetTypeMoveCount( p_wk->cp_system );
	hanabi_st	= WFLBY_SYSTEM_Event_GetHanabi( p_wk->cp_system );

	// 花火状態が変更したかチェック
	if( p_wk->last_hanabi_st != hanabi_st ){
		p_wk->last_hanabi_st = hanabi_st;

		if( hanabi_st == WFLBY_EVENT_HANABI_PLAY ){

			// 花火が開始された

			// 銅像から花火を出す
			WFLBY_3DMAPOBJCONT_MAP_SetDouzouFire( p_wk->p_mapobjcont, TRUE );

			// 動作初期化
			WFLBY_FIRE_CONT_Init( p_wk, move_type, count );

			//SE
			WFLBY_FIRE_CONT_StartFireSe( p_wk );
		}
	}



	// 花火前以外の状態の場合だけ
	// メイン動作させる
	// move_typeが終わりになったら勝手に終わる
	if( hanabi_st != WFLBY_EVENT_HANABI_BEFORE ){

		// 動作が変わったら初期化
		if( p_wk->now_move_type != move_type ){
			// 動作初期化
			WFLBY_FIRE_CONT_Init( p_wk, move_type, count );
		}

		// 動作
		WFLBY_FIRE_CONT_Move( p_wk, move_type, count );
	}
	
}

//----------------------------------------------------------------------------
/**
 *	@brief	花火イベント管理	SE停止
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_FIRECONT_StopFireSe( WFLBY_FIRE_CONT* p_wk )
{
	WFLBY_FIRE_CONT_StopFireSe( p_wk );
}






//-----------------------------------------------------------------------------
/**
 *			プライベート関数
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	花火の動作
 *
 *	@param	p_sys		ワーク
 *	@param	move_type	動作タイプ
 *	@param	count		カウンタ
 */
//-----------------------------------------------------------------------------
static void WFLBY_FIRE_CONT_Move( WFLBY_FIRE_CONT* p_sys, u32 move_type, u32 count )
{
	static void (* const pFunc[])( WFLBY_FIRE_CONT* p_sys, u32 count ) = {
		WFLBY_FIRE_CONT_MoveNone,
		WFLBY_FIRE_CONT_MoveNone,
		WFLBY_FIRE_CONT_MoveNormal,
		WFLBY_FIRE_CONT_MoveKougo,
		WFLBY_FIRE_CONT_MoveNami,
		WFLBY_FIRE_CONT_MoveClimax,
	};

	GF_ASSERT( move_type < WFLBY_FIRE_NUM );
	if( move_type < WFLBY_FIRE_NUM ){
		pFunc[ move_type ]( p_sys, count );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	花火タイプに対応した動作関数
 *
 *	@param	p_sys			ワーク
 *	@param	count			今の動作カウンタ
 */
//-----------------------------------------------------------------------------
// 動きなし
static void WFLBY_FIRE_CONT_MoveNone( WFLBY_FIRE_CONT* p_sys, u32 count )
{
}
// 通常あにめ
static void WFLBY_FIRE_CONT_MoveNormal( WFLBY_FIRE_CONT* p_sys, u32 count )
{
}
//  交互発射
static void WFLBY_FIRE_CONT_MoveKougo( WFLBY_FIRE_CONT* p_sys, u32 count )
{
	BOOL result;
	u32 idx;
	BOOL flag;
	BOOL set_fire_flag;
	u32 kougo_idx;
	static const u32 sc_Flag[ WFLBY_FIRE_CONT_KOUGO_ANM_SEQ_NUM ][ WFLBY_FIRE_CONT_KOUGO_FLAG_NUM ] = {
		{ 0, 2 },
		{ 1, 3 },
		{ 2, 0 },
		{ 3, 1 },
	};

#if 0
	// 指定した位置の花火をつける
	if( sys.trg & PAD_BUTTON_R ){
		static s32 hanabi_no;
		static s32 last_hanabi_no;

		OS_TPrintf( "hanabi_no %d\n", hanabi_no );

		WFLBY_3DMAPOBJCONT_MAP_SetFloorLightFireIdx( p_sys->p_mapobjcont, 
				last_hanabi_no, FALSE, WFLBY_FIRE_CONT_KOUGO_ANM_SPEED );

		result = WFLBY_3DMAPOBJCONT_MAP_SetFloorLightFireIdx( p_sys->p_mapobjcont, 
				hanabi_no, TRUE, WFLBY_FIRE_CONT_KOUGO_ANM_SPEED );

		last_hanabi_no = hanabi_no;
		
		if( result == FALSE ){
			hanabi_no = 0;
		}else{
			hanabi_no ++;
		}
	}
#endif

#if 1
	if( (count % WFLBY_FIRE_CONT_KOUGO_ONEANM_FRAME) == 0 ){


		// アニメ設定
		idx = 0;
		do{
			
			// 花火の操作を行うか
			set_fire_flag = FALSE;

			// seqで 発射、そのまま、とめる を切り替える
			if( (idx % WFLBY_FIRE_CONT_KOUGO_ANM_SEQ_NUM) == sc_Flag[ p_sys->move_wk.kougo.seq ][ WFLBY_FIRE_CONT_KOUGO_ON ] ){
				flag = TRUE;
				set_fire_flag = TRUE;
			}
			else if( (idx % WFLBY_FIRE_CONT_KOUGO_ANM_SEQ_NUM) == sc_Flag[ p_sys->move_wk.kougo.seq ][ WFLBY_FIRE_CONT_KOUGO_OFF ] ){
				flag = FALSE;
				set_fire_flag = TRUE;
			}
			
			if( set_fire_flag == TRUE ){

				// インデックスを交互花火構成インデックスに変更
				if( idx < NELEMS(sc_WFLBY_FIRE_CONT_KOUGO_FIRE_IDX) ){
					kougo_idx = sc_WFLBY_FIRE_CONT_KOUGO_FIRE_IDX[idx];
					
					result = WFLBY_3DMAPOBJCONT_MAP_SetFloorLightFireIdx( p_sys->p_mapobjcont, 
							kougo_idx, flag, WFLBY_FIRE_CONT_KOUGO_ANM_SPEED );
					GF_ASSERT( result == TRUE );	// 花火構成データがあっていない
				}else{
					result = FALSE;
				}
			}else{
				result = TRUE;
			}

			idx ++;
		}while( result == TRUE );

		p_sys->move_wk.kougo.seq = (p_sys->move_wk.kougo.seq + 1) % WFLBY_FIRE_CONT_KOUGO_ANM_SEQ_NUM;
	}
#endif
}
// 波状発射
static void WFLBY_FIRE_CONT_MoveNami( WFLBY_FIRE_CONT* p_sys, u32 count )
{
	int i;
	u32	off, on;
	WFLBY_FIRECONT_NAMI_TBL nami_tbl;	// 波テーブルデータ
	BOOL result;
	
	if( (count % WFLBY_FIRE_CONT_NAMI_ONEANM_FRAME) == 0 ){


		// ON/OFF情報取得
		off = sc_WFLBY_FIRE_CONT_NAMI_ONOFF_FLAG[ p_sys->move_wk.nami.seq ][ WFLBY_FIRE_CONT_NAMI_OFF ];
		on	= sc_WFLBY_FIRE_CONT_NAMI_ONOFF_FLAG[ p_sys->move_wk.nami.seq ][ WFLBY_FIRE_CONT_NAMI_ON ];

		
		// アニメ設定
		// アニメOFF
		WFLBY_FIRE_CONT_Nami_GetTblData( off, &nami_tbl );
		for( i=0; i<nami_tbl.tblnum; i++ ){
			result = WFLBY_3DMAPOBJCONT_MAP_SetFloorLightFireIdx( p_sys->p_mapobjcont, 
					nami_tbl.cp_tbl[i], FALSE, WFLBY_FIRE_CONT_KOUGO_ANM_SPEED );
			GF_ASSERT( result == TRUE );	// 花火構成データがあっていない
		}

		// アニメON
		WFLBY_FIRE_CONT_Nami_GetTblData( on, &nami_tbl );
		for( i=0; i<nami_tbl.tblnum; i++ ){
			result = WFLBY_3DMAPOBJCONT_MAP_SetFloorLightFireIdx( p_sys->p_mapobjcont, 
					nami_tbl.cp_tbl[i], TRUE, WFLBY_FIRE_CONT_KOUGO_ANM_SPEED );
			GF_ASSERT( result == TRUE );	// 花火構成データがあっていない
		}
		
		p_sys->move_wk.nami.seq = (p_sys->move_wk.nami.seq + 1) % WFLBY_FIRE_CONT_NAMI_SEQ_NUM;

	}
}
// クライマックス
static void WFLBY_FIRE_CONT_MoveClimax( WFLBY_FIRE_CONT* p_sys, u32 count )
{
	if( p_sys->move_wk.climax.anmidx < WFLBY_FIRECONT_CLIMAX_ANMNUM ){
		
		if( sc_WFLBY_FIRECONT_CLIMAX_ANMDATA[p_sys->move_wk.climax.anmidx].start_count < count ){
			// 発射！
			WFLBY_3DMAPOBJCONT_MAP_SetFloorLightBigFire( p_sys->p_mapobjcont, 
					sc_WFLBY_FIRECONT_CLIMAX_ANMDATA[p_sys->move_wk.climax.anmidx].anm_speed );

			WFLBY_3DMAPOBJCONT_MAP_SetDouzouBigFire( p_sys->p_mapobjcont, 
					sc_WFLBY_FIRECONT_CLIMAX_ANMDATA[p_sys->move_wk.climax.anmidx].anm_speed );

			Snd_SePlay( WFLBY_SND_FIREWORKS02 );

			// 次のアニメへ
			p_sys->move_wk.climax.anmidx ++;
		}
		
	}
	
}


//----------------------------------------------------------------------------
/**
 *	@brief	花火の初期化
 *
 *	@param	p_sys		ワーク		
 *	@param	move_type	動作タイプ
 *	@param	count		カウンタ
 */
//-----------------------------------------------------------------------------
static void WFLBY_FIRE_CONT_Init( WFLBY_FIRE_CONT* p_sys, u32 move_type, u32 count )
{
	static void (* const pFunc[])( WFLBY_FIRE_CONT* p_sys, u32 count ) = {
		WFLBY_FIRE_CONT_InitNone,
		WFLBY_FIRE_CONT_InitNone,
		WFLBY_FIRE_CONT_InitNormal,
		WFLBY_FIRE_CONT_InitKougo,
		WFLBY_FIRE_CONT_InitNami,
		WFLBY_FIRE_CONT_InitClimax,
	};

	GF_ASSERT( move_type < WFLBY_FIRE_NUM );
	if( move_type < WFLBY_FIRE_NUM ){

		// ワークのクリア
		memset( &p_sys->move_wk, 0, sizeof(WFLBY_FIRECONT_WK) );
		
		pFunc[ move_type ]( p_sys, count );
		p_sys->now_move_type = move_type;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	花火動作タイプに対応した動きの初期化
 *
 *	@param	p_sys		システム
 *	@param	count		カウント値
 */
//-----------------------------------------------------------------------------
static void WFLBY_FIRE_CONT_InitNone( WFLBY_FIRE_CONT* p_sys, u32 count )
{
	// 全アニメをとめる
	WFLBY_3DMAPOBJCONT_MAP_SetFloorLightFire( p_sys->p_mapobjcont, FALSE, FX32_ONE );
}
// 通常アニメ
static void WFLBY_FIRE_CONT_InitNormal( WFLBY_FIRE_CONT* p_sys, u32 count )
{
	// 全アニメ発射
	WFLBY_3DMAPOBJCONT_MAP_SetFloorLightFire( p_sys->p_mapobjcont, TRUE, FX32_ONE );
}
// 交互発射
static void WFLBY_FIRE_CONT_InitKougo( WFLBY_FIRE_CONT* p_sys, u32 count )
{
	// そのうち発射されるはずなので今は全花火をOFF
	WFLBY_3DMAPOBJCONT_MAP_SetFloorLightFire( p_sys->p_mapobjcont, FALSE, FX32_ONE );
}
// 波状
static void WFLBY_FIRE_CONT_InitNami( WFLBY_FIRE_CONT* p_sys, u32 count )
{
	// そのうち発射されるはずなので今は全花火をOFF
	WFLBY_3DMAPOBJCONT_MAP_SetFloorLightFire( p_sys->p_mapobjcont, FALSE, FX32_ONE );
}
// クライマックス
static void WFLBY_FIRE_CONT_InitClimax( WFLBY_FIRE_CONT* p_sys, u32 count )
{
	//SE
	WFLBY_FIRE_CONT_StopFireSe( p_sys );
	
	// そのうち発射されるはずなので今は全花火をOFF
	WFLBY_3DMAPOBJCONT_MAP_SetFloorLightFire( p_sys->p_mapobjcont, FALSE, FX32_ONE );
	WFLBY_3DMAPOBJCONT_MAP_SetDouzouFire( p_sys->p_mapobjcont, FALSE );
}




//----------------------------------------------------------------------------
/**
 *	@brief	波テーブルデータ取得関数
 *
 *	@param	idx		波インデックス
 *	@param	p_buff	格納先
 */
//-----------------------------------------------------------------------------
static void WFLBY_FIRE_CONT_Nami_GetTblData( u32 idx, WFLBY_FIRECONT_NAMI_TBL* p_buff )
{
	switch( idx ){
	case 0:
		p_buff->tblnum	= NELEMS(sc_WFLBY_FIRE_CONT_NAMI_00_IDX);
		p_buff->cp_tbl	= sc_WFLBY_FIRE_CONT_NAMI_00_IDX;
		break;
		
	case 1:
		p_buff->tblnum	= NELEMS(sc_WFLBY_FIRE_CONT_NAMI_01_IDX);
		p_buff->cp_tbl	= sc_WFLBY_FIRE_CONT_NAMI_01_IDX;
		break;
		
	case 2:
		p_buff->tblnum	= NELEMS(sc_WFLBY_FIRE_CONT_NAMI_02_IDX);
		p_buff->cp_tbl	= sc_WFLBY_FIRE_CONT_NAMI_02_IDX;
		break;
		
	case 3:
		p_buff->tblnum	= NELEMS(sc_WFLBY_FIRE_CONT_NAMI_03_IDX);
		p_buff->cp_tbl	= sc_WFLBY_FIRE_CONT_NAMI_03_IDX;
		break;
		
	case 4:
		p_buff->tblnum	= NELEMS(sc_WFLBY_FIRE_CONT_NAMI_04_IDX);
		p_buff->cp_tbl	= sc_WFLBY_FIRE_CONT_NAMI_04_IDX;
		break;
		
	case 5:
		p_buff->tblnum	= NELEMS(sc_WFLBY_FIRE_CONT_NAMI_05_IDX);
		p_buff->cp_tbl	= sc_WFLBY_FIRE_CONT_NAMI_05_IDX;
		break;

	case 6:
		p_buff->tblnum	= NELEMS(sc_WFLBY_FIRE_CONT_NAMI_06_IDX);
		p_buff->cp_tbl	= sc_WFLBY_FIRE_CONT_NAMI_06_IDX;
		break;

	default:
		GF_ASSERT(0);
		break;
	}
}


//----------------------------------------------------------------------------
/**
 *	@brief	花火SE再生
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_FIRE_CONT_StartFireSe( WFLBY_FIRE_CONT* p_wk )
{
	if( p_wk->fire_se == FALSE ){
		Snd_SePlay( WFLBY_SND_FIREWORKS01 );
		p_wk->fire_se = TRUE;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	花火SE停止
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_FIRE_CONT_StopFireSe( WFLBY_FIRE_CONT* p_wk )
{
	if( p_wk->fire_se ){
		Snd_SeStopBySeqNo( WFLBY_SND_FIREWORKS01, 0 );
		p_wk->fire_se = FALSE;
	}
}
