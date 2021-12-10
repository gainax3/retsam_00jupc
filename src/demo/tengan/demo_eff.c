
#include "common.h"

#include "system/lib_pack.h"
#include "system/brightness.h"
#include "system/arc_util.h"
#include "system/palanm.h"
#include "system/laster.h"

#include "demo_eff.h"

#include "system/lib_pack.h"



//-----------------------------------------------------------------------------
//
//	最初のピカピカ用タスク開始関数
//
//-----------------------------------------------------------------------------


static void DEFF_BrightnessVset( TCB_PTR tcb, void* work );

typedef struct{
	int seq;
	u32 flash_num;
	int count;
	int disp;		// 画面指定
	u32 end_bright;
	u32 sub_bright;
	BR_BRIGHTNESS_FADE br_main;
	BR_BRIGHTNESS_FADE br_sub;
	BOOL* end;		// 終了ﾁｪｯｸ用
} BR_FLASH_TASK;
#define FLASH_EFF_SYNC_SUB	( 8 )
#define FLASH_EFF_SYNC_ONE	( 3 )

// 開始エフェクトタスク（最初の光るものなどを制御）
static void BR_FlashTask(TCB_PTR tcb, void *work);

enum{
	FADE_SUB_EFFECT,
	FADE_EFFECT,
	FADE_WAIT,
	FADE_RET_EFFECT,
	FADE_RET_WAIT,
	END_EFFECT,
};

//----------------------------------------------------------------------------
/**
 *
 *@brief	最初のフラッシュの表現をするタスクを実行する
 *					
 *@param	disp	メイン画面にかける:MASK_MAIN_DISPLAY
 *					サブ画面にかける:MASK_SUB_DISPLAY
 *					両画面にかける:MASK_DOUBLE_DISPLAY
 *
 *@param	end		終了したらTRUEを返すフラグ
 *@param	flash_num	フラッシュ回数
 *
 *@return	none
 *
 *
 */
//-----------------------------------------------------------------------------
void DEFF_EncountFlashTask(int disp, u32 bright_color, u32 sub_bright_color, BOOL* end, u32 flash_num )
{
	TCB_PTR tcb;
	BR_FLASH_TASK *eew;

	eew = sys_AllocMemory( HEAPID_WORLD, sizeof(BR_FLASH_TASK) );
	memset( eew, 0, sizeof(BR_FLASH_TASK) );
	TCB_Add( BR_FlashTask, eew, TCB_TSK_PRI );
	
	eew->end = end;		// 終了フラグ設定
	if(eew->end != NULL){
		*(eew->end) = FALSE;
	}
	eew->disp = disp;
	eew->end_bright = bright_color;
	eew->sub_bright = sub_bright_color;
	eew->flash_num	= flash_num;
}

//------------------------------------------------------------------
/**
 * 
 * エフェクト
 *
 * 最初のピカピカ表現
 * 
 * @param   tcb		タスクポインタ
 * @param   work	ワークポインタ
 *
 * @retval  none		
 *
 * ピカピカ　20シンク使用
 */
//------------------------------------------------------------------
static void BR_FlashTask(TCB_PTR tcb, void *work)
{
	BR_FLASH_TASK *eew = work;

	switch(eew->seq){
	case FADE_SUB_EFFECT:
		if(eew->disp == MASK_MAIN_DISPLAY){
			//ブラックアウト
//			ChangeBrightnessRequest( 40,-16,0, eew->plane, MASK_SUB_DISPLAY);
			DEFF_ChangeMstBrightness( &eew->br_sub, 0, eew->sub_bright, MASK_SUB_DISPLAY, FLASH_EFF_SYNC_SUB );
		}else{
			if(eew->disp == MASK_SUB_DISPLAY){
				//ブラックアウト
//				ChangeBrightnessRequest(40,-16,0, eew->plane, MASK_MAIN_DISPLAY);
				DEFF_ChangeMstBrightness( &eew->br_sub, 0, eew->sub_bright, MASK_MAIN_DISPLAY, FLASH_EFF_SYNC_SUB );
			}
		}
		eew->seq++;
		break;

	case FADE_EFFECT:
		//ホワイトアウト
//		ChangeBrightnessRequest(4,16,0,eew->plane,eew->disp);
		DEFF_ChangeMstBrightness( &eew->br_main, 0, eew->end_bright, eew->disp, FLASH_EFF_SYNC_ONE );
		eew->seq++;
		break;
		
	case FADE_WAIT:
		if( DEFF_ChangeMstBrightnessMain( &eew->br_main ) ){
			eew->seq++;
		}
		break;
	case FADE_RET_EFFECT:
		//ホワイトイン
//		ChangeBrightnessRequest(4,0,16,eew->plane,eew->disp);
		DEFF_ChangeMstBrightness( &eew->br_main, eew->end_bright, 0, eew->disp, FLASH_EFF_SYNC_ONE );
		eew->seq++;
		break;
		
	case FADE_RET_WAIT:
		if( DEFF_ChangeMstBrightnessMain( &eew->br_main ) ){
			
			eew->count++;
			if(eew->count==eew->flash_num){
				eew->seq = END_EFFECT;
			}else{
				eew->seq = FADE_EFFECT;
			}
		}
		break;
	case END_EFFECT:		// メインエフェクトに移動
		eew->seq = 0;
		eew->count = 0;

		if(eew->end != NULL){
			*(eew->end) = TRUE;		// タスク終了を報告
		}

		TCB_Delete(tcb);	//タスク終了
		sys_FreeMemoryEz( eew );
		return ;
	}

	// サブ面フェード
	DEFF_ChangeMstBrightnessMain( &eew->br_sub );
}


//----------------------------------------------------------------------------
/**
 *	@brief	ﾏｽﾀｰブライトネス設定
 *
 *	@param	disp	面
 *	@param	no		値
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
void DEFF_SetMstBrightness( int disp, int no )
{
	// ブライトネス解除
	if( disp == MASK_MAIN_DISPLAY ){
		GX_SetMasterBrightness(no);
	}else{
		GXS_SetMasterBrightness(no);
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	ﾏｽﾀｰブライトネス変更　フェードデータ格納
 *
 *	@param	p_data		格納先
 *	@param	start		開始値
 *	@param	end			終了値
 *	@param	disp		面
 *	@param	sync		シンク数
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
void DEFF_ChangeMstBrightness( BR_BRIGHTNESS_FADE* p_data, int start, int end, int disp, int sync )
{
	p_data->disp = disp;
	DEFF_MoveReq( &p_data->brightness, start, end, sync );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ﾏｽﾀｰブライトネス変更　フェードメイン
 *
 *	@param	p_data	ワーク
 *
 *	@retval	TRUE	終了
 *	@retval	FALSE	途中
 */
