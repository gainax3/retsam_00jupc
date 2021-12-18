//==============================================================================================
/**
 * @file	factory_3d.c
 * @brief	「バトルファクトリー」3D
 * @author	Satoshi Nohara
 * @date	2007.03.20
 */
//==============================================================================================
#include "common.h"
#include "src_os_print.h"
#include "system/main.h"
#include "system/arc_tool.h"
#include "system/arc_util.h"
#include "system/clact_tool.h"
#include "system/buflen.h"
#include "system/wordset.h"
#include "system/softsprite.h"

#include "factory_sys.h"


//==============================================================================================
//
//	プロトタイプ宣言
//
//==============================================================================================
void Factory_SimpleSetUp( void );
void Factory_3DMain( SOFT_SPRITE_MANAGER* ssm_p );
POKEMON_PARAM* Factory_PokeMake( u16 monsno, u8 level, u16 itemno, int place_id, int ground_id );
SOFT_SPRITE* Factory_SoftSpritePokeAdd( SOFT_SPRITE_MANAGER *ssm_p, int poly_id, POKEMON_PARAM* pp, int x, int y, int z );
void Factory_PokeHFlip( SOFT_SPRITE* ss, int flag );


//==============================================================================================
//
//	関数
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief   3Dシンプルセットアップ
 *
 * @param	none
 *
 * @retval  none
 */
//--------------------------------------------------------------
void Factory_SimpleSetUp( void )
{
	//３Ｄ使用面の設定(表示＆プライオリティー)
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
    G2_SetBG0Priority( 0 );

	//各種描画モードの設定(シェード＆アンチエイリアス＆半透明)
    G3X_SetShading( GX_SHADING_TOON );
    G3X_AntiAlias( TRUE );
	G3X_AlphaTest( FALSE, 0 );			//アルファテスト　　オフ
	G3X_AlphaBlend( TRUE );				//アルファブレンド　オン
	G3X_EdgeMarking( FALSE );
	G3X_SetFog( FALSE, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x8000, 0 );

	//クリアカラーの設定
    G3X_SetClearColor( GX_RGB(0,0,0), 0, 0x7fff, 63, FALSE);	//color,alpha,depth,polygonID,fog

	//ビューポートの設定
    G3_ViewPort( 0, 0, 255, 191 );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	3Dメイン処理
 *
 * @param   ssm_p	ソフトウェアスプライトマネージャへのポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
void Factory_3DMain( SOFT_SPRITE_MANAGER* ssm_p )
{
	GF_G3X_Reset();
	//GFC_CameraLookAt();

	G3_MtxMode( GX_MTXMODE_PROJECTION );
	G3_Identity();
	G3_MtxMode( GX_MTXMODE_POSITION_VECTOR );
	G3_Identity();

	NNS_G3dGlbFlush();

	NNS_G2dSetupSoftwareSpriteCamera();

	//メインループで呼び出し
	SoftSpriteMain( ssm_p );

	G3_SwapBuffers( GX_SORTMODE_MANUAL, GX_BUFFERMODE_Z );

	return;
}

//------------------------------------------------------------------
/**
 * @brief	ポケモン生成処理
 *
 * @param	monsno
 * @param	level
 * @param	itemno
 * @param	place_id
 * @param	ground_id
 *
 * @retval	"ポケモンのポインタ"
 */
//------------------------------------------------------------------
POKEMON_PARAM* Factory_PokeMake( u16 monsno, u8 level, u16 itemno, int place_id, int ground_id )
{
	POKEMON_PARAM* poke_param;
	u32 buf;

	poke_param = PokemonParam_AllocWork( HEAPID_FACTORY );
	PokeParaInit( poke_param );
	PokeParaSet( poke_param, monsno, level, POW_RND, RND_NO_SET, 0, ID_NO_SET, 0 );

	//PokeParaGetInfoSet( poke_param, SaveData_GetMyStatus(sv), 
	//						ITEM_MONSUTAABOORU, place_id, ground_id, heapID );
	
	//buf = itemno;
	//PokeParaPut( poke_param, ID_PARA_item, &buf );
	
	return poke_param;
}

//--------------------------------------------------------------
/**
 * @brief   ポケモンソフトウェアスプライト生成関数
 *
 * @param   ssm_p		ソフトウェアスプライトマネージャへのポインタ
 * @param   poly_id		ポリゴンID
 * @param   pp			POKEMON_PARAMへのポインタ
 * @param   dir			ポケモンの向き(PARA_FRONT or PARA_BACK)
 * @param   heap_id		ヒープID(一時使用)
 * @param   x			表示座標X
 * @param   y			表示座標Y
 * @param   z			表示座標Z
 *
 * @retval  ソフトウェアスプライトのポインタ
 */
//--------------------------------------------------------------
SOFT_SPRITE* Factory_SoftSpritePokeAdd( SOFT_SPRITE_MANAGER *ssm_p, int poly_id, POKEMON_PARAM* pp, int x, int y, int z )
{
	SOFT_SPRITE_ARC ssa;
	SOFT_SPRITE	*ss;
	int height;
	u32 monsno;
	
	PokeGraArcDataGetPP( &ssa, pp, PARA_FRONT );

	//足元を合わせる処理
	//height = PokeParaHeightGet( pp, PARA_FRONT );			//高さ情報取得
	height = 0;

	ss = SoftSpriteAdd( ssm_p, &ssa, x, (y + height), z, poly_id, NULL, NULL );
	return ss;
}

//--------------------------------------------------------------
/**
 * @brief   ポケモン反転
 *
 * @param	ss		SOFT_SPRITE型のポインタ
 * @param	flag	左右反転させるか
 *
 * @retval  none
 */
//--------------------------------------------------------------
void Factory_PokeHFlip( SOFT_SPRITE* ss, int flag )
{
	SoftSpriteParaSet( ss, SS_PARA_H_FLIP, flag );	//左右反転フラグ
	return;
}	


