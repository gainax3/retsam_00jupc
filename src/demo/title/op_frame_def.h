//=============================================================================
/**
 * @file	op_frame_def.h
 * @brief	オープニングデモフレーム定義
 * @author	Saito  GAME FREAK Inc.
 */
//=============================================================================

#ifndef __OP_FRANE_DEF_H__
#define __OP_FRANE_DEF_H__

#define FM_KIRA_SATRT1	(180)
#define FM_KIRA_SATRT2	(285)
#define FM_COPY_LIGHT_OUT	(115)
#define FM_FG_LOGO_IN		(265)
#define FM_GF_LOGO_OUT			(490)
#define FM_TITLE_LOGO_IN	(640)
#define FM_TITLE_LOGO_PARTICLE	(700)
#define FM_TITLE_LOGO_FLASH_OUT	(785)
#define FM_TITLE_LOGO_FLASH_IN	(FM_TITLE_LOGO_FLASH_OUT + 5)
#define FM_KIRA_DISP_START	(945)
#define FM_SKY_CAMERA_START	(975)	//(33*30 -15)

#define FM_CAMEA1_END		(40*30 - 15)
#define FM_MAP2_LOAD		(FM_CAMEA1_END + 6)
#define FM_MAP2_LOAD_1		(FM_MAP2_LOAD + 1)
#define FM_MAP2_LOAD_2		(FM_MAP2_LOAD + 2)
#define FM_MAP2_LOAD_3		(FM_MAP2_LOAD + 3)
#define FM_HEROINE_APPEAR	(FM_MAP2_LOAD)
#define FM_CAMEA2_END		(47*30 - 15)
#define FM_MAP3_LOAD		(FM_CAMEA2_END + 6)
#define FM_RIVAL_APPEAR		(FM_CAMEA2_END + 15)
#define FM_HAKASE_APPEAR	(FM_RIVAL_APPEAR + 3*30)
#define FM_BIRD_IN_1		(1460 - 15)
#define FM_BIRD_IN_2		(1490 - 15 + 15)
#define FM_BIRD_IN_3		(1510 - 15 + 15)
#define FM_CAM_ZOOM_OUT_START		(1560)
#define FM_RIVAL_OUT		(53*30 - 15)
#define FM_HAKASE_OUT		(FM_RIVAL_OUT)
#define FM_3D_WHITE_OUT		(1576)	//(54*30 - 15 - 30 + 1)

#define FM_HERO_WALK		(1600)	//(55*30 - 15)
#define FM_BALL_THROW		(62*30 - 15 - 15)		//ボール投げるアニメ
#define FM_BALL_HAND		(64*30 - 15 - 15 - 45)		//手のアップ
#define FM_BALL_HAND_RELEASE	(65*30 - 15 - 15 - 45 - 15)	//手からボールを離す

#define FM_POKE_APPEAR_FLASH	(1935 - 15)	//ポケモン登場時のフラッシュ
#define FM_POKE_APPEAR_FIRST	(FM_POKE_APPEAR_FLASH + 8)
#define FM_POKE_APPEAR_SECOND	(FM_POKE_APPEAR_FLASH + 14)
#define FM_POKE_APPEAR_THIRD	(FM_POKE_APPEAR_FLASH + 20)

#define FM_WAZA_RIGHT		(1995 + 15)//右側のポケモンが技繰り出し(以降技終了後次々と繰り出す)
#define FM_WAZA_LEFT		(2085 + 15+30)

#define FM_POKE_ATTACK		(2200 + 15)		//ポケモン体当たり
#define FM_SCENE2_END_WIPE	(FM_POKE_ATTACK + 1)	//2216	//シーン2終了のワイプ開始



#define FM_NAETORU_SETUP	(1715)
#define FM_RIVAL_SETUP		(1825)
#define FM_RIVAL_SLIDE_IN	(1825)
#define FM_HIKOZARU_SETUP	(1925)
#define FM_HEROINE_SETUP	(2040)
#define FM_POTTYAMA_SETUP	(2145)

#define FM_NAETORU_WAIT		(1795)
#define FM_HIKOZARU_WAIT	(1998)
#define FM_POTTYAMA_WAIT	(2216)

#if 0	//プラチナではテンガンザンの尺を短くして、その分タイトル(槍の柱デモ)で時間を使う
#define FM_TENGAN_IN		(2275)
#define FM_THUNDER1			(2430)
#define FM_THUNDER2			(2515)
#define FM_THUNDER3			(2545)
#define FM_THUNDER4			(2590)
#define FM_MT_AFFINE		(2630+30)
#define FM_TENGAN_OUT		(2660+20)
#define FM_END				(2700)
#else
#define FM_TENGAN_IN		(2285 - 15 - 65)
#define FM_THUNDER1			(2435 - 15 - 65)
#define FM_THUNDER2			(2465 - 15 - 65)
//#define FM_THUNDER3			(2580 - 90)
//#define FM_THUNDER4			(2620 - 90)
#define FM_MT_AFFINE		(2500 - 15 - 65)
#define FM_TENGAN_OUT		(FM_MT_AFFINE)
#define FM_END				(2430)
#endif

#endif	//__OP_FRANE_DEF_H__

