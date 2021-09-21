//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wflby_light.c
 *	@brief		ライトコントロール
 *	@author		tomoya takahashi
 *	@data		2008.01.25
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "common.h"
#include "wflby_light.h"

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
#ifdef PM_DEBUG
//#define WFLBY_DEBUG_LIGHT_PRINT_ON
#endif

#ifdef WFLBY_DEBUG_LIGHT_PRINT_ON
#define WFLBY_DEBUG_LIGHT_PRINT(...)		OS_TPrintf(__VA_ARGS__)
#else
#define WFLBY_DEBUG_LIGHT_PRINT(...)		((void)0);
#endif


//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
#define WFLBY_LIGHT_LIGHT_NUM	(2)		// ライト数
#define WFLBY_LIGHT_MAT_NUM		(4)		// マテリアル
//  RGB
enum{
	WFLBY_LIGHT_COL_R,
	WFLBY_LIGHT_COL_G,
	WFLBY_LIGHT_COL_B,
	WFLBY_LIGHT_COL_PAD,	// パディング
	WFLBY_LIGHT_COL_NUM,
};

// ライトデータ反映フレーム数
#define WFLBY_LIGHT_FADE_COUNT	( 60 )

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	ライトデータ
//=====================================
typedef struct{
	GXRgb lightcolor;
	GXRgb matcolor[WFLBY_LIGHT_MAT_NUM];
} WFLBY_LIGHTDATA_ROOM;
typedef struct{
	GXRgb lightcolor;
} WFLBY_LIGHTDATA_FLOOR;

//-------------------------------------
///	カラーデータ
//=====================================
typedef struct {
	s8	color[ WFLBY_LIGHT_COL_NUM ];
} WFLBY_LIGHT_COL;


//-------------------------------------
///	ライト詳細データ
//=====================================
typedef struct {
	WFLBY_LIGHT_COL lightcolor_ful[WFLBY_LIGHT_LIGHT_NUM];
	WFLBY_LIGHT_COL matcolor_ful[WFLBY_LIGHT_MAT_NUM];
} WFLBY_LIGHT_FULL;


//-------------------------------------
///	ライトシステム
//=====================================
typedef struct _WFLBY_LIGHTWK {
	WFLBY_LIGHT_FULL	now;		// 設定値
	WFLBY_LIGHT_FULL	last;		// 変更前
	WFLBY_LIGHT_FULL	next;		// 変更後
	s32					lightcount[WFLBY_LIGHT_LIGHT_NUM];	// 変更カウンタ
	s32					matcount[WFLBY_LIGHT_MAT_NUM];	// 変更カウンタ
}WFLBY_LIGHTWK;


//-----------------------------------------------------------------------------
/**
 *					ライトデータ
 */
//-----------------------------------------------------------------------------
//-------------------------------------
///	室内ライトデータ
//=====================================
static const WFLBY_LIGHTDATA_ROOM sc_WFLBY_LIGHTDATA_ROOM[ WFLBY_LIGHT_NEON_ROOMNUM ] = {
	// WFLBY_LIGHT_NEON_ROOM0
	{
		GX_RGB( 23,23,25 ),
		{
			GX_RGB( 16,16,16 ),
			GX_RGB( 14,14,14 ),
			GX_RGB( 20,20,20 ),
			GX_RGB( 16,16,16 ),
		},
	},

	// WFLBY_LIGHT_NEON_ROOM1
	{
		GX_RGB( 10,10,16 ),
		{
			GX_RGB( 16,16,16 ),
			GX_RGB( 14,14,14 ),
			GX_RGB( 20,20,20 ),
			GX_RGB( 16,16,16 ),
		},
	},

	// WFLBY_LIGHT_NEON_ROOM2
	{
		GX_RGB( 8,8,12 ),
		{
			GX_RGB( 16,16,16 ),
			GX_RGB( 14,14,14 ),
			GX_RGB( 20,20,20 ),
			GX_RGB( 16,16,16 ),
		},
	},

	// WFLBY_LIGHT_NEON_ROOM3
	{
		GX_RGB( 5,5,8 ),
		{
			GX_RGB( 16,16,16 ),
			GX_RGB( 13,13,14 ),
			GX_RGB( 20,20,20 ),
			GX_RGB( 16,16,16 ),
		},
	},

	// WFLBY_LIGHT_NEON_ROOM4
	{
		GX_RGB( 3,3,7 ),
		{
			GX_RGB( 16,16,16 ),
			GX_RGB( 10,10,12 ),
			GX_RGB( 20,20,20 ),
			GX_RGB( 14,14,16 ),
		},
	},

	// WFLBY_LIGHT_NEON_ROOM5
	{
		GX_RGB( 0,0,6 ),
		{
			GX_RGB( 16,16,16 ),
			GX_RGB( 8,8,10 ),
			GX_RGB( 20,20,20 ),
			GX_RGB( 12,12,16 ),
		},
	},
};

