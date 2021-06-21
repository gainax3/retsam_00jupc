#ifndef __DEMO_EFF_H__
#define __DEMO_EFF_H__


#define DEMO_TR_DAN_HIGH_BALL_MOVE_SYNC		( 5 )	// 移動シンク
#define DEMO_TR_DAN_HIGH_BLOCK_SYNC			( 1 )	// ワイプシンク
#define DEMO_TR_DAN_HIGH_BLOCK_START_SYNC	( 1 )	// ワイプシンク


#define TCB_TSK_PRI	(5)
#define TCB_VWAIT_BR_TSK_PRI	(10)

//-------------------------------------
//	汎用動作
//=====================================
typedef struct {
	int x;
	int s_x;
	int dis_x;
	int count;
	int count_max;
} DEFF_MOVE_WORK;

extern void DEFF_MoveReq( DEFF_MOVE_WORK* p_work, int s_x, int e_x, int count_max );
extern BOOL	DEFF_MoveMain( DEFF_MOVE_WORK* p_work );

// フラッシュ
extern void DEFF_EncountFlashTask(int disp, u32 bright_color, u32 sub_bright_color, BOOL* end, u32 flash_num );

//-------------------------------------
//	
//	ﾏｽﾀｰブライトネス関連
//	
//=====================================
typedef struct {
	DEFF_MOVE_WORK brightness;
	int disp;
} BR_BRIGHTNESS_FADE;

#define DEMO_TR_DAN_HIGH_BLOCK_FILLCOLOR	( 15 )	// 塗りつぶ

extern void DEFF_SetMstBrightness( int disp, int no );
extern void DEFF_ChangeMstBrightness( BR_BRIGHTNESS_FADE* p_data, int start, int end, int disp, int sync );
extern BOOL DEFF_ChangeMstBrightnessMain( BR_BRIGHTNESS_FADE* p_data );
extern void DEFF_V_SetMstBrightness( int disp, int no );


// ビットマップブロック状塗りつぶしオブジェクト
typedef struct _DEFF_BMP_FILL_BLOCK DEFF_BMP_FILL_BLOCK;

extern DEFF_BMP_FILL_BLOCK* DEFF_BMP_FillBlockAlloc( u32 heapID );
extern void DEFF_BMP_FillBlockDelete( DEFF_BMP_FILL_BLOCK* p_work );
extern void DEFF_BMP_FillBlockStart( DEFF_BMP_FILL_BLOCK* p_work, int s_x, int e_x, int s_y, int e_y, int sync, GF_BGL_BMPWIN* p_bmp, u32 width, u32 height, u8 col );
extern BOOL DEFF_BMP_FillBlockMain( DEFF_BMP_FILL_BLOCK* p_work );

// ビットマップブロック埋め尽くしシステム
typedef struct _DEFF_BMP_FILL_BLOCK_MOVE DEFF_BMP_FILL_BLOCK_MOVE;

extern DEFF_BMP_FILL_BLOCK_MOVE* DEFF_BMP_FillBlockMoveAlloc( u32 heapID );
extern void DEFF_BMP_FillBlockMoveDelete( DEFF_BMP_FILL_BLOCK_MOVE* p_work );
extern void DEFF_BMP_FillBlockMoveStart( DEFF_BMP_FILL_BLOCK_MOVE* p_work, u8 sync, u8 start, GF_BGL_BMPWIN* p_bmp, u8 col );

extern BOOL DEFF_BMP_FillBlockMoveMain_Random( DEFF_BMP_FILL_BLOCK_MOVE* p_work );
extern BOOL DEFF_BMP_FillBlockMoveMain_LinesUP( DEFF_BMP_FILL_BLOCK_MOVE* p_work );
extern BOOL DEFF_BMP_FillBlockMoveMain_Place( DEFF_BMP_FILL_BLOCK_MOVE* p_work );


#define DEMO_LASTER_SCALE_SS		( FX32_CONST( 0.1f ) )	// 拡縮初速度
#define DEMO_LASTER_SIN_R		( FX32_CONST( 18 ) )	// 半径
#define DEMO_LASTER_SP			( 4*100 )				// ラスタースピード
#define DEMO_LASTER_SIN_ADDR		( (0xffff/192) * 2 )	// ラスター角度スピード

#define DEMO_LASTER_TASK_PRI		( TCB_TSK_PRI - 1 )
#define DEMO_LASTER_FLIP_DOT		( 2 )


