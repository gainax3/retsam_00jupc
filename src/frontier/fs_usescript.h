//==============================================================================
/**
 * @file	fs_usescript.h
 * @brief	フロンティア用スクリプトで使用する定義など
 * @author	matsuda
 * @date	2007.03.30(金)
 *
 * アセンブラファイルからincludeする場合は、外側で先に__ASM_ONLY_INC_をdefine定義しておくこと
 */
//==============================================================================
#ifndef __FS_USESCRIPT_H__
#define __FS_USESCRIPT_H__

#ifdef __ASM_ONLY_INC_
#define	__ASM_NO_DEF_
#define ASM_CPP							//pm_version.h
#endif	//__ASM_ONLY_INC_

///アセンブラデータの終端コードに使用
#define FSS_DATA_END_CODE	(0xfd13)	//適当な値

///OBJコード：自分自身
#define FSS_CODE_MYSELF		(0xeeee)
///OBJコード：通信プレイヤー
#define FSS_CODE_SIO_PLAYER	(0xeeef)

#define ON					(1)
#define OFF					(0)

//include\gflib\camera.hから抜粋(松田さん確認)
#define FSS_CAMERA_PERSPECTIV	(0)		// 透視射影
#define FSS_CAMERA_ORTHO		(1)		// 正射影

//--------------------------------------------------------------
//	ワークID定義
//--------------------------------------------------------------
///そのスクリプトのみで使用するワーク
#define FSW_LOCAL_START		(0x8000)
#define FSW_LOCAL0			(FSW_LOCAL_START + 0)
#define FSW_LOCAL1			(FSW_LOCAL_START + 1)
#define FSW_LOCAL2			(FSW_LOCAL_START + 2)
#define FSW_LOCAL3			(FSW_LOCAL_START + 3)
#define FSW_LOCAL4			(FSW_LOCAL_START + 4)
#define FSW_LOCAL5			(FSW_LOCAL_START + 5)
#define FSW_LOCAL6			(FSW_LOCAL_START + 6)
#define FSW_LOCAL7			(FSW_LOCAL_START + 7)
#define FSW_LOCAL_MAX		(FSW_LOCAL7 + 1)

///スクリプト間で共有するワーク
#define FSW_PARAM_START		(FSW_LOCAL_MAX)
#define	FSW_ANSWER			(FSW_PARAM_START + 0)
#define	FSW_PARAM0			(FSW_PARAM_START + 1)
#define	FSW_PARAM1			(FSW_PARAM_START + 2)
#define	FSW_PARAM2			(FSW_PARAM_START + 3)
#define	FSW_PARAM3			(FSW_PARAM_START + 4)
#define	FSW_PARAM4			(FSW_PARAM_START + 5)
#define	FSW_PARAM5			(FSW_PARAM_START + 6)
#define	FSW_PARAM6			(FSW_PARAM_START + 7)
#define FSW_WORK_MAX		(FSW_PARAM6 + 1)

///レジスタ
#define FSW_REG_START		(FSW_WORK_MAX)
#define	FSW_REG0			(FSW_REG_START + 0)
#define	FSW_REG1			(FSW_REG_START + 1)
#define	FSW_REG2			(FSW_REG_START + 2)
#define	FSW_REG3			(FSW_REG_START + 3)
#define FSW_REG_MAX			(FSW_REG3 + 1)
#define FSW_REG_WORK_MAX	(FSW_REG_MAX - FSW_REG_START)

//--------------------------------------------------------------
//	アクターポインタ記憶用バッファのID
//--------------------------------------------------------------
#define ACTWORK_0			(0)
#define ACTWORK_1			(1)
#define ACTWORK_2			(2)
#define ACTWORK_3			(3)
#define ACTWORK_4			(4)
#define ACTWORK_5			(5)
#define ACTWORK_6			(6)
#define ACTWORK_7			(7)
#define ACTWORK_MAX			(8)

#define ACTWORK_RESOURCE_MAX	(ACTWORK_MAX)

