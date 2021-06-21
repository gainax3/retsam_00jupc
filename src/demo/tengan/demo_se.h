#ifndef __DEMO_SE_H__
#define __DEMO_SE_H__

#include "system/snd_tool.h"

enum {
	////////////////////////////
	//ディアルガ、パルキア登場
	////////////////////////////
	
	//赤い鎖
	//DS_EFF_00 = SEQ_SE_PL_KUSARI,						//これだけBANK_SE_FIELD

	//黒い玉がゆらゆら落ちてくる
	DS_EFF_01 = SEQ_SE_DP_CLIMAX01,						//

	//黒い玉が大きくなる
	DS_EFF_02 = SEQ_SE_DP_CLIMAX03,						//

	//迫力出すために雷系の音
	DS_EFF_03 = SEQ_SE_DP_CLIMAX06,						//黒い玉のタメ中に鳴らす(DPと同じ感じで)

	//迫力だすためにゴー系の音
	DS_EFF_04 = SEQ_SE_DP_CLIMAX09,						//黒い玉のタメ中に鳴らす(DPと同じ感じで)

	//黒い玉に縦線が入る音
	DS_EFF_05 = SEQ_SE_DP_CLIMAX10,						//

	//ディアルガ、パルキアが黒い玉から登場
	DS_EFF_06 = SEQ_SE_DP_CLIMAX12,						//

	////////////////////////////
	//ギラティナ登場
	////////////////////////////

	//DS_DARK = SEQ_SE_PL_W467,							//不穏な雰囲気
	DS_HOLE = SEQ_SE_PL_W060,							//穴が開く
	DS_WAVE	= SEQ_SE_PL_W082C,							//登場

	//DS_BLACK_BALL_ON = SEQ_SE_DP_CLIMAX03,
	
	////////////////////////////
	//ユクシーなど登場
	////////////////////////////
	
	//DS_APPEAR = SEQ_SE_DP_182,						//光飛んでいく
	//DS_APPEAR2= SEQ_SE_DP_W392,						//光広がる
	DS_APPEAR2= SEQ_SE_PL_W392,							//光広がる

	//DS_JUMP = SEQ_SE_DP_023,
	//DS_COIN = SEQ_SE_DP_031,
	//DS_TERE	= SEQ_SE_DP_W195,
	
};


#endif
