//==============================================================================
/**
 * @file	footprint_control.h
 * @brief	足跡ボード：頻繁に調整しそうなものをまとめた
 * @author	matsuda
 * @date	2008.01.26(土)
 */
//==============================================================================
#ifndef __FOOTPRINT_CONTROL_H__
#define __FOOTPRINT_CONTROL_H__


//==============================================================================
//	定数定義
//==============================================================================
///インクの下地のEVY値(元の色からどこまで色を落とすか。0 ～ 16)
#define INK_FOUNDATION_EVY		(12)
///インクの下地の目指すカラー
#define INK_FOUNDATION_COLOR	(0x0000)

///インクパレットに置かれている自分の足跡マークの色：ホワイトボード
#define INKPAL_FOOT_COLOR_CODE_WHITE		(0x0000)
///インクパレットに置かれている自分の足跡マークの色：ブラックボード
#define INKPAL_FOOT_COLOR_CODE_BLACK		(0x7fff)

///押されてるインクパレットを変更する時のEVY
#define TOUCH_EFF_EVY			(6)
///押されてるインクパレットを変更する時のカラーコード
#define TOUCH_EFF_COLOR			(0x0000)


//--------------------------------------------------------------
//	インクゲージ
//--------------------------------------------------------------
///インクゲージの長さ(ドット単位)
#define INK_GAUGE_LEN				(32)	//※編集禁止
///インクゲージの最大容量
#define INK_GAUGE_TANK_MAX			(INK_GAUGE_LEN << 8)	//※編集禁止
///MAXの状態から何回スタンプを打ったらゲージを空っぽにするか
#define INK_GAUGE_MAX_STAMP			(6)
///1つスタンプを押すごとに消費する量
#define INK_GAUGE_CONSUME_STAMP	(INK_GAUGE_TANK_MAX / INK_GAUGE_MAX_STAMP)	//※編集禁止
///インクを消費させた時、ゲージを何フレームかけて、消費後の位置まで移動させるか
#define INK_GAUGE_SUB_FRAME			(4)
///空っぽの状態から何フレームでインクを最大まで回復させるか
#define INK_GAUGE_RESTORE_FRAME		(60)
///1フレームで回復させるインク量
#define INK_GAUGE_RESTORE_NUM		(INK_GAUGE_TANK_MAX / INK_GAUGE_RESTORE_FRAME)	//※編集禁止



#endif	//__FOOTPRINT_CONTROL_H__
