//==============================================================================================
/**
 * @file	poke_test.c
 * @brief	ポケモングラフィックチェック
 * @author	sogabe
 * @date	2005.11.08
 */
//==============================================================================================

#ifdef PM_DEBUG

///< 0 = プレゼン用
///< 1 = 編集用
#define ANIME_EDIT_MODE	( 1 )

#if 1//ANIME_EDIT_MODE

#include "common.h"
#include "system/fontproc.h"
#include "system/font_arc.h"
#include "system/pm_str.h"
#include "system/msgdata.h"						//MSGDATA_MANAGER
#include "system/arc_tool.h"
#include "system/arc_util.h"
#include "system/softsprite.h"
#include "poke_anime.h"

#include "msgdata/msg.naix"						//NARC_msg_??_dat
#include "battle/graphic/pl_batt_bg_def.h"
#include "battle/battle_common.h"
#include "battle/battle_server.h"
#include "battle/fight.h"
#include "poketool/monsno.h"
#include "poketool/poke_tool.h"
#include "system/snd_tool.h"
#include "system/wordset.h"
#include "system/window.h"

#include "pokeanime/p_anm_sys.h"

#include "system/main.h"
#include "demo/title.h"
#include "msgdata/msg_poke_test.h"
#include "msgdata/msg_poke_anime.h"

#include "poketool/pokegra/pokegrano.dat"

extern	BOOL	PMSVLD_Load(u32 src, void * dst, u32 len);
extern	BOOL	PMSVLD_Save(u32 src, void * dst, u32 len);

///< コレを有効にすると、SAVE_BANKからLOADする
//#define		SAVE_BANK_LOAD

#define		SAVE_BANK	(30)
#define		EDIT_BANK	( EDIT_ANM_SAVE_2 )//( EDIT_ANM_SAVE )
#define		HEIGHT_MAX	(32)
#define		HEIGHT_MIN	(-32)
#define		SHADOW_MAX	(2)

//#define		GO_USE		//有効にするとichinose仕様

enum{
	SIDE_MAIN=0,
	SIDE_SUB,
};

enum{
	NUM_CALC_NONE=0,
	NUM_CALC_MOVE,
	NUM_CALC_CHANGE,
	NUM_CALC_END,
};

enum{
	MODE_NORMAL=0,
	MODE_ACTIVE,
	MODE_LOOP,
	MODE_STOP,
	MODE_DEFAULT_PARAM,
	MODE_DEFAULT_WAZA,
};

enum{
	VOICE_ON=0,
	VOICE_OFF,
};

//#if 1
#define POKE_ANM_DEBUG_PRINT(...) (void) ((OS_TPrintf(__VA_ARGS__)))
//#define POKE_ANM_PRINT(...)           ((void) 0)

//==============================================================================================
//
//	構造体
//
//==============================================================================================

#define	PGT_POKE_MAX	(600)					//ポケモンアニメーションデータのMAX
#define	PGT_END_KEY		(PAD_BUTTON_L|PAD_BUTTON_R|PAD_BUTTON_SELECT)

enum{
	SEQ_POKEMON_CHECK_INIT=0,
	SEQ_POKEMON_CHECK,
	SEQ_POKEMON_CHECK_EDIT,
	SEQ_POKEMON_ANIME,
	SEQ_POKEMON_ANIME_EDIT,
	SEQ_LOAD,
	SEQ_SAVE,
	SEQ_POKEMON_ANIME_ACT,
	SEQ_POKEMON_ANIME_ACT_B,
	SEQ_POKEMON_ANIME_ACT_F,
};

enum{
	CUR_PUT=0,
	CUR_DEL
};

typedef struct{
	u8	patno;
	u8	wait;
}PRG_ANIME;

typedef struct{
	u8					prg_anm_f;				//プログラムアニメナンバー（正面）
	u8					prg_anm_f_wait;
	PRG_ANIME			prg_anm_b[3];			//プログラムアニメナンバー（背面）
	SOFT_SPRITE_ANIME	ssanm[SS_ANM_SEQ_MAX];	//パターンアニメデータ
}POKE_ANM_DATA;



// -----------------------------------------
//
//	■パターン定義
//
// -----------------------------------------
enum {
	PAD_F = 0,										///< 正面アニメ	
	PAD_B,											///< 背面アニメ
	PAD_MAX,										///< アニメ最大数	
};

enum {
	
	PAD_MODE_Init = 0,								///< 初期化
	PAD_MODE_BaseParaEdit,							///< 基本パラメータ設定
	PAD_MODE_F_AnimeEdit,							///< 正面アニメ設定
	PAD_MODE_B_AnimeEdit,							///< 背面アニメ設定
	
};

enum {
	
	PAD_PAT_0	= +0,
	PAD_PAT_1	= +1,
	
	PAD_DEF		= -1,
	
	PAD_LOOP_1	= -2,
	PAD_LOOP_2	= -3,
	PAD_LOOP_3	= -4,
	PAD_LOOP_4	= -5,
	PAD_LOOP_5	= -6,
	PAD_LOOP_6	= -7,
	PAD_LOOP_7	= -8,
	PAD_LOOP_8	= -9,
	PAD_LOOP_9	= -10,
	PAD_LOOP_10	= -11,	
};

#define GET_LOOP_LABEL( x )		( ( x - PAD_DEF) * -1 )

#include "src/poketool/poke_tool_def.h"

typedef struct{
	BOOL end_flag;
	GF_BGL_INI			*bgl;
	GF_BGL_BMPWIN		*win_m;
	GF_BGL_BMPWIN		*win_s;
	SOFT_SPRITE_MANAGER	*ssm;
	SOFT_SPRITE			*ss[2];
	MSGDATA_MANAGER		*man_msg;
	MSGDATA_MANAGER		*man_pam;
	MSGDATA_MANAGER		*man_poke;
	STRBUF				*msg_buf;
	STRBUF				*msg_buf2;
	WORDSET*			wordset;
	POKE_ANM_SYS_PTR	pasp;
	int					seq_no;
	int					seq_no_temp;
	int					sub_seq_no;
	int					mons_no;
	int					sex;
	int					col;
	int					anm_pat;
	int					form_no;
	int					pos;
	int					yes_no_pos;
	int					put_req;
	int					main_pos;
	int					cur_pos_keta;
	int					shadow_flag;
	int					bg_flag;
	int					col_flag;
	int					bg_r;
	int					bg_g;
	int					bg_b;
	
	// 新規
	int					sub_page;
	u32					save_bank;
	u32					load_bank;
	BOOL				is_anime;
	BOOL				is_main_page;

	// ここ以下は、セーブしているので、位置の変更は厳禁！	
	POKE_ANM_DATA		pad[PGT_POKE_MAX];
	s8					height[PGT_POKE_MAX];
	u8					shadow_size[PGT_POKE_MAX];
	s8					shadow_ofx[PGT_POKE_MAX];
	
	// プラチナ用保存データ
	POKE_ANM_EDIT_DATA	paed[ PGT_POKE_MAX ];
}POKE_GRA_TEST;

#define POKE_ANM_SAVE_SIZE	( sizeof(POKE_ANM_EDIT_DATA) * PGT_POKE_MAX )


//==============================================================================================
//
//	プロトタイプ宣言
//
//==============================================================================================

static	void	PokeGraInit(POKE_GRA_TEST *pgt);
static	void	PokeGraTestBGCreate(POKE_GRA_TEST *pgt,GF_BGL_INI *bgl);
static	void	PokeTestMainScreenCreate(POKE_GRA_TEST *pgt);
static	void	PokeGraTestDebugPrint(POKE_GRA_TEST *pgt,int mode);
static	void	PokeGraTestVBlank(void *work);
static	void	Init3DStuff_(void);
static	u8		PokeGraTestSexGet(u16 monsno);
static	void	PokeGraTestMSG_Print(POKE_GRA_TEST *pgt);
static	void	PokeGraTestSubScreenMake(POKE_GRA_TEST *pgt);
static	void	PokeGraTestSubScreenAnmFDataPut(POKE_GRA_TEST *pgt,GF_PRINTCOLOR col);
static	void	PokeGraTestSubScreenAnmFWaitDataPut(POKE_GRA_TEST *pgt,GF_PRINTCOLOR col);
static	void	PokeGraTestSubScreenAnmBDataPut(POKE_GRA_TEST *pgt,int num,GF_PRINTCOLOR col);
static	void	PokeGraTestSubScreenAnmBWaitDataPut(POKE_GRA_TEST *pgt,int num,GF_PRINTCOLOR col);
static	void	PokeGraTestSubScreenPatDataPut(POKE_GRA_TEST *pgt,int pos,GF_PRINTCOLOR col);
static	void	PokeGraTestSubScreenWaitDataPut(POKE_GRA_TEST *pgt,int pos,GF_PRINTCOLOR col);
static	void	CursorMove(POKE_GRA_TEST *pgt,int side,int pos,int mode);
static	void	YesNoCursorMove(POKE_GRA_TEST *pgt,int pos,int mode);
static	BOOL	PokeNumAct(POKE_GRA_TEST *pgt);
static	BOOL	PokeSexAct(POKE_GRA_TEST *pgt);
static	BOOL	PokeColorAct(POKE_GRA_TEST *pgt);
static	BOOL	PokePatAct(POKE_GRA_TEST *pgt);
static	BOOL	PokeFormAct(POKE_GRA_TEST *pgt);
static	BOOL	PokeHeightAct(POKE_GRA_TEST *pgt);
static	BOOL	PokeOfxAct(POKE_GRA_TEST *pgt);
static	BOOL	PokeShadowAct(POKE_GRA_TEST *pgt);
static	BOOL	PokeBGColorRAct(POKE_GRA_TEST *pgt);
static	BOOL	PokeBGColorGAct(POKE_GRA_TEST *pgt);
static	BOOL	PokeBGColorBAct(POKE_GRA_TEST *pgt);
static	BOOL	AnmFrontAct(POKE_GRA_TEST *pgt);
static	BOOL	AnmFrontWaitAct(POKE_GRA_TEST *pgt);
static	BOOL	AnmBackAct(POKE_GRA_TEST *pgt);
static	BOOL	AnmBackWaitAct(POKE_GRA_TEST *pgt);
static	BOOL	PatDataAct(POKE_GRA_TEST *pgt);
static	BOOL	WaitDataAct(POKE_GRA_TEST *pgt);
static	BOOL	PokeGraTestLoadAct(POKE_GRA_TEST *pgt);
static	BOOL	PokeGraTestSaveAct(POKE_GRA_TEST *pgt);
static	void	PokeGraTestYesNoPut(POKE_GRA_TEST *pgt);

static	void	PokeTestPokeNumDataPut(POKE_GRA_TEST *pgt,int mode);
static	void	PokeTestPokeSexDataPut(POKE_GRA_TEST *pgt);
static	void	PokeTestPokeColorDataPut(POKE_GRA_TEST *pgt);
static	void	PokeTestPokePatDataPut(POKE_GRA_TEST *pgt);
static	void	PokeTestPokeFormDataPut(POKE_GRA_TEST *pgt,int mode);
static	void	PokeTestPokeHeightDataPut(POKE_GRA_TEST *pgt,GF_PRINTCOLOR col);
static	void	PokeTestPokeOfxDataPut(POKE_GRA_TEST *pgt,GF_PRINTCOLOR col);
static	void	PokeTestPokeShadowDataPut(POKE_GRA_TEST *pgt,GF_PRINTCOLOR col);
static	void	PokeTestPokeBGColorRDataPut(POKE_GRA_TEST *pgt,int mode);
static	void	PokeTestPokeBGColorGDataPut(POKE_GRA_TEST *pgt,int mode);
static	void	PokeTestPokeBGColorBDataPut(POKE_GRA_TEST *pgt,int mode);
static	void	PokeTestBGColorSet(POKE_GRA_TEST *pgt);

static	void	PokeTestNumPut(POKE_GRA_TEST *pgt,int x,int y,int param,int keta,int pos,int mode);
static	int		PokeTestNumCalc(POKE_GRA_TEST *pgt,int *param,int keta,int min,int max,int mode);
static	int		PokeTestPokeFormMaxGet(POKE_GRA_TEST *pgt);

static	void	PokeGraTestShadowSet(POKE_GRA_TEST *pgt,int height);

static void PDA_PageChageView( POKE_GRA_TEST* pgt, GF_PRINTCOLOR col );

static void	PAD_PageView(POKE_GRA_TEST* pgt,GF_PRINTCOLOR col);
static BOOL	PAD_PageMain(POKE_GRA_TEST* pgt);

static void	PAD_VoiceView(POKE_GRA_TEST* pgt,GF_PRINTCOLOR col);
static BOOL	PAD_VoiceMain(POKE_GRA_TEST* pgt);

static void	PAD_PG_Anm_View(POKE_GRA_TEST* pgt,GF_PRINTCOLOR col);
static BOOL	PAD_PG_Anm_Main(POKE_GRA_TEST* pgt);

static void	PAD_PG_AnmWait_View(POKE_GRA_TEST* pgt,GF_PRINTCOLOR col);
static BOOL	PAD_PG_AnmWait_Main(POKE_GRA_TEST* pgt);

static void	PAD_PAT_Anm_View(POKE_GRA_TEST* pgt, GF_PRINTCOLOR col, int no);
static BOOL	PAD_PAT_Anm_Main(POKE_GRA_TEST* pgt);

static void	PAD_PAT_AnmWait_View(POKE_GRA_TEST* pgt, GF_PRINTCOLOR col, int no);
static BOOL	PAD_PAT_AnmWait_Main(POKE_GRA_TEST* pgt);

static void	PAD_PAT_AnmPos_View(POKE_GRA_TEST* pgt, GF_PRINTCOLOR col, int no);
static BOOL	PAD_PAT_AnmPos_Main(POKE_GRA_TEST* pgt);

//==================================================================================================
//
//	データ
//
//==================================================================================================

static	const	u16	pal[]={
		GX_RGB(31,31,31),				//0:白
		GX_RGB(13,13,13),				//1:濃い灰
		GX_RGB(24,24,24),				//2:薄い灰
		GX_RGB(15,31,15),				//3:うすみどり
		GX_RGB(31,15,15),				//4:うすあか
		GX_RGB(19,19,19),				//5:灰
		GX_RGB( 0, 0, 0),
		GX_RGB( 0, 0, 0),
		GX_RGB( 0, 0, 0),
		GX_RGB( 0, 0, 0),
		GX_RGB( 0, 0, 0),
		GX_RGB( 0, 0, 0),
		GX_RGB( 0, 0, 0),
		GX_RGB( 0, 0, 0),
		GX_RGB( 0, 0, 0),
		GX_RGB(31,31,31),				//15:白
};

//ポケモンチェック
enum{
	POKE_NUM=0,
	POKE_SEX,
	POKE_COLOR,
	POKE_PAT,
	POKE_FORM,
	POKE_HEIGHT,
	POKE_OFX,
	POKE_SHADOW,
	POKE_BG_COLOR_R,
	POKE_BG_COLOR_G,
	POKE_BG_COLOR_B
};

//アニメーション設定
enum{
	MENU_ANM_FRONT=0,
	MENU_ANM_BACK1,
	MENU_ANM_BACK2,
	MENU_ANM_BACK3,
	MENU_ANM_FRONT_WAIT,
	MENU_ANM_BACK1_WAIT,
	MENU_ANM_BACK2_WAIT,
	MENU_ANM_BACK3_WAIT,
	MENU_PAT_01,
	MENU_PAT_02,
	MENU_PAT_03,
	MENU_PAT_04,
	MENU_PAT_05,
	MENU_PAT_06,
	MENU_PAT_07,
	MENU_PAT_08,
	MENU_PAT_09,
	MENU_PAT_10,
	MENU_WAIT_01,
	MENU_WAIT_02,
	MENU_WAIT_03,
	MENU_WAIT_04,
	MENU_WAIT_05,
	MENU_WAIT_06,
	MENU_WAIT_07,
	MENU_WAIT_08,
	MENU_WAIT_09,
	MENU_WAIT_10,
};

typedef BOOL (*PT_FUNC)(POKE_GRA_TEST *);
typedef void (*PT_VIEW_FUNC)(POKE_GRA_TEST *, GF_PRINTCOLOR );
typedef void (*PT_VIEW_FUNC_EX)(POKE_GRA_TEST *, GF_PRINTCOLOR, int );

typedef struct
{
	u8		cursor_pos_x;	//カーソルX位置
	u8		cursor_pos_y;	//カーソルY位置
	u8		move_up;		//上キーを押したときの移動先
	u8		move_down;		//下キーを押したときの移動先
	u8		move_left;		//左キーを押したときの移動先
	u8		move_right;		//右キーを押したときの移動先
	PT_FUNC	a_button_act;	//Aボタンを押したときのアクション
}MENU_LIST_PARAM;

#define	MONS_NUM_X	(0)		//モンスターナンバー表示X位置
#define	MONS_NUM_Y	(8)		//モンスターナンバー表示Y位置
#define	MONS_NAME_X	(32)	//モンスター名表示X位置
#define	MONS_NAME_Y	(8)		//モンスター名表示Y位置
#define	MONS_SEX_X	(96)	//モンスター性別表示X位置
#define	MONS_SEX_Y	(8)		//モンスター性別表示Y位置
#define	MONS_COL_X	(128)	//モンスターカラー表示X位置
#define	MONS_COL_Y	(8)		//モンスターカラー表示Y位置
#define	MONS_PAT_X	(188)	//モンスターアニメパターンナンバー表示X位置
#define	MONS_PAT_Y	(8)		//モンスターアニメパターンナンバー表示Y位置

#define	POKE_NUM_DATA_X			(12)
#define	POKE_NUM_DATA_Y			(0)
#define	POKE_NUM_X				(POKE_NUM_DATA_X-12)
#define	POKE_NUM_Y				(0)
#define	POKE_SEX_DATA_X			(108)
#define	POKE_SEX_DATA_Y			(0)
#define	POKE_SEX_X				(POKE_SEX_DATA_X-12)
#define	POKE_SEX_Y				(0)
#define	POKE_COLOR_DATA_X		(140)
#define	POKE_COLOR_DATA_Y		(0)
#define	POKE_COLOR_X			(POKE_COLOR_DATA_X-12)
#define	POKE_COLOR_Y			(0)
#define	POKE_PAT_DATA_X			(200)
#define	POKE_PAT_DATA_Y			(0)
#define	POKE_PAT_X				(POKE_PAT_DATA_X-12)
#define	POKE_PAT_Y				(0)
#define	POKE_FORM_DATA_X		(12)
#define	POKE_FORM_DATA_Y		(16)
#define	POKE_FORM_X				(POKE_FORM_DATA_X-12)
#define	POKE_FORM_Y				(16)
#define	POKE_HEIGHT_DATA_X		(60)
#define	POKE_HEIGHT_DATA_Y		(16)
#define	POKE_HEIGHT_X			(POKE_HEIGHT_DATA_X-12)
#define	POKE_HEIGHT_Y			(16)
#define	POKE_OFX_DATA_X			(88)
#define	POKE_OFX_DATA_Y			(16)
#define	POKE_OFX_X				(POKE_OFX_DATA_X-12)
#define	POKE_OFX_Y				(16)
#define	POKE_SHADOW_DATA_X		(124)
#define	POKE_SHADOW_DATA_Y		(16)
#define	POKE_SHADOW_X			(POKE_SHADOW_DATA_X-12)
#define	POKE_SHADOW_Y			(16)
#define	POKE_BG_COLOR_R_DATA_X	(144)
#define	POKE_BG_COLOR_R_DATA_Y	(16)
#define	POKE_BG_COLOR_R_X		(POKE_BG_COLOR_R_DATA_X-12)
#define	POKE_BG_COLOR_R_Y		(16)
#define	POKE_BG_COLOR_G_DATA_X	(172)
#define	POKE_BG_COLOR_G_DATA_Y	(16)
#define	POKE_BG_COLOR_G_X		(POKE_BG_COLOR_G_DATA_X-12)
#define	POKE_BG_COLOR_G_Y		(16)
#define	POKE_BG_COLOR_B_DATA_X	(200)
#define	POKE_BG_COLOR_B_DATA_Y	(16)
#define	POKE_BG_COLOR_B_X		(POKE_BG_COLOR_B_DATA_X-12)
#define	POKE_BG_COLOR_B_Y		(16)

static	const	MENU_LIST_PARAM	mlp_m[]={
	//POKE_NUM
	{
		POKE_NUM_X,
		POKE_NUM_Y,
		POKE_FORM,
		POKE_FORM,
		POKE_PAT,
		POKE_SEX,
		PokeNumAct,
	},
	//POKE_SEX
	{
		POKE_SEX_X,
		POKE_SEX_Y,
		POKE_OFX,
		POKE_OFX,
		POKE_NUM,
		POKE_COLOR,
		PokeSexAct,
	},
	//POKE_COLOR
	{
		POKE_COLOR_X,
		POKE_COLOR_Y,
		POKE_SHADOW,
		POKE_SHADOW,
		POKE_SEX,
		POKE_PAT,
		PokeColorAct,
	},
	//POKE_PAT
	{
		POKE_PAT_X,
		POKE_PAT_Y,
		POKE_BG_COLOR_R,
		POKE_BG_COLOR_R,
		POKE_COLOR,
		POKE_NUM,
		PokePatAct,
	},
	//POKE_FORM
	{
		POKE_FORM_X,
		POKE_FORM_Y,
		POKE_NUM,
		POKE_NUM,
		POKE_BG_COLOR_B,
		POKE_HEIGHT,
		PokeFormAct,
	},
	//POKE_HEIGHT
	{
		POKE_HEIGHT_X,
		POKE_HEIGHT_Y,
		POKE_SEX,
		POKE_SEX,
		POKE_FORM,
		POKE_OFX,
		PokeHeightAct,
	},
	//POKE_OFX
	{
		POKE_OFX_X,
		POKE_OFX_Y,
		POKE_SEX,
		POKE_SEX,
		POKE_HEIGHT,
		POKE_SHADOW,
		PokeOfxAct,
	},
	//POKE_SHADOW
	{
		POKE_SHADOW_X,
		POKE_SHADOW_Y,
		POKE_COLOR,
		POKE_COLOR,
		POKE_OFX,
		POKE_BG_COLOR_R,
		PokeShadowAct,
	},
	//POKE_BG_COLOR_R
	{
		POKE_BG_COLOR_R_X,
		POKE_BG_COLOR_R_Y,
		POKE_PAT,
		POKE_PAT,
		POKE_SHADOW,
		POKE_BG_COLOR_G,
		PokeBGColorRAct,
	},
	//POKE_BG_COLOR_G
	{
		POKE_BG_COLOR_G_X,
		POKE_BG_COLOR_G_Y,
		POKE_PAT,
		POKE_PAT,
		POKE_BG_COLOR_R,
		POKE_BG_COLOR_B,
		PokeBGColorGAct,
	},
	//POKE_BG_COLOR_B
	{
		POKE_BG_COLOR_B_X,
		POKE_BG_COLOR_B_Y,
		POKE_PAT,
		POKE_PAT,
		POKE_BG_COLOR_G,
		POKE_FORM,
		PokeBGColorBAct,
	},
};

#define	CUR_POS_X1		(4)
#define	CUR_POS_X2		(48)
#define	CUR_POS_X3		(96)
#define	CUR_POS_X4		(192)


#define	CUR_POS_X2_PATW	(64)

#define	CUR_POS_X2_PATP	(128-16)

#define	CUR_POS_Y1		(0)
#define	CUR_POS_Y2		(52)
#define	CUR_POS_OFS_Y1	(13)
#define	CUR_POS_OFS_Y2	(14)

#define CUR_POS_FIX( n )	( CUR_POS_Y1 + ( CUR_POS_OFS_Y2 * n ) )

#define	NO_MOVE			(0xff)

enum {
	
	ePAD_FB_CHANGE = 0,
	ePAD_VOICE_WAIT,
	
	ePAD_PG_ANIME,
	ePAD_PG_ANIME_WAIT,

	ePAD_EDIT_01,
	ePAD_EDIT_02,
	ePAD_EDIT_03,
	ePAD_EDIT_04,
	ePAD_EDIT_05,
	ePAD_EDIT_06,
	ePAD_EDIT_07,
	ePAD_EDIT_08,
	ePAD_EDIT_09,
	ePAD_EDIT_10,
	
	ePAD_EDIT_01_WAIT,
	ePAD_EDIT_02_WAIT,
	ePAD_EDIT_03_WAIT,
	ePAD_EDIT_04_WAIT,
	ePAD_EDIT_05_WAIT,
	ePAD_EDIT_06_WAIT,
	ePAD_EDIT_07_WAIT,
	ePAD_EDIT_08_WAIT,
	ePAD_EDIT_09_WAIT,
	ePAD_EDIT_10_WAIT,
	
	ePAD_EDIT_01_POS,
	ePAD_EDIT_02_POS,
	ePAD_EDIT_03_POS,
	ePAD_EDIT_04_POS,
	ePAD_EDIT_05_POS,
	ePAD_EDIT_06_POS,
	ePAD_EDIT_07_POS,
	ePAD_EDIT_08_POS,
	ePAD_EDIT_09_POS,
	ePAD_EDIT_10_POS,
};

static	const	MENU_LIST_PARAM	mlp[]={
	///< 前面 背面 切り替え
	{
		CUR_POS_X1,
		CUR_POS_FIX( 0 ),
		ePAD_EDIT_10,
		ePAD_VOICE_WAIT,
		ePAD_FB_CHANGE,
		ePAD_FB_CHANGE,
		PAD_PageMain,
	},
	///< 鳴き声ウェイト
	{
		CUR_POS_X1,
		CUR_POS_FIX( 1 ),
		ePAD_FB_CHANGE,
		ePAD_PG_ANIME,
		ePAD_VOICE_WAIT,
		ePAD_VOICE_WAIT,
		PAD_VoiceMain,			
	},
	///< プログラムアニメ
	{
		CUR_POS_X1,
		CUR_POS_FIX( 2 ),
		ePAD_VOICE_WAIT,
		ePAD_EDIT_01 + 0,
		ePAD_PG_ANIME_WAIT,
		ePAD_PG_ANIME_WAIT,
		PAD_PG_Anm_Main,			
	},
	///< プログラムアニメ wait
	{
		CUR_POS_X4,
		CUR_POS_FIX( 2 ),
		ePAD_VOICE_WAIT,
		ePAD_EDIT_01 + 0,
		ePAD_PG_ANIME,
		ePAD_PG_ANIME,
		PAD_PG_AnmWait_Main,			
	},

	///< パターンアニメ　１
	{
		CUR_POS_X1,				/// pos x
		CUR_POS_FIX( 3 ),		/// pos y
		ePAD_PG_ANIME,			/// ↑
		ePAD_EDIT_01 + 1,		/// ↓
		ePAD_EDIT_01_POS + 0,		/// ←
		ePAD_EDIT_01_WAIT + 0,		/// →
		PAD_PAT_Anm_Main,		/// [ A ]
	},
	///< パターンアニメ　２
	{
		CUR_POS_X1,
		CUR_POS_FIX( 4 ),
		ePAD_EDIT_01 + 0,
		ePAD_EDIT_01 + 2,
		ePAD_EDIT_01_POS + 1,
		ePAD_EDIT_01_WAIT + 1,
		PAD_PAT_Anm_Main,			
	},
	///< パターンアニメ　３
	{
		CUR_POS_X1,
		CUR_POS_FIX( 5 ),
		ePAD_EDIT_01 + 1,
		ePAD_EDIT_01 + 3,
		ePAD_EDIT_01_POS + 2,
		ePAD_EDIT_01_WAIT + 2,
		PAD_PAT_Anm_Main,			
	},
	///< パターンアニメ　４
	{
		CUR_POS_X1,
		CUR_POS_FIX( 6 ),
		ePAD_EDIT_01 + 2,
		ePAD_EDIT_01 + 4,
		ePAD_EDIT_01_POS + 3,
		ePAD_EDIT_01_WAIT + 3,
		PAD_PAT_Anm_Main,			
	},
	///< パターンアニメ　５
	{
		CUR_POS_X1,
		CUR_POS_FIX( 7 ),
		ePAD_EDIT_01 + 3,
		ePAD_EDIT_01 + 5,
		ePAD_EDIT_01_POS + 4,
		ePAD_EDIT_01_WAIT + 4,
		PAD_PAT_Anm_Main,			
	},
	///< パターンアニメ　６
	{
		CUR_POS_X1,
		CUR_POS_FIX( 8 ),
		ePAD_EDIT_01 + 4,
		ePAD_EDIT_01 + 6,
		ePAD_EDIT_01_POS + 5,
		ePAD_EDIT_01_WAIT + 5,
		PAD_PAT_Anm_Main,			
	},
	///< パターンアニメ　７
	{
		CUR_POS_X1,
		CUR_POS_FIX( 9 ),
		ePAD_EDIT_01 + 5,
		ePAD_EDIT_01 + 7,
		ePAD_EDIT_01_POS + 6,
		ePAD_EDIT_01_WAIT + 6,
		PAD_PAT_Anm_Main,			
	},
	///< パターンアニメ　８
	{
		CUR_POS_X1,
		CUR_POS_FIX( 10 ),
		ePAD_EDIT_01 + 6,
		ePAD_EDIT_01 + 8,
		ePAD_EDIT_01_POS + 7,
		ePAD_EDIT_01_WAIT + 7,
		PAD_PAT_Anm_Main,			
	},
	///< パターンアニメ　９
	{
		CUR_POS_X1,
		CUR_POS_FIX( 11 ),
		ePAD_EDIT_01 + 7,
		ePAD_EDIT_01 + 9,
		ePAD_EDIT_01_POS + 8,
		ePAD_EDIT_01_WAIT + 8,
		PAD_PAT_Anm_Main,			
	},
	///< パターンアニメ　１０
	{
		CUR_POS_X1,
		CUR_POS_FIX( 12 ),
		ePAD_EDIT_01 + 8,
		ePAD_FB_CHANGE,
		ePAD_EDIT_01_POS + 9,
		ePAD_EDIT_01_WAIT + 9,
		PAD_PAT_Anm_Main,			
	},

	///< パターンアニメ ウエイト　１
	{
		CUR_POS_X2_PATW,				/// pos x
		CUR_POS_FIX( 3 ),		/// pos y
		ePAD_PG_ANIME,			/// ↑
		ePAD_EDIT_01_WAIT + 1,	/// ↓
		ePAD_EDIT_01 + 0,		/// ←
		ePAD_EDIT_01_POS + 0,		/// →
		PAD_PAT_AnmWait_Main,		/// [ A ]
	},
	///< パターンアニメ ウエイト　２
	{
		CUR_POS_X2_PATW,
		CUR_POS_FIX( 4 ),
		ePAD_EDIT_01_WAIT + 0,
		ePAD_EDIT_01_WAIT + 2,
		ePAD_EDIT_01 + 1,
		ePAD_EDIT_01_POS + 1,
		PAD_PAT_AnmWait_Main,			
	},
	///< パターンアニメ ウエイト　３
	{
		CUR_POS_X2_PATW,
		CUR_POS_FIX( 5 ),
		ePAD_EDIT_01_WAIT + 1,
		ePAD_EDIT_01_WAIT + 3,
		ePAD_EDIT_01 + 2,
		ePAD_EDIT_01_POS + 2,
		PAD_PAT_AnmWait_Main,			
	},
	///< パターンアニメ ウエイト　４
	{
		CUR_POS_X2_PATW,
		CUR_POS_FIX( 6 ),
		ePAD_EDIT_01_WAIT + 2,
		ePAD_EDIT_01_WAIT + 4,
		ePAD_EDIT_01 + 3,
		ePAD_EDIT_01_POS + 3,
		PAD_PAT_AnmWait_Main,			
	},
	///< パターンアニメ ウエイト　５
	{
		CUR_POS_X2_PATW,
		CUR_POS_FIX( 7 ),
		ePAD_EDIT_01_WAIT + 3,
		ePAD_EDIT_01_WAIT + 5,
		ePAD_EDIT_01 + 4,
		ePAD_EDIT_01_POS + 4,
		PAD_PAT_AnmWait_Main,			
	},
	///< パターンアニメ ウエイト　６
	{
		CUR_POS_X2_PATW,
		CUR_POS_FIX( 8 ),
		ePAD_EDIT_01_WAIT + 4,
		ePAD_EDIT_01_WAIT + 6,
		ePAD_EDIT_01 + 5,
		ePAD_EDIT_01_POS + 5,
		PAD_PAT_AnmWait_Main,			
	},
	///< パターンアニメ ウエイト　７
	{
		CUR_POS_X2_PATW,
		CUR_POS_FIX( 9 ),
		ePAD_EDIT_01_WAIT + 5,
		ePAD_EDIT_01_WAIT + 7,
		ePAD_EDIT_01 + 6,
		ePAD_EDIT_01_POS + 6,
		PAD_PAT_AnmWait_Main,			
	},
	///< パターンアニメ ウエイト　８
	{
		CUR_POS_X2_PATW,
		CUR_POS_FIX( 10 ),
		ePAD_EDIT_01_WAIT + 6,
		ePAD_EDIT_01_WAIT + 8,
		ePAD_EDIT_01 + 7,
		ePAD_EDIT_01_POS + 7,
		PAD_PAT_AnmWait_Main,			
	},
	///< パターンアニメ ウエイト　９
	{
		CUR_POS_X2_PATW,
		CUR_POS_FIX( 11 ),
		ePAD_EDIT_01_WAIT + 7,
		ePAD_EDIT_01_WAIT + 9,
		ePAD_EDIT_01 + 8,
		ePAD_EDIT_01_POS + 8,
		PAD_PAT_AnmWait_Main,			
	},
	///< パターンアニメ ウエイト　１０
	{
		CUR_POS_X2_PATW,
		CUR_POS_FIX( 12 ),
		ePAD_EDIT_01_WAIT + 8,
		ePAD_FB_CHANGE,
		ePAD_EDIT_01 + 9,
		ePAD_EDIT_01_POS + 9,
		PAD_PAT_AnmWait_Main,			
	},

	///< パターンアニメ ポジション　１
	{
		CUR_POS_X2_PATP,				/// pos x
		CUR_POS_FIX( 3 ),		/// pos y
		ePAD_PG_ANIME,			/// ↑
		ePAD_EDIT_01_POS + 1,	/// ↓
		ePAD_EDIT_01_WAIT + 0,		/// ←
		ePAD_EDIT_01 + 0,		/// →
		PAD_PAT_AnmPos_Main,		/// [ A ]
	},
	///< パターンアニメ ポジション　２
	{
		CUR_POS_X2_PATP,
		CUR_POS_FIX( 4 ),
		ePAD_EDIT_01_POS + 0,
		ePAD_EDIT_01_POS + 2,
		ePAD_EDIT_01_WAIT + 1,
		ePAD_EDIT_01 + 1,
		PAD_PAT_AnmPos_Main,			
	},
	///< パターンアニメ ポジション　３
	{
		CUR_POS_X2_PATP,
		CUR_POS_FIX( 5 ),
		ePAD_EDIT_01_POS + 1,
		ePAD_EDIT_01_POS + 3,
		ePAD_EDIT_01_WAIT + 2,
		ePAD_EDIT_01 + 2,
		PAD_PAT_AnmPos_Main,			
	},
	///< パターンアニメ ポジション　４
	{
		CUR_POS_X2_PATP,
		CUR_POS_FIX( 6 ),
		ePAD_EDIT_01_POS + 2,
		ePAD_EDIT_01_POS + 4,
		ePAD_EDIT_01_WAIT + 3,
		ePAD_EDIT_01 + 3,
		PAD_PAT_AnmPos_Main,			
	},
	///< パターンアニメ ポジション　５
	{
		CUR_POS_X2_PATP,
		CUR_POS_FIX( 7 ),
		ePAD_EDIT_01_POS + 3,
		ePAD_EDIT_01_POS + 5,
		ePAD_EDIT_01_WAIT + 4,
		ePAD_EDIT_01 + 4,
		PAD_PAT_AnmPos_Main,			
	},
	///< パターンアニメ ポジション　６
	{
		CUR_POS_X2_PATP,
		CUR_POS_FIX( 8 ),
		ePAD_EDIT_01_POS + 4,
		ePAD_EDIT_01_POS + 6,
		ePAD_EDIT_01_WAIT + 5,
		ePAD_EDIT_01 + 5,
		PAD_PAT_AnmPos_Main,			
	},
	///< パターンアニメ ポジション　７
	{
		CUR_POS_X2_PATP,
		CUR_POS_FIX( 9 ),
		ePAD_EDIT_01_POS + 5,
		ePAD_EDIT_01_POS + 7,
		ePAD_EDIT_01_WAIT + 6,
		ePAD_EDIT_01 + 6,
		PAD_PAT_AnmPos_Main,			
	},
	///< パターンアニメ ポジション　８
	{
		CUR_POS_X2_PATP,
		CUR_POS_FIX( 10 ),
		ePAD_EDIT_01_POS + 6,
		ePAD_EDIT_01_POS + 8,
		ePAD_EDIT_01_WAIT + 7,
		ePAD_EDIT_01 + 7,
		PAD_PAT_AnmPos_Main,			
	},
	///< パターンアニメ ポジション　９
	{
		CUR_POS_X2_PATP,
		CUR_POS_FIX( 11 ),
		ePAD_EDIT_01_POS + 7,
		ePAD_EDIT_01_POS + 9,
		ePAD_EDIT_01_WAIT + 8,
		ePAD_EDIT_01 + 8,
		PAD_PAT_AnmPos_Main,			
	},
	///< パターンアニメ ポジション　１０
	{
		CUR_POS_X2_PATP,
		CUR_POS_FIX( 12 ),
		ePAD_EDIT_01_POS + 8,
		ePAD_FB_CHANGE,
		ePAD_EDIT_01_WAIT + 9,
		ePAD_EDIT_01 + 9,
		PAD_PAT_AnmPos_Main,			
	},
};

#define	NORMAL_LETTER	(1)
#define	NORMAL_SHADOW	(2)
#define	NORMAL_GROUND	(15)

#define	ACTIVE_LETTER	(3)
#define	ACTIVE_SHADOW	(2)
#define	ACTIVE_GROUND	(15)

#define	CURSOR_LETTER	(4)
#define	CURSOR_SHADOW	(2)
#define	CURSOR_GROUND	(15)

#define	NONE_LETTER		(5)
#define	NONE_SHADOW		(2)
#define	NONE_GROUND		(15)

#define	COL_NORMAL		(GF_PRINTCOLOR_MAKE(NORMAL_LETTER,NORMAL_SHADOW,NORMAL_GROUND))
#define	COL_ACTIVE		(GF_PRINTCOLOR_MAKE(ACTIVE_LETTER,ACTIVE_SHADOW,ACTIVE_GROUND))
#define	COL_CURSOR		(GF_PRINTCOLOR_MAKE(CURSOR_LETTER,CURSOR_SHADOW,CURSOR_GROUND))
#define	COL_NONE		(GF_PRINTCOLOR_MAKE(NONE_LETTER,NONE_SHADOW,NONE_GROUND))

///< ポケモンプログラムアニメデータ復元
static void RC_PPAD( u16 mons_no, u8* patno, u8* wait )
{
	POKE_ANM_TABLE_DP		pat;

	ArchiveDataLoadOfs(&pat, ARC_POKE_ANM_TBL, 0, mons_no*sizeof(POKE_ANM_TABLE_DP), sizeof(POKE_ANM_TABLE_DP));

	*patno = pat.poke_f.patno;
	*wait  = pat.poke_f.wait;
}


static void FrontAnime_DP_Recover( POKE_GRA_TEST *pgt, int no )
{
	int i, j;		
	s8	ox;
	s8	oy;
	u8	size;
	
	int index;
	
	POKE_ANM_DEBUG_PRINT( "\n ----- DP データの 読み込み開始 ----- \n" );
	POKE_ANM_DEBUG_PRINT( "\n");
	POKE_ANM_DEBUG_PRINT( " ----- DP データの 読み込み中･･･ ----- \n" );
	POKE_ANM_DEBUG_PRINT( "\n");

	//PGT_POKE_MAX
	for ( i = 0; i < 494; i++ ){
		
		if ( i != no ){ continue; }
		
		///< 適正なポケモンNoに配置する必要がある
		index = PokeGraNoTable[ i ];
		
		// POKE_ANM_DEBUG_PRINT( " i = %3d  index = %3d \n", i, index );
					
		pgt->paed[ index ].poke_anime[ 0 ].voice_wait	= 0;
		pgt->paed[ index ].poke_anime[ 0 ].prg_anm		= 0;
		pgt->paed[ index ].poke_anime[ 0 ].prg_anm_wait	= 0;
							
		for ( j = 0; j < SS_ANM_SEQ_MAX; j++ ){
			
			pgt->paed[ index ].poke_anime[ 0 ].ssanm[ j ].patno = -1;
			pgt->paed[ index ].poke_anime[ 0 ].ssanm[ j ].wait	= 0;
			pgt->paed[ index ].poke_anime[ 0 ].ssanm[ j ].ox	= 0;
			pgt->paed[ index ].poke_anime[ 0 ].ssanm[ j ].oy	= 0;
		}

		///< 影サイズ 影高さ 高さ情報
		ArchiveDataLoadOfs( &ox,	ARC_POKE_SHADOW_OFX, 0, i, 1);
		ArchiveDataLoadOfs( &oy,	ARC_POKE_YOFS,		 0, i, 1);
		ArchiveDataLoadOfs( &size,	ARC_POKE_SHADOW,	 0, i, 1);
		pgt->paed[ index ].height		= oy;
		pgt->paed[ index ].shadow_size	= size;
		pgt->paed[ index ].shadow_ofx	= ox;
		
		///< アニメデータ復元
		{
			SOFT_SPRITE_ANIME_DP ssa[10];
			
			PokeAnmDataSetDP( &ssa[0], i );
			
			for ( j = 0; j < SS_ANM_SEQ_MAX; j++ ){
				
				pgt->paed[ index ].poke_anime[ 0 ].ssanm[ j ].patno = ssa[ j ].patno;
				pgt->paed[ index ].poke_anime[ 0 ].ssanm[ j ].wait	= ssa[ j ].wait;
				pgt->paed[ index ].poke_anime[ 0 ].ssanm[ j ].ox	= 0;
				pgt->paed[ index ].poke_anime[ 0 ].ssanm[ j ].oy	= 0;
	
			}
		}
		
		///< プログラムアニメデータ復元
		{
			u8 patno;
			u8 wait;
			
			RC_PPAD( i, &patno, &wait );
			
			pgt->paed[ index ].poke_anime[ 0 ].prg_anm		= patno;
			pgt->paed[ index ].poke_anime[ 0 ].prg_anm_wait	= wait;
		}
	}
	
	POKE_ANM_DEBUG_PRINT( "\n ----- DP データの 読み込み終了 ----- \n" );
}

//==================================================================================================
//
//	関数
//
//==================================================================================================

static	void	PokeGraInit(POKE_GRA_TEST *pgt)
{
	int				i,j;

#ifdef SAVE_BANK_LOAD	
	pgt->save_bank = EDIT_BANK;
	pgt->load_bank = SAVE_BANK;
	OS_Printf( " load = SAVE_BANK  save = EDIT_BANK\n" );
	OS_Printf( " load = 0x%08x save = 0x%08x\n", pgt->load_bank, pgt->save_bank );
#else
	pgt->save_bank = EDIT_BANK;
	pgt->load_bank = EDIT_BANK;
	OS_Printf( " load = EDIT_BANK  save = EDIT_BANK\n" );
	OS_Printf( " load = 0x%08x save = 0x%08x\n", pgt->load_bank, pgt->save_bank );
#endif

	simple_3DBGInit(HEAPID_BATTLE);
	sys_HBlankIntrStop();

	MI_CpuClearFast(pgt,sizeof(POKE_GRA_TEST));

	pgt->man_msg=MSGMAN_Create(MSGMAN_TYPE_NORMAL,ARC_MSG,NARC_msg_poke_test_dat,HEAPID_BATTLE);
	pgt->man_pam=MSGMAN_Create(MSGMAN_TYPE_NORMAL,ARC_MSG,NARC_msg_poke_anime_dat,HEAPID_BATTLE);
	pgt->man_poke=MSGMAN_Create(MSGMAN_TYPE_NORMAL,ARC_MSG,NARC_msg_monsname_dat,HEAPID_BATTLE);
	pgt->wordset = WORDSET_Create( HEAPID_BATTLE );
	pgt->msg_buf=STRBUF_Create(0x100,HEAPID_BATTLE);
	pgt->msg_buf2=STRBUF_Create(0x100,HEAPID_BATTLE);

	pgt->put_req=0;
	pgt->anm_pat=0;
	pgt->form_no=0;

	pgt->end_flag = FALSE;
	pgt->bgl=GF_BGL_BglIniAlloc(HEAPID_BATTLE);
	pgt->win_m=GF_BGL_BmpWinAllocGet(HEAPID_BATTLE,1);
	pgt->win_s=GF_BGL_BmpWinAllocGet(HEAPID_BATTLE,1);

	PokeGraTestBGCreate(pgt,pgt->bgl);

	pgt->ssm=SoftSpriteInit(HEAPID_BATTLE);
	SoftSpritePaletteTransParamSet(pgt->ssm,0,SOFT_SPRITE_COL_SIZE);	//影の分も転送するようにサイズを変更
	pgt->pasp=PokeAnm_AllocMemory(HEAPID_BATTLE,2,0);

	pgt->mons_no=1;
	pgt->sex=PokeGraTestSexGet(pgt->mons_no);
	if(pgt->sex==3){
		pgt->sex=0;
	}
	pgt->main_pos=0;
	pgt->pos=0;
	pgt->seq_no=SEQ_POKEMON_CHECK_INIT;
	pgt->seq_no_temp=0;
	pgt->sub_seq_no=0;
	pgt->bg_flag=0;
	pgt->col_flag=0;
	pgt->bg_r=0;
	pgt->bg_g=0;
	pgt->bg_b=0;

	GF_BGL_BmpWinAdd(pgt->bgl,pgt->win_m,GF_BGL_FRAME1_M,
					 TALK_WIN_X,TALK_WIN_Y,TALK_WIN_SIZE_X,TALK_WIN_SIZE_Y,TALK_MSG_PAL_NO,TALK_MSG_START);
	GF_BGL_BmpWinDataFill(pgt->win_m,0xff);
	BmpTalkWinWrite(pgt->win_m,WINDOW_TRANS_ON,TALK_WIN_CGX_START,TALK_WIN_PAL_NO);
	GF_BGL_BmpWinAdd(pgt->bgl,pgt->win_s,GF_BGL_FRAME1_S,0x00,0x00,32,24,0x00,0x01);

	for(i = 0; i < PGT_POKE_MAX; i++){
		
		pgt->paed[ i ].poke_anime[ 0 ].voice_wait	= 0;
		pgt->paed[ i ].poke_anime[ 0 ].prg_anm		= 0;
		pgt->paed[ i ].poke_anime[ 0 ].prg_anm_wait	= 0;
		
		pgt->paed[ i ].poke_anime[ 1 ].voice_wait	= 0;
		pgt->paed[ i ].poke_anime[ 1 ].prg_anm		= 0;
		pgt->paed[ i ].poke_anime[ 1 ].prg_anm_wait	= 0;
		
		for ( j = 0; j < SS_ANM_SEQ_MAX; j++ ){
			
			pgt->paed[ i ].poke_anime[ 0 ].ssanm[ j ].patno = -1;
			pgt->paed[ i ].poke_anime[ 0 ].ssanm[ j ].wait	= 0;
			pgt->paed[ i ].poke_anime[ 0 ].ssanm[ j ].ox	= 0;
			pgt->paed[ i ].poke_anime[ 0 ].ssanm[ j ].oy	= 0;
			
			pgt->paed[ i ].poke_anime[ 1 ].ssanm[ j ].patno = -1;
			pgt->paed[ i ].poke_anime[ 1 ].ssanm[ j ].wait	= 0;
			pgt->paed[ i ].poke_anime[ 1 ].ssanm[ j ].ox	= 0;
			pgt->paed[ i ].poke_anime[ 1 ].ssanm[ j ].oy	= 0;
		}
				
		pgt->paed[ i ].height		= 0;
		pgt->paed[ i ].shadow_size	= 1;
		pgt->paed[ i ].shadow_ofx	= 0;
	}

	pgt->paed[ 0 ].height = 0x55;

	
	if ( sys.cont & PAD_BUTTON_START
	&&	 sys.cont & PAD_BUTTON_SELECT )
	{		
		s8	ox;
		s8	oy;
		u8	size;
		
		int index;
		
		POKE_ANM_DEBUG_PRINT( "\n ----- DP データの 読み込み開始 ----- \n" );
		POKE_ANM_DEBUG_PRINT( "\n");
		POKE_ANM_DEBUG_PRINT( " ----- DP データの 読み込み中･･･ ----- \n" );
		POKE_ANM_DEBUG_PRINT( "\n");

		//PGT_POKE_MAX
		for ( i = 0; i < 494; i++ ){
			
			///< 適正なポケモンNoに配置する必要がある
			index = PokeGraNoTable[ i ];
			
			// POKE_ANM_DEBUG_PRINT( " i = %3d  index = %3d \n", i, index );
						
			pgt->paed[ index ].poke_anime[ 0 ].voice_wait	= 0;
			pgt->paed[ index ].poke_anime[ 0 ].prg_anm		= 0;
			pgt->paed[ index ].poke_anime[ 0 ].prg_anm_wait	= 0;
			
			pgt->paed[ index ].poke_anime[ 1 ].voice_wait	= 0;
			pgt->paed[ index ].poke_anime[ 1 ].prg_anm		= 0;
			pgt->paed[ index ].poke_anime[ 1 ].prg_anm_wait	= 0;
						
			for ( j = 0; j < SS_ANM_SEQ_MAX; j++ ){
				
				pgt->paed[ index ].poke_anime[ 0 ].ssanm[ j ].patno = -1;
				pgt->paed[ index ].poke_anime[ 0 ].ssanm[ j ].wait	= 0;
				pgt->paed[ index ].poke_anime[ 0 ].ssanm[ j ].ox	= 0;
				pgt->paed[ index ].poke_anime[ 0 ].ssanm[ j ].oy	= 0;
				
				pgt->paed[ index ].poke_anime[ 1 ].ssanm[ j ].patno = -1;
				pgt->paed[ index ].poke_anime[ 1 ].ssanm[ j ].wait	= 0;
				pgt->paed[ index ].poke_anime[ 1 ].ssanm[ j ].ox	= 0;
				pgt->paed[ index ].poke_anime[ 1 ].ssanm[ j ].oy	= 0;
			}

			///< 影サイズ 影高さ 高さ情報
			ArchiveDataLoadOfs( &ox,	ARC_POKE_SHADOW_OFX, 0, i, 1);
			ArchiveDataLoadOfs( &oy,	ARC_POKE_YOFS,		 0, i, 1);
			ArchiveDataLoadOfs( &size,	ARC_POKE_SHADOW,	 0, i, 1);
			pgt->paed[ index ].height		= oy;
			pgt->paed[ index ].shadow_size	= size;
			pgt->paed[ index ].shadow_ofx	= ox;
			
			///< アニメデータ復元
			{
				SOFT_SPRITE_ANIME_DP ssa[10];
				
				PokeAnmDataSetDP( &ssa[0], i );
				
				for ( j = 0; j < SS_ANM_SEQ_MAX; j++ ){
					
					pgt->paed[ index ].poke_anime[ 0 ].ssanm[ j ].patno = ssa[ j ].patno;
					pgt->paed[ index ].poke_anime[ 0 ].ssanm[ j ].wait	= ssa[ j ].wait;
					pgt->paed[ index ].poke_anime[ 0 ].ssanm[ j ].ox	= 0;
					pgt->paed[ index ].poke_anime[ 0 ].ssanm[ j ].oy	= 0;
		
				}
			}
			
			///< プログラムアニメデータ復元
			{
				u8 patno;
				u8 wait;
				
				RC_PPAD( i, &patno, &wait );
				
				pgt->paed[ index ].poke_anime[ 0 ].prg_anm		= patno;
				pgt->paed[ index ].poke_anime[ 0 ].prg_anm_wait	= wait;
			}
		}
		
		POKE_ANM_DEBUG_PRINT( "\n ----- DP データの 読み込み終了 ----- \n" );
	}
#if 0	
	if ( !(sys.cont & ( PAD_BUTTON_R | PAD_BUTTON_L )) ){
		
		PMSVLD_Load(EDIT_BANK, &pgt->paed[0], POKE_ANM_SAVE_SIZE);
		
		if(pgt->paed[0].height!=0x55){
			for(i=0;i<PGT_POKE_MAX;i++){
				pgt->paed[i].height=0;
			}
			pgt->paed[0].height=0x55;
		}
		for(i=0;i<PGT_POKE_MAX;i++){
			if(pgt->paed[i].shadow_size>3){
				pgt->paed[i].shadow_size=1;
			}
		}
		if(pgt->paed[0].shadow_ofx!=0x55){
			for(i=0;i<PGT_POKE_MAX;i++){
				pgt->paed[i].shadow_ofx=0;
			}
			pgt->paed[0].shadow_ofx=0x55;
		}
		POKE_ANM_DEBUG_PRINT( "\n ----- データ自動ロード ----- \n" );
	}
#endif
	
//	PokeGraTestMSG_Print(pgt);

	PokeTestMainScreenCreate(pgt);
	
	CursorMove(pgt,SIDE_MAIN,pgt->main_pos,CUR_PUT);

	PokeGraTestSubScreenMake(pgt);
	Snd_Stop();

	POKE_ANM_DEBUG_PRINT( "\n --- save_ram : 0x%08x size : 0x%08x\n",&pgt->paed[ 0 ], POKE_ANM_SAVE_SIZE );
}

static	void	PokeTestMainScreenCreate(POKE_GRA_TEST *pgt)
{
	PokeTestPokeNumDataPut(pgt,MODE_NORMAL);
	PokeTestPokeSexDataPut(pgt);
	PokeTestPokeColorDataPut(pgt);
	PokeTestPokePatDataPut(pgt);
	PokeTestPokeFormDataPut(pgt,MODE_NORMAL);
	PokeTestPokeHeightDataPut(pgt,COL_NORMAL);
	PokeTestPokeOfxDataPut(pgt,COL_NORMAL);
	PokeTestPokeShadowDataPut(pgt,COL_NORMAL);
	PokeTestPokeBGColorRDataPut(pgt,MODE_NORMAL);
	PokeTestPokeBGColorGDataPut(pgt,MODE_NORMAL);
	PokeTestPokeBGColorBDataPut(pgt,MODE_NORMAL);
}

//============================================================================================
/**
 *	戦闘背景画面初期化＆画面生成
 */
//============================================================================================
static	void	PokeGraTestBGCreate(POKE_GRA_TEST *pgt,GF_BGL_INI *bgl)
{
	GF_Disp_GX_VisibleControlInit();

	//VRAM設定
	{
		GF_BGL_DISPVRAM vramSetTable = {
			GX_VRAM_BG_128_A,				// メイン2DエンジンのBG
			GX_VRAM_BGEXTPLTT_NONE,			// メイン2DエンジンのBG拡張パレット
			GX_VRAM_SUB_BG_32_H,			// サブ2DエンジンのBG
			GX_VRAM_SUB_BGEXTPLTT_NONE,		// サブ2DエンジンのBG拡張パレット
			GX_VRAM_OBJ_64_E,				// メイン2DエンジンのOBJ
			GX_VRAM_OBJEXTPLTT_NONE,		// メイン2DエンジンのOBJ拡張パレット
			GX_VRAM_SUB_OBJ_16_I,			// サブ2DエンジンのOBJ
			GX_VRAM_SUB_OBJEXTPLTT_NONE,	// サブ2DエンジンのOBJ拡張パレット
			GX_VRAM_TEX_01_BC,				// テクスチャイメージスロット
			GX_VRAM_TEXPLTT_01_FG			// テクスチャパレットスロット
		};
		GF_Disp_SetBank( &vramSetTable );
		MI_CpuClear32((void*)HW_BG_VRAM, HW_BG_VRAM_SIZE);
		MI_CpuClear32((void*)HW_DB_BG_VRAM, HW_DB_BG_VRAM_SIZE);
		MI_CpuClear32((void*)HW_OBJ_VRAM, HW_OBJ_VRAM_SIZE);
		MI_CpuClear32((void*)HW_DB_OBJ_VRAM, HW_DB_OBJ_VRAM_SIZE);
	}

	// BG SYSTEM
	{
		GF_BGL_SYS_HEADER BGsys_data = {
			GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_3D,
		};
		GF_BGL_InitBG( &BGsys_data );
	}

	//メイン画面フレーム設定
	{
		GF_BGL_BGCNT_HEADER TextBgCntDat[] = {
			///<FRAME1_M
			{
				0, 0, 0x0800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GX_BG_EXTPLTT_01,
				0, 0, 0, FALSE
			},
			///<FRAME2_M
			{
				0, 0, 0x2000, 0, GF_BGL_SCRSIZ_512x512, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x0c000, GX_BG_EXTPLTT_01,
				1, 0, 0, FALSE
			},
			///<FRAME3_M
			{
				0, 0, 0x1000, 0, GF_BGL_SCRSIZ_512x256, GX_BG_COLORMODE_256,
				GX_BG_SCRBASE_0x3000, GX_BG_CHARBASE_0x10000, GX_BG_EXTPLTT_01,
				3, 0, 0, FALSE
			},
		};
		GF_BGL_BGControlSet(bgl, GF_BGL_FRAME1_M, &TextBgCntDat[0], GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear(bgl, GF_BGL_FRAME1_M );
		GF_BGL_BGControlSet(bgl, GF_BGL_FRAME2_M, &TextBgCntDat[1], GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear(bgl, GF_BGL_FRAME2_M );
		GF_BGL_BGControlSet(bgl, GF_BGL_FRAME3_M, &TextBgCntDat[2], GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear(bgl, GF_BGL_FRAME3_M );

		G2_SetBG0Priority(0x01);
		GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
	}
	//サブ画面フレーム設定
	{
		GF_BGL_BGCNT_HEADER TextBgCntDat[]={
			///<FRAME1_S
			{
				0,0,0x800,0,GF_BGL_SCRSIZ_256x256,GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x7800,GX_BG_CHARBASE_0x00000,GX_BG_EXTPLTT_01,
				0,0,0,FALSE
			},
		};
		GF_BGL_BGControlSet(bgl, GF_BGL_FRAME1_S, &TextBgCntDat[0], GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear(bgl, GF_BGL_FRAME1_S );
	}

	//背景グラフィックデータロード
	{
		TalkWinGraphicNullSet(bgl, GF_BGL_FRAME1_M,TALK_WIN_CGX_START,TALK_WIN_PAL_NO,0,HEAPID_BATTLE);
		ArcUtil_BgCharSet(ARC_BATT_BG,BATTLE_BG00_NCGR_BIN,bgl,GF_BGL_FRAME3_M,0,0,1,HEAPID_BATTLE);
		ArcUtil_PalSet(ARC_BATT_BG,BATT_BG00_D_NCLR,PALTYPE_MAIN_BG,0,0,HEAPID_BATTLE);
		ArcUtil_ScrnSet(ARC_BATT_BG,BATTLE_BG00_NSCR_BIN,bgl,GF_BGL_FRAME3_M,0,0,1,HEAPID_BATTLE);
		ArcUtil_PalSet(ARC_WINFRAME,TalkWinPalArcGet(0),PALTYPE_MAIN_BG,TALK_WIN_PAL_NO*0x20,0x20,HEAPID_BATTLE);
//		ArcUtil_PalSet(ARC_FONT,NARC_font_talk_ncrl,PALTYPE_MAIN_BG,TALK_MSG_PAL_NO*0x20,0x20,HEAPID_BATTLE);
//
		DC_FlushRange((void *)&pal,sizeof(pal));
		GX_LoadBGPltt((void *)&pal,TALK_MSG_PAL_NO*0x20,sizeof(pal));

		DC_FlushRange((void *)&pal,sizeof(pal));
		GXS_LoadBGPltt((void *)&pal,0,sizeof(pal));
	}

	GF_Disp_DispOn();

	sys_VBlankFuncChange(PokeGraTestVBlank,pgt);
}

//============================================================================================
/**
 *	メインループタスク
 */
//============================================================================================
static	void	PokeGraTestMain(POKE_GRA_TEST * pgt)
{
	u8	sex;
	int	move=NO_MOVE;

	if(pgt->put_req){
		pgt->put_req=0;
		GF_BGL_BmpWinOn(pgt->win_m);
		GF_BGL_BmpWinOn(pgt->win_s);
	}

	if((sys.trg == PAD_BUTTON_Y)&&(pgt->seq_no<SEQ_LOAD)&&(pgt->seq_no!=SEQ_POKEMON_CHECK_EDIT)){
#ifdef GO_USE
		Snd_Stop();
#else
		pgt->seq_no_temp=pgt->seq_no;
		pgt->seq_no=SEQ_LOAD;
		pgt->sub_seq_no=0;
		pgt->yes_no_pos=1;
#endif
	}

	if((sys.trg == PAD_BUTTON_X)&&(pgt->seq_no<SEQ_LOAD)&&(pgt->seq_no!=SEQ_POKEMON_CHECK_EDIT)){
#ifdef GO_USE
		Snd_SceneSet(SND_SCENE_DUMMY);
		Snd_DataSetByScene( SND_SCENE_BATTLE, SEQ_FIGHT0101, 1 );	//バトル曲再生
#else
		pgt->seq_no_temp=pgt->seq_no;
		pgt->seq_no=SEQ_SAVE;
		pgt->sub_seq_no=0;
		pgt->yes_no_pos=1;
#endif
	}

	if((pgt->seq_no!=SEQ_POKEMON_ANIME)&&
	   (pgt->seq_no!=SEQ_POKEMON_ANIME_EDIT)){
		if(sys.trg == PAD_BUTTON_B
		&& pgt->is_main_page == TRUE){
			pgt->shadow_flag++;
			switch(pgt->shadow_flag%3){
			case 0:
				PokeTestPokeHeightDataPut(pgt,COL_NORMAL);
				PokeTestPokeOfxDataPut(pgt,COL_NORMAL);
				PokeTestPokeShadowDataPut(pgt,COL_NORMAL);
				break;
			case 1:
				SoftSpriteSSSystemFlagSet(pgt->ssm,SS_SYSTEM_FLAG_SHADOW_OFF);
				break;
			case 2:
				PokeTestPokeHeightDataPut(pgt,COL_NONE);
				PokeTestPokeOfxDataPut(pgt,COL_NONE);
				PokeTestPokeShadowDataPut(pgt,COL_NONE);
				SoftSpriteSSSystemFlagReset(pgt->ssm,SS_SYSTEM_FLAG_SHADOW_OFF);
				break;
			}
			PokeGraTestDebugPrint(pgt,VOICE_OFF);
		}
		if( sys.trg == PAD_BUTTON_L && pgt->is_anime == FALSE ){
#ifdef GO_USE
			if((pgt->seq_no!=SEQ_POKEMON_ANIME)&&
			   (pgt->seq_no!=SEQ_POKEMON_ANIME_ACT)&&
			   (pgt->seq_no!=SEQ_POKEMON_ANIME_ACT_B)&&
			   (pgt->seq_no!=SEQ_POKEMON_ANIME_ACT_F)&&
			   (pgt->seq_no!=SEQ_POKEMON_ANIME_EDIT)){
				PokePrgAnmDataSet(pgt->pasp,pgt->ss[0],pgt->mons_no,PARA_BACK,0,PARA_HFLIP_OFF,0);
				Snd_PMVoicePlayEx(PV_NORMAL,pgt->mons_no,-117,127,HEAPID_BASE_APP,pgt->form_no);
				pgt->seq_no_temp=pgt->seq_no;
				pgt->seq_no=SEQ_POKEMON_ANIME_ACT_B;
			}
#else
			pgt->bg_flag++;
			switch(pgt->bg_flag&3){
			case 0:
				GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_ON );
				GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );
				break;
			case 1:
				GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_ON );
				GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_OFF );
				break;
			case 2:
				GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_OFF );
				GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );
				break;
			case 3:
				GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_OFF );
				GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_OFF );
				break;
			}
