//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		bct_surver.c
 *	@brief		サーバー
 *	@author		tomoya takahashi
 *	@data		2007.06.19
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "common.h"
#include "bct_surver.h"

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

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	サーバーワーク
//=====================================
typedef struct _BCT_SURVER{
	s32 time;
	BOOL countdown;	// カウントダウンするか
	u32 comm_num;
	BCT_SCORE_COMM score;
	u32 score_msk;						// スコアを格納したNETIDマスク
	u32 ranktbl[ BCT_PLAYER_NUM ];		// 順位計算用テーブル
	const BCT_GAMEDATA* cp_gamedata;	// ゲームデータ
	u16 nutsin_count;					// 木の実が入った数
	u8	now_tbl_idx;					// 今のゲーム段階 
	u8	tbl_idx_change;					// テーブルのインでクスが変わった
} ;

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
static u32 BCT_SURVER_ScoreSetNumGet( const BCT_SURVER* cp_wk );


//----------------------------------------------------------------------------
/**
 *	@brief	サーバーワークの作成
 *
 *	@param	heapID		使用するヒープ
 *	@param	timeover	時間制限
 *	@param	comm_num	通信人数
 *	@param	cp_gamedata	ゲームデータ
 *
 *	@return	作成したワーク
 */
//-----------------------------------------------------------------------------
BCT_SURVER* BCT_SURVER_Init( u32 heapID, u32 timeover, u32 comm_num, const BCT_GAMEDATA* cp_gamedata )
{
	BCT_SURVER* p_wk;

	p_wk = sys_AllocMemory( heapID, sizeof(BCT_SURVER) );
	memset( p_wk, 0, sizeof(BCT_SURVER) );

	p_wk->time			= timeover;
	p_wk->comm_num		= comm_num;
	p_wk->cp_gamedata	= cp_gamedata;
	p_wk->countdown		= TRUE;
	
	return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief	サーバーワークの破棄
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
void BCT_SURVER_Delete( BCT_SURVER* p_wk )
{
	sys_FreeMemoryEz( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	サーバーワークメイン
 *
 *	@param	p_wk	ワーク
 *
 *	@retval	TRUE	途中
 *	@retval	FALSE	終了
 *
// FALSEが帰ってきたら終了
// 終了命令とスコアをみんなに送る
 */
//-----------------------------------------------------------------------------
BOOL BCT_SURVER_Main( BCT_SURVER* p_wk )
{
	int i;
	
#ifndef BCT_DEBUG_NOT_TIMECOUNT
	if( (p_wk->time > 0) && (p_wk->countdown == TRUE) ){
		p_wk->time --;
	}
#endif
	if( p_wk->time <= 0 ){
		return FALSE;
	}

	// 次の段階チェック
	for( i=p_wk->now_tbl_idx; i<p_wk->cp_gamedata->tblnum; i++ ){
		if( p_wk->cp_gamedata->p_tbl[i].nuts_num >= p_wk->nutsin_count ){
			break;
		}
	}

	// インデックスがp_wk->cp_gamedata.tblnumになってしまったら
	// １つ戻す
	if( i == p_wk->cp_gamedata->tblnum ){
		i--;
	}
	// インデックスが変わったら設定
	if( i != p_wk->now_tbl_idx ){
		p_wk->now_tbl_idx = i;

		// さらに段階が変わったフラグを設定
		p_wk->tbl_idx_change = TRUE;
//		OS_TPrintf( "game data idx chenge %d\n", p_wk->now_tbl_idx );
	}
	
	return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ゲームレベルの変更をチェック
 *
 *	@param	cp_wk	ワーク
 *
 *	@retval	TRUE	レベルが変わってる
 *	@retval	FALSE	レベルが変わってない
 */
//-----------------------------------------------------------------------------
BOOL BCT_SURVER_CheckGameLevelChange( const BCT_SURVER* cp_wk )
{
	return cp_wk->tbl_idx_change;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ゲームレベルの変更フラグをクリア
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
void BCT_SURVER_ClearGameLevelChange( BCT_SURVER* p_wk )
{
	p_wk->tbl_idx_change = FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ゲームレベルの変更フラグをクリア
 *
 *	@param	cp_wk	ワーク
 *
 *	@return	レベル
 */
//-----------------------------------------------------------------------------
u32	BCT_SURVER_GetGameLevel( const BCT_SURVER* cp_wk )
{
	return cp_wk->now_tbl_idx;
}

//----------------------------------------------------------------------------
/**
 *	@brief	受信木の実データ設定
 *
 *	@param	p_wk	ワーク
 *	@param	cp_data	木の実データ
 *	@param	plno	ネットID
 */
//-----------------------------------------------------------------------------
void BCT_SURVER_SetNutData( BCT_SURVER* p_wk, const BCT_NUT_COMM* cp_data, u32 plno )
{
	if( cp_data->in_flag ){
		// 個数カウント
		p_wk->nutsin_count ++;
//		OS_TPrintf( "nutsin count %d\n", p_wk->nutsin_count );
	} 
}

//----------------------------------------------------------------------------
/**
 *	@brief	スコアを設定する
 *
 *	@param	p_wk		ワーク
 *	@param	score		スコア
 *	@param	plno		ネットID
 */
//-----------------------------------------------------------------------------
void BCT_SURVER_ScoreSet( BCT_SURVER* p_wk, u32 score, u32 plno )
{
	int innum;
	int i;

	// 最大値オーバーチェック
	if( score > BCT_SCORE_MAX ){
		score = BCT_SCORE_MAX;
	}
	
	// まだ登録していないかチェック
	if( (p_wk->score_msk & (1 << plno)) == 0 ){

		p_wk->score.score[ plno ] = score;
//		OS_Printf( "p_wk->score_msk %x  score = %d\n", p_wk->score_msk, score );
		// 取得完了
		p_wk->score_msk |= (1 << plno);
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	みんなからスコアを受信したかをチェックする
 *
 *	@param	 cp_wk		ワーク
 *
 *	@retval	TRUE	みんなから受信した
 *	@retval	FALSE	みんなから受信してない
 */
//-----------------------------------------------------------------------------
BOOL BCT_SURVER_ScoreAllUserGetCheck( const BCT_SURVER* cp_wk )
{
	int num;
	num = BCT_SURVER_ScoreSetNumGet( cp_wk );
	if( num >= cp_wk->comm_num ){ 
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	みんなのスコアを取得する
 *
 *	@param	cp_wk		ワーク
 *	@param	p_data		みんなのスコア格納先
 */
//-----------------------------------------------------------------------------
void BCT_SURVER_ScoreGet( BCT_SURVER* p_wk, BCT_SCORE_COMM* p_data )
{
	*p_data = p_wk->score;
}

//----------------------------------------------------------------------------
/**
 *	@brief	サーバカウントダウン設定
 *
 *	@param	p_wk	ワーク
 *	@param	flag	フラグ
 */
//-----------------------------------------------------------------------------
void BCT_SURVER_SetCountDown( BCT_SURVER* p_wk, BOOL flag )
{
	p_wk->countdown = flag;
}




//----------------------------------------------------------------------------
/**
 *	@brief	スコアを設定した数を求める
 *
 *	@param	cp_wk	ワーク
 *
 *	@return	スコアを設定した数
 */
//-----------------------------------------------------------------------------
static u32 BCT_SURVER_ScoreSetNumGet( const BCT_SURVER* cp_wk )
{
	int i;
	int num;
	
	num = 0;
	for( i=0; i<BCT_PLAYER_NUM; i++ ){
		if( (cp_wk->score_msk & (1<<i)) != 0 ){
			num ++;
		}
	}
	return num;
}
