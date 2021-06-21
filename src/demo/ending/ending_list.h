//============================================================================================
/**
 * @file	demo_ending_list.h
 * @brief	エンディングデモ　スタッフリスト処理
 * @author	taya
 * @date	2006.05.22
 */
//============================================================================================
#ifndef __DEMO_ENDING_LIST_H__
#define __DEMO_ENDING_LIST_H__

//typedef struct _PL_ENDING_LIST_WORK	PL_ENDING_LIST_WORK;



extern PL_ENDING_LIST_WORK* PlEnding_ListWork_Create( GF_BGL_INI* bgl, int startLine, int bgFrame, int palPos, MSGDATA_MANAGER* msgMan );
extern void PlEnding_ListWork_Delete( PL_ENDING_LIST_WORK* wk );
extern BOOL PlEnding_ListWork_Scroll( PL_ENDING_LIST_WORK* wk, int height );

extern int PlEnding_FinStrIDGet(void);


extern void PlEnding_ListWork_WriteEndStr( PL_ENDING_LIST_WORK* wk, BOOL fillFlag );
extern void PlEnding_ListWork_WriteCopyLights( PL_ENDING_LIST_WORK* wk );

#endif  /* __DEMO_ENDING_LIST_H__ */