#endif
		}
		if(sys.trg == PAD_BUTTON_R){
#ifdef GO_USE
			if((pgt->seq_no!=SEQ_POKEMON_ANIME)&&
			   (pgt->seq_no!=SEQ_POKEMON_ANIME_ACT)&&
			   (pgt->seq_no!=SEQ_POKEMON_ANIME_ACT_B)&&
			   (pgt->seq_no!=SEQ_POKEMON_ANIME_ACT_F)&&
			   (pgt->seq_no!=SEQ_POKEMON_ANIME_EDIT)){
				{
					SOFT_SPRITE_ANIME	ssa[10];
	
					PokePrgAnmDataSet(pgt->pasp,pgt->ss[1],pgt->mons_no,PARA_FRONT,0,PARA_HFLIP_OFF,1);
					Snd_PMVoicePlayEx(PV_NORMAL,pgt->mons_no,117,127,HEAPID_BASE_APP,pgt->form_no);
	
				//	PokeAnmDataSet(&ssa[0],pgt->mons_no);
					SoftSpriteAnimeDataSet(pgt->ss[1],&ssa[0]);
					SoftSpriteAnimeSet(pgt->ss[1],NULL);
					SoftSpriteParaSet(pgt->ss[1],SS_PARA_SHADOW_Y_REQ,0);
					SoftSpriteParaSet(pgt->ss[1],SS_PARA_SHADOW_AFF_REQ,0);
				}
				pgt->seq_no_temp=pgt->seq_no;
				pgt->seq_no=SEQ_POKEMON_ANIME_ACT_F;
			}
#else
			pgt->col_flag^=1;
			PokeTestBGColorSet(pgt);
#endif
		}
	}

	if(pgt->col_flag&1){
		PokeTestBGColorSet(pgt);
	}

	switch(pgt->seq_no){
	case SEQ_POKEMON_CHECK_INIT:
		{
			SOFT_SPRITE_ARC	ssa;
			int				height;

			PokeGraArcDataGet(&ssa,pgt->mons_no,pgt->sex,PARA_BACK,pgt->col,NULL,0x88888888);
			height=PokeHeightGet(pgt->mons_no,pgt->sex,PARA_BACK,NULL,NULL);
			pgt->ss[0]=SoftSpriteAdd(pgt->ssm,&ssa,APPEAR_X_TYPE_AA,APPEAR_Y_TYPE_AA+height,APPEAR_Z_TYPE_AA,
									 0,NULL,NULL);
			PokeGraArcDataGet(&ssa,pgt->mons_no,pgt->sex,PARA_FRONT,pgt->col,NULL,0x88888888);
			height=PokeHeightGet(pgt->mons_no,pgt->sex,PARA_FRONT,NULL,NULL);
			pgt->ss[1]=SoftSpriteAdd(pgt->ssm,&ssa,APPEAR_X_TYPE_BB,APPEAR_Y_TYPE_BB+height,APPEAR_Z_TYPE_BB,
									 1,NULL,NULL);
			SoftSpriteParaSet(pgt->ss[1],SS_PARA_SHADOW_REQ,1);
			SoftSpriteParaSet(pgt->ss[1],SS_PARA_SHADOW_SIZE,pgt->paed[PokeGraNoTable[pgt->mons_no]].shadow_size);
			SoftSpriteParaSet(pgt->ss[1],SS_PARA_SHADOW_OX,pgt->paed[PokeGraNoTable[pgt->mons_no]].shadow_ofx);
			SoftSpriteParaSet(pgt->ss[1],SS_PARA_SHADOW_OY,SHADOW_OFS-height);
		}
		pgt->seq_no=SEQ_POKEMON_CHECK;
		break;
	//ポケモンチェック
	case SEQ_POKEMON_CHECK:
		if(sys.trg & PAD_BUTTON_SELECT){
			pgt->is_main_page = FALSE;
			pgt->pos=0;
			pgt->seq_no=SEQ_POKEMON_ANIME;
			PokeGraTestSubScreenMake(pgt);
			CursorMove(pgt,SIDE_SUB,pgt->pos,CUR_PUT);
			break;
		}
		if(sys.trg==PAD_KEY_UP){
			move=mlp_m[pgt->main_pos].move_up;
		}
		else if(sys.trg == PAD_KEY_DOWN){
			move=mlp_m[pgt->main_pos].move_down;
		}
		else if(sys.trg == PAD_KEY_LEFT){
			move=mlp_m[pgt->main_pos].move_left;
		}
		else if(sys.trg == PAD_KEY_RIGHT){
			move=mlp_m[pgt->main_pos].move_right;
		}
		if(sys.trg == PAD_BUTTON_A){
			if(mlp_m[pgt->main_pos].a_button_act!=NULL){
				pgt->sub_seq_no=0;
				pgt->seq_no=SEQ_POKEMON_CHECK_EDIT;
			}
		}
		if(move!=NO_MOVE){
			CursorMove(pgt,SIDE_MAIN,move,CUR_PUT);
		}
		break;
	case SEQ_POKEMON_CHECK_EDIT:
		if(mlp_m[pgt->main_pos].a_button_act(pgt)==TRUE){
			pgt->seq_no=SEQ_POKEMON_CHECK;
		}
		break;
	//ポケモンアニメセット
	case SEQ_POKEMON_ANIME:
		if(sys.repeat == PAD_KEY_UP){
			move=mlp[pgt->pos].move_up;
		}
		else if(sys.repeat == PAD_KEY_DOWN){
			move=mlp[pgt->pos].move_down;
		}
		else if(sys.repeat == PAD_KEY_LEFT){
			move=mlp[pgt->pos].move_left;
		}
		else if(sys.repeat == PAD_KEY_RIGHT){
			move=mlp[pgt->pos].move_right;
		}
		
		if(sys.trg == PAD_BUTTON_SELECT){
			pgt->is_main_page = TRUE;
			CursorMove(pgt,SIDE_SUB,pgt->pos,CUR_DEL);
			pgt->seq_no=SEQ_POKEMON_CHECK;
		}
#if 0
		///< 今の場所の正面アニメをDPに戻す
		if ( sys.trg & PAD_KEY_LEFT ){
			if( sys.cont & PAD_BUTTON_B ){
				FrontAnime_DP_Recover( pgt, pgt->mons_no );
			}
		}
#endif			
		if ( sys.trg & PAD_BUTTON_START ){
			pgt->is_anime = TRUE;
						
			if( sys.cont & PAD_BUTTON_B )
			{
				POKE_ANIME_DATA pad;
				ARCHANDLE* handle;
				int dir;
				int client_type;
				u8	wait;
				int	num = pgt->sub_page ^ 1;
				SOFT_SPRITE_ANIME ssanm[10];
				
				if ( pgt->sub_page == 0 ){
					dir = PARA_FRONT;
					client_type = CLIENT_TYPE_BB;
				}
				else {
					dir = PARA_BACK;
					client_type = CLIENT_TYPE_AA;
				}
				
				handle = ArchiveDataHandleOpen( ARC_PM_EDIT, HEAPID_BASE_APP );
				
				PokeEditData_AnimeSet( handle, &ssanm[0], pgt->mons_no, client_type );
				SoftSpriteAnimeDataSet(pgt->ss[num], &ssanm[0]);
				SoftSpriteAnimeSet(pgt->ss[num], NULL);
				
				PokeEditData_PrgAnimeSet( handle, pgt->pasp, pgt->ss[num], pgt->mons_no, dir, PARA_HFLIP_OFF, client_type );
				
				#if 0
				{
					int no;
					
					for ( no = 0; no < 500; no++ ){
						PokeEditData_VoiceWaitSet( handle, &wait, no, client_type );
						
						OS_Printf( "no = %3d wait = %2d\n", no, wait );
					}
				}
				#endif
				
				SoftSpriteParaSet(pgt->ss[1], SS_PARA_SHADOW_Y_REQ, 0);
				SoftSpriteParaSet(pgt->ss[1], SS_PARA_SHADOW_AFF_REQ, 0);
				
				PokeEditData_VoiceWaitSet( handle, &wait, pgt->mons_no, client_type );
				
				ArchiveDataHandleClose( handle );
				
				Snd_PMVoiceWaitPlayEx(PV_NORMAL, pgt->mons_no, 117, 127, HEAPID_BASE_APP, wait, pgt->form_no );

				pgt->seq_no_temp=pgt->seq_no;
				pgt->seq_no=SEQ_POKEMON_ANIME_ACT_B+num;	
			}
			else {
				P_ANM_SETTING_PARAM	pas_p;
				int					num = 0;
				u8					wait;
				
				pas_p.AnimeNo	= pgt->paed[ PokeGraNoTable[pgt->mons_no] ].poke_anime[ pgt->sub_page ].prg_anm;
				pas_p.Wait		= pgt->paed[ PokeGraNoTable[pgt->mons_no] ].poke_anime[ pgt->sub_page ].prg_anm_wait;
				pas_p.Reverse	= 0;
				wait			= pgt->paed[ PokeGraNoTable[pgt->mons_no] ].poke_anime[ pgt->sub_page ].voice_wait;
				num				= pgt->sub_page ^ 1;
				SoftSpriteAnimeDataSet(pgt->ss[num], &pgt->paed[ PokeGraNoTable[pgt->mons_no] ].poke_anime[ pgt->sub_page ].ssanm[0]);
				SoftSpriteAnimeSet(pgt->ss[num], NULL);
				SoftSpriteParaSet(pgt->ss[1], SS_PARA_SHADOW_Y_REQ, 0);
				SoftSpriteParaSet(pgt->ss[1], SS_PARA_SHADOW_AFF_REQ, 0);
				
				PokeAnm_SetPokeAnime(pgt->pasp,pgt->ss[num],&pas_p,num);
				
				OS_Printf( " voice wait = %2d\n", wait );
				
				Snd_PMVoiceWaitPlayEx(PV_NORMAL, pgt->mons_no, 117, 127, HEAPID_BASE_APP, wait, pgt->form_no );

				pgt->seq_no_temp=pgt->seq_no;
				pgt->seq_no=SEQ_POKEMON_ANIME_ACT_B+num;
			}
		}
		if(sys.trg == PAD_BUTTON_A){
			if(mlp[pgt->pos].a_button_act!=NULL){
				pgt->sub_seq_no=0;
				pgt->seq_no=SEQ_POKEMON_ANIME_EDIT;
			}
		}
		
		if(move!=NO_MOVE){
			CursorMove(pgt,SIDE_SUB,move,CUR_PUT);
		}
		
		if(sys.repeat & PAD_BUTTON_L){
			if(pgt->mons_no==1){
				pgt->mons_no=MONSNO_END;
			}
			else{
				pgt->mons_no--;
			}
			pgt->sex=PokeGraTestSexGet(pgt->mons_no);
			if(pgt->sex==3){
				pgt->sex=0;
			}
			PokeGraTestDebugPrint(pgt,VOICE_ON);
			PokeTestMainScreenCreate(pgt);
			PokeGraTestSubScreenMake(pgt);
			CursorMove(pgt,SIDE_SUB,pgt->pos,CUR_PUT);
		}
		else if(sys.repeat & PAD_BUTTON_R){
			if(pgt->mons_no==MONSNO_END){
				pgt->mons_no=1;
			}
			else{
				pgt->mons_no++;
			}
			pgt->sex=PokeGraTestSexGet(pgt->mons_no);
			if(pgt->sex==3){
				pgt->sex=0;
			}
			PokeGraTestDebugPrint(pgt,VOICE_ON);
			PokeTestMainScreenCreate(pgt);
			PokeGraTestSubScreenMake(pgt);
			CursorMove(pgt,SIDE_SUB,pgt->pos,CUR_PUT);
		}
		break;
	case SEQ_POKEMON_ANIME_EDIT:
		if(mlp[pgt->pos].a_button_act(pgt)==TRUE){
			pgt->seq_no=SEQ_POKEMON_ANIME;
		}
		break;
	//ロード
	case SEQ_LOAD:
		if(PokeGraTestLoadAct(pgt)==TRUE){
			pgt->seq_no=pgt->seq_no_temp;
			if(pgt->seq_no){
				CursorMove(pgt,SIDE_SUB,pgt->pos,CUR_PUT);
			}
		}
		break;
	//セーブ
	case SEQ_SAVE:
		if(PokeGraTestSaveAct(pgt)==TRUE){
			pgt->seq_no=pgt->seq_no_temp;
			if(pgt->seq_no){
				CursorMove(pgt,SIDE_SUB,pgt->pos,CUR_PUT);
			}
		}
		break;
	//アニメ終了待ち
	case SEQ_POKEMON_ANIME_ACT:
		if((PokeAnm_IsFinished(pgt->pasp,0)==TRUE)&&
		   (PokeAnm_IsFinished(pgt->pasp,1)==TRUE)&&
		   (SoftSpriteAnimeEndCheck(pgt->ss[0])==FALSE)&&
		   (SoftSpriteAnimeEndCheck(pgt->ss[1])==FALSE)){
			SoftSpriteParaSet(pgt->ss[1],SS_PARA_SHADOW_Y_REQ,1);
			SoftSpriteParaSet(pgt->ss[1],SS_PARA_SHADOW_AFF_REQ,1);
			pgt->seq_no=pgt->seq_no_temp;
			pgt->is_anime = FALSE;
		}
		break;
	//アニメ終了待ち
	case SEQ_POKEMON_ANIME_ACT_B:
		if((PokeAnm_IsFinished(pgt->pasp,0)==TRUE)&&
		   (SoftSpriteAnimeEndCheck(pgt->ss[0])==FALSE)){
			pgt->seq_no=pgt->seq_no_temp;
			pgt->is_anime = FALSE;
		}
		break;
	//アニメ終了待ち
	case SEQ_POKEMON_ANIME_ACT_F:
		if((PokeAnm_IsFinished(pgt->pasp,1)==TRUE)&&
		   (SoftSpriteAnimeEndCheck(pgt->ss[1])==FALSE)){
			SoftSpriteParaSet(pgt->ss[1],SS_PARA_SHADOW_Y_REQ,1);
			SoftSpriteParaSet(pgt->ss[1],SS_PARA_SHADOW_AFF_REQ,1);
			pgt->seq_no=pgt->seq_no_temp;
			pgt->is_anime = FALSE;
		}
		break;
	}

	SoftSpriteMain(pgt->ssm);
	GF_G3_RequestSwapBuffers(GX_SORTMODE_MANUAL, GX_BUFFERMODE_Z);
}

//============================================================================================
/**
 *	メインループタスク
 */
//============================================================================================
static	void	PokeGraTestDebugPrint(POKE_GRA_TEST *pgt,int mode)
{
	int				height;
	SOFT_SPRITE_ARC	*ssa;

	ssa=SoftSpriteArcPointerGet(pgt->ssm,0);
	PokeGraArcDataGet(ssa,pgt->mons_no,pgt->sex,PARA_BACK,pgt->col,pgt->form_no,0x88888888);
	height=PokeHeightGet(pgt->mons_no,pgt->sex,PARA_BACK,pgt->form_no,NULL)+APPEAR_Y_TYPE_AA;
	SoftSpriteParaSet(pgt->ss[0],SS_PARA_POS_Y,height);
	SoftSpriteParaSet(pgt->ss[0],SS_PARA_ANM_PAT,pgt->anm_pat);

	ssa=SoftSpriteArcPointerGet(pgt->ssm,1);
	PokeGraArcDataGet(ssa,pgt->mons_no,pgt->sex,PARA_FRONT,pgt->col,pgt->form_no,0x88888888);
	height=PokeHeightGet(pgt->mons_no,pgt->sex,PARA_FRONT,pgt->form_no,NULL);
	SoftSpriteParaSet(pgt->ss[1],SS_PARA_POS_Y,APPEAR_Y_TYPE_BB+height);
	SoftSpriteParaSet(pgt->ss[1],SS_PARA_ANM_PAT,pgt->anm_pat);
	PokeGraTestShadowSet(pgt,height);

	SoftSpriteRecover(pgt->ssm);

	if(mode==VOICE_ON){
		Snd_PMVoicePlay(pgt->mons_no,pgt->form_no);
	}
}

static	void	PokeGraTestShadowSet(POKE_GRA_TEST *pgt,int height)
{
	s8		ox;
	s8		oy;
	u8		size;

	switch(pgt->shadow_flag%3){
	case 0:
	case 1:
		ox	=pgt->paed[PokeGraNoTable[pgt->mons_no]].shadow_ofx;
		oy	=pgt->paed[PokeGraNoTable[pgt->mons_no]].height;
		size=pgt->paed[PokeGraNoTable[pgt->mons_no]].shadow_size;
		break;
	case 2:
		{
			ARCHANDLE* handle;
			
			handle = ArchiveDataHandleOpen( ARC_PM_EDIT, HEAPID_BASE_APP );
			
			PokeEditData_HeightSet( handle, &oy, pgt->mons_no );
			PokeEditData_ShadowOfsXSet( handle, &ox, pgt->mons_no );
			PokeEditData_ShadowSizeSet( handle, &size, pgt->mons_no );
			
			ArchiveDataHandleClose( handle );
		}
	//	ArchiveDataLoadOfs(&ox,ARC_POKE_SHADOW_OFX,0,pgt->mons_no,1);
	//	ArchiveDataLoadOfs(&oy,ARC_POKE_YOFS,0,pgt->mons_no,1);
	//	ArchiveDataLoadOfs(&size,ARC_POKE_SHADOW,0,pgt->mons_no,1);
		break;
	}
	SoftSpriteParaSet(pgt->ss[1],SS_PARA_SHADOW_OY,SHADOW_OFS-height);
	SoftSpriteParaSet(pgt->ss[1],SS_PARA_SHADOW_OX,ox);
	SoftSpriteParaSet(pgt->ss[1],SS_PARA_SHADOW_HEIGHT,oy);
	SoftSpriteParaSet(pgt->ss[1],SS_PARA_SHADOW_SIZE,size);
}

