//============================================================================================
/**
 * @file	fieldmap.c
 * @bfief	ƒtƒB[ƒ‹ƒhƒ}ƒbƒv
 * @author	GAME FREAK inc.
 */
//============================================================================================

#include "common.h"
#include "system/bmp_list.h"
#include "system/bmp_menu.h"
#include "system/palanm.h"
#include "system/pm_rtc.h"
#include "talk_msg.h"
#include "vram_transfer_manager.h"
#include "vram_transfer_anm.h"
#include "fieldsys.h"
#include "fieldmap_work.h"
#include "sxy.h"
#include "fld_debug.h"
#include "worldmap.h"
#include "mapresource.h"
#include "system/snd_tool.h"
#include "fld_bgm.h"
#include "div_map.h"
#include "system/pm_overlay.h"

#include "camera_move_test.h"
#include "field_light.h"
#include "field_anime.h"
#include "field_glb_state.h"
#include "field_event.h"
#include "fieldobj.h"
#include "weather_sys.h"

#include "player.h"
#include "field_anime.h"
#include "effect_uground.h"

#include "system/render_oam.h"
#include "system/arc_util.h"

#include "debug_saito.h"

#include "fieldmap.h"

#include "mapdefine.h"
#include "zonedata.h"
#include "eventdata.h"

#include "field_3d_anime.h"
#include "place_name.h"	//’n–¼•/¦

#include "field_effect.h"

#include "script.h"

#include "field_subscreen.h"

#include "situation_local.h"	//Situation_GetTMFootMark
#include "townmap_footmark.h"	//TMFootMark_Update
#include "honey_tree_enc.h"		//HTE_SetHoneyTreeAnimeCallBack

#include "field_camera.h"
#include "ev_time.h"
#include "ev_mapchange.h"		//MapChg_UpdateGameData

#include "fieldmap_func.h"
#include "field_gimmick.h"
#include "field_poketch.h"

#include "system/wipe.h"		//for wipe

#include "ug_base_layout.h"
#include "mapdata_weather.h"
#include "comm_player.h"    //ƒƒjƒ…[‚©‚ç•œ‹A‚µ‚½‚ÌFIELDOBJˆÊ’u

#include "quick_sand.h"

#include "system/pm_debug_wifi.h"

#ifdef PM_DEBUG
#include "debug_3d_draw_count.h"
#endif


// WIFI@‘ÎíAUTOÓ°ÄŞƒfƒoƒbƒN
#ifdef _WIFI_DEBUG_TUUSHIN

extern WIFI_DEBUG_BATTLE_WK WIFI_DEBUG_BATTLE_Work;
#endif	//_WIFI_DEBUG_TUUSHIN

// ”j‚ê‚½¢ŠE
#include "include/savedata/gimmickwork.h"
#include "field_gimmick_def.h"
#include "field_tornworld.h"

//============================================================================================
//	’è”’è‹`
//============================================================================================
#define TEST_OBJCHAR_32K	//’è‹`‚ÅOBJ1024ƒLƒƒƒ‰‰» kaga
//#define TEST_BGMODE_1		//’è‹`‚ÅBGƒ‚[ƒh1‚Ö

#define	BG_CLEAR_COLOR		( 0x0000 )

#define	FLD_SET_FIELD_ANIME		( 1 )
#define	FLD_SET_DIVMAP_LOAD		( 2 )
#define	FLD_SET_MAP3D_WRITE		( 4 )
#define	FLD_SET_FIELD_3D_ANIME	( 8 )

#define	SET_FUNC_CHECK(p,flg)	( p & flg )

//** Vram“]‘—ƒ}ƒl[ƒWƒƒ[ƒ^ƒXƒN” **//
#define VRAM_TRANFER_TASK_NUM	(128)

//** CharManager PlttManager—p **//
#define FLD_CHAR_CONT_NUM				(20)

#ifdef TEST_OBJCHAR_32K
#define FLD_CHAR_VRAMTRANS_MAIN_SIZE	(0x8000)
#else
#define FLD_CHAR_VRAMTRANS_MAIN_SIZE	(0x4000)
#endif

#define FLD_CHAR_VRAMTRANS_SUB_SIZE		(0x4000)
#define FLD_PLTT_CONT_NUM				(20)

//•ªŠ„ƒ}ƒbƒvƒf[ƒ^‚Ì’nŒ`—pƒƒ‚ƒŠ‚Æ‚‚³ƒf[ƒ^—pƒƒ‚ƒŠ‚ÌƒTƒCƒY’è‹`
#define GROUND_MEM_SIZE			(0xf000)
#define HEIGHT_MEM_SIZE			(0x9000)
#define UNDER_GROUND_MEM_SIZE	(0xa800)
#define UNDER_HEIGHT_MEM_SIZE	(0x0)

//ƒvƒƒWƒFƒNƒVƒ‡ƒ“ƒ}ƒgƒŠƒNƒX‚ğ‘€ì‚·‚éÛ‚Ì‚yƒIƒtƒZƒbƒg
#define	PRO_MAT_Z_OFS	(4)

//Šg’£‚‚³‚ÌŒÂ”
#define EX_HEIGHT_NUM	(8)

///fieldmap_sub‚Ì‰º‚É”z’u‚·‚éƒI[ƒo[ƒŒƒC
enum{
	FIELDMAP_SUBAPP_OVERLAY_GYM,		///<fieldmap_sub_gym ‚ğƒI[ƒo[ƒŒƒC
	FIELDMAP_SUBAPP_OVERLAY_FIELD,		///<fieldmap_sub_field ‚ğƒI[ƒo[ƒŒƒC
	FIELDMAP_SUBAPP_OVERLAY_HAKAI,		///<fieldmap_sub_hakai ‚ğƒI[ƒo[ƒŒƒC
};

//============================================================================================
//	ƒvƒƒgƒ^ƒCƒvéŒ¾
//============================================================================================
static void bg_set( GF_BGL_INI * ini );
static void bg_exit( GF_BGL_INI * ini );
static void join_clact_oam_init(void);				// ƒZƒ‹ƒAƒNƒ^[—p‹¤—LOAMƒ}ƒl[ƒWƒƒ‚Ì‰Šú‰»
static void clact_delete(void);		// ƒZƒ‹ƒAƒNƒ^[‚Ì”jŠü

static void field_glb_state_init(GLST_DATA_PTR glb);		// ƒfƒtƒHƒ‹ƒg‚ÌƒOƒ[ƒoƒ‹ƒXƒe[ƒg‚Ìİ’è

static void	Map3Dwrite(FIELDSYS_WORK * repw);

static void fldmap_update_msk_Init( FIELDSYS_WORK* p_fsys );



static void vram_bank_set(void);
static void debug_vram_bank_set(void);
static void char_pltt_manager_init(void);		// ƒLƒƒƒ‰ƒNƒ^EƒpƒŒƒbƒgƒ}ƒl[ƒWƒƒ[‚Ì‰Šú‰»
static void char_pltt_manager_delete(void);		// ƒLƒƒƒ‰ƒNƒ^EƒpƒŒƒbƒgƒ}ƒl[ƒWƒƒ[‚Ì”jŠü


static void FieldMapLoadSeq( FIELDSYS_WORK * repw );
static void FieldMapLoadSeqDivMap( FIELDSYS_WORK * repw );
static void FieldMapLoadSeqFeMoveObj( FIELDSYS_WORK * repw );
static void FieldMapLoadSeqRest( FIELDSYS_WORK * repw );

static PROC_RESULT FieldMapProc_Init(PROC * proc, int * seq);
static PROC_RESULT FieldMapProc_Main(PROC * proc, int * seq);
static PROC_RESULT FieldMapProc_End(PROC * proc, int * seq);

static BOOL FieldMap_ZoneChange(FIELDSYS_WORK * fsys);
static void FieldMap_Update(FIELDSYS_WORK * fsys, u8 flag);
static BOOL WatchPlayerPosition(FIELDSYS_WORK * fsys);

static void vBlankFunc(void * work);
static void UpdateFootMark(FIELDSYS_WORK * fsys);

static int FieldMapOverlay_SubAppLoad(FIELDSYS_WORK *fsys);

static BOOL GimmickTornWorldCheck( FIELDSYS_WORK *fsys );

//#define	OSP_HEAP_SIZE

#ifdef PM_DEBUG
void DebugDispTexBank(const u8 inNo);
#endif

//------------------------------------------------------------------
//	“®ìƒ‚ƒfƒ‹ƒŠƒXƒg§ŒäŠÖ˜A
//------------------------------------------------------------------
static MMDL_WORK * MMDL_Create(int heapID, int area_id);
static const int * MMDL_GetList(const MMDL_WORK * mmdl);
static const int MMDL_GetListSize(const MMDL_WORK * mmdl);
static void MMDL_Delete(MMDL_WORK * mmdl);

FS_EXTERN_OVERLAY(fieldmap_sub);
FS_EXTERN_OVERLAY(fieldmap_sub_gym);
FS_EXTERN_OVERLAY(fieldmap_sub_field);
FS_EXTERN_OVERLAY(fieldmap_sub_hakai);

//============================================================================================
//	ƒOƒ[ƒoƒ‹•Ï”
//============================================================================================


//============================================================================================
//
//
//				ŠÖ”
//
//
//============================================================================================

//----------------------------------------------------------------------------
/**
 *	@brief	ƒtƒB[ƒ‹ƒhƒIƒuƒWƒF—pƒrƒ‹ƒ{[ƒhƒAƒNƒ^[Vƒuƒ‰ƒ“ƒNˆ—
 */
//-----------------------------------------------------------------------------
static inline void fldobj_BlactVblank( FIELDSYS_WORK * fsys )
{
	FIELD_OBJ_BLACT_CONT *conot = FieldOBJSys_BlActContGet( fsys->fldobjsys );
	BLACT_SET_PTR blactset = FieldOBJ_BlActCont_SetPtrGet( conot );
	BLACT_VBlankFunc( blactset );
}

//------------------------------------------------------------------
/**
 * @brief	ƒtƒB[ƒ‹ƒh—pVBLANKŠÖ”
 * @param	work
 */
