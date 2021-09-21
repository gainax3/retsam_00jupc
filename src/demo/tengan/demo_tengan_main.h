
#ifndef __DEMO_TENGAN_MAIN_H__
#define __DEMO_TENGAN_MAIN_H__

#include "common.h"
#include "field/fieldsys.h"
#include "field/field_event.h"


typedef struct {
	
	int		seq;	
	void*	gen_wk;
	
} DEMO_MAIN_WORK;

extern BOOL GMEVENT_DemoMain( GMEVENT_CONTROL * event );
extern void EventCmd_DemoTanganProc( GMEVENT_CONTROL * event, SAVEDATA* save );

#endif