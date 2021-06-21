//==============================================================================================
/**
 * @file	factory_csr.c
 * @brief	「バトルファクトリー」カーソル
 * @author	Satoshi Nohara
 * @date	2007.03.20
 */
//==============================================================================================
#include "common.h"
#include "src_os_print.h"
#include "system/arc_tool.h"
#include "system/arc_util.h"

#include "factory_sys.h"
#include "factory_clact.h"
#include "factory_csr.h"


//==============================================================================================
//
//	構造体宣言
//
//==============================================================================================
//カーソルOBJ
struct _FACTORY_CSR{
	u8	sel_max;					//選択の最大数
	u8	mode;						//横選択か、縦選択か
	u8	csr_pos;					//カーソル位置
	u8	pause;						//ポーズフラグ

	//POS* pos_tbl;					//カーソルが移動する座標テーブルのポインタ
	const FACTORY_POS* pos_tbl;		//カーソルが移動する座標テーブルのポインタ
	const u8* anm_tbl;				//カーソルアニメテーブルのポインタ
	CLACT_WORK_PTR p_clact;			//セルアクターワークポインタ

	u8 sel_h_max;					//横選択の最大数(sel_maxの中のいくつが横選択か)
};

//カーソルのパレット
enum{
	//PAL_CSR_MOVE = 0,
	//PAL_CSR_STOP,
	PAL_CSR_MOVE = 6,
	PAL_CSR_STOP = 6,
};


//==============================================================================================
//
//	プロトタイプ宣言
//
//==============================================================================================
FACTORY_CSR* FactoryCsr_Create( FACTORY_CLACT* factory_clact, u8 sel_max, u8 sel_h_max, u8 mode, u8 csr_pos, const FACTORY_POS* pos_tbl, const u8* anm_tbl );
void* FactoryCsr_Delete( FACTORY_CSR* wk );
void FactoryCsr_Vanish( FACTORY_CSR* wk, int flag );
void FactoryCsr_Move( FACTORY_CSR* wk );
u8 FactoryCsr_GetCsrPos( FACTORY_CSR* wk );
void FactoryCsr_Pause( FACTORY_CSR* wk, int flag );
void FactoryCsr_SetCsrPos( FACTORY_CSR* wk, u8 csr_pos );


//==============================================================================================
//
//	関数
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	カーソルOBJ作成
 *
 * @param	factory_clact	FACTORY_CLACT型のポインタ
 * @param	data_index		データindex
 * @param	heapID			ヒープID
 *
 * @retval	"FACTORY_CSRワークへのポインタ"
 */
//--------------------------------------------------------------
FACTORY_CSR* FactoryCsr_Create( FACTORY_CLACT* factory_clact, u8 sel_max, u8 sel_h_max, u8 mode, u8 csr_pos, const FACTORY_POS* pos_tbl, const u8* anm_tbl )
{
	FACTORY_CSR* wk;
	VecFx32	vec;

	wk = sys_AllocMemory( HEAPID_FACTORY, sizeof(FACTORY_CSR) );		//メモリ確保
	memset( wk, 0, sizeof(FACTORY_CSR) );

	wk->sel_max		= sel_max;				//選択最大数
	wk->sel_h_max	= sel_h_max;			//横選択の最大数
	wk->mode		= mode;					//横選択か、縦選択か
	wk->csr_pos		= csr_pos;				//カーソル位置
	wk->pos_tbl		= pos_tbl;				//カーソルが移動する座標テーブルのポインタ
	wk->anm_tbl		= anm_tbl;				//カーソルのアニメテーブルのポインタ

	//アニメテーブルの指定がある時
	if( anm_tbl != NULL ){
		wk->p_clact = FactoryClact_SetActor(	factory_clact, 0, 
												wk->anm_tbl[csr_pos], 0, 0, DISP_MAIN );
	}else{
		//現状、タイプは固定(data_no,anm_no,pri)
		if( mode == CSR_H_MODE ){
			wk->p_clact = FactoryClact_SetActor( factory_clact, 0, ANM_BALL_CSR, 0, 0, DISP_MAIN );
		}else{
			wk->p_clact = FactoryClact_SetActor( factory_clact, 0, ANM_MENU_CSR, 0, 0, DISP_MAIN );
		}
	}

	//初期カーソル座標をセット
	vec.x = (wk->pos_tbl[wk->csr_pos].x * FX32_ONE);
	vec.y = (wk->pos_tbl[wk->csr_pos].y * FX32_ONE);
	CLACT_SetMatrix( wk->p_clact, &vec );

	return wk;
}

//--------------------------------------------------------------
/**
 * @brief	カーソルOBJワーク削除
 *
 * @param	wk		FACTORY_CSRワークのポインタ
 *
 * @retval	"NULL"
 */
//--------------------------------------------------------------
void* FactoryCsr_Delete( FACTORY_CSR* wk )
{
	CLACT_Delete( wk->p_clact );

	//FACTORY_CSRのメンバでメモリ確保したものがあったら開放する

	sys_FreeMemoryEz( wk );
	return NULL;
}