//--------------------------------------------------------------
//	表示用ポケモン管理ID
//--------------------------------------------------------------
#define POKE_DISP_MANAGER_ID_START	(50000)
#define POKE_DISP_MANAGER_ID_0		(POKE_DISP_MANAGER_ID_START + 0)
#define POKE_DISP_MANAGER_ID_1		(POKE_DISP_MANAGER_ID_START + 1)
#define POKE_DISP_MANAGER_ID_2		(POKE_DISP_MANAGER_ID_START + 2)
#define POKE_DISP_MANAGER_ID_3		(POKE_DISP_MANAGER_ID_START + 3)
#define POKE_DISP_MANAGER_ID_NUM	(POKE_DISP_MANAGER_ID_3 - POKE_DISP_MANAGER_ID_START + 1)

//--------------------------------------------------------------
//	パーティクルのSPA管理番号	※PARTICLE_GLOBAL_MAXを超えないようにすること
//	
//	SPAWORK_?はスクリプトで使用する
//	SPAWORK_EFF_?はタスク上などCファイルから使用する
//	エンカウントエフェクトなど、タスク上から生成されるものはSPAWORK_EFF_?を使用していれば
//	スクリプトで使用している管理番号とは被らない
//	場合によってはSPAWORK_EFF_?の方はマネージャ式にするかも
//--------------------------------------------------------------
#define SPAWORK_0			(0)		//スクリプト上で使用
#define SPAWORK_1			(1)
#define SPAWORK_2			(2)
#define SPAWORK_3			(3)
#define SPAWORK_EFF_0		(SPAWORK_3 + 1)			//Cファイルから使用
#define SPAWORK_EFF_1		(SPAWORK_EFF_0 + 1)
#define SPAWORK_EFF_2		(SPAWORK_EFF_0 + 2)
#define SPAWORK_EFF_3		(SPAWORK_EFF_0 + 3)
#define SPAWORK_MAX			(SPAWORK_EFF_3 + 1)


//==============================================================================
//	
//==============================================================================
#define	LT				0				/* <	*/
#define	EQ				1				/* ==	*/
#define	GT				2				/* >	*/
#define	LE				3				/* <=	*/
#define	GE				4				/* >=	*/
#define	NE				5				/* !=	*/
#define	EQUAL			EQ
#define	LITTLER			LT
#define	GREATER			GT
#define	LT_EQ			LE
#define	GT_EQ			GE

#define FSEV_WIN_B_CANCEL		(0xfffe)		//Bキャンセル
#define FSEV_WIN_NOTHING		(0xeeee)		//初期化の値
#define FSEV_WIN_COMPULSION_DEL	(0xeedd)		//強制削除の値
#define	FSEV_WIN_LIST_LABEL		(0xfa)			//リスト使用時にラベルを指定する定義(スクリプト用)
#define	FSEV_WIN_TALK_MSG_NONE	(0xff)			//リスト使用時の会話メッセージなし

//ワイプフェードの基本の値
#define SCR_WIPE_DIV				(WIPE_DEF_DIV)
#define SCR_WIPE_SYNC				(WIPE_DEF_SYNC)