//------------------------------------------------------------------
static void vBlankFunc(void * work)
{
	FIELDSYS_WORK * fsys = work;

	// BG‘‚«Š·‚¦
	GF_BGL_VBlankFunc( fsys->bgl );

	// Vram“]‘—ƒ}ƒl[ƒWƒƒ[Às
	DoVramTransferManager();

	// ƒŒƒ“ƒ_ƒ‰‹¤—LOAMƒ}ƒl[ƒWƒƒVram“]‘—
	REND_OAMTrans();	

	// ”jŠüƒ|ƒŠƒSƒ“‚ÌŠ®‘S•`‰æ”jŠü
	// ‚ğ’Ê’m
	fldobj_BlactVblank( fsys );
}

//============================================================================================
//
//
//		ƒvƒƒZƒXŠÖ”
//
//
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	ƒvƒƒZƒXŠÖ”F‰Šú‰»FƒtƒB[ƒ‹ƒhƒ}ƒbƒv
 */
//------------------------------------------------------------------
static PROC_RESULT FieldMapProc_Init(PROC * proc, int * seq)
{
	u16 bgm_no;
	FIELDSYS_WORK * fsys;
	PROC_RESULT pr = PROC_RES_CONTINUE;
	
	fsys = PROC_GetParentWork(proc);

	switch (*seq) {
	case 0:
		sys_VBlankFuncChange(NULL, NULL);
		sys_HBlankIntrStop();	//HBlankŠ„‚è‚İ’â~
		G2_BlendNone();//“ÁêŒø‰Ê–³Œø‰»
		G2S_BlendNone();//“ÁêŒø‰Ê–³Œø‰»

// WIFI@‘ÎíAUTOÓ°ÄŞƒfƒoƒbƒN
#ifdef _WIFI_DEBUG_TUUSHIN
		WIFI_DEBUG_BATTLE_Work.DEBUG_WIFI_SEQ = 1;
		WIFI_DEBUG_BATTLE_Work.DEBUG_WIFI_A_REQ = TRUE;
#endif	//_WIFI_DEBUG_TUUSHIN

		//ƒƒCƒvƒEƒBƒ“ƒhƒEƒ}ƒXƒNƒŠƒZƒbƒg
		WIPE_ResetWndMask( WIPE_DISP_MAIN );
		WIPE_ResetWndMask( WIPE_DISP_SUB );

		// ƒtƒB[ƒ‹ƒhƒ}ƒbƒvƒAƒbƒvƒf[ƒgƒ}ƒXƒN‰Šú‰»
		fldmap_update_msk_Init( fsys );

		//‰æ–Êİ’è
		MapChg_Set3DDisplay(fsys);
		
		if (fsys->MapModeData->UseOverlayFlag) {
		//ƒtƒB[ƒ‹ƒhƒTƒuƒI[ƒo[ƒŒƒC—Ìˆæ‚ğ“Ç‚İ‚İ
			Overlay_Load(FS_OVERLAY_ID(fieldmap_sub), OVERLAY_LOAD_NOT_SYNCHRONIZE);
			
			switch(FieldMapOverlay_SubAppLoad(fsys)){
			case FIELDMAP_SUBAPP_OVERLAY_GYM:
				Overlay_Load(FS_OVERLAY_ID(fieldmap_sub_gym), OVERLAY_LOAD_NOT_SYNCHRONIZE);
				break;
			case FIELDMAP_SUBAPP_OVERLAY_FIELD:
				Overlay_Load(FS_OVERLAY_ID(fieldmap_sub_field), OVERLAY_LOAD_NOT_SYNCHRONIZE);
				break;
			case FIELDMAP_SUBAPP_OVERLAY_HAKAI:
				Overlay_Load(FS_OVERLAY_ID(fieldmap_sub_hakai), OVERLAY_LOAD_NOT_SYNCHRONIZE);
				break;
			}
		}
		//ƒtƒB[ƒ‹ƒhƒq[ƒvì¬
		sys_CreateHeap( HEAPID_BASE_APP, HEAPID_FIELD, fsys->MapModeData->HeapSize );
		GF_ASSERT(fsys->fldmap == NULL);
		fsys->fldmap = sys_AllocMemory(HEAPID_FIELD, sizeof(FIELDMAP_WORK));
		MI_CpuClear8(fsys->fldmap, sizeof(FIELDMAP_WORK));
		fsys->fldmap->fmapfunc_sys = FLDMAPFUNC_Sys_Create(fsys, HEAPID_FIELD, 8);

		vram_bank_set();				// ‚u‚q‚`‚lƒoƒ“ƒNİ’è
		initVramTransferManagerHeap( VRAM_TRANFER_TASK_NUM, HEAPID_FIELD );// ‚u‚q‚`‚l“]‘—ƒ}ƒl[ƒWƒƒ[‚ğ‰Šú‰»
		BLACT_InitSys(FIELDSYS_BLACTSET_NUM, HEAPID_FIELD);	// ƒrƒ‹ƒ{[ƒhƒAƒNƒ^[ƒVƒXƒeƒ€‰Šú‰»
		simple_3DBGInit(HEAPID_FIELD);
		char_pltt_manager_init();		// ƒLƒƒƒ‰ƒNƒ^EƒpƒŒƒbƒgƒ}ƒl[ƒWƒƒ[‚Ì‰Šú‰»
		join_clact_oam_init();// ƒtƒB[ƒ‹ƒh—pƒZƒ‹ƒAƒNƒ^[‰Šú‰»

		GF_Disp_DispSelect();

		fsys->bgl = GF_BGL_BglIniAlloc( HEAPID_FIELD );
		bg_set( fsys->bgl );			// ‚a‚fİ’è 
		FieldMsgPrintInit( PALTYPE_MAIN_BG, MSG_PRINT_INIT_ON );

		SpScriptSearch(fsys, SP_SCRID_INIT_CHANGE);

		DEBUG_ENTRY_STRESSPRINT_TASK(fsys);
		break;


	case 1:
		FieldMapLoadSeq( fsys );

		//FieldMapLoadSeq‚ÌŒã‚Éifsys->MapResource‚Ìƒƒ‚ƒŠŠm•Û‚ªÏ‚ñ‚Å‚©‚çjŒÄ‚Ô
		SetupMapResource(fsys->MapResource);
		//Šg’£”z’uƒ‚ƒfƒ‹
		fsys->Map3DObjExp = M3DO_AllocMap3DObjList(HEAPID_FIELD);

		//’n‰º”é–§Šî’nƒOƒbƒY”z’u(’nã‚É‚¢‚é‚Æ‚«‚ÍƒXƒ‹[‚µ‚Ü‚·)
		BaseLayout_SetupUGBaseGoodsObj(fsys);	
		FieldMapLoadSeqDivMap( fsys );
		FieldMapLoadSeqFeMoveObj( fsys );
		FieldMapLoadSeqRest( fsys );

		#ifdef TORNWORLD_WEATHER_OFF
		if( fsys->fldmap->weather_data != NULL ){
			u16 weather = Situation_GetWeatherID(
				SaveData_GetSituation(fsys->savedata));
			WEATHER_Set(fsys->fldmap->weather_data, weather);
		}
		#else
		{
			u16 weather = Situation_GetWeatherID(SaveData_GetSituation(fsys->savedata));
			WEATHER_Set(fsys->fldmap->weather_data, weather);
		}
		#endif
		
		//ƒtƒB[ƒ‹ƒh‰Šú‰»FƒTƒEƒ“ƒhƒf[ƒ^ƒZƒbƒg(ƒV[ƒ“‚ª•ÏX‚³‚ê‚È‚¢‚Í‰½‚à‚µ‚È‚¢)
		Snd_FieldMapInitBgmPlay( fsys, fsys->location->zone_id );

		//“ÁêƒXƒNƒŠƒvƒg‚É‚æ‚éOBJ§Œä
		SpScriptSearch(fsys, SP_SCRID_OBJ_CHANGE);
		// ƒtƒB[ƒ‹ƒh“à´Ìª¸ÄHBLANKƒ[ƒN‚Ìì¬‚Æ
		// HBLANKŠ„‚è‚İ‚ÌÀs	tomoya takahashi 
		fsys->fldmap->hblanksys = FLDHBLANK_SYS_Init( HEAPID_FIELD );
		FLDHBLANK_SYS_Start( fsys->fldmap->hblanksys );
		// “Å´Ìª¸Ä
		fsys->fldmap->poisoneffect = POISON_EFFE_Init( HEAPID_FIELD, fsys->fldmap->hblanksys );
		break;


	case 2:
		FieldSubScreen_Init(fsys);					//ƒTƒu‰æ–ÊŠJnˆ—ƒŠƒNƒGƒXƒg
		break;


	case 3:
		if (FieldSubScreen_WaitInit(fsys)) {		//ƒTƒu‰æ–ÊŠJnˆ—‘Ò‚¿
			fsys->main_mode_flag = TRUE;
			pr = PROC_RES_FINISH;
		}
		break;
	}

	(*seq)++;
	return pr;
}

//------------------------------------------------------------------
/**
 * @brief	ƒvƒƒZƒXŠÖ”FƒƒCƒ“FƒtƒB[ƒ‹ƒhƒ}ƒbƒv
 */
//------------------------------------------------------------------
static PROC_RESULT FieldMapProc_Main(PROC * proc, int * seq)
{
	FIELDSYS_WORK * fsys;
	fsys = PROC_GetParentWork(proc);

	//‚±‚Ì•”•ª‚Í©‹@‚Ì“®ìó‘Ô‚Å‚È‚­ƒJƒƒ‰ˆÚ“®‚È‚Ç‚Æ“¯‚¶‚­À•W‚¾‚¯‚Å§Œä‰Â”\‚É‚µ‚½‚¢B
	//i¡‚Íƒ][ƒ“Ø‘Ö‚ª©‹@“®ì‚ÉˆË‘¶‚µ‚·‚¬‚Ä‚¢‚é‚½‚ßj
	//if ( Player_MoveStateGet(fsys->player) == OBJ_MOVE_STATE_START ) {
	//
	//c‚Æ‚¢‚¤‚±‚Æ‚Å©‹@À•Wƒ`ƒFƒbƒN‚ÉˆÚs‚µ‚Ä‚İ‚½B2005.12.22	tamada
	if ( WatchPlayerPosition(fsys)) {
		SeedSys_CheckInScreen(fsys);
		UpdateFootMark(fsys);
		Field_SendPoketchInfo(fsys, POKETCH_SEND_MOVE_PLAYER, TRUE);
		if (FieldMap_ZoneChange(fsys)) {
			//SwayGrass_InitSwayGrass(fsys->SwayGrass);	//—h‚ê‘ƒNƒŠƒA
		}
	}

	FieldMap_Update(fsys, fsys->fldmap_update_msk );

	if (fsys->main_mode_flag) {
		return PROC_RES_CONTINUE;
	} else {
		return PROC_RES_FINISH;
	}
}


