//==============================================================================
/**
 * @file	guru2_snd.h
 * @brief	ぐるぐる交換　サウンド定義ヘッダーファイル
 * @author	kagaya
 * @data	05.07.13
 */
//==============================================================================
#ifndef GURU2_SND_H_FILE
#define GURU2_SND_H_FILE

//==============================================================================
//	define
//==============================================================================
#define GURU2_SE_COUNT_3		(SEQ_SE_DP_DECIDE)	//3
#define GURU2_SE_COUNT_2		(SEQ_SE_DP_DECIDE)	//2
#define GURU2_SE_COUNT_1		(SEQ_SE_DP_DECIDE)	//1
#define GURU2_SE_START			(SEQ_SE_DP_CON_016)	//スタート
#define GURU2_SE_BTN_PUSH		(SEQ_SE_DP_BUTTON9)	//下画面ボタン押す

//#define GURU2_SE_EGG_FALL		(SEQ_SE_DP_HYUN2)	//スタート時にタマゴが落ちてくる
#define GURU2_SE_EGG_FALL		(SEQ_SE_DP_FW104)	//スタート時にタマゴが落ちてくる

//#define GURU2_SE_EGG_JUMP		(SEQ_SE_DP_DANSA4)	//タマゴジャンプ
#define GURU2_SE_EGG_JUMP		(SEQ_SE_DP_DANSA5)	//タマゴジャンプ

#define GURU2_SE_EGG_LANDING	(SEQ_SE_DP_SUTYA)	//タマゴ着地

#define GURU2_SE_TIMEUP			(SEQ_SE_DP_PINPON)	//タイムアップ
//#define GURU2_SE_TIMEUP		(SEQ_SE_DP_MAZYO)	//鳴らない)


#endif //GURU2_SND_H_FILE
