//==============================================================================
/**
 * @file	gds_main.h
 * @brief	GDSプロック制御メインのヘッダ
 * @author	matsuda
 * @date	2008.01.17(木)
 */
//==============================================================================
#ifndef __GDS_MAIN_H__
#define __GDS_MAIN_H__


//==============================================================================
//	構造体定義
//==============================================================================
///GDSプロック制御を呼び出すときに引き渡すパラメータ構造体
typedef struct{
	FIELDSYS_WORK *fsys;	///<フィールドシステムへのポインタ
	SAVEDATA *savedata;		///<セーブデータへのポインタ
	int connect;			///<0以外:初回接続(接続時の「はい/いいえ」選択無し)。0以外:2度目以降
	u8 gds_mode;			///<BR_MODE_GDS_???
}GDSPROC_PARAM;

///GDSプロック制御のメイン構造体
typedef struct{
	GDSPROC_PARAM *proc_param;		///<呼び出しパラメータへのポインタ
	BOOL connect_success;			///<TRUE:ネット接続中。　FALSE:接続していない
	PROC *sub_proc;					///<実行中のサブプロセス

	void 			*heapPtr;		///<NitroDWCに渡すヒープワークの解放用ポインタ
	NNSFndHeapHandle heapHandle;	///<heapPtrを32バイトアライメントに合わせたポインタ

	DWCInetControl   stConnCtrl;	// DWC接続ワーク
	
	int comm_initialize_ok;			///<TRUE:通信初期化済み

	
	//Wifi接続画面の結果を受け取る
	BOOL ret_connect;				///<TRUE:Wifi接続行った。　FALSE:接続しなかった
}GDSPROC_MAIN_WORK;


//==============================================================================
//	外部関数宣言
//==============================================================================

extern PROC_RESULT GdsMainProc_Init( PROC * proc, int * seq );
extern PROC_RESULT GdsMainProc_Main( PROC * proc, int * seq );
extern PROC_RESULT GdsMainProc_End( PROC * proc, int * seq );

//-- gds_connect.h --//
extern PROC_RESULT GdsConnectProc_Init( PROC * proc, int * seq );
extern PROC_RESULT GdsConnectProc_Main( PROC * proc, int * seq );
extern PROC_RESULT GdsConnectProc_End( PROC * proc, int * seq );


#endif	//__GDS_MAIN_H__