//------------------------------------------------------------------
/**
 * @brief	ƒvƒƒZƒXŠÖ”FI—¹FƒtƒB[ƒ‹ƒhƒ}ƒbƒv
 */
//------------------------------------------------------------------
static PROC_RESULT FieldMapProc_End(PROC * proc, int * seq)
{
	FIELDSYS_WORK * fsys;

	fsys = PROC_GetParentWork(proc);
	DivMapLoadMain(fsys, fsys->map_cont_dat);				//ƒ}ƒbƒvƒ[ƒhƒƒCƒ“ŠÖ”
	
	
	switch(*seq){
	case 0:
		//ƒMƒ~ƒbƒNI—¹
		FLDGMK_EndFieldGimmick(fsys);
		//ƒoƒCƒ“ƒh‘ÎÛ‚ğ‚Í‚ª‚·
		DivMapPurgeTarget(fsys->map_cont_dat);
		//©‹@Œ»İÀ•W‚Ì‘Ş”ğ
		fsys->location->grid_x = Player_NowGPosXGet( fsys->player );
		fsys->location->grid_z = Player_NowGPosZGet( fsys->player );
		fsys->location->dir = Player_DirGet( fsys->player );
		//Šg’£‚‚³ƒf[ƒ^—Ìˆæ‰ğ•ú
		EXH_FreeExHeightList(fsys->ExHeightList);
		
		{
			GF_ASSERT(fsys->field_3d_anime!=0);
			DivMapLoad_FreeMap(fsys->map_cont_dat);
		}
		
		RereaseFld3DAnimeAll(fsys->field_3d_anime);
		FreeFld3DAnimeManager(fsys->field_3d_anime);
		F3DASub_FreeAnimeContManager(&fsys->AnimeContMng);

		FieldAnimeAllRelease(fsys->fldmap->field_trans_anime);
		ReleaseFieldAnime(fsys->fldmap->field_trans_anime);
		fsys->fldmap->field_trans_anime = NULL;

		FieldOBJ_DrawProcPushAll( fsys->fldobjsys );			//•`‰æˆ—‘Ş”ğ
		FieldOBJSys_DrawDelete( fsys->fldobjsys );				//•`‰æˆ—íœ
		FieldOBJSys_MoveStopAll( fsys->fldobjsys );
		FE_Delete( fsys->fes );	//ƒtƒB[ƒ‹ƒhƒGƒtƒFƒNƒgíœ
		
		MMDL_Delete(fsys->mmdl);					// “®ìƒ‚ƒfƒ‹ƒŠƒXƒg‰ğ•ú
		fsys->mmdl = NULL;

		M3DO_FreeMap3DObjList(fsys->Map3DObjExp);

		(*seq)++;
		break;
	case 1:
		if(WaitMapFree(fsys->map_cont_dat) == TRUE){
			ReleaseMapResource(&fsys->MapResource);		// ƒ}ƒbƒvƒŠƒ\[ƒX‰ğ•ú
			FreeDivMapCont(fsys->map_cont_dat);

			HTE_FreeHoneyTreeDat(&fsys->HoneyTreePtr);//–¨–Øƒf[ƒ^‰ğ•ú

			FieldCameraEnd(fsys);					//ƒtƒB[ƒ‹ƒhƒJƒƒ‰I—¹
			
			DellLightCont(&fsys->light_cont_data);	// ƒ‰ƒCƒg”jŠü
			BoardWorkDelete( fsys->board );			// ŠÅ”Âƒ[ƒN‰ğ•ú
			FreePlaceNameCont(fsys->fldmap->place_name_cont);	//’n–¼•\¦\‘¢‘Ì‰ğ•ú
			#ifdef TORNWORLD_WEATHER_OFF
			if( fsys->fldmap->weather_data != NULL ){
				WEATHER_Delete(fsys->fldmap->weather_data);	// “V‹C”jŠü
			}
			#else
			WEATHER_Delete(fsys->fldmap->weather_data);	// “V‹C”jŠü
			#endif
			
			POISON_EFFE_Delete( fsys->fldmap->poisoneffect );// “Å´Ìª¸Ä
			FLDHBLANK_SYS_Delete( fsys->fldmap->hblanksys );// Hƒuƒ‰ƒ“ƒN
			SeedSys_Finish(fsys->fldmap->seedsys);
			FogSys_Delete(&fsys->fog_data);			// ƒtƒHƒOƒf[ƒ^
			GLST_Delete(&fsys->glst_data);			// ƒOƒ[ƒoƒ‹ƒXƒe[ƒgƒf[ƒ^ 
			
			clact_delete();							// ƒZƒ‹ƒAƒNƒ^[ƒf[ƒ^
			
//			FieldMessageWinDel(fsys->bgl);		// ƒƒbƒZ[ƒWƒEƒBƒ“ƒhƒEŠJ•ú
			bg_exit(fsys->bgl);					// BGLŠJ•ú

			FieldSubScreen_Quit(fsys);			//ƒTƒu‰æ–ÊI—¹ˆ—ƒŠƒNƒGƒXƒg
			(*seq) ++;
		}
		break;

	case 2:
		if (FieldSubScreen_WaitQuit(fsys)) {	//ƒTƒu‰æ–ÊI—¹ˆ—‘Ò‚¿
			char_pltt_manager_delete();	// ƒLƒƒƒ‰ƒNƒ^EƒpƒŒƒbƒgƒ}ƒl[ƒWƒƒ[‚Ì”jŠü
			BLACT_DestSys();		// ƒrƒ‹ƒ{[ƒhƒAƒNƒ^[ƒVƒXƒeƒ€‚ğ”jŠü
			DellVramTransferManager();
			simple_3DBGExit();
			FLDMAPFUNC_Sys_Delete(fsys->fldmap->fmapfunc_sys);
			sys_VBlankFuncChange(NULL, NULL);
			sys_FreeMemoryEz( fsys->bgl );
			sys_FreeMemoryEz( fsys->fldmap );
			fsys->fldmap = NULL;

// WIFI@‘ÎíAUTOÓ°ÄŞƒfƒoƒbƒN
#ifdef _WIFI_DEBUG_TUUSHIN
		WIFI_DEBUG_BATTLE_Work.DEBUG_WIFI_SEQ = 0;
#endif	//_WIFI_DEBUG_TUUSHIN

			//ƒtƒB[ƒ‹ƒhƒq[ƒv‰ğ•ú
			sys_DeleteHeap( HEAPID_FIELD );

			if (fsys->MapModeData->UseOverlayFlag) {
			//ƒtƒB[ƒ‹ƒhƒTƒuƒI[ƒo[ƒŒƒC—Ìˆæ‚ğƒAƒ“ƒ[ƒh
				Overlay_UnloadID(FS_OVERLAY_ID(fieldmap_sub));
				Overlay_UnloadID(FS_OVERLAY_ID(fieldmap_sub_gym));
				Overlay_UnloadID(FS_OVERLAY_ID(fieldmap_sub_field));
				Overlay_UnloadID(FS_OVERLAY_ID(fieldmap_sub_hakai));
			}

			return PROC_RES_FINISH;
		}
		break;

	}
	return PROC_RES_CONTINUE;
}


//------------------------------------------------------------------
/**
 * @brief	ƒvƒƒZƒX’è‹`ƒf[ƒ^FƒtƒB[ƒ‹ƒh
 */
//------------------------------------------------------------------
const PROC_DATA FieldProcData = {
	FieldMapProc_Init,
	FieldMapProc_Main,
	FieldMapProc_End,
	NO_OVERLAY_ID,
};

//--------------------------------------------------------------
/**
 * @brief   fieldmap_sub‚Ì‰º‚É•t‚¯‚éƒI[ƒo[ƒŒƒC‚ğ”»’è
 *
 * @param   fsys		
 *
 * @retval  FIELDMAP_SUBAPP_OVERLAY_???
 */
//--------------------------------------------------------------
static int FieldMapOverlay_SubAppLoad(FIELDSYS_WORK *fsys)
{
	//ƒMƒ~ƒbƒNƒ[ƒNæ“¾
	GIMMICKWORK *work = SaveData_GetGimmickWork(GameSystem_GetSaveData(fsys));
	//ƒMƒ~ƒbƒNƒR[ƒh‚ğæ“¾
	int gimmick_id = GIMMICKWORK_GetAssignID(work);
	
	if( gimmick_id == FLD_GIMMICK_NONE ){
		return FIELDMAP_SUBAPP_OVERLAY_FIELD;
	}
	
	if( gimmick_id == FLD_GIMMICK_TORNWORLD ){	
		return FIELDMAP_SUBAPP_OVERLAY_HAKAI;
	}
	
	return FIELDMAP_SUBAPP_OVERLAY_GYM;
}


