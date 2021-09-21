
//============================================================================================
/**
 * @file	attr_def.h
 * @bfief	戦闘背景用定数定義
 * @author	HisashiSogabe
 * @date	06.02.24
 */
//============================================================================================

#ifndef __ATTR_DEF_H_
#define __ATTR_DEF_H_

//地面に関する定義
#define	GROUND_ID_GRAVEL	(0)		//砂利
#define	GROUND_ID_SANDS		(1)		//砂浜
#define	GROUND_ID_LAWN		(2)		//芝生
#define	GROUND_ID_POOL		(3)		//水たまり
#define	GROUND_ID_ROCK		(4)		//岩肌
#define	GROUND_ID_CAVE		(5)		//洞窟
#define	GROUND_ID_SNOW		(6)		//雪原
#define	GROUND_ID_WATER		(7)		//水上
#define	GROUND_ID_ICE		(8)		//氷上
#define	GROUND_ID_FLOOR		(9)		//床
#define	GROUND_ID_BOG		(10)	//沼地
#define	GROUND_ID_BRIDGE	(11)	//吊り橋
#define	GROUND_ID_END		(12)	//それ以外（一応定義）
//プラチナから追加
#define	GROUND_ID_BIG_FOUR_A	(12)	//四天王地面A
#define	GROUND_ID_BIG_FOUR_B	(13)	//四天王地面B
#define	GROUND_ID_BIG_FOUR_C	(14)	//四天王地面C
#define	GROUND_ID_BIG_FOUR_D	(15)	//四天王地面D
#define	GROUND_ID_CHAMPION		(16)	//チャンピオン地面
#define	GROUND_ID_HAKAI			(17)	//破れた世界(相手地面有り)
#define	GROUND_ID_TOWER			(18)	//バトルタワー
#define	GROUND_ID_FACTORY		(19)	//バトルファクトリー
#define	GROUND_ID_ROULETTE		(20)	//バトルルーレット
#define	GROUND_ID_CASTLE		(21)	//バトルキャッスル
#define	GROUND_ID_STAGE			(22)	//バトルステージ
#define GROUND_ID_HAKAI_GIRA	(23)	//破れた世界(ギラティナ戦用。相手地面無し)
#define	GROUND_ID_ALL		(24)	//それ以外（一応定義）

//※上記定義で、エクセル表では、砂利＆砂浜、芝生＆水たまり、岩肌＆洞窟となっているが、
//　分かれる可能性もあるという話なので、別定義にしています

//背景に関する定義
#define	BG_ID_BASIC			(0)		//草原
#define	BG_ID_SEA			(1)		//海
#define	BG_ID_CITY			(2)		//街
#define	BG_ID_FOREST		(3)		//森林
#define	BG_ID_MOUNTAIN		(4)		//山岳
#define	BG_ID_SNOW_MOUNTAIN	(5)		//雪山
#define	BG_ID_ROOM_A		(6)		//室内Ａ
#define	BG_ID_ROOM_B		(7)		//室内Ｂ
#define	BG_ID_ROOM_C		(8)		//室内Ｃ
#define	BG_ID_CAVE_A		(9)		//洞窟Ａ
#define	BG_ID_CAVE_B		(10)	//洞窟Ｂ
#define	BG_ID_CAVE_C		(11)	//洞窟Ｃ
//プラチナから追加
#define BG_ID_BIG_FOUR_A	(12)	//四天王
#define BG_ID_BIG_FOUR_B	(13)	//四天王
#define BG_ID_BIG_FOUR_C	(14)	//四天王
#define BG_ID_BIG_FOUR_D	(15)	//四天王
#define BG_ID_CHAMPION		(16)	//チャンピオン
#define BG_ID_HAKAI			(17)	//破れた世界
#define BG_ID_TOWER			(18)	//バトルタワー
#define BG_ID_FACTORY		(19)	//バトルファクトリー
#define BG_ID_ROULETTE		(20)	//バトルルーレット
#define BG_ID_CASTLE		(21)	//バトルキャッスル
#define BG_ID_STAGE			(22)	//バトルステージ


#endif __ATTR_DEF_H_