//-------------------------------------
//	ラスタースクロール　タスク優先順位
//=====================================
#define DEMO_EFF_LASTER_SCRLL_VWAIT_BUFF_SWITCH_TASK_PRI	( 1024 )
#define DEMO_EFF_LASTER_SCRLL_VINTR_DMA_SET_TASK_PRI	( 1024 )
#define DEMO_TR_WATER_HIGH_SIN_R		( FX32_CONST( 12 ) )	// 半径
#define DEMO_TR_WATER_HIGH_LASTER_SP	( 100 )	// ラスタースピード
#define DEMO_TR_WATER_HIGH_SIN_ADDR	( (0xffff/192) * 2 )// ラスター角度スピード
#define DEMO_TR_WATER_HIGH_LASTER_TASK_PRI	( 0x10000 - 1 )
#define DEMO_TR_WATER_HIGH_LASTER_FLIP_DOT	( 2 )
//-------------------------------------
//	p_lasterスクロールワーク
//=====================================
typedef struct {
	LASTER_SCROLL_PTR p_laster;
	TCB_PTR VDma;
	u32 dmacount;
} DEMO_LASTER_SCROLL;


extern void DEFF_Laster_Init( DEMO_LASTER_SCROLL* p_laster, u32 heapID );
extern void DEFF_Laster_Delete( DEMO_LASTER_SCROLL* p_laster );
extern void DEFF_Laster_Start( DEMO_LASTER_SCROLL* p_laster, u8 start, u8 end, u16 add_r, fx32 r_w, s16 scr_sp, u32 bg_no, u32 init_num, u32 tsk_pri, int mode );
extern void DEFF_Laster_BuffYSet( DEMO_LASTER_SCROLL* p_laster, s16 start_y );
extern void DEFF_Laster_BuffXFlip( DEMO_LASTER_SCROLL* p_laster, u32 dot );


//-------------------------------------
//	
//	デフォルトラスターシステム
//
//	３０分の１フレームVar
//
//	このシステムがやること
//		初期化処理		をまとめて行う
//		破棄処理		をまとめて行う
//		VBlank期間処理	をまとめて行う
//
//	使う人がやること
//		バッファへの値の設定
//	
//=====================================
#define FEFTOOL_DEFLASTER_BUFF_SIZE	(192)
typedef struct _FEFTOOL_DEFLASTER*			FEFTOOL_DEFLASTER_PTR;
typedef const struct _FEFTOOL_DEFLASTER*	CONST_FEFTOOL_DEFLASTER_PTR;

extern FEFTOOL_DEFLASTER_PTR FEFTool_InitDefLaster( u32 addr, u32 init_num, int heap );
extern void FEFTool_DeleteDefLaster( FEFTOOL_DEFLASTER_PTR p_lst );
extern void* FEFTool_GetDefLasterWriteBuff( CONST_FEFTOOL_DEFLASTER_PTR cp_lst );
extern const void* FEFTool_GetDefLasterReadBuff( CONST_FEFTOOL_DEFLASTER_PTR cp_lst );
extern void FEFTool_StopDefLaster( FEFTOOL_DEFLASTER_PTR p_lst );
extern void FEFTool_StartDefLaster( FEFTOOL_DEFLASTER_PTR p_lst );
extern void FEFTool_SetDefLasterTransAddr( FEFTOOL_DEFLASTER_PTR p_lst, u32 addr );
extern u32	FEFTool_GetDefLasterTransAddr( FEFTOOL_DEFLASTER_PTR p_lst );

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
//
//	モーションブラー初期化構造体
//
//=====================================
typedef struct {
	// グラフィックモード
	GXDispMode dispMode;	// ディスプレイモード
	GXBGMode bgMode;		// BGモード	
	GXBG0As bg0_2d3d;		// BG0を3dに使用するか

	// キャプチャ
	GXCaptureSize sz;		// キャプチャサイズ
    GXCaptureMode mode;		// キャプチャモード
    GXCaptureSrcA a;		// キャプチャ先A
    GXCaptureSrcB b;		// キャプチャ先B
    GXCaptureDest dest;		// キャプチャデータ転送Vram
    int eva;				// ブレンド係数A
    int evb;				// ブレンド係数B

	int heap_id;			// 使用するヒープID
}DEMO_MOTION_BL_DATA;


typedef struct _DEMO_MOTION_BL_TASK*	DEMO_MOTION_BL_PTR;	// モーションブラー動作オブジェクト

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
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
extern DEMO_MOTION_BL_PTR DEMO_MOTION_BL_Init(DEMO_MOTION_BL_DATA* init);

//----------------------------------------------------------------------------
/**
 *
 *@brief	モーションブラーを解除
 *
 *@param	data		モーションブラーオブジェクト
 *
 *@return	none
 *	
 *	
 *
 */
//-----------------------------------------------------------------------------
extern void DEMO_MOTION_BL_Delete(DEMO_MOTION_BL_PTR* data, GXDispMode dispMode, GXBGMode bgMode, GXBG0As bg0_2d3d);

//----------------------------------------------------------------------------
/**
 *
 *@brief	途中でパラメーターを変更する
 *
 *@param	eva			ブレンド係数A		Capture元
 *@param	evb			ブレンド係数B		Captureデータ転送先
 *
 *@return	none
 *
 *
 */
//-----------------------------------------------------------------------------
extern void DEMO_MOTION_BL_ParamChg(DEMO_MOTION_BL_PTR data, int eva, int evb);


#endif