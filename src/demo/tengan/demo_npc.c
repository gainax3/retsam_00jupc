
#include "common.h"
#include "gflib/camera.h"

#include "system/procsys.h"
#include "system/msgdata.h"
#include "system/font_arc.h"
#include "system/brightness.h"
#include "system/arc_tool.h"
#include "system/clact_tool.h"

#include "system/laster.h"
#include "system/wipe.h"

#include "demo_tengan_common.h"

//==============================================================
// Prototype
//==============================================================
extern void Demo_Anime_Start( T3D_MODEL* mdl, MDL_CMD_TBL* cmd );
extern BOOL Demo_Anime_EndCheck( T3D_MODEL* mdl );
extern void Demo_Anime( T3D_MODEL* mdl );
extern void Demo_Anime_Command( T3D_MODEL* mdl );

#define MDL_ANIME_FRAME		( 4 )
#define MDL_ANIME_COMMAND	( 4 )	///< 4個で１つのanime

static const anime_tbl[][ 5 ] = {
	{  1,  2,  3,  2, 0xFF },		///< 上
	{  5,  6,  7,  6, 0xFF },		///< 下
	{  9, 10, 11, 10, 0xFF },		///< 左
	{ 13, 14, 15, 14, 0xFF },		///< 右
	{  0,  0,  0,  0, 0xFF },		///< 上待機
	{  4,  4,  4,  4, 0xFF },		///< 下待機
	{ 10, 10, 10, 10, 0xFF },		///< 左待機
	{ 14, 14, 14, 14, 0xFF },		///< 右待機
	
	{  0,  0,  0,  0, 0xFF },		///< 上待機
	
	{ 0,1,2,3, 0xFF },
	{ 4,5,6,7, 0xFF },
	{ 8,9,10,11, 0xFF },
	{ 12,13,14,15, 0xFF },
};

//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	mdl	
 * @param	cmd	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void Demo_Anime_Start( T3D_MODEL* mdl, MDL_CMD_TBL* cmd )
{
	mdl->bAnime = TRUE;
	mdl->cmd_table	   = cmd;
	mdl->cmd_index	   = 0;
	mdl->cmd_count	   = cmd[ mdl->cmd_index ].count;
	mdl->cmd_count_max = cmd[ mdl->cmd_index ].count;
	mdl->anm_no	   	   = cmd[ mdl->cmd_index ].anime;
}

//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	mdl	
 *
 * @retval	BOOL	
 *
 */
//--------------------------------------------------------------
BOOL Demo_Anime_EndCheck( T3D_MODEL* mdl )
{
	return ( mdl->cmd_index == 0xFF ) ? TRUE : FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	mdl	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void Demo_Anime( T3D_MODEL* mdl )
{
	int  old;
	int  now;
	int anime;
	fx32 set;
	fx32 old_f = D3DOBJ_AnmGet( &mdl->anm[ 0 ] );
	fx32 now_f;
	
	old = old_f >> FX32_SHIFT;
	old %= MDL_ANIME_FRAME;
	
	now_f = old_f + mdl->anm_speed;
	now = now_f >> FX32_SHIFT;
	now %= MDL_ANIME_FRAME;
	
	anime = anime_tbl[ mdl->anm_no - 1 ][ mdl->index ];
	
	if ( anime == 0xFF ){ return; }

	if ( now != old ){		
		mdl->index++;
		set = FX32_CONST( anime * MDL_ANIME_FRAME );
	}
	else {		
		set = now_f;
	}
	
	D3DOBJ_AnmSet( &mdl->anm[ 0 ], set );
}


//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	mdl	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void Demo_Anime_Command( T3D_MODEL* mdl )
{
	int param;
	MDL_CMD_TBL* cmd = mdl->cmd_table;
	
	if ( mdl->cmd_count == 0xFF || mdl->cmd_index == 0xFF ){ return; }
	
	param = cmd[ mdl->cmd_index ].anime;
	
	if ( param == eCMD_NONE ){
		mdl->bAnime	   = FALSE;
		mdl->cmd_index = 0xFF;
		mdl->cmd_count = 0xFF;
		mdl->cmd_count_max = 0xFF;
	}
	else {		
		if ( mdl->index >= MDL_ANIME_COMMAND ){
			
			mdl->cmd_count--;
			mdl->index = 0;
			
			if ( mdl->cmd_count == 0 ){
				mdl->cmd_index++;
				mdl->anm_no = cmd[ mdl->cmd_index ].anime;
				mdl->cmd_count = cmd[ mdl->cmd_index ].count;				
				mdl->cmd_count_max = cmd[ mdl->cmd_index ].count;				
				
				if ( mdl->anm_no == eCMD_NONE ){
					mdl->bAnime	   = FALSE;
					mdl->cmd_index = 0xFF;
					mdl->cmd_count = 0xFF;
					mdl->cmd_count_max = 0xFF;
				}
			}
		}
		else {
			if ( mdl->index == 0 && ( mdl->cmd_count == mdl->cmd_count_max ) ){ return; }
			switch( mdl->anm_no ){
			case eCMD_UM:
			case eCMD_DM:
			case eCMD_UWB:
				mdl->obj.matrix.z -= cmd[ mdl->cmd_index ].speed;
				break;
			case eCMD_RM:
			case eCMD_LM:
				mdl->obj.matrix.x += cmd[ mdl->cmd_index ].speed;
				break;
			}
		}
//		mdl->anm_no = cmd[ mdl->cmd_index ].anime;
	}
}