//-----------------------------------------------------------------------------
BOOL DEFF_ChangeMstBrightnessMain( BR_BRIGHTNESS_FADE* p_data )
{
	BOOL result;
	result = DEFF_MoveMain( &p_data->brightness );
	
	// Vブランク期間中に設定しないときれいにフェードしない
	VWaitTCB_Add( DEFF_BrightnessVset, p_data, TCB_VWAIT_BR_TSK_PRI );
	return result;
}

//----------------------------------------------------------------------------
/**
 *	@brief	Vブランク期間中にﾌﾞﾗｲﾄﾈｽ値を設定するタスク
 *
 *	@param	tcb		TCBワーク
 *	@param	work	ワーク
 *
 *	@return	none
 *	Vブランク期間中に設定しないときれいにフェードしない
 */
//-----------------------------------------------------------------------------
static void DEFF_BrightnessVset( TCB_PTR tcb, void* work )
{
	BR_BRIGHTNESS_FADE* p_data = work;
	DEFF_SetMstBrightness( p_data->disp, p_data->brightness.x );
	TCB_Delete( tcb );
}




//-----------------------------------------------------------------------------
//
//	ビットマップ操作
//
//-----------------------------------------------------------------------------
// ビットマップウィンドウ塗りつぶしオブジェクト
typedef struct _DEFF_BMP_FILL_OBJ{
	DEFF_MOVE_WORK move_x;
	DEFF_MOVE_WORK move_y;
	GF_BGL_BMPWIN* p_bmp;
	u8 width;
	u8 height;
	u8 move_flg;
	u8 col;
} ;

// ビットマップウィンドウブロック上塗りつぶしオブジェクト
typedef struct _DEFF_BMP_FILL_BLOCK{
	DEFF_MOVE_WORK move_x;
	DEFF_MOVE_WORK move_y;
	GF_BGL_BMPWIN* p_bmp;
	u8 width;
	u8 height;
	u8 move_flg;
	u8 col;
} ;

static void DEFF_BMP_Fill( GF_BGL_BMPWIN* p_bmp, s32 top, s32 bottom, s32 left, s32 right, u8 col );

// ビットマップブロック埋め尽くしシステム
#define DEMO_BLOCK_MOVE_WIDTH	( 32 )	// ブロックサイズ
#define DEMO_BLOCK_MOVE_HEIGHT	( 32 )	// ブロックサイズ
#define DEMO_BLOCK_MOVE_MAT_Y	( -16 )	// 開始座標
#define DEMO_BLOCK_MOVE_MOVE_Y	( 192 + DEMO_BLOCK_MOVE_MAT_Y )	// 移動距離
#define DEMO_BLOCK_MOVE_OBJNUM	( 48 )	// xブロックで埋める
#define DEMO_BLOCK_MOVE_X_NUM	( 8 )	// 横の数
#define DEMO_BLOCK_MOVE_Y_NUM	( 6 )	// 縦の数

static const u8 DEMO_BLOCK_MOVE_MoveParam_Random[ DEMO_BLOCK_MOVE_X_NUM ] = {	// 出てくるx座標の順番テーブル
	 0, 2, 5, 7, 1, 6, 3, 4 
};

static const u8 DEMO_BLOCK_MOVE_MoveParam_LinesUP[][ DEMO_BLOCK_MOVE_X_NUM ] = {	// 出てくるx座標の順番テーブル
	{ 0, 1, 2, 3, 4, 5, 6, 7, },
	{ 7, 6, 5, 4, 3, 2, 1, 0, },
};


typedef struct _DEFF_BMP_FILL_BLOCK_MOVE{
	GF_BGL_BMPWIN* p_bmp;
	DEFF_BMP_FILL_BLOCK* p_block[ DEMO_BLOCK_MOVE_OBJNUM ];
	u8	block_count;
	u8	line_x;
	u8	line_y;
	u8	block_line;
	u8	block_end_count;
	u8	move_sync;
	u8	start_sync;
	s8	wait;
	u8	col;
	u8	init_flg;
	u8	dummy[1];
};



//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	p_work	
 * @param	s_x	
 * @param	e_x	
 * @param	count_max	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void DEFF_MoveReq( DEFF_MOVE_WORK* p_work, int s_x, int e_x, int count_max )
{
	p_work->x = s_x;
	p_work->s_x = s_x;
	p_work->dis_x = e_x - s_x;
	p_work->count_max = count_max;
	p_work->count = 0;
}


//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	p_work	
 *
 * @retval	BOOL	
 *
 */
//--------------------------------------------------------------
BOOL DEFF_MoveMain( DEFF_MOVE_WORK* p_work )
{
	int w_x;
	
	w_x = p_work->dis_x * p_work->count;
	w_x = w_x / p_work->count_max;
	p_work->x = w_x + p_work->s_x;
	if( (p_work->count + 1) <= p_work->count_max ){
		p_work->count++;
		return FALSE;
	}
	p_work->count = p_work->count_max;
	return TRUE;
}

//----------------------------------------------------------------------------
/**
 *		【ビットマップブロック上塗りつぶしオブジェクト】
 *	@brief	ワークの確保
 *
 *	@param	heapID	ヒープID
 *
 *	@return	ワーク
 */
//-----------------------------------------------------------------------------
DEFF_BMP_FILL_BLOCK* DEFF_BMP_FillBlockAlloc( u32 heapID )
{
	DEFF_BMP_FILL_BLOCK* p_work;

	p_work = sys_AllocMemory( heapID, sizeof(DEFF_BMP_FILL_BLOCK) );
	memset( p_work, 0, sizeof(DEFF_BMP_FILL_BLOCK) );
	return p_work;
}

//----------------------------------------------------------------------------
/**
 *		【ビットマップブロック上塗りつぶしオブジェクト】
 *	@brief	ワーク破棄
 *
 *	@param	p_work	ワーク
 */
//-----------------------------------------------------------------------------
void DEFF_BMP_FillBlockDelete( DEFF_BMP_FILL_BLOCK* p_work )
{
	sys_FreeMemoryEz( p_work );
}

//----------------------------------------------------------------------------
/**
 *		【ビットマップブロック上塗りつぶしオブジェクト】
 *	@brief	動さ開始
 *
 *	@param	p_work		ワーク
 *	@param	s_x			塗りつぶし開始ｘ座標
 *	@param	e_x			塗りつぶし終了ｘ座標
 *	@param	s_y			塗りつぶし開始ｙ座標
 *	@param	e_y			塗りつぶし終了ｙ座標
 *	@param	sync		シンク数
 *	@param	p_bmp		ビットマップ
 *	@param	width		幅
 *	@param	height		高さ
 *	@param	col			塗りつぶすパレット番号
 */
