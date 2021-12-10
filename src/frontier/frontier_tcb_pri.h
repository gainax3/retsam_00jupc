//==============================================================================
/**
 * @file	frontier_tcb_pri.h
 * @brief	フロンティアのシステム関連TCBプライオリティ定義
 * @author	matsuda
 * @date	2007.04.16(月)
 */
//==============================================================================
#ifndef __FRONTIER_TCB_PRI_H__
#define __FRONTIER_TCB_PRI_H__


//--------------------------------------------------------------
//	システム系
//--------------------------------------------------------------
///マップ：オブジェクトメイン実行処理
#define TCBPRI_OBJSYS_UPDATE		(60000)
///マップ：オブジェクトコマンド判断処理
#define TCBPRI_CMDJUDGE_UPDATE		(61000)
///マップ：メインループの最後に行うシステム関連の更新処理
#define TCBPRI_MAP_UPDATE			(80000)

///フィールドOBJアニメーション実行TCBプライオリティ
#define TCBPRI_FIELDOBJ_ANIME		(TCBPRI_OBJSYS_UPDATE + 100)

///ステージ：BGアニメ制御タスク
#define TCBPRI_STAGE_BGANIME_CONTROL		(TCBPRI_MAP_UPDATE - 500)
///ステージ：パレットアニメ制御タスク
#define TCBPRI_STAGE_PALANIME_CONTROL		(TCBPRI_STAGE_BGANIME_CONTROL + 1)
///ステージ：ラスターアニメ制御タスク
#define TCBPRI_STAGE_LASTER_CONTROL			(TCBPRI_STAGE_PALANIME_CONTROL + 1)

///地震制御タスク
#define TCBPRI_SHAKE				(70000)
///Window制御タスク
#define TCBPRI_WND					(300)

//--------------------------------------------------------------
//	エフェクト系
//--------------------------------------------------------------
///フロンティアブレーン戦のカットインエフェクト
#define TCBPRI_ENC_CUTIN			(1000)
///カットインエフェクトのジグザグBGのパレットアニメ
#define TCBPRI_ENC_CUTIN_BGPALANM	(1100)


#endif	//__FRONTIER_TCB_PRI_H__
