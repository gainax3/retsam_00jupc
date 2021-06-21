#include "common.h"
#include "system/lib_pack.h"
#include "system/bmp_list.h"
#include "system/wipe.h"
#include "system/fontproc.h"
#include "system/pm_str.h"
#include "system/pm_overlay.h"
#include "system\palanm.h"
#include "application\pms_input.h"
#include "system\pm_rtc.h"

#include "field/field_common.h"
#include "field/field_event.h"
#include "field/fieldmap.h"
#include "field/fieldsys.h"
#include "field/ev_mapchange.h"

#include "demo/demo_tengan.h"
#include "demo_tengan_main.h"

FS_EXTERN_OVERLAY(demo_tenganzan);


void EventCmd_DemoTanganProc( GMEVENT_CONTROL * event, SAVEDATA* save )
{
	DEMO_MAIN_WORK* wk;
	DEMO_TENGAN_PARAM* para;

	wk		= sys_AllocMemory( HEAPID_WORLD, sizeof( DEMO_MAIN_WORK ) );
	para	= sys_AllocMemory( HEAPID_WORLD, sizeof( DEMO_TENGAN_PARAM ) );

	para->cfg			= SaveData_GetConfig( save );
	para->my_status		= SaveData_GetMyStatus( save );
	para->player_sex	= MyStatus_GetMySex( para->my_status );
	
	wk->seq = 0;
	wk->gen_wk = para;

	FieldEvent_Call( event, GMEVENT_DemoMain, wk );
}


BOOL GMEVENT_DemoMain( GMEVENT_CONTROL * event )
{
	FIELDSYS_WORK* 	fsys;
	DEMO_MAIN_WORK*	wk;
	
	
	fsys = FieldEvent_GetFieldSysWork( event );
	wk	 = FieldEvent_GetSpecialWork( event );
	
	switch ( wk->seq ){
	case 0:
		FieldFadeWipeSet( FLD_DISP_BRIGHT_BLACKOUT );
		wk->seq++;
	
	case 1:
		if( WIPE_SYS_EndCheck() == FALSE ) { break; }		
		{					
			static const PROC_DATA proc_data = {
				DemoTengan_Proc_Init,
				DemoTengan_Proc_Main,
				DemoTengan_Proc_Exit,
				FS_OVERLAY_ID( demo_tenganzan ),
			};					
			
			GameSystem_StartSubProc( fsys,  &proc_data, wk->gen_wk );
			wk->seq++;
		}
		break;
	
	case 2:
		if ( FieldEvent_Cmd_WaitSubProcEnd( fsys ) == FALSE ) {
			 
			 FieldEvent_Cmd_SetMapProc( fsys );
			 wk->seq++;
		}
		break;

	case 3:
		if ( FieldEvent_Cmd_WaitMapProcStart( fsys ) == FALSE ) {
			
		//	 FieldFadeWipeSet( FLD_DISP_BRIGHT_BLACKIN );	///< このデモ終了後はフェードをしないため消す
			 wk->seq++;
		}
		break;
		
	case 4:
		 wk->seq++;
		 break;
		 
	case 5:
	//	if( WIPE_SYS_EndCheck() ) {
			
			sys_FreeMemoryEz( wk->gen_wk );			
			sys_FreeMemoryEz( wk );
			return TRUE;
	//	}
		break;
	}
	return FALSE;
}