//-------------------------------------
///	床　モニュメントライトデータ
//=====================================
static const WFLBY_LIGHTDATA_FLOOR sc_WFLBY_LIGHTDATA_FLOOR[ WFLBY_LIGHT_NEON_FLOORNUM ] = {
	// WFLBY_LIGHT_NEON_FLOOR0
	{
		GX_RGB( 14,14,14 ),
	},

	// WFLBY_LIGHT_NEON_FLOOR1
	{
		GX_RGB( 20,20,20 ),
	},

	// WFLBY_LIGHT_NEON_FLOOR2
	{
		GX_RGB( 31,31,31 ),
	},

	// WFLBY_LIGHT_NEON_FLOOR3
	{
		GX_RGB( 8,8,8 ),
	},

};



//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
static void WFLBY_LIGHT_ConvetLightColToNitro( const WFLBY_LIGHT_COL* cp_data, GXRgb* p_col );
static void WFLBY_LIGHT_ConvetNitroToLightCol( WFLBY_LIGHT_COL* p_data, const GXRgb* cp_col );
static void WFLBY_LIGHT_Col_Marge( const WFLBY_LIGHT_COL* cp_data0, const WFLBY_LIGHT_COL* cp_data1, WFLBY_LIGHT_COL* p_datain, s32 count, s32 countmax );

static void WFLBY_LIGHT_Ful_SetLIGHTDATA_ROOM( WFLBY_LIGHT_FULL* p_data, const WFLBY_LIGHTDATA_ROOM* cp_lightdata );
static void WFLBY_LIGHT_Ful_SetLIGHTDATA_FLOOR( WFLBY_LIGHT_FULL* p_data, const WFLBY_LIGHTDATA_FLOOR* cp_lightdata );
static void WFLBY_LIGHT_Ful_SetLight( const WFLBY_LIGHT_FULL* cp_data0 );


//----------------------------------------------------------------------------
/**
 *	@brief	ライト初期化
 *
 *	@param	heapID		ヒープID
 *	@param	roomtype	部屋タイプ
 *	@param	floor		床タイプ
 *	@param	monu		モニュメントタイプ
 *
 *	@return	出来たワーク
 */
