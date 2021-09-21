#ifndef __F_ECCOUNT_H__
#define __F_ECCOUNT_H__

///HDMA転送先アドレス
#define ENCOUNT_HDMA_DEST_ADDRS					((u32)&reg_G2_BG2PA)

#define ENCOUNT_TR_DAN_HIGH_BALL_MOVE_SYNC		( 2 )	// 移動シンク
#define ENCOUNT_TR_DAN_HIGH_BLOCK_SYNC			( 1 )	// ワイプシンク
#define ENCOUNT_TR_DAN_HIGH_BLOCK_START_SYNC	( 1 )	// ワイプシンク


#define TCB_TSK_PRI	(5)
#define TCB_VWAIT_BR_TSK_PRI	(10)

///ラスターバッファ
typedef struct{
	//BG2
	u16 bg2_pa;
	u16 bg2_pb;
	u16 bg2_pc;
	u16 bg2_pd;
	u32 bg2_x;
	u32 bg2_y;

	//BG3
	u16 bg3_pa;
	u16 bg3_pb;
	u16 bg3_pc;
	u16 bg3_pd;
	u32 bg3_x;
	u32 bg3_y;
}ENC_LASTER_BUFFER;

//-------------------------------------
//	汎用動作
//=====================================
typedef struct {
	int x;
	int s_x;
	int dis_x;
	int count;
	int count_max;
} FENC_MOVE_WORK;

extern void FENC_MoveReq( FENC_MOVE_WORK* p_work, int s_x, int e_x, int count_max );
extern BOOL	FENC_MoveMain( FENC_MOVE_WORK* p_work );

// フラッシュ
extern void FENC_EncountFlashTask(int disp, u32 bright_color, u32 sub_bright_color, BOOL* end, u32 flash_num );

//-------------------------------------
//	
//	ﾏｽﾀｰブライトネス関連
//	
//=====================================
typedef struct {
	FENC_MOVE_WORK brightness;
	int disp;
} BR_BRIGHTNESS_FADE;

#define ENCOUNT_TR_DAN_HIGH_BLOCK_FILLCOLOR	( 15 )	// 塗りつぶ

extern void FENC_SetMstBrightness( int disp, int no );
extern void FENC_ChangeMstBrightness( BR_BRIGHTNESS_FADE* p_data, int start, int end, int disp, int sync );
extern BOOL FENC_ChangeMstBrightnessMain( BR_BRIGHTNESS_FADE* p_data );
extern void FENC_V_SetMstBrightness( int disp, int no );


// ビットマップブロック状塗りつぶしオブジェクト
typedef struct _FENC_BMP_FILL_BLOCK FENC_BMP_FILL_BLOCK;

extern FENC_BMP_FILL_BLOCK* FENC_BMP_FillBlockAlloc( u32 heapID );
extern void FENC_BMP_FillBlockDelete( FENC_BMP_FILL_BLOCK* p_work );
extern void FENC_BMP_FillBlockStart( FENC_BMP_FILL_BLOCK* p_work, int s_x, int e_x, int s_y, int e_y, int sync, GF_BGL_BMPWIN* p_bmp, u32 width, u32 height, u8 col );
extern BOOL FENC_BMP_FillBlockMain( FENC_BMP_FILL_BLOCK* p_work );

// ビットマップブロック埋め尽くしシステム
typedef struct _FENC_BMP_FILL_BLOCK_MOVE FENC_BMP_FILL_BLOCK_MOVE;

extern FENC_BMP_FILL_BLOCK_MOVE* FENC_BMP_FillBlockMoveAlloc( u32 heapID );
extern void FENC_BMP_FillBlockMoveDelete( FENC_BMP_FILL_BLOCK_MOVE* p_work );
extern void FENC_BMP_FillBlockMoveStart( FENC_BMP_FILL_BLOCK_MOVE* p_work, u8 sync, u8 start, GF_BGL_BMPWIN* p_bmp, u8 col );

