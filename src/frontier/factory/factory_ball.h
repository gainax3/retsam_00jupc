//==============================================================================================
/**
 * @file	factory_ball.h
 * @brief	「バトルファクトリー」ボール
 * @author	Satoshi Nohara
 * @date	2007.03.15
 */
//==============================================================================================
#ifndef _FACTORY_BALL_H_
#define _FACTORY_BALL_H_

#include "system/clact_tool.h"
#include "factory_sys.h"


//==============================================================================================
//
//	ボールOBJワークへの不完全型ポインタ
//
//==============================================================================================
typedef	struct _FACTORY_BALL FACTORY_BALL;


//==============================================================================================
//
//	定義
//
//==============================================================================================
//ボールのパレット
enum{
	PAL_BALL_MOVE = 0,
	PAL_BALL_STOP,
	//PAL_BALL_MOVE = 6,
	//PAL_BALL_STOP = 5,
};


//==============================================================================================
//
//	extern宣言
//
//==============================================================================================
extern FACTORY_BALL*	FactoryBall_Create( FACTORY_CLACT* factory_clact, int x, int y, u32 heapID );
extern void*		FactoryBall_Delete( FACTORY_BALL* wk );
extern void			FactoryBall_Vanish( FACTORY_BALL* wk, int flag );
extern VecFx32 FactoryBall_Move( FACTORY_BALL* wk, int x, int y );
extern void			FactoryBall_SetPos( FACTORY_BALL* wk, int x, int y );
extern const VecFx32*	FactoryBall_GetPos( FACTORY_BALL* wk );
extern void			FactoryBall_SetDecideFlag( FACTORY_BALL* wk, u8 hit );
extern u8			FactoryBall_GetDecideFlag( FACTORY_BALL* wk );
extern BOOL			FactoryBall_Main( FACTORY_BALL* wk );
extern void			FactoryBall_Decide( FACTORY_BALL* factory_clact );
extern void			FactoryBall_Cancel( FACTORY_BALL* factory_clact );
extern void			FactoryBall_Entrance( FACTORY_BALL* wk );
extern int			FactoryBall_GetInitX( FACTORY_BALL* wk );
extern int			FactoryBall_GetInitY( FACTORY_BALL* wk );
extern BOOL			FactoryBall_AnmActiveCheck( FACTORY_BALL* wk );
extern void			FactoryBall_AnmChg( FACTORY_BALL* wk, u32 num );
extern void			FactoryBall_PalChg( FACTORY_BALL* wk, u32 pal );


#endif //_FACTORY_BALL_H_