//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	©‹@À•WXV‚ÌŠÄ‹
 * @param	fsys	ƒtƒB[ƒ‹ƒh§Œäƒ[ƒN‚Ö‚Ìƒ|ƒCƒ“ƒ^
 */
//------------------------------------------------------------------
static BOOL WatchPlayerPosition(FIELDSYS_WORK * fsys)
{
	int x,z;
	x = Player_NowGPosXGet( fsys->player );
	z = Player_NowGPosZGet( fsys->player );
	if (x != fsys->location->grid_x || z != fsys->location->grid_z) {
		fsys->location->grid_x = x;
		fsys->location->grid_z = z;
		return TRUE;
	} else {
		return FALSE;
	}
}

//------------------------------------------------------------------
/**
 * @brief	ƒtƒB[ƒ‹ƒh•à‚«‚Å‚Ìƒ][ƒ“Ø‘Ö
 * @param	fsys	ƒtƒB[ƒ‹ƒh§Œäƒ[ƒN‚Ö‚Ìƒ|ƒCƒ“ƒ^
 * @return	BOOL	TRUE‚Ì‚Æ‚«Aƒ][ƒ“‚ªØ‚è‘Ö‚í‚Á‚½
 */
//------------------------------------------------------------------
static BOOL FieldMap_ZoneChange(FIELDSYS_WORK * fsys)
{
	u32 new_zone_id;
	u32 old_zone_id;
	int x,z;
	SITUATION * sit;
	
	if( GimmickTornWorldCheck(fsys) == TRUE ){
		return( FALSE );
	}
	
	x = (Player_NowGPosXGet(fsys->player)-
		GetDivMapOriginGridX(fsys->map_cont_dat))/ BLOCK_GRID_W;
	z = (Player_NowGPosZGet(fsys->player)-
		GetDivMapOriginGridZ(fsys->map_cont_dat)) / BLOCK_GRID_H;
	new_zone_id = World_GetZoneIDFromMatrixXZ(fsys->World, x, z);
	old_zone_id = fsys->location->zone_id;
	//’¼‘O‚Ìƒ][ƒ“‚ÆŸ‚Ìƒ][ƒ“‚ª“¯‚¶ê‡‚Í‰½‚à‚µ‚È‚¢
	if (new_zone_id == old_zone_id) {
		return FALSE;
	}
	
	//++++++++++++++	ƒ][ƒ“XVˆ—			++++++++++++
	sit = SaveData_GetSituation(fsys->savedata);
	{
		//ƒƒP[ƒVƒ‡ƒ“XVi’n‘±‚«‚Ì•ÏˆÚ‚Ì‚½‚ßAƒ][ƒ“‚h‚c‚Ì‚İ‚ÌXV‚Æ‚È‚éBj
		fsys->location->zone_id = new_zone_id;
		//ƒ][ƒ“•Êƒf[ƒ^‚Ì“Ç‚İ‚İ
		EventData_LoadZoneData(fsys, new_zone_id);
		
		//ƒ}ƒbƒv‘JˆÚ‚Ìƒf[ƒ^XVˆ—
		MapChg_UpdateGameData(fsys, TRUE);
	}
	
	//++++++++++++++	‹Œƒ][ƒ““®ìƒ‚ƒfƒ‹íœˆ—	++++++++++++
	{
		int max = EventData_GetNpcCount( fsys );
		const FIELD_OBJ_H *head = EventData_GetNpcData( fsys );
		FieldOBJSys_ZoneUpdateDelete( fsys->fldobjsys, old_zone_id, new_zone_id, max, head );
	}
	
	//++++++++++++++	Vƒ][ƒ“ƒf[ƒ^’Ç‰Áˆ—	++++++++++++
	{
		SwayGrass_InitSwayGrass(fsys->SwayGrass);	//—h‚ê‘ƒNƒŠƒA
			
		//ƒtƒF[ƒhƒAƒEƒg ¨ ’Ç‰Á”gŒ`“Ç‚İ‚İ ¨ BGMÄ¶
		Snd_FadeOutNextPlayCall( fsys, Snd_FieldBgmNoGet(fsys,fsys->location->zone_id), 
									BGM_FADE_FIELD_MODE );

		;//OBJ“o˜^
		EventData_SetFieldOBJ(fsys);

		//“VŒóƒŠƒNƒGƒXƒg
		#ifdef TORNWORLD_WEATHER_OFF
		if( fsys->fldmap->weather_data != NULL ){
			WEATHER_ChengeReq(
				fsys->fldmap->weather_data, Situation_GetWeatherID(sit));
		}
		#else
		WEATHER_ChengeReq(
			fsys->fldmap->weather_data, Situation_GetWeatherID(sit));
		#endif
	}
	
	//’n–¼•\¦ƒŠƒNƒGƒXƒg
	{
		int place_id_old,place_id_new;
		int area_win_id;
		place_id_old = ZoneData_GetPlaceNameID(old_zone_id);
		place_id_new = ZoneData_GetPlaceNameID(new_zone_id);
		area_win_id	 = ZoneData_GetAreaWindowID(new_zone_id);		///< area_win_id ‚ğæ“¾‚µ‚Ä window ‚ğİ’è
		if (place_id_old != place_id_new){
			
			if ( area_win_id != 0 ){
				 area_win_id--;
			}			
			PlaceNameRequestEx(fsys->fldmap->place_name_cont, place_id_new, area_win_id);//’n–¼•\¦
		}
	}
	
	return TRUE;
}

//------------------------------------------------------------------
/**
 * @brief	ƒtƒB[ƒ‹ƒh•à‚«‚Å‚Ìƒ][ƒ“Ø‘Ö@”j‚ê‚½¢ŠEê—p
 * @param	fsys	ƒtƒB[ƒ‹ƒh§Œäƒ[ƒN‚Ö‚Ìƒ|ƒCƒ“ƒ^
 * @param	new_zone_id	V‹K‚Ìƒ][ƒ“ID
 * @return	nothing
 */
//------------------------------------------------------------------
void FieldMap_TornWorldZoneChange( FIELDSYS_WORK * fsys, u32 new_zone_id )
{
	u32 old_zone_id;
	SITUATION * sit;
	
	old_zone_id = fsys->location->zone_id;
	
	//++++++++++++++	ƒ][ƒ“XVˆ—			++++++++++++
	sit = SaveData_GetSituation(fsys->savedata);
	
	{
		//ƒƒP[ƒVƒ‡ƒ“XV(’n‘±‚«‚Ì•ÏˆÚ‚Ì‚½‚ßAƒ][ƒ“‚h‚c‚Ì‚İ‚ÌXV‚Æ‚È‚é
		fsys->location->zone_id = new_zone_id;
		//ƒ][ƒ“•Êƒf[ƒ^‚Ì“Ç‚İ‚İ
		EventData_LoadZoneData(fsys, new_zone_id);
		//ƒ}ƒbƒv‘JˆÚ‚Ìƒf[ƒ^XVˆ—
		MapChg_UpdateGameDataTornWorld( fsys, TRUE );
	}
	
	//++++++++++++++	‹Œƒ][ƒ““®ìƒ‚ƒfƒ‹íœˆ—	++++++++++++
	{
		int max = EventData_GetNpcCount( fsys );
		const FIELD_OBJ_H *head = EventData_GetNpcData( fsys );
		FieldOBJSys_ZoneUpdateDelete(
			fsys->fldobjsys, old_zone_id, new_zone_id, max, head );
	}
	
	//++++++++++++++	Vƒ][ƒ“ƒf[ƒ^’Ç‰Áˆ—	++++++++++++
	{
		//ƒtƒF[ƒhƒAƒEƒg ¨ ’Ç‰Á”gŒ`“Ç‚İ‚İ ¨ BGMÄ¶
		Snd_FadeOutNextPlayCall(
			fsys,
			Snd_FieldBgmNoGet(fsys,fsys->location->zone_id), 
			BGM_FADE_FIELD_MODE );
		
		EventData_SetFieldOBJ(fsys);
		
		//“VŒóƒŠƒNƒGƒXƒg
		#ifdef TORNWORLD_WEATHER_OFF
		if( fsys->fldmap->weather_data != NULL ){
			WEATHER_ChengeReq(
				fsys->fldmap->weather_data,
				Situation_GetWeatherID(sit) );
		}
		#else
		WEATHER_ChengeReq(
			fsys->fldmap->weather_data,
			Situation_GetWeatherID(sit) );
		#endif
	}
}

//------------------------------------------------------------------
/**
 * @brief	ƒtƒB[ƒ‹ƒhƒ}ƒbƒvXVˆ—
 * @param	fsys	ƒtƒB[ƒ‹ƒh§Œäƒ[ƒN‚Ö‚Ìƒ|ƒCƒ“ƒ^
 */
//------------------------------------------------------------------
static void FieldMap_Update(FIELDSYS_WORK * fsys, u8 flag)
{

	if (FieldEvent_Check(fsys) == FALSE) {
		EVTIME_Update(fsys);
	}

	MainLightCont(fsys->light_cont_data);		// ƒ‰ƒCƒgƒƒCƒ“

	//ƒ`ƒƒƒ“ƒlƒ‹ƒtƒF[ƒhƒVƒXƒeƒ€(’‹‚Æ–é‚Å‹È‚Ìƒ`ƒƒƒ“ƒlƒ‹‘€ì)
	//Snd_TrackFadeCall( fsys,Snd_FieldBgmNoGet(fsys,fsys->location->zone_id) );


	BoardMain( fsys );		// ŠÅ”Â§Œä

	DEBUG_INIT_TICK();
	DEBUG_STORE_TICK(0);
	if( SET_FUNC_CHECK( flag, FLD_SET_FIELD_ANIME ) != 0 ){
		FieldAnimeMain(fsys->fldmap->field_trans_anime);	// ƒtƒB[ƒ‹ƒh“]‘—ƒAƒjƒ
	}
	if( SET_FUNC_CHECK( flag, FLD_SET_FIELD_3D_ANIME ) != 0 ){	//ƒtƒB[ƒ‹ƒh3‚cƒAƒjƒ
		F3DA_Main(fsys->field_3d_anime);
	}
	DEBUG_STORE_TICK(1);
	if( SET_FUNC_CHECK( flag, FLD_SET_DIVMAP_LOAD ) != 0 ){
		DivMapLoadMain(fsys, fsys->map_cont_dat);	//ƒ}ƒbƒvƒ[ƒhƒƒCƒ“ŠÖ”
		
		if( GimmickTornWorldCheck(fsys) == TRUE ){
			FldTornWorld_MapUpdate( fsys );
		}
	}
	DEBUG_STORE_TICK(2);
	if( SET_FUNC_CHECK( flag, FLD_SET_MAP3D_WRITE ) != 0 ){
		Map3Dwrite( fsys );						// •`‰æƒGƒ“ƒWƒ“
	}
	DEBUG_STORE_TICK(3);

	if (sys.trg & PAD_BUTTON_X){
		DEBUG_CLEAR_TICK();
	}

}


//------------------------------------------------------------------
/**
 * @brief	ƒ^ƒEƒ“ƒ}ƒbƒv‘«Õî•ñ‚ÌƒAƒbƒvƒf[ƒg
 * @param	fsys	ƒtƒB[ƒ‹ƒh§Œäƒ[ƒN‚Ö‚Ìƒ|ƒCƒ“ƒ^
 * 
 * townmap_footmark.c‚ÉˆÚ“®‚·‚é‚©‚à‚µ‚ê‚È‚¢B
 */
//------------------------------------------------------------------
static void UpdateFootMark(FIELDSYS_WORK * fsys)
{
	TOWN_MAP_FOOTMARK * footmark;
	int x, z, dir;

	if (ZoneData_IsSinouField(fsys->location->zone_id) == FALSE) {
		//ƒtƒB[ƒ‹ƒhˆÈŠO‚Å‚Í‘«Õ‚ÌXV‚Í‚È‚¢I
		return;
	}
	footmark = Situation_GetTMFootMark(SaveData_GetSituation(fsys->savedata));
	x = (Player_NowGPosXGet(fsys->player)-
		GetDivMapOriginGridX(fsys->map_cont_dat)) / BLOCK_GRID_W;
	z = (Player_NowGPosZGet(fsys->player)-
		GetDivMapOriginGridZ(fsys->map_cont_dat)) / BLOCK_GRID_H;
	dir = Player_DirGet(fsys->player);
	TMFootMark_Update(footmark, x, z, dir);
}


//============================================================================================
//============================================================================================
//===========================================================================
/**
 * 
 * ‚u‚q‚`‚lƒoƒ“ƒNİ’è
 *
 */
//===========================================================================
static void vram_bank_set(void)
{
#ifdef TEST_OBJCHAR_32K
	GF_BGL_DISPVRAM vramSetTable = {
		GX_VRAM_BG_128_C,				// ƒƒCƒ“2DƒGƒ“ƒWƒ“‚ÌBG
		GX_VRAM_BGEXTPLTT_NONE,			// ƒƒCƒ“2DƒGƒ“ƒWƒ“‚ÌBGŠg’£ƒpƒŒƒbƒg
		GX_VRAM_SUB_BG_32_H,			// ƒTƒu2DƒGƒ“ƒWƒ“‚ÌBG
		GX_VRAM_SUB_BGEXTPLTT_NONE,		// ƒTƒu2DƒGƒ“ƒWƒ“‚ÌBGŠg’£ƒpƒŒƒbƒg
		GX_VRAM_OBJ_32_FG,				// ƒƒCƒ“2DƒGƒ“ƒWƒ“‚ÌOBJ
		GX_VRAM_OBJEXTPLTT_NONE,		// ƒƒCƒ“2DƒGƒ“ƒWƒ“‚ÌOBJŠg’£ƒpƒŒƒbƒg
		GX_VRAM_SUB_OBJ_16_I,			// ƒTƒu2DƒGƒ“ƒWƒ“‚ÌOBJ
		GX_VRAM_SUB_OBJEXTPLTT_NONE,	// ƒTƒu2DƒGƒ“ƒWƒ“‚ÌOBJŠg’£ƒpƒŒƒbƒg
		GX_VRAM_TEX_01_AB,				// ƒeƒNƒXƒ`ƒƒƒCƒ[ƒWƒXƒƒbƒg
		GX_VRAM_TEXPLTT_0123_E			// ƒeƒNƒXƒ`ƒƒƒpƒŒƒbƒgƒXƒƒbƒg
	};
#else
	GF_BGL_DISPVRAM vramSetTable = {
		GX_VRAM_BG_128_C,				// ƒƒCƒ“2DƒGƒ“ƒWƒ“‚ÌBG
		GX_VRAM_BGEXTPLTT_NONE,			// ƒƒCƒ“2DƒGƒ“ƒWƒ“‚ÌBGŠg’£ƒpƒŒƒbƒg
		GX_VRAM_SUB_BG_32_H,			// ƒTƒu2DƒGƒ“ƒWƒ“‚ÌBG
		GX_VRAM_SUB_BGEXTPLTT_NONE,		// ƒTƒu2DƒGƒ“ƒWƒ“‚ÌBGŠg’£ƒpƒŒƒbƒg
		GX_VRAM_OBJ_16_F,				// ƒƒCƒ“2DƒGƒ“ƒWƒ“‚ÌOBJ
		GX_VRAM_OBJEXTPLTT_NONE,		// ƒƒCƒ“2DƒGƒ“ƒWƒ“‚ÌOBJŠg’£ƒpƒŒƒbƒg
		GX_VRAM_SUB_OBJ_16_I,			// ƒTƒu2DƒGƒ“ƒWƒ“‚ÌOBJ
		GX_VRAM_SUB_OBJEXTPLTT_NONE,	// ƒTƒu2DƒGƒ“ƒWƒ“‚ÌOBJŠg’£ƒpƒŒƒbƒg
		GX_VRAM_TEX_01_AB,				// ƒeƒNƒXƒ`ƒƒƒCƒ[ƒWƒXƒƒbƒg
		GX_VRAM_TEXPLTT_0123_E			// ƒeƒNƒXƒ`ƒƒƒpƒŒƒbƒgƒXƒƒbƒg
	};
#endif	
	GF_Disp_SetBank( &vramSetTable );
}

static void debug_vram_bank_set(void)
{
	GF_BGL_DISPVRAM vramSetTable = {
		GX_VRAM_BG_128_C,				// ƒƒCƒ“2DƒGƒ“ƒWƒ“‚ÌBG
		GX_VRAM_BGEXTPLTT_NONE,			// ƒƒCƒ“2DƒGƒ“ƒWƒ“‚ÌBGŠg’£ƒpƒŒƒbƒg
		GX_VRAM_SUB_BG_32_H,			// ƒTƒu2DƒGƒ“ƒWƒ“‚ÌBG
		GX_VRAM_SUB_BGEXTPLTT_NONE,		// ƒTƒu2DƒGƒ“ƒWƒ“‚ÌBGŠg’£ƒpƒŒƒbƒg
		GX_VRAM_OBJ_16_F,				// ƒƒCƒ“2DƒGƒ“ƒWƒ“‚ÌOBJ
		GX_VRAM_OBJEXTPLTT_NONE,		// ƒƒCƒ“2DƒGƒ“ƒWƒ“‚ÌOBJŠg’£ƒpƒŒƒbƒg
		GX_VRAM_SUB_OBJ_16_I,			// ƒTƒu2DƒGƒ“ƒWƒ“‚ÌOBJ
		GX_VRAM_SUB_OBJEXTPLTT_NONE,	// ƒTƒu2DƒGƒ“ƒWƒ“‚ÌOBJŠg’£ƒpƒŒƒbƒg
		GX_VRAM_TEX_NONE,				// ƒeƒNƒXƒ`ƒƒƒCƒ[ƒWƒXƒƒbƒg
		GX_VRAM_TEXPLTT_0123_E			// ƒeƒNƒXƒ`ƒƒƒpƒŒƒbƒgƒXƒƒbƒg
	};
	GF_Disp_SetBank( &vramSetTable );
}

//===========================================================================
/**
 * 
 * ‚a‚fİ’è 
 *
 */
//===========================================================================
// ŠO•”ŒöŠJ—pŠÖ”
void Fieldmap_BgSet( GF_BGL_INI * ini )
{
	bg_set( ini );
}
void Fieldmap_BgExit( GF_BGL_INI * ini )
{
	bg_exit( ini );
}

// “à•”ŠÖ”
#ifdef TEST_BGMODE_1
static void bg_set( GF_BGL_INI * ini )
{
	{
		GF_BGL_SYS_HEADER BGsys_data = {
			GX_DISPMODE_GRAPHICS,GX_BGMODE_1,GX_BGMODE_0, GX_BG0_AS_3D
		};
		GF_BGL_InitBG(&BGsys_data);
	}
	
	{	// MAIN DISPiƒGƒtƒFƒNƒg‚Pj
		GF_BGL_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x10000, GX_BG_EXTPLTT_01,
			3, 0, 0, FALSE
		};
		GF_BGL_BGControlSet( ini, FLD_MBGFRM_EFFECT1, &TextBgCntDat, GF_BGL_MODE_TEXT );
		GF_BGL_ClearCharSet( FLD_MBGFRM_EFFECT1, 32, 0, HEAPID_FIELD );
		GF_BGL_ScrClear( ini, FLD_MBGFRM_EFFECT1 );
	}
	
	{	// MAIN DISPiƒGƒtƒFƒNƒg‚Qj
		GF_BGL_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x2000, 0, GF_BGL_SCRSIZ_512x512, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x14000, GX_BG_EXTPLTT_23,
			3, 0, 0, FALSE
		};
		GF_BGL_BGControlSet( ini, FLD_MBGFRM_EFFECT2, &TextBgCntDat, GF_BGL_MODE_TEXT );
		GF_BGL_ClearCharSet( FLD_MBGFRM_EFFECT2, 32, 0, HEAPID_FIELD );
		GF_BGL_ScrClear( ini, FLD_MBGFRM_EFFECT2 );
	}
	{	// MAIN DISPiƒƒbƒZ[ƒWj
		GF_BGL_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x2800, GX_BG_CHARBASE_0x08000, GX_BG_EXTPLTT_23,
			0, 0, 0, FALSE
		};
		GF_BGL_BGControlSet( ini, FLD_MBGFRM_FONT, &TextBgCntDat, GF_BGL_MODE_TEXT );
		GF_BGL_ClearCharSet( FLD_MBGFRM_FONT, 32, 0, HEAPID_FIELD );
		GF_BGL_ScrClear( ini, FLD_MBGFRM_FONT );
	}
	
