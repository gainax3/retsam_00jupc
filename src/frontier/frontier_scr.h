//==============================================================================
/**
 * @file	frontier_scr.h
 * @brief	フロンティア用スクリプト制御のヘッダ
 * @author	matsuda
 * @date	2007.03.30(金)
 */
//==============================================================================
#ifndef __FRONTIER_SCR_H__
#define __FRONTIER_SCR_H__

#include "gflib/msg_print.h"						//STRCODE
#include "system\msgdata.h"							//MSGMAN_TYPE_DIRECT
#include "system\bmp_menu.h"
#include "system/bmp_list.h"
#include "system/pm_str.h"
#include "system/msgdata_util.h"


#include "system/wordset.h"							//WORDSET_Create
#include "system/clact_tool.h"

#include "frontier_types.h"
#include "fss_task.h"



//==============================================================================
//	構造体定義
//==============================================================================
///仮想マシン間で共有するワークの構造体
typedef struct{
	u16 work[FSW_WORK_MAX - FSW_PARAM_START];	///<各仮想マシンが共有して使用するワーク
}FS_PARAM_WORK;

//スクリプト制御ワーク構造体
struct _FS_SYSTEM{
	FMAIN_PTR fmain;

	FSS_TASK *fss_task[FSS_TASK_MAX];	///<スクリプト制御タスクへのポインタ
	FS_PARAM_WORK param_work;			///<各仮想マシンが共有して使用するワーク
	
	int heap_id;						///スクリプトで使用するヒープID
	u8 seq;								//

	MSGDATA_MANAGER* def_msgman;	// 標準メッセージマネージャー
	FSS_CODE * def_script;			// ロードした標準スクリプト

	WORDSET* wordset;				//単語セット
	STRBUF* msg_buf;				//メッセージバッファポインタ
	STRBUF* tmp_buf;				//テンポラリバッファポインタ
	u8 msg_talk_index;				///<会話メッセージの文字描画ルーチンのインデックス

	s8 fss_task_count;			//追加した仮想マシンの数

	u16 def_scene_id;				//メインシーンID

	int player_dir;					//イベント起動時の主人公の向き

//	u32 magic_no;					//イベントのワークがスクリプト制御ワークかを判別
	u8 MsgIndex;					//メッセージインデックス
	u8 anm_count;					//アニメーションしている数
//	u8 common_scr_flag;				//ローカル・共通スクリプト切り替えフラグ(0=ローカル、1=共通)
	u8 win_open_flag;				//会話ウィンドウを開いたかフラグ(0=開いていない、1=開いた)
	BOOL win_flag;					///<戦闘結果保持用ワーク

	//イベントウィンドウ
	FSEVWIN_PTR ev_win;				//イベントウィンドウワークへのポインタ

	//会話ウィンドウ
	GF_BGL_BMPWIN bmpwin_talk;		//ビットマップウィンドウ(会話メッセージ用)
	BMPMENU_WORK* mw;				//ビットマップメニューワーク

//	TARGET_OBJ_PTR target_obj;		//話しかけ対象のOBJのポインタ
//	TARGET_OBJ_PTR dummy_obj;		//透明ダミーOBJのポインタ

	u16* ret_script_wk;				//スクリプト結果を代入するワークのポインタ

	void * waiticon;				///<待機アイコンのポインタ
	
//	u16 work[EVSCR_WORK_MAX];		//ワーク(ANSWORK,TMPWORKなどの代わり)

//	fsysFunc next_func;				//スクリプト終了時に呼び出される関数

	void * subproc_work;			//サブプロセスとのやりとりに使用するワークへのポインタ
	void* pWork;					//ワークへの汎用ポインタ

//	TCB_PTR player_tcb;				//自機形態レポートTCB

	GF_BGL_BMPWIN CoinWinDat;		//ビットマップウィンドウデータ
	GF_BGL_BMPWIN GoldWinDat;		//ビットマップウィンドウデータ

	GF_BGL_BMPWIN* pParentNameCPWin;//ビットマップウィンドウデータ
	GF_BGL_BMPWIN* pChildNameCPWin;	//ビットマップウィンドウデータ

//	REPORT_INFO * riw;				///<レポート情報用ウィンドウ制御ワーク

	u16 save_seq0;
	u16 save_seq1;
	u16 save_rec_mode;
	u16 save_rensyou;
	u16 *save_ret_wk;
};

//==============================================================================
//	外部関数宣言
//==============================================================================
extern FSS_PTR FSS_SystemCreate(FMAIN_PTR fmain, int heap_id, int scene_id);
extern BOOL FSS_ScriptMain(FSS_PTR fss);
extern void FSS_SystemFree(FSS_PTR fss);
extern void FSS_ScrTaskAdd(FSS_PTR fss, int scene_id, int event_id);
extern void FSS_MsgManageSceneChange(FSS_PTR fss, int new_scene_id, int heap_id);
extern FS_PARAM_WORK *FSS_ParamWorkBackup(FSS_PTR fss, int heap_id);
extern void FSS_ParamWorkRecover(FSS_PTR fss, FS_PARAM_WORK *pw);

//--------------------------------------------------------------
//	スクリプトシステム系が使用するようなツール
//--------------------------------------------------------------
extern u16 * FSS_ParamWorkAdrsGet(FSS_PTR fss, int work_no);
extern FMAP_PTR FSS_GetFMapAdrs(FSS_PTR fss);



#endif	//__FRONTIER_SCR_H__