//==============================================================================================
//
//	タイトルデバックメニューから呼ばれるように追加
//
//==============================================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
static PROC_RESULT PokeGraTestProc_Init(PROC * proc, int * seq)
{
	POKE_GRA_TEST	*pgt;
	sys_CreateHeap(HEAPID_BASE_APP,HEAPID_BATTLE,BATTLE_ALLOC_SIZE);
	pgt=PROC_AllocWork(proc,sizeof(POKE_GRA_TEST),HEAPID_BATTLE);
	PokeGraInit(pgt);
	return PROC_RES_FINISH;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static PROC_RESULT PokeGraTestProc_Main(PROC * proc, int * seq)
{
	POKE_GRA_TEST * pgt = PROC_GetWork(proc);
	PokeGraTestMain(pgt);
	if (pgt->end_flag) {
		return PROC_RES_FINISH;
	} else {
		return PROC_RES_CONTINUE;
	}
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static PROC_RESULT PokeGraTestProc_End(PROC * proc, int * seq)
{
	PROC_FreeWork(proc);
	sys_DeleteHeap(HEAPID_BATTLE);
	Main_SetNextProc(NO_OVERLAY_ID, &TitleProcData);
	//sys_MainProcChange( TitleMainProc );				//復帰先
	//ソフトリセット
	//詳細はソース先頭の「サウンドテストの例外処理について」を参照して下さい
	//OS_InitReset();
	OS_ResetSystem(0);									//ソフトリセット
	return PROC_RES_FINISH;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
const PROC_DATA PokeAnimeEditorProcData = {
	PokeGraTestProc_Init,
	PokeGraTestProc_Main,
	PokeGraTestProc_End,
	NO_OVERLAY_ID
};

//--------------------------------------------------------------
/**
 * @brief	VBLANK関数
 *
 * @param	work	VBLankには引数が必要なので定義してあるが実際にはNULLが入っているので、アクセスは禁止！
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static	void	PokeGraTestVBlank(void *work)
{
	POKE_GRA_TEST	*pgt=(POKE_GRA_TEST *)work;

	G2_BlendNone();
	G2S_BlendNone();

	SoftSpriteTextureTrans(pgt->ssm);

	GF_BGL_VBlankFunc(pgt->bgl);

	OS_SetIrqCheckFlag( OS_IE_V_BLANK);
}

//--------------------------------------------------------------
/**
 * @brief	3D初期化
 *
 * @retval	none	
 */
//--------------------------------------------------------------
static void Init3DStuff_(void)
{
    G3X_Init();                                            // initialize the 3D graphics states
    G3X_InitMtxStack();                                    // initialize the matrix stack

    G2_SetBG0Priority(1);

    G3X_SetShading(GX_SHADING_TOON);
    G3X_AntiAlias(TRUE);
    G3X_AlphaBlend(TRUE);

    // 3D 面は特殊効果OFFでも他のBG面とαブレンディングが行われますが、
    // そのためにはαブレンディング対象面の指定を行う必要があります。
    // SDK には対象面のみを設定する API が存在しないので、代わりに G2_SetBlendAlpha を使います。
    // この場合 G2_SetBlendAlpha の後ろ2つの引数は無視されます。
    G2_SetBlendAlpha(GX_BLEND_PLANEMASK_BG0, GX_BLEND_PLANEMASK_BD, 0, 0);

    // clear color
    G3X_SetClearColor(GX_RGB(0, 0, 0), 0, 0x7fff, 0, FALSE );

    G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_Z);
}

//--------------------------------------------------------------
/**
 * @brief	ポケモン性別ゲット
 *
 * @retval	none	
 */
//--------------------------------------------------------------
static	u8	PokeGraTestSexGet(u16 monsno)
{
	u8	sex;

	sex=PokePersonalParaGet(monsno,ID_PER_sex);
	switch(sex){
		case	MONS_MALE:
			return	PARA_MALE;
		case	MONS_FEMALE:
			return	PARA_FEMALE;
		case	MONS_UNKNOWN:
			return	PARA_UNK;
	}
	return	3;
}

//--------------------------------------------------------------
//	メッセージ表示位置定義
//--------------------------------------------------------------
#define	ANM_FRONT_NUM_X		(16)	//アニメーションナンバー
#define	ANM_FRONT_X			(48)	//アニメーションナンバー
#define	ANM_FRONT_WAIT_X	(208)	//アニメーションナンバー
#define	ANM_FRONT_Y			(0)		//アニメーションナンバー

#define	ANM_BACK_NUM_X		(16)	//アニメーションナンバー
#define	ANM_BACK_X			(48)	//アニメーションナンバー
#define	ANM_BACK_WAIT_X		(208)	//アニメーションナンバー
#define	ANM_BACK_Y			(13)	//アニメーションナンバー
#define	ANM_BACK_OFS_Y		(13)	//アニメーションナンバー

#define	ANM_NO_X	(16)	//アニメーションナンバー
#define	ANM_NO_Y	(52)	//アニメーションナンバー
#define	PAT_NO_X	(64)	//Patナンバー
#define	PAT_NO_Y	(52)	//Patナンバー
#define	WAIT_X		(112)	//アニメーションWait
#define	WAIT_Y		(52)	//アニメーションWait

#define	ANM_NO_OFS_Y	(14)	//アニメーションナンバー

#define	SVLD_X		(160 - 80)
#define	SVLD_Y		( 80 - 16)

#define	YES_NO_X	(160 - 80)
#define	YES_NO_Y	( 96 - 16)

#define	YES_NO_CUR_X	(144 - 80)
#define	YES_NO_CUR_Y	( 96 - 16)

static void	PAD_PageView(POKE_GRA_TEST* pgt, GF_PRINTCOLOR col)
{
	int page;
	
	page = pgt->sub_page;

	GF_BGL_BmpWinFill( pgt->win_s,0x0f, 16, 0, 144, 14 );
	
	MSGMAN_GetString( pgt->man_msg, POKE_TEST_025 + page, pgt->msg_buf );

	GF_STR_PrintColor(pgt->win_s, FONT_SYSTEM,pgt->msg_buf, 16, 0, MSG_NO_PUT, col, NULL);

	pgt->put_req = 1;
}

static BOOL	PAD_PageMain(POKE_GRA_TEST* pgt)
{
	if ( pgt->sub_seq_no == 0 ){
		
		pgt->sub_seq_no++;
		
		PAD_PageView( pgt, COL_ACTIVE );
	}
	
	if ( sys.repeat & PAD_KEY_UP
	||	 sys.repeat & PAD_KEY_DOWN
	||	 sys.repeat & PAD_KEY_LEFT
	||	 sys.repeat & PAD_KEY_RIGHT ){
	
		pgt->sub_page ^= 1;
	}
	
	if( sys.repeat ){
		
		PDA_PageChageView( pgt, COL_ACTIVE );
	}
	
	if( sys.trg & PAD_BUTTON_A ){
		
		PAD_PageView( pgt, COL_NORMAL );
		
		return TRUE;
	}
	
	return FALSE;
}


static void	PAD_VoiceView(POKE_GRA_TEST* pgt, GF_PRINTCOLOR col)
{
	u8	vo_wait;
	int grano = PokeGraNoTable[ pgt->mons_no ];

	vo_wait = pgt->paed[ grano ].poke_anime[ pgt->sub_page ].voice_wait;

	GF_BGL_BmpWinFill(pgt->win_s, 0x0f, 16, 14, 238, 14);

	STRBUF_SetNumber(pgt->msg_buf, vo_wait, 2, NUMBER_DISPTYPE_ZERO,NUMBER_CODETYPE_DEFAULT);
	GF_STR_PrintColor(pgt->win_s, FONT_SYSTEM, pgt->msg_buf, 208, 14, MSG_NO_PUT, col, NULL);
	
	MSGMAN_GetString(pgt->man_msg, POKE_TEST_024, pgt->msg_buf);	
	GF_STR_PrintColor(pgt->win_s, FONT_SYSTEM, pgt->msg_buf, 16, 14, MSG_NO_PUT, col, NULL);

	pgt->put_req=1;
}

static BOOL PAD_Calc_u8(u8* dat, int add, int skp, int min, int max, PT_VIEW_FUNC view_func, POKE_GRA_TEST* pgt)
{
	if ( pgt->sub_seq_no == 0 ){
		
		pgt->sub_seq_no++;
		
		view_func( pgt, COL_ACTIVE );
	}
	
	if ( sys.repeat & PAD_KEY_UP ){
		
		if ( *dat + add < max ){
			
			*dat += add;	
		}
		else {
			
			*dat = max;
		}
	}
	
	if ( sys.repeat & PAD_KEY_DOWN ){
		
		if ( *dat - add > min ){
			
			*dat -= add;
		}
		else {
			
			*dat = min;
		}
	}
	
	if ( sys.repeat & PAD_KEY_RIGHT ){
		
		if ( *dat + skp < max ){
			
			*dat += skp;	
		}
		else {
			
			*dat = max;
		}
	}
	
	if ( sys.repeat & PAD_KEY_LEFT ){
		
		if ( *dat - skp > min ){
			
			*dat -= skp;
		}
		else {
			
			*dat = min;
		}
	}
		
	if( sys.repeat ){
		
		view_func( pgt, COL_ACTIVE );
	}
	
	if( sys.trg & PAD_BUTTON_A ){
		
		view_func( pgt, COL_NORMAL );
		
		return TRUE;
	}
	
	return FALSE;
}


static BOOL PAD_Calc_u8_ex(u8* dat, int add, int skp, int min, int max, PT_VIEW_FUNC_EX view_func, POKE_GRA_TEST* pgt, int no)
{
	if ( pgt->sub_seq_no == 0 ){
		
		pgt->sub_seq_no++;
		
		view_func( pgt, COL_ACTIVE, no );
	}
	
	if ( sys.repeat & PAD_KEY_UP ){
		
		if ( *dat + add < max ){
			
			*dat += add;	
		}
		else {
			
			*dat = max;
		}
	}
	
	if ( sys.repeat & PAD_KEY_DOWN ){
		
		if ( *dat - add > min ){
			
			*dat -= add;
		}
		else {
			
			*dat = min;
		}
	}
	
	if ( sys.repeat & PAD_KEY_RIGHT ){
		
		if ( *dat + skp < max ){
			
			*dat += skp;	
		}
		else {
			
			*dat = max;
		}
	}
	
	if ( sys.repeat & PAD_KEY_LEFT ){
		
		if ( *dat - skp > min ){
			
			*dat -= skp;
		}
		else {
			
			*dat = min;
		}
	}
		
	if( sys.repeat ){
		
		view_func( pgt, COL_ACTIVE, no );
	}
	
	if( sys.trg & PAD_BUTTON_A ){
		
		view_func( pgt, COL_NORMAL, no );
		
		return TRUE;
	}
	
	return FALSE;
}


static BOOL PAD_Calc_s8(s8* dat, int add, int skp, int min, int max, PT_VIEW_FUNC view_func, POKE_GRA_TEST* pgt)
{
	if ( pgt->sub_seq_no == 0 ){
		
		pgt->sub_seq_no++;
		
		view_func( pgt, COL_ACTIVE );
	}
	
	if ( sys.repeat & PAD_KEY_UP ){
		
		if ( *dat + add < max ){
			
			*dat += add;	
		}
		else {
			
			*dat = max;
		}
	}
	
	if ( sys.repeat & PAD_KEY_DOWN ){
		
		if ( *dat - add > min ){
			
			*dat -= add;
		}
		else {
			
			*dat = min;
		}
	}
	
	if ( sys.repeat & PAD_KEY_RIGHT ){
		
		if ( *dat + skp < max ){
			
			*dat += skp;	
		}
		else {
			
			*dat = max;
		}
	}
	
	if ( sys.repeat & PAD_KEY_LEFT ){
		
		if ( *dat - skp > min ){
			
			*dat -= skp;
		}
		else {
			
			*dat = min;
		}
	}
		
	if( sys.repeat ){
		
		view_func( pgt, COL_ACTIVE );
	}
	
	if( sys.trg & PAD_BUTTON_A ){
		
		view_func( pgt, COL_NORMAL );
		
		return TRUE;
	}
	
	return FALSE;
}


static BOOL PAD_Calc_s8_ex(s8* dat, int add, int skp, int min, int max, PT_VIEW_FUNC_EX view_func, POKE_GRA_TEST* pgt, int no)
{
	if ( pgt->sub_seq_no == 0 ){
		
		pgt->sub_seq_no++;
		
		view_func( pgt, COL_ACTIVE, no );
	}
	
	if ( sys.repeat & PAD_KEY_UP ){
		
		if ( *dat + add < max ){
			
			*dat += add;	
		}
		else {
			
			*dat = max;
		}
	}
	
	if ( sys.repeat & PAD_KEY_DOWN ){
		
		if ( *dat - add > min ){
			
			*dat -= add;
		}
		else {
			
			*dat = min;
		}
	}
	
	if ( sys.repeat & PAD_KEY_RIGHT ){
		
		if ( *dat + skp < max ){
			
			*dat += skp;	
		}
		else {
			
			*dat = max;
		}
	}
	
	if ( sys.repeat & PAD_KEY_LEFT ){
		
		if ( *dat - skp > min ){
			
			*dat -= skp;
		}
		else {
			
			*dat = min;
		}
	}
		
	if( sys.repeat ){
		
		view_func( pgt, COL_ACTIVE, no );
	}
	
	if( sys.trg & PAD_BUTTON_A ){
		
		view_func( pgt, COL_NORMAL, no );
		
		return TRUE;
	}
	
	return FALSE;
}

static BOOL	PAD_VoiceMain(POKE_GRA_TEST* pgt)
{
	int add = 1;
	int skp = 3;
	int min = 0;
	int max = 15;
	PT_VIEW_FUNC view_func = PAD_VoiceView;
	int grano = PokeGraNoTable[ pgt->mons_no ];
	u8* dat = &pgt->paed[ grano ].poke_anime[ pgt->sub_page ].voice_wait;
	
	return PAD_Calc_u8(dat, add, skp, min, max, view_func, pgt);
}


static void	PAD_PG_Anm_View(POKE_GRA_TEST* pgt,GF_PRINTCOLOR col)
{
	int	grano;
	u8	anm_no;

	grano  = PokeGraNoTable[ pgt->mons_no ];
	anm_no = pgt->paed[ grano ].poke_anime[ pgt->sub_page ].prg_anm;

	GF_BGL_BmpWinFill(pgt->win_s, 0x0f, 16, 28, 160, 14);

	STRBUF_SetNumber(pgt->msg_buf, anm_no + 1, 3, NUMBER_DISPTYPE_ZERO, NUMBER_CODETYPE_DEFAULT);
	GF_STR_PrintColor(pgt->win_s, FONT_SYSTEM, pgt->msg_buf, 16, 28, MSG_NO_PUT, col, NULL);
	
	MSGMAN_GetString(pgt->man_pam, MSG_POKE_ANM001 + anm_no, pgt->msg_buf);
	GF_STR_PrintColor(pgt->win_s, FONT_SYSTEM, pgt->msg_buf, 48, 28, MSG_NO_PUT, col, NULL);

	pgt->put_req=1;
}

static BOOL	PAD_PG_Anm_Main(POKE_GRA_TEST* pgt)
{
	int add = 1;
	int skp = 10;
	int min = 0;
	int	max = POKE_ANIME_MAX-1;
	PT_VIEW_FUNC view_func = PAD_PG_Anm_View;
	int grano = PokeGraNoTable[ pgt->mons_no ];
	u8* dat  = &pgt->paed[ grano ].poke_anime[ pgt->sub_page ].prg_anm;
	
	return PAD_Calc_u8(dat, add, skp, min, max, view_func, pgt);
}


static void	PAD_PG_AnmWait_View(POKE_GRA_TEST* pgt,GF_PRINTCOLOR col)
{
	int	grano;
	u8	wait;

	grano  = PokeGraNoTable[ pgt->mons_no ];
	wait   = pgt->paed[ grano ].poke_anime[ pgt->sub_page ].prg_anm_wait;

	GF_BGL_BmpWinFill(pgt->win_s, 0x0f, 208, 28, 48, 14);

	STRBUF_SetNumber(pgt->msg_buf, wait, 2, NUMBER_DISPTYPE_ZERO, NUMBER_CODETYPE_DEFAULT);
	GF_STR_PrintColor(pgt->win_s, FONT_SYSTEM, pgt->msg_buf, 208, 28, MSG_NO_PUT, col, NULL);

	pgt->put_req=1;
}

static BOOL	PAD_PG_AnmWait_Main(POKE_GRA_TEST* pgt)
{
	int add = 1;
	int skp = 3;
	int min = 0;
	int	max = 15;
	PT_VIEW_FUNC view_func = PAD_PG_AnmWait_View;
	int grano = PokeGraNoTable[ pgt->mons_no ];
	u8* dat  = &pgt->paed[ grano ].poke_anime[ pgt->sub_page ].prg_anm_wait;
	
	return PAD_Calc_u8(dat, add, skp, min, max, view_func, pgt);
}

static void	PAD_PAT_Anm_View(POKE_GRA_TEST* pgt, GF_PRINTCOLOR col, int no)
{
	int	grano;
	s8	dat;

	grano = PokeGraNoTable[ pgt->mons_no ];
	dat	  = pgt->paed[ grano ].poke_anime[ pgt->sub_page ].ssanm[ no ].patno;

	GF_BGL_BmpWinFill(pgt->win_s, 0x0f, 48, 42 + (no * 14), 32, 14);
		
	if ( dat < 0 ){
		
		if ( dat == -1 ){
			
			MSGMAN_GetString(pgt->man_msg, POKE_TEST_007, pgt->msg_buf);
		}
		else {
			
			MSGMAN_GetString(pgt->man_msg,POKE_TEST_014 + ( (dat * -1) -2 ), pgt->msg_buf);
		}		
	}
	else {
		
		STRBUF_SetNumber(pgt->msg_buf, dat, 1, NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT);
	}
	GF_STR_PrintColor(pgt->win_s, FONT_SYSTEM, pgt->msg_buf, 48, 42 + (no * 14), MSG_NO_PUT, col, NULL);

	pgt->put_req=1;
}

static BOOL	PAD_PAT_Anm_Main(POKE_GRA_TEST* pgt)
{
	int add = 1;
	int skp = 1;
	int min = -10;
	int	max = 1;
	PT_VIEW_FUNC_EX view_func = PAD_PAT_Anm_View;
	int	no	= pgt->pos - ePAD_EDIT_01;
	int grano = PokeGraNoTable[ pgt->mons_no ];
	s8*	dat	  = &pgt->paed[ grano ].poke_anime[ pgt->sub_page ].ssanm[ no ].patno;
	
	return PAD_Calc_s8_ex(dat, add, skp, min, max, view_func, pgt, no);
}


static void	PAD_PAT_AnmWait_View(POKE_GRA_TEST* pgt, GF_PRINTCOLOR col, int no)
{
	int	grano;
	u8	wait;

	grano  = PokeGraNoTable[ pgt->mons_no ];
	wait   = pgt->paed[ grano ].poke_anime[ pgt->sub_page ].ssanm[ no ].wait;

	GF_BGL_BmpWinFill(pgt->win_s, 0x0f, 80, 42 + (no * 14), 32, 14);	

	STRBUF_SetNumber(pgt->msg_buf, wait, 2, NUMBER_DISPTYPE_ZERO, NUMBER_CODETYPE_DEFAULT);
	GF_STR_PrintColor(pgt->win_s, FONT_SYSTEM, pgt->msg_buf, 80, 42 + (no * 14), MSG_NO_PUT, col, NULL);

	pgt->put_req=1;
}


static BOOL	PAD_PAT_AnmWait_Main(POKE_GRA_TEST* pgt)
{
	int add = 1;
	int skp = 3;
	int min = 0;
	int	max = 60;
	PT_VIEW_FUNC_EX view_func = PAD_PAT_AnmWait_View;
	int grano = PokeGraNoTable[ pgt->mons_no ];
	int	no	= pgt->pos - ePAD_EDIT_01_WAIT;
	u8* dat  = &pgt->paed[ grano ].poke_anime[ pgt->sub_page ].ssanm[ no ].wait;

	return PAD_Calc_u8_ex(dat, add, skp, min, max, view_func, pgt, no);
}



static void	PAD_PAT_AnmPos_View(POKE_GRA_TEST* pgt, GF_PRINTCOLOR col, int no)
{
	int	grano;
	s8	x, y;

	grano  = PokeGraNoTable[ pgt->mons_no ];
	x	   = pgt->paed[ grano ].poke_anime[ pgt->sub_page ].ssanm[ no ].ox;
	y	   = pgt->paed[ grano ].poke_anime[ pgt->sub_page ].ssanm[ no ].oy;

	GF_BGL_BmpWinFill(pgt->win_s, 0x0f, 128, 42 + (no * 14), 128, 14);
	
	WORDSET_RegisterNumber( pgt->wordset, 4, x, 3, NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT );
	WORDSET_RegisterNumber( pgt->wordset, 5, y, 3, NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT );
	
	MSGMAN_GetString(pgt->man_msg, POKE_TEST_023, pgt->msg_buf);
	
	WORDSET_ExpandStr( pgt->wordset, pgt->msg_buf2, pgt->msg_buf );
	
	GF_STR_PrintColor(pgt->win_s, FONT_SYSTEM, pgt->msg_buf2, 128, 42 + (no * 14), MSG_NO_PUT, col, NULL);

	pgt->put_req=1;
}

static BOOL	PAD_PAT_AnmPos_Main(POKE_GRA_TEST* pgt)
{
	s8*	x;
	s8* y;
	
	int add = 1;
	int min = -32;
	int	max =  32;
	PT_VIEW_FUNC_EX view_func = PAD_PAT_AnmPos_View;
	int grano = PokeGraNoTable[ pgt->mons_no ];
	int	no	= pgt->pos - ePAD_EDIT_01_POS;

	x	   = &pgt->paed[ grano ].poke_anime[ pgt->sub_page ].ssanm[ no ].ox;
	y	   = &pgt->paed[ grano ].poke_anime[ pgt->sub_page ].ssanm[ no ].oy;
	
	if ( pgt->sub_seq_no == 0 ){
		
		pgt->sub_seq_no++;
		
		view_func( pgt, COL_ACTIVE, no );
	}
	
	if ( sys.repeat & PAD_KEY_DOWN ){
		
		if ( *y + add < max ){
			
			*y += add;	
		}
		else {
			
			*y = max;
		}
	}
	
	if ( sys.repeat & PAD_KEY_UP ){
		
		if ( *y - add > min ){
			
			*y -= add;
		}
		else {
			
			*y = min;
		}
	}
	
	if ( sys.repeat & PAD_KEY_RIGHT ){
		
		if ( *x + add < max ){
			
			*x += add;	
		}
		else {
			
			*x = max;
		}
	}
	
	if ( sys.repeat & PAD_KEY_LEFT ){
		
		if ( *x - add > min ){
			
			*x -= add;
		}
		else {
			
			*x = min;
		}
	}
		
	if( sys.repeat ){
		
		view_func( pgt, COL_ACTIVE, no );
	}
	
	if( sys.trg & PAD_BUTTON_A ){
		
		view_func( pgt, COL_NORMAL, no );
		
		return TRUE;
	}
	
	return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief	サブスクリーン表示
 *
 * @retval	none	
 */
//--------------------------------------------------------------
static	void	PokeGraTestSubScreenMake(POKE_GRA_TEST *pgt)
{
	int		i;	

	GF_BGL_BmpWinDataFill(pgt->win_s,0xff);
	
	PAD_PageView( pgt, COL_NORMAL );
	PAD_VoiceView( pgt, COL_NORMAL );
	PAD_PG_Anm_View( pgt, COL_NORMAL );
	PAD_PG_AnmWait_View( pgt, COL_NORMAL );
	
	for ( i = 0; i < SS_ANM_SEQ_MAX; i++ ){
			
		STRBUF_SetNumber(pgt->msg_buf, i + 1, 2, NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT);
		GF_STR_PrintSimple(pgt->win_s, FONT_SYSTEM, pgt->msg_buf, 24, 42 + (i * 14), MSG_NO_PUT, NULL);

		PAD_PAT_Anm_View( pgt, COL_NORMAL, i );
		PAD_PAT_AnmWait_View( pgt, COL_NORMAL, i );
		PAD_PAT_AnmPos_View( pgt, COL_NORMAL, i );
	}

	pgt->put_req=1;
}

static void PDA_PageChageView( POKE_GRA_TEST* pgt, GF_PRINTCOLOR col )
{
	int i;
	
	PAD_PageView( pgt, col );
	PAD_VoiceView( pgt, COL_NORMAL );
	PAD_PG_Anm_View( pgt, COL_NORMAL );
	PAD_PG_AnmWait_View( pgt, COL_NORMAL );

	for ( i = 0; i < SS_ANM_SEQ_MAX; i++ ){
		
		PAD_PAT_Anm_View( pgt, COL_NORMAL, i );
		PAD_PAT_AnmWait_View( pgt, COL_NORMAL, i );
		PAD_PAT_AnmPos_View( pgt, COL_NORMAL, i );
	}

	pgt->put_req=1;
}


//--------------------------------------------------------------
/**
 * @brief	サブスクリーンデータ表示
 *
 * @retval	none	
 */
//--------------------------------------------------------------
static	void	PokeTestPokeNumDataPut(POKE_GRA_TEST *pgt,int mode)
{
	GF_BGL_BmpWinFill(pgt->win_m,0x0f,POKE_NUM_DATA_X,POKE_NUM_DATA_Y,12*8,16);

	PokeTestNumPut(pgt,POKE_NUM_DATA_X,POKE_NUM_DATA_Y,pgt->mons_no,3,pgt->cur_pos_keta,mode);

	MSGMAN_GetString(pgt->man_poke,pgt->mons_no,pgt->msg_buf);
	GF_STR_PrintSimple(pgt->win_m,FONT_SYSTEM,pgt->msg_buf,POKE_NUM_DATA_X+32,POKE_NUM_DATA_Y,MSG_NO_PUT,NULL);

	pgt->put_req=1;
}

static	void	PokeTestPokeSexDataPut(POKE_GRA_TEST *pgt)
{
	GF_BGL_BmpWinFill(pgt->win_m,0x0f,POKE_SEX_DATA_X,POKE_SEX_DATA_Y,12*2,16);

	MSGMAN_GetString(pgt->man_msg,POKE_TEST_002+pgt->sex,pgt->msg_buf);
	GF_STR_PrintSimple(pgt->win_m,FONT_SYSTEM,pgt->msg_buf,POKE_SEX_DATA_X,POKE_SEX_DATA_Y,MSG_NO_PUT,NULL);

	pgt->put_req=1;
}

static	void	PokeTestPokeColorDataPut(POKE_GRA_TEST *pgt)
{
	GF_BGL_BmpWinFill(pgt->win_m,0x0f,POKE_COLOR_DATA_X,POKE_COLOR_DATA_Y,12*4,16);

	MSGMAN_GetString(pgt->man_msg,POKE_TEST_005+pgt->col,pgt->msg_buf);
	GF_STR_PrintSimple(pgt->win_m,FONT_SYSTEM,pgt->msg_buf,POKE_COLOR_DATA_X,POKE_COLOR_DATA_Y,MSG_NO_PUT,NULL);

	pgt->put_req=1;
}

static	void	PokeTestPokePatDataPut(POKE_GRA_TEST *pgt)
{
	GF_BGL_BmpWinFill(pgt->win_m,0x0f,POKE_PAT_DATA_X,POKE_PAT_DATA_Y,12*1,16);

	//アニメパターンナンバー
	STRBUF_SetNumber(pgt->msg_buf, pgt->anm_pat,1,NUMBER_DISPTYPE_SPACE,NUMBER_CODETYPE_DEFAULT);
	GF_STR_PrintSimple(pgt->win_m,FONT_SYSTEM,pgt->msg_buf,POKE_PAT_DATA_X,POKE_PAT_DATA_Y,MSG_NO_PUT,NULL);

	pgt->put_req=1;
}

static	void	PokeTestPokeFormDataPut(POKE_GRA_TEST *pgt,int mode)
{
	int	max;

	GF_BGL_BmpWinFill(pgt->win_m,0x0f,POKE_FORM_DATA_X,POKE_FORM_DATA_Y,12*4,16);

	PokeTestNumPut(pgt,POKE_FORM_DATA_X,POKE_FORM_DATA_Y,pgt->form_no,2,pgt->cur_pos_keta,mode);

	max=PokeTestPokeFormMaxGet(pgt);

	STRBUF_SetNumber(pgt->msg_buf,max,2,NUMBER_DISPTYPE_ZERO,NUMBER_CODETYPE_DEFAULT);
	GF_STR_PrintSimple(pgt->win_m,FONT_SYSTEM,pgt->msg_buf,POKE_FORM_DATA_X+20,POKE_FORM_DATA_Y,MSG_NO_PUT,NULL);

	pgt->put_req=1;
}

static	void	PokeTestPokeHeightDataPut(POKE_GRA_TEST *pgt,GF_PRINTCOLOR col)
{
	GF_BGL_BmpWinFill(pgt->win_m,0x0f,POKE_HEIGHT_DATA_X,POKE_HEIGHT_DATA_Y,12*2,16);

	STRBUF_SetNumber(pgt->msg_buf,pgt->paed[PokeGraNoTable[pgt->mons_no]].height,3,NUMBER_DISPTYPE_LEFT,NUMBER_CODETYPE_DEFAULT);
	GF_STR_PrintColor(pgt->win_m,FONT_SYSTEM,pgt->msg_buf,POKE_HEIGHT_DATA_X,POKE_HEIGHT_DATA_Y,MSG_NO_PUT,col,NULL);

	pgt->put_req=1;
}

static	void	PokeTestPokeOfxDataPut(POKE_GRA_TEST *pgt,GF_PRINTCOLOR col)
{
	GF_BGL_BmpWinFill(pgt->win_m,0x0f,POKE_OFX_DATA_X,POKE_OFX_DATA_Y,12*2,16);

	STRBUF_SetNumber(pgt->msg_buf,pgt->paed[PokeGraNoTable[pgt->mons_no]].shadow_ofx,3,NUMBER_DISPTYPE_LEFT,NUMBER_CODETYPE_DEFAULT);
	GF_STR_PrintColor(pgt->win_m,FONT_SYSTEM,pgt->msg_buf,POKE_OFX_DATA_X,POKE_OFX_DATA_Y,MSG_NO_PUT,col,NULL);

	pgt->put_req=1;
}

static	void	PokeTestPokeShadowDataPut(POKE_GRA_TEST *pgt,GF_PRINTCOLOR col)
{
	GF_BGL_BmpWinFill(pgt->win_m,0x0f,POKE_SHADOW_DATA_X,POKE_SHADOW_DATA_Y,12*1,16);

	STRBUF_SetNumber(pgt->msg_buf,pgt->paed[PokeGraNoTable[pgt->mons_no]].shadow_size,1,NUMBER_DISPTYPE_SPACE,NUMBER_CODETYPE_DEFAULT);
	GF_STR_PrintColor(pgt->win_m,FONT_SYSTEM,pgt->msg_buf,POKE_SHADOW_DATA_X,POKE_SHADOW_DATA_Y,MSG_NO_PUT,col,NULL);

	pgt->put_req=1;
}

static	void	PokeTestPokeBGColorRDataPut(POKE_GRA_TEST *pgt,int mode)
{
	GF_BGL_BmpWinFill(pgt->win_m,0x0f,POKE_BG_COLOR_R_DATA_X,POKE_BG_COLOR_R_DATA_Y,12*2,16);

	PokeTestNumPut(pgt,POKE_BG_COLOR_R_DATA_X,POKE_BG_COLOR_R_DATA_Y,pgt->bg_r,2,pgt->cur_pos_keta,mode);

	pgt->put_req=1;
}

static	void	PokeTestPokeBGColorGDataPut(POKE_GRA_TEST *pgt,int mode)
{
	GF_BGL_BmpWinFill(pgt->win_m,0x0f,POKE_BG_COLOR_G_DATA_X,POKE_BG_COLOR_G_DATA_Y,12*2,16);

	PokeTestNumPut(pgt,POKE_BG_COLOR_G_DATA_X,POKE_BG_COLOR_G_DATA_Y,pgt->bg_g,2,pgt->cur_pos_keta,mode);

	pgt->put_req=1;
}

static	void	PokeTestPokeBGColorBDataPut(POKE_GRA_TEST *pgt,int mode)
{
	GF_BGL_BmpWinFill(pgt->win_m,0x0f,POKE_BG_COLOR_B_DATA_X,POKE_BG_COLOR_B_DATA_Y,12*2,16);

	PokeTestNumPut(pgt,POKE_BG_COLOR_B_DATA_X,POKE_BG_COLOR_B_DATA_Y,pgt->bg_b,2,pgt->cur_pos_keta,mode);

	pgt->put_req=1;
}

static	void	PokeTestBGColorSet(POKE_GRA_TEST *pgt)
{
	u16	col;
	u16	src;
	u8	evy;

	if(pgt->col_flag&1){
		pgt->col_flag+=0x100;
		if((pgt->col_flag&0x300)==0){
			if(pgt->col_flag&0x80){
				pgt->col_flag-=2;
				if((pgt->col_flag&0x0e)==0){
					pgt->col_flag^=0x80;
				}
			}
			else{
				pgt->col_flag+=2;
				if((pgt->col_flag&0x0e)==0x0e){
					pgt->col_flag^=0x80;
				}
			}
			evy=((pgt->col_flag&0x0e)>>1)+8;
			src=0;
			SoftFade(&src,&col,1,evy,GX_RGB(pgt->bg_r,pgt->bg_g,pgt->bg_b));
		}
		else{
			return;
		}
	}
	else{
		col=GX_RGB(pgt->bg_r,pgt->bg_g,pgt->bg_b);
	}

	DC_FlushRange((void *)&col,sizeof(col));
	GX_LoadBGPltt((void *)&col,0,sizeof(col));
}

static	void	PokeTestNumPut(POKE_GRA_TEST *pgt,int x,int y,int param,int keta,int pos,int mode)
{
	int		i;
	int		data,ofs;
	int		keta_tbl[10]={
		1000000000,
		100000000,
		10000000,
		1000000,
		100000,
		10000,
		1000,
		100,
		10,
		1,
	};

	ofs=0;

	for(i=0;i<keta;i++){
		data=param/keta_tbl[10-keta+i];
		param%=keta_tbl[10-keta+i];
		STRBUF_SetNumber(pgt->msg_buf,data,1,NUMBER_DISPTYPE_ZERO,NUMBER_CODETYPE_DEFAULT);
		if(mode==MODE_NORMAL){
			GF_STR_PrintColor(pgt->win_m,FONT_SYSTEM,pgt->msg_buf,x+ofs,y,MSG_NO_PUT,COL_NORMAL,NULL);
		}
		else{
			if((keta-1)-i==pos){
				GF_STR_PrintColor(pgt->win_m,FONT_SYSTEM,pgt->msg_buf,x+ofs,y,MSG_NO_PUT,COL_CURSOR,NULL);
			}
			else{
				GF_STR_PrintColor(pgt->win_m,FONT_SYSTEM,pgt->msg_buf,x+ofs,y,MSG_NO_PUT,COL_ACTIVE,NULL);
			}
		}
		ofs+=FontProc_GetPrintStrWidth(FONT_SYSTEM,pgt->msg_buf,0);
	}

	pgt->put_req=1;
}

static	int	PokeTestPokeFormMaxGet(POKE_GRA_TEST *pgt)
{
	int	ret;

	switch(pgt->mons_no){
	//ミノムッチ
	//ミノメス
	case MONSNO_MINOMUTTI:
	case MONSNO_MINOMESU:
		ret=FORMNO_MINOMUTTI_MAX;
		break;
	//シーウシ
	//シードルゴ
	case MONSNO_SIIUSI:
	case MONSNO_SIIDORUGO:
		ret=FORMNO_SII_MAX;
		break;
	//チェリシュ
	case MONSNO_THERISYU:
		ret=FORMNO_THERISYU_MAX;
		break;
	//アウス
	case MONSNO_AUSU:
		ret=POKE_WAZA_TYPE_MAX;
		break;
	//ポワルン
	case MONSNO_POWARUN:
		ret=FORMNO_POWARUN_MAX;
		break;
	//デオキシス
	case MONSNO_DEOKISISU:
		ret=FORMNO_DEOKISISU_MAX;
		break;
	//アンノーン
	case MONSNO_ANNOON:
		ret=UNK_QUE;
		break;
	// ロトム
	case MONSNO_PURAZUMA:
		ret=FORMNO_ROTOMU_MAX;		///<ロトムの別フォルムMAX
		break;
	// ギラティナ
	case MONSNO_KIMAIRAN:
		ret=FORMNO_GIRATINA_MAX;
		break;
	
	// しぇ意味
	case MONSNO_EURISU:
		ret=FORMNO_SHEIMI_MAX;
		break;
	default:
		ret=0;
		break;
	}
	return ret;
}

//==============================================================================================
//
//	メニューカーソル移動
//
//==============================================================================================
static	void	CursorMove(POKE_GRA_TEST *pgt,int side,int pos,int mode)
{
	switch(side){
	default:
	case SIDE_MAIN:
		GF_BGL_BmpWinFill(pgt->win_m,0x0f,mlp_m[pgt->main_pos].cursor_pos_x,mlp_m[pgt->main_pos].cursor_pos_y,12,16);
		if(mode==CUR_PUT){
			MSGMAN_GetString(pgt->man_msg,POKE_TEST_009,pgt->msg_buf);
			GF_STR_PrintSimple(pgt->win_m,FONT_SYSTEM,pgt->msg_buf,mlp_m[pos].cursor_pos_x,mlp_m[pos].cursor_pos_y,
							   MSG_NO_PUT,NULL);
			pgt->main_pos=pos;
		}
		break;
	case SIDE_SUB:
		GF_BGL_BmpWinFill(pgt->win_s,0x0f,mlp[pgt->pos].cursor_pos_x,mlp[pgt->pos].cursor_pos_y,12,16);
		if(mode==CUR_PUT){
			MSGMAN_GetString(pgt->man_msg,POKE_TEST_009,pgt->msg_buf);
			GF_STR_PrintSimple(pgt->win_s,FONT_SYSTEM,pgt->msg_buf,mlp[pos].cursor_pos_x,mlp[pos].cursor_pos_y,MSG_NO_PUT,NULL);
			pgt->pos=pos;
		}
		break;
	}
	pgt->put_req=1;
}

//==============================================================================================
//
//	メニューカーソル移動
//
//==============================================================================================
static	void	YesNoCursorMove(POKE_GRA_TEST *pgt,int pos,int mode)
{
	GF_BGL_BmpWinFill(pgt->win_s,0x0f,YES_NO_CUR_X,YES_NO_CUR_Y+ANM_NO_OFS_Y*pgt->yes_no_pos,12,16);
	if(mode==CUR_PUT){
		MSGMAN_GetString(pgt->man_msg,POKE_TEST_009,pgt->msg_buf);
		GF_STR_PrintSimple(pgt->win_s,FONT_SYSTEM,pgt->msg_buf,YES_NO_CUR_X,YES_NO_CUR_Y+ANM_NO_OFS_Y*pos,MSG_NO_PUT,NULL);
		pgt->yes_no_pos=pos;
	}
	pgt->put_req=1;
}

static	BOOL	PokeNumAct(POKE_GRA_TEST *pgt)
{
	BOOL	ret=FALSE;

	if(sys.trg==PAD_BUTTON_X){
		PokeColorAct(pgt);
	}
	if(sys.trg==PAD_BUTTON_Y){
		PokeSexAct(pgt);
	}

	switch(pgt->sub_seq_no){
	case 0:
		PokeTestPokeNumDataPut(pgt,MODE_ACTIVE);
		pgt->sub_seq_no++;
	case 1:
		switch(PokeTestNumCalc(pgt,&pgt->mons_no,3,MONSNO_HUSIGIDANE,MONSNO_END,MODE_LOOP)){
		case NUM_CALC_NONE:
			break;
		case NUM_CALC_CHANGE:
			pgt->sex=PokeGraTestSexGet(pgt->mons_no);
			if(pgt->sex==3){
				pgt->sex=0;
			}
			pgt->form_no=0;
			PokeTestPokeSexDataPut(pgt);
			PokeTestPokeFormDataPut(pgt,MODE_NORMAL);
			if((pgt->shadow_flag%3)==2){
				PokeTestPokeHeightDataPut(pgt,COL_NONE);
				PokeTestPokeOfxDataPut(pgt,COL_NONE);
				PokeTestPokeShadowDataPut(pgt,COL_NONE);
			}
			else{
				PokeTestPokeHeightDataPut(pgt,COL_NORMAL);
				PokeTestPokeOfxDataPut(pgt,COL_NORMAL);
				PokeTestPokeShadowDataPut(pgt,COL_NORMAL);
			}
			PokeGraTestDebugPrint(pgt,VOICE_ON);
		case NUM_CALC_MOVE:
			PokeTestPokeNumDataPut(pgt,MODE_ACTIVE);
			
			///< アニメデータ更新
			PokeGraTestSubScreenMake(pgt);
			break;
		case NUM_CALC_END:
			PokeTestPokeNumDataPut(pgt,MODE_NORMAL);
			ret=TRUE;
			break;
		}
		break;
	}
	return ret;
}

static	BOOL	PokeSexAct(POKE_GRA_TEST *pgt)
{
	int	sex;

	sex=PokeGraTestSexGet(pgt->mons_no);
	if(sex==3){
		pgt->sex^=1;
	}
	PokeTestPokeSexDataPut(pgt);
	PokeGraTestDebugPrint(pgt,VOICE_OFF);

	return TRUE;
}

static	BOOL	PokeColorAct(POKE_GRA_TEST *pgt)
{
	pgt->col^=1;
	PokeTestPokeColorDataPut(pgt);
	PokeGraTestDebugPrint(pgt,VOICE_OFF);

	return TRUE;
}

static	BOOL	PokePatAct(POKE_GRA_TEST *pgt)
{
	pgt->anm_pat^=1;
	PokeTestPokePatDataPut(pgt);
	PokeGraTestDebugPrint(pgt,VOICE_OFF);

	return TRUE;
}

static	BOOL	PokeFormAct(POKE_GRA_TEST *pgt)
{
	BOOL	ret=FALSE;
	int		max;

	if(sys.trg==PAD_BUTTON_X){
		PokeColorAct(pgt);
	}
	if(sys.trg==PAD_BUTTON_Y){
		PokeSexAct(pgt);
	}

	max=PokeTestPokeFormMaxGet(pgt);

	switch(pgt->sub_seq_no){
	case 0:
		PokeTestPokeFormDataPut(pgt,MODE_ACTIVE);
		pgt->sub_seq_no++;
	case 1:
		switch(PokeTestNumCalc(pgt,&pgt->form_no,2,0,max,MODE_LOOP)){
		case NUM_CALC_NONE:
			break;
		case NUM_CALC_CHANGE:
			PokeGraTestDebugPrint(pgt,VOICE_OFF);
		case NUM_CALC_MOVE:
			PokeTestPokeFormDataPut(pgt,MODE_ACTIVE);
			break;
		case NUM_CALC_END:
			PokeTestPokeFormDataPut(pgt,MODE_NORMAL);
			ret=TRUE;
			break;
		}
		break;
	}
	return ret;
}

static	BOOL	PokeHeightAct(POKE_GRA_TEST *pgt)
{
	BOOL	ret=FALSE;

	if((pgt->shadow_flag%3)==2){
		return TRUE;
	}

	switch(pgt->sub_seq_no){
	case 0:
		PokeTestPokeHeightDataPut(pgt,COL_ACTIVE);
		pgt->sub_seq_no++;
	case 1:
		if((sys.repeat & PAD_KEY_UP)&&(pgt->paed[PokeGraNoTable[pgt->mons_no]].height<HEIGHT_MAX)){
			pgt->paed[PokeGraNoTable[pgt->mons_no]].height++;
			SoftSpriteParaSet(pgt->ss[1],SS_PARA_SHADOW_HEIGHT,pgt->paed[PokeGraNoTable[pgt->mons_no]].height);
			PokeTestPokeHeightDataPut(pgt,COL_ACTIVE);
		}
		if((sys.repeat & PAD_KEY_DOWN)&&(pgt->paed[PokeGraNoTable[pgt->mons_no]].height>HEIGHT_MIN)){
			pgt->paed[PokeGraNoTable[pgt->mons_no]].height--;
			SoftSpriteParaSet(pgt->ss[1],SS_PARA_SHADOW_HEIGHT,pgt->paed[PokeGraNoTable[pgt->mons_no]].height);
			PokeTestPokeHeightDataPut(pgt,COL_ACTIVE);
		}
		if(sys.trg&PAD_BUTTON_A){
			PokeTestPokeHeightDataPut(pgt,COL_NORMAL);
			ret=TRUE;
		}
		break;
	}
	return ret;
}

static	BOOL	PokeOfxAct(POKE_GRA_TEST *pgt)
{
	BOOL	ret=FALSE;

	if((pgt->shadow_flag%3)==2){
		return TRUE;
	}

	switch(pgt->sub_seq_no){
	case 0:
		PokeTestPokeOfxDataPut(pgt,COL_ACTIVE);
		pgt->sub_seq_no++;
	case 1:
		if((sys.repeat & PAD_KEY_RIGHT)&&(pgt->paed[PokeGraNoTable[pgt->mons_no]].shadow_ofx<HEIGHT_MAX)){
			pgt->paed[PokeGraNoTable[pgt->mons_no]].shadow_ofx++;
			SoftSpriteParaSet(pgt->ss[1],SS_PARA_SHADOW_OX,pgt->paed[PokeGraNoTable[pgt->mons_no]].shadow_ofx);
			PokeTestPokeOfxDataPut(pgt,COL_ACTIVE);
		}
		if((sys.repeat & PAD_KEY_LEFT)&&(pgt->paed[PokeGraNoTable[pgt->mons_no]].shadow_ofx>HEIGHT_MIN)){
			pgt->paed[PokeGraNoTable[pgt->mons_no]].shadow_ofx--;
			SoftSpriteParaSet(pgt->ss[1],SS_PARA_SHADOW_OX,pgt->paed[PokeGraNoTable[pgt->mons_no]].shadow_ofx);
			PokeTestPokeOfxDataPut(pgt,COL_ACTIVE);
		}
		if(sys.trg&PAD_BUTTON_A){
			PokeTestPokeOfxDataPut(pgt,COL_NORMAL);
			ret=TRUE;
		}
		break;
	}
	return ret;
}

static	BOOL	PokeShadowAct(POKE_GRA_TEST *pgt)
{
	BOOL	ret=FALSE;

	if((pgt->shadow_flag%3)==2){
		return TRUE;
	}

	switch(pgt->sub_seq_no){
	case 0:
		PokeTestPokeShadowDataPut(pgt,COL_ACTIVE);
		pgt->sub_seq_no++;
	case 1:
		if((sys.repeat & PAD_KEY_UP)&&(pgt->paed[PokeGraNoTable[pgt->mons_no]].shadow_size<3)){
			pgt->paed[PokeGraNoTable[pgt->mons_no]].shadow_size++;
			SoftSpriteParaSet(pgt->ss[1],SS_PARA_SHADOW_SIZE,pgt->paed[PokeGraNoTable[pgt->mons_no]].shadow_size);
			PokeTestPokeShadowDataPut(pgt,COL_ACTIVE);
		}
		if((sys.repeat & PAD_KEY_DOWN)&&(pgt->paed[PokeGraNoTable[pgt->mons_no]].shadow_size>0)){
			pgt->paed[PokeGraNoTable[pgt->mons_no]].shadow_size--;
			SoftSpriteParaSet(pgt->ss[1],SS_PARA_SHADOW_SIZE,pgt->paed[PokeGraNoTable[pgt->mons_no]].shadow_size);
			PokeTestPokeShadowDataPut(pgt,COL_ACTIVE);
		}
		if(sys.trg&PAD_BUTTON_A){
			PokeTestPokeShadowDataPut(pgt,COL_NORMAL);
			ret=TRUE;
		}
		break;
	}
	return ret;
}

static	BOOL	PokeBGColorRAct(POKE_GRA_TEST *pgt)
{
	BOOL	ret=FALSE;

	switch(pgt->sub_seq_no){
	case 0:
		PokeTestPokeBGColorRDataPut(pgt,MODE_ACTIVE);
		pgt->sub_seq_no++;
	case 1:
		switch(PokeTestNumCalc(pgt,&pgt->bg_r,2,0,31,MODE_LOOP)){
		case NUM_CALC_NONE:
			break;
		case NUM_CALC_CHANGE:
			PokeTestBGColorSet(pgt);
		case NUM_CALC_MOVE:
			PokeTestPokeBGColorRDataPut(pgt,MODE_ACTIVE);
			break;
		case NUM_CALC_END:
			PokeTestPokeBGColorRDataPut(pgt,MODE_NORMAL);
			ret=TRUE;
			break;
		}
		break;
	}
	return ret;
}

static	BOOL	PokeBGColorGAct(POKE_GRA_TEST *pgt)
{
	BOOL	ret=FALSE;

	switch(pgt->sub_seq_no){
	case 0:
		PokeTestPokeBGColorGDataPut(pgt,MODE_ACTIVE);
		pgt->sub_seq_no++;
	case 1:
		switch(PokeTestNumCalc(pgt,&pgt->bg_g,2,0,31,MODE_LOOP)){
		case NUM_CALC_NONE:
			break;
		case NUM_CALC_CHANGE:
			PokeTestBGColorSet(pgt);
		case NUM_CALC_MOVE:
			PokeTestPokeBGColorGDataPut(pgt,MODE_ACTIVE);
			break;
		case NUM_CALC_END:
			PokeTestPokeBGColorGDataPut(pgt,MODE_NORMAL);
			ret=TRUE;
			break;
		}
		break;
	}
	return ret;
}

static	BOOL	PokeBGColorBAct(POKE_GRA_TEST *pgt)
{
	BOOL	ret=FALSE;

	switch(pgt->sub_seq_no){
	case 0:
		PokeTestPokeBGColorBDataPut(pgt,MODE_ACTIVE);
		pgt->sub_seq_no++;
	case 1:
		switch(PokeTestNumCalc(pgt,&pgt->bg_b,2,0,31,MODE_LOOP)){
		case NUM_CALC_NONE:
			break;
		case NUM_CALC_CHANGE:
			PokeTestBGColorSet(pgt);
		case NUM_CALC_MOVE:
			PokeTestPokeBGColorBDataPut(pgt,MODE_ACTIVE);
			break;
		case NUM_CALC_END:
			PokeTestPokeBGColorBDataPut(pgt,MODE_NORMAL);
			ret=TRUE;
			break;
		}
		break;
	}
	return ret;
}

//==============================================================================================
//
//	数値編集
//
//==============================================================================================
static	int	PokeTestNumCalc(POKE_GRA_TEST *pgt,int *param,int keta,int min,int max,int mode)
{
	int ret=NUM_CALC_NONE;
	u32	calc_work[]={
		1,
		10,
		100,
		1000,
		10000,
		100000,
		1000000,
		10000000,
		100000000,
		1000000000,
	};

	if((sys.repeat & PAD_KEY_LEFT)&&(pgt->cur_pos_keta<(keta-1))){
		pgt->cur_pos_keta++;
		ret=NUM_CALC_MOVE;
	}
	else if((sys.repeat & PAD_KEY_RIGHT)&&(pgt->cur_pos_keta)){
		pgt->cur_pos_keta--;
		ret=NUM_CALC_MOVE;
	}
	else if(sys.repeat & PAD_KEY_UP){
		if(mode==MODE_LOOP){
			if((param[0]+calc_work[pgt->cur_pos_keta])>max){
				param[0]=min;
				ret=NUM_CALC_CHANGE;
			}
			else{
				param[0]+=calc_work[pgt->cur_pos_keta];
				ret=NUM_CALC_CHANGE;
			}
		}
		else{
			if((param[0]+calc_work[pgt->cur_pos_keta])>max){
				param[0]=max;
				ret=NUM_CALC_CHANGE;
			}
			else{
				param[0]+=calc_work[pgt->cur_pos_keta];
				ret=NUM_CALC_CHANGE;
			}
		}
	}
	else if(sys.repeat & PAD_KEY_DOWN){
		if(mode==MODE_LOOP){
			if((param[0]<calc_work[pgt->cur_pos_keta])||((param[0]-calc_work[pgt->cur_pos_keta])<min)){
				param[0]=max;
				ret=NUM_CALC_CHANGE;
			}
			else{
				param[0]-=calc_work[pgt->cur_pos_keta];
				ret=NUM_CALC_CHANGE;
			}
		}
		else{
			if((param[0]<calc_work[pgt->cur_pos_keta])||((param[0]-calc_work[pgt->cur_pos_keta])<min)){
				param[0]=min;
				ret=NUM_CALC_CHANGE;
			}
			else{
				param[0]-=calc_work[pgt->cur_pos_keta];
				ret=NUM_CALC_CHANGE;
			}
		}
	}
	else if(sys.trg & PAD_BUTTON_A){
		pgt->cur_pos_keta=0;
		ret=NUM_CALC_END;
	}

	return ret;
}

//==============================================================================================
//
//	パラメータロード
//
//==============================================================================================
static	BOOL	PokeGraTestLoadAct(POKE_GRA_TEST *pgt)
{
	int	i;

	switch(pgt->sub_seq_no){
	case 0:
		POKE_ANM_DEBUG_PRINT( " --- save_ram : 0x%08x size : 0x%08x\n",&pgt->paed[ 0 ], POKE_ANM_SAVE_SIZE );
		GF_BGL_BmpWinFill( pgt->win_s, 0x0f, 0, 0, 255, 192 );
		MSGMAN_GetString(pgt->man_msg,POKE_TEST_010,pgt->msg_buf);
		GF_STR_PrintSimple(pgt->win_s,FONT_SYSTEM,pgt->msg_buf,SVLD_X,SVLD_Y,MSG_NO_PUT,NULL);
		PokeGraTestYesNoPut(pgt);
		YesNoCursorMove(pgt,pgt->yes_no_pos,CUR_PUT);
		pgt->sub_seq_no++;
		break;
	case 1:
		if((sys.trg==PAD_KEY_UP)&&(pgt->yes_no_pos==1)){
			YesNoCursorMove(pgt,0,CUR_PUT);
		}
		if((sys.trg==PAD_KEY_DOWN)&&(pgt->yes_no_pos==0)){
			YesNoCursorMove(pgt,1,CUR_PUT);
		}
		if(sys.trg==PAD_BUTTON_A){
			if(pgt->yes_no_pos==0){
			#if 1		///< ATODE
			//	PMSVLD_Load(pgt->load_bank, &pgt->paed[0], POKE_ANM_SAVE_SIZE);
				PMSVLD_Load(EDIT_BANK, &pgt->paed[0], POKE_ANM_SAVE_SIZE);
			#endif
			}
			if(pgt->paed[0].height!=0x55){
				for(i=0;i<PGT_POKE_MAX;i++){
					pgt->paed[i].height=0;
				}
				pgt->paed[0].height=0x55;
			}
			for(i=0;i<PGT_POKE_MAX;i++){
				if(pgt->paed[i].shadow_size>3){
					pgt->paed[i].shadow_size=1;
				}
			}
			if(pgt->paed[0].shadow_ofx!=0x55){
				for(i=0;i<PGT_POKE_MAX;i++){
					pgt->paed[i].shadow_ofx=0;
				}
				pgt->paed[0].shadow_ofx=0x55;
			}
			#if 0
			{
				int i;
				int num = 0;
				OS_Printf( " 鳴き声書き換え中 \n" );
				for ( i = 0; i < PGT_POKE_MAX;i++){
					
					if ( pgt->paed[ i ].poke_anime[ 1 ].voice_wait == 15
					&&	 pgt->paed[ i ].poke_anime[ 1 ].ssanm[ 0 ].patno == -1 ){
						
						pgt->paed[ i ].poke_anime[ 1 ].voice_wait = 0;
					}
					else {
						num++;
					}
				}
				
				
				OS_Printf( " %2d \n", num );
			}
			#endif
			PokeGraTestSubScreenMake(pgt);
			return TRUE;
		}
		if(sys.trg==PAD_BUTTON_B){
			PokeGraTestSubScreenMake(pgt);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

//==============================================================================================
//
//	パラメータセーブ
//
//==============================================================================================
static	BOOL	PokeGraTestSaveAct(POKE_GRA_TEST *pgt)
{
	switch(pgt->sub_seq_no){
	case 0:
		POKE_ANM_DEBUG_PRINT( " --- save_ram : 0x%08x size : 0x%08x\n",&pgt->paed[ 0 ], POKE_ANM_SAVE_SIZE );
		GF_BGL_BmpWinFill( pgt->win_s, 0x0f, 0, 0, 255, 192 );
		MSGMAN_GetString(pgt->man_msg,POKE_TEST_011,pgt->msg_buf);
		GF_STR_PrintSimple(pgt->win_s,FONT_SYSTEM,pgt->msg_buf,SVLD_X,SVLD_Y,MSG_NO_PUT,NULL);
		PokeGraTestYesNoPut(pgt);
		YesNoCursorMove(pgt,pgt->yes_no_pos,CUR_PUT);
		pgt->sub_seq_no++;
		break;
	case 1:
		if((sys.trg==PAD_KEY_UP)&&(pgt->yes_no_pos==1)){
			YesNoCursorMove(pgt,0,CUR_PUT);
		}
		if((sys.trg==PAD_KEY_DOWN)&&(pgt->yes_no_pos==0)){
			YesNoCursorMove(pgt,1,CUR_PUT);
		}
		if(sys.trg==PAD_BUTTON_A){
			if(pgt->yes_no_pos==0){
			#if 1		///< ATODE	
			//	PMSVLD_Save(pgt->save_bank, &pgt->paed[0], POKE_ANM_SAVE_SIZE);
				PMSVLD_Save(EDIT_BANK, &pgt->paed[0], POKE_ANM_SAVE_SIZE);
				#if 0
				{
					int i;
					
					for ( i = 0; i < 500; i++ ){
						OS_Printf( "%2d\n", pgt->paed[i].poke_anime[1].ssanm[0].patno);
					}
				}
				#endif
			#endif
			}
			PokeGraTestSubScreenMake(pgt);
			return TRUE;
		}
		if(sys.trg==PAD_BUTTON_B){
			PokeGraTestSubScreenMake(pgt);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

//==============================================================================================
//
//	YesNo
//
//==============================================================================================
static	void	PokeGraTestYesNoPut(POKE_GRA_TEST *pgt)
{
	MSGMAN_GetString(pgt->man_msg,POKE_TEST_012,pgt->msg_buf);
	GF_STR_PrintSimple(pgt->win_s,FONT_SYSTEM,pgt->msg_buf,YES_NO_X,YES_NO_Y,MSG_NO_PUT,NULL);
	MSGMAN_GetString(pgt->man_msg,POKE_TEST_013,pgt->msg_buf);
	GF_STR_PrintSimple(pgt->win_s,FONT_SYSTEM,pgt->msg_buf,YES_NO_X,YES_NO_Y+ANM_NO_OFS_Y,MSG_NO_PUT,NULL);
	pgt->put_req=1;
}

#else

///< プレゼン用

#include "common.h"
#include "system/fontproc.h"
#include "system/font_arc.h"
#include "system/pm_str.h"
#include "system/msgdata.h"						//MSGDATA_MANAGER
#include "system/arc_tool.h"
#include "system/arc_util.h"
#include "system/softsprite.h"
#include "poke_anime.h"

#include "msgdata/msg.naix"						//NARC_msg_??_dat
#include "battle/graphic/pl_batt_bg_def.h"
#include "battle/battle_common.h"
#include "battle/battle_server.h"
#include "battle/fight.h"
#include "poketool/monsno.h"
#include "poketool/poke_tool.h"
#include "system/snd_tool.h"
#include "system/wordset.h"
#include "system/window.h"

#include "pokeanime/p_anm_sys.h"

#include "system/main.h"
#include "demo/title.h"
#include "msgdata/msg_poke_test.h"
#include "msgdata/msg_poke_anime.h"

#include "poketool/pokegra/pokegrano.dat"

extern	BOOL	PMSVLD_Load(u32 src, void * dst, u32 len);
extern	BOOL	PMSVLD_Save(u32 src, void * dst, u32 len);

///< コレを有効にすると、SAVE_BANKからLOADする
//#define		SAVE_BANK_LOAD

#define		SAVE_BANK	(30)
#define		EDIT_BANK	( EDIT_ANM_SAVE )
#define		HEIGHT_MAX	(32)
#define		HEIGHT_MIN	(-32)
#define		SHADOW_MAX	(2)

//#define		GO_USE		//有効にするとichinose仕様

enum{
	SIDE_MAIN=0,
	SIDE_SUB,
};

enum{
	NUM_CALC_NONE=0,
	NUM_CALC_MOVE,
	NUM_CALC_CHANGE,
	NUM_CALC_END,
};

enum{
	MODE_NORMAL=0,
	MODE_ACTIVE,
	MODE_LOOP,
	MODE_STOP,
	MODE_DEFAULT_PARAM,
	MODE_DEFAULT_WAZA,
};

enum{
	VOICE_ON=0,
	VOICE_OFF,
};

//#if 1
#define POKE_ANM_DEBUG_PRINT(...) (void) ((OS_TPrintf(__VA_ARGS__)))
//#define POKE_ANM_PRINT(...)           ((void) 0)

//==============================================================================================
//
//	構造体
//
//==============================================================================================

#define	PGT_POKE_MAX	(600)					//ポケモンアニメーションデータのMAX
#define	PGT_END_KEY		(PAD_BUTTON_L|PAD_BUTTON_R|PAD_BUTTON_SELECT)

enum{
	SEQ_POKEMON_CHECK_INIT=0,
	SEQ_POKEMON_CHECK,
	SEQ_POKEMON_CHECK_EDIT,
	SEQ_POKEMON_ANIME,
	SEQ_POKEMON_ANIME_EDIT,
	SEQ_LOAD,
	SEQ_SAVE,
	SEQ_POKEMON_ANIME_ACT,
	SEQ_POKEMON_ANIME_ACT_B,
	SEQ_POKEMON_ANIME_ACT_F,
};

enum{
	CUR_PUT=0,
	CUR_DEL
};

typedef struct{
	u8	patno;
	u8	wait;
}PRG_ANIME;

typedef struct{
	u8					prg_anm_f;				//プログラムアニメナンバー（正面）
	u8					prg_anm_f_wait;
	PRG_ANIME			prg_anm_b[3];			//プログラムアニメナンバー（背面）
	SOFT_SPRITE_ANIME	ssanm[SS_ANM_SEQ_MAX];	//パターンアニメデータ
}POKE_ANM_DATA;



// -----------------------------------------
//
//	■パターン定義
//
// -----------------------------------------
enum {
	PAD_F = 0,										///< 正面アニメ	
	PAD_B,											///< 背面アニメ
	PAD_MAX,										///< アニメ最大数	
};

enum {
	
	PAD_MODE_Init = 0,								///< 初期化
	PAD_MODE_BaseParaEdit,							///< 基本パラメータ設定
	PAD_MODE_F_AnimeEdit,							///< 正面アニメ設定
	PAD_MODE_B_AnimeEdit,							///< 背面アニメ設定
	
};

enum {
	
	PAD_PAT_0	= +0,
	PAD_PAT_1	= +1,
	
	PAD_DEF		= -1,
	
	PAD_LOOP_1	= -2,
	PAD_LOOP_2	= -3,
	PAD_LOOP_3	= -4,
	PAD_LOOP_4	= -5,
	PAD_LOOP_5	= -6,
	PAD_LOOP_6	= -7,
	PAD_LOOP_7	= -8,
	PAD_LOOP_8	= -9,
	PAD_LOOP_9	= -10,
	PAD_LOOP_10	= -11,	
};

#define GET_LOOP_LABEL( x )		( ( x - PAD_DEF) * -1 )

#include "src/poketool/poke_tool_def.h"

typedef struct{
	BOOL end_flag;
	GF_BGL_INI			*bgl;
	GF_BGL_BMPWIN		*win_m;
	GF_BGL_BMPWIN		*win_s;
	SOFT_SPRITE_MANAGER	*ssm;
	SOFT_SPRITE			*ss[2];
	MSGDATA_MANAGER		*man_msg;
	MSGDATA_MANAGER		*man_pam;
	MSGDATA_MANAGER		*man_poke;
	STRBUF				*msg_buf;
	STRBUF				*msg_buf2;
	WORDSET*			wordset;
	POKE_ANM_SYS_PTR	pasp;
	int					seq_no;
	int					seq_no_temp;
	int					sub_seq_no;
	int					mons_no;
	int					sex;
	int					col;
	int					anm_pat;
	int					form_no;
	int					pos;
	int					yes_no_pos;
	int					put_req;
	int					main_pos;
	int					cur_pos_keta;
	int					shadow_flag;
	int					bg_flag;
	int					col_flag;
	int					bg_r;
	int					bg_g;
	int					bg_b;
	
	// 新規
	int					sub_page;
	u32					save_bank;
	u32					load_bank;
	BOOL				is_anime;
	BOOL				is_main_page;

	// ここ以下は、セーブしているので、位置の変更は厳禁！	
	POKE_ANM_DATA		pad[PGT_POKE_MAX];
	s8					height[PGT_POKE_MAX];
	u8					shadow_size[PGT_POKE_MAX];
	s8					shadow_ofx[PGT_POKE_MAX];
	
	// プラチナ用保存データ
	POKE_ANM_EDIT_DATA	paed[ PGT_POKE_MAX ];
}POKE_GRA_TEST;

#define POKE_ANM_SAVE_SIZE	( sizeof(POKE_ANM_EDIT_DATA) * PGT_POKE_MAX )

//==============================================================================================
//
//	プロトタイプ宣言
//
//==============================================================================================

static	void	PokeGraInit(POKE_GRA_TEST *pgt);
static	void	PokeGraTestBGCreate(POKE_GRA_TEST *pgt,GF_BGL_INI *bgl);
static	void	PokeTestMainScreenCreate(POKE_GRA_TEST *pgt);
static	void	PokeGraTestDebugPrint(POKE_GRA_TEST *pgt,int mode);
static	void	PokeGraTestVBlank(void *work);
static	void	Init3DStuff_(void);
static	u8		PokeGraTestSexGet(u16 monsno);
static	void	PokeGraTestMSG_Print(POKE_GRA_TEST *pgt);
static	void	PokeGraTestSubScreenMake(POKE_GRA_TEST *pgt);
static	void	PokeGraTestSubScreenAnmFDataPut(POKE_GRA_TEST *pgt,GF_PRINTCOLOR col);
static	void	PokeGraTestSubScreenAnmFWaitDataPut(POKE_GRA_TEST *pgt,GF_PRINTCOLOR col);
static	void	PokeGraTestSubScreenAnmBDataPut(POKE_GRA_TEST *pgt,int num,GF_PRINTCOLOR col);
static	void	PokeGraTestSubScreenAnmBWaitDataPut(POKE_GRA_TEST *pgt,int num,GF_PRINTCOLOR col);
static	void	PokeGraTestSubScreenPatDataPut(POKE_GRA_TEST *pgt,int pos,GF_PRINTCOLOR col);
static	void	PokeGraTestSubScreenWaitDataPut(POKE_GRA_TEST *pgt,int pos,GF_PRINTCOLOR col);
static	void	CursorMove(POKE_GRA_TEST *pgt,int side,int pos,int mode);
static	void	YesNoCursorMove(POKE_GRA_TEST *pgt,int pos,int mode);
static	BOOL	PokeNumAct(POKE_GRA_TEST *pgt);
static	BOOL	PokeSexAct(POKE_GRA_TEST *pgt);
static	BOOL	PokeColorAct(POKE_GRA_TEST *pgt);
static	BOOL	PokePatAct(POKE_GRA_TEST *pgt);
static	BOOL	PokeFormAct(POKE_GRA_TEST *pgt);
static	BOOL	PokeHeightAct(POKE_GRA_TEST *pgt);
static	BOOL	PokeOfxAct(POKE_GRA_TEST *pgt);
static	BOOL	PokeShadowAct(POKE_GRA_TEST *pgt);
static	BOOL	PokeBGColorRAct(POKE_GRA_TEST *pgt);
static	BOOL	PokeBGColorGAct(POKE_GRA_TEST *pgt);
static	BOOL	PokeBGColorBAct(POKE_GRA_TEST *pgt);
static	BOOL	AnmFrontAct(POKE_GRA_TEST *pgt);
static	BOOL	AnmFrontWaitAct(POKE_GRA_TEST *pgt);
static	BOOL	AnmBackAct(POKE_GRA_TEST *pgt);
static	BOOL	AnmBackWaitAct(POKE_GRA_TEST *pgt);
static	BOOL	PatDataAct(POKE_GRA_TEST *pgt);
static	BOOL	WaitDataAct(POKE_GRA_TEST *pgt);
static	BOOL	PokeGraTestLoadAct(POKE_GRA_TEST *pgt);
static	BOOL	PokeGraTestSaveAct(POKE_GRA_TEST *pgt);
static	void	PokeGraTestYesNoPut(POKE_GRA_TEST *pgt);

static	void	PokeTestPokeNumDataPut(POKE_GRA_TEST *pgt,int mode);
static	void	PokeTestPokeSexDataPut(POKE_GRA_TEST *pgt);
static	void	PokeTestPokeColorDataPut(POKE_GRA_TEST *pgt);
static	void	PokeTestPokePatDataPut(POKE_GRA_TEST *pgt);
static	void	PokeTestPokeFormDataPut(POKE_GRA_TEST *pgt,int mode);
static	void	PokeTestPokeHeightDataPut(POKE_GRA_TEST *pgt,GF_PRINTCOLOR col);
static	void	PokeTestPokeOfxDataPut(POKE_GRA_TEST *pgt,GF_PRINTCOLOR col);
static	void	PokeTestPokeShadowDataPut(POKE_GRA_TEST *pgt,GF_PRINTCOLOR col);
static	void	PokeTestPokeBGColorRDataPut(POKE_GRA_TEST *pgt,int mode);
static	void	PokeTestPokeBGColorGDataPut(POKE_GRA_TEST *pgt,int mode);
static	void	PokeTestPokeBGColorBDataPut(POKE_GRA_TEST *pgt,int mode);
static	void	PokeTestBGColorSet(POKE_GRA_TEST *pgt);

static	void	PokeTestNumPut(POKE_GRA_TEST *pgt,int x,int y,int param,int keta,int pos,int mode);
static	int		PokeTestNumCalc(POKE_GRA_TEST *pgt,int *param,int keta,int min,int max,int mode);
static	int		PokeTestPokeFormMaxGet(POKE_GRA_TEST *pgt);

static	void	PokeGraTestShadowSet(POKE_GRA_TEST *pgt,int height);

static void PDA_PageChageView( POKE_GRA_TEST* pgt, GF_PRINTCOLOR col );

static void	PAD_PageView(POKE_GRA_TEST* pgt,GF_PRINTCOLOR col);
static BOOL	PAD_PageMain(POKE_GRA_TEST* pgt);

static void	PAD_VoiceView(POKE_GRA_TEST* pgt,GF_PRINTCOLOR col);
static BOOL	PAD_VoiceMain(POKE_GRA_TEST* pgt);

static void	PAD_PG_Anm_View(POKE_GRA_TEST* pgt,GF_PRINTCOLOR col);
static BOOL	PAD_PG_Anm_Main(POKE_GRA_TEST* pgt);

static void	PAD_PG_AnmWait_View(POKE_GRA_TEST* pgt,GF_PRINTCOLOR col);
static BOOL	PAD_PG_AnmWait_Main(POKE_GRA_TEST* pgt);

static void	PAD_PAT_Anm_View(POKE_GRA_TEST* pgt, GF_PRINTCOLOR col, int no);
static BOOL	PAD_PAT_Anm_Main(POKE_GRA_TEST* pgt);

static void	PAD_PAT_AnmWait_View(POKE_GRA_TEST* pgt, GF_PRINTCOLOR col, int no);
static BOOL	PAD_PAT_AnmWait_Main(POKE_GRA_TEST* pgt);

static void	PAD_PAT_AnmPos_View(POKE_GRA_TEST* pgt, GF_PRINTCOLOR col, int no);
static BOOL	PAD_PAT_AnmPos_Main(POKE_GRA_TEST* pgt);

//==================================================================================================
//
//	データ
//
//==================================================================================================

static	const	u16	pal[]={
		GX_RGB(31,31,31),				//0:白
		GX_RGB(13,13,13),				//1:濃い灰
		GX_RGB(24,24,24),				//2:薄い灰
		GX_RGB(15,31,15),				//3:うすみどり
		GX_RGB(31,15,15),				//4:うすあか
		GX_RGB(19,19,19),				//5:灰
		GX_RGB( 0, 0, 0),
		GX_RGB( 0, 0, 0),
		GX_RGB( 0, 0, 0),
		GX_RGB( 0, 0, 0),
		GX_RGB( 0, 0, 0),
		GX_RGB( 0, 0, 0),
		GX_RGB( 0, 0, 0),
		GX_RGB( 0, 0, 0),
		GX_RGB( 0, 0, 0),
		GX_RGB(31,31,31),				//15:白
};

//ポケモンチェック
enum{
	POKE_NUM=0,
	POKE_SEX,
	POKE_COLOR,
	POKE_PAT,
	POKE_FORM,
	POKE_HEIGHT,
	POKE_OFX,
	POKE_SHADOW,
	POKE_BG_COLOR_R,
	POKE_BG_COLOR_G,
	POKE_BG_COLOR_B
};

//アニメーション設定
enum{
	MENU_ANM_FRONT=0,
	MENU_ANM_BACK1,
	MENU_ANM_BACK2,
	MENU_ANM_BACK3,
	MENU_ANM_FRONT_WAIT,
	MENU_ANM_BACK1_WAIT,
	MENU_ANM_BACK2_WAIT,
	MENU_ANM_BACK3_WAIT,
	MENU_PAT_01,
	MENU_PAT_02,
	MENU_PAT_03,
	MENU_PAT_04,
	MENU_PAT_05,
	MENU_PAT_06,
	MENU_PAT_07,
	MENU_PAT_08,
	MENU_PAT_09,
	MENU_PAT_10,
	MENU_WAIT_01,
	MENU_WAIT_02,
	MENU_WAIT_03,
	MENU_WAIT_04,
	MENU_WAIT_05,
	MENU_WAIT_06,
	MENU_WAIT_07,
	MENU_WAIT_08,
	MENU_WAIT_09,
	MENU_WAIT_10,
};

typedef BOOL (*PT_FUNC)(POKE_GRA_TEST *);
typedef void (*PT_VIEW_FUNC)(POKE_GRA_TEST *, GF_PRINTCOLOR );
typedef void (*PT_VIEW_FUNC_EX)(POKE_GRA_TEST *, GF_PRINTCOLOR, int );

typedef struct
{
	u8		cursor_pos_x;	//カーソルX位置
	u8		cursor_pos_y;	//カーソルY位置
	u8		move_up;		//上キーを押したときの移動先
	u8		move_down;		//下キーを押したときの移動先
	u8		move_left;		//左キーを押したときの移動先
	u8		move_right;		//右キーを押したときの移動先
	PT_FUNC	a_button_act;	//Aボタンを押したときのアクション
}MENU_LIST_PARAM;

#define	MONS_NUM_X	(0)		//モンスターナンバー表示X位置
#define	MONS_NUM_Y	(8)		//モンスターナンバー表示Y位置
#define	MONS_NAME_X	(32)	//モンスター名表示X位置
#define	MONS_NAME_Y	(8)		//モンスター名表示Y位置
#define	MONS_SEX_X	(96)	//モンスター性別表示X位置
#define	MONS_SEX_Y	(8)		//モンスター性別表示Y位置
#define	MONS_COL_X	(128)	//モンスターカラー表示X位置
#define	MONS_COL_Y	(8)		//モンスターカラー表示Y位置
#define	MONS_PAT_X	(188)	//モンスターアニメパターンナンバー表示X位置
#define	MONS_PAT_Y	(8)		//モンスターアニメパターンナンバー表示Y位置

#define	POKE_NUM_DATA_X			(12)
#define	POKE_NUM_DATA_Y			(0)
#define	POKE_NUM_X				(POKE_NUM_DATA_X-12)
#define	POKE_NUM_Y				(0)
#define	POKE_SEX_DATA_X			(108)
#define	POKE_SEX_DATA_Y			(0)
#define	POKE_SEX_X				(POKE_SEX_DATA_X-12)
#define	POKE_SEX_Y				(0)
#define	POKE_COLOR_DATA_X		(140)
#define	POKE_COLOR_DATA_Y		(0)
#define	POKE_COLOR_X			(POKE_COLOR_DATA_X-12)
#define	POKE_COLOR_Y			(0)
#define	POKE_PAT_DATA_X			(200)
#define	POKE_PAT_DATA_Y			(0)
#define	POKE_PAT_X				(POKE_PAT_DATA_X-12)
#define	POKE_PAT_Y				(0)
#define	POKE_FORM_DATA_X		(12)
#define	POKE_FORM_DATA_Y		(16)
#define	POKE_FORM_X				(POKE_FORM_DATA_X-12)
#define	POKE_FORM_Y				(16)
#define	POKE_HEIGHT_DATA_X		(60)
#define	POKE_HEIGHT_DATA_Y		(16)
#define	POKE_HEIGHT_X			(POKE_HEIGHT_DATA_X-12)
#define	POKE_HEIGHT_Y			(16)
#define	POKE_OFX_DATA_X			(88)
#define	POKE_OFX_DATA_Y			(16)
#define	POKE_OFX_X				(POKE_OFX_DATA_X-12)
#define	POKE_OFX_Y				(16)
#define	POKE_SHADOW_DATA_X		(124)
#define	POKE_SHADOW_DATA_Y		(16)
#define	POKE_SHADOW_X			(POKE_SHADOW_DATA_X-12)
#define	POKE_SHADOW_Y			(16)
#define	POKE_BG_COLOR_R_DATA_X	(144)
#define	POKE_BG_COLOR_R_DATA_Y	(16)
#define	POKE_BG_COLOR_R_X		(POKE_BG_COLOR_R_DATA_X-12)
#define	POKE_BG_COLOR_R_Y		(16)
#define	POKE_BG_COLOR_G_DATA_X	(172)
#define	POKE_BG_COLOR_G_DATA_Y	(16)
#define	POKE_BG_COLOR_G_X		(POKE_BG_COLOR_G_DATA_X-12)
#define	POKE_BG_COLOR_G_Y		(16)
#define	POKE_BG_COLOR_B_DATA_X	(200)
#define	POKE_BG_COLOR_B_DATA_Y	(16)
#define	POKE_BG_COLOR_B_X		(POKE_BG_COLOR_B_DATA_X-12)
#define	POKE_BG_COLOR_B_Y		(16)

static	const	MENU_LIST_PARAM	mlp_m[]={
	//POKE_NUM
	{
		POKE_NUM_X,
		POKE_NUM_Y,
		POKE_FORM,
		POKE_FORM,
		POKE_PAT,
		POKE_SEX,
		PokeNumAct,
	},
	//POKE_SEX
	{
		POKE_SEX_X,
		POKE_SEX_Y,
		POKE_OFX,
		POKE_OFX,
		POKE_NUM,
		POKE_COLOR,
		PokeSexAct,
	},
	//POKE_COLOR
	{
		POKE_COLOR_X,
		POKE_COLOR_Y,
		POKE_SHADOW,
		POKE_SHADOW,
		POKE_SEX,
		POKE_PAT,
		PokeColorAct,
	},
	//POKE_PAT
	{
		POKE_PAT_X,
		POKE_PAT_Y,
		POKE_BG_COLOR_R,
		POKE_BG_COLOR_R,
		POKE_COLOR,
		POKE_NUM,
		PokePatAct,
	},
	//POKE_FORM
	{
		POKE_FORM_X,
		POKE_FORM_Y,
		POKE_NUM,
		POKE_NUM,
		POKE_BG_COLOR_B,
		POKE_HEIGHT,
		PokeFormAct,
	},
	//POKE_HEIGHT
	{
		POKE_HEIGHT_X,
		POKE_HEIGHT_Y,
		POKE_SEX,
		POKE_SEX,
		POKE_FORM,
		POKE_OFX,
		PokeHeightAct,
	},
	//POKE_OFX
	{
		POKE_OFX_X,
		POKE_OFX_Y,
		POKE_SEX,
		POKE_SEX,
		POKE_HEIGHT,
		POKE_SHADOW,
		PokeOfxAct,
	},
	//POKE_SHADOW
	{
		POKE_SHADOW_X,
		POKE_SHADOW_Y,
		POKE_COLOR,
		POKE_COLOR,
		POKE_OFX,
		POKE_BG_COLOR_R,
		PokeShadowAct,
	},
	//POKE_BG_COLOR_R
	{
		POKE_BG_COLOR_R_X,
		POKE_BG_COLOR_R_Y,
		POKE_PAT,
		POKE_PAT,
		POKE_SHADOW,
		POKE_BG_COLOR_G,
		PokeBGColorRAct,
	},
	//POKE_BG_COLOR_G
	{
		POKE_BG_COLOR_G_X,
		POKE_BG_COLOR_G_Y,
		POKE_PAT,
		POKE_PAT,
		POKE_BG_COLOR_R,
		POKE_BG_COLOR_B,
		PokeBGColorGAct,
	},
	//POKE_BG_COLOR_B
	{
		POKE_BG_COLOR_B_X,
		POKE_BG_COLOR_B_Y,
		POKE_PAT,
		POKE_PAT,
		POKE_BG_COLOR_G,
		POKE_FORM,
		PokeBGColorBAct,
	},
};

#define	CUR_POS_X1		(4)
#define	CUR_POS_X2		(48)
#define	CUR_POS_X3		(96)
#define	CUR_POS_X4		(192)


#define	CUR_POS_X2_PATW	(64)

#define	CUR_POS_X2_PATP	(128-16)

#define	CUR_POS_Y1		(0)
#define	CUR_POS_Y2		(52)
#define	CUR_POS_OFS_Y1	(13)
#define	CUR_POS_OFS_Y2	(14)

#define CUR_POS_FIX( n )	( CUR_POS_Y1 + ( CUR_POS_OFS_Y2 * n ) )

#define	NO_MOVE			(0xff)

enum {
	
	ePAD_FB_CHANGE = 0,
	ePAD_VOICE_WAIT,
	
	ePAD_PG_ANIME,
	ePAD_PG_ANIME_WAIT,

	ePAD_EDIT_01,
	ePAD_EDIT_02,
	ePAD_EDIT_03,
	ePAD_EDIT_04,
	ePAD_EDIT_05,
	ePAD_EDIT_06,
	ePAD_EDIT_07,
	ePAD_EDIT_08,
	ePAD_EDIT_09,
	ePAD_EDIT_10,
	
	ePAD_EDIT_01_WAIT,
	ePAD_EDIT_02_WAIT,
	ePAD_EDIT_03_WAIT,
	ePAD_EDIT_04_WAIT,
	ePAD_EDIT_05_WAIT,
	ePAD_EDIT_06_WAIT,
	ePAD_EDIT_07_WAIT,
	ePAD_EDIT_08_WAIT,
	ePAD_EDIT_09_WAIT,
	ePAD_EDIT_10_WAIT,
	
	ePAD_EDIT_01_POS,
	ePAD_EDIT_02_POS,
	ePAD_EDIT_03_POS,
	ePAD_EDIT_04_POS,
	ePAD_EDIT_05_POS,
	ePAD_EDIT_06_POS,
	ePAD_EDIT_07_POS,
	ePAD_EDIT_08_POS,
	ePAD_EDIT_09_POS,
	ePAD_EDIT_10_POS,
};

static	const	MENU_LIST_PARAM	mlp[]={
	///< 前面 背面 切り替え
	{
		CUR_POS_X1,
		CUR_POS_FIX( 0 ),
		ePAD_EDIT_10,
		ePAD_VOICE_WAIT,
		ePAD_FB_CHANGE,
		ePAD_FB_CHANGE,
		PAD_PageMain,
	},
	///< 鳴き声ウェイト
	{
		CUR_POS_X1,
		CUR_POS_FIX( 1 ),
		ePAD_FB_CHANGE,
		ePAD_PG_ANIME,
		ePAD_VOICE_WAIT,
		ePAD_VOICE_WAIT,
		PAD_VoiceMain,			
	},
	///< プログラムアニメ
	{
		CUR_POS_X1,
		CUR_POS_FIX( 2 ),
		ePAD_VOICE_WAIT,
		ePAD_EDIT_01 + 0,
		ePAD_PG_ANIME_WAIT,
		ePAD_PG_ANIME_WAIT,
		PAD_PG_Anm_Main,			
	},
	///< プログラムアニメ wait
	{
		CUR_POS_X4,
		CUR_POS_FIX( 2 ),
		ePAD_VOICE_WAIT,
		ePAD_EDIT_01 + 0,
		ePAD_PG_ANIME,
		ePAD_PG_ANIME,
		PAD_PG_AnmWait_Main,			
	},

	///< パターンアニメ　１
	{
		CUR_POS_X1,				/// pos x
		CUR_POS_FIX( 3 ),		/// pos y
		ePAD_PG_ANIME,			/// ↑
		ePAD_EDIT_01 + 1,		/// ↓
		ePAD_EDIT_01_POS + 0,		/// ←
		ePAD_EDIT_01_WAIT + 0,		/// →
		PAD_PAT_Anm_Main,		/// [ A ]
	},
	///< パターンアニメ　２
	{
		CUR_POS_X1,
		CUR_POS_FIX( 4 ),
		ePAD_EDIT_01 + 0,
		ePAD_EDIT_01 + 2,
		ePAD_EDIT_01_POS + 1,
		ePAD_EDIT_01_WAIT + 1,
		PAD_PAT_Anm_Main,			
	},
	///< パターンアニメ　３
	{
		CUR_POS_X1,
		CUR_POS_FIX( 5 ),
		ePAD_EDIT_01 + 1,
		ePAD_EDIT_01 + 3,
		ePAD_EDIT_01_POS + 2,
		ePAD_EDIT_01_WAIT + 2,
		PAD_PAT_Anm_Main,			
	},
	///< パターンアニメ　４
	{
		CUR_POS_X1,
		CUR_POS_FIX( 6 ),
		ePAD_EDIT_01 + 2,
		ePAD_EDIT_01 + 4,
		ePAD_EDIT_01_POS + 3,
		ePAD_EDIT_01_WAIT + 3,
		PAD_PAT_Anm_Main,			
	},
	///< パターンアニメ　５
	{
		CUR_POS_X1,
		CUR_POS_FIX( 7 ),
		ePAD_EDIT_01 + 3,
		ePAD_EDIT_01 + 5,
		ePAD_EDIT_01_POS + 4,
		ePAD_EDIT_01_WAIT + 4,
		PAD_PAT_Anm_Main,			
	},
	///< パターンアニメ　６
	{
		CUR_POS_X1,
		CUR_POS_FIX( 8 ),
		ePAD_EDIT_01 + 4,
		ePAD_EDIT_01 + 6,
		ePAD_EDIT_01_POS + 5,
		ePAD_EDIT_01_WAIT + 5,
		PAD_PAT_Anm_Main,			
	},
	///< パターンアニメ　７
	{
		CUR_POS_X1,
		CUR_POS_FIX( 9 ),
		ePAD_EDIT_01 + 5,
		ePAD_EDIT_01 + 7,
		ePAD_EDIT_01_POS + 6,
		ePAD_EDIT_01_WAIT + 6,
		PAD_PAT_Anm_Main,			
	},
	///< パターンアニメ　８
	{
		CUR_POS_X1,
		CUR_POS_FIX( 10 ),
		ePAD_EDIT_01 + 6,
		ePAD_EDIT_01 + 8,
		ePAD_EDIT_01_POS + 7,
		ePAD_EDIT_01_WAIT + 7,
		PAD_PAT_Anm_Main,			
	},
	///< パターンアニメ　９
	{
		CUR_POS_X1,
		CUR_POS_FIX( 11 ),
		ePAD_EDIT_01 + 7,
		ePAD_EDIT_01 + 9,
		ePAD_EDIT_01_POS + 8,
		ePAD_EDIT_01_WAIT + 8,
		PAD_PAT_Anm_Main,			
	},
	///< パターンアニメ　１０
	{
		CUR_POS_X1,
		CUR_POS_FIX( 12 ),
		ePAD_EDIT_01 + 8,
		ePAD_FB_CHANGE,
		ePAD_EDIT_01_POS + 9,
		ePAD_EDIT_01_WAIT + 9,
		PAD_PAT_Anm_Main,			
	},

	///< パターンアニメ ウエイト　１
	{
		CUR_POS_X2_PATW,				/// pos x
		CUR_POS_FIX( 3 ),		/// pos y
		ePAD_PG_ANIME,			/// ↑
		ePAD_EDIT_01_WAIT + 1,	/// ↓
		ePAD_EDIT_01 + 0,		/// ←
		ePAD_EDIT_01_POS + 0,		/// →
		PAD_PAT_AnmWait_Main,		/// [ A ]
	},
	///< パターンアニメ ウエイト　２
	{
		CUR_POS_X2_PATW,
		CUR_POS_FIX( 4 ),
		ePAD_EDIT_01_WAIT + 0,
		ePAD_EDIT_01_WAIT + 2,
		ePAD_EDIT_01 + 1,
		ePAD_EDIT_01_POS + 1,
		PAD_PAT_AnmWait_Main,			
	},
	///< パターンアニメ ウエイト　３
	{
		CUR_POS_X2_PATW,
		CUR_POS_FIX( 5 ),
		ePAD_EDIT_01_WAIT + 1,
		ePAD_EDIT_01_WAIT + 3,
		ePAD_EDIT_01 + 2,
		ePAD_EDIT_01_POS + 2,
		PAD_PAT_AnmWait_Main,			
	},
	///< パターンアニメ ウエイト　４
	{
		CUR_POS_X2_PATW,
		CUR_POS_FIX( 6 ),
		ePAD_EDIT_01_WAIT + 2,
		ePAD_EDIT_01_WAIT + 4,
		ePAD_EDIT_01 + 3,
		ePAD_EDIT_01_POS + 3,
		PAD_PAT_AnmWait_Main,			
	},
	///< パターンアニメ ウエイト　５
	{
		CUR_POS_X2_PATW,
		CUR_POS_FIX( 7 ),
		ePAD_EDIT_01_WAIT + 3,
		ePAD_EDIT_01_WAIT + 5,
		ePAD_EDIT_01 + 4,
		ePAD_EDIT_01_POS + 4,
		PAD_PAT_AnmWait_Main,			
	},
	///< パターンアニメ ウエイト　６
	{
		CUR_POS_X2_PATW,
		CUR_POS_FIX( 8 ),
		ePAD_EDIT_01_WAIT + 4,
		ePAD_EDIT_01_WAIT + 6,
		ePAD_EDIT_01 + 5,
		ePAD_EDIT_01_POS + 5,
		PAD_PAT_AnmWait_Main,			
	},
	///< パターンアニメ ウエイト　７
	{
		CUR_POS_X2_PATW,
		CUR_POS_FIX( 9 ),
		ePAD_EDIT_01_WAIT + 5,
		ePAD_EDIT_01_WAIT + 7,
		ePAD_EDIT_01 + 6,
		ePAD_EDIT_01_POS + 6,
		PAD_PAT_AnmWait_Main,			
	},
	///< パターンアニメ ウエイト　８
	{
		CUR_POS_X2_PATW,
		CUR_POS_FIX( 10 ),
		ePAD_EDIT_01_WAIT + 6,
		ePAD_EDIT_01_WAIT + 8,
		ePAD_EDIT_01 + 7,
		ePAD_EDIT_01_POS + 7,
		PAD_PAT_AnmWait_Main,			
	},
	///< パターンアニメ ウエイト　９
	{
		CUR_POS_X2_PATW,
		CUR_POS_FIX( 11 ),
		ePAD_EDIT_01_WAIT + 7,
		ePAD_EDIT_01_WAIT + 9,
		ePAD_EDIT_01 + 8,
		ePAD_EDIT_01_POS + 8,
		PAD_PAT_AnmWait_Main,			
	},
	///< パターンアニメ ウエイト　１０
	{
		CUR_POS_X2_PATW,
		CUR_POS_FIX( 12 ),
		ePAD_EDIT_01_WAIT + 8,
		ePAD_FB_CHANGE,
		ePAD_EDIT_01 + 9,
		ePAD_EDIT_01_POS + 9,
		PAD_PAT_AnmWait_Main,			
	},

	///< パターンアニメ ポジション　１
	{
		CUR_POS_X2_PATP,				/// pos x
		CUR_POS_FIX( 3 ),		/// pos y
		ePAD_PG_ANIME,			/// ↑
		ePAD_EDIT_01_POS + 1,	/// ↓
		ePAD_EDIT_01_WAIT + 0,		/// ←
		ePAD_EDIT_01 + 0,		/// →
		PAD_PAT_AnmPos_Main,		/// [ A ]
	},
	///< パターンアニメ ポジション　２
	{
		CUR_POS_X2_PATP,
		CUR_POS_FIX( 4 ),
		ePAD_EDIT_01_POS + 0,
		ePAD_EDIT_01_POS + 2,
		ePAD_EDIT_01_WAIT + 1,
		ePAD_EDIT_01 + 1,
		PAD_PAT_AnmPos_Main,			
	},
	///< パターンアニメ ポジション　３
	{
		CUR_POS_X2_PATP,
		CUR_POS_FIX( 5 ),
		ePAD_EDIT_01_POS + 1,
		ePAD_EDIT_01_POS + 3,
		ePAD_EDIT_01_WAIT + 2,
		ePAD_EDIT_01 + 2,
		PAD_PAT_AnmPos_Main,			
	},
	///< パターンアニメ ポジション　４
	{
		CUR_POS_X2_PATP,
		CUR_POS_FIX( 6 ),
		ePAD_EDIT_01_POS + 2,
		ePAD_EDIT_01_POS + 4,
		ePAD_EDIT_01_WAIT + 3,
		ePAD_EDIT_01 + 3,
		PAD_PAT_AnmPos_Main,			
	},
	///< パターンアニメ ポジション　５
	{
		CUR_POS_X2_PATP,
		CUR_POS_FIX( 7 ),
		ePAD_EDIT_01_POS + 3,
		ePAD_EDIT_01_POS + 5,
		ePAD_EDIT_01_WAIT + 4,
		ePAD_EDIT_01 + 4,
		PAD_PAT_AnmPos_Main,			
	},
	///< パターンアニメ ポジション　６
	{
		CUR_POS_X2_PATP,
		CUR_POS_FIX( 8 ),
		ePAD_EDIT_01_POS + 4,
		ePAD_EDIT_01_POS + 6,
		ePAD_EDIT_01_WAIT + 5,
		ePAD_EDIT_01 + 5,
		PAD_PAT_AnmPos_Main,			
	},
	///< パターンアニメ ポジション　７
	{
		CUR_POS_X2_PATP,
		CUR_POS_FIX( 9 ),
		ePAD_EDIT_01_POS + 5,
		ePAD_EDIT_01_POS + 7,
		ePAD_EDIT_01_WAIT + 6,
		ePAD_EDIT_01 + 6,
		PAD_PAT_AnmPos_Main,			
	},
	///< パターンアニメ ポジション　８
	{
		CUR_POS_X2_PATP,
		CUR_POS_FIX( 10 ),
		ePAD_EDIT_01_POS + 6,
		ePAD_EDIT_01_POS + 8,
		ePAD_EDIT_01_WAIT + 7,
		ePAD_EDIT_01 + 7,
		PAD_PAT_AnmPos_Main,			
	},
	///< パターンアニメ ポジション　９
	{
		CUR_POS_X2_PATP,
		CUR_POS_FIX( 11 ),
		ePAD_EDIT_01_POS + 7,
		ePAD_EDIT_01_POS + 9,
		ePAD_EDIT_01_WAIT + 8,
		ePAD_EDIT_01 + 8,
		PAD_PAT_AnmPos_Main,			
	},
	///< パターンアニメ ポジション　１０
	{
		CUR_POS_X2_PATP,
		CUR_POS_FIX( 12 ),
		ePAD_EDIT_01_POS + 8,
		ePAD_FB_CHANGE,
		ePAD_EDIT_01_WAIT + 9,
		ePAD_EDIT_01 + 9,
		PAD_PAT_AnmPos_Main,			
	},
};

#define	NORMAL_LETTER	(1)
#define	NORMAL_SHADOW	(2)
#define	NORMAL_GROUND	(15)

#define	ACTIVE_LETTER	(3)
#define	ACTIVE_SHADOW	(2)
#define	ACTIVE_GROUND	(15)

#define	CURSOR_LETTER	(4)
#define	CURSOR_SHADOW	(2)
#define	CURSOR_GROUND	(15)

#define	NONE_LETTER		(5)
#define	NONE_SHADOW		(2)
#define	NONE_GROUND		(15)

#define	COL_NORMAL		(GF_PRINTCOLOR_MAKE(NORMAL_LETTER,NORMAL_SHADOW,NORMAL_GROUND))
#define	COL_ACTIVE		(GF_PRINTCOLOR_MAKE(ACTIVE_LETTER,ACTIVE_SHADOW,ACTIVE_GROUND))
#define	COL_CURSOR		(GF_PRINTCOLOR_MAKE(CURSOR_LETTER,CURSOR_SHADOW,CURSOR_GROUND))
#define	COL_NONE		(GF_PRINTCOLOR_MAKE(NONE_LETTER,NONE_SHADOW,NONE_GROUND))

///< ポケモンプログラムアニメデータ復元
static void RC_PPAD( u16 mons_no, u8* patno, u8* wait )
{
	POKE_ANM_TABLE_DP		pat;

	ArchiveDataLoadOfs(&pat, ARC_POKE_ANM_TBL, 0, mons_no*sizeof(POKE_ANM_TABLE_DP), sizeof(POKE_ANM_TABLE_DP));

	*patno = pat.poke_f.patno;
	*wait  = pat.poke_f.wait;
}

//==================================================================================================
//
//	関数
//
//==================================================================================================

static	void	PokeGraInit(POKE_GRA_TEST *pgt)
{
	int				i,j;

#ifdef SAVE_BANK_LOAD	
	pgt->save_bank = EDIT_BANK;
	pgt->load_bank = SAVE_BANK;
	OS_Printf( " load = SAVE_BANK  save = EDIT_BANK\n" );
	OS_Printf( " load = 0x%08x save = 0x%08x\n", pgt->load_bank, pgt->save_bank );
#else
	pgt->save_bank = EDIT_BANK;
	pgt->load_bank = EDIT_BANK;
	OS_Printf( " load = EDIT_BANK  save = EDIT_BANK\n" );
	OS_Printf( " load = 0x%08x save = 0x%08x\n", pgt->load_bank, pgt->save_bank );
#endif

	simple_3DBGInit(HEAPID_BATTLE);
	sys_HBlankIntrStop();

	MI_CpuClearFast(pgt,sizeof(POKE_GRA_TEST));

	pgt->man_msg=MSGMAN_Create(MSGMAN_TYPE_NORMAL,ARC_MSG,NARC_msg_poke_test_dat,HEAPID_BATTLE);
	pgt->man_pam=MSGMAN_Create(MSGMAN_TYPE_NORMAL,ARC_MSG,NARC_msg_poke_anime_dat,HEAPID_BATTLE);
	pgt->man_poke=MSGMAN_Create(MSGMAN_TYPE_NORMAL,ARC_MSG,NARC_msg_monsname_dat,HEAPID_BATTLE);
	pgt->wordset = WORDSET_Create( HEAPID_BATTLE );
	pgt->msg_buf=STRBUF_Create(0x100,HEAPID_BATTLE);
	pgt->msg_buf2=STRBUF_Create(0x100,HEAPID_BATTLE);

	pgt->put_req=0;
	pgt->anm_pat=0;
	pgt->form_no=0;

	pgt->end_flag = FALSE;
	pgt->bgl=GF_BGL_BglIniAlloc(HEAPID_BATTLE);
	pgt->win_m=GF_BGL_BmpWinAllocGet(HEAPID_BATTLE,1);
	pgt->win_s=GF_BGL_BmpWinAllocGet(HEAPID_BATTLE,1);

	PokeGraTestBGCreate(pgt,pgt->bgl);

	pgt->ssm=SoftSpriteInit(HEAPID_BATTLE);
	SoftSpritePaletteTransParamSet(pgt->ssm,0,SOFT_SPRITE_COL_SIZE);	//影の分も転送するようにサイズを変更
	pgt->pasp=PokeAnm_AllocMemory(HEAPID_BATTLE,2,0);

	pgt->mons_no=MONSNO_NAETORU;
	pgt->sex=PokeGraTestSexGet(pgt->mons_no);
	if(pgt->sex==3){
		pgt->sex=0;
	}
	pgt->main_pos=0;
	pgt->pos=0;
	pgt->seq_no=SEQ_POKEMON_CHECK_INIT;
	pgt->seq_no_temp=0;
	pgt->sub_seq_no=0;
	pgt->bg_flag=0;
	pgt->col_flag=0;
	pgt->bg_r=0;
	pgt->bg_g=0;
	pgt->bg_b=0;

	GF_BGL_BmpWinAdd(pgt->bgl,pgt->win_m,GF_BGL_FRAME1_M,
					 TALK_WIN_X,TALK_WIN_Y,TALK_WIN_SIZE_X,TALK_WIN_SIZE_Y,TALK_MSG_PAL_NO,TALK_MSG_START);
	GF_BGL_BmpWinDataFill(pgt->win_m,0xff);
	BmpTalkWinWrite(pgt->win_m,WINDOW_TRANS_ON,TALK_WIN_CGX_START,TALK_WIN_PAL_NO);
	GF_BGL_BmpWinAdd(pgt->bgl,pgt->win_s,GF_BGL_FRAME1_S,0x00,0x00,32,24,0x00,0x01);

	for(i = 0; i < PGT_POKE_MAX; i++){
		
		pgt->paed[ i ].poke_anime[ 0 ].voice_wait	= 0;
		pgt->paed[ i ].poke_anime[ 0 ].prg_anm		= 0;
		pgt->paed[ i ].poke_anime[ 0 ].prg_anm_wait	= 0;
		
		pgt->paed[ i ].poke_anime[ 1 ].voice_wait	= 0;
		pgt->paed[ i ].poke_anime[ 1 ].prg_anm		= 0;
		pgt->paed[ i ].poke_anime[ 1 ].prg_anm_wait	= 0;
		
		for ( j = 0; j < SS_ANM_SEQ_MAX; j++ ){
			
			pgt->paed[ i ].poke_anime[ 0 ].ssanm[ j ].patno = -1;
			pgt->paed[ i ].poke_anime[ 0 ].ssanm[ j ].wait	= 0;
			pgt->paed[ i ].poke_anime[ 0 ].ssanm[ j ].ox	= 0;
			pgt->paed[ i ].poke_anime[ 0 ].ssanm[ j ].oy	= 0;
			
			pgt->paed[ i ].poke_anime[ 1 ].ssanm[ j ].patno = -1;
			pgt->paed[ i ].poke_anime[ 1 ].ssanm[ j ].wait	= 0;
			pgt->paed[ i ].poke_anime[ 1 ].ssanm[ j ].ox	= 0;
			pgt->paed[ i ].poke_anime[ 1 ].ssanm[ j ].oy	= 0;
		}
				
		pgt->paed[ i ].height		= 0;
		pgt->paed[ i ].shadow_size	= 1;
		pgt->paed[ i ].shadow_ofx	= 0;
	}

	pgt->paed[ 0 ].height = 0x55;

	
	if ( sys.cont & PAD_BUTTON_START
	&&	 sys.cont & PAD_BUTTON_SELECT )
	{		
		s8	ox;
		s8	oy;
		u8	size;
		
		int index;
		
		POKE_ANM_DEBUG_PRINT( "\n ----- DP データの 読み込み開始 ----- \n" );
		POKE_ANM_DEBUG_PRINT( "\n");
		POKE_ANM_DEBUG_PRINT( " ----- DP データの 読み込み中･･･ ----- \n" );
		POKE_ANM_DEBUG_PRINT( "\n");

		//PGT_POKE_MAX
		for ( i = 0; i < 494; i++ ){
			
			///< 適正なポケモンNoに配置する必要がある
			index = PokeGraNoTable[ i ];
			
			// POKE_ANM_DEBUG_PRINT( " i = %3d  index = %3d \n", i, index );
						
			pgt->paed[ index ].poke_anime[ 0 ].voice_wait	= 0;
			pgt->paed[ index ].poke_anime[ 0 ].prg_anm		= 0;
			pgt->paed[ index ].poke_anime[ 0 ].prg_anm_wait	= 0;
			
			pgt->paed[ index ].poke_anime[ 1 ].voice_wait	= 0;
			pgt->paed[ index ].poke_anime[ 1 ].prg_anm		= 0;
			pgt->paed[ index ].poke_anime[ 1 ].prg_anm_wait	= 0;
						
			for ( j = 0; j < SS_ANM_SEQ_MAX; j++ ){
				
				pgt->paed[ index ].poke_anime[ 0 ].ssanm[ j ].patno = -1;
				pgt->paed[ index ].poke_anime[ 0 ].ssanm[ j ].wait	= 0;
				pgt->paed[ index ].poke_anime[ 0 ].ssanm[ j ].ox	= 0;
				pgt->paed[ index ].poke_anime[ 0 ].ssanm[ j ].oy	= 0;
				
				pgt->paed[ index ].poke_anime[ 1 ].ssanm[ j ].patno = -1;
				pgt->paed[ index ].poke_anime[ 1 ].ssanm[ j ].wait	= 0;
				pgt->paed[ index ].poke_anime[ 1 ].ssanm[ j ].ox	= 0;
				pgt->paed[ index ].poke_anime[ 1 ].ssanm[ j ].oy	= 0;
			}

			///< 影サイズ 影高さ 高さ情報
			ArchiveDataLoadOfs( &ox,	ARC_POKE_SHADOW_OFX, 0, i, 1);
			ArchiveDataLoadOfs( &oy,	ARC_POKE_YOFS,		 0, i, 1);
			ArchiveDataLoadOfs( &size,	ARC_POKE_SHADOW,	 0, i, 1);
			pgt->paed[ index ].height		= oy;
			pgt->paed[ index ].shadow_size	= size;
			pgt->paed[ index ].shadow_ofx	= ox;
			
			///< アニメデータ復元
			{
				SOFT_SPRITE_ANIME_DP ssa[10];
				
				PokeAnmDataSetDP( &ssa[0], i );
				
				for ( j = 0; j < SS_ANM_SEQ_MAX; j++ ){
					
					pgt->paed[ index ].poke_anime[ 0 ].ssanm[ j ].patno = ssa[ j ].patno;
					pgt->paed[ index ].poke_anime[ 0 ].ssanm[ j ].wait	= ssa[ j ].wait;
					pgt->paed[ index ].poke_anime[ 0 ].ssanm[ j ].ox	= 0;
					pgt->paed[ index ].poke_anime[ 0 ].ssanm[ j ].oy	= 0;
		
				}
			}
			
			///< プログラムアニメデータ復元
			{
				u8 patno;
				u8 wait;
				
				RC_PPAD( i, &patno, &wait );
				
				pgt->paed[ index ].poke_anime[ 0 ].prg_anm		= patno;
				pgt->paed[ index ].poke_anime[ 0 ].prg_anm_wait	= wait;
			}
		}
		
		POKE_ANM_DEBUG_PRINT( "\n ----- DP データの 読み込み終了 ----- \n" );
	}
#if 0
	if ( !(sys.cont & ( PAD_BUTTON_R | PAD_BUTTON_L )) ){
		
		PMSVLD_Load(EDIT_BANK, &pgt->paed[0], POKE_ANM_SAVE_SIZE);
		
		if(pgt->paed[0].height!=0x55){
			for(i=0;i<PGT_POKE_MAX;i++){
				pgt->paed[i].height=0;
			}
			pgt->paed[0].height=0x55;
		}
		for(i=0;i<PGT_POKE_MAX;i++){
			if(pgt->paed[i].shadow_size>3){
				pgt->paed[i].shadow_size=1;
			}
		}
		if(pgt->paed[0].shadow_ofx!=0x55){
			for(i=0;i<PGT_POKE_MAX;i++){
				pgt->paed[i].shadow_ofx=0;
			}
			pgt->paed[0].shadow_ofx=0x55;
		}
		POKE_ANM_DEBUG_PRINT( "\n ----- データ自動ロード ----- \n" );
	}
#endif
/*
	最初からした画面
	すたーと前面
	せれくと背面
	ひこざる
	さゆう1個
	LR10個
*/

	{
		SOFT_SPRITE_ARC	ssa;
		int				height;

		PokeGraArcDataGet(&ssa,pgt->mons_no,pgt->sex,PARA_BACK,pgt->col,NULL,0x88888888);
		height=PokeHeightGet(pgt->mons_no,pgt->sex,PARA_BACK,NULL,NULL);
		pgt->ss[0]=SoftSpriteAdd(pgt->ssm,&ssa,APPEAR_X_TYPE_AA,APPEAR_Y_TYPE_AA+height,APPEAR_Z_TYPE_AA,
								 0,NULL,NULL);
		PokeGraArcDataGet(&ssa,pgt->mons_no,pgt->sex,PARA_FRONT,pgt->col,NULL,0x88888888);
		height=PokeHeightGet(pgt->mons_no,pgt->sex,PARA_FRONT,NULL,NULL);
		pgt->ss[1]=SoftSpriteAdd(pgt->ssm,&ssa,APPEAR_X_TYPE_BB,APPEAR_Y_TYPE_BB+height,APPEAR_Z_TYPE_BB,
								 1,NULL,NULL);
		SoftSpriteParaSet(pgt->ss[1],SS_PARA_SHADOW_REQ,1);
		SoftSpriteParaSet(pgt->ss[1],SS_PARA_SHADOW_SIZE,pgt->paed[PokeGraNoTable[pgt->mons_no]].shadow_size);
		SoftSpriteParaSet(pgt->ss[1],SS_PARA_SHADOW_OX,pgt->paed[PokeGraNoTable[pgt->mons_no]].shadow_ofx);
		SoftSpriteParaSet(pgt->ss[1],SS_PARA_SHADOW_OY,SHADOW_OFS-height);
	}
	pgt->is_main_page = FALSE;
	pgt->pos=0;
	pgt->seq_no=SEQ_POKEMON_ANIME;
	PokeGraTestSubScreenMake(pgt);
	CursorMove(pgt,SIDE_SUB,pgt->pos,CUR_PUT);
	
//	PokeGraTestMSG_Print(pgt);

	PokeTestMainScreenCreate(pgt);
	
	CursorMove(pgt,SIDE_MAIN,pgt->main_pos,CUR_PUT);

	PokeGraTestSubScreenMake(pgt);
	Snd_Stop();

	POKE_ANM_DEBUG_PRINT( "\n --- save_ram : 0x%08x size : 0x%08x\n",&pgt->paed[ 0 ], POKE_ANM_SAVE_SIZE );
}

static	void	PokeTestMainScreenCreate(POKE_GRA_TEST *pgt)
{
	PokeTestPokeNumDataPut(pgt,MODE_NORMAL);
	PokeTestPokeSexDataPut(pgt);
	PokeTestPokeColorDataPut(pgt);
	PokeTestPokePatDataPut(pgt);
	PokeTestPokeFormDataPut(pgt,MODE_NORMAL);
	PokeTestPokeHeightDataPut(pgt,COL_NORMAL);
	PokeTestPokeOfxDataPut(pgt,COL_NORMAL);
	PokeTestPokeShadowDataPut(pgt,COL_NORMAL);
	PokeTestPokeBGColorRDataPut(pgt,MODE_NORMAL);
	PokeTestPokeBGColorGDataPut(pgt,MODE_NORMAL);
	PokeTestPokeBGColorBDataPut(pgt,MODE_NORMAL);
}

//============================================================================================
/**
 *	戦闘背景画面初期化＆画面生成
 */
//============================================================================================
static	void	PokeGraTestBGCreate(POKE_GRA_TEST *pgt,GF_BGL_INI *bgl)
{
	GF_Disp_GX_VisibleControlInit();

	//VRAM設定
	{
		GF_BGL_DISPVRAM vramSetTable = {
			GX_VRAM_BG_128_A,				// メイン2DエンジンのBG
			GX_VRAM_BGEXTPLTT_NONE,			// メイン2DエンジンのBG拡張パレット
			GX_VRAM_SUB_BG_32_H,			// サブ2DエンジンのBG
			GX_VRAM_SUB_BGEXTPLTT_NONE,		// サブ2DエンジンのBG拡張パレット
			GX_VRAM_OBJ_64_E,				// メイン2DエンジンのOBJ
			GX_VRAM_OBJEXTPLTT_NONE,		// メイン2DエンジンのOBJ拡張パレット
			GX_VRAM_SUB_OBJ_16_I,			// サブ2DエンジンのOBJ
			GX_VRAM_SUB_OBJEXTPLTT_NONE,	// サブ2DエンジンのOBJ拡張パレット
			GX_VRAM_TEX_01_BC,				// テクスチャイメージスロット
			GX_VRAM_TEXPLTT_01_FG			// テクスチャパレットスロット
		};
		GF_Disp_SetBank( &vramSetTable );
		MI_CpuClear32((void*)HW_BG_VRAM, HW_BG_VRAM_SIZE);
		MI_CpuClear32((void*)HW_DB_BG_VRAM, HW_DB_BG_VRAM_SIZE);
		MI_CpuClear32((void*)HW_OBJ_VRAM, HW_OBJ_VRAM_SIZE);
		MI_CpuClear32((void*)HW_DB_OBJ_VRAM, HW_DB_OBJ_VRAM_SIZE);
	}

	// BG SYSTEM
	{
		GF_BGL_SYS_HEADER BGsys_data = {
			GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_3D,
		};
		GF_BGL_InitBG( &BGsys_data );
	}

	//メイン画面フレーム設定
	{
		GF_BGL_BGCNT_HEADER TextBgCntDat[] = {
			///<FRAME1_M
			{
				0, 0, 0x0800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GX_BG_EXTPLTT_01,
				0, 0, 0, FALSE
			},
			///<FRAME2_M
			{
				0, 0, 0x2000, 0, GF_BGL_SCRSIZ_512x512, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x0c000, GX_BG_EXTPLTT_01,
				1, 0, 0, FALSE
			},
			///<FRAME3_M
			{
				0, 0, 0x1000, 0, GF_BGL_SCRSIZ_512x256, GX_BG_COLORMODE_256,
				GX_BG_SCRBASE_0x3000, GX_BG_CHARBASE_0x10000, GX_BG_EXTPLTT_01,
				3, 0, 0, FALSE
			},
		};
		GF_BGL_BGControlSet(bgl, GF_BGL_FRAME1_M, &TextBgCntDat[0], GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear(bgl, GF_BGL_FRAME1_M );
		GF_BGL_BGControlSet(bgl, GF_BGL_FRAME2_M, &TextBgCntDat[1], GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear(bgl, GF_BGL_FRAME2_M );
		GF_BGL_BGControlSet(bgl, GF_BGL_FRAME3_M, &TextBgCntDat[2], GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear(bgl, GF_BGL_FRAME3_M );

		G2_SetBG0Priority(0x01);
		GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
	}
	//サブ画面フレーム設定
	{
		GF_BGL_BGCNT_HEADER TextBgCntDat[]={
			///<FRAME1_S
			{
				0,0,0x800,0,GF_BGL_SCRSIZ_256x256,GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x7800,GX_BG_CHARBASE_0x00000,GX_BG_EXTPLTT_01,
				0,0,0,FALSE
			},
		};
		GF_BGL_BGControlSet(bgl, GF_BGL_FRAME1_S, &TextBgCntDat[0], GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear(bgl, GF_BGL_FRAME1_S );
	}

	//背景グラフィックデータロード
	{
		TalkWinGraphicNullSet(bgl, GF_BGL_FRAME1_M,TALK_WIN_CGX_START,TALK_WIN_PAL_NO,0,HEAPID_BATTLE);
		ArcUtil_BgCharSet(ARC_BATT_BG,BATTLE_BG00_NCGR_BIN,bgl,GF_BGL_FRAME3_M,0,0,1,HEAPID_BATTLE);
		ArcUtil_PalSet(ARC_BATT_BG,BATT_BG00_D_NCLR,PALTYPE_MAIN_BG,0,0,HEAPID_BATTLE);
		ArcUtil_ScrnSet(ARC_BATT_BG,BATTLE_BG00_NSCR_BIN,bgl,GF_BGL_FRAME3_M,0,0,1,HEAPID_BATTLE);
		ArcUtil_PalSet(ARC_WINFRAME,TalkWinPalArcGet(0),PALTYPE_MAIN_BG,TALK_WIN_PAL_NO*0x20,0x20,HEAPID_BATTLE);
//		ArcUtil_PalSet(ARC_FONT,NARC_font_talk_ncrl,PALTYPE_MAIN_BG,TALK_MSG_PAL_NO*0x20,0x20,HEAPID_BATTLE);
//
		DC_FlushRange((void *)&pal,sizeof(pal));
		GX_LoadBGPltt((void *)&pal,TALK_MSG_PAL_NO*0x20,sizeof(pal));

		DC_FlushRange((void *)&pal,sizeof(pal));
		GXS_LoadBGPltt((void *)&pal,0,sizeof(pal));
	}

	GF_Disp_DispOn();

	sys_VBlankFuncChange(PokeGraTestVBlank,pgt);
}

//============================================================================================
/**
 *	メインループタスク
 */
//============================================================================================
static	void	PokeGraTestMain(POKE_GRA_TEST * pgt)
{
	u8	sex;
	int	move=NO_MOVE;

	if(pgt->put_req){
		pgt->put_req=0;
		GF_BGL_BmpWinOn(pgt->win_m);
	//	GF_BGL_BmpWinOn(pgt->win_s);
	}

	if((sys.trg == PAD_BUTTON_Y)&&(pgt->seq_no<SEQ_LOAD)&&(pgt->seq_no!=SEQ_POKEMON_CHECK_EDIT)){
		
		PokeSexAct(pgt);
		/*
#ifdef GO_USE
		Snd_Stop();
#else
		pgt->seq_no_temp=pgt->seq_no;
		pgt->seq_no=SEQ_LOAD;
		pgt->sub_seq_no=0;
		pgt->yes_no_pos=1;
#endif
		*/
	}

	if((sys.trg == PAD_BUTTON_X)&&(pgt->seq_no<SEQ_LOAD)&&(pgt->seq_no!=SEQ_POKEMON_CHECK_EDIT)){
		PokeColorAct(pgt);
		/*
#ifdef GO_USE
		Snd_SceneSet(SND_SCENE_DUMMY);
		Snd_DataSetByScene( SND_SCENE_BATTLE, SEQ_FIGHT0101, 1 );	//バトル曲再生
#else
		pgt->seq_no_temp=pgt->seq_no;
		pgt->seq_no=SEQ_SAVE;
		pgt->sub_seq_no=0;
		pgt->yes_no_pos=1;
#endif
		*/
	}

	if((pgt->seq_no!=SEQ_POKEMON_ANIME)&&
	   (pgt->seq_no!=SEQ_POKEMON_ANIME_EDIT)){
		if(sys.trg == PAD_BUTTON_B
		&& pgt->is_main_page == TRUE){
			pgt->shadow_flag++;
			switch(pgt->shadow_flag%3){
			case 0:
				PokeTestPokeHeightDataPut(pgt,COL_NORMAL);
				PokeTestPokeOfxDataPut(pgt,COL_NORMAL);
				PokeTestPokeShadowDataPut(pgt,COL_NORMAL);
				break;
			case 1:
				SoftSpriteSSSystemFlagSet(pgt->ssm,SS_SYSTEM_FLAG_SHADOW_OFF);
				break;
			case 2:
				PokeTestPokeHeightDataPut(pgt,COL_NONE);
				PokeTestPokeOfxDataPut(pgt,COL_NONE);
				PokeTestPokeShadowDataPut(pgt,COL_NONE);
				SoftSpriteSSSystemFlagReset(pgt->ssm,SS_SYSTEM_FLAG_SHADOW_OFF);
				break;
			}
			PokeGraTestDebugPrint(pgt,VOICE_OFF);
		}
		if( sys.trg == PAD_BUTTON_L && pgt->is_anime == FALSE ){
#ifdef GO_USE
			if((pgt->seq_no!=SEQ_POKEMON_ANIME)&&
			   (pgt->seq_no!=SEQ_POKEMON_ANIME_ACT)&&
			   (pgt->seq_no!=SEQ_POKEMON_ANIME_ACT_B)&&
			   (pgt->seq_no!=SEQ_POKEMON_ANIME_ACT_F)&&
			   (pgt->seq_no!=SEQ_POKEMON_ANIME_EDIT)){
				PokePrgAnmDataSet(pgt->pasp,pgt->ss[0],pgt->mons_no,PARA_BACK,0,PARA_HFLIP_OFF,0);
				Snd_PMVoicePlayEx(PV_NORMAL,pgt->mons_no,-117,127,HEAPID_BASE_APP, pgt->form_no );
				pgt->seq_no_temp=pgt->seq_no;
				pgt->seq_no=SEQ_POKEMON_ANIME_ACT_B;
			}
#else
			pgt->bg_flag++;
			switch(pgt->bg_flag&3){
			case 0:
				GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_ON );
				GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );
				break;
			case 1:
				GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_ON );
				GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_OFF );
				break;
			case 2:
				GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_OFF );
				GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );
				break;
			case 3:
				GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_OFF );
				GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_OFF );
				break;
			}
#endif
		}
		if(sys.trg == PAD_BUTTON_R){
#ifdef GO_USE
			if((pgt->seq_no!=SEQ_POKEMON_ANIME)&&
			   (pgt->seq_no!=SEQ_POKEMON_ANIME_ACT)&&
			   (pgt->seq_no!=SEQ_POKEMON_ANIME_ACT_B)&&
			   (pgt->seq_no!=SEQ_POKEMON_ANIME_ACT_F)&&
			   (pgt->seq_no!=SEQ_POKEMON_ANIME_EDIT)){
				{
					SOFT_SPRITE_ANIME	ssa[10];
	
					PokePrgAnmDataSet(pgt->pasp,pgt->ss[1],pgt->mons_no,PARA_FRONT,0,PARA_HFLIP_OFF,1);
					Snd_PMVoicePlayEx(PV_NORMAL,pgt->mons_no,117,127,HEAPID_BASE_APP,pgt->form_no);
	
				//	PokeAnmDataSet(&ssa[0],pgt->mons_no);
					SoftSpriteAnimeDataSet(pgt->ss[1],&ssa[0]);
					SoftSpriteAnimeSet(pgt->ss[1],NULL);
					SoftSpriteParaSet(pgt->ss[1],SS_PARA_SHADOW_Y_REQ,0);
					SoftSpriteParaSet(pgt->ss[1],SS_PARA_SHADOW_AFF_REQ,0);
				}
				pgt->seq_no_temp=pgt->seq_no;
				pgt->seq_no=SEQ_POKEMON_ANIME_ACT_F;
			}
#else
			pgt->col_flag^=1;
			PokeTestBGColorSet(pgt);
#endif
		}
	}

	if(pgt->col_flag&1){
		PokeTestBGColorSet(pgt);
	}

	switch(pgt->seq_no){
	case SEQ_POKEMON_CHECK_INIT:
		{
			SOFT_SPRITE_ARC	ssa;
			int				height;

			PokeGraArcDataGet(&ssa,pgt->mons_no,pgt->sex,PARA_BACK,pgt->col,NULL,0x88888888);
			height=PokeHeightGet(pgt->mons_no,pgt->sex,PARA_BACK,NULL,NULL);
			pgt->ss[0]=SoftSpriteAdd(pgt->ssm,&ssa,APPEAR_X_TYPE_AA,APPEAR_Y_TYPE_AA+height,APPEAR_Z_TYPE_AA,
									 0,NULL,NULL);
			PokeGraArcDataGet(&ssa,pgt->mons_no,pgt->sex,PARA_FRONT,pgt->col,NULL,0x88888888);
			height=PokeHeightGet(pgt->mons_no,pgt->sex,PARA_FRONT,NULL,NULL);
			pgt->ss[1]=SoftSpriteAdd(pgt->ssm,&ssa,APPEAR_X_TYPE_BB,APPEAR_Y_TYPE_BB+height,APPEAR_Z_TYPE_BB,
									 1,NULL,NULL);
			SoftSpriteParaSet(pgt->ss[1],SS_PARA_SHADOW_REQ,1);
			SoftSpriteParaSet(pgt->ss[1],SS_PARA_SHADOW_SIZE,pgt->paed[PokeGraNoTable[pgt->mons_no]].shadow_size);
			SoftSpriteParaSet(pgt->ss[1],SS_PARA_SHADOW_OX,pgt->paed[PokeGraNoTable[pgt->mons_no]].shadow_ofx);
			SoftSpriteParaSet(pgt->ss[1],SS_PARA_SHADOW_OY,SHADOW_OFS-height);
		}
		pgt->seq_no=SEQ_POKEMON_CHECK;
		break;
	//ポケモンチェック
	case SEQ_POKEMON_CHECK:
		if(sys.trg & PAD_BUTTON_SELECT){
			pgt->is_main_page = FALSE;
			pgt->pos=0;
			pgt->seq_no=SEQ_POKEMON_ANIME;
			PokeGraTestSubScreenMake(pgt);
			CursorMove(pgt,SIDE_SUB,pgt->pos,CUR_PUT);
			break;
		}
		if(sys.trg==PAD_KEY_UP){
			move=mlp_m[pgt->main_pos].move_up;
		}
		else if(sys.trg == PAD_KEY_DOWN){
			move=mlp_m[pgt->main_pos].move_down;
		}
		else if(sys.trg == PAD_KEY_LEFT){
			move=mlp_m[pgt->main_pos].move_left;
		}
		else if(sys.trg == PAD_KEY_RIGHT){
			move=mlp_m[pgt->main_pos].move_right;
		}
		if(sys.trg == PAD_BUTTON_A){
		/*
			if(mlp_m[pgt->main_pos].a_button_act!=NULL){
				pgt->sub_seq_no=0;
				pgt->seq_no=SEQ_POKEMON_CHECK_EDIT;
			}
		*/
		}
		if(move!=NO_MOVE){
			CursorMove(pgt,SIDE_MAIN,move,CUR_PUT);
		}
		break;
	case SEQ_POKEMON_CHECK_EDIT:
		if(mlp_m[pgt->main_pos].a_button_act(pgt)==TRUE){
			pgt->seq_no=SEQ_POKEMON_CHECK;
		}
		break;
	//ポケモンアニメセット
	case SEQ_POKEMON_ANIME:
		if(sys.repeat == PAD_KEY_UP){
			move=mlp[pgt->pos].move_up;
		}
		else if(sys.repeat == PAD_KEY_DOWN){
			move=mlp[pgt->pos].move_down;
		}
		else if(sys.repeat == PAD_KEY_LEFT){
			move=mlp[pgt->pos].move_left;
		}
		else if(sys.repeat == PAD_KEY_RIGHT){
			move=mlp[pgt->pos].move_right;
		}
		#if 0
		if(sys.trg == PAD_BUTTON_SELECT){
			pgt->is_main_page = TRUE;
			CursorMove(pgt,SIDE_SUB,pgt->pos,CUR_DEL);
			pgt->seq_no=SEQ_POKEMON_CHECK;
		}
		#endif
		
		if( sys.trg == PAD_BUTTON_SELECT ){
			POKE_ANIME_DATA pad;
			ARCHANDLE* handle;
			int dir;
			int client_type;
			u8	wait;
			int	num = pgt->sub_page ^ 1;
			SOFT_SPRITE_ANIME ssanm[10];
			
			pgt->is_anime = TRUE;
			
			dir = PARA_BACK;
			client_type = CLIENT_TYPE_AA;
			pgt->sub_page = 0;
			num = pgt->sub_page;
			
			handle = ArchiveDataHandleOpen( ARC_PM_EDIT, HEAPID_BASE_APP );
			
			PokeEditData_AnimeSet( handle, &ssanm[0], pgt->mons_no, client_type );
			SoftSpriteAnimeDataSet(pgt->ss[num], &ssanm[0]);
			SoftSpriteAnimeSet(pgt->ss[num], NULL);
			
			PokeEditData_PrgAnimeSet( handle, pgt->pasp, pgt->ss[num], pgt->mons_no, dir, PARA_HFLIP_OFF, client_type );
			
			SoftSpriteParaSet(pgt->ss[1], SS_PARA_SHADOW_Y_REQ, 0);
			SoftSpriteParaSet(pgt->ss[1], SS_PARA_SHADOW_AFF_REQ, 0);
			
			PokeEditData_VoiceWaitSet( handle, &wait, pgt->mons_no, client_type );
			
			ArchiveDataHandleClose( handle );
			
			Snd_PMVoiceWaitPlayEx(PV_NORMAL, pgt->mons_no, 117, 127, HEAPID_BASE_APP, wait, pgt->form_no );

			pgt->seq_no_temp=pgt->seq_no;
			pgt->seq_no=SEQ_POKEMON_ANIME_ACT_B+num;
		}			
		if ( sys.trg & PAD_BUTTON_START ){
			int dir;
			int client_type;
			pgt->is_anime = TRUE;
			
			dir = PARA_FRONT;
			client_type = CLIENT_TYPE_BB;
			pgt->sub_page = 1;
			
			if( sys.cont & PAD_BUTTON_B )
			{	
			}
			else {
				P_ANM_SETTING_PARAM	pas_p;
				int					num = 0;
				u8					wait;
				ARCHANDLE*			handle;
				SOFT_SPRITE_ANIME ssanm[10];
				
				pas_p.AnimeNo	= pgt->paed[ PokeGraNoTable[pgt->mons_no] ].poke_anime[ pgt->sub_page ].prg_anm;
				pas_p.Wait		= pgt->paed[ PokeGraNoTable[pgt->mons_no] ].poke_anime[ pgt->sub_page ].prg_anm_wait;
				pas_p.Reverse	= 0;
				wait			= pgt->paed[ PokeGraNoTable[pgt->mons_no] ].poke_anime[ pgt->sub_page ].voice_wait;
				num				= pgt->sub_page ^ 1;
				num = pgt->sub_page;
				
				handle = ArchiveDataHandleOpen( ARC_PM_EDIT, HEAPID_BASE_APP );
				
				PokeEditData_AnimeSet( handle, &ssanm[0], pgt->mons_no, client_type );
				SoftSpriteAnimeDataSet(pgt->ss[num], &ssanm[0]);
				SoftSpriteAnimeSet(pgt->ss[num], NULL);
				
				PokeEditData_PrgAnimeSet( handle, pgt->pasp, pgt->ss[num], pgt->mons_no, dir, PARA_HFLIP_OFF, client_type );
				
				SoftSpriteParaSet(pgt->ss[1], SS_PARA_SHADOW_Y_REQ, 0);
				SoftSpriteParaSet(pgt->ss[1], SS_PARA_SHADOW_AFF_REQ, 0);
				
				PokeEditData_VoiceWaitSet( handle, &wait, pgt->mons_no, client_type );
				
				Snd_PMVoiceWaitPlayEx(PV_NORMAL, pgt->mons_no, 117, 127, HEAPID_BASE_APP, wait, pgt->form_no );
				
				ArchiveDataHandleClose( handle );
				#if 0
				SoftSpriteAnimeDataSet(pgt->ss[num], &pgt->paed[ PokeGraNoTable[pgt->mons_no] ].poke_anime[ pgt->sub_page ].ssanm[0]);
				SoftSpriteAnimeSet(pgt->ss[num], NULL);
				SoftSpriteParaSet(pgt->ss[1], SS_PARA_SHADOW_Y_REQ, 0);
				SoftSpriteParaSet(pgt->ss[1], SS_PARA_SHADOW_AFF_REQ, 0);
				
				PokeAnm_SetPokeAnime(pgt->pasp,pgt->ss[num],&pas_p,num);
				
				OS_Printf( " voice wait = %2d\n", wait );
				
				Snd_PMVoiceWaitPlayEx(PV_NORMAL, pgt->mons_no, 117, 127, HEAPID_BASE_APP, wait, pgt->form_no );
				#endif
				pgt->seq_no_temp=pgt->seq_no;
				pgt->seq_no=SEQ_POKEMON_ANIME_ACT_B+num;
			}
		}
		if(sys.trg == PAD_BUTTON_A){
		/*
			if(mlp[pgt->pos].a_button_act!=NULL){
				pgt->sub_seq_no=0;
				pgt->seq_no=SEQ_POKEMON_ANIME_EDIT;
			}
		*/
		}
		
		if(move!=NO_MOVE){
			CursorMove(pgt,SIDE_SUB,move,CUR_PUT);
		}
		
		if(sys.repeat & PAD_BUTTON_L){
			if(pgt->mons_no <= 10){
				pgt->mons_no=MONSNO_END;
			}
			else{
				pgt->mons_no-=10;
			}
			pgt->sex=PokeGraTestSexGet(pgt->mons_no);
			if(pgt->sex==3){
				pgt->sex=0;
			}
			PokeGraTestDebugPrint(pgt,VOICE_ON);
			PokeTestMainScreenCreate(pgt);
			PokeGraTestSubScreenMake(pgt);
			CursorMove(pgt,SIDE_SUB,pgt->pos,CUR_PUT);
		}
		else if( sys.repeat & PAD_BUTTON_R ){
			if(pgt->mons_no + 10 >= MONSNO_END){
				pgt->mons_no=1;
			}
			else{
				pgt->mons_no+=10;
			}
			pgt->sex=PokeGraTestSexGet(pgt->mons_no);
			if(pgt->sex==3){
				pgt->sex=0;
			}
			PokeGraTestDebugPrint(pgt,VOICE_ON);
			PokeTestMainScreenCreate(pgt);
			PokeGraTestSubScreenMake(pgt);
			CursorMove(pgt,SIDE_SUB,pgt->pos,CUR_PUT);
		}
		else if( sys.repeat & PAD_KEY_LEFT ){
			if(pgt->mons_no==1){
				pgt->mons_no=MONSNO_END;
			}
			else{
				pgt->mons_no--;
			}
			pgt->sex=PokeGraTestSexGet(pgt->mons_no);
			if(pgt->sex==3){
				pgt->sex=0;
			}
			PokeGraTestDebugPrint(pgt,VOICE_ON);
			PokeTestMainScreenCreate(pgt);
			PokeGraTestSubScreenMake(pgt);
			CursorMove(pgt,SIDE_SUB,pgt->pos,CUR_PUT);
		}
		else if( sys.repeat & PAD_KEY_RIGHT ){
			if(pgt->mons_no==MONSNO_END){
				pgt->mons_no=1;
			}
			else{
				pgt->mons_no++;
			}
			pgt->sex=PokeGraTestSexGet(pgt->mons_no);
			if(pgt->sex==3){
				pgt->sex=0;
			}
			PokeGraTestDebugPrint(pgt,VOICE_ON);
			PokeTestMainScreenCreate(pgt);
			PokeGraTestSubScreenMake(pgt);
			CursorMove(pgt,SIDE_SUB,pgt->pos,CUR_PUT);
		}
		break;
	case SEQ_POKEMON_ANIME_EDIT:
		if(mlp[pgt->pos].a_button_act(pgt)==TRUE){
			pgt->seq_no=SEQ_POKEMON_ANIME;
		}
		break;
	//ロード
	case SEQ_LOAD:
		if(PokeGraTestLoadAct(pgt)==TRUE){
			pgt->seq_no=pgt->seq_no_temp;
			if(pgt->seq_no){
				CursorMove(pgt,SIDE_SUB,pgt->pos,CUR_PUT);
			}
		}
		break;
	//セーブ
	case SEQ_SAVE:
		if(PokeGraTestSaveAct(pgt)==TRUE){
			pgt->seq_no=pgt->seq_no_temp;
			if(pgt->seq_no){
				CursorMove(pgt,SIDE_SUB,pgt->pos,CUR_PUT);
			}
		}
		break;
	//アニメ終了待ち
	case SEQ_POKEMON_ANIME_ACT:
		if((PokeAnm_IsFinished(pgt->pasp,0)==TRUE)&&
		   (PokeAnm_IsFinished(pgt->pasp,1)==TRUE)&&
		   (SoftSpriteAnimeEndCheck(pgt->ss[0])==FALSE)&&
		   (SoftSpriteAnimeEndCheck(pgt->ss[1])==FALSE)){
			SoftSpriteParaSet(pgt->ss[1],SS_PARA_SHADOW_Y_REQ,1);
			SoftSpriteParaSet(pgt->ss[1],SS_PARA_SHADOW_AFF_REQ,1);
			pgt->seq_no=pgt->seq_no_temp;
			pgt->is_anime = FALSE;
		}
		break;
	//アニメ終了待ち
	case SEQ_POKEMON_ANIME_ACT_B:
		if((PokeAnm_IsFinished(pgt->pasp,0)==TRUE)&&
		   (SoftSpriteAnimeEndCheck(pgt->ss[0])==FALSE)){
			pgt->seq_no=pgt->seq_no_temp;
			pgt->is_anime = FALSE;
		}
		break;
	//アニメ終了待ち
	case SEQ_POKEMON_ANIME_ACT_F:
		if((PokeAnm_IsFinished(pgt->pasp,1)==TRUE)&&
		   (SoftSpriteAnimeEndCheck(pgt->ss[1])==FALSE)){
			SoftSpriteParaSet(pgt->ss[1],SS_PARA_SHADOW_Y_REQ,1);
			SoftSpriteParaSet(pgt->ss[1],SS_PARA_SHADOW_AFF_REQ,1);
			pgt->seq_no=pgt->seq_no_temp;
			pgt->is_anime = FALSE;
		}
		break;
	}

	SoftSpriteMain(pgt->ssm);
	GF_G3_RequestSwapBuffers(GX_SORTMODE_MANUAL, GX_BUFFERMODE_Z);
}

//============================================================================================
/**
 *	メインループタスク
 */
//============================================================================================
static	void	PokeGraTestDebugPrint(POKE_GRA_TEST *pgt,int mode)
{
	int				height;
	SOFT_SPRITE_ARC	*ssa;

	ssa=SoftSpriteArcPointerGet(pgt->ssm,0);
	PokeGraArcDataGet(ssa,pgt->mons_no,pgt->sex,PARA_BACK,pgt->col,pgt->form_no,0x88888888);
	height=PokeHeightGet(pgt->mons_no,pgt->sex,PARA_BACK,pgt->form_no,NULL)+APPEAR_Y_TYPE_AA;
	SoftSpriteParaSet(pgt->ss[0],SS_PARA_POS_Y,height);
	SoftSpriteParaSet(pgt->ss[0],SS_PARA_ANM_PAT,pgt->anm_pat);

	ssa=SoftSpriteArcPointerGet(pgt->ssm,1);
	PokeGraArcDataGet(ssa,pgt->mons_no,pgt->sex,PARA_FRONT,pgt->col,pgt->form_no,0x88888888);
	height=PokeHeightGet(pgt->mons_no,pgt->sex,PARA_FRONT,pgt->form_no,NULL);
	SoftSpriteParaSet(pgt->ss[1],SS_PARA_POS_Y,APPEAR_Y_TYPE_BB+height);
	SoftSpriteParaSet(pgt->ss[1],SS_PARA_ANM_PAT,pgt->anm_pat);
	PokeGraTestShadowSet(pgt,height);

	SoftSpriteRecover(pgt->ssm);

	if(mode==VOICE_ON){
		Snd_PMVoicePlay(pgt->mons_no,pgt->form_no);
	}
}

static	void	PokeGraTestShadowSet(POKE_GRA_TEST *pgt,int height)
{
	s8		ox;
	s8		oy;
	u8		size;

	switch(pgt->shadow_flag%3){
	case 0:
	case 1:
		ox	=pgt->paed[PokeGraNoTable[pgt->mons_no]].shadow_ofx;
		oy	=pgt->paed[PokeGraNoTable[pgt->mons_no]].height;
		size=pgt->paed[PokeGraNoTable[pgt->mons_no]].shadow_size;
		break;
	case 2:
		{
			ARCHANDLE* handle;
			
			handle = ArchiveDataHandleOpen( ARC_PM_EDIT, HEAPID_BASE_APP );
			
			PokeEditData_HeightSet( handle, &oy, pgt->mons_no );
			PokeEditData_ShadowOfsXSet( handle, &ox, pgt->mons_no );
			PokeEditData_ShadowSizeSet( handle, &size, pgt->mons_no );
			
			ArchiveDataHandleClose( handle );
		}
	//	ArchiveDataLoadOfs(&ox,ARC_POKE_SHADOW_OFX,0,pgt->mons_no,1);
	//	ArchiveDataLoadOfs(&oy,ARC_POKE_YOFS,0,pgt->mons_no,1);
	//	ArchiveDataLoadOfs(&size,ARC_POKE_SHADOW,0,pgt->mons_no,1);
		break;
	}
	SoftSpriteParaSet(pgt->ss[1],SS_PARA_SHADOW_OY,SHADOW_OFS-height);
	SoftSpriteParaSet(pgt->ss[1],SS_PARA_SHADOW_OX,ox);
	SoftSpriteParaSet(pgt->ss[1],SS_PARA_SHADOW_HEIGHT,oy);
	SoftSpriteParaSet(pgt->ss[1],SS_PARA_SHADOW_SIZE,size);
}

//==============================================================================================
//
//	タイトルデバックメニューから呼ばれるように追加
//
//==============================================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
static PROC_RESULT PokeGraTestProc_Init(PROC * proc, int * seq)
{
	POKE_GRA_TEST	*pgt;
	sys_CreateHeap(HEAPID_BASE_APP,HEAPID_BATTLE,BATTLE_ALLOC_SIZE);
	pgt=PROC_AllocWork(proc,sizeof(POKE_GRA_TEST),HEAPID_BATTLE);
	PokeGraInit(pgt);
	return PROC_RES_FINISH;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static PROC_RESULT PokeGraTestProc_Main(PROC * proc, int * seq)
{
	POKE_GRA_TEST * pgt = PROC_GetWork(proc);
	PokeGraTestMain(pgt);
	if (pgt->end_flag) {
		return PROC_RES_FINISH;
	} else {
		return PROC_RES_CONTINUE;
	}
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static PROC_RESULT PokeGraTestProc_End(PROC * proc, int * seq)
{
	PROC_FreeWork(proc);
	sys_DeleteHeap(HEAPID_BATTLE);
	Main_SetNextProc(NO_OVERLAY_ID, &TitleProcData);
	//sys_MainProcChange( TitleMainProc );				//復帰先
	//ソフトリセット
	//詳細はソース先頭の「サウンドテストの例外処理について」を参照して下さい
	//OS_InitReset();
	OS_ResetSystem(0);									//ソフトリセット
	return PROC_RES_FINISH;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
const PROC_DATA PokeAnimeEditorProcData = {
	PokeGraTestProc_Init,
	PokeGraTestProc_Main,
	PokeGraTestProc_End,
	NO_OVERLAY_ID
};

//--------------------------------------------------------------
/**
 * @brief	VBLANK関数
 *
 * @param	work	VBLankには引数が必要なので定義してあるが実際にはNULLが入っているので、アクセスは禁止！
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static	void	PokeGraTestVBlank(void *work)
{
	POKE_GRA_TEST	*pgt=(POKE_GRA_TEST *)work;

	G2_BlendNone();
	G2S_BlendNone();

	SoftSpriteTextureTrans(pgt->ssm);

	GF_BGL_VBlankFunc(pgt->bgl);

	OS_SetIrqCheckFlag( OS_IE_V_BLANK);
}

//--------------------------------------------------------------
/**
 * @brief	3D初期化
 *
 * @retval	none	
 */
//--------------------------------------------------------------
static void Init3DStuff_(void)
{
    G3X_Init();                                            // initialize the 3D graphics states
    G3X_InitMtxStack();                                    // initialize the matrix stack

    G2_SetBG0Priority(1);

    G3X_SetShading(GX_SHADING_TOON);
    G3X_AntiAlias(TRUE);
    G3X_AlphaBlend(TRUE);

    // 3D 面は特殊効果OFFでも他のBG面とαブレンディングが行われますが、
    // そのためにはαブレンディング対象面の指定を行う必要があります。
    // SDK には対象面のみを設定する API が存在しないので、代わりに G2_SetBlendAlpha を使います。
    // この場合 G2_SetBlendAlpha の後ろ2つの引数は無視されます。
    G2_SetBlendAlpha(GX_BLEND_PLANEMASK_BG0, GX_BLEND_PLANEMASK_BD, 0, 0);

    // clear color
    G3X_SetClearColor(GX_RGB(0, 0, 0), 0, 0x7fff, 0, FALSE );

    G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_Z);
}

//--------------------------------------------------------------
/**
 * @brief	ポケモン性別ゲット
 *
 * @retval	none	
 */
//--------------------------------------------------------------
static	u8	PokeGraTestSexGet(u16 monsno)
{
	u8	sex;

	sex=PokePersonalParaGet(monsno,ID_PER_sex);
	switch(sex){
		case	MONS_MALE:
			return	PARA_MALE;
		case	MONS_FEMALE:
			return	PARA_FEMALE;
		case	MONS_UNKNOWN:
			return	PARA_UNK;
	}
	return	3;
}

//--------------------------------------------------------------
//	メッセージ表示位置定義
//--------------------------------------------------------------
#define	ANM_FRONT_NUM_X		(16)	//アニメーションナンバー
#define	ANM_FRONT_X			(48)	//アニメーションナンバー
#define	ANM_FRONT_WAIT_X	(208)	//アニメーションナンバー
#define	ANM_FRONT_Y			(0)		//アニメーションナンバー

#define	ANM_BACK_NUM_X		(16)	//アニメーションナンバー
#define	ANM_BACK_X			(48)	//アニメーションナンバー
#define	ANM_BACK_WAIT_X		(208)	//アニメーションナンバー
#define	ANM_BACK_Y			(13)	//アニメーションナンバー
#define	ANM_BACK_OFS_Y		(13)	//アニメーションナンバー

#define	ANM_NO_X	(16)	//アニメーションナンバー
#define	ANM_NO_Y	(52)	//アニメーションナンバー
#define	PAT_NO_X	(64)	//Patナンバー
#define	PAT_NO_Y	(52)	//Patナンバー
#define	WAIT_X		(112)	//アニメーションWait
#define	WAIT_Y		(52)	//アニメーションWait

#define	ANM_NO_OFS_Y	(14)	//アニメーションナンバー

#define	SVLD_X		(160 - 80)
#define	SVLD_Y		( 80 - 16)

#define	YES_NO_X	(160 - 80)
#define	YES_NO_Y	( 96 - 16)

#define	YES_NO_CUR_X	(144 - 80)
#define	YES_NO_CUR_Y	( 96 - 16)

static void	PAD_PageView(POKE_GRA_TEST* pgt, GF_PRINTCOLOR col)
{
	int page;
	
	page = pgt->sub_page;

	GF_BGL_BmpWinFill( pgt->win_s,0x0f, 16, 0, 144, 14 );
	
	MSGMAN_GetString( pgt->man_msg, POKE_TEST_025 + page, pgt->msg_buf );

	GF_STR_PrintColor(pgt->win_s, FONT_SYSTEM,pgt->msg_buf, 16, 0, MSG_NO_PUT, col, NULL);

	pgt->put_req = 1;
}

static BOOL	PAD_PageMain(POKE_GRA_TEST* pgt)
{
	if ( pgt->sub_seq_no == 0 ){
		
		pgt->sub_seq_no++;
		
		PAD_PageView( pgt, COL_ACTIVE );
	}
	
	if ( sys.repeat & PAD_KEY_UP
	||	 sys.repeat & PAD_KEY_DOWN
	||	 sys.repeat & PAD_KEY_LEFT
	||	 sys.repeat & PAD_KEY_RIGHT ){
	
		pgt->sub_page ^= 1;
	}
	
	if( sys.repeat ){
		
		PDA_PageChageView( pgt, COL_ACTIVE );
	}
	
	if( sys.trg & PAD_BUTTON_A ){
		/*
		
		PAD_PageView( pgt, COL_NORMAL );
		*/
		return TRUE;
	}
	
	return FALSE;
}


static void	PAD_VoiceView(POKE_GRA_TEST* pgt, GF_PRINTCOLOR col)
{
	u8	vo_wait;
	int grano = PokeGraNoTable[ pgt->mons_no ];

	vo_wait = pgt->paed[ grano ].poke_anime[ pgt->sub_page ].voice_wait;

	GF_BGL_BmpWinFill(pgt->win_s, 0x0f, 16, 14, 238, 14);

	STRBUF_SetNumber(pgt->msg_buf, vo_wait, 2, NUMBER_DISPTYPE_ZERO,NUMBER_CODETYPE_DEFAULT);
	GF_STR_PrintColor(pgt->win_s, FONT_SYSTEM, pgt->msg_buf, 208, 14, MSG_NO_PUT, col, NULL);
	
	MSGMAN_GetString(pgt->man_msg, POKE_TEST_024, pgt->msg_buf);	
	GF_STR_PrintColor(pgt->win_s, FONT_SYSTEM, pgt->msg_buf, 16, 14, MSG_NO_PUT, col, NULL);

	pgt->put_req=1;
}

static BOOL PAD_Calc_u8(u8* dat, int add, int skp, int min, int max, PT_VIEW_FUNC view_func, POKE_GRA_TEST* pgt)
{
	if ( pgt->sub_seq_no == 0 ){
		
		pgt->sub_seq_no++;
		
		view_func( pgt, COL_ACTIVE );
	}
	
	if ( sys.repeat & PAD_KEY_UP ){
		
		if ( *dat + add < max ){
			
			*dat += add;	
		}
		else {
			
			*dat = max;
		}
	}
	
	if ( sys.repeat & PAD_KEY_DOWN ){
		
		if ( *dat - add > min ){
			
			*dat -= add;
		}
		else {
			
			*dat = min;
		}
	}
	
	if ( sys.repeat & PAD_KEY_RIGHT ){
		
		if ( *dat + skp < max ){
			
			*dat += skp;	
		}
		else {
			
			*dat = max;
		}
	}
	
	if ( sys.repeat & PAD_KEY_LEFT ){
		
		if ( *dat - skp > min ){
			
			*dat -= skp;
		}
		else {
			
			*dat = min;
		}
	}
		
	if( sys.repeat ){
		
		view_func( pgt, COL_ACTIVE );
	}
	
	if( sys.trg & PAD_BUTTON_A ){
		/*
		
		view_func( pgt, COL_NORMAL );
		*/
		return TRUE;
	}
	
	return FALSE;
}


static BOOL PAD_Calc_u8_ex(u8* dat, int add, int skp, int min, int max, PT_VIEW_FUNC_EX view_func, POKE_GRA_TEST* pgt, int no)
{
	if ( pgt->sub_seq_no == 0 ){
		
		pgt->sub_seq_no++;
		
		view_func( pgt, COL_ACTIVE, no );
	}
	
	if ( sys.repeat & PAD_KEY_UP ){
		
		if ( *dat + add < max ){
			
			*dat += add;	
		}
		else {
			
			*dat = max;
		}
	}
	
	if ( sys.repeat & PAD_KEY_DOWN ){
		
		if ( *dat - add > min ){
			
			*dat -= add;
		}
		else {
			
			*dat = min;
		}
	}
	
	if ( sys.repeat & PAD_KEY_RIGHT ){
		
		if ( *dat + skp < max ){
			
			*dat += skp;	
		}
		else {
			
			*dat = max;
		}
	}
	
	if ( sys.repeat & PAD_KEY_LEFT ){
		
		if ( *dat - skp > min ){
			
			*dat -= skp;
		}
		else {
			
			*dat = min;
		}
	}
		
	if( sys.repeat ){
		
		view_func( pgt, COL_ACTIVE, no );
	}
	
	if( sys.trg & PAD_BUTTON_A ){
		
		view_func( pgt, COL_NORMAL, no );
		
		return TRUE;
	}
	
	return FALSE;
}


static BOOL PAD_Calc_s8(s8* dat, int add, int skp, int min, int max, PT_VIEW_FUNC view_func, POKE_GRA_TEST* pgt)
{
	if ( pgt->sub_seq_no == 0 ){
		
		pgt->sub_seq_no++;
		
		view_func( pgt, COL_ACTIVE );
	}
	
	if ( sys.repeat & PAD_KEY_UP ){
		
		if ( *dat + add < max ){
			
			*dat += add;	
		}
		else {
			
			*dat = max;
		}
	}
	
	if ( sys.repeat & PAD_KEY_DOWN ){
		
		if ( *dat - add > min ){
			
			*dat -= add;
		}
		else {
			
			*dat = min;
		}
	}
	
	if ( sys.repeat & PAD_KEY_RIGHT ){
		
		if ( *dat + skp < max ){
			
			*dat += skp;	
		}
		else {
			
			*dat = max;
		}
	}
	
	if ( sys.repeat & PAD_KEY_LEFT ){
		
		if ( *dat - skp > min ){
			
			*dat -= skp;
		}
		else {
			
			*dat = min;
		}
	}
		
	if( sys.repeat ){
		
		view_func( pgt, COL_ACTIVE );
	}
	
	if( sys.trg & PAD_BUTTON_A ){
		
		view_func( pgt, COL_NORMAL );
		
		return TRUE;
	}
	
	return FALSE;
}


static BOOL PAD_Calc_s8_ex(s8* dat, int add, int skp, int min, int max, PT_VIEW_FUNC_EX view_func, POKE_GRA_TEST* pgt, int no)
{
	if ( pgt->sub_seq_no == 0 ){
		
		pgt->sub_seq_no++;
		
		view_func( pgt, COL_ACTIVE, no );
	}
	
	if ( sys.repeat & PAD_KEY_UP ){
		
		if ( *dat + add < max ){
			
			*dat += add;	
		}
		else {
			
			*dat = max;
		}
	}
	
	if ( sys.repeat & PAD_KEY_DOWN ){
		
		if ( *dat - add > min ){
			
			*dat -= add;
		}
		else {
			
			*dat = min;
		}
	}
	
	if ( sys.repeat & PAD_KEY_RIGHT ){
		
		if ( *dat + skp < max ){
			
			*dat += skp;	
		}
		else {
			
			*dat = max;
		}
	}
	
	if ( sys.repeat & PAD_KEY_LEFT ){
		
		if ( *dat - skp > min ){
			
			*dat -= skp;
		}
		else {
			
			*dat = min;
		}
	}
		
	if( sys.repeat ){
		
		view_func( pgt, COL_ACTIVE, no );
	}
	
	if( sys.trg & PAD_BUTTON_A ){
		
		view_func( pgt, COL_NORMAL, no );
		
		return TRUE;
	}
	
	return FALSE;
}

static BOOL	PAD_VoiceMain(POKE_GRA_TEST* pgt)
{
	int add = 1;
	int skp = 3;
	int min = 0;
	int max = 15;
	PT_VIEW_FUNC view_func = PAD_VoiceView;
	int grano = PokeGraNoTable[ pgt->mons_no ];
	u8* dat = &pgt->paed[ grano ].poke_anime[ pgt->sub_page ].voice_wait;
	
	return PAD_Calc_u8(dat, add, skp, min, max, view_func, pgt);
}


static void	PAD_PG_Anm_View(POKE_GRA_TEST* pgt,GF_PRINTCOLOR col)
{
	int	grano;
	u8	anm_no;

	grano  = PokeGraNoTable[ pgt->mons_no ];
	anm_no = pgt->paed[ grano ].poke_anime[ pgt->sub_page ].prg_anm;

	GF_BGL_BmpWinFill(pgt->win_s, 0x0f, 16, 28, 160, 14);

	STRBUF_SetNumber(pgt->msg_buf, anm_no + 1, 3, NUMBER_DISPTYPE_ZERO, NUMBER_CODETYPE_DEFAULT);
	GF_STR_PrintColor(pgt->win_s, FONT_SYSTEM, pgt->msg_buf, 16, 28, MSG_NO_PUT, col, NULL);
	
	MSGMAN_GetString(pgt->man_pam, MSG_POKE_ANM001 + anm_no, pgt->msg_buf);
	GF_STR_PrintColor(pgt->win_s, FONT_SYSTEM, pgt->msg_buf, 48, 28, MSG_NO_PUT, col, NULL);

	pgt->put_req=1;
}

static BOOL	PAD_PG_Anm_Main(POKE_GRA_TEST* pgt)
{
	int add = 1;
	int skp = 10;
	int min = 0;
	int	max = POKE_ANIME_MAX-1;
	PT_VIEW_FUNC view_func = PAD_PG_Anm_View;
	int grano = PokeGraNoTable[ pgt->mons_no ];
	u8* dat  = &pgt->paed[ grano ].poke_anime[ pgt->sub_page ].prg_anm;
	
	return PAD_Calc_u8(dat, add, skp, min, max, view_func, pgt);
}


static void	PAD_PG_AnmWait_View(POKE_GRA_TEST* pgt,GF_PRINTCOLOR col)
{
	int	grano;
	u8	wait;

	grano  = PokeGraNoTable[ pgt->mons_no ];
	wait   = pgt->paed[ grano ].poke_anime[ pgt->sub_page ].prg_anm_wait;

	GF_BGL_BmpWinFill(pgt->win_s, 0x0f, 208, 28, 48, 14);

	STRBUF_SetNumber(pgt->msg_buf, wait, 2, NUMBER_DISPTYPE_ZERO, NUMBER_CODETYPE_DEFAULT);
	GF_STR_PrintColor(pgt->win_s, FONT_SYSTEM, pgt->msg_buf, 208, 28, MSG_NO_PUT, col, NULL);

	pgt->put_req=1;
}

static BOOL	PAD_PG_AnmWait_Main(POKE_GRA_TEST* pgt)
{
	int add = 1;
	int skp = 3;
	int min = 0;
	int	max = 60;
	PT_VIEW_FUNC view_func = PAD_PG_AnmWait_View;
	int grano = PokeGraNoTable[ pgt->mons_no ];
	u8* dat  = &pgt->paed[ grano ].poke_anime[ pgt->sub_page ].prg_anm_wait;
	
	return PAD_Calc_u8(dat, add, skp, min, max, view_func, pgt);
}

static void	PAD_PAT_Anm_View(POKE_GRA_TEST* pgt, GF_PRINTCOLOR col, int no)
{
	int	grano;
	s8	dat;

	grano = PokeGraNoTable[ pgt->mons_no ];
	dat	  = pgt->paed[ grano ].poke_anime[ pgt->sub_page ].ssanm[ no ].patno;

	GF_BGL_BmpWinFill(pgt->win_s, 0x0f, 48, 42 + (no * 14), 32, 14);
		
	if ( dat < 0 ){
		
		if ( dat == -1 ){
			
			MSGMAN_GetString(pgt->man_msg, POKE_TEST_007, pgt->msg_buf);
		}
		else {
			
			MSGMAN_GetString(pgt->man_msg,POKE_TEST_014 + ( (dat * -1) -2 ), pgt->msg_buf);
		}		
	}
	else {
		
		STRBUF_SetNumber(pgt->msg_buf, dat, 1, NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT);
	}
	GF_STR_PrintColor(pgt->win_s, FONT_SYSTEM, pgt->msg_buf, 48, 42 + (no * 14), MSG_NO_PUT, col, NULL);

	pgt->put_req=1;
}

static BOOL	PAD_PAT_Anm_Main(POKE_GRA_TEST* pgt)
{
	int add = 1;
	int skp = 1;
	int min = -10;
	int	max = 1;
	PT_VIEW_FUNC_EX view_func = PAD_PAT_Anm_View;
	int	no	= pgt->pos - ePAD_EDIT_01;
	int grano = PokeGraNoTable[ pgt->mons_no ];
	s8*	dat	  = &pgt->paed[ grano ].poke_anime[ pgt->sub_page ].ssanm[ no ].patno;
	
	return PAD_Calc_s8_ex(dat, add, skp, min, max, view_func, pgt, no);
}


static void	PAD_PAT_AnmWait_View(POKE_GRA_TEST* pgt, GF_PRINTCOLOR col, int no)
{
	int	grano;
	u8	wait;

	grano  = PokeGraNoTable[ pgt->mons_no ];
	wait   = pgt->paed[ grano ].poke_anime[ pgt->sub_page ].ssanm[ no ].wait;

	GF_BGL_BmpWinFill(pgt->win_s, 0x0f, 80, 42 + (no * 14), 32, 14);	

	STRBUF_SetNumber(pgt->msg_buf, wait, 2, NUMBER_DISPTYPE_ZERO, NUMBER_CODETYPE_DEFAULT);
	GF_STR_PrintColor(pgt->win_s, FONT_SYSTEM, pgt->msg_buf, 80, 42 + (no * 14), MSG_NO_PUT, col, NULL);

	pgt->put_req=1;
}


static BOOL	PAD_PAT_AnmWait_Main(POKE_GRA_TEST* pgt)
{
	int add = 1;
	int skp = 3;
	int min = 0;
	int	max = 15;
	PT_VIEW_FUNC_EX view_func = PAD_PAT_AnmWait_View;
	int grano = PokeGraNoTable[ pgt->mons_no ];
	int	no	= pgt->pos - ePAD_EDIT_01_WAIT;
	u8* dat  = &pgt->paed[ grano ].poke_anime[ pgt->sub_page ].ssanm[ no ].wait;

	return PAD_Calc_u8_ex(dat, add, skp, min, max, view_func, pgt, no);
}



static void	PAD_PAT_AnmPos_View(POKE_GRA_TEST* pgt, GF_PRINTCOLOR col, int no)
{
	int	grano;
	s8	x, y;

	grano  = PokeGraNoTable[ pgt->mons_no ];
	x	   = pgt->paed[ grano ].poke_anime[ pgt->sub_page ].ssanm[ no ].ox;
	y	   = pgt->paed[ grano ].poke_anime[ pgt->sub_page ].ssanm[ no ].oy;

	GF_BGL_BmpWinFill(pgt->win_s, 0x0f, 128, 42 + (no * 14), 128, 14);
	
	WORDSET_RegisterNumber( pgt->wordset, 4, x, 3, NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT );
	WORDSET_RegisterNumber( pgt->wordset, 5, y, 3, NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT );
	
	MSGMAN_GetString(pgt->man_msg, POKE_TEST_023, pgt->msg_buf);
	
	WORDSET_ExpandStr( pgt->wordset, pgt->msg_buf2, pgt->msg_buf );
	
	GF_STR_PrintColor(pgt->win_s, FONT_SYSTEM, pgt->msg_buf2, 128, 42 + (no * 14), MSG_NO_PUT, col, NULL);

	pgt->put_req=1;
}

static BOOL	PAD_PAT_AnmPos_Main(POKE_GRA_TEST* pgt)
{
	s8*	x;
	s8* y;
	
	int add = 1;
	int min = -32;
	int	max =  32;
	PT_VIEW_FUNC_EX view_func = PAD_PAT_AnmPos_View;
	int grano = PokeGraNoTable[ pgt->mons_no ];
	int	no	= pgt->pos - ePAD_EDIT_01_POS;

	x	   = &pgt->paed[ grano ].poke_anime[ pgt->sub_page ].ssanm[ no ].ox;
	y	   = &pgt->paed[ grano ].poke_anime[ pgt->sub_page ].ssanm[ no ].oy;
	
	if ( pgt->sub_seq_no == 0 ){
		
		pgt->sub_seq_no++;
		
		view_func( pgt, COL_ACTIVE, no );
	}
	
	if ( sys.repeat & PAD_KEY_DOWN ){
		
		if ( *y + add < max ){
			
			*y += add;	
		}
		else {
			
			*y = max;
		}
	}
	
	if ( sys.repeat & PAD_KEY_UP ){
		
		if ( *y - add > min ){
			
			*y -= add;
		}
		else {
			
			*y = min;
		}
	}
	
	if ( sys.repeat & PAD_KEY_RIGHT ){
		
		if ( *x + add < max ){
			
			*x += add;	
		}
		else {
			
			*x = max;
		}
	}
	
	if ( sys.repeat & PAD_KEY_LEFT ){
		
		if ( *x - add > min ){
			
			*x -= add;
		}
		else {
			
			*x = min;
		}
	}
		
	if( sys.repeat ){
		
		view_func( pgt, COL_ACTIVE, no );
	}
	
	if( sys.trg & PAD_BUTTON_A ){
		
		view_func( pgt, COL_NORMAL, no );
		
		return TRUE;
	}
	
	return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief	サブスクリーン表示
 *
 * @retval	none	
 */
//--------------------------------------------------------------
static	void	PokeGraTestSubScreenMake(POKE_GRA_TEST *pgt)
{
#if 0
	int		i;	

	GF_BGL_BmpWinDataFill(pgt->win_s,0xff);
	
	PAD_PageView( pgt, COL_NORMAL );
	PAD_VoiceView( pgt, COL_NORMAL );
	PAD_PG_Anm_View( pgt, COL_NORMAL );
	PAD_PG_AnmWait_View( pgt, COL_NORMAL );
	
	for ( i = 0; i < SS_ANM_SEQ_MAX; i++ ){
			
		STRBUF_SetNumber(pgt->msg_buf, i + 1, 2, NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT);
		GF_STR_PrintSimple(pgt->win_s, FONT_SYSTEM, pgt->msg_buf, 24, 42 + (i * 14), MSG_NO_PUT, NULL);

		PAD_PAT_Anm_View( pgt, COL_NORMAL, i );
		PAD_PAT_AnmWait_View( pgt, COL_NORMAL, i );
		PAD_PAT_AnmPos_View( pgt, COL_NORMAL, i );
	}

	pgt->put_req=1;
#endif
}

static void PDA_PageChageView( POKE_GRA_TEST* pgt, GF_PRINTCOLOR col )
{
	int i;
	
	PAD_PageView( pgt, col );
	PAD_VoiceView( pgt, COL_NORMAL );
	PAD_PG_Anm_View( pgt, COL_NORMAL );
	PAD_PG_AnmWait_View( pgt, COL_NORMAL );

	for ( i = 0; i < SS_ANM_SEQ_MAX; i++ ){
		
		PAD_PAT_Anm_View( pgt, COL_NORMAL, i );
		PAD_PAT_AnmWait_View( pgt, COL_NORMAL, i );
		PAD_PAT_AnmPos_View( pgt, COL_NORMAL, i );
	}

	pgt->put_req=1;
}


//--------------------------------------------------------------
/**
 * @brief	サブスクリーンデータ表示
 *
 * @retval	none	
 */
//--------------------------------------------------------------
static	void	PokeTestPokeNumDataPut(POKE_GRA_TEST *pgt,int mode)
{
	GF_BGL_BmpWinFill(pgt->win_m,0x0f,POKE_NUM_DATA_X,POKE_NUM_DATA_Y,12*8,16);

	PokeTestNumPut(pgt,POKE_NUM_DATA_X,POKE_NUM_DATA_Y,pgt->mons_no,3,pgt->cur_pos_keta,mode);

	MSGMAN_GetString(pgt->man_poke,pgt->mons_no,pgt->msg_buf);
	GF_STR_PrintSimple(pgt->win_m,FONT_SYSTEM,pgt->msg_buf,POKE_NUM_DATA_X+32,POKE_NUM_DATA_Y,MSG_NO_PUT,NULL);

	pgt->put_req=1;
}

static	void	PokeTestPokeSexDataPut(POKE_GRA_TEST *pgt)
{
	GF_BGL_BmpWinFill(pgt->win_m,0x0f,POKE_SEX_DATA_X,POKE_SEX_DATA_Y,12*2,16);

	MSGMAN_GetString(pgt->man_msg,POKE_TEST_002+pgt->sex,pgt->msg_buf);
	GF_STR_PrintSimple(pgt->win_m,FONT_SYSTEM,pgt->msg_buf,POKE_SEX_DATA_X,POKE_SEX_DATA_Y,MSG_NO_PUT,NULL);

	pgt->put_req=1;
}

static	void	PokeTestPokeColorDataPut(POKE_GRA_TEST *pgt)
{
	GF_BGL_BmpWinFill(pgt->win_m,0x0f,POKE_COLOR_DATA_X,POKE_COLOR_DATA_Y,12*4,16);

	MSGMAN_GetString(pgt->man_msg,POKE_TEST_005+pgt->col,pgt->msg_buf);
	GF_STR_PrintSimple(pgt->win_m,FONT_SYSTEM,pgt->msg_buf,POKE_COLOR_DATA_X,POKE_COLOR_DATA_Y,MSG_NO_PUT,NULL);

	pgt->put_req=1;
}

static	void	PokeTestPokePatDataPut(POKE_GRA_TEST *pgt)
{
	GF_BGL_BmpWinFill(pgt->win_m,0x0f,POKE_PAT_DATA_X,POKE_PAT_DATA_Y,12*1,16);

	//アニメパターンナンバー
	STRBUF_SetNumber(pgt->msg_buf, pgt->anm_pat,1,NUMBER_DISPTYPE_SPACE,NUMBER_CODETYPE_DEFAULT);
	GF_STR_PrintSimple(pgt->win_m,FONT_SYSTEM,pgt->msg_buf,POKE_PAT_DATA_X,POKE_PAT_DATA_Y,MSG_NO_PUT,NULL);

	pgt->put_req=1;
}

static	void	PokeTestPokeFormDataPut(POKE_GRA_TEST *pgt,int mode)
{
	int	max;

	GF_BGL_BmpWinFill(pgt->win_m,0x0f,POKE_FORM_DATA_X,POKE_FORM_DATA_Y,12*4,16);

	PokeTestNumPut(pgt,POKE_FORM_DATA_X,POKE_FORM_DATA_Y,pgt->form_no,2,pgt->cur_pos_keta,mode);

	max=PokeTestPokeFormMaxGet(pgt);

	STRBUF_SetNumber(pgt->msg_buf,max,2,NUMBER_DISPTYPE_ZERO,NUMBER_CODETYPE_DEFAULT);
	GF_STR_PrintSimple(pgt->win_m,FONT_SYSTEM,pgt->msg_buf,POKE_FORM_DATA_X+20,POKE_FORM_DATA_Y,MSG_NO_PUT,NULL);

	pgt->put_req=1;
}

static	void	PokeTestPokeHeightDataPut(POKE_GRA_TEST *pgt,GF_PRINTCOLOR col)
{
	GF_BGL_BmpWinFill(pgt->win_m,0x0f,POKE_HEIGHT_DATA_X,POKE_HEIGHT_DATA_Y,12*2,16);

	STRBUF_SetNumber(pgt->msg_buf,pgt->paed[PokeGraNoTable[pgt->mons_no]].height,3,NUMBER_DISPTYPE_LEFT,NUMBER_CODETYPE_DEFAULT);
	GF_STR_PrintColor(pgt->win_m,FONT_SYSTEM,pgt->msg_buf,POKE_HEIGHT_DATA_X,POKE_HEIGHT_DATA_Y,MSG_NO_PUT,col,NULL);

	pgt->put_req=1;
}

static	void	PokeTestPokeOfxDataPut(POKE_GRA_TEST *pgt,GF_PRINTCOLOR col)
{
	GF_BGL_BmpWinFill(pgt->win_m,0x0f,POKE_OFX_DATA_X,POKE_OFX_DATA_Y,12*2,16);

	STRBUF_SetNumber(pgt->msg_buf,pgt->paed[PokeGraNoTable[pgt->mons_no]].shadow_ofx,3,NUMBER_DISPTYPE_LEFT,NUMBER_CODETYPE_DEFAULT);
	GF_STR_PrintColor(pgt->win_m,FONT_SYSTEM,pgt->msg_buf,POKE_OFX_DATA_X,POKE_OFX_DATA_Y,MSG_NO_PUT,col,NULL);

	pgt->put_req=1;
}

static	void	PokeTestPokeShadowDataPut(POKE_GRA_TEST *pgt,GF_PRINTCOLOR col)
{
	GF_BGL_BmpWinFill(pgt->win_m,0x0f,POKE_SHADOW_DATA_X,POKE_SHADOW_DATA_Y,12*1,16);

	STRBUF_SetNumber(pgt->msg_buf,pgt->paed[PokeGraNoTable[pgt->mons_no]].shadow_size,1,NUMBER_DISPTYPE_SPACE,NUMBER_CODETYPE_DEFAULT);
	GF_STR_PrintColor(pgt->win_m,FONT_SYSTEM,pgt->msg_buf,POKE_SHADOW_DATA_X,POKE_SHADOW_DATA_Y,MSG_NO_PUT,col,NULL);

	pgt->put_req=1;
}

static	void	PokeTestPokeBGColorRDataPut(POKE_GRA_TEST *pgt,int mode)
{
	GF_BGL_BmpWinFill(pgt->win_m,0x0f,POKE_BG_COLOR_R_DATA_X,POKE_BG_COLOR_R_DATA_Y,12*2,16);

	PokeTestNumPut(pgt,POKE_BG_COLOR_R_DATA_X,POKE_BG_COLOR_R_DATA_Y,pgt->bg_r,2,pgt->cur_pos_keta,mode);

	pgt->put_req=1;
}

static	void	PokeTestPokeBGColorGDataPut(POKE_GRA_TEST *pgt,int mode)
{
	GF_BGL_BmpWinFill(pgt->win_m,0x0f,POKE_BG_COLOR_G_DATA_X,POKE_BG_COLOR_G_DATA_Y,12*2,16);

	PokeTestNumPut(pgt,POKE_BG_COLOR_G_DATA_X,POKE_BG_COLOR_G_DATA_Y,pgt->bg_g,2,pgt->cur_pos_keta,mode);

	pgt->put_req=1;
}

static	void	PokeTestPokeBGColorBDataPut(POKE_GRA_TEST *pgt,int mode)
{
	GF_BGL_BmpWinFill(pgt->win_m,0x0f,POKE_BG_COLOR_B_DATA_X,POKE_BG_COLOR_B_DATA_Y,12*2,16);

	PokeTestNumPut(pgt,POKE_BG_COLOR_B_DATA_X,POKE_BG_COLOR_B_DATA_Y,pgt->bg_b,2,pgt->cur_pos_keta,mode);

	pgt->put_req=1;
}

static	void	PokeTestBGColorSet(POKE_GRA_TEST *pgt)
{
	u16	col;
	u16	src;
	u8	evy;

	if(pgt->col_flag&1){
		pgt->col_flag+=0x100;
		if((pgt->col_flag&0x300)==0){
			if(pgt->col_flag&0x80){
				pgt->col_flag-=2;
				if((pgt->col_flag&0x0e)==0){
					pgt->col_flag^=0x80;
				}
			}
			else{
				pgt->col_flag+=2;
				if((pgt->col_flag&0x0e)==0x0e){
					pgt->col_flag^=0x80;
				}
			}
			evy=((pgt->col_flag&0x0e)>>1)+8;
			src=0;
			SoftFade(&src,&col,1,evy,GX_RGB(pgt->bg_r,pgt->bg_g,pgt->bg_b));
		}
		else{
			return;
		}
	}
	else{
		col=GX_RGB(pgt->bg_r,pgt->bg_g,pgt->bg_b);
	}

	DC_FlushRange((void *)&col,sizeof(col));
	GX_LoadBGPltt((void *)&col,0,sizeof(col));
}

static	void	PokeTestNumPut(POKE_GRA_TEST *pgt,int x,int y,int param,int keta,int pos,int mode)
{
	int		i;
	int		data,ofs;
	int		keta_tbl[10]={
		1000000000,
		100000000,
		10000000,
		1000000,
		100000,
		10000,
		1000,
		100,
		10,
		1,
	};

	ofs=0;

	for(i=0;i<keta;i++){
		data=param/keta_tbl[10-keta+i];
		param%=keta_tbl[10-keta+i];
		STRBUF_SetNumber(pgt->msg_buf,data,1,NUMBER_DISPTYPE_ZERO,NUMBER_CODETYPE_DEFAULT);
		if(mode==MODE_NORMAL){
			GF_STR_PrintColor(pgt->win_m,FONT_SYSTEM,pgt->msg_buf,x+ofs,y,MSG_NO_PUT,COL_NORMAL,NULL);
		}
		else{
			if((keta-1)-i==pos){
				GF_STR_PrintColor(pgt->win_m,FONT_SYSTEM,pgt->msg_buf,x+ofs,y,MSG_NO_PUT,COL_CURSOR,NULL);
			}
			else{
				GF_STR_PrintColor(pgt->win_m,FONT_SYSTEM,pgt->msg_buf,x+ofs,y,MSG_NO_PUT,COL_ACTIVE,NULL);
			}
		}
		ofs+=FontProc_GetPrintStrWidth(FONT_SYSTEM,pgt->msg_buf,0);
	}

	pgt->put_req=1;
}

static	int	PokeTestPokeFormMaxGet(POKE_GRA_TEST *pgt)
{
	int	ret;

	switch(pgt->mons_no){
	//ミノムッチ
	//ミノメス
	case MONSNO_MINOMUTTI:
	case MONSNO_MINOMESU:
		ret=FORMNO_MINOMUTTI_MAX;
		break;
	//シーウシ
	//シードルゴ
	case MONSNO_SIIUSI:
	case MONSNO_SIIDORUGO:
		ret=FORMNO_SII_MAX;
		break;
	//チェリシュ
	case MONSNO_THERISYU:
		ret=FORMNO_THERISYU_MAX;
		break;
	//アウス
	case MONSNO_AUSU:
		ret=POKE_WAZA_TYPE_MAX;
		break;
	//ポワルン
	case MONSNO_POWARUN:
		ret=FORMNO_POWARUN_MAX;
		break;
	//デオキシス
	case MONSNO_DEOKISISU:
		ret=FORMNO_DEOKISISU_MAX;
		break;
	//アンノーン
	case MONSNO_ANNOON:
		ret=UNK_QUE;
		break;
	default:
		ret=0;
		break;
	}
	return ret;
}

//==============================================================================================
//
//	メニューカーソル移動
//
//==============================================================================================
static	void	CursorMove(POKE_GRA_TEST *pgt,int side,int pos,int mode)
{
	switch(side){
	default:
	case SIDE_MAIN:
		GF_BGL_BmpWinFill(pgt->win_m,0x0f,mlp_m[pgt->main_pos].cursor_pos_x,mlp_m[pgt->main_pos].cursor_pos_y,12,16);
		if(mode==CUR_PUT){
			MSGMAN_GetString(pgt->man_msg,POKE_TEST_009,pgt->msg_buf);
			GF_STR_PrintSimple(pgt->win_m,FONT_SYSTEM,pgt->msg_buf,mlp_m[pos].cursor_pos_x,mlp_m[pos].cursor_pos_y,
							   MSG_NO_PUT,NULL);
			pgt->main_pos=pos;
		}
		break;
	case SIDE_SUB:
		GF_BGL_BmpWinFill(pgt->win_s,0x0f,mlp[pgt->pos].cursor_pos_x,mlp[pgt->pos].cursor_pos_y,12,16);
		if(mode==CUR_PUT){
			MSGMAN_GetString(pgt->man_msg,POKE_TEST_009,pgt->msg_buf);
			GF_STR_PrintSimple(pgt->win_s,FONT_SYSTEM,pgt->msg_buf,mlp[pos].cursor_pos_x,mlp[pos].cursor_pos_y,MSG_NO_PUT,NULL);
			pgt->pos=pos;
		}
		break;
	}
	pgt->put_req=1;
}

//==============================================================================================
//
//	メニューカーソル移動
//
//==============================================================================================
static	void	YesNoCursorMove(POKE_GRA_TEST *pgt,int pos,int mode)
{
	GF_BGL_BmpWinFill(pgt->win_s,0x0f,YES_NO_CUR_X,YES_NO_CUR_Y+ANM_NO_OFS_Y*pgt->yes_no_pos,12,16);
	if(mode==CUR_PUT){
		MSGMAN_GetString(pgt->man_msg,POKE_TEST_009,pgt->msg_buf);
		GF_STR_PrintSimple(pgt->win_s,FONT_SYSTEM,pgt->msg_buf,YES_NO_CUR_X,YES_NO_CUR_Y+ANM_NO_OFS_Y*pos,MSG_NO_PUT,NULL);
		pgt->yes_no_pos=pos;
	}
	pgt->put_req=1;
}

static	BOOL	PokeNumAct(POKE_GRA_TEST *pgt)
{
	BOOL	ret=FALSE;

	if(sys.trg==PAD_BUTTON_X){
		PokeColorAct(pgt);
	}
	if(sys.trg==PAD_BUTTON_Y){
		PokeSexAct(pgt);
	}

	switch(pgt->sub_seq_no){
	case 0:
		PokeTestPokeNumDataPut(pgt,MODE_ACTIVE);
		pgt->sub_seq_no++;
	case 1:
		switch(PokeTestNumCalc(pgt,&pgt->mons_no,3,MONSNO_HUSIGIDANE,MONSNO_END,MODE_LOOP)){
		case NUM_CALC_NONE:
			break;
		case NUM_CALC_CHANGE:
			pgt->sex=PokeGraTestSexGet(pgt->mons_no);
			if(pgt->sex==3){
				pgt->sex=0;
			}
			pgt->form_no=0;
			PokeTestPokeSexDataPut(pgt);
			PokeTestPokeFormDataPut(pgt,MODE_NORMAL);
			if((pgt->shadow_flag%3)==2){
				PokeTestPokeHeightDataPut(pgt,COL_NONE);
				PokeTestPokeOfxDataPut(pgt,COL_NONE);
				PokeTestPokeShadowDataPut(pgt,COL_NONE);
			}
			else{
				PokeTestPokeHeightDataPut(pgt,COL_NORMAL);
				PokeTestPokeOfxDataPut(pgt,COL_NORMAL);
				PokeTestPokeShadowDataPut(pgt,COL_NORMAL);
			}
			PokeGraTestDebugPrint(pgt,VOICE_ON);
		case NUM_CALC_MOVE:
			PokeTestPokeNumDataPut(pgt,MODE_ACTIVE);
			
			///< アニメデータ更新
			PokeGraTestSubScreenMake(pgt);
			break;
		case NUM_CALC_END:
			PokeTestPokeNumDataPut(pgt,MODE_NORMAL);
			ret=TRUE;
			break;
		}
		break;
	}
	return ret;
}

static	BOOL	PokeSexAct(POKE_GRA_TEST *pgt)
{
	int	sex;

	sex=PokeGraTestSexGet(pgt->mons_no);
	if(sex==3){
		pgt->sex^=1;
	}
	PokeTestPokeSexDataPut(pgt);
	PokeGraTestDebugPrint(pgt,VOICE_OFF);

	return TRUE;
}

static	BOOL	PokeColorAct(POKE_GRA_TEST *pgt)
{
	pgt->col^=1;
	PokeTestPokeColorDataPut(pgt);
	PokeGraTestDebugPrint(pgt,VOICE_OFF);

	return TRUE;
}

static	BOOL	PokePatAct(POKE_GRA_TEST *pgt)
{
	pgt->anm_pat^=1;
	PokeTestPokePatDataPut(pgt);
	PokeGraTestDebugPrint(pgt,VOICE_OFF);

	return TRUE;
}

static	BOOL	PokeFormAct(POKE_GRA_TEST *pgt)
{
	BOOL	ret=FALSE;
	int		max;

	if(sys.trg==PAD_BUTTON_X){
		PokeColorAct(pgt);
	}
	if(sys.trg==PAD_BUTTON_Y){
		PokeSexAct(pgt);
	}

	max=PokeTestPokeFormMaxGet(pgt);

	switch(pgt->sub_seq_no){
	case 0:
		PokeTestPokeFormDataPut(pgt,MODE_ACTIVE);
		pgt->sub_seq_no++;
	case 1:
		switch(PokeTestNumCalc(pgt,&pgt->form_no,2,0,max,MODE_LOOP)){
		case NUM_CALC_NONE:
			break;
		case NUM_CALC_CHANGE:
			PokeGraTestDebugPrint(pgt,VOICE_OFF);
		case NUM_CALC_MOVE:
			PokeTestPokeFormDataPut(pgt,MODE_ACTIVE);
			break;
		case NUM_CALC_END:
			PokeTestPokeFormDataPut(pgt,MODE_NORMAL);
			ret=TRUE;
			break;
		}
		break;
	}
	return ret;
}

static	BOOL	PokeHeightAct(POKE_GRA_TEST *pgt)
{
	BOOL	ret=FALSE;

	if((pgt->shadow_flag%3)==2){
		return TRUE;
	}

	switch(pgt->sub_seq_no){
	case 0:
		PokeTestPokeHeightDataPut(pgt,COL_ACTIVE);
		pgt->sub_seq_no++;
	case 1:
		if((sys.repeat & PAD_KEY_UP)&&(pgt->paed[PokeGraNoTable[pgt->mons_no]].height<HEIGHT_MAX)){
			pgt->paed[PokeGraNoTable[pgt->mons_no]].height++;
			SoftSpriteParaSet(pgt->ss[1],SS_PARA_SHADOW_HEIGHT,pgt->paed[PokeGraNoTable[pgt->mons_no]].height);
			PokeTestPokeHeightDataPut(pgt,COL_ACTIVE);
		}
		if((sys.repeat & PAD_KEY_DOWN)&&(pgt->paed[PokeGraNoTable[pgt->mons_no]].height>HEIGHT_MIN)){
			pgt->paed[PokeGraNoTable[pgt->mons_no]].height--;
			SoftSpriteParaSet(pgt->ss[1],SS_PARA_SHADOW_HEIGHT,pgt->paed[PokeGraNoTable[pgt->mons_no]].height);
			PokeTestPokeHeightDataPut(pgt,COL_ACTIVE);
		}
		if(sys.trg&PAD_BUTTON_A){
			PokeTestPokeHeightDataPut(pgt,COL_NORMAL);
			ret=TRUE;
		}
		break;
	}
	return ret;
}

static	BOOL	PokeOfxAct(POKE_GRA_TEST *pgt)
{
	BOOL	ret=FALSE;

	if((pgt->shadow_flag%3)==2){
		return TRUE;
	}

	switch(pgt->sub_seq_no){
	case 0:
		PokeTestPokeOfxDataPut(pgt,COL_ACTIVE);
		pgt->sub_seq_no++;
	case 1:
		if((sys.repeat & PAD_KEY_RIGHT)&&(pgt->paed[PokeGraNoTable[pgt->mons_no]].shadow_ofx<HEIGHT_MAX)){
			pgt->paed[PokeGraNoTable[pgt->mons_no]].shadow_ofx++;
			SoftSpriteParaSet(pgt->ss[1],SS_PARA_SHADOW_OX,pgt->paed[PokeGraNoTable[pgt->mons_no]].shadow_ofx);
			PokeTestPokeOfxDataPut(pgt,COL_ACTIVE);
		}
		if((sys.repeat & PAD_KEY_LEFT)&&(pgt->paed[PokeGraNoTable[pgt->mons_no]].shadow_ofx>HEIGHT_MIN)){
			pgt->paed[PokeGraNoTable[pgt->mons_no]].shadow_ofx--;
			SoftSpriteParaSet(pgt->ss[1],SS_PARA_SHADOW_OX,pgt->paed[PokeGraNoTable[pgt->mons_no]].shadow_ofx);
			PokeTestPokeOfxDataPut(pgt,COL_ACTIVE);
		}
		if(sys.trg&PAD_BUTTON_A){
			PokeTestPokeOfxDataPut(pgt,COL_NORMAL);
			ret=TRUE;
		}
		break;
	}
	return ret;
}

static	BOOL	PokeShadowAct(POKE_GRA_TEST *pgt)
{
	BOOL	ret=FALSE;

	if((pgt->shadow_flag%3)==2){
		return TRUE;
	}

	switch(pgt->sub_seq_no){
	case 0:
		PokeTestPokeShadowDataPut(pgt,COL_ACTIVE);
		pgt->sub_seq_no++;
	case 1:
		if((sys.repeat & PAD_KEY_UP)&&(pgt->paed[PokeGraNoTable[pgt->mons_no]].shadow_size<3)){
			pgt->paed[PokeGraNoTable[pgt->mons_no]].shadow_size++;
			SoftSpriteParaSet(pgt->ss[1],SS_PARA_SHADOW_SIZE,pgt->paed[PokeGraNoTable[pgt->mons_no]].shadow_size);
			PokeTestPokeShadowDataPut(pgt,COL_ACTIVE);
		}
		if((sys.repeat & PAD_KEY_DOWN)&&(pgt->paed[PokeGraNoTable[pgt->mons_no]].shadow_size>0)){
			pgt->paed[PokeGraNoTable[pgt->mons_no]].shadow_size--;
			SoftSpriteParaSet(pgt->ss[1],SS_PARA_SHADOW_SIZE,pgt->paed[PokeGraNoTable[pgt->mons_no]].shadow_size);
			PokeTestPokeShadowDataPut(pgt,COL_ACTIVE);
		}
		if(sys.trg&PAD_BUTTON_A){
			PokeTestPokeShadowDataPut(pgt,COL_NORMAL);
			ret=TRUE;
		}
		break;
	}
	return ret;
}

static	BOOL	PokeBGColorRAct(POKE_GRA_TEST *pgt)
{
	BOOL	ret=FALSE;

	switch(pgt->sub_seq_no){
	case 0:
		PokeTestPokeBGColorRDataPut(pgt,MODE_ACTIVE);
		pgt->sub_seq_no++;
	case 1:
		switch(PokeTestNumCalc(pgt,&pgt->bg_r,2,0,31,MODE_LOOP)){
		case NUM_CALC_NONE:
			break;
		case NUM_CALC_CHANGE:
			PokeTestBGColorSet(pgt);
		case NUM_CALC_MOVE:
			PokeTestPokeBGColorRDataPut(pgt,MODE_ACTIVE);
			break;
		case NUM_CALC_END:
			PokeTestPokeBGColorRDataPut(pgt,MODE_NORMAL);
			ret=TRUE;
			break;
		}
		break;
	}
	return ret;
}

static	BOOL	PokeBGColorGAct(POKE_GRA_TEST *pgt)
{
	BOOL	ret=FALSE;

	switch(pgt->sub_seq_no){
	case 0:
		PokeTestPokeBGColorGDataPut(pgt,MODE_ACTIVE);
		pgt->sub_seq_no++;
	case 1:
		switch(PokeTestNumCalc(pgt,&pgt->bg_g,2,0,31,MODE_LOOP)){
		case NUM_CALC_NONE:
			break;
		case NUM_CALC_CHANGE:
			PokeTestBGColorSet(pgt);
		case NUM_CALC_MOVE:
			PokeTestPokeBGColorGDataPut(pgt,MODE_ACTIVE);
			break;
		case NUM_CALC_END:
			PokeTestPokeBGColorGDataPut(pgt,MODE_NORMAL);
			ret=TRUE;
			break;
		}
		break;
	}
	return ret;
}

static	BOOL	PokeBGColorBAct(POKE_GRA_TEST *pgt)
{
	BOOL	ret=FALSE;

	switch(pgt->sub_seq_no){
	case 0:
		PokeTestPokeBGColorBDataPut(pgt,MODE_ACTIVE);
		pgt->sub_seq_no++;
	case 1:
		switch(PokeTestNumCalc(pgt,&pgt->bg_b,2,0,31,MODE_LOOP)){
		case NUM_CALC_NONE:
			break;
		case NUM_CALC_CHANGE:
			PokeTestBGColorSet(pgt);
		case NUM_CALC_MOVE:
			PokeTestPokeBGColorBDataPut(pgt,MODE_ACTIVE);
			break;
		case NUM_CALC_END:
			PokeTestPokeBGColorBDataPut(pgt,MODE_NORMAL);
			ret=TRUE;
			break;
		}
		break;
	}
	return ret;
}

//==============================================================================================
//
//	数値編集
//
//==============================================================================================
static	int	PokeTestNumCalc(POKE_GRA_TEST *pgt,int *param,int keta,int min,int max,int mode)
{
	int ret=NUM_CALC_NONE;
	u32	calc_work[]={
		1,
		10,
		100,
		1000,
		10000,
		100000,
		1000000,
		10000000,
		100000000,
		1000000000,
	};

	if((sys.repeat & PAD_KEY_LEFT)&&(pgt->cur_pos_keta<(keta-1))){
		pgt->cur_pos_keta++;
		ret=NUM_CALC_MOVE;
	}
	else if((sys.repeat & PAD_KEY_RIGHT)&&(pgt->cur_pos_keta)){
		pgt->cur_pos_keta--;
		ret=NUM_CALC_MOVE;
	}
	else if(sys.repeat & PAD_KEY_UP){
		if(mode==MODE_LOOP){
			if((param[0]+calc_work[pgt->cur_pos_keta])>max){
				param[0]=min;
				ret=NUM_CALC_CHANGE;
			}
			else{
				param[0]+=calc_work[pgt->cur_pos_keta];
				ret=NUM_CALC_CHANGE;
			}
		}
		else{
			if((param[0]+calc_work[pgt->cur_pos_keta])>max){
				param[0]=max;
				ret=NUM_CALC_CHANGE;
			}
			else{
				param[0]+=calc_work[pgt->cur_pos_keta];
				ret=NUM_CALC_CHANGE;
			}
		}
	}
	else if(sys.repeat & PAD_KEY_DOWN){
		if(mode==MODE_LOOP){
			if((param[0]<calc_work[pgt->cur_pos_keta])||((param[0]-calc_work[pgt->cur_pos_keta])<min)){
				param[0]=max;
				ret=NUM_CALC_CHANGE;
			}
			else{
				param[0]-=calc_work[pgt->cur_pos_keta];
				ret=NUM_CALC_CHANGE;
			}
		}
		else{
			if((param[0]<calc_work[pgt->cur_pos_keta])||((param[0]-calc_work[pgt->cur_pos_keta])<min)){
				param[0]=min;
				ret=NUM_CALC_CHANGE;
			}
			else{
				param[0]-=calc_work[pgt->cur_pos_keta];
				ret=NUM_CALC_CHANGE;
			}
		}
	}
	else if(sys.trg & PAD_BUTTON_A){
		pgt->cur_pos_keta=0;
		ret=NUM_CALC_END;
	}

	return ret;
}

//==============================================================================================
//
//	パラメータロード
//
//==============================================================================================
static	BOOL	PokeGraTestLoadAct(POKE_GRA_TEST *pgt)
{
	int	i;

	switch(pgt->sub_seq_no){
	case 0:
		POKE_ANM_DEBUG_PRINT( " --- save_ram : 0x%08x size : 0x%08x\n",&pgt->paed[ 0 ], POKE_ANM_SAVE_SIZE );
		GF_BGL_BmpWinFill( pgt->win_s, 0x0f, 0, 0, 255, 192 );
		MSGMAN_GetString(pgt->man_msg,POKE_TEST_010,pgt->msg_buf);
		GF_STR_PrintSimple(pgt->win_s,FONT_SYSTEM,pgt->msg_buf,SVLD_X,SVLD_Y,MSG_NO_PUT,NULL);
		PokeGraTestYesNoPut(pgt);
		YesNoCursorMove(pgt,pgt->yes_no_pos,CUR_PUT);
		pgt->sub_seq_no++;
		break;
	case 1:
		if((sys.trg==PAD_KEY_UP)&&(pgt->yes_no_pos==1)){
			YesNoCursorMove(pgt,0,CUR_PUT);
		}
		if((sys.trg==PAD_KEY_DOWN)&&(pgt->yes_no_pos==0)){
			YesNoCursorMove(pgt,1,CUR_PUT);
		}
		if(sys.trg==PAD_BUTTON_A){
			if(pgt->yes_no_pos==0){
			#if 1		///< ATODE
			//	PMSVLD_Load(pgt->load_bank, &pgt->paed[0], POKE_ANM_SAVE_SIZE);
				PMSVLD_Load(EDIT_BANK, &pgt->paed[0], POKE_ANM_SAVE_SIZE);
			#endif
			}
			if(pgt->paed[0].height!=0x55){
				for(i=0;i<PGT_POKE_MAX;i++){
					pgt->paed[i].height=0;
				}
				pgt->paed[0].height=0x55;
			}
			for(i=0;i<PGT_POKE_MAX;i++){
				if(pgt->paed[i].shadow_size>3){
					pgt->paed[i].shadow_size=1;
				}
			}
			if(pgt->paed[0].shadow_ofx!=0x55){
				for(i=0;i<PGT_POKE_MAX;i++){
					pgt->paed[i].shadow_ofx=0;
				}
				pgt->paed[0].shadow_ofx=0x55;
			}
			#if 0
			{
				int i;
				int num = 0;
				OS_Printf( " 鳴き声書き換え中 \n" );
				for ( i = 0; i < PGT_POKE_MAX;i++){
					
					if ( pgt->paed[ i ].poke_anime[ 1 ].voice_wait == 15
					&&	 pgt->paed[ i ].poke_anime[ 1 ].ssanm[ 0 ].patno == -1 ){
						
						pgt->paed[ i ].poke_anime[ 1 ].voice_wait = 0;
					}
					else {
						num++;
					}
				}
				
				
				OS_Printf( " %2d \n", num );
			}
			#endif
			PokeGraTestSubScreenMake(pgt);
			return TRUE;
		}
		if(sys.trg==PAD_BUTTON_B){
			PokeGraTestSubScreenMake(pgt);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

//==============================================================================================
//
//	パラメータセーブ
//
//==============================================================================================
static	BOOL	PokeGraTestSaveAct(POKE_GRA_TEST *pgt)
{
	switch(pgt->sub_seq_no){
	case 0:
		POKE_ANM_DEBUG_PRINT( " --- save_ram : 0x%08x size : 0x%08x\n",&pgt->paed[ 0 ], POKE_ANM_SAVE_SIZE );
		GF_BGL_BmpWinFill( pgt->win_s, 0x0f, 0, 0, 255, 192 );
		MSGMAN_GetString(pgt->man_msg,POKE_TEST_011,pgt->msg_buf);
		GF_STR_PrintSimple(pgt->win_s,FONT_SYSTEM,pgt->msg_buf,SVLD_X,SVLD_Y,MSG_NO_PUT,NULL);
		PokeGraTestYesNoPut(pgt);
		YesNoCursorMove(pgt,pgt->yes_no_pos,CUR_PUT);
		pgt->sub_seq_no++;
		break;
	case 1:
		if((sys.trg==PAD_KEY_UP)&&(pgt->yes_no_pos==1)){
			YesNoCursorMove(pgt,0,CUR_PUT);
		}
		if((sys.trg==PAD_KEY_DOWN)&&(pgt->yes_no_pos==0)){
			YesNoCursorMove(pgt,1,CUR_PUT);
		}
		if(sys.trg==PAD_BUTTON_A){
			if(pgt->yes_no_pos==0){
			#if 1		///< ATODE	
			//	PMSVLD_Save(pgt->save_bank, &pgt->paed[0], POKE_ANM_SAVE_SIZE);
				PMSVLD_Save(EDIT_BANK, &pgt->paed[0], POKE_ANM_SAVE_SIZE);
				#if 0
				{
					int i;
					
					for ( i = 0; i < 500; i++ ){
						OS_Printf( "%2d\n", pgt->paed[i].poke_anime[1].ssanm[0].patno);
					}
				}
				#endif
			#endif
			}
			PokeGraTestSubScreenMake(pgt);
			return TRUE;
		}
		if(sys.trg==PAD_BUTTON_B){
			PokeGraTestSubScreenMake(pgt);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

//==============================================================================================
//
//	YesNo
//
//==============================================================================================
static	void	PokeGraTestYesNoPut(POKE_GRA_TEST *pgt)
{
	MSGMAN_GetString(pgt->man_msg,POKE_TEST_012,pgt->msg_buf);
	GF_STR_PrintSimple(pgt->win_s,FONT_SYSTEM,pgt->msg_buf,YES_NO_X,YES_NO_Y,MSG_NO_PUT,NULL);
	MSGMAN_GetString(pgt->man_msg,POKE_TEST_013,pgt->msg_buf);
	GF_STR_PrintSimple(pgt->win_s,FONT_SYSTEM,pgt->msg_buf,YES_NO_X,YES_NO_Y+ANM_NO_OFS_Y,MSG_NO_PUT,NULL);
	pgt->put_req=1;
}

#endif

#endif //PM_DEBUG