//	OS_Printf(
//		"BGL_Alloc!! 1st appHeap = %ld\n", sys_GetHeapFreeSize(HEAPID_FIELD) );
	
	{
		u16	pal = BG_CLEAR_COLOR;
		DC_FlushRange( (void*)pal, 2 );
		GX_LoadBGPltt( &pal, 0, 2 );
	}
}
#else
static void bg_set( GF_BGL_INI * ini )
{
	{
		GF_BGL_SYS_HEADER BGsys_data = {
			GX_DISPMODE_GRAPHICS,GX_BGMODE_0,GX_BGMODE_0, GX_BG0_AS_3D
		};
		GF_BGL_InitBG(&BGsys_data);
	}
	
	{	// MAIN DISPiƒGƒtƒFƒNƒg‚Pj
		GF_BGL_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x10000, GX_BG_EXTPLTT_01,
			3, 0, 0, FALSE
		};
		GF_BGL_BGControlSet( ini, FLD_MBGFRM_EFFECT1, &TextBgCntDat, GF_BGL_MODE_TEXT );
		GF_BGL_ClearCharSet( FLD_MBGFRM_EFFECT1, 32, 0, HEAPID_FIELD );
		GF_BGL_ScrClear( ini, FLD_MBGFRM_EFFECT1 );
	}
	
	{	// MAIN DISPiƒGƒtƒFƒNƒg‚Qj
		GF_BGL_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x14000, GX_BG_EXTPLTT_23,
			3, 0, 0, FALSE
		};
		GF_BGL_BGControlSet( ini, FLD_MBGFRM_EFFECT2, &TextBgCntDat, GF_BGL_MODE_TEXT );
		GF_BGL_ClearCharSet( FLD_MBGFRM_EFFECT2, 32, 0, HEAPID_FIELD );
		GF_BGL_ScrClear( ini, FLD_MBGFRM_EFFECT2 );
	}
	{	// MAIN DISPiƒƒbƒZ[ƒWj
		GF_BGL_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x08000, GX_BG_EXTPLTT_23,
			0, 0, 0, FALSE
		};
		GF_BGL_BGControlSet( ini, FLD_MBGFRM_FONT, &TextBgCntDat, GF_BGL_MODE_TEXT );
		GF_BGL_ClearCharSet( FLD_MBGFRM_FONT, 32, 0, HEAPID_FIELD );
		GF_BGL_ScrClear( ini, FLD_MBGFRM_FONT );
	}
	