//-----------------------------------------------------------------------------
WFLBY_LIGHTWK* WFLBY_LIGHT_Init( u32 heapID, WFLBY_LIGHT_NEON_ROOMTYPE roomtype, WFLBY_LIGHT_NEON_FLOORTYPE floor, WFLBY_LIGHT_NEON_MONUTYPE monu )
{
	WFLBY_LIGHTWK* p_wk;
	int i;

	p_wk = sys_AllocMemory( heapID, sizeof(WFLBY_LIGHTWK) );
	memset( p_wk, 0, sizeof(WFLBY_LIGHTWK) );

	WFLBY_LIGHT_Ful_SetLIGHTDATA_ROOM( &p_wk->now, &sc_WFLBY_LIGHTDATA_ROOM[ roomtype ] );
	WFLBY_LIGHT_Ful_SetLIGHTDATA_FLOOR( &p_wk->now, &sc_WFLBY_LIGHTDATA_FLOOR[ floor ] );

	for( i=0; i<WFLBY_LIGHT_LIGHT_NUM; i++ ){
		p_wk->lightcount[i] = WFLBY_LIGHT_FADE_COUNT;
	}
	for( i=0; i<WFLBY_LIGHT_MAT_NUM; i++ ){
		p_wk->matcount[i] = WFLBY_LIGHT_FADE_COUNT;
	}

	WFLBY_LIGHT_Ful_SetLight( &p_wk->now );
	
	return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ライト破棄
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_LIGHT_Exit( WFLBY_LIGHTWK* p_wk )
{
	sys_FreeMemoryEz( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ライトメイン
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_LIGHT_Main( WFLBY_LIGHTWK* p_wk )
{
	int i;
	BOOL set_light = FALSE;

	for( i=0; i<WFLBY_LIGHT_LIGHT_NUM;  i++ ){
		if( (p_wk->lightcount[i]+1) <= WFLBY_LIGHT_FADE_COUNT ){
			p_wk->lightcount[i] ++;
			WFLBY_LIGHT_Col_Marge( &p_wk->last.lightcolor_ful[i],  &p_wk->next.lightcolor_ful[i], 
					&p_wk->now.lightcolor_ful[i], p_wk->lightcount[i], WFLBY_LIGHT_FADE_COUNT );
			set_light = TRUE;
		}
	}

	for( i=0; i<WFLBY_LIGHT_MAT_NUM;  i++ ){
		if( (p_wk->matcount[i]+1) <= WFLBY_LIGHT_FADE_COUNT ){
			p_wk->matcount[i] ++;
			WFLBY_LIGHT_Col_Marge( &p_wk->last.matcolor_ful[i],  &p_wk->next.matcolor_ful[i], 
					&p_wk->now.matcolor_ful[i], p_wk->matcount[i], WFLBY_LIGHT_FADE_COUNT );
			set_light = TRUE;
		}
	}

	if( set_light ){
		WFLBY_LIGHT_Ful_SetLight( &p_wk->now );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	部屋ネオン設定
 *
 *	@param	p_wk		ワーク
 *	@param	roomtype	部屋タイプ
 */
//-----------------------------------------------------------------------------
void WFLBY_LIGHT_SetRoom( WFLBY_LIGHTWK* p_wk, WFLBY_LIGHT_NEON_ROOMTYPE roomtype )
{
	int i;
	
	// ラストに今までの物を設定
	// カウント設定
	p_wk->last.lightcolor_ful[0]	= p_wk->now.lightcolor_ful[0];
	p_wk->lightcount[0]				= 0;
	WFLBY_LIGHT_ConvetNitroToLightCol( &p_wk->next.lightcolor_ful[0], 
			&sc_WFLBY_LIGHTDATA_ROOM[ roomtype ].lightcolor );

	for( i=0; i<WFLBY_LIGHT_MAT_NUM; i++ ){
		p_wk->last.matcolor_ful[i]		= p_wk->now.matcolor_ful[i];
		p_wk->matcount[i]				= 0;
		WFLBY_LIGHT_ConvetNitroToLightCol( &p_wk->next.matcolor_ful[i], 
				&sc_WFLBY_LIGHTDATA_ROOM[ roomtype ].matcolor[i] );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	床ネオン設定
 *
 *	@param	p_wk		ワーク
 *	@param	floor		床ネオン設定
 */
//-----------------------------------------------------------------------------
void WFLBY_LIGHT_SetFloor( WFLBY_LIGHTWK* p_wk, WFLBY_LIGHT_NEON_FLOORTYPE floor )
{
	p_wk->last.lightcolor_ful[1]	= p_wk->now.lightcolor_ful[1];
	p_wk->lightcount[1]				= 0;
	WFLBY_LIGHT_ConvetNitroToLightCol( &p_wk->next.lightcolor_ful[1], 
			&sc_WFLBY_LIGHTDATA_FLOOR[ floor ].lightcolor );
}

//----------------------------------------------------------------------------
/**
 *	@brief	モニュメントネオン設定
 *
 *	@param	p_wk		ワーク
 *	@param	monu		モニュメントネオン設定
 */
//-----------------------------------------------------------------------------
void WFLBY_LIGHT_SetMonu( WFLBY_LIGHTWK* p_wk, WFLBY_LIGHT_NEON_MONUTYPE monu )
{
}






//-----------------------------------------------------------------------------
/**
 *		プライベート関数
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	カラーコンバート
 *
 *	@param	cp_data		カラーデータ
 *	@param	p_col		NitroRGB
 */
//-----------------------------------------------------------------------------
static void WFLBY_LIGHT_ConvetLightColToNitro( const WFLBY_LIGHT_COL* cp_data, GXRgb* p_col )
{
	WFLBY_DEBUG_LIGHT_PRINT( "red=%d	green=%d	blue=%d\n", cp_data->color[ WFLBY_LIGHT_COL_R ], cp_data->color[ WFLBY_LIGHT_COL_G ], cp_data->color[ WFLBY_LIGHT_COL_B ] );
	
	*p_col = GX_RGB( cp_data->color[ WFLBY_LIGHT_COL_R ],
			cp_data->color[ WFLBY_LIGHT_COL_G ],
			cp_data->color[ WFLBY_LIGHT_COL_B ] );
}
static void WFLBY_LIGHT_ConvetNitroToLightCol( WFLBY_LIGHT_COL* p_data, const GXRgb* cp_col )
{
	p_data->color[ WFLBY_LIGHT_COL_R ] = ((*cp_col) & GX_RGB_R_MASK) >> GX_RGB_R_SHIFT;
	p_data->color[ WFLBY_LIGHT_COL_G ] = ((*cp_col) & GX_RGB_G_MASK) >> GX_RGB_G_SHIFT;
	p_data->color[ WFLBY_LIGHT_COL_B ] = ((*cp_col) & GX_RGB_B_MASK) >> GX_RGB_B_SHIFT;
}

//----------------------------------------------------------------------------
/**
 *	@brief	カラーMerge	データ１からデータ２にする
 *
 *	@param	cp_data0		データ１
 *	@param	cp_data1		データ２
 *	@param	p_datain		格納先
 *	@param	count			カウンタ
 *	@param	countmax		カウンタMAX
 */
//-----------------------------------------------------------------------------
static void WFLBY_LIGHT_Col_Marge( const WFLBY_LIGHT_COL* cp_data0, const WFLBY_LIGHT_COL* cp_data1, WFLBY_LIGHT_COL* p_datain, s32 count, s32 countmax )
{
	s32 r;
	s32 g;
	s32 b;
	
	r = cp_data1->color[ WFLBY_LIGHT_COL_R ] - cp_data0->color[ WFLBY_LIGHT_COL_R ];
	g = cp_data1->color[ WFLBY_LIGHT_COL_G ] - cp_data0->color[ WFLBY_LIGHT_COL_G ];
	b = cp_data1->color[ WFLBY_LIGHT_COL_B ] - cp_data0->color[ WFLBY_LIGHT_COL_B ];
	
	p_datain->color[ WFLBY_LIGHT_COL_R ] = cp_data0->color[ WFLBY_LIGHT_COL_R ] + ( (count * r) / countmax );
	p_datain->color[ WFLBY_LIGHT_COL_G ] = cp_data0->color[ WFLBY_LIGHT_COL_G ] + ( (count * g) / countmax );
	p_datain->color[ WFLBY_LIGHT_COL_B ] = cp_data0->color[ WFLBY_LIGHT_COL_B ] + ( (count * b) / countmax );
}



//----------------------------------------------------------------------------
/**
 *	@brief	ライトデータの設定
 *
 *	@param	p_data			ワーク
 *	@param	cp_lightdata	ライトデータ
 */
//-----------------------------------------------------------------------------
static void WFLBY_LIGHT_Ful_SetLIGHTDATA_ROOM( WFLBY_LIGHT_FULL* p_data, const WFLBY_LIGHTDATA_ROOM* cp_lightdata )
{
	int i;

	WFLBY_LIGHT_ConvetNitroToLightCol( &p_data->lightcolor_ful[0], &cp_lightdata->lightcolor );

	for( i=0; i<WFLBY_LIGHT_MAT_NUM;  i++ ){
		WFLBY_LIGHT_ConvetNitroToLightCol( &p_data->matcolor_ful[i], &cp_lightdata->matcolor[i] );
	}
}
// 床ライト用
static void WFLBY_LIGHT_Ful_SetLIGHTDATA_FLOOR( WFLBY_LIGHT_FULL* p_data, const WFLBY_LIGHTDATA_FLOOR* cp_lightdata )
{
	WFLBY_LIGHT_ConvetNitroToLightCol( &p_data->lightcolor_ful[1], &cp_lightdata->lightcolor );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ライトデータ設定
 *
 *	@param cp_data0			データ
 */
//-----------------------------------------------------------------------------
static void WFLBY_LIGHT_Ful_SetLight( const WFLBY_LIGHT_FULL* cp_data0 )
{
	int i;
	GXRgb rgb, rgb2;

	for( i=0; i<WFLBY_LIGHT_LIGHT_NUM;  i++ ){
		WFLBY_DEBUG_LIGHT_PRINT( "light %d color\n", i );
		WFLBY_LIGHT_ConvetLightColToNitro( &cp_data0->lightcolor_ful[i], &rgb  );
		NNS_G3dGlbLightColor( i, rgb );
	}
	WFLBY_DEBUG_LIGHT_PRINT( "diff\n" );
	WFLBY_LIGHT_ConvetLightColToNitro( &cp_data0->matcolor_ful[0], &rgb  );
	WFLBY_DEBUG_LIGHT_PRINT( "amb\n" );
	WFLBY_LIGHT_ConvetLightColToNitro( &cp_data0->matcolor_ful[1], &rgb2  );
	NNS_G3dGlbMaterialColorDiffAmb( rgb, rgb2,  FALSE );
	
	WFLBY_DEBUG_LIGHT_PRINT( "spec\n" );
	WFLBY_LIGHT_ConvetLightColToNitro( &cp_data0->matcolor_ful[2], &rgb  );
	WFLBY_DEBUG_LIGHT_PRINT( "emi\n" );
	WFLBY_LIGHT_ConvetLightColToNitro( &cp_data0->matcolor_ful[3], &rgb2  );
	NNS_G3dGlbMaterialColorSpecEmi( rgb, rgb2,  FALSE );
}


