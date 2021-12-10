//==============================================================================
/**
 * @file	demo_tengan_common.h
 * @brief	簡単な説明を書く
 * @author	goto
 * @date	2007.11.05(月)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================

#ifndef __DEMO_TENGAN_COMMON_H__
#define __DEMO_TENGAN_COMMON_H__

//#include "system/clact_tool.h"

#include "demo/demo_tengan.h"
#include "demo_tengan_gra.naix"
#include "system/snd_tool.h"
#include "poketool/monsno.h"
#include "system/window.h"
#include "system/wordset.h"

#include "demo_eff.h"
#include "system/d3dobj.h"

#include "msgdata/msg.naix"
#include "msgdata/msg_demo_tengan.h"
#include "msgdata/msg_d05r0114.h"

#include "demo_se.h"

#define ANGLE( n )				( ( 65535 / 360 ) * n )

#define ILLUMINANT_MAX	( 3 )

#define LIGHT_PARAM_MAX			( 31 )
#define LIGHT_PARAM_MIN			( 12 )
#define BRIGHTNESS_MIN			( -6 )
#define BRIGHTNESS_BK			( -16 )
#define DEMO_BRIGHTNESS_WHITE	( +16 )
#define DEMO_BRIGHTNESS_NONE	( 0 )
#define DEMO_BRIGHTNESS_BLACK	( -16 )

#define DEMO_BLEND_MASK_M ( GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD )
#define DEMO_BLEND_MASK_S ( GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD )

typedef struct {
	
	CATS_ACT_PTR cap;
	CATS_ACT_PTR cap_dmy;
	int			 id;
	int			 ai;
	
	s16			 x;
	s16			 y;
	s16			 z;
	
	s16			 tx;
	s16			 ty;
	
	int			 rad;		///< 公転用
	
	int			 rad2;		///< 自転用
	int			 count;
	int			 para[ 4 ];

	int			 flag;
	
	BOOL		 bActive;
	
	TCB_PTR		 tcb;

} ILLUMINANT;

typedef struct {
	
	int	 anime;
	int	 count;
	fx32 speed;
	
} MDL_CMD_TBL;

typedef struct {
		
    D3DOBJ				obj;
    D3DOBJ_MDL			mdl;
    D3DOBJ_ANM			anm[ 10 ];			///< とりあえず10個
    
    fx32				obj_scale;
    fx32				anm_speed;
    int					anm_no;
    int					index;
    
    BOOL				bAnime;
    BOOL				bAnimeLoop;
    BOOL				bTexAnime;    
    BOOL				bColorAnime;
    BOOL				bFullAnime;			///< すべてのアニメを使う    
    BOOL				bInit;
    
    int					cmd_count;
    int					cmd_count_max;
    int					cmd_index;
    MDL_CMD_TBL*		cmd_table;
    
} T3D_MODEL;

typedef struct {
	
	u8	r;
	u8	g;
	u8	b;
	s8	brightness;
	
} DEMO_LIGHT;

typedef struct {
	
	GF_CAMERA_PTR	camera_p;
	VecFx32*		camera_target;
	
	int set_frame;
	s32	x;			///< アングル
	s32	y;
	s32	z;
	
	fx32 mx;		///< 移動量
	fx32 my;
	fx32 mz;
	
	s32 	work[ 5 ];
	fx32	mwork[ 5 ];
	CAMERA_ANGLE angle;
	
} CAMERA_WORK;

typedef struct {
	
	int		main_seq;	///< main の シーケンス
	
	int		seq;
	int		cnt;
	int		wait;
	
	CAMERA_WORK* cw;
	
} SHAKE;

typedef struct {
	
	int			seq;
	int			wait;
	int			monsno;
	s16			pan_x;
	T3D_MODEL*	poke;
	T3D_MODEL*	okage;
	
} POKE_ON;

typedef struct _DEMO_TENGAN_SYSTEM {
	
	ARCHANDLE*			p_handle;
	
	CATS_SYS_PTR		csp;				///< OAMシステム
	CATS_RES_PTR		crp;				///< リソース一括管理

	GF_BGL_INI*			bgl;				///< BGL
	PALETTE_FADE_PTR	pfd;				///< パレットフェード
	
	GF_G3DMAN*			g3Dman;				///< 3D
	GF_CAMERA_PTR		camera_p;			///< カメラ

	NNSFndAllocator		allocator;
	
	MSGDATA_MANAGER*	man;	
	
	GF_BGL_BMPWIN		win;
	int					msg_index;
		
	VecFx32				camera_target;	
	DEMO_LIGHT			light_color;
	
	DEMO_MOTION_BL_PTR	motion_bl;
	
	CAMERA_WORK			cw;
	SHAKE				shake;
	
	u8	evy_pat;		///< BG フェード
	u8	evy;			///< BG フェード
	u8	evy_wait;
	
} DEMO_SYSTEM;

typedef struct _DEMO_TENGAN_WORK {
	
	BOOL	bFlag;
	
	int		scene_no;
	void*	scene_wk;

	DEMO_SYSTEM			sys;	
	DEMO_TENGAN_PARAM*	param;
	
} DEMO_TENGAN_WORK;


enum {
	
	eOAM_ID_ILLUMINANT = 50000,	
};

enum {
	
	eTCB_PRI_ILLUM_AI = 0x1000,
};

extern void Illuminant_AI_TCB( TCB_PTR tcb, void* work );
extern void Illuminant_S0_AI_TCB( TCB_PTR tcb, void* work );

extern void* Demo_Tengan_Scene00_Init( DEMO_TENGAN_WORK* main_wk );
extern BOOL  Demo_Tengan_Scene00_Main( void* work );
extern BOOL  Demo_Tengan_Scene00_Exit( void* work );

extern void* Demo_Tengan_Scene01_Init( DEMO_TENGAN_WORK* main_wk );
extern BOOL  Demo_Tengan_Scene01_Main( void* work );
extern BOOL  Demo_Tengan_Scene01_Exit( void* work );

extern void* Demo_Tengan_Scene02_Init( DEMO_TENGAN_WORK* main_wk );
extern BOOL  Demo_Tengan_Scene02_Main( void* work );
extern BOOL  Demo_Tengan_Scene02_Exit( void* work );

extern int  Demo_MSG_Set( DEMO_SYSTEM* sys, DEMO_TENGAN_PARAM* param, int msg );
extern void Demo_MSG_Off( DEMO_SYSTEM* sys );
extern void Demo_Light( DEMO_SYSTEM* sys );
extern void Demo_Camera( DEMO_SYSTEM* sys );
extern void Camera_Work_Init( CAMERA_WORK* cw );
extern BOOL Camera_Work_Main( CAMERA_WORK* cw );
extern void D3D_Draw( T3D_MODEL* mdl );
extern void res_delete( T3D_MODEL* mdl );
extern void res_anime_delete( int no, T3D_MODEL* mdl, NNSFndAllocator* allocator );
extern void res_model_delete( T3D_MODEL* mdl, NNSFndAllocator* allocator, int anm_num );
extern void res_load_init( T3D_MODEL* mdl, int id, ARCHANDLE* hdl );
extern void res_anime_init( int no, T3D_MODEL* mdl, int id, ARCHANDLE* hdl, NNSFndAllocator* allocator );
extern void BG_PaletteFade_Sub( DEMO_SYSTEM* sys );
extern void BG_PaletteFade_Sub_UMA( DEMO_SYSTEM* sys, int poke_type );
extern void res_load_init_Ex( T3D_MODEL* base_mdl, T3D_MODEL* mdl,  int id, ARCHANDLE* hdl );
extern void res_anime_init_Ex( int no, T3D_MODEL* base_mdl, T3D_MODEL* mdl, int id, ARCHANDLE* hdl, NNSFndAllocator* allocator );


enum {
	eCMD_NONE = 0,
	eCMD_UM,
	eCMD_DM,
	eCMD_LM,
	eCMD_RM,
	eCMD_UW,
	eCMD_DW,
	eCMD_LW,
	eCMD_RW,
	
	eCMD_UWB,	///< 上向き静止後ずさり
	
	eCMD_D1,
	eCMD_D2,
	eCMD_D3,
	eCMD_D4,
};

extern void Demo_Anime( T3D_MODEL* mdl );
extern void Demo_Anime_Start( T3D_MODEL* mdl, MDL_CMD_TBL* cmd );
extern BOOL Demo_Anime_EndCheck( T3D_MODEL* mdl );
extern void Demo_Anime_Command( T3D_MODEL* mdl );

enum {
	
	eMSG_1_1 = 0,
	eMSG_1_2,
	eMSG_1_3,
	eMSG_1_4,
	eMSG_1_5,
	eMSG_1_6,
	eMSG_1_7,
	eMSG_1_8,
	eMSG_1_9,
	eMSG_1_10,
};

typedef struct {
		
	DEMO_LASTER_SCROLL laster;
	ILLUMINANT illum[ ILLUMINANT_MAX ];
	ILLUMINANT illum_dmy[ ILLUMINANT_MAX ];
	
} DEMO_SD_S1;

typedef struct {
	
	int i;
	
	T3D_MODEL map;
	T3D_MODEL tama;
	T3D_MODEL colum;
	T3D_MODEL siru;
	T3D_MODEL poke[ 2 ];
	T3D_MODEL gira[ 5 ];
	T3D_MODEL kage[ 2 ];
	T3D_MODEL hobj[ 2 ];
	T3D_MODEL okage[ 4 ]; ///< poke hobj の影
	
} DEMO_MD_S1;

typedef struct {
	
	T3D_MODEL uma[ 3 ];
	T3D_MODEL bg;
	
} DEMO_MD_S0;


///< シーン０のワーク	山頂に球が集まるシーン
typedef struct {
	
	int		cut;
	int		wait;
	int		poke_type;

	DEMO_SD_S1			s_disp;
	DEMO_MD_S0			m_disp;
	
	DEMO_SYSTEM*		sys;	
	DEMO_TENGAN_PARAM*	param;
	
} DEMO_SCENE_00_WORK;


///< シーン１のワーク	アカギが破れた世界に引き込まれるシーン
typedef struct {

	int		cut;
	int		wait;
	
	DEMO_SD_S1			s_disp;
	DEMO_MD_S1			m_disp;
	
	DEMO_SYSTEM*		sys;	
	DEMO_TENGAN_PARAM*	param;
	
} DEMO_SCENE_01_WORK;


///< シーン２のワーク	ディアルガパルキアが同時に登場するシーン
typedef struct {
	
	int cut;
	int wait;
	int flash;

	DEMO_MD_S1			m_disp;

	DEMO_SYSTEM*		sys;	
	DEMO_TENGAN_PARAM*	param;
	
	POKE_ON				pokeon[2];
	
} DEMO_SCENE_02_WORK;

extern void Demo_MainDispSet( int mode );
extern void Demo_BlendBrightness( DEMO_TENGAN_WORK* wk, int para );


#endif