//--------------------------------------------------------------
/**
 * @brief	バニッシュ操作
 *
 * @param	wk		FACTORY_CSR型のポインタ
 * @param	flag	0:非描画 1:レンダラ描画	
 *
 * @return	none
 */
//--------------------------------------------------------------
void FactoryCsr_Vanish( FACTORY_CSR* wk, int flag )
{
	CLACT_SetDrawFlag( wk->p_clact, flag );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	移動
 *
 * @param	wk		FACTORY_CSR型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
void FactoryCsr_Move( FACTORY_CSR* wk )
{
	VecFx32	vec;
	u32	check_key1,check_key2;

	//ポーズ中は何もしない
	if( wk->pause == 1 ){
		return;
	}

	if( sys.trg == 0 ){
		return;
	}

	//HVに対応させる特殊なモード
	if( wk->mode == CSR_HV_MODE ){

		if( sys.trg & PAD_KEY_LEFT ){
			if( wk->csr_pos == 0 ){
				wk->csr_pos = (wk->sel_max-1);
			}else{
				wk->csr_pos--;
			}
		}else if( sys.trg & PAD_KEY_RIGHT ){
			if( wk->csr_pos == (wk->sel_max-1) ){
				wk->csr_pos = 0;
			}else{
				wk->csr_pos++;
			}
		}else if( sys.trg & PAD_KEY_DOWN ){
			if( wk->csr_pos < (wk->sel_h_max) ){		//下にカーソル移動
				wk->csr_pos = wk->sel_h_max;
			}else if( wk->csr_pos == (wk->sel_max-1) ){	//すでに下にカーソルがある時は、左に戻る
				wk->csr_pos = 0;
			}else{
				wk->csr_pos++;
			}
		}else if( sys.trg & PAD_KEY_UP ){
			if( wk->csr_pos < (wk->sel_h_max) ){		//上にカーソル移動
				wk->csr_pos = (wk->sel_max-1);
			}else{										//下にカーソル移動
				wk->csr_pos--;
			}
		}

	//------------------------------------------------------------------------------------------
	}else{

		//チェックするキー
		if( wk->mode == CSR_H_MODE ){
			check_key1 = PAD_KEY_RIGHT;
			check_key2 = PAD_KEY_LEFT;
		}else{
			check_key1 = PAD_KEY_DOWN;
			check_key2 = PAD_KEY_UP;
		}

		//キー対応する処理
		if( sys.trg & check_key1 ){
			wk->csr_pos++;
			if( wk->csr_pos >= wk->sel_max ){
				wk->csr_pos = 0;
			}
		}else if( sys.trg & check_key2 ){
			if( wk->csr_pos == 0 ){
			wk->csr_pos = wk->sel_max;
				}
			wk->csr_pos--;
		}
	}

	//アニメを切り替える
	if( wk->anm_tbl != NULL ){
		CLACT_AnmChgCheck( wk->p_clact, wk->anm_tbl[wk->csr_pos] );
	}

	//座標を取得
	vec = *( CLACT_GetMatrix(wk->p_clact) );
	vec.x = (wk->pos_tbl[wk->csr_pos].x * FX32_ONE);
	vec.y = (wk->pos_tbl[wk->csr_pos].y * FX32_ONE);

	//座標を設定
	CLACT_SetMatrix( wk->p_clact, &vec );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	カーソル位置を取得
 *
 * @param	wk		FACTORY_CSR型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
u8 FactoryCsr_GetCsrPos( FACTORY_CSR* wk )
{
	return wk->csr_pos;
}

//--------------------------------------------------------------
/**
 * @brief	ポーズ
 *
 * @param	wk		FACTORY_CSR型のポインタ
 * @param	flag	0:ポーズ解除 1:ポーズ
 *
 * @return	none
 */
//--------------------------------------------------------------
void FactoryCsr_Pause( FACTORY_CSR* wk, int flag )
{
	wk->pause = flag;

	if( flag == 0 ){
		CLACT_AnmChgCheck( wk->p_clact, ANM_BALL_CSR );
		//CLACT_PaletteNoChg( wk->p_clact, PAL_CSR_MOVE );		//p_csr以外渡すとダメ。。。
	}else{
		CLACT_AnmChgCheck( wk->p_clact, ANM_BALL_CSR_STOP );
		//CLACT_PaletteNoChg( wk->p_clact, PAL_CSR_STOP );		//p_csr以外渡すとダメ。。。
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	カーソル位置をセット
 *
 * @param	wk		FACTORY_CSR型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
void FactoryCsr_SetCsrPos( FACTORY_CSR* wk, u8 csr_pos )
{
	VecFx32	vec;

	wk->csr_pos = csr_pos;

	//アニメを切り替える
	if( wk->anm_tbl != NULL ){
		CLACT_AnmChgCheck( wk->p_clact, wk->anm_tbl[wk->csr_pos] );
	}

	//座標を取得
	vec = *( CLACT_GetMatrix(wk->p_clact) );
	vec.x = (wk->pos_tbl[wk->csr_pos].x * FX32_ONE);
	vec.y = (wk->pos_tbl[wk->csr_pos].y * FX32_ONE);

	//座標を設定
	CLACT_SetMatrix( wk->p_clact, &vec );
	return;
}