//	OS_Printf(
//		"BGL_Alloc!! 1st appHeap = %ld\n", sys_GetHeapFreeSize(HEAPID_FIELD) );
	
	{
		u16	pal = BG_CLEAR_COLOR;
		DC_FlushRange( (void*)pal, 2 );
		GX_LoadBGPltt( &pal, 0, 2 );
	}
}
#endif

// BGLŠJ•ú
static void bg_exit( GF_BGL_INI * ini )
{
	GF_Disp_GX_VisibleControl(
		GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2 | GX_PLANEMASK_BG3, VISIBLE_OFF );

	GF_BGL_BGControlExit( ini, FLD_MBGFRM_EFFECT1 );
	GF_BGL_BGControlExit( ini, FLD_MBGFRM_EFFECT2 );
	GF_BGL_BGControlExit( ini, FLD_MBGFRM_FONT );
}

// ƒZƒ‹ƒAƒNƒ^[—p‹¤—LOAMƒ}ƒl[ƒWƒƒ‚Ì‰Šú‰»
static void join_clact_oam_init(void)
{
	// OAMƒ}ƒl[ƒWƒƒ[‚Ì‰Šú‰»
	NNS_G2dInitOamManagerModule();

	// ‹¤—LOAMƒ}ƒl[ƒWƒƒì¬
	// ƒŒƒ“ƒ_ƒ‰—pOAMƒ}ƒl[ƒWƒƒì¬
	// ‚±‚±‚Åì¬‚µ‚½OAMƒ}ƒl[ƒWƒƒ‚ğ‚İ‚ñ‚È‚Å‹¤—L‚·‚é
	REND_OAMInit( 
			0, 124,		// ƒƒCƒ“‰æ–ÊOAMŠÇ——Ìˆæ
			0, 31,		// ƒƒCƒ“‰æ–ÊƒAƒtƒBƒ“ŠÇ——Ìˆæ
			0, 124,		// ƒTƒu‰æ–ÊOAMŠÇ——Ìˆæ
			0, 31,		// ƒTƒu‰æ–ÊƒAƒtƒBƒ“ŠÇ——Ìˆæ
			HEAPID_FIELD);
	
}

// ƒZƒ‹ƒAƒNƒ^[‰Šú‰»
static void clact_delete(void)
{
	// ƒŒƒ“ƒ_ƒ‰[‹¤—LOAMƒ}ƒl[ƒWƒƒ”jŠü
	REND_OAM_Delete();
}


// ƒfƒtƒHƒ‹ƒg‚ÌƒOƒ[ƒoƒ‹ƒXƒe[ƒg‚Ìó‘Ô‚ğİ’è
static void field_glb_state_init(GLST_DATA_PTR glb)
{
	GLST_MdlPolyMode(glb, GX_POLYGONMODE_MODULATE, FALSE);
	GLST_MdlCullMode(glb, GX_CULL_BACK, FALSE);
	GLST_MdlAlpha(glb, 31, FALSE);
	GLST_MdlMisc(glb, GX_POLYGON_ATTR_MISC_FOG, TRUE, FALSE);

	GLST_Reflect(glb, GLST_ALL);		// İ’è
}

//-------------------------------------
//
//	ƒLƒƒƒ‰ƒNƒ^ƒ}ƒl[ƒWƒƒ[
//	ƒpƒŒƒbƒgƒ}ƒl[ƒWƒƒ[‚Ì‰Šú‰»
//
//=====================================
void char_pltt_manager_init(void)
{
	// ƒLƒƒƒ‰ƒNƒ^ƒ}ƒl[ƒWƒƒ[‰Šú‰»
	{
		CHAR_MANAGER_MAKE cm = {
			FLD_CHAR_CONT_NUM,
			FLD_CHAR_VRAMTRANS_MAIN_SIZE,
			FLD_CHAR_VRAMTRANS_SUB_SIZE,
			HEAPID_FIELD
		};
		
		InitCharManagerReg(&cm, GX_OBJVRAMMODE_CHAR_1D_32K, GX_OBJVRAMMODE_CHAR_1D_32K );
	}
	// ƒpƒŒƒbƒgƒ}ƒl[ƒWƒƒ[‰Šú‰»
	InitPlttManager(FLD_PLTT_CONT_NUM, HEAPID_FIELD);

	// “Ç‚İ‚İŠJnˆÊ’u‚ğ‰Šú‰»
	CharLoadStartAll();
	PlttLoadStartAll();

	// ’ÊMƒAƒCƒRƒ“—Ìˆæ‚ğ—\–ñ‚·‚é
}

//-------------------------------------
//
//	ƒLƒƒƒ‰ƒNƒ^ƒ}ƒl[ƒWƒƒ[
//	ƒpƒŒƒbƒgƒ}ƒl[ƒWƒƒ[‚Ì”jŠü
//
//=====================================
void char_pltt_manager_delete(void)
{
	// ƒLƒƒƒ‰ƒNƒ^ƒf[ƒ^”jŠü
	DeleteCharManager();
	// ƒpƒŒƒbƒgƒf[ƒ^”jŠü
	DeletePlttManager();
}

//===========================================================================
/**
 * 
 * •`‰æ
 *
 */
//===========================================================================
static void Map3Dwrite(FIELDSYS_WORK * repw)
{
	MtxFx44 org_pm,pm;

	//‚R‚c•`‰æŠJn
	GF_G3X_Reset();
	
	if( repw->camera_type == FIELD_CAMERA_LOOKAT_Z ){
		//”j‚ê‚½¢ŠE—p‰¼ˆ— kaga
		if( GimmickTornWorldCheck(repw) == TRUE ){
			FldTornWorld_CameraUpdate( repw );
		}
		GFC_CameraLookAtZ();
	}else{
		GFC_CameraLookAt();
	}
	
	//—h‚ê‘”ÍˆÍŠOƒ`ƒFƒbƒN
	SwayGrass_CheckIO(repw);
	
	DrawAroundBlock(repw->map_cont_dat,repw->glst_data);
	
	if( GimmickTornWorldCheck(repw) == TRUE ){
		FldTornWorld_Draw( repw );
	}
	
	M3DO_DrawMap3DObjExp(repw->Map3DObjExp, repw->MapResource);
	//ƒvƒƒWƒFƒNƒVƒ‡ƒ“ƒ}ƒgƒŠƒNƒX‚Ì•ÏŠ·
	{
		const MtxFx44 *m;
		m = NNS_G3dGlbGetProjectionMtx();
/**		
		OS_Printf("%x,%x,%x,%x\n%x,%x,%x,%x\n%x,%x,%x,%x\n%x,%x,%x,%x\n",
				m->_00,m->_01,m->_02,m->_03,
				m->_10,m->_11,m->_12,m->_13,
				m->_20,m->_21,m->_22,m->_23,
				m->_30,m->_31,m->_32,m->_33);
*/				
		org_pm = *m;
		pm = org_pm;
		pm._32 += FX_Mul(pm._22,PRO_MAT_Z_OFS*FX32_ONE);
		NNS_G3dGlbSetProjectionMtx(&pm);
		NNS_G3dGlbFlush();		//@ƒWƒIƒƒgƒŠƒRƒ}ƒ“ƒh‚ğ“]‘—
	}

	//ƒtƒB[ƒ‹ƒhƒGƒtƒFƒNƒg•`‰æ
	FE_Draw( repw->fes );
	
	// ƒrƒ‹ƒ{[ƒhƒAƒNƒ^[ƒVƒXƒeƒ€•`‰æ
	BLACT_DrawSys();
	
	/*
	 	ƒrƒ‹ƒ{[ƒhƒAƒNƒ^[ ƒeƒNƒXƒ`ƒƒ“]‘—ƒVƒXƒeƒ€‚Ì“s‡ã
		‚±‚ÌêŠ‚ÅƒeƒNƒXƒ`ƒƒƒpƒŒƒbƒg‚ğ•ÏX‚µ‚È‚¢‚Æ
		—v–]’Ê‚è‚Ì‰‰o‚ªo—ˆ‚È‚¢B080415 kaga
	*/
	if( GimmickTornWorldCheck(repw) == TRUE ){
		FldTornWorld_GiratinaPlttFade( repw );
	}
	
	//ƒvƒƒWƒFƒNƒVƒ‡ƒ“ƒ}ƒgƒŠƒNƒX‚ğŒ³‚É–ß‚·
	{
		NNS_G3dGlbSetProjectionMtx(&org_pm);
		NNS_G3dGlbFlush();		//@ƒWƒIƒƒgƒŠƒRƒ}ƒ“ƒh‚ğ“]‘—
	}

	///<fieldmap_func•`‰æˆ—
	FLDMAPFUNC_Sys_Draw3D( repw->fldmap->fmapfunc_sys );

	/* ƒWƒIƒƒgƒŠ•ƒŒƒ“ƒ_ƒŠƒ“ƒOƒGƒ“ƒWƒ“ŠÖ˜Aƒƒ‚ƒŠ‚ÌƒXƒƒbƒv */
	GF_G3_RequestSwapBuffers(GX_SORTMODE_AUTO, SwapBuffMode);
#ifdef DEBUG_3DDRAW_COUNT	
	D_3D_DC_DrawPolygonCountUp();
#endif
}



