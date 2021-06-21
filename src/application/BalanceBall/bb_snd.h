//==============================================================================
/**
 * @file	bb_snd.h
 * @brief	簡単な説明を書く
 * @author	goto
 * @date	2007.10.01(月)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================

#ifndef __BB_SND_H__
#define __BB_SND_H__

#define BB_SND_COUNT		( SEQ_SE_DP_DECIDE )
#define BB_SND_START		( SEQ_SE_DP_DECIDE )

////////////////////////////////////////////////////////////////////////////////
//以下を当てて下さい。よろしくです。

#define BB_SND_KIRAKIRA		( SEQ_SE_PL_KIRAKIRA )	//ボールを動かしてキラキラ

//ループなので停止処理を呼んでください。Snd_SeStopBySeqNo(no,0);
#define BB_SND_HAND			( SEQ_SE_PL_HAND )		//拍手(小、大)同じ

#define BB_SND_KANSEI		( SEQ_SE_DP_DENDOU )	//歓声(拍手大の開始時に再生)

#define BB_SND_JUMP_IN		( SEQ_SE_DP_DANSA )		//マネネがボールに乗る
#define BB_SND_JUMP_OUT		( SEQ_SE_DP_DANSA )		//マネネがボールから落ちる
#define BB_SND_LANDING		( SEQ_SE_DP_SUTYA )		//マネネがボールから落ちて着地
#define BB_SND_NEJI			( SEQ_SE_PL_KIN )		//ネジガとれた

//音数問題で、現状は爆発音。
//#define BB_SND_HANABI		( SEQ_SE_PL_FW120 )		//花火が出るたびに再生してください
#define BB_SND_HANABI		( SEQ_SE_PL_140_2_2 )	//花火が出るたびに再生してください

#define BB_SND_LV_UP		( SEQ_SE_PL_LV_UP )		//レベルアップ

#endif