//-----------------------------------------------------------------------------
void DEFF_BMP_FillBlockStart( DEFF_BMP_FILL_BLOCK* p_work, int s_x, int e_x, int s_y, int e_y, int sync, GF_BGL_BMPWIN* p_bmp, u32 width, u32 height, u8 col )
{
	GF_ASSERT( p_work->move_flg == FALSE );

	// 動さパラメータ
	DEFF_MoveReq( &p_work->move_x, s_x, e_x, sync );
	DEFF_MoveReq( &p_work->move_y, s_y, e_y, sync );

	// その他パラメータ
	p_work->p_bmp	= p_bmp;	// 塗りつぶすビットマップ
	p_work->width	= width;	// 塗りつぶし幅
	p_work->height	= height;	// 塗りつぶし高さ
	p_work->col		= col;		// 塗りつぶしカラー番号

	p_work->move_flg = TRUE;
}

//----------------------------------------------------------------------------
/**
 *		【ビットマップブロック上塗りつぶしオブジェクト】
 *	@brief	動さメイン
 *
 *	@param	p_work	ワーク
 *
 *	@retval	TRUE	終了
 *	@retval	FALSE	途中
 */
//-----------------------------------------------------------------------------
BOOL DEFF_BMP_FillBlockMain( DEFF_BMP_FILL_BLOCK* p_work )
{
	BOOL result;
	s16 top, bottom, left, right;
	if( p_work->move_flg == FALSE ){
		return TRUE;
	}

	// 1つ前を消す
	left = p_work->move_x.x - (p_work->width/2);
	top = p_work->move_y.x - (p_work->height/2);
	right = left + p_work->width;
	bottom = top + p_work->height;
//	DEFF_BMP_Fill( p_work->p_bmp, top, bottom, left, right, 0 );
	
	result = DEFF_MoveMain( &p_work->move_x );
	DEFF_MoveMain( &p_work->move_y );

	left = p_work->move_x.x - (p_work->width/2);
	top = p_work->move_y.x - (p_work->height/2);
	right = left + p_work->width;
	bottom = top + p_work->height;

	DEFF_BMP_Fill( p_work->p_bmp, top, bottom, left, right, p_work->col );

	return result;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ビットマップウィンドウ塗りつぶし　矩形
 *
 *	@param	p_bmp		ワーク
 *	@param	top			上
 *	@param	bottom		下
 *	@param	left		左
 *	@param	right		右
 *	@param	col			カラー番号
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
static void DEFF_BMP_Fill( GF_BGL_BMPWIN* p_bmp, s32 top, s32 bottom, s32 left, s32 right, u8 col )
{
	// 画面外チェック
	if( (right <= 0) || (bottom <= 0) ){
		return ;
	}

	// 一緒なら描画しない
	if( (left == right) || (top == bottom) ){
		return ;
	}
	
	if( left < 0 ){
		left = 0;
	}
	if( right > 256 ){
		right = 256;
	}
	if( top < 0 ){
		top = 0;
	}
	if( bottom > 256 ){
		bottom = 256;
	}

	// ビットマップ塗りつぶし
	GF_BGL_BmpWinFill( p_bmp, col,
			left,
			top,
			right - left,
			bottom - top );
}


//----------------------------------------------------------------------------
/**
 *	@brief	ワーク確保
 *
 *	@param	heapID	ヒープ
 *
 *	@return	ワーク
 */
//-----------------------------------------------------------------------------
DEFF_BMP_FILL_BLOCK_MOVE* DEFF_BMP_FillBlockMoveAlloc( u32 heapID )
{
	DEFF_BMP_FILL_BLOCK_MOVE* p_work;
	int i;

	p_work = sys_AllocMemory( heapID, sizeof(DEFF_BMP_FILL_BLOCK_MOVE) );
	memset( p_work, 0, sizeof(DEFF_BMP_FILL_BLOCK_MOVE) );

	// オブジェクト作成
	for( i=0; i<DEMO_BLOCK_MOVE_OBJNUM; i++ ){
		p_work->p_block[i] = DEFF_BMP_FillBlockAlloc( heapID );
	}
	return p_work;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ワーク破棄
 *
 *	@param	p_work	ワーク
 */
//-----------------------------------------------------------------------------
void DEFF_BMP_FillBlockMoveDelete( DEFF_BMP_FILL_BLOCK_MOVE* p_work )
{
	int i;
	
	// ブロックオブジェ破棄
	for( i=0; i<DEMO_BLOCK_MOVE_OBJNUM; i++ ){
		DEFF_BMP_FillBlockDelete( p_work->p_block[i] );
	}
	sys_FreeMemoryEz( p_work );
}

//----------------------------------------------------------------------------
/**
 *	@brief	動さ開始
 *	
 *	@param	p_work	ワーク
 *	@param	sync	１つのオブジェが動くシンク数
 *	@param	start	１つのオブジェ発射タイミング
 *	@param	p_bmp	ビットマップ
 *	@param	col		カラー
 */
//-----------------------------------------------------------------------------
void DEFF_BMP_FillBlockMoveStart( DEFF_BMP_FILL_BLOCK_MOVE* p_work, u8 sync, u8 start, GF_BGL_BMPWIN* p_bmp, u8 col )
{
	p_work->p_bmp		= p_bmp;
	p_work->col			= col;
	p_work->move_sync	= sync;
	p_work->start_sync	= start;
	p_work->block_count = 0;
	p_work->line_x		= 0;
	p_work->line_y		= 0;
	p_work->block_line  = 0;
	p_work->block_end_count = 0;
	p_work->wait		= 0;
	p_work->init_flg	= 1;
}

//----------------------------------------------------------------------------
/**
 *	@brief	メイン関数
 *
 *	@param	p_work	ワーク
 *
 *	@retval	TRUE	終了
 *	@retval	FALSE	途中
 */
//-----------------------------------------------------------------------------
///< ランダムに敷き詰められていく
BOOL DEFF_BMP_FillBlockMoveMain_Random( DEFF_BMP_FILL_BLOCK_MOVE* p_work )
{
	int x, s_y, e_y;
	int i;
	BOOL result;
	
	if( p_work->init_flg == 0 ){
		return TRUE;
	}
	
	// 発射部
	if( p_work->block_count < DEMO_BLOCK_MOVE_OBJNUM ){
		p_work->wait --;
		if( p_work->wait <= 0 ){
			p_work->wait = p_work->start_sync;

			x = DEMO_BLOCK_MOVE_MoveParam_Random[ p_work->block_count % DEMO_BLOCK_MOVE_X_NUM ];
			x = (DEMO_BLOCK_MOVE_WIDTH/2) + (x * DEMO_BLOCK_MOVE_WIDTH);
			s_y = DEMO_BLOCK_MOVE_MAT_Y - ((p_work->block_count/DEMO_BLOCK_MOVE_X_NUM) * DEMO_BLOCK_MOVE_HEIGHT);
			e_y = DEMO_BLOCK_MOVE_MOVE_Y - ((p_work->block_count/DEMO_BLOCK_MOVE_X_NUM) * DEMO_BLOCK_MOVE_HEIGHT);
			// 発射
			DEFF_BMP_FillBlockStart( 
					p_work->p_block[ p_work->block_count ],
					x, x,
					s_y, e_y,
					p_work->move_sync, p_work->p_bmp,
					DEMO_BLOCK_MOVE_WIDTH, DEMO_BLOCK_MOVE_HEIGHT,
					p_work->col );

			p_work->block_count ++;
		}
	}
	

	// 動さ部
	for( i=p_work->block_end_count; i<p_work->block_count; i++ ){
		result = DEFF_BMP_FillBlockMain( p_work->p_block[i] );
		if( result == TRUE ){
			p_work->block_end_count++;
		}
	}

	// 全動さ終了チェック
	if( (p_work->block_end_count >= DEMO_BLOCK_MOVE_OBJNUM) &&
		(result == TRUE) ){
		p_work->init_flg = FALSE;
		return TRUE;
	}

	return FALSE;
}

///< 互い違いにラインが上に上がる
BOOL DEFF_BMP_FillBlockMoveMain_LinesUP( DEFF_BMP_FILL_BLOCK_MOVE* p_work )
{
	int x, s_y, e_y;
	int i;
	BOOL result;
	
	if( p_work->init_flg == 0 ){
		return TRUE;
	}
	
	// 発射部
	if( p_work->block_count < DEMO_BLOCK_MOVE_OBJNUM ){
		p_work->wait --;
		if( p_work->wait <= 0 ){
			p_work->wait = p_work->start_sync;

			x = DEMO_BLOCK_MOVE_MoveParam_LinesUP[ p_work->block_line ][ p_work->block_count % DEMO_BLOCK_MOVE_X_NUM ];
			x = (DEMO_BLOCK_MOVE_WIDTH/2) + (x * DEMO_BLOCK_MOVE_WIDTH);
			s_y = DEMO_BLOCK_MOVE_MAT_Y - ((p_work->block_count/DEMO_BLOCK_MOVE_X_NUM) * DEMO_BLOCK_MOVE_HEIGHT);
			e_y = DEMO_BLOCK_MOVE_MOVE_Y - ((p_work->block_count/DEMO_BLOCK_MOVE_X_NUM) * DEMO_BLOCK_MOVE_HEIGHT);
			// 発射
			DEFF_BMP_FillBlockStart( 
					p_work->p_block[ p_work->block_count ],
					x, x,
					s_y, e_y,
					p_work->move_sync, p_work->p_bmp,
					DEMO_BLOCK_MOVE_WIDTH, DEMO_BLOCK_MOVE_HEIGHT,
					p_work->col );
			
			p_work->block_count ++;
			
			if ( ( p_work->block_count % DEMO_BLOCK_MOVE_X_NUM ) == 0 ){
				p_work->block_line ^= 1;
			}
		}
	}
	

	// 動さ部
	for( i=p_work->block_end_count; i<p_work->block_count; i++ ){
		result = DEFF_BMP_FillBlockMain( p_work->p_block[i] );
		if( result == TRUE ){
			p_work->block_end_count++;
		}
	}

	// 全動さ終了チェック
	if( (p_work->block_end_count >= DEMO_BLOCK_MOVE_OBJNUM) &&
		(result == TRUE) ){
		p_work->init_flg = FALSE;
		return TRUE;
	}

	return FALSE;
}


///< ブロック落下
static const u8 PlaceTable[][ 6 ] = {
	{ 0, 1, 2, 3, 4, 5 },
	{ 5, 4, 3, 2, 1, 0 },
};
BOOL DEFF_BMP_FillBlockMoveMain_Place( DEFF_BMP_FILL_BLOCK_MOVE* p_work )
{
	int x, s_y, e_y;
	int i;
	BOOL result;
	
	if( p_work->init_flg == 0 ){
		return TRUE;
	}
	
	// 発射部
	if( p_work->block_count < DEMO_BLOCK_MOVE_OBJNUM ){
		p_work->wait --;
		if( p_work->wait <= 0 ){
			p_work->wait = p_work->start_sync;

			x	= p_work->line_x;
			x	= ( DEMO_BLOCK_MOVE_WIDTH / 2 ) + ( x * DEMO_BLOCK_MOVE_WIDTH );
			s_y = DEMO_BLOCK_MOVE_HEIGHT * ( PlaceTable[ p_work->block_line ][ p_work->line_y ] * 1);//DEMO_BLOCK_MOVE_HEIGHT );
			e_y = s_y + DEMO_BLOCK_MOVE_HEIGHT;
			
//			OS_Printf( "2:x = %3d  sy = %3d  ey = %3d\n", x, s_y, e_y );

			DEFF_BMP_FillBlockStart( 
					p_work->p_block[ p_work->block_count ],
					x, x,
					s_y, e_y,
					p_work->move_sync, p_work->p_bmp,
					DEMO_BLOCK_MOVE_WIDTH, DEMO_BLOCK_MOVE_HEIGHT,
					p_work->col );
			
			x	= 7 - p_work->line_x;
			x	= ( DEMO_BLOCK_MOVE_WIDTH / 2 ) + ( x * DEMO_BLOCK_MOVE_WIDTH );
			s_y = DEMO_BLOCK_MOVE_HEIGHT * ( PlaceTable[ p_work->block_line ^ 1 ][ p_work->line_y ] * 1);//DEMO_BLOCK_MOVE_HEIGHT );
			e_y = s_y + DEMO_BLOCK_MOVE_HEIGHT;
			
//			OS_Printf( "2:x = %3d  sy = %3d  ey = %3d\n", x, s_y, e_y );

			DEFF_BMP_FillBlockStart( 
					p_work->p_block[ p_work->block_count+1 ],
					x, x,
					s_y, e_y,
					p_work->move_sync, p_work->p_bmp,
					DEMO_BLOCK_MOVE_WIDTH, DEMO_BLOCK_MOVE_HEIGHT,
					p_work->col );
					
			p_work->block_count++;
			p_work->block_count++;
			p_work->line_y++;
			
			if ( ( p_work->line_y % 6 ) == 0 ){
				p_work->block_line ^= 1;
				p_work->line_x++;
				p_work->line_y = 0;
			}
		}
	}
	

	// 動さ部
	for( i=p_work->block_end_count; i<p_work->block_count; i++ ){
		result = DEFF_BMP_FillBlockMain( p_work->p_block[i] );
		if( result == TRUE ){
			p_work->block_end_count++;
		}
	}

	// 全動さ終了チェック
	if( (p_work->block_end_count >= DEMO_BLOCK_MOVE_OBJNUM) &&
		(result == TRUE) ){
		p_work->init_flg = FALSE;
		return TRUE;
	}

	return FALSE;
}








static TCB_PTR EncountEffect_LasterScroll_VDmaTaskReq( DEMO_LASTER_SCROLL* p_lstscr );
static void EncountEffect_LasterScroll_VDmaTCB( TCB_PTR tcb, void* p_work );
//static void DEFF_Laster_Init( DEMO_LASTER_SCROLL* p_laster, u32 heapID );
//static void DEFF_Laster_Delete( DEMO_LASTER_SCROLL* p_laster );
//static void DEFF_Laster_Start( DEMO_LASTER_SCROLL* p_laster, u8 start, u8 end, u16 add_r, fx32 r_w, s16 scr_sp, u32 bg_no, u32 init_num, u32 tsk_pri );
//static void DEFF_Laster_BuffYSet( DEMO_LASTER_SCROLL* p_laster, s16 start_y );
//static void DEFF_Laster_BuffXFlip( DEMO_LASTER_SCROLL* p_laster, u32 dot );

//----------------------------------------------------------------------------
/**
 *	@brief	ラスタースクロールシステム　VブランクDMA転送リクエストタスク
 *
 *	@param	p_lstscr	ラスタースクロールシステム
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
static TCB_PTR EncountEffect_LasterScroll_VDmaTaskReq( DEMO_LASTER_SCROLL* p_lstscr )
{
	return VIntrTCB_Add( EncountEffect_LasterScroll_VDmaTCB,
			p_lstscr, DEMO_EFF_LASTER_SCRLL_VINTR_DMA_SET_TASK_PRI );
}

//----------------------------------------------------------------------------
/**
 *	@brief	DMA転送タスク
 *
 *	@param	tcb		タスクワーク
 *	@param	p_work	ワーク
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static void EncountEffect_LasterScroll_VDmaTCB( TCB_PTR tcb, void* p_work )
{
	DEMO_LASTER_SCROLL* p_lstscr = p_work;

	if( p_lstscr->dmacount >= 2 ){
		LASTER_ScrollVBuffSwitch( p_lstscr->p_laster );
		p_lstscr->dmacount = 0;
	}

	LASTER_ScrollVDma( p_lstscr->p_laster );
	p_lstscr->dmacount ++;
}


//----------------------------------------------------------------------------
/**
 *	@brief	ラスター初期化
 *
 *	@param	p_laster	ラスターワーク
 *	@param	heapID		ヒープID
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
void DEFF_Laster_Init( DEMO_LASTER_SCROLL* p_laster, u32 heapID )
{
	p_laster->p_laster = LASTER_ScrollInit( heapID );
	p_laster->dmacount = 0;
	p_laster->VDma = EncountEffect_LasterScroll_VDmaTaskReq( p_laster );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ラスター破棄
 *
 *	@param	p_laster	ラスターワーク
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
void DEFF_Laster_Delete( DEMO_LASTER_SCROLL* p_laster )
{
	TCB_Delete( p_laster->VDma );
	LASTER_ScrollEnd( p_laster->p_laster );
	LASTER_ScrollDelete( p_laster->p_laster );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ラスター開始
 *
 *	@param	p_laster	ワーク
 *	@param	start		ラスタースクロールさせる開始ｙ座標
 *	@param	end			ラスタースクロールさせる終了ｙ座標
 *	@param	add_r		sinカーブテーブル作成時の角度を足していく値(0～65535)
 *	@param	r_w			sinカーブの半径
 *	@param	scr_sp		ラスタースクロールスピード	整数*100した値	マイナス有効
 *	@param	bg_no		ラスターをかけるBGの番号
 *	@param	init_num	ワークを初期化する値
 *	@param	tsk_pri		タスク優先順位
 *
 *	@return	none
 *
 * bg_no
 * 		LASTER_SCROLL_MBG0,
 *		LASTER_SCROLL_MBG1,
 *		LASTER_SCROLL_MBG2,
 *		LASTER_SCROLL_MBG3,
 *		LASTER_SCROLL_SBG0,
 *		LASTER_SCROLL_SBG1,
 *		LASTER_SCROLL_SBG2,
 *		LASTER_SCROLL_SBG3
 */
//-----------------------------------------------------------------------------
void DEFF_Laster_Start( DEMO_LASTER_SCROLL* p_laster, u8 start, u8 end, u16 add_r, fx32 r_w, s16 scr_sp, u32 bg_no, u32 init_num, u32 tsk_pri, int mode )
{
	if ( mode == 0 ){
		LASTER_ScrollStart( p_laster->p_laster, start, end, add_r, r_w, scr_sp,	bg_no, init_num, tsk_pri );
	}
	else {
		LASTER_YScrollStart( p_laster->p_laster, start, end, add_r, r_w, scr_sp,	bg_no, init_num, tsk_pri );
	}
}


//----------------------------------------------------------------------------
/**
 *	@brief	ラスター開始Y位置設定
 *
 *	@param	p_laster
 *	@param	start_y 
 *
 *	@return
 */
//-----------------------------------------------------------------------------
void DEFF_Laster_BuffYSet( DEMO_LASTER_SCROLL* p_laster, s16 start_y )
{
	u32* p_buff;
	u32* p_write;
	int i;
	u32 num;

	p_write = sys_AllocMemory( HEAPID_WORLD, sizeof(u32) * LASTER_SCROLL_BUFF_SIZE );
	memset( p_write, 9, sizeof(u32) * LASTER_SCROLL_BUFF_SIZE );

	p_buff = LASTER_GetScrollWriteBuff( p_laster->p_laster );
	
	for( i=0; i<192; i++ ){
		if( ((-start_y + i) >= 0) &&
			((-start_y + i) < 192 ) ){
			num = p_buff[ (-start_y + i) ];
		}else{
			num = 0;
		}
		p_write[i] = num;
	}

	memcpy( p_buff, p_write, sizeof(u32) * LASTER_SCROLL_BUFF_SIZE );
	
	sys_FreeMemoryEz( p_write );	
}

//----------------------------------------------------------------------------
/**
 *	@brief	ラスターの横座標をdotごとに反転させる
 *
 *	@param	p_laster	ラスターワーク
 *	@param	dot			ドット数
 */
//-----------------------------------------------------------------------------
void DEFF_Laster_BuffXFlip( DEMO_LASTER_SCROLL* p_laster, u32 dot )
{
	u32* p_buff;
	int i;
	s16 x;
	
	p_buff = LASTER_GetScrollWriteBuff( p_laster->p_laster );

	for( i=0; i<192; i++ ){
		x = p_buff[i] & 0xffff;
		if( ((i / dot) % 2) == 0 ){
			p_buff[i] = (x & 0xffff);
		}else{
			p_buff[i] = (-x & 0xffff);
		}
	}
}










//-----------------------------------------------------------------------------
/**
 *		ラスター用システム		LASTER
 */
//-----------------------------------------------------------------------------
//-------------------------------------
//	
//	HDMAコントロール
//	
//=====================================
typedef void (*pFEFTool_LSVBCFunc)( void* work );
// 1/30 をカバーするシステム
typedef struct _FEFTOOL_LASTER_VBLANK_CONT{
	TCB_PTR		vdma_tcb;
	TCB_PTR		vbuff_tcb;
	BOOL vblank_lst_flg;// VBlankで行う処理を行うかのフラグ
	BOOL dma_set_flg;	//メインループ中のVブランかをチェックするフラグ
	
	pFEFTool_LSVBCFunc	vblank1;	// ３０フレームの通常のVブランク用処理
	pFEFTool_LSVBCFunc	vblank2;	// 途中のVブランク用処理
	void * work;
} FEFTOOL_LSVBC;

//----------------------------------------------------------------------------
/**
 *
 *	@brief	通常Vブランク
 *
 *	@param	tcb		TCBワーク
 *	@param	work	ワーク
 *	
 *	@return	none
 *
 *
 */
//-----------------------------------------------------------------------------
static void FEFTool_LSVBC_VBlank1( TCB_PTR tcb, void* work )
{
	FEFTOOL_LSVBC* wk = work;
	
	if( wk->vblank_lst_flg ){
		// 次のVブランク期間中にDMA設定を行う
		// ２回目はここで行うので１回目だけメインループ
		// ないでくるVブランク期間中にやってもらう
		wk->dma_set_flg = TRUE;

		// 通常のVブランク処理
		wk->vblank1( wk->work );

	}else{
		LASTER_DmaStop();
	}
}
//----------------------------------------------------------------------------
/**
 *
 *	@brief	途中に入るVブランク期間用関数
 *
 *	@param	tcb
 *	@param	work 
 *
 *	@return
 *
 *
 */
//-----------------------------------------------------------------------------
static void FEFTool_LSVBC_VBlank2( TCB_PTR tcb, void* work )
{
	FEFTOOL_LSVBC* wk = work;
	
	if( wk->vblank_lst_flg && wk->dma_set_flg ){

		// 途中のVブランク処理
		wk->vblank2( wk->work );
		
		wk->dma_set_flg = FALSE;
	}
}

//----------------------------------------------------------------------------
/**
 *
 *	@brief	ラスターVブランクシステム
 *
 *	@param	p_lsvbc		ラスターVブランクシステムワーク
 *	@param	work		引数に渡すワーク
 *	@param	pfunc1		通常のVブランク
 *	@param	pfunc2		途中のVブランク
 *
 *	@return
 *
 *
 */
//-----------------------------------------------------------------------------
static void FEFTool_LSVBC_Init( FEFTOOL_LSVBC* p_lsvbc, void* work, pFEFTool_LSVBCFunc pfunc1, pFEFTool_LSVBCFunc pfunc2 )
{
	GF_ASSERT( p_lsvbc );

	p_lsvbc->vblank_lst_flg	= TRUE;
	p_lsvbc->dma_set_flg	= FALSE;
	p_lsvbc->work			= work;
	p_lsvbc->vblank1		= pfunc1;
	p_lsvbc->vblank2		= pfunc2;

	// タスク登録
	p_lsvbc->vbuff_tcb = VWaitTCB_Add( FEFTool_LSVBC_VBlank1, p_lsvbc, 0 );
	p_lsvbc->vdma_tcb = VIntrTCB_Add( FEFTool_LSVBC_VBlank2, p_lsvbc, 0 );
}

//----------------------------------------------------------------------------
/**
 *
 *	@brief	ラスター用Vブランク管理システム破棄
 *
 *	@param	p_lsvbc		破棄するラスターVブランク管理システム
 *
 *	@return	none
 *
 *
 */
//-----------------------------------------------------------------------------
static void FEFTool_LSVBC_Delete( FEFTOOL_LSVBC* p_lsvbc )
{
	GF_ASSERT( p_lsvbc );

	// VBlank関数破棄
	if( p_lsvbc->vbuff_tcb != NULL ){
		TCB_Delete( p_lsvbc->vbuff_tcb );
	}
	if( p_lsvbc->vdma_tcb != NULL ){
		TCB_Delete( p_lsvbc->vdma_tcb );
	}
	LASTER_DmaStop();
}

//----------------------------------------------------------------------------
/**
 *
 *	@brief	Vブランクシステムの１じ停止
 *
 *	@param	p_lsvbc		ワーク
 *
 *	@return	none
 *
 *
 */
//-----------------------------------------------------------------------------
static void FEFTool_LSVBC_Stop( FEFTOOL_LSVBC* p_lsvbc )
{
	GF_ASSERT( p_lsvbc );
	p_lsvbc->vblank_lst_flg = FALSE;
}

//----------------------------------------------------------------------------
/**
 *
 *	@brief	Vブランクシステムの再開
 *
 *	@param	p_lsvbc		ワーク
 *
 *	@return	none
 *
 *
 */
//-----------------------------------------------------------------------------
static void FEFTool_LSVBC_Start( FEFTOOL_LSVBC* p_lsvbc )
{
	GF_ASSERT( p_lsvbc );
	p_lsvbc->vblank_lst_flg = TRUE;
}


//-------------------------------------
//	
//	バッファ内を何も操作しない
//	
//=====================================
typedef struct _FEFTOOL_DEFLASTER{
	FEFTOOL_LSVBC vblank;

	// ラスターデータ
	LASTER_SYS_PTR lst;	// ラスターシステム
	u32	buff1[ FEFTOOL_DEFLASTER_BUFF_SIZE ];	// ダブルバッファ
	u32	buff2[ FEFTOOL_DEFLASTER_BUFF_SIZE ];	// ダブルバッファ
	u32	addr;	// DMA転送先
	
} FEFTOOL_DEFLASTER;

//----------------------------------------------------------------------------
/**
 *
 *	@brief	DMA転送設定
 *
 *	@param	p_lst	デフォルトラスターシステム
 *
 *	@return	none
 *
 *
 */
//-----------------------------------------------------------------------------
static void FEFTool_DefLasterDmaTransSet( FEFTOOL_DEFLASTER* wk )
{
	const void* buff;

	// 参照用バッファ取得
	buff = LASTER_GetReadBuff( wk->lst );

	LASTER_DmaStop();
	
	// HBlankDmaTransに設定
	LASTER_DmaSet( buff, (void*)wk->addr, 4, LASTER_DMA_TYPE32 );
}

//----------------------------------------------------------------------------
/**
 *
 *	@brief	ラスター用Vブランク管理システムに渡す通常Vブランク関数
 *
 *	@param	work	ワーク
 *
 *	@return	none
 *
 *
 */
//-----------------------------------------------------------------------------
static void FEFTool_DefLasterNormVblank( void* work )
{
	FEFTOOL_DEFLASTER* wk = work;

	// バッファスイッチ
	LASTER_VBlank( wk->lst );

	// HDMA登録
	FEFTool_DefLasterDmaTransSet( wk );
}

//----------------------------------------------------------------------------
/**
 *
 *	@brief ラスター用Vブランク管理システムに渡す途中に呼ばれるVブランク用処理
 *
 *	@param		work	ワーク
 *	
 *	@return		none
 *
 *
 */
//-----------------------------------------------------------------------------
static void FEFTool_DefLasterCenterVblank( void* work )
{
	FEFTOOL_DEFLASTER* wk = work;

	// HDMA登録
	FEFTool_DefLasterDmaTransSet( wk );
}

//----------------------------------------------------------------------------
/**
 *
 *	@brief	デフォルトラスター処理初期化
 *
 *	@param	addr		転送先レジスタアドレス
 *	@param	init_num	バッファの初期値
 *	@param	heap		ヒープID
 *
 *	@return	作成したラスター処理データ
 *
 *
 */
//-----------------------------------------------------------------------------
FEFTOOL_DEFLASTER_PTR FEFTool_InitDefLaster( u32 addr, u32 init_num, int heap )
{
	FEFTOOL_DEFLASTER_PTR p_lst;
	
	p_lst = sys_AllocMemory( heap, sizeof(FEFTOOL_DEFLASTER) );
	memset( p_lst, 0, sizeof(FEFTOOL_DEFLASTER) );
	GF_ASSERT( p_lst );


	p_lst->lst			= LASTER_Init( heap, p_lst->buff1, p_lst->buff2 );
	GF_ASSERT( p_lst->lst );
	p_lst->addr			= addr;

	// バッファの初期化
	MI_CpuFill32( p_lst->buff1, init_num, sizeof(u32) * FEFTOOL_DEFLASTER_BUFF_SIZE );
	MI_CpuFill32( p_lst->buff2, init_num, sizeof(u32) * FEFTOOL_DEFLASTER_BUFF_SIZE );

	// Vブランク管理システムを初期化
	FEFTool_LSVBC_Init( &p_lst->vblank, p_lst, FEFTool_DefLasterNormVblank, FEFTool_DefLasterCenterVblank );

	return p_lst;
}

//----------------------------------------------------------------------------
/**
 *
 *	@brief	ラスター処理の終了
 *
 *	@param	p_lst	デフォルトラスターシステムワーク
 *
 *	@return	none
 *
 *
 */
//-----------------------------------------------------------------------------
void FEFTool_DeleteDefLaster( FEFTOOL_DEFLASTER_PTR p_lst )
{
	GF_ASSERT( p_lst );

	// Vblank管理システム破棄
	FEFTool_LSVBC_Delete( &p_lst->vblank );
	
	// ラスターシステム破棄
	if( p_lst->lst != NULL ){
		LASTER_Delete( p_lst->lst );
	}

	sys_FreeMemoryEz( p_lst );
}

//----------------------------------------------------------------------------
/**
 *
 *	@brief	書き込み用バッファの取得
 *
 *	@param	cp_lst	デフォルトラスターシステム
 *
 *	@return	書き込み用バッファ
 *
 *
 */
//-----------------------------------------------------------------------------
void* FEFTool_GetDefLasterWriteBuff( CONST_FEFTOOL_DEFLASTER_PTR cp_lst )
{
	GF_ASSERT( cp_lst );

	return LASTER_GetWriteBuff( cp_lst->lst );
}

//----------------------------------------------------------------------------
/**
 *
 *	@brief	参照用バッファを取得
 *
 *	@param	cp_lst	デフォルトラスターシステム
 *
 *	@return	参照用バッファ
 *
 *
 */
//-----------------------------------------------------------------------------
const void* FEFTool_GetDefLasterReadBuff( CONST_FEFTOOL_DEFLASTER_PTR cp_lst )
{
	GF_ASSERT( cp_lst );

	return LASTER_GetReadBuff( cp_lst->lst );
}

//----------------------------------------------------------------------------
/**
 *
 *	@brief	ラスター処理の停止
 *
 *	@param	p_lst	デフォルトラスターワーク
 *
 *	@return	none
 *
 * Vブランク関数が機能しなくなるので、
 * 画面にラスターが反映されなくなります。
 *
 */
//-----------------------------------------------------------------------------
void FEFTool_StopDefLaster( FEFTOOL_DEFLASTER_PTR p_lst )
{
	GF_ASSERT( p_lst );

	FEFTool_LSVBC_Stop( &p_lst->vblank );
}

//----------------------------------------------------------------------------
/**
 *
 *	@brief	ラスター処理スタート
 *
 *	@param	p_lst	デフォルトラスターシステム
 *
 *	@return	none
 *
 * 画面にラスターが反映されるようになります。
 *
 */
//-----------------------------------------------------------------------------
void FEFTool_StartDefLaster( FEFTOOL_DEFLASTER* p_lst )
{
	GF_ASSERT( p_lst );

	FEFTool_LSVBC_Start( &p_lst->vblank );
}


//----------------------------------------------------------------------------
/**
 *
 *	@brief	データ転送先アドレスを再設定
 *
 *	@param	p_lst	デフォルトラスターシステム
 *	@param	addr	転送先アドレス
 *
 *	@return	none
 *
 *
 */
//-----------------------------------------------------------------------------
void FEFTool_SetDefLasterTransAddr( FEFTOOL_DEFLASTER_PTR p_lst, u32 addr )
{
	GF_ASSERT( p_lst );

	p_lst->addr = addr;
}


//--------------------------------------------------------------
/**
 * @brief	データ転送先アドレスを取得
 *
 * @param	p_lst	デフォルトラスターシステム
 *
 * @retval	u32		転送先アドレス
 *
 */
//--------------------------------------------------------------
u32 FEFTool_GetDefLasterTransAddr( FEFTOOL_DEFLASTER_PTR p_lst )
{
	GF_ASSERT( p_lst );

	return p_lst->addr;
}

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
//	
//	モーションブラーオブジェクト
//	
//=====================================
typedef struct _DEMO_MOTION_BL_TASK{
	GXVRamLCDC		lcdc;		// 元のLCDC
	DEMO_MOTION_BL_DATA	data;
	BOOL			init_flg;
	TCB_PTR			tcb;	
} DEMO_MOTION_BL_TASK;


//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
static void VBlankLCDCChange(TCB_PTR tcb, void* work);	// LCDCの状態を設定するタスク
static void DEMO_MOTION_BL_Task(TCB_PTR tcb, void* work);		// モーションブラータスク
static void DEMO_MOTION_BL_Capture( DEMO_MOTION_BL_DATA* p_data );



//----------------------------------------------------------------------------
/**
 *
 *@brief	モーションブラーを使える状態に初期化
 *
 *@param	init		初期化データ
 *
 *@return	DEMO_MOTION_BL_PTR	モーションブラーオブジェクト
 *
 *
 */
//-----------------------------------------------------------------------------
DEMO_MOTION_BL_PTR DEMO_MOTION_BL_Init(DEMO_MOTION_BL_DATA* init)
{
	TCB_PTR task;
	DEMO_MOTION_BL_PTR mb;
	
	// モーションブラータスクをセット
	task = PMDS_taskAdd(DEMO_MOTION_BL_Task, sizeof(DEMO_MOTION_BL_TASK), 5, init->heap_id);
	mb = TCB_GetWork(task);

	mb->data = *init;
	mb->tcb = task;
	mb->init_flg = FALSE;

	mb->lcdc = GX_GetBankForLCDC();

	DEMO_MOTION_BL_Capture( &mb->data );

	// LCDCチェンジ
	VWaitTCB_Add( VBlankLCDCChange, mb, 0 );
//	addVBlankData(mb, VBlankLCDCChange);

	return mb;
}



//----------------------------------------------------------------------------
/**
 *
 *@brief	モーションブラーの解除
 *
 *@param	data		モーションブラーオブジェ
 *@param	dispMode	解除後のディスプレイモード
 *@param	bgMode		解除後のBGモード
 *@param	bg0_2d3d	解除後のBG０を３Dに使用するか
 *
 *@return	none
 *
 *
 */
//-----------------------------------------------------------------------------
void DEMO_MOTION_BL_Delete(DEMO_MOTION_BL_PTR* data, GXDispMode dispMode, GXBGMode bgMode, GXBG0As bg0_2d3d)
{
	

	// グラフィックモードを戻す
	GX_SetGraphicsMode(dispMode, bgMode,bg0_2d3d);

	GX_SetBankForLCDC((*data)->lcdc);
	
	switch((*data)->data.dispMode){
	case GX_DISPMODE_VRAM_A:
		MI_CpuClearFast( (void*)HW_LCDC_VRAM_A, HW_VRAM_A_SIZE );
		break;
	case GX_DISPMODE_VRAM_B:
		MI_CpuClearFast( (void*)HW_LCDC_VRAM_B, HW_VRAM_B_SIZE );
		break;
	case GX_DISPMODE_VRAM_C:
		MI_CpuClearFast( (void*)HW_LCDC_VRAM_C, HW_VRAM_C_SIZE );
		break;
	case GX_DISPMODE_VRAM_D:
		MI_CpuClearFast( (void*)HW_LCDC_VRAM_D, HW_VRAM_D_SIZE );
		break;
	default:
		GX_SetBankForLCDC(GX_VRAM_LCDC_NONE);
		break;
	}
		
	
	// タスク破棄
	PMDS_taskDel((*data)->tcb);
	*data = NULL;
}

//----------------------------------------------------------------------------
/**
 *
 *@brief	途中でパラメータを変更
 *
 *@param	data	モーションブラーオブジェ
 *@param	eva		ブレンド係数A
 *@param	evb		ブレンド係数B
 *
 *@return	none
 *
 *
 */
//-----------------------------------------------------------------------------
void DEMO_MOTION_BL_ParamChg(DEMO_MOTION_BL_PTR data, int eva, int evb)
{
	data->data.eva = eva;
	data->data.evb = evb;
}


//----------------------------------------------------------------------------
/**
 *
 *@brief	モーションブラー　キャプチャタスク
 *
 *@param	tcb		タスクポインタ
 *@param	work	モーションブラーデータ
 *
 *@return	none
 *
 *
 */
//-----------------------------------------------------------------------------
static void DEMO_MOTION_BL_Task(TCB_PTR tcb, void* work)
{
	DEMO_MOTION_BL_PTR mb = work;
	
	//初期化完了待ち
	if( mb->init_flg ){
		GX_SetCapture(
				mb->data.sz,			// キャプチャサイズ
				mb->data.mode,			// キャプチャモード
				mb->data.a,				// キャプチャブレンドA
				mb->data.b,				// キャプチャブレンドB
				mb->data.dest,			// 転送Vram
				mb->data.eva,			// ブレンド係数A
				mb->data.evb);			// ブレンド係数B
				
//				OS_Printf( " %d, %d\n", mb->data.eva, mb->data.evb );
	}
}

//----------------------------------------------------------------------------
/**
 *
 *@brief	LCDCの状態を設定するタスク
 *
 *@param	work	設定する値が入っている
 *
 *@return	none
 *
 *
 */
//-----------------------------------------------------------------------------
static void VBlankLCDCChange(TCB_PTR tcb, void* work)
{
	DEMO_MOTION_BL_PTR mb = (DEMO_MOTION_BL_PTR)work;

	// 描画Vram設定
	switch(mb->data.dispMode){
	case GX_DISPMODE_VRAM_A:
		GX_SetBankForLCDC(GX_VRAM_LCDC_A);
		break;
	case GX_DISPMODE_VRAM_B:
		GX_SetBankForLCDC(GX_VRAM_LCDC_B);
		break;
	case GX_DISPMODE_VRAM_C:
		GX_SetBankForLCDC(GX_VRAM_LCDC_C);
		break;
	case GX_DISPMODE_VRAM_D:
		GX_SetBankForLCDC(GX_VRAM_LCDC_D);
		break;
	default:
		GX_SetBankForLCDC(GX_VRAM_LCDC_NONE);
		break;
	}

	// グラフィックモード設定
	GX_SetGraphicsMode(mb->data.dispMode, mb->data.bgMode,mb->data.bg0_2d3d);	

	// 初期化完了
	mb->init_flg = TRUE;

	TCB_Delete( tcb );
}

//----------------------------------------------------------------------------
/**
 *	@brief	Capture関数
 *
 *	@param	p_data 
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static void DEMO_MOTION_BL_Capture( DEMO_MOTION_BL_DATA* p_data )
{
	// 描画Vram初期化
	switch(p_data->dispMode){
	case GX_DISPMODE_VRAM_A:
		MI_CpuClearFast( (void*)HW_LCDC_VRAM_A, HW_VRAM_A_SIZE );
		break;
	case GX_DISPMODE_VRAM_B:
		MI_CpuClearFast( (void*)HW_LCDC_VRAM_B, HW_VRAM_B_SIZE );
		break;
	case GX_DISPMODE_VRAM_C:
		MI_CpuClearFast( (void*)HW_LCDC_VRAM_C, HW_VRAM_C_SIZE );
		break;
	case GX_DISPMODE_VRAM_D:
		MI_CpuClearFast( (void*)HW_LCDC_VRAM_D, HW_VRAM_D_SIZE );
		break;
	default:
		break;
	}

	GX_SetCapture(
			p_data->sz,			// キャプチャサイズ
			p_data->mode,			// キャプチャモード
			p_data->a,				// キャプチャブレンドA
			p_data->b,				// キャプチャブレンドB
			p_data->dest,			// 転送Vram
			16,						// ブレンド係数A
			0);						// ブレンド係数B	
}
