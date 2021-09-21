
#ifndef __BR_SND_DEF_H__
#define __BR_SND_DEF_H__

#include "system/snd_tool.h"

/// Snd_SePlay( );

enum {
	
	//eBR_SND_NG		= SEQ_SE_DP_CUSTOM06,		//タッチ：NG
	//eBR_SND_NG			= SEQ_SE_PL_BREC57,			//タッチ：NG
	eBR_SND_NG			= SEQ_SE_PL_BREC58,			//タッチ：NG

	//eBR_SND_TOUCH		= SEQ_SE_DP_BUTTON9,		//タッチ：OK
	eBR_SND_TOUCH		= SEQ_SE_PL_BREC57,			//タッチ：OK
	//eBR_SND_TOUCH		= SEQ_SE_PL_BREC58,			//タッチ：OK
	//eBR_SND_TOUCH		= SEQ_SE_PL_BREC59,			//タッチ：OK

	//eBR_SND_SEND_OK	= SEQ_SE_DP_TARARARAN,		//データ送信"完了"
	eBR_SND_SEND_OK		= SEQ_SE_PL_BREC12,			//データ送信"完了"

	//eBR_SND_RECV_OK	= SEQ_SE_DP_TARARARAN,		//データ受信"完了"
	eBR_SND_RECV_OK		= SEQ_SE_PL_BREC12,			//データ受信"完了"

	eBR_SND_FRONTIER_IN	= SEQ_SE_PL_BREC20,			//「フロンティアのきろく」呼び出し
	eBR_SND_FRONTIER_OUT= SEQ_SE_PL_BREC21,			//「フロンティアのきろく」抜ける

	/////////////////////////////////////////////////////////////////////////////////////
	//08.03.19 追加
	eBR_SND_SEARCH		= SEQ_SE_PL_BREC80,			//データ検索中、送信中、受信中(ループ音)

	//↓ループにするか未定
	eBR_SND_SCROLL		= SEQ_SE_PL_BUTTON,			//タッチ：スクロールバーを動かす

	eBR_SND_COL_CHG		= eBR_SND_TOUCH,			//タッチ：画面の色を変更

	//eBR_SND_TOUCH_HERE= SEQ_SE_DP_SELECT,			//画面に滴？が落ちた音(TOUCH HERE)
	//eBR_SND_TOUCH_HERE	= SEQ_SE_PL_BREC11,			//画面に滴？が落ちた音(TOUCH HERE)
	eBR_SND_TOUCH_HERE	= SEQ_SE_PL_BREC11_2,			//画面に滴？が落ちた音(TOUCH HERE)

	//↓br_start.cのSE呼び出し定義を置き換えて欲しいです。
	eBR_SND_START	= SEQ_SE_PL_BREC03,			//起動
	eBR_SND_START2	= SEQ_SE_PL_BREC03,				//起動
	eBR_SND_START3	= SEQ_SE_PL_BREC03,				//起動

	//ほか何かあるかな？
};


#endif