//----------------------------------------------------------------------------
/**
 *	@brief	3D•\¦‚ÌXV‚ğs‚í‚È‚­‚·‚é‚©‚ğİ’è‚·‚é
 *
 *	@param	fsys	ƒtƒB[ƒ‹ƒhƒVƒXƒeƒ€
 *	@param	flag	ƒtƒ‰ƒO		TRUEFXV‚·‚é	FALSEFXV‚µ‚È‚¢
 */
//-----------------------------------------------------------------------------
void Fieldmap_UpdateMsk_Set3Dwrite( FIELDSYS_WORK * fsys, BOOL flag )
{
	if( flag == TRUE ){
		fsys->fldmap_update_msk |= FLD_SET_MAP3D_WRITE;
	}else{
		fsys->fldmap_update_msk &= ~FLD_SET_MAP3D_WRITE;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	ƒtƒB[ƒ‹ƒh“]‘—ƒAƒjƒ‚ÌXV—L–³‚ğİ’è
 *
 *	@param	fsys	ƒtƒB[ƒ‹ƒhƒVƒXƒeƒ€
 *	@param	flag	ƒtƒ‰ƒO
 */
//-----------------------------------------------------------------------------
void Fieldmap_UpdateMsk_SetFieldAnime( FIELDSYS_WORK * fsys, BOOL flag )
{
	if( flag == TRUE ){
		fsys->fldmap_update_msk |= FLD_SET_FIELD_ANIME;
	}else{
		fsys->fldmap_update_msk &= ~FLD_SET_FIELD_ANIME;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	ƒtƒB[ƒ‹ƒhƒ}ƒbƒvƒAƒbƒvƒf[ƒgƒ}ƒXƒN	‰Šú‰»
 *	
 *	@param	p_fsys	ƒtƒB[ƒ‹ƒhƒVƒXƒeƒ€
 */
//-----------------------------------------------------------------------------
static void fldmap_update_msk_Init( FIELDSYS_WORK* p_fsys )
{
	p_fsys->fldmap_update_msk = (FLD_SET_FIELD_3D_ANIME | FLD_SET_FIELD_ANIME | FLD_SET_DIVMAP_LOAD | FLD_SET_MAP3D_WRITE);
}

//===========================================================================
/**
 *
 * ƒtƒB[ƒ‹ƒh—pƒƒCƒvƒVƒXƒeƒ€‚ğg—p‚µ‚½‹P“xƒtƒF[ƒh
 *
 */
//===========================================================================
void FieldFadeWipeSet(const u8 flg)
{
	if( flg == FLD_DISP_BRIGHT_BLACKIN ){
		WIPE_SYS_Start(
			WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN,
			WIPE_TYPE_FADEIN, WIPE_FADE_BLACK, COMM_BRIGHTNESS_SYNC, 1, HEAPID_FIELD  );
	}else if (flg == FLD_DISP_BRIGHT_BLACKOUT){
		WIPE_SYS_Start(
			WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT,
			WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK, COMM_BRIGHTNESS_SYNC, 1, HEAPID_FIELD );
	}else{
		GF_ASSERT(0&&"ƒtƒF[ƒhw’èƒ~ƒX");
	}
}

static void FieldMapLoadSeq( FIELDSYS_WORK * repw )
{
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_OFF );	// BG0‚ğ”ñ•\¦

	// ƒWƒIƒƒgƒŠ•ƒŒƒ“ƒ_ƒŠƒ“ƒOƒGƒ“ƒWƒ“ŠÖ˜Aƒƒ‚ƒŠ‚ÌƒXƒƒbƒv
	// •\¦ƒf[ƒ^‚ğŠ®‘S‚ÉÁ‚µ‚ÄAƒ[ƒh‚Ì‚¿‚ç‚Â‚«‚ğ–h‚¬‚Ü‚·B
	G3_SwapBuffers( GX_SORTMODE_AUTO, SwapBuffMode );

	//ƒƒP[ƒVƒ‡ƒ“î•ñ‚©‚ç“Ç‚İ‚Şƒ}ƒbƒv‚ğ¯•Ê
	//
	repw->field_3d_anime = F3DA_Field3DAnimeInit();	//ƒtƒB[ƒ‹ƒh3‚cƒAƒjƒƒƒ‚ƒŠŠm•Û
	repw->AnimeContMng = F3DASub_InitField3DAnimeCont();

//SetUpWorldMatrix( repw->location->zone_id, repw->World ); <<20060116 move
	{
		u16 area_id, mml_id;
		area_id = ZoneData_GetAreaID(repw->location->zone_id);
		repw->MapResource = AllocMapResource(	area_id,
												repw->field_3d_anime	);
		
		mml_id = ZoneData_GetMoveModelID(repw->location->zone_id);
		GF_ASSERT(repw->mmdl == NULL);
		repw->mmdl = MMDL_Create(HEAPID_FIELD, mml_id);
		
	}
}

static void FieldMapLoadSeqDivMap(FIELDSYS_WORK * repw)
{
	// ƒ}ƒbƒvŠÇ—ƒf[ƒ^Šm•Û
	repw->map_cont_dat = InitDivMap(repw->World,
									repw->MapResource,
									repw->field_3d_anime,
									repw->DivMapMode);		
	
	if( GimmickTornWorldCheck(repw) == TRUE ){
		int x = 0,y = 0,z = 0;
		FldTornWorld_ArcDataMapOriginGridPosGet(
			repw->location->zone_id, &x, &y, &z );
		SetDivMapOriginGridPos( repw->map_cont_dat, x, y, z );
		SetDivMapBlockMode( repw->map_cont_dat, BLOCKMODE_2x2 );
		SetDivMapM3dObjSetFlag( repw->map_cont_dat, DIVM3DO_OFF );
	}
	
	//Šg’£‚‚³ƒf[ƒ^—ÌˆæŠm•Û
	repw->ExHeightList = EXH_AllocExHeightList(EX_HEIGHT_NUM, HEAPID_FIELD);

	repw->HoneyTreePtr = HTE_AllocHoneyTreeDat();	//–¨“h‚è–Ø—pƒf[ƒ^Šm•Û(’n‰º‚Å‚Íg‚¢‚Ü‚¹‚ñ‚ªAŠm•Û‚µ‚¿‚á‚¢‚Ü‚·)
	if (repw->MapMode == MAP_MODE_GROUND){
		//DIV‚É–¨“h‚è—p‚ÌƒR[ƒ‹ƒoƒbƒN‚ğİ’è
		DIVM_SetLoadCallBack( repw->map_cont_dat, HTE_SetHoneyTreeAnimeCallBack, repw );
	}
	
	LoadDivMap( repw->map_cont_dat,
				repw->location->grid_x,
				repw->location->grid_z);
}

#define FE_EOA_MAX_TORNWORLD (112)

static void FieldMapLoadSeqFeMoveObj(FIELDSYS_WORK * repw)
{
	//ƒtƒB[ƒ‹ƒhƒGƒtƒFƒNƒg‰Šú‰»
	repw->fes = FE_Init( repw, FE_EFFECT_MAX, HEAPID_FIELD );
	#if 0
	FE_ParamInit_EOA( repw->fes, FE_EOA_MAX+16 );
	#else
	{
		int max = FE_EOA_MAX;
		if( GimmickTornWorldCheck(repw) == TRUE ){
			max = FE_EOA_MAX_TORNWORLD;
		}
		FE_ParamInit_EOA( repw->fes, max );
	}
	#endif
	
	FE_ParamInit_BlAct( repw->fes, HEAPID_FIELD, FE_BLACT_MAX,
		FE_BLACT_MDL_MAX, FE_BLACT_ANM_MAX, FE_BLACT_TEX_MAX,
		FE_BLACT_MDL_HEAP_SIZE, FE_BLACT_ANM_HEAP_SIZE, FE_BLACT_TEX_HEAP_SIZE );
	
	//’n‰º‚©ƒ†ƒjƒIƒ“‚Å‚ ‚ê‚ÎƒtƒB[ƒ‹ƒhOBJ‚É‰e‚ğ•t‚¯‚È‚¢
	if( repw->MapMode == MAP_MODE_UNDER || repw->MapMode == MAP_MODE_UNION ){
		FieldOBJSys_ShadowJoinSet( repw->fldobjsys, FALSE );
	}

	//’n‰º‚Æ‚»‚êˆÈŠO‚Å“o˜^‚·‚éƒGƒtƒFƒNƒgƒf[ƒ^‚ğ•Ï‚¦‚é
	{
		const u32 *tbl;
		
		if( repw->MapMode == MAP_MODE_UNDER ){
       		tbl = DATA_FE_UnderProcRegistTbl;
		}else{	//MAP_MODE_GROUND
			if( GimmickTornWorldCheck(repw) == TRUE ){
				tbl = DATA_FE_TornWorldRegistTbl;
			}else{
				tbl = DATA_FE_GroundProcRegistTbl;
			}
		}
		
		FE_EffectRegistMore( repw->fes, tbl );
	}
	
	//ƒtƒB[ƒ‹ƒhOBJ•`‰æˆ—‰Šú‰»
	#if 0 // old DP
	FieldOBJSys_DrawInit( repw->fldobjsys,
		FLDOBJ_RESM_MAX_TEX,		//“o˜^Å‘å”
		MMDL_GetListSize(repw->mmdl) + FLDOBJ_RESM_REG_ALWAYS_NUM,	//í’““o˜^Å‘å”iƒVƒXƒeƒ€—p‚S‚Âj
		MMDL_GetList(repw->mmdl) );
	#else // new PL
	{
		int frame_trans_max = FLDOBJ_FRAME_TRANS_MAX;
		
		if( repw->MapMode == MAP_MODE_UNION ){
			frame_trans_max = FLDOBJ_FRAME_TRANS_MAX_UNION;
		}
		
		FieldOBJSys_DrawInit( repw->fldobjsys,
			//“o˜^Å‘å”
			FLDOBJ_RESM_MAX_TEX,
			//í’““o˜^Å‘å”iƒVƒXƒeƒ€—p‚S‚Âj
			MMDL_GetListSize(repw->mmdl) + FLDOBJ_RESM_REG_ALWAYS_NUM,
			//“o˜^‚·‚éƒeƒNƒXƒ`ƒƒƒŠƒXƒg
			MMDL_GetList(repw->mmdl),
			//1ƒtƒŒ[ƒ€‚É“]‘—‚Å‚«‚éƒf[ƒ^”
			frame_trans_max );
	}
	#endif
	
	//ƒtƒB[ƒ‹ƒhOBJAƒtƒB[ƒ‹ƒhƒGƒtƒFƒNƒgg—pƒŒƒ“ƒ_OBJˆ—‰Šú‰»
	FE_FieldRenderOBJInit( repw->fes );
	
	{
		GIMMICKWORK *gimmick =
		SaveData_GetGimmickWork(GameSystem_GetSaveData(repw));
		int gimmick_id = GIMMICKWORK_GetAssignID(gimmick);
		Player_DrawInit( repw->player, gimmick_id );	//©‹@•`‰æ‰Šú‰»
	}
	
	FieldOBJ_DrawProcPopAll( repw->fldobjsys );			//”­¶‚µ‚Ä‚¢‚é‘SOBJ‚Ì•`‰æˆ—•œ‹A
    CommPlayerSetForcePos();  // ’ÊM‚µ‚Ä‚¢‚éê‡FLDOBJ‚ÌŒ»İˆÊ’u‚Ö‚ÌˆÚ“®
    FieldOBJSys_MoveStopAllClear( repw->fldobjsys );
	DivMapBindTarget(
			Player_VecPosPtrGet(repw->player), repw->map_cont_dat);//’nŒ`ƒoƒCƒ“ƒh

	repw->fldmap->seedsys = SeedSys_Init(repw, HEAPID_FIELD);
}

static void FieldMapLoadSeqRest(FIELDSYS_WORK * repw)
{
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );	// BG0‚ğ•\¦

	GF_Disp_DispOn();		// ƒfƒBƒXƒvƒŒƒCƒIƒ“

	repw->glst_data = GLST_Init();			// ƒ‰ƒCƒgEƒ}ƒeƒŠƒAƒ‹Eƒ|ƒŠƒSƒ“İ’è‚Ìó‘Ô‚ğ•Û‘¶‚·‚éƒVƒXƒeƒ€ì¬
	field_glb_state_init(repw->glst_data);	// ƒfƒtƒHƒ‹ƒg‚Ìó‘Ô‚ğ•Û‘¶

	repw->fog_data = FogSys_Init();			// ƒtƒHƒO‚Ìó‘Ô‚ğ•Û‘¶‚·‚éƒVƒXƒeƒ€‚ğì¬
	//ƒJƒƒ‰‚Ìİ’è
	{
///		BOOL cycling_flg;
		int camera_id = Situation_GetCameraID(SaveData_GetSituation(repw->savedata));
///		cycling_flg = Player_MoveBitCheck_CyclingRoad( repw->player );
		FieldCameraInit( Player_VecPosPtrGet(repw->player), repw, camera_id, TRUE );
	}
	
	repw->light_cont_data = InitLightCont(repw->glst_data,
											MAPRES_GetLightIndex(repw->MapResource));
	#ifdef TORNWORLD_WEATHER_OFF
	if( GimmickTornWorldCheck(repw) == TRUE ){
		repw->fldmap->weather_data = NULL;
	}else{
		repw->fldmap->weather_data = WEATHER_Init(repw); // “V‹CƒVƒXƒeƒ€‰Šú‰»
	}
	#else
	repw->fldmap->weather_data = WEATHER_Init(repw);	// “V‹CƒVƒXƒeƒ€‰Šú‰»
	#endif
	
	repw->fldmap->place_name_cont = AllocPlaceNameCont(repw->bgl);//’n–¼•\¦\‘¢‘ÌŠm•Û
	repw->board = BoardWorkAlloc( HEAPID_FIELD );		// ŠÅ”Âƒ[ƒNŠm•Û
	
	// ƒtƒB[ƒ‹ƒh“]‘—ƒAƒjƒˆ—‰Šú‰»
	repw->fldmap->field_trans_anime = InitFieldAnime();
	FieldAnimeSets(repw->fldmap->field_trans_anime, GetMapResourceTexturePTR(repw->MapResource));

	//ƒtƒB[ƒ‹ƒhƒMƒ~ƒbƒNƒZƒbƒgƒAƒbƒv
	FLDGMK_SetUpFieldGimmick(repw);

	//—¬»ƒ`ƒFƒbƒNƒ^ƒXƒN’Ç‰Á
	QuickSand_CheckTask(repw);
	
	//Vƒuƒ‰ƒ“ƒNŠÖ”ƒZƒbƒg
	sys_VBlankFuncChange(vBlankFunc, repw);
}





//============================================================================================
//
//
//				ƒGƒŠƒA•Ê“®ìƒ‚ƒfƒ‹ƒŠƒXƒgŠÖ˜A
//
//
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	“®ìƒ‚ƒfƒ‹ƒŠƒXƒg‚ÌãŒÀ
 */
//------------------------------------------------------------------
#define	MMDL_MAX	(FLDOBJ_RESM_REG_MAX_TEX)

//------------------------------------------------------------------
/**
 * @brief	“®ìƒ‚ƒfƒ‹ƒŠƒXƒg§Œäƒ[ƒN’è‹`
 */
//------------------------------------------------------------------
struct MMDL_WORK{
	u16 count;				///<“®ìƒ‚ƒfƒ‹ƒŠƒXƒg‚Ì’è‹`”
	int id[MMDL_MAX];		///<“®ìƒ‚ƒfƒ‹ƒŠƒXƒg
};
//------------------------------------------------------------------
/**
 * @brief	“®ìƒ‚ƒfƒ‹ƒŠƒXƒgF‰Šú‰»ˆ—
 * @param	fsys	ƒtƒB[ƒ‹ƒh§Œäƒ[ƒN‚Ö‚Ìƒ|ƒCƒ“ƒ^
 * @param	area_id	ƒZƒbƒg‚·‚éƒGƒŠƒA‚Ìw’è
 */
//------------------------------------------------------------------
static MMDL_WORK * MMDL_Create(int heapID, int area_id)
{
	int i;
	u16 * arc;
	MMDL_WORK * mmdl;

	mmdl = sys_AllocMemory(heapID, sizeof(MMDL_WORK));
	arc = ArchiveDataLoadMallocLo(ARC_MMODEL_LIST, area_id, heapID);
	//arc = ArchiveDataLoadMallocLo(ARC_MMDL, area_id, heapID);
	for (i = 0; i < MMDL_MAX; i++) {
		mmdl->id[i] = OBJCODEMAX;
	}
	for (i = 0; i < MMDL_MAX; i++) {
		mmdl->id[i] = arc[i];
		if (arc[i] == OBJCODEMAX) {
			break;
		}
	}
	mmdl->count = i;
	sys_FreeMemoryEz(arc);
	return mmdl;
}

//------------------------------------------------------------------
/**
 * @brief	“®ìƒ‚ƒfƒ‹ƒŠƒXƒgFƒŠƒXƒg‚ğæ“¾
 * @param	fsys	ƒtƒB[ƒ‹ƒh§Œäƒ[ƒN‚Ö‚Ìƒ|ƒCƒ“ƒ^
 * @return	u16		“®ìƒ‚ƒfƒ‹ƒŠƒXƒg‚Ö‚Ìƒ|ƒCƒ“ƒ^
 */
//------------------------------------------------------------------
static const int * MMDL_GetList(const MMDL_WORK * mmdl)
{
	return mmdl->id;
}
//------------------------------------------------------------------
/**
 * @brief	“®ìƒ‚ƒfƒ‹ƒŠƒXƒgFƒŠƒXƒg‚Ì’è‹`”‚ğæ“¾
 * @param	fsys	ƒtƒB[ƒ‹ƒh§Œäƒ[ƒN‚Ö‚Ìƒ|ƒCƒ“ƒ^
 * @return	u16		“®ìƒ‚ƒfƒ‹ƒŠƒXƒgã‚Ì’è‹`”
 */
//------------------------------------------------------------------
static const int MMDL_GetListSize(const MMDL_WORK * mmdl)
{
	return mmdl->count;
}

//------------------------------------------------------------------
/**
 * @brief	“®ìƒ‚ƒfƒ‹ƒŠƒXƒgFI—¹ˆ—
 * @param	fsys	ƒtƒB[ƒ‹ƒh§Œäƒ[ƒN‚Ö‚Ìƒ|ƒCƒ“ƒ^
 */
//------------------------------------------------------------------
static void MMDL_Delete(MMDL_WORK * mmdl)
{
	sys_FreeMemoryEz(mmdl);
}

#ifdef PM_DEBUG
void DebugDispTexBank(const u8 inNo)
{
	GXVRamTex tex_bank;
	tex_bank = GX_GetBankForTex();
	if (tex_bank==0){
		OS_Printf("%d:disp_tex_bank=%d\n",inNo,tex_bank);
	}
	GF_ASSERT(tex_bank!=0&&"ƒeƒNƒXƒ`ƒƒƒoƒ“ƒN”j‰ó”­¶");
/*	
	if (tex_bank==0){
		OS_Printf("ƒeƒNƒXƒ`ƒƒƒoƒ“ƒN”j‰ó”­¶");
	}
*/	
}
#endif

//============================================================================================
//
//
//				ƒGƒŠƒA•Ê“®ìƒ‚ƒfƒ‹ƒŠƒXƒgŠÖ˜A
//
//
//============================================================================================

//==============================================================================
//	‚â‚Ô‚ê‚½‚¹‚©‚¢ŠÖ˜A
//==============================================================================
static BOOL GimmickTornWorldCheck( FIELDSYS_WORK *fsys )
{
	GIMMICKWORK *gimmick =
	SaveData_GetGimmickWork(GameSystem_GetSaveData(fsys));
	int gimmick_id = GIMMICKWORK_GetAssignID(gimmick);
	if( gimmick_id == FLD_GIMMICK_TORNWORLD ){
		return( TRUE );
	}
	return( FALSE );
}