//==============================================================================
//	アニメーションコード	※AnmCmdTblと並びを同じにしておくこと
//==============================================================================
#define FC_DIR_U				0		///<上に向く
#define FC_DIR_D				1		///<下に向く
#define FC_DIR_L				2		///<左に向く
#define FC_DIR_R				3		///<右に向く
#define FC_WALK_U_8F			4		///<上に移動　8フレーム
#define FC_WALK_D_8F			5		///<下に移動　8フレーム
#define FC_WALK_L_8F			6		///<左に移動　8フレーム
#define FC_WALK_R_8F			7		///<右に移動　8フレーム
#define FC_WALK_U_16F			8		///<上に移動　16フレーム
#define FC_WALK_D_16F			9		///<下に移動　16フレーム
#define FC_WALK_L_16F			10		///<左に移動　16フレーム
#define FC_WALK_R_16F			11		///<右に移動　16フレーム
#define FC_WALK_U_4F			12		///<上に移動　4フレーム
#define FC_WALK_D_4F			13		///<下に移動　4フレーム
#define FC_WALK_L_4F			14		///<左に移動　4フレーム
#define FC_WALK_R_4F			15		///<右に移動　4フレーム
#define FC_STAY_WALK_U_8F		16		///<その場歩き　上向き　8フレーム
#define FC_STAY_WALK_D_8F		17		///<その場歩き　下向き　8フレーム
#define FC_STAY_WALK_L_8F		18		///<その場歩き　左向き　8フレーム
#define FC_STAY_WALK_R_8F		19		///<その場歩き　右向き　8フレーム
#define FC_STAY_WALK_U_16F		20		///<その場歩き　上向き　16フレーム
#define FC_STAY_WALK_D_16F		21      ///<その場歩き　下向き　16フレーム
#define FC_STAY_WALK_L_16F		22      ///<その場歩き　左向き　16フレーム
#define FC_STAY_WALK_R_16F		23      ///<その場歩き　右向き　16フレーム
#define FC_STAY_WALK_U_2F		24		///<その場歩き　上向き　8フレーム
#define FC_STAY_WALK_D_2F		25		///<その場歩き　下向き　8フレーム
#define FC_STAY_WALK_L_2F		26		///<その場歩き　左向き　8フレーム
#define FC_STAY_WALK_R_2F		27		///<その場歩き　右向き　8フレーム
#define FC_STAY_WALK_U_4F		28		///<その場歩き　上向き　8フレーム
#define FC_STAY_WALK_D_4F		29		///<その場歩き　下向き　8フレーム
#define FC_STAY_WALK_L_4F		30		///<その場歩き　左向き　8フレーム
#define FC_STAY_WALK_R_4F		31		///<その場歩き　右向き　8フレーム
#define FC_ANMCMD_END			32	///<AnmCmdTblに定義してある命令ここまで。下記からはウェイトのみ
#define FC_WAIT_1F				(FC_ANMCMD_END + 0)		///<1フレーム待ち
#define FC_WAIT_2F				(FC_ANMCMD_END + 1)		///<2フレーム待ち
#define FC_WAIT_4F				(FC_ANMCMD_END + 2)		///<4フレーム待ち
#define FC_WAIT_8F				(FC_ANMCMD_END + 3)		///<8フレーム待ち
#define FC_WAIT_15F				(FC_ANMCMD_END + 4)		///<15フレーム待ち
#define FC_WAIT_16F				(FC_ANMCMD_END + 5)		///<16フレーム待ち
#define FC_WAIT_32F				(FC_ANMCMD_END + 6)		///<32フレーム待ち
#define FC_SYSCMD_START			(FC_ANMCMD_END + 7)	//システムコマンド開始
#define FC_SYSCMD_VISIBLE_ON	(FC_SYSCMD_START + 0)	///<OBJを表示させる
#define FC_SYSCMD_VISIBLE_OFF	(FC_SYSCMD_START + 1)	///<OBJを非表示にする
#define FC_SYSCMD_END			(FC_SYSCMD_START + 2)//システムコマンド終了



//==============================================================================
//	アセンブラファイルのみのinclude
//==============================================================================
#ifdef __ASM_ONLY_INC_

#include "../../../include/pm_version.h"
#include "../../../include/system/wipe.h"			//ワイプ
#include "../../../include/system/snd_def.h"
#include "../../field/fieldobj_code.h"
#include "../../field/comm_direct_counter_def.h"
#include "../../../include/application/wifi_2dmap/wifi_2dcharcon.h"
#include "../../../include/application/wifi_2dmap/wf2dmap_common_c.h"
#include "../../../include/application/wifi_2dmap/wf2dmap_objst.h"
#include "../fss_scene.h"	//シーンID
#include "../frontier_act_id.h"
#include "../frontier_def.h"
#include "../../../include/savedata/battle_rec.h"
#include "../../../include/savedata/record.h"
#include "../../../include/savedata/score_def.h"

#endif	//__ASM_ONLY_INC_


#endif	//__FS_USESCRIPT_H__
