//==============================================================================
/**
 * @file	br_private.h
 * @brief	
 * @author	goto
 * @date	2007.07.27(金)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================
#ifndef __BR_PRIVATE_H__
#define __BR_PRIVATE_H__

#include "system/snd_tool.h"
#include "field/fld_bgm.h"
#include "field/fieldsys.h"
#include "savedata/battle_rec.h"
#include "field/field_battle.h"
#include "include/application/br_sys.h"
#include "batt_rec_gra.naix"
#include "application/codein/cur.h"
#include "gflib/button_man.h"
#include "system/font_arc.h"
#include "include/msgdata/msg_battle_rec.h"
#include "include/msgdata/msg_br_ranking.h"
#include "include/savedata/battle_rec.h"
#include "system/fontoam.h"
#include "application/imageClip/imc_playsys.h"
#include "savedata/imageclip_data.h"
#include "src/savedata/gds_profile_types.h"

#include "gds_dressup.h"
#include "gds_boxshot.h"
#include "gds_data_conv.h"
#include "gds_ranking.h"
#include "gds_battle_rec.h"
#include "gds_rap.h"
#include "gds_rap_response.h"

#include "src/application/zukanlist/zkn_data/zukan_data.naix"
#include "application/zukanlist/zkn_sort_akstnhmyrw_idx.h"
#include "include/savedata/zukanwork.h"
#include "include/system/softsprite.h"
#include "include/poketool/boxdata.h"

#include "src/field/fieldobj_code.h"
#include "include/battle/trtype_def.h"
#include "src/graphic/wifi2dchar.naix"
#include "communication/communication.h"
#include "include/savedata/fnote_mem.h"

#include "br_snd_def.h"

#include "br_define.h"			///< いろんな定義がまとめてある


// -----------------------------------------
//
//	□メニューデータ管理
//
// -----------------------------------------
typedef struct _BR_MENU_DATA BR_MENU_DATA;
typedef struct _BR_MENU_DATA {	
	BOOL			active;		///< 有効なデータかどうか	
	BOOL			seamless;	///< シームレス
	int				msgID;		///< message
	int				ID;			///< 識別用	
	int				seq;		///< 次のシーケンス
	int				tag_seq;	///< タグの動き
	int				ex_param1;	///< 拡張データ
	int				ex_param2;
	const BR_MENU_DATA*	data;	///< データ	
} BR_MENU_DATA;


// -----------------------------------------
//
//	□スライダー管理
//
// -----------------------------------------
typedef struct {
	int 	frame;
	BOOL	bHit;
	u32 	old_x;
	u32 	old_y;
	s16		value;	
	BOOL	bAnime;
	CATS_ACT_PTR cap[ 2 ];	
} BR_SLIDER;


// -----------------------------------------
//
//	□リスト描画情報
//
// -----------------------------------------
typedef struct {
	int msg;
	int	param;
	u64	param_ex;
} TL_INFO;


// -----------------------------------------
//
//	□リスト登録ヘッダー
//
// -----------------------------------------
typedef struct {	
	const TL_INFO*	info;			///< 情報
	int			data_max;		///< データの個数
	int			list_height;	///< １件の高さ( 2行なら2 )	
	int			px;
	int			py;
	int			sx;
	int			sy;
	int			ofs;
	int			frame;
	u32			lim;
	const RECT_HIT_TBL* hit;		
} TOUCH_LIST_HEADER;


// -----------------------------------------
//
//	□可視範囲でのリスト操作用ワーク
//
// -----------------------------------------
typedef struct {
	int			this;		///< 現在値
	int			l_pos;		///< 最後の移動位置
	int			t_stoper;	///< 上のストッパー位置(下へ行くとき参照)
	int			b_stoper;	///< 下のストッパー位置(上へ行くとき参照)
} TOUCH_LIST_VIEW_MODE;


// -----------------------------------------
//
//	□タッチリスト管理構造体
//
// -----------------------------------------
typedef struct {
	int min;
	int max;
	s16 now;
	int old;
	int	lim;
	int	page;
	GF_BGL_BMPWIN		win;
	const TOUCH_LIST_HEADER*	head;
	CATS_ACT_PTR		cap[ 2 ];		///< 上下カーソル
	MSGDATA_MANAGER*	man;			///< どのメッセージマネージャーを使うか	
	TOUCH_LIST_VIEW_MODE view;			///< 可視範囲でのリスト操作用	
	BOOL bView;
	int	 dmy;	
} TOUCH_LIST;

// -------------------------------------
//
//	□エラー処理
//
// -------------------------------------
typedef struct {	
	GF_BGL_BMPWIN		err_win;		///< err win
	int					msg_index;		///< index;	
	BOOL				bErrorInit;
} ERROR_WORK;

typedef struct {
	int					main_seq;
	int					sub_seq;
	int					proc_seq;
	int					err_msg_id;
	
	int					err_seq;
	int					err_count;
	
	BOOL				bError;
	BOOL				dmy;
} COMM_ERROR_WORK;

// -------------------------------------
//
//	□システムワーク
//
// -------------------------------------
typedef struct {
	
	ARCHANDLE*				p_handle;			///< アーカイブハンドル
	
	CATS_SYS_PTR			csp;				///< OAMシステム
	CATS_RES_PTR			crp;				///< リソース一括管理
	CATS_RES_PTR			vintr_crp;			///< VBlank
	GF_BGL_INI*				bgl;				///< BGL
	PALETTE_FADE_PTR		pfd;				///< パレットフェード
	
	BUTTON_MAN*				btn;				///< ボタン	
	RECT_HIT_TBL			rht[ eHRT_MAX ];	///< 当たり判定(managerに登録する)		
	BOOL					touch;
	
	MSGDATA_MANAGER*		man;
	MSGDATA_MANAGER*		man_ranking;
	FONTOAM_SYS_PTR			fontoam_sys;
	
	int						master_color;		///< カラー
	int						logo_color;			///< バックグラウンドの色指定
	u8						color_type;
	u8						dummy[ 3 ];
	
	GF_G3DMAN*				g3Dman;
	SOFT_SPRITE_MANAGER*	ssm_p;
	
	GDS_RAP_WORK			gdsrap;				///<GDSライブラリ、NitroDWC関連のワーク構造体
	
	BR_SLIDER				slider;				///< スライダーワーク　どうかと思ったがＯＡＭも入れた
	TOUCH_LIST				touch_list;			///< タッチリストワーク
	
	ERROR_WORK				err_wk;
	COMM_ERROR_WORK			comm_err_wk;		///< 通信エラーワーク
	
	BOOL					bDraw;
	BOOL					bCATS;
	
} BR_SYS;


// -------------------------------------
//
//	□サイドバー
//
// -------------------------------------
typedef struct {
	
	CATS_ACT_PTR	cap[ SIDE_BAR_OAM ];
	TCB_PTR			tcb;

	fx32			ox;
	f32				scale;
	s16				val_x;
	s16				pos_t;
	
	s8				move;
	u8				id;	
	int				vcount;
	
	int				rad;
	int				move_f;
	int				wait;
	int				state;
	
} SIDE_BAR;


// -------------------------------------
//
//	□タッチアクション
//
// -------------------------------------
typedef struct {
	BOOL 			bAction;
	int				cnt;
	CURSOR_WORK*	cur;
	TCB_PTR			tcb;
} TOUCH_ACTION;


// -------------------------------------
//
//	□タグ
//
// -------------------------------------
typedef struct {	
	CATS_ACT_PTR	cap;				///< OAM
	TCB_PTR			tcb;				///< TCB	
	int				id;					///< 管理用ID	
	BOOL			active;				///< 自律フラグ　TRUE = 外影響は受けない	
	RECT_HIT_TBL*	hit;				///< 当たり判定のポインタ	
	FONTOAM_OBJ_PTR				font_obj;	///< font oam
	FONTOAM_OAM_DATA_PTR		font_data;
	CHAR_MANAGER_ALLOCDATA		font_vram;	
} TAG;


// -------------------------------------
//
//	□タグ管理
//
// -------------------------------------
typedef struct {	
	TAG			tag_s[ TAG_MAX ];		///< 上のTAG
	TAG			tag_m[ TAG_MAX ];		///< 下のTAG	
	const BR_MENU_DATA*	menu_data_stack[ TAG_MAX ];	
	int			stack_s;				///< どこまで積んでるか
	int			page;					///< ページ
	int			item_max;				///< ページアイテム数
	int			cur_pos;				///< カーソル位置
	int			hit_state;	
	BOOL		bHit;	
} TAG_MANAGER;

// -------------------------------------
//
//	□マネージャが監視するためのワーク
//
// -------------------------------------
typedef struct {	
	BOOL	bFlag;			///< とりあえず、TRUE=録画再生 FALSE=通常終了
	int		state;	
	int		ID;
	int		dummy;	
} POST_MESSAGE;


// -------------------------------------
//
//	□リカバーワーク
//
// -------------------------------------
typedef struct {	
	BOOL	bRecover;	
	int		last_seq;
	int		param[ BR_RECOVER_WORK_MAX ];	
} BR_RECOVER_WORK;


// -------------------------------------
//
//	□戦闘から復帰した際に残しておきたい受信データ
//
// -------------------------------------
typedef struct {
	GT_BATTLE_REC_RECV*			rec_data_tmp;				///< レコードデータ
	GT_BATTLE_REC_RECV			rec_data;
	u8							rec_secure_flag;			///< 
	u8							dmy[ 3 ];
	
	GT_BATTLE_REC_OUTLINE_RECV*	outline_data_tmp[ 30 ];		///< DL した概要一覧
	GT_BATTLE_REC_OUTLINE_RECV	outline_data[ 30 ];			///< DL した概要一覧
	int							outline_data_num;			///< DL した概要一覧の件数
	u8							outline_secure_flag[ 30 ];	///< DL した概要のフラグ
	
} BR_RECV_DATA;


// ------------------------------------
//
//	□VBLANK期間中に画面を切り替えるワーク
//
// ------------------------------------
typedef struct {	
	int		disp_sw;
	BOOL	bChange;	
} BR_DISP_SW;

// -------------------------------------
//
//	メインワーク
//
// -------------------------------------
typedef struct {	
	int					mode;
	int					main_seq;
	int					sub_seq;
	int					wait;
	int*				proc_seq;					///< プロックの*seq
	
	BR_SYS				sys;						///< システム	
	BR_DISP_SW			disp;						///< 表示面
	
	TAG_MANAGER			tag_man;					///< タグ管理	
	CURSOR_WORK*		cur_wk;						///< ホーミングカーソル		
	CURSOR_WORK*		cur_wk_top;					///< ホーミングカーソル	(上画面用)
	SIDE_BAR			bar[ SIDE_BAR_TOTAL ];		///< サイドバー
	
	const BR_MENU_DATA*	menu_data;					///< メニューデータ
	const BR_MENU_DATA*	menu_data_mem[ TAG_MAX ];	///< メニューデータ
	
	SAVEDATA*			save;						///< 必要になった
	u16					save_work1;					///< 分割セーブ用
	u16					save_work2;					///< 分割セーブ用
	
	BR_RECOVER_WORK		recover_wk;					///< 復帰用ワーク
	
	void*				sub_work;					///< 内部で汎用的に使うワーク
	void*				app_work;					///< 内部でアプリを起動したときに使うワーク
	POST_MESSAGE*		message;					///< 管理部分とのやり取り
		
	int					ex_param1;					///< メニューからもらうデータ
	BOOL				ex_param2;					///< 起動後1度しかできないものを管理するフラグ
	u32					ex_param3;					///< バトルビデオ安全フラグ TRUE=危険	
	u8*					secure_addrs;				///< secure_flag の アドレス

	BATTLE_REC_HEADER_PTR	br_head[ 4 ];			///< 録画データヘッダ
	GDS_PROFILE_PTR			br_gpp[ 4 ];			///< 録画データプロフィール
	
	///< info用
	BOOL				bWiFiEnd;					///< wifi 終了時のフラグ
	BOOL				bInfo;						///< info win
	GF_BGL_BMPWIN		info_win;					///< info win
	
	BR_RECV_DATA		recv_data;					///< 通信対戦後も保持するため	
	TOUCH_ACTION		touch_work;					///< タッチアクション
	
	TCB_PTR				view_tcb;
//	FIELDSYS_WORK*		fsys;						///< いらなくなった

} BR_WORK;

// -------------------------------------------------------------
//
//	■ サブワーク関連 いろんなシーケンスの中でメモリ確保される
//
// -------------------------------------------------------------
// -----------------------------------------
//
//	【 サブワークに登録 】タグの移動用ワーク
//
// -----------------------------------------
typedef struct {	
	
	int		wait;		///< ウェイト
	int		rad;		///< 角度
	s16		ty;
	s16		dmy;
	fx32	ox;
	
} BR_TAG_MOVE_WORK;

// -----------------------------------------
//
//	【 サブワークに登録 】タグ専用ワーク
//
// -----------------------------------------
typedef struct {

	BR_TAG_MOVE_WORK	mv_wk[ TAG_MAX ];	
	u8	color;
	u8	dmy[ 3 ];
	
} BR_TAG_WORK;


// -----------------------------------------
//
//	【 サブワークに登録 】タグ専用選択ワーク
//
// -----------------------------------------
typedef struct {
	
	int		color;
	BOOL 	bEnd;
	BOOL	bInit;
	BOOL 	bTagMove_U;
	
} BR_TAG_SEL_WORK;


// -----------------------------------------
//
//	【 サブワークに登録 】タグ専用TCBワーク
//
// -----------------------------------------
typedef struct {
	
	s16		s_px;		///< 開始位置 X
	s16		s_py;		///< 開始位置 Y
	s16		t_px;		///< 目的位置 X
	s16		t_py;		///< 目的位置 Y
	s16		mx;
	s16		my;
	int		s_rad;		///< 角度
	int		t_rad;		///< 角度
	int		add_rad;
	int		temp;
	
	u8		wait;		///< wait
	u8		seq;		///< シーケンス
	u8		tag_max;
	u8		dmy;
	
	CATS_ACT_PTR cap;	///< 対象のOAM
	FONTOAM_OBJ_PTR	font_obj;
	
	CURSOR_WORK* cur_wk;
	
	BOOL*	bEnd;
	BOOL*	bTagMove_U;
	
} BR_TAG_TCB_WORK;



// =============================================================================
//
//
//	■プロトタイプ宣言
//
//
// =============================================================================
extern void BR_VBlank( void* work );

// br_manager.c
extern BR_WORK* BR_PROC_GetWork( PROC * proc );
extern void BR_PostMessage( POST_MESSAGE* message, BOOL bFlag, int state );


// br_main.c
extern void BR_SystemInit( BR_WORK* wk );
extern void BR_SystemExit( BR_WORK* wk );
extern BOOL BR_Main_Update( BR_WORK* wk, int* seq );
extern void BR_Main_ProcSeqChange( BR_WORK* wk, int next_seq, BOOL flag, int* seq );
extern void BR_Main_SeqChange( BR_WORK* wk, int next_seq );
extern void BR_Main_SubSeqChange( BR_WORK* wk, int next_seq );
extern BOOL BR_UnlockMenuCheck( BR_WORK* wk );

extern void BR_Main_MenuChangeInit( BR_WORK* wk );
extern BOOL BR_Main_MenuChangeNext( BR_WORK* wk );
extern BOOL BR_Main_MenuChangeBack( BR_WORK* wk );

extern void BR_MenuData_Analysis( BR_WORK* wk );
extern void BR_Stack_Menu_Back( BR_WORK* wk );

extern void BR_TouchPanel_Init( BR_WORK* wk );
extern void BR_TouchPanel_TagHRT_Set( BR_WORK* wk ); 
extern void BR_TouchPanel_TagHRT_ReSet( BR_WORK* wk );
extern void BR_TouchPanel_Update( BR_WORK* wk );

extern void BR_RecoverWorkClear( BR_WORK* wk );
extern BOOL BR_RecoverCheck( BR_WORK* wk );
extern int BR_RecoverSeqGet( BR_WORK* wk );
extern void BR_RecoverSeqSet( BR_WORK* wk, int seq );

extern void BR_ChangeDisplay( int mode );

// br_disp.c
extern void BR_disp_BG_Load( BR_WORK* wk );
extern void BR_disp_BG_InfoWindowLoad( BR_WORK* wk, int frame, BOOL yesno );
extern void BR_disp_BG_InfoWindowLoad_Single( BR_WORK* wk, int frame );
extern void BR_SideBar_Load( BR_WORK* wk );
extern void BR_SideBar_TCB( TCB_PTR tcb, void* work );
extern void BR_SideBar_Add( BR_WORK* wk );
extern void BR_SideBar_Delete( BR_WORK* wk );
extern void BR_SideBar_TCB_Delete( BR_WORK* wk );
extern void BR_Tag_Load( BR_WORK* wk );
extern void BR_Tag_Add( BR_WORK* wk );
extern void BR_Tag_Delete( BR_WORK* wk );
extern void BR_Tag_ReSet( BR_WORK* wk );
extern void BR_Tag_SubRecover( BR_WORK* wk );
extern void BR_Tag_PosReset_Sub( BR_WORK* wk );

extern void BR_disp_FontOam_Create( BR_WORK* wk );
extern void BR_disp_FontOAM_MegWrite( BR_WORK* wk, TAG* tag, int mes_id, const BR_MENU_DATA* dat );
extern void BR_disp_FontOam_Delete( BR_WORK* wk );
extern void BR_Tag_Release( BR_WORK* wk );

// br_start.c
extern BOOL (* const BR_start_MainTable[])( BR_WORK* wk );
extern BOOL BR_start_Fade( BR_WORK* wk );
extern BOOL BR_start_On( BR_WORK* wk );
extern BOOL BR_start_MenuIn( BR_WORK* wk );

// br_input.c
extern BOOL (* const BR_input_MainTable[])( BR_WORK* wk );
extern void BR_TagMove_TCB( TCB_PTR tcb, void* work );
extern void BR_CATS_ObjectPosSetCap(CATS_ACT_PTR cap, s16 x, s16 y);
extern void BR_CATS_ObjectPosGetCap(CATS_ACT_PTR cap, s16* x, s16* y);
extern void BR_CATS_ObjectPosSetCapFx32(CATS_ACT_PTR cap, fx32 x, fx32 y);
extern void BR_CATS_ObjectPosGetCapFx32(CATS_ACT_PTR cap, fx32* x, fx32* y);


// br_menu_data.c
extern const BR_MENU_DATA* BattleRecorder_MenuDataGet( int mode );
extern const BR_MENU_DATA* BattleRecorder_MenuDataGetEx( BR_WORK* wk, int mode );
extern void BattleRecorder_MenuSetting( BR_WORK* wk );

// br_delete.c
extern BOOL (* const BR_delete_MainTable[])( BR_WORK* wk );

// br_profile.c
extern BOOL (* const BR_prof_MainTable[])( BR_WORK* wk );
extern int  BR_print_x_Get( GF_BGL_BMPWIN* win, STRBUF* str );

// br_frontier.c
extern BOOL (* const BR_frec_MainTable[])( BR_WORK* wk );

// br_photo
extern BOOL (* const BR_photo_send_MainTable[])( BR_WORK* wk );
extern BOOL (* const BR_photo_recv_MainTable[])( BR_WORK* wk );

// br_tool.c
extern void PaletteFadeClear( BR_WORK* wk );
extern void BG2_Recover( BR_WORK* wk );


///< あかさたな～取得
extern u16* ZukanSortDataGet_50on( int heap, int idx, int* p_arry_num );

// =============================================================================
//
//
//	■ FONT OAM
//
//
// =============================================================================
typedef struct {

	int type;	
	CATS_ACT_PTR				cap;
	FONTOAM_OBJ_PTR				font_obj;
	FONTOAM_OAM_DATA_PTR		font_data;
	CHAR_MANAGER_ALLOCDATA		font_vram;

} OAM_BUTTON;

extern void FontOam_Create( OAM_BUTTON* obtn, BR_SYS* sys, int draw_area );
extern void FontOam_MsgSet( OAM_BUTTON* obtn, BR_SYS* sys, int msg_id );
extern void FontOam_Delete( OAM_BUTTON* obtn );

extern void ExTag_ResourceLoad_Box( BR_WORK* wk, int draw_area );
extern void ExTag_ResourceLoad_Save( BR_WORK* wk, int draw_area );
extern void ExTag_ResourceLoad_VideoRank( BR_WORK* wk, int draw_area );
extern void ExTag_ResourceLoad_Rank( BR_WORK* wk, int draw_area );
extern void ExTag_ResourceLoad( BR_WORK* wk, int draw_area );
extern void ExTag_ResourceDelete( BR_WORK* wk );
extern CATS_ACT_PTR ExTag_Add( BR_WORK* wk, int d_area );

extern void BR_Tag_Enable( BR_WORK* wk, BOOL flag );

extern void BR_Tag_FontOamReset( BR_WORK* wk );

extern void NormalTag_DeleteAllOp( BR_WORK* wk );
extern void NormalTag_RecoverAllOp( BR_WORK* wk );
extern void NormalTag_RecoverAllOp_Simple( BR_WORK* wk );

enum {
	
	eFADE_MODE_IN	= 0,
	eFADE_MODE_OUT,
	
};

enum {
	
	ePLANE_ALL = 0,
	ePLANE_MAIN,
	ePLANE_SUB,
	
};

extern void Plate_AlphaInit( int* eva, int* evb, int mp1, int mp2, int sp1, int sp2, int plane );
extern void Plate_AlphaSetPlane( int* eva, int* evb, int mp1, int mp2, int sp1, int sp2, int plane );
extern BOOL Plate_AlphaFade( int* eva, int* evb, int mode, int plane );
extern void Plate_AlphaInit_Default( int* eva, int* evb, int plane );
extern BOOL BR_PaletteFade( int* color, int mode );
extern void BR_PaletteFadeIn_Init( int* color );
extern void BR_PaletteFadeOut_Init( int* color );

extern void Recoder_ColorChange( BR_WORK* wk, int color );

// =============================================================================
//
//
//	■	ボックス
//
//
// =============================================================================
extern BOOL (* const BR_BoxS_MainTable[])( BR_WORK* wk );	///< 自分の送信
extern BOOL (* const BR_BoxR_MainTable[])( BR_WORK* wk );	///< ランキングから受信

extern void HitTbl_to_Center( const RECT_HIT_TBL* tbl, s16* x, s16* y );
extern BOOL Cursor_Panel_Check( BR_WORK* wk );
extern void HormingCursor_Init( BR_WORK* wk, CURSOR_WORK* cur, s16 x, s16 y );



extern void ExParam2_Set( BR_WORK* wk, BOOL flag );
extern BOOL ExParam2_Get( BR_WORK* wk );

extern void Tool_InfoMessage( BR_WORK* wk, int no );
extern void Tool_InfoMessageSet( BR_WORK* wk, int no );
extern void Tool_InfoMessageDel( BR_WORK* wk );
extern void Tool_InfoMessageMain( BR_WORK* wk, int no );
extern void Tool_InfoMessageMainSet( BR_WORK* wk, int no );
extern void Tool_InfoMessageMainDel( BR_WORK* wk );
extern void Tool_WiFiEnd_Message( BR_WORK* wk, int no );
extern void InfoMessageDel( BR_WORK* wk );


typedef struct {
	
	GDS_PROFILE_PTR		gpp;
	CATS_ACT_PTR		cap_poke;
	CATS_ACT_PTR		cap_hero;
	GF_BGL_BMPWIN		win[ 8 ];
	
} GPP_WORK;

extern void GppDataMake( GPP_WORK* gwk, BR_WORK* wk );
extern void GppDataFree( GPP_WORK* gwk, BR_WORK* wk );
extern void GppData_IconEnable( GPP_WORK* gwk, int flag );


typedef struct {
	
	GDS_PROFILE_PTR			gpp;
	BATTLE_REC_HEADER_PTR	br_head;
//	GT_BATTLE_REC_HEADER	br_head;
	
	CATS_ACT_PTR		cap_hero;
	CATS_ACT_PTR		cap_poke[ 12 ];
	GF_BGL_BMPWIN		win[ 8 ];
	
} VIDEO_WORK;

extern void VideoDataMake( VIDEO_WORK* vwk, BR_WORK* wk );
extern void VideoDataFree( VIDEO_WORK* vwk, BR_WORK* wk );
extern void VideoData_IconEnable( VIDEO_WORK* vwk, int flag );

// =============================================================================
//
//
//	■ランキング
//
//
// =============================================================================
extern BOOL (* const BR_WeeklyRanking_MainTable[])( BR_WORK* wk );

// =============================================================================
//
//
//	■ビデオランキング
//
//
// =============================================================================
extern BOOL (* const BR_VideoRanking_MainTable[])( BR_WORK* wk );

// =============================================================================
//
//
//	■ビデオ検索
//
//
// =============================================================================
extern BOOL (* const BR_Search_MainTable[])( BR_WORK* wk );
extern BOOL (* const BR_Video_MainTable[])( BR_WORK* wk );
extern BOOL (* const BR_Video_Send_MainTable[])( BR_WORK* wk );
extern int WiFi_TR_DataGet( int trtype, int data );
extern void CurPosSet( CURSOR_WORK* cur, s16 x, s16 y, u32 event );
extern int Number_to_Unit_Get( u64 num );


// =============================================================================
//
//
//	■　Wi-Fi 関連
//
//
// =============================================================================
extern GDS_RAP_WORK* BR_GDSRAP_WorkGet( BR_WORK* wk );
extern void BR_GDSRAP_Init( BR_WORK* wk );
extern BOOL BR_GDSRAP_MoveStatusAllCheck( BR_WORK* wk );
extern void BR_GDSRAP_Main( BR_WORK* wk );
extern void BR_GDSRAP_Exit( BR_WORK* wk );


extern void BR_Response_BoxRegist( void *work, const GDS_RAP_ERROR_INFO *error_info );
extern void BR_Response_BoxGet( void *work, const GDS_RAP_ERROR_INFO *error_info );
extern void BR_Response_DressRegist(void *work, const GDS_RAP_ERROR_INFO *error_info);
extern void BR_Response_DressGet(void *work, const GDS_RAP_ERROR_INFO *error_info);

extern void BR_Response_RankingType(void *work, const GDS_RAP_ERROR_INFO *error_info);
extern void BR_Response_RankingUpdate(void *work, const GDS_RAP_ERROR_INFO *error_info);

extern void BR_Response_VideoRegist( void *work, const GDS_RAP_ERROR_INFO* error_info );
extern void BR_Response_VideoGet( void *work, const GDS_RAP_ERROR_INFO* error_info );

extern void BR_Response_VideoDL( void *work, const GDS_RAP_ERROR_INFO* error_info );


// =============================================================================
//
//
//	■色換え
//
//
// =============================================================================
extern int BR_FadeColorCode_Get( BR_WORK* wk );
extern int BR_ColorPaletteID_Get( BR_WORK* wk, int type );
extern void BR_ColorPaletteChange( BR_WORK* wk, int type );

// =============================================================================
//
//
//	■スライダー
//
//
// =============================================================================
extern void Slider_WorkClear( BR_SLIDER* wk );
extern void Slider_Init( BR_SLIDER* wk, BR_WORK* br_wk );
extern void Slider_InitEx( BR_SLIDER* wk, BR_WORK* br_wk, int animeno );
extern void Slider_Exit( BR_SLIDER* wk );
extern void Slider_Value_CheckSet( BR_SLIDER* wk, u32 y );
extern void Slider_HitCheck_Main( BR_SLIDER* wk );
extern void Slider_AnimeCheck( BR_SLIDER* wk, TOUCH_LIST* tl );

// =============================================================================
//
//
//	■タッチリスト
//
//
// =============================================================================
extern void TouchList_WorkClear( TOUCH_LIST* wk );
extern void TouchList_Init( TOUCH_LIST* tl, BR_WORK* wk, const TOUCH_LIST_HEADER* head );
extern void TouchList_InitEx( TOUCH_LIST* tl, BR_WORK* wk, MSGDATA_MANAGER* man, const TOUCH_LIST_HEADER* head );
extern void TouchList_InitRanking( TOUCH_LIST* tl, BR_WORK* wk, MSGDATA_MANAGER* man, const TOUCH_LIST_HEADER* head, int ranking_no, int group_no, STRBUF* unit );
extern void TouchList_InitBoxShot( TOUCH_LIST* tl, BR_WORK* wk, const TOUCH_LIST_HEADER* head, STRBUF* str[20][2] );
extern int TouchList_Ranking_Main( TOUCH_LIST* tl, BR_WORK* wk, int ranking_no, int group_no, STRBUF* unit );
//extern void TouchList_Main( TOUCH_LIST* tl );
extern int TouchList_Main( TOUCH_LIST* tl, BR_WORK* wk );
extern int TouchList_MainBoxShot( TOUCH_LIST* tl, BR_WORK* wk, STRBUF* str[20][2] );
extern void TouchList_Exit( TOUCH_LIST* tl );
extern void TouchList_ValueSet( TOUCH_LIST* tl, s16 val );

extern void TouchList_CursorDel( TOUCH_LIST* tl, BR_WORK* br_wk );
extern void TouchList_CursorEnableCheck( TOUCH_LIST* tl );
extern void TouchList_ViewModeCursorEnableCheck( TOUCH_LIST* tl );
extern void TouchList_CursorAdd( TOUCH_LIST* tl, BR_WORK* br_wk, int d_area );
extern void TouchList_CursorPosSet( TOUCH_LIST* tl, s16 ty, s16 by );

extern void TouchList_ViewMode_ValueSet( TOUCH_LIST* tl, s16 val );
extern int TouchList_ViewMode_Main( TOUCH_LIST* tl, BR_WORK* wk );

// =============================================================================
//
//
//	■
//
//
// =============================================================================
extern BOOL BR_IsGameClear( BR_WORK* wk );
extern void BR_AddScore( BR_WORK* wk, u32 id );
extern void BR_Response_BattleVideoSearch( void *work, const GDS_RAP_ERROR_INFO *error_info );

extern void RecData_To_brs( BR_WORK* wk );
extern BOOL RecData_Save( BR_WORK* wk, int no, u8 secure );
extern void BR_SaveWork_Clear( BR_WORK* wk );

extern void BR_ChangeDisplayVIntr( BR_WORK* wk, int mode );
extern BOOL BR_IsChangeDisplay( BR_WORK* wk );
extern void BR_DrawTCB_Add( BR_WORK* wk );
extern void TouchAction( BR_WORK* wk );

extern void PokemonIcon_ResLoad( BR_WORK* wk );
extern void PokemonIcon_ResFree( BR_WORK* wk );
extern CATS_ACT_PTR PokemonIcon_Add( BR_WORK* wk, int id, int monsno, int form, int egg );
extern void PokemonIcon_Del( BR_WORK* wk, int id, CATS_ACT_PTR cap );
extern void PokemonIcon_DelActor( BR_WORK* wk, int id, CATS_ACT_PTR cap );
extern CATS_ACT_PTR PokemonIcon_AddHDL( BR_WORK* wk, ARCHANDLE* hdl, int id, int monsno, int form, int egg );

extern void BR_SaveData_GppHead_Load( BR_WORK* wk );
extern void BR_SaveData_GppHead_Delete( BR_WORK* wk );

extern ARCHANDLE* PokemonGraphic_ArcOpen( int heap );
extern void		  PokemonGraphic_ArcClose( ARCHANDLE* hdl );


// =============================================================================
//
//
//	■ポケモンアイコン管理
//
//
// =============================================================================
typedef struct {
	
	int	 poke_list_num;	///< リストに表示できる件数
	int	 poke_num;		///< 取得データ件数
	u16* poke_data;		///< ポケモンデータ
	
	MSGDATA_MANAGER* man;
	
	BOOL bInit;
	
} POKE_SEL_WORK;

typedef struct {
	
	int			 gra_id;
	CATS_ACT_PTR cap;
	
} POKE_ICON;

typedef struct {
	
	ARCHANDLE*	 handle;
	int 		 view_start;		///< リストの参照位置
	int			 global_gra_id;		///< グラフィックID
	POKE_ICON 	 poke[ 5 ][ 6 ];	///< 5件 6個
	
} POKE_ICON_WORK;


// =============================================================================
//
//
//	■エラー処理
//
//
// =============================================================================
extern void BR_Error_Init( BR_WORK* wk );
extern void BR_Error_Exit( BR_WORK* wk );
extern void BR_Error_CallBack( void* work, STRBUF* error_msg );


extern void BR_ScrSet( BR_WORK* wk, int id, int frame );
extern BOOL BR_Executed_RecDelete( BR_WORK* wk );


extern int BR_FadeColorCode_Get( BR_WORK* wk );
extern int BR_FadeColorCode_Get_BootLine( BR_WORK* wk );

extern void PrioritySet_Common( void );

extern BOOL IsCATS_SystemActive( BR_WORK* wk );
extern void CATS_SystemActiveSet( BR_WORK* wk, BOOL flag );

extern void BR_ErrorSet( BR_WORK* wk, int mode, int result, int type );
extern void BR_Error_Main( BR_WORK* wk );
extern int BR_ErrorMsgGet( BR_WORK* wk, int mode, int result, int type );

extern BOOL BattleVideo_Preserved( BR_WORK* wk, u64 video_no );

extern int ProfileBGGet( VIDEO_WORK* vwk );
extern BOOL VideoCheck2vs2_Head( BATTLE_REC_HEADER_PTR head );

extern void RecData_RecStopFlagSet( BR_WORK* wk, BOOL flag );
extern void BR_ErrorStrChange( BR_WORK* wk, STRBUF *strbuf );

extern WORDSET* BR_WORDSET_Create( u32 heapID );
extern void BR_Tool_FontButton_Create( BR_WORK* wk, OAM_BUTTON* obtn, int draw_area, int msg_id, s16 x, s16 y, s16 ox, s16 oy, int anime, BOOL flag );
extern void BR_Tool_FontButton_Delete( OAM_BUTTON* obtn );

#endif	// __BR_PRIVATE_H__
