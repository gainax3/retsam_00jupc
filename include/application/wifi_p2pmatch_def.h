//============================================================================================
/**
 * @file	wifi_p2pmatch_def.h
 * @bfief	WIFI P2P接続のマッチングに関する定義  スクリプトで使用
 * @author	k.ohno
 * @date	06.04.07
 */
//============================================================================================
#ifndef __WIFI_P2PMATCH_DEF_H__
#define __WIFI_P2PMATCH_DEF_H__


// 各デバック用定義
#ifdef PM_DEBUG
#define WFP2P_DEBUG	// デバック機能をON

#ifdef WFP2P_DEBUG
//#define WFP2P_DEBUG_EXON	// ほぼ必要ないリストも表示する
//#define WFP2P_DEBUG_PLON_PC	// プレイヤーの初期位置パソコン前
#endif

#endif

// P2PMATCH への指定
#define WIFI_P2PMATCH_DPW     (1)   // この後DPWへ移行する
#define WIFI_P2PMATCH_P2P     (2)   // このままP2P処理に移行



// P2PMATCH からの戻り値
#define WIFI_P2PMATCH_SBATTLE50     (1)   // 通信対戦呼び出し
#define WIFI_P2PMATCH_SBATTLE100     (2)   // 通信対戦呼び出し
#define WIFI_P2PMATCH_SBATTLE_FREE     (3)   // 通信対戦呼び出し
#define WIFI_P2PMATCH_DBATTLE50     (4)   // 通信対戦呼び出し
#define WIFI_P2PMATCH_DBATTLE100     (5)   // 通信対戦呼び出し
#define WIFI_P2PMATCH_DBATTLE_FREE     (6)   // 通信対戦呼び出し
#define WIFI_P2PMATCH_TRADE      (7)   // ポケモントレード呼び出し
#define WIFI_P2PMATCH_END        (8)   // 通信切断してます。終了します。
#define WIFI_P2PMATCH_UTIL       (9)   // 接続設定を呼び出します。戻れません
#define WIFI_P2PMATCH_DPW_END    (10)   // 世界対戦用設定終了
#define WIFI_P2PMATCH_POFIN      (11)   // ポフィン料理呼び出し
#define WIFI_P2PMATCH_FRONTIER   (12)   // フロンティア呼び出し
#define WIFI_P2PMATCH_BUCKET	(13)   // バケットゲーム呼び出し
#define WIFI_P2PMATCH_BALANCEBALL	(14)   // 玉乗りゲーム呼び出し
#define WIFI_P2PMATCH_BALLOON	(15)   // ばるーんゲーム呼び出し

#ifdef WFP2P_DEBUG_EXON	// 
#define WIFI_P2PMATCH_BATTLEROOM	(16)   // バトルルーム呼び出し
#define WIFI_P2PMATCH_MBATTLE_FREE	(17)   // 通信対戦マルチバトル呼び出し
#endif	// WFP2P_DEBUG_EXON


#endif //__WIFI_P2PMATCH_DEF_H__