extern BOOL FENC_BMP_FillBlockMoveMain_Random( FENC_BMP_FILL_BLOCK_MOVE* p_work );
extern BOOL FENC_BMP_FillBlockMoveMain_LinesUP( FENC_BMP_FILL_BLOCK_MOVE* p_work );
extern BOOL FENC_BMP_FillBlockMoveMain_Place( FENC_BMP_FILL_BLOCK_MOVE* p_work );


#define ENCOUNT_LASTER_SCALE_SS		( FX32_CONST( 0.1f ) )	// 拡縮初速度
#define ENCOUNT_LASTER_SIN_R		( FX32_CONST( 18 ) )	// 半径
#define ENCOUNT_LASTER_SP			( 4*100 )				// ラスタースピード
#define ENCOUNT_LASTER_SIN_ADDR		( (0xffff/192) * 2 )	// ラスター角度スピード

#define ENCOUNT_LASTER_TASK_PRI		( TCB_TSK_PRI - 1 )
#define ENCOUNT_LASTER_FLIP_DOT		( 2 )


//-------------------------------------
//	ラスタースクロール　タスク優先順位
//=====================================
#define ENCOUNT_EFF_LASTER_SCRLL_VWAIT_BUFF_SWITCH_TASK_PRI	( 1024 )
#define ENCOUNT_EFF_LASTER_SCRLL_VINTR_DMA_SET_TASK_PRI	( 1024 )
#define ENCOUNT_TR_WATER_HIGH_SIN_R		( FX32_CONST( 12 ) )	// 半径
#define ENCOUNT_TR_WATER_HIGH_LASTER_SP	( 100 )	// ラスタースピード
#define ENCOUNT_TR_WATER_HIGH_SIN_ADDR	( (0xffff/192) * 2 )// ラスター角度スピード
#define ENCOUNT_TR_WATER_HIGH_LASTER_TASK_PRI	( 0x10000 - 1 )
#define ENCOUNT_TR_WATER_HIGH_LASTER_FLIP_DOT	( 2 )
//-------------------------------------
//	p_lasterスクロールワーク
//=====================================
typedef struct {
	LASTER_SCROLL_PTR p_laster;
	TCB_PTR VDma;
	u32 dmacount;
} FENCOUNT_LASTER_SCROLL;


extern void FEF_Laster_Init( FENCOUNT_LASTER_SCROLL* p_laster, u32 heapID );
extern void FEF_Laster_Delete( FENCOUNT_LASTER_SCROLL* p_laster );
extern void FEF_Laster_Start( FENCOUNT_LASTER_SCROLL* p_laster, u8 start, u8 end, u16 add_r, fx32 r_w, s16 scr_sp, u32 bg_no, u32 init_num, u32 tsk_pri );
extern void FEF_Laster_BuffYSet( FENCOUNT_LASTER_SCROLL* p_laster, s16 start_y );
extern void FEF_Laster_BuffXFlip( FENCOUNT_LASTER_SCROLL* p_laster, u32 dot );


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

extern FEFTOOL_DEFLASTER_PTR FEFTool_InitDefLaster(u32 addr,ENC_LASTER_BUFFER *init_num,int heap );
extern void FEFTool_DeleteDefLaster( FEFTOOL_DEFLASTER_PTR p_lst );
extern void* FEFTool_GetDefLasterWriteBuff( CONST_FEFTOOL_DEFLASTER_PTR cp_lst );
extern const void* FEFTool_GetDefLasterReadBuff( CONST_FEFTOOL_DEFLASTER_PTR cp_lst );
extern void FEFTool_StopDefLaster( FEFTOOL_DEFLASTER_PTR p_lst );
extern void FEFTool_StartDefLaster( FEFTOOL_DEFLASTER_PTR p_lst );
extern void FEFTool_SetDefLasterTransAddr( FEFTOOL_DEFLASTER_PTR p_lst, u32 addr );
extern u32	FEFTool_GetDefLasterTransAddr( FEFTOOL_DEFLASTER_PTR p_lst );


#endif