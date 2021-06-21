//==============================================================================
/**
 * @file	bb_game.h
 * @brief	簡単な説明を書く
 * @author	goto
 * @date	2007.10.19(金)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================

#ifndef __BB_GAME_H__
#define __BB_GAME_H__

#ifdef PM_DEBUG

///< 有効で玉が転がらない
#define BB_SW_GAME_UPDATE_OFF

///< 有効でセットアップする
//#define BB_GAME_SETUP_ON

///< 有効でエンドレス
//#define BB_GAME_ENDLESS

///< 有効でエフェクト発動
//#define BB_EFFECT_ON

///< 有効で常にレアゲーム
///#define BB_REA_GAME_ON

#endif


/// ゲーム調整で操作できるパラメータ

///< 玉の傾き更新タイミング ( 30ﾌﾚ x 秒数 なので、30*2なら２秒に１回 )
#define BB_ROOL_UPDATE_LV1	( 30 * 4 )					///< 回転の更新ﾌﾚｰﾑ
#define BB_ROOL_UPDATE_LV2	( 30 * 3 )					///< 回転の更新ﾌﾚｰﾑ
#define BB_ROOL_UPDATE_LV3	( 30 * 2 )				///< 回転の更新ﾌﾚｰﾑ
#define BB_ROOL_UPDATE_LV4	( 30 * 1.5 )					///< 回転の更新ﾌﾚｰﾑ

///< パワー  最小値は19　最大は50が現実的です。 
///< 確立は それが起こる確率なので足して１００になるように
#define LV1_0_X				( 19 )		///< パワー
#define LV1_1_X				( 19 )
#define LV1_2_X				( 19 )
#define LV1_3_X				( 19 )
#define LV1_0_P				( 45 )		///< 確立
#define LV1_1_P				( 45 )
#define LV1_2_P				(  5 )
#define LV1_3_P				(  5 )

#define LV2_0_X				( 20 )		///< パワー
#define LV2_1_X				( 20 )
#define LV2_2_X				( 25 )
#define LV2_3_X				( 25 )
#define LV2_0_P				( 30 )		///< 確立
#define LV2_1_P				( 50 )
#define LV2_2_P				( 20 )
#define LV2_3_P				( 10 )

#define LV3_0_X				( 20 )		///< パワー
#define LV3_1_X				( 30 )
#define LV3_2_X				( 35 )
#define LV3_3_X				( 40 )
#define LV3_0_P				( 18 )		///< 確立
#define LV3_1_P				( 40 )
#define LV3_2_P				( 30 )
#define LV3_3_P				( 12 )

#define LV4_0_X				( 20 )		///< パワー
#define LV4_1_X				( 30 )
#define LV4_2_X				( 35 )
#define LV4_3_X				( 45 )
#define LV4_0_P				( 15 )		///< 確立
#define LV4_1_P				( 30 )
#define LV4_2_P				( 40 )
#define LV4_3_P				( 15 )

#define BB_POINT_FALL		( FX32_CONST(  45 ) )		///< 落ちる割合　POINT_Yからの割合

/// -----ここまで----------------------------------------







#define BB_REA_GAME			( 100 )					///< 発生率
#define BB_PLAYER_NUM		( 4 )					///< プレイヤー人数
#define BB_SCORE_UP			( 100 )					///< スコアｘ倍
#define BB_SCORE_MAX		( 100000 )				///< スコアの最大
#define BB_SCORE_MIN		(   4500 )				///< スコアの最小
#define BB_SCORE_MIN_CHK	( 1 )
#define BB_SCORE_SET_MIN	( 0 )

///< スコアの計算
#define BB_P_TO_SCORE( n )	( ( n / 10 ) * BB_SCORE_UP )
#define BB_P_LV1_BASE		(    1 )			///< １フレ辺りのスコア
#define BB_P_LV2_BASE		(    2 )
#define BB_P_LV3_BASE		(    3 )
#define BB_P_LV4_BASE		(    4 )
#define BB_P_LV1_BONUS		(  700 )			///< レベルアップボーナス
#define BB_P_LV2_BONUS		( 1400 )
#define BB_P_LV3_BONUS		( 2100 )
#define BB_P_LV4_BONUS		( 2800 )

///< ゲーム時間とレベルアップ
#define BB_GAME_TIMER		( ( 30 * 40 ) - 0 )	///< ゲーム時間
#define BB_GAME_LV_1UP		( ( 30 *  0 ) - 1 )	///< Lv1 になる時間
#define BB_GAME_LV_2UP		( ( 30 * 10 ) - 1 )	///< Lv2 になる時間
#define BB_GAME_LV_3UP		( ( 30 * 20 ) - 1 )	///< Lv3 になる時間
#define BB_GAME_LV_4UP		( ( 30 * 30 ) - 1 )	///< Lv4 になる時間
#define BB_GAME_LV_MAX		( ( 30 * 40 ) - 1 )	///< Lv5 になる時間
#define BB_GANE_LEVEL_MAX	( 4 )

#define BB_RECOVER_WAIT		( ( 30 * 3 ) )		///< 落ちてから戻るまで
#define BB_JUMP_BALL_WAIT	( 30 )				///< 玉に乗るまでのwait

///< シンクロの演出
#define BB_FEVER_COUNT		( ( 30 * 5 ) -1 )	///< フィーバーLVアップする時間
//#define BB_FEVER_LEVEL_MAX	( 7 )				///< 最大LV
#define BB_FEVER_LEVEL_MAX	( 8 )				///< 最大LV

///< 個人的な演出
#define BB_MY_EFF_1			( (  4 * 30 ) - 1 )	///< ライト
#define BB_MY_EFF_2			( (  9 * 30 ) - 1 )	///< 拍手x2
#define BB_MY_EFF_3			( ( 14 * 30 ) - 1 )	///< 紙ふぶき
#define BB_MY_EFF_4			( ( 19 * 30 ) - 1 )	///< 拍手x4
#define BB_MY_EFF_5			( ( 24 * 30 ) - 1 )	///< 花火

#define BB_SURFACE_LIMIT	( 192 + 32 )
#define BB_SURFACE_Y		( 192 + 64 )
#define BB_SURFACE_Y_FX		( ( 192 + 64 ) << FX32_SHIFT )



#define BB_CONST_INI_FRAME	( 8 )						///< 一定量で動くﾌﾚｰﾑ
#define BB_CONST_ROLL_FRAME	( BB_CONST_INI_FRAME + 2 )	///< 一定量で動くﾌﾚｰﾑ
#define BB_ACCEL_ROLL_FRAME	( 12 )						///< 加速するﾌﾚｰﾑ


///< 補正係数とか
#define BB_COEFFICIENT_DX		( ( ( 1.33 ) / 256 ) )		///< 移動量補正値
#define BB_COEFFICIENT_DY		( ( ( 1.00 ) / 192 ) )		///< 移動量補正値
#define BB_COEFFICIENT_A		( 1.00 )					///< 移動量の係数 この値を大きくすると移動量が増える
#define BB_COEFFICIENT_AB		( 2.35 * 0.80 )				///< さらに外側から付加する値（ボール）
#define BB_COEFFICIENT_AB_PEN	( 2.50 * 0.80 )				///< さらに外側から付加する値（ボール）
#define BB_COEFFICIENT_AM		( 0.22 )					///< さらに外側から付加する値（マネネ ペンでの移動）
#define BB_COEFFICIENT_AMI		( 0.80 )					///< さらに外側から付加する値（マネネ ボールでの移動）
#define BB_COEFFICIENT_AMB		( 0.80 )					///< さらに外側から付加する値（マネネ ボールでの移動）
#define BB_COEFFICIENT_POW		( 1 )						///< 1ドットでは計算結果の反映ができないのでｘ倍して扱う

#define BB_COEFFICIENT_PEN( n )	( ( 100 - ( n >> FX32_SHIFT ) ) / 40 )


///< 判定座標
#define BB_POINT_Y			( FX32_CONST( 100 ) )		///< この数値は変化しない


#define BB_POINT_1S			( FX32_CONST(  75 ) )		///< 落ちる割合　POINT_Yからの割合
#define BB_POINT_1E			( FX32_CONST(  61 ) )		///< 落ちる割合　POINT_Yからの割合
#define BB_POINT_2S			( FX32_CONST(  60 ) )			///< 落ちる割合　POINT_Yからの割合
#define BB_POINT_2E			( FX32_CONST(  46 )  )				///< 落ちる割合　POINT_Yからの割合


///< ゲームデータ
#define LVA_DEFAUT_POW		( 20 )		///< 最初にかかる力



#endif
