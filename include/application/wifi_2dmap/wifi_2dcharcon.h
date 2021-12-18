//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wifi_2dcharcon.h
 *	@brief		定数のdefineバージョン
 *	@author		tomoya takahashi
 *	@data		2007.04.12
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WIFI_2DCHARCON_H__
#define __WIFI_2DCHARCON_H__

//-------------------------------------
///	動作タイプ
// 内部的には、セルアニメリソースの種類
//=====================================
#define	WF2DC_C_MOVERUN	(0)		// 歩き＋振り向き＋走りアニメ（主人公のみ）
#define	WF2DC_C_MOVENORMAL	(1)	// 歩き＋振り向きのみアニメ
#define	WF2DC_C_MOVETURN	(2)	// 振り向きのみアニメ
		// 拡張アニメ
		// アニメーションタイプ数
#define	WF2DC_C_MOVENUM	(3)

//-------------------------------------
///	アニメーションタイプ
//　動作タイプがオーソドックスアニメの場合にのみ
//　使用できるフラグです。
//	
//=====================================
#define	WF2DC_C_ANMWAY	(0)	// 向き変えアニメ	1フレームで切り替わります
#define	WF2DC_C_ANMROTA	(1)	// 回転アニメ		ループ

		// WF2DC_C_MOVETURNモードでないと指定できません
#define	WF2DC_C_ANMWALK	(2)	// 歩きアニメ		1歩8フレーム
#define	WF2DC_C_ANMTURN	(3)	// 振り向きアニメ	2フレーム

		// WF2DC_C_MOVERUNモードでないと指定できません
#define	WF2DC_C_ANMRUN	(4)	// 走りアニメ		1歩4フレーム

		// 壁方向アニメ
#define	WF2DC_C_ANMWALLWALK	(5)	// 壁歩きアニメ		1歩16フレーム
		
		// ゆっくり歩き
#define	WF2DC_C_ANMSLOWWALK	(6)	// ゆっくりアニメ		1歩16フレーム

		// 高速歩き
#define	WF2DC_C_ANMHIGHWALK2 (7)	// ゆっくりアニメ		1歩2フレーム
#define	WF2DC_C_ANMHIGHWALK4 (8)	// ゆっくりアニメ		1歩4フレーム
	
		// 何も無いときはこれ
#define	WF2DC_C_ANMNONE	(9)
#define	WF2DC_C_ANMNUM	(10)



#endif		// __WIFI_2DCHARCON_H__

