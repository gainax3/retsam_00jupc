//==============================================================================
/**
 * @file	emai_main.c
 * @brief	Eメール画面メインPROC
 * @author	matsuda
 * @date	2007.10.17(水)
 */
//==============================================================================
#include "common.h"
#include <dwc.h>
#include "libdpw/dpw_tr.h"
#include "system/procsys.h"
#include "system/arc_tool.h"
#include "system/arc_util.h"
#include "system/clact_tool.h"
#include "system/msgdata_util.h"
#include "system/wordset.h"
#include "msgdata/msg.naix"
#include "system/wipe.h"
#include "system/fontproc.h"
#include "system/lib_pack.h"
#include "system/lib_pack.h"
#include "system/fontoam.h"
#include "system/window.h"
#include "gflib/touchpanel.h"
#include "system/bmp_menu.h"
#include "system/snd_tool.h"
#include "gflib/strbuf_family.h"
#include "savedata/wifilist.h"
#include "savedata/zukanwork.h"
#include "communication/communication.h"
#include "demo/title.h"	//TitleProcData

#include "msgdata/msg_wifi_lobby.h"
#include "msgdata/msg_wifi_gtc.h"
#include "msgdata/msg_wifi_system.h"

#include "application/email_main.h"
#include "savedata/config.h"
#include "system/pm_overlay.h"
#include "system/main.h"
#include "application/email.h"
#include "savedata/email_savedata.h"
#include "application/wifi_p2pmatch.h"
#include "field/sysflag.h"

// MatchComment: include this header
#include "application/codein/codein.h"

FS_EXTERN_OVERLAY( title );

//==============================================================================
//	定数定義
//==============================================================================
#define MYDWC_HEAPSIZE		0x20000

enum{
	SEQ_INIT_DPW,
	SEQ_INIT_DPW_WAIT,
	SEQ_INIT,
	SEQ_MAIN,
	SEQ_END,
};

//==============================================================================
//	グローバル変数
//==============================================================================
static NNSFndHeapHandle _wtHeapHandle;


//==============================================================================
//	プロトタイプ宣言
//==============================================================================
static void Email_CommInitialize(EMAIL_SYSWORK *esys);
static void Email_CommFree(EMAIL_SYSWORK *esys);
static void *AllocFunc( DWCAllocType name, u32   size, int align );
static void FreeFunc(DWCAllocType name, void* ptr,  u32 size);

static void * SubFuncCall_EmailMenu(EMAIL_SYSWORK *esys);
static void ReturnFuncCall_EmailMenu(EMAIL_SYSWORK *esys);
static void * SubFuncCall_AddressInput(EMAIL_SYSWORK *esys);
static void ReturnFuncCall_AddressInput(EMAIL_SYSWORK *esys);
static void * SubFuncCall_GSProfileIDGet(EMAIL_SYSWORK *esys);
static void ReturnFuncCall_GSProfileIDGet(EMAIL_SYSWORK *esys);

static void * ov98_2246FC4(EMAIL_SYSWORK *esys);
static void ov98_2246FFC(EMAIL_SYSWORK *esys);
static void * ov98_2247070(EMAIL_SYSWORK *esys);
static void ov98_22470B8(EMAIL_SYSWORK *esys);
static void * ov98_22470F8(EMAIL_SYSWORK *esys);
static void ov98_2247134(EMAIL_SYSWORK *esys);

//==============================================================================
//	PROCデータ
//==============================================================================
FS_EXTERN_OVERLAY(email);
FS_EXTERN_OVERLAY(worldtrade);

///Eメール設定画面プロセス定義データ
const PROC_DATA EmailProcData = {
	EmailProc_Init,
	EmailProc_Main,
	EmailProc_End,
	NO_OVERLAY_ID,//FS_OVERLAY_ID(email),
};


///Eメールメニュー画面プロセス定義データ
static const PROC_DATA EmailMenuProcData = {
	EmailMenu_Enter_Init,
	EmailMenu_Enter_Main,
	EmailMenu_Enter_End,
	NO_OVERLAY_ID,//FS_OVERLAY_ID(email),
};

///Eメール数値入力画面プロセス定義データ
static const PROC_DATA EmailInputProcData = {
	EmailInput_Init,
	EmailInput_Main,
	EmailInput_End,
	NO_OVERLAY_ID,//FS_OVERLAY_ID(email),
};

///Eメールアドレス入力画面プロセス定義データ　※check　後で消す
static const PROC_DATA EmailAddressProcData = {
	EmailAddress_Init,
	EmailAddress_Main,
	EmailAddress_End,
	NO_OVERLAY_ID,//FS_OVERLAY_ID(email),
};

FS_EXTERN_OVERLAY(wifi_p2pmatch);
// 友達コード取得プロセス定義データ
static const PROC_DATA WifiP2PMatchProcData = {
	WifiP2PMatchProc_Init,
	WifiP2PMatchProc_Main,
	WifiP2PMatchProc_End,
	FS_OVERLAY_ID(wifi_p2pmatch),
};


///サブPROC呼び出し時のパラメータ作成関数
typedef void * (*EMAIL_SUBPROC_PARAM_FUNC)(EMAIL_SYSWORK *esys);
///サブPROC終了時のパラメータ引渡し関数
typedef void (*EMAIL_SUBPROC_RETURN_FUNC)(EMAIL_SYSWORK *esys);

///PROCデータのポインタテーブル　※EMAIL_SUBPROC_???と並びを同じにしておくこと！
static const struct{
	EMAIL_SUBPROC_PARAM_FUNC param_func;
	EMAIL_SUBPROC_RETURN_FUNC return_func;
	const PROC_DATA *proc_data;
	int comm_free_call;			//TRUE＝通信ライブラリなどを解放した状態でPROCを作る
}EmailProcDataTbl[] = {
    // MatchComment: use matching plat US data
	{SubFuncCall_EmailMenu, ReturnFuncCall_EmailMenu, &EmailMenuProcData, FALSE},
	{ov98_2246FC4, ov98_2246FFC, &CodeInput_ProcData, FALSE},
	{ov98_2247070, ov98_22470B8, &CodeInput_ProcData, FALSE},
	{ov98_22470F8, ov98_2247134, &CodeInput_ProcData, FALSE},
	{ov98_22470F8, ov98_2247134, &CodeInput_ProcData, FALSE},
	{SubFuncCall_GSProfileIDGet, ReturnFuncCall_GSProfileIDGet, &WifiP2PMatchProcData, TRUE},
};


//============================================================================================
//	プロセス関数
//============================================================================================
//==============================================================================
/**
 * $brief   世界交換入り口画面初期化
 *
 * @param   esys		
 * @param   seq		
 *
 * @retval  int		
 */
//==============================================================================
PROC_RESULT EmailProc_Init( PROC * proc, int * seq )
{
	EMAIL_SYSWORK *esys;

	//Eメール管理用ヒープ作成
	sys_CreateHeap( HEAPID_BASE_APP, HEAPID_EMAIL_MANAGE, 0x28000 );
	//通信アイコンを出すのに必要
	sys_CreateHeap( HEAPID_BASE_SYSTEM, HEAPID_COMMICON, 0x300 );

	esys = PROC_AllocWork(proc, sizeof(EMAIL_SYSWORK), HEAPID_EMAIL_MANAGE );
	MI_CpuClear8(esys, sizeof(EMAIL_SYSWORK));
	esys->savedata = ((MAINWORK*)PROC_GetParentWork(proc))->savedata;
	esys->config = SaveData_GetConfig(esys->savedata);
	esys->email_address = STRBUF_Create(EMAIL_ADDRESS_LEN_SIZE, HEAPID_EMAIL_MANAGE);
	esys->now_email_address = STRBUF_Create(EMAIL_ADDRESS_LEN_SIZE, HEAPID_EMAIL_MANAGE);

	//サウンドデータセット
	Snd_DataSetByScene( SND_SCENE_EMAIL, SEQ_WIFILOBBY, 1 );
		
	//最初に実行するサブプロセス
	esys->sub_nextprocess = EMAIL_SUBPROC_MENU;
	
	return PROC_RES_FINISH;
}

//==============================================================================
/**
 * $brief   Eメール管理メイン
 *
 * @param   esys		
 * @param   seq		
 *
 * @retval  int		
 */
//==============================================================================
PROC_RESULT EmailProc_Main( PROC * proc, int * seq )
{
	EMAIL_SYSWORK * esys  = PROC_GetWork( proc );
	BOOL result;
	
	if(esys->comm_initialize_ok == TRUE){
		// 受信強度リンクを反映させる
		DWC_UpdateConnection();

		// Dpw_Tr_Main() だけは例外的にいつでも呼べる
		Dpw_Tr_Main();

		// 通信状態を確認してアイコンの表示を変える
		WirelessIconEasy_SetLevel( WM_LINK_LEVEL_3 - DWC_GetLinkLevel() );
	}

	switch(*seq){
	case SEQ_INIT_DPW:
		Email_CommInitialize(esys);
		*seq = SEQ_INIT_DPW_WAIT;
		break;
	case SEQ_INIT_DPW_WAIT:
		if(CommIsVRAMDInitialize()){
			_wtHeapHandle = esys->heapHandle;
	
			// wifiメモリ管理関数呼び出し
			DWC_SetMemFunc( AllocFunc, FreeFunc );
			
			esys->comm_initialize_ok = TRUE;
			*seq = SEQ_INIT;
		}
		break;
	case SEQ_INIT:
		esys->sub_proc = PROC_Create(EmailProcDataTbl[esys->sub_nextprocess].proc_data, 
			EmailProcDataTbl[esys->sub_nextprocess].param_func(esys), HEAPID_EMAIL_MANAGE);
		esys->now_process = esys->sub_nextprocess;
		esys->sub_nextprocess = EMAIL_SUBPROC_END;
		*seq = SEQ_MAIN;
		break;
	case SEQ_MAIN:
		if(ProcMain(esys->sub_proc) == TRUE){
			EmailProcDataTbl[esys->now_process].return_func(esys);
			PROC_Delete(esys->sub_proc);
			if(esys->sub_nextprocess == EMAIL_SUBPROC_END){
				*seq = SEQ_END;
			}
			else{
				if(EmailProcDataTbl[esys->sub_nextprocess].comm_free_call == TRUE){
					Email_CommFree(esys);
					*seq = SEQ_INIT;
				}
				else if(esys->comm_initialize_ok == TRUE){
					*seq = SEQ_INIT;
				}
				else{
					*seq = SEQ_INIT_DPW;
				}
			}
		}
		break;
	case SEQ_END:
		return PROC_RES_FINISH;
	}
	return PROC_RES_CONTINUE;
}


//==============================================================================
/**
 * $brief   世界交換入り口画面終了
 *
 * @param   esys		
 * @param   seq		
 *
 * @retval  int		
 */
//==============================================================================
PROC_RESULT EmailProc_End(PROC *proc, int *seq)
{
	EMAIL_SYSWORK * esys  = PROC_GetWork( proc );

	Email_CommFree(esys);
	
	STRBUF_Delete(esys->now_email_address);
	STRBUF_Delete(esys->email_address);

	PROC_FreeWork( proc );				// PROCワーク開放

	sys_DeleteHeap( HEAPID_EMAIL_MANAGE );
	sys_DeleteHeap( HEAPID_COMMICON );

	//-- タイトル画面へ --//
	Main_SetNextProc( FS_OVERLAY_ID(title), &TitleProcData);

	return PROC_RES_FINISH;
}


//--------------------------------------------------------------
/**
 * @brief   通信ライブラリ関連の初期化処理
 *
 * @param   esys		
 */
//--------------------------------------------------------------
static void Email_CommInitialize(EMAIL_SYSWORK *esys)
{
	if(esys->comm_initialize_ok == FALSE){
		OS_TPrintf("Comm初期化開始\n");
		
		//世界交換のWifi通信命令を使用するためオーバーレイを読み出す(dpw_tr.c等)
		Overlay_Load(FS_OVERLAY_ID(worldtrade), OVERLAY_LOAD_NOT_SYNCHRONIZE);

		// DWCライブラリ（Wifi）に渡すためのワーク領域を確保
		esys->heapPtr    = sys_AllocMemory(HEAPID_EMAIL_MANAGE, MYDWC_HEAPSIZE + 32);
		esys->heapHandle = NNS_FndCreateExpHeap( (void *)( ((u32)esys->heapPtr + 31) / 32 * 32 ), MYDWC_HEAPSIZE);

		//DWCオーバーレイ読み込み
		DwcOverlayStart();
		DpwCommonOverlayStart();
		// イクニューモン転送
		CommVRAMDInitialize();
		sys_SleepOK(SLEEPTYPE_COMM);  // スリープ禁止を解除。実際に通信する時にこちら側でNGにする

		OS_TPrintf("Comm初期化終了\n");
	}
}

//--------------------------------------------------------------
/**
 * @brief   通信ライブラリ関連の解放処理
 *
 * @param   esys		
 */
//--------------------------------------------------------------
static void Email_CommFree(EMAIL_SYSWORK *esys)
{
	if(esys->comm_initialize_ok == TRUE){
		OS_TPrintf("Comm解放開始\n");
		
		NNS_FndDestroyExpHeap(esys->heapHandle);
		sys_FreeMemoryEz( esys->heapPtr );
		DpwCommonOverlayEnd();
		DwcOverlayEnd();

		// イクニューモン解放
		CommVRAMDFinalize();

		Overlay_UnloadID(FS_OVERLAY_ID(worldtrade));
		
		esys->comm_initialize_ok = FALSE;

		OS_TPrintf("Comm解放完了\n");
	}
}


//==============================================================================
/**
 * @brief   サブプロセスモードの切り替え指定
 *
 * @param   esys			
 * @param   subprccess	
 * @param   mode		
 *
 * @retval  none		
 */
//==============================================================================
void Email_SubProcessChange( EMAIL_SYSWORK *esys, int subprccess, int mode )
{
	esys->sub_nextprocess  = subprccess;
	esys->sub_process_mode = mode;
}

//--------------------------------------------------------------
/**
 * @brief   サブプロセスモードを強制で終了モードにする
 *
 * @param   esys		
 *
 * 通信エラーなどでの強制終了用
 */
//--------------------------------------------------------------
void Email_SubProcessEndSet(EMAIL_SYSWORK *esys)
{
	esys->sub_nextprocess = EMAIL_SUBPROC_END;
}

//--------------------------------------------------------------
/**
 * @brief   メニュー画面を起動するときのモードをセット
 *
 * @param   esys				
 * @param   mode				起動モード
 */
//--------------------------------------------------------------
void Email_RecoveryMenuModeSet(EMAIL_SYSWORK *esys, int mode)
{
	esys->sub_menu_recovery_mode = mode;
}

//--------------------------------------------------------------
/**
 * @brief   メニュー画面を起動するときのモード取得
 *
 * @param   esys				
 * @param   mode				起動モード
 */
//--------------------------------------------------------------
int Email_RecoveryMenuModeGet(EMAIL_SYSWORK *esys)
{
	return esys->sub_menu_recovery_mode;
}


/*---------------------------------------------------------------------------*
  メモリ確保関数
 *---------------------------------------------------------------------------*/
static void *AllocFunc( DWCAllocType name, u32   size, int align )
{
#pragma unused( name )
    void * ptr;
    OSIntrMode old;
    old = OS_DisableInterrupts();
    ptr = NNS_FndAllocFromExpHeapEx( _wtHeapHandle, size, align );
    OS_RestoreInterrupts( old );
    if(ptr == NULL){
	}
	
    return ptr;
}

/*---------------------------------------------------------------------------*
  メモリ開放関数
 *---------------------------------------------------------------------------*/
static void FreeFunc(DWCAllocType name, void* ptr,  u32 size)
{
#pragma unused( name, size )
    OSIntrMode old;

    if ( !ptr ) return;
    old = OS_DisableInterrupts();
    NNS_FndFreeToExpHeap( _wtHeapHandle, ptr );
    OS_RestoreInterrupts( old );
}


//--------------------------------------------------------------
/**
 * @brief   EMAIL_SYSWORKに持っているEメールアドレスをセーブデータの方にセットする
 *
 * @param   esys		
 */
//--------------------------------------------------------------
void Email_AddressSaveWorkSet(EMAIL_SYSWORK *esys)
{
	char *address_char;
	
	address_char = sys_AllocMemory(HEAPID_EMAIL_MANAGE, sizeof(char) * EMAIL_ADDRESS_LEN_SIZE);
	
	Email_Strcode_to_Ascii(esys->email_address, address_char, HEAPID_EMAIL_MANAGE);
	EMAILSAVE_AddressSet(esys->savedata, address_char);
	
	sys_FreeMemoryEz(address_char);
}

//--------------------------------------------------------------
/**
 * @brief   EMAIL_SYSWORKに持っている認証コード(下4桁)をセーブデータの方にセットする
 *
 * @param   esys		
 */
//--------------------------------------------------------------
void Email_AuthenticateCodeSaveWorkSet(EMAIL_SYSWORK *esys)
{
	EMAILSAVE_ParamSet(esys->savedata, EMAIL_PARAM_AUTH_CODE_HIGH, esys->authenticate_rand_code);
	EMAILSAVE_ParamSet(esys->savedata, EMAIL_PARAM_AUTH_CODE_LOW, esys->ret_authenticate_code);
}

//--------------------------------------------------------------
/**
 * @brief   EMAIL_SYSWORKに持っているパスワードをセーブデータの方にセットする
 *
 * @param   esys		
 */
//--------------------------------------------------------------
void Email_PasswordSaveWorkSet(EMAIL_SYSWORK *esys)
{
	EMAILSAVE_ParamSet(esys->savedata, EMAIL_PARAM_PASSWORD, esys->ret_password);
}

//--------------------------------------------------------------
/**
 * @brief   認証用プロフィール作成
 *
 * @param   esys			
 */
//--------------------------------------------------------------
void Email_DCProfileCreate(EMAIL_SYSWORK *esys)
{
	u32 auth_veri;
	
	auth_veri = EMAILSAVE_DCProfileCreate_AuthStart(esys->savedata, &esys->dc_profile);
	
	Email_AuthenticateRandCodeSet(esys, auth_veri);
}

//--------------------------------------------------------------
/**
 * @brief   Email_DCProfileCreateで作成済みのプロフィールデータに対してEMAIL_SYSWORKが
 * 			所持しているEメールアドレスをセットする
 *
 * @param   esys		
 */
//--------------------------------------------------------------
void Email_DCProfileSet_Address(EMAIL_SYSWORK *esys)
{
	Email_Strcode_to_Ascii(esys->email_address, esys->dc_profile.mailAddr, HEAPID_EMAIL_MANAGE);
}

//--------------------------------------------------------------
/**
 * @brief   Email_DCProfileCreateで作成済みのプロフィールデータに対してEMAIL_SYSWORKが
 * 			所持している認証コード(下4桁)をセットする
 *
 * @param   esys		
 */
//--------------------------------------------------------------
void Email_DCProfileSet_Authenticate(EMAIL_SYSWORK *esys)
{
	esys->dc_profile.mailAddrAuthPass = esys->ret_authenticate_code;
}

//--------------------------------------------------------------
/**
 * @brief   クライアント側で決定される認証コード上3桁をセットする
 *
 * @param   esys		
 * @param   code		認証コード上3桁
 */
//--------------------------------------------------------------
void Email_AuthenticateRandCodeSet(EMAIL_SYSWORK *esys, u32 code)
{
	esys->authenticate_rand_code = code;
}

//--------------------------------------------------------------
/**
 * @brief   クライアント側で決定される認証コード上3桁を取得する
 *
 * @retval		認証コード上3桁
 */
//--------------------------------------------------------------
u32 Email_AuthenticateRandCodeGet(EMAIL_SYSWORK *esys)
{
	return esys->authenticate_rand_code;
}

//--------------------------------------------------------------
/**
 * @brief   入力画面から認証コードをセットする
 *
 * @param   esys		
 * @param   code		認証コード(下4桁)。キャンセル終了の場合はEMAIL_AUTHENTICATE_CODE_CANCEL
 */
//--------------------------------------------------------------
void Email_AuthenticateCodeSet(EMAIL_SYSWORK *esys, u32 code)
{
	esys->ret_authenticate_code = code;
}

//--------------------------------------------------------------
/**
 * @brief   入力画面で入力された認証コードを取得する
 *
 * @param   esys		
 * 
 * @retval   code		認証コード(下4桁)。キャンセル終了の場合はEMAIL_AUTHENTICATE_CODE_CANCEL
 */
//--------------------------------------------------------------
u32 Email_AuthenticateCodeGet(EMAIL_SYSWORK *esys)
{
	return esys->ret_authenticate_code;
}

//--------------------------------------------------------------
/**
 * @brief   入力画面からパスワードをセットする
 *
 * @param   esys		
 * @param   code		認証コード(下4桁)。キャンセル終了の場合はEMAIL_PASSWORD_CANCEL
 */
//--------------------------------------------------------------
void Email_PasswordNumberSet(EMAIL_SYSWORK *esys, u32 password)
{
	esys->ret_password = password;
}

//--------------------------------------------------------------
/**
 * @brief   入力画面で入力されたパスワードを取得する
 *
 * @param   esys		
 * 
 * @retval   code		認証コード(下4桁)。キャンセル終了の場合はEMAIL_PASSWORD_CANCEL
 */
//--------------------------------------------------------------
u32 Email_PasswordNumberGet(EMAIL_SYSWORK *esys)
{
	return esys->ret_password;
}

//--------------------------------------------------------------
/**
 * @brief   Eメールアドレス入力画面終了フラグをセットする
 *
 * @param   esys		
 * @param   flag		EMAIL_ADDRESS_RET_SET or EMAIL_ADDRESS_RET_CANCEL
 */
//--------------------------------------------------------------
void Email_AddressReturnFlagSet(EMAIL_SYSWORK *esys, int flag)
{
	esys->ret_address_flag = flag;
}

//--------------------------------------------------------------
/**
 * @brief   Eメールアドレス入力画面終了フラグを取得する
 *
 * @param   esys		
 * 
 * @retval	EMAIL_ADDRESS_RET_SET or EMAIL_ADDRESS_RET_CANCEL
 */
//--------------------------------------------------------------
int Email_AddressReturnFlagGet(EMAIL_SYSWORK *esys)
{
	return esys->ret_address_flag;
}

//--------------------------------------------------------------
/**
 * @brief   Eメールアドレスをセットする(セットされている)STRBUFのポインタを取得する
 *
 * @param   esys		
 */
//--------------------------------------------------------------
STRBUF * Email_AddressStrbufGet(EMAIL_SYSWORK *esys)
{
	return esys->email_address;
}


//==============================================================================
//	サブPROC呼び出し時のパラメータ作成関数
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   メニュー画面呼び出しパラメータ作成関数
 *
 * @param   esys		
 *
 * @retval  SubProcにParentWorkとして渡すワークのポインタ
 */
//--------------------------------------------------------------
static void * SubFuncCall_EmailMenu(EMAIL_SYSWORK *esys)
{
	return esys;
}

//--------------------------------------------------------------
/**
 * @brief   メニュー画面から戻ってきた時のパラメータ引渡し関数
 *
 * @param   esys		
 */
//--------------------------------------------------------------
static void ReturnFuncCall_EmailMenu(EMAIL_SYSWORK *esys)
{
	;
}

// NONMATCHING
asm void * ov98_2246FC4(EMAIL_SYSWORK *esys)
{
	push {r4, lr}
	sub sp, #0x18
	add r4, r0, #0
	mov r0, #4
	str r0, [sp, #8]
	str r0, [sp, #0xc]
	str r0, [sp, #0x10]
	str r0, [sp, #0x14]
	ldr r0, [r4, #4]
	bl SaveData_GetConfig
	add r3, r0, #0
	mov r0, #4
	str r0, [sp]
	mov r0, #0
	str r0, [sp, #4]
	mov r0, #0x6c
	mov r1, #0x10
	add r2, sp, #8
	bl sub_2089400
	add r1, r4, #0
	add r1, #0x94
	add r4, #0x94
	str r0, [r1, #0]
	ldr r0, [r4, #0]
	add sp, #0x18
	pop {r4, pc}
}

// NONMATCHING
asm void ov98_2246FFC(EMAIL_SYSWORK *esys)
{
	push {r3, r4, r5, lr}
	add r5, r0, #0
	add r0, #0x94
	ldr r4, [r0, #0]
	mov r1, #0x6c
	ldr r0, [r4, #0x1c]
	bl ov98_2249A80
	cmp r0, #0
	beq _0224701A
	add r0, r5, #0
	mov r1, #1
	bl Email_AddressReturnFlagSet
	b _0224705C
_0224701A:
	add r0, r5, #0
	bl Email_AddressReturnFlagGet
	cmp r0, #2
	bne _02247048
	mov r0, #0x11
	lsl r0, r0, #4
	ldr r0, [r5, r0]
	ldr r1, [r4, #0x1c]
	bl STRBUF_Compare
	cmp r0, #0
	beq _0224703E
	add r0, r5, #0
	mov r1, #3
	bl Email_AddressReturnFlagSet
	b _0224705C
_0224703E:
	add r0, r5, #0
	mov r1, #0
	bl Email_AddressReturnFlagSet
	b _0224705C
_02247048:
	mov r0, #0x11
	lsl r0, r0, #4
	ldr r0, [r5, r0]
	ldr r1, [r4, #0x1c]
	bl STRBUF_Copy
	add r0, r5, #0
	mov r1, #2
	bl Email_AddressReturnFlagSet
_0224705C:
	add r0, r4, #0
	bl CodeInput_ParamDelete
	mov r1, #0
	add r0, r5, #0
	add r2, r1, #0
	bl Email_SubProcessChange
	pop {r3, r4, r5, pc}
	// .align 2, 0
}

// NONMATCHING
asm void * ov98_2247070(EMAIL_SYSWORK *esys)
{
	push {r4, lr}
	sub sp, #0x20
	add r4, r0, #0
	mov r0, #3
	str r0, [sp, #0x10]
	mov r0, #4
	str r0, [sp, #0x14]
	mov r0, #0
	str r0, [sp, #0x18]
	str r0, [sp, #0x1c]
	ldr r0, [r4, #4]
	bl SaveData_GetConfig
	add r3, r0, #0
	mov r0, #5
	str r0, [sp]
	mov r0, #1
	str r0, [sp, #4]
	str r0, [sp, #8]
	add r0, r4, #0
	add r0, #0x78
	ldrh r0, [r0]
	mov r1, #7
	add r2, sp, #0x10
	str r0, [sp, #0xc]
	mov r0, #0x6c
	bl sub_208941C
	add r1, r4, #0
	add r1, #0x94
	add r4, #0x94
	str r0, [r1, #0]
	ldr r0, [r4, #0]
	add sp, #0x20
	pop {r4, pc}
	// .align 2, 0
}

extern void _ull_mod(void);
// NONMATCHING

asm void ov98_22470B8(EMAIL_SYSWORK *esys)
{
	push {r3, r4, r5, lr}
	add r5, r0, #0
	add r0, #0x94
	ldr r4, [r0, #0]
	add r1, sp, #0
	ldr r0, [r4, #0x1c]
	bl STRBUF_GetNumber
	ldr r2, =0x00002710 // _022470F4
	mov r3, #0
	bl _ull_mod
	mov r1, #0x42
	lsl r1, r1, #2
	str r0, [r5, r1]
	ldr r0, [sp]
	cmp r0, #0
	bne _022470E0
	bl GF_AssertFailedWarningCall
_022470E0:
	add r0, r4, #0
	bl CodeInput_ParamDelete
	mov r1, #0
	add r0, r5, #0
	add r2, r1, #0
	bl Email_SubProcessChange
	pop {r3, r4, r5, pc}
	nop
// _022470F4: .4byte 0x00002710
}

// NONMATCHING
asm void * ov98_22470F8(EMAIL_SYSWORK *esys)
{
	push {r4, lr}
	sub sp, #0x18
	add r4, r0, #0
	mov r0, #4
	str r0, [sp, #8]
	mov r0, #0
	str r0, [sp, #0xc]
	str r0, [sp, #0x10]
	str r0, [sp, #0x14]
	ldr r0, [r4, #4]
	bl SaveData_GetConfig
	add r3, r0, #0
	mov r0, #6
	str r0, [sp]
	mov r0, #0
	str r0, [sp, #4]
	mov r0, #0x6c
	mov r1, #4
	add r2, sp, #8
	bl sub_2089400
	add r1, r4, #0
	add r1, #0x94
	add r4, #0x94
	str r0, [r1, #0]
	ldr r0, [r4, #0]
	add sp, #0x18
	pop {r4, pc}
	// .align 2, 0
}

// NONMATCHING
asm void ov98_2247134(EMAIL_SYSWORK *esys)
{
	push {r3, r4, r5, lr}
	add r5, r0, #0
	add r0, #0x94
	ldr r4, [r0, #0]
	add r1, sp, #0
	ldr r0, [r4, #0x1c]
	bl STRBUF_GetNumber
	mov r1, #0x41
	lsl r1, r1, #2
	str r0, [r5, r1]
	ldr r0, [sp]
	cmp r0, #0
	bne _02247154
	bl GF_AssertFailedWarningCall
_02247154:
	add r0, r4, #0
	bl CodeInput_ParamDelete
	mov r1, #0
	add r0, r5, #0
	add r2, r1, #0
	bl Email_SubProcessChange
	pop {r3, r4, r5, pc}
	// .align 2, 0
}

//--------------------------------------------------------------
/**
 * @brief   アドレス入力画面呼び出しパラメータ作成関数
 *
 * @param   esys		
 *
 * @retval  SubProcにParentWorkとして渡すワークのポインタ
 */
//--------------------------------------------------------------
static void * SubFuncCall_AddressInput(EMAIL_SYSWORK *esys)
{
	EMAIL_PARAM *para;
	
	para = sys_AllocMemory(HEAPID_EMAIL_MANAGE, sizeof(EMAIL_PARAM));
	MI_CpuClear8(para, sizeof(EMAIL_PARAM));
	
	para->mode = esys->sub_process_mode;	//EMAIL_MODE_INPUT_???
	
	Email_Ascii_to_Strcode(
		EMAILSAVE_AddressGet(esys->savedata), esys->now_email_address, HEAPID_EMAIL_MANAGE);
	para->now_str = esys->now_email_address;
	para->str = esys->email_address;
	
	para->authenticate_rand_code = esys->authenticate_rand_code;
	para->password = EMAILSAVE_ParamGet(esys->savedata, EMAIL_PARAM_PASSWORD);

#if 0	//他の画面呼び出し時に値の初期化をしてしまうのでキャンセル値セットするのやめる
	para->ret_address_flag = EMAIL_ADDRESS_RET_CANCEL;
	para->ret_authenticate_code = EMAIL_AUTHENTICATE_CODE_CANCEL;
	para->ret_password = 0;
#else
	para->ret_address_flag = esys->ret_address_flag;
	para->ret_authenticate_code = esys->ret_authenticate_code;;
	para->ret_password = esys->ret_password;
#endif

	esys->sub_proc_parent_work = para;
	return esys->sub_proc_parent_work;
}

//--------------------------------------------------------------
/**
 * @brief   アドレス入力画面呼び出しパラメータ作成関数
 *
 * @param   esys		
 *
 * @retval  SubProcにParentWorkとして渡すワークのポインタ
 */
//--------------------------------------------------------------
static void ReturnFuncCall_AddressInput(EMAIL_SYSWORK *esys)
{
	EMAIL_PARAM *para;
	
	para = esys->sub_proc_parent_work;
	
	esys->ret_address_flag = para->ret_address_flag;
	esys->ret_password = para->ret_password;
	esys->ret_authenticate_code = para->ret_authenticate_code;
	
	sys_FreeMemoryEz(esys->sub_proc_parent_work);

	//サブPROCをリストに戻すためにセット
	Email_SubProcessChange(esys, EMAIL_SUBPROC_MENU, 0 );
}

//--------------------------------------------------------------
/**
 * @brief   アドレス入力画面呼び出しパラメータ作成関数
 *
 * @param   esys		
 *
 * @retval  SubProcにParentWorkとして渡すワークのポインタ
 */
//--------------------------------------------------------------
static void * SubFuncCall_GSProfileIDGet(EMAIL_SYSWORK *esys)
{
	WIFIP2PMATCH_PROC_PARAM* para;
	
	para = sys_AllocMemory(HEAPID_EMAIL_MANAGE, sizeof(WIFIP2PMATCH_PROC_PARAM));
	MI_CpuClear8(para, sizeof(WIFIP2PMATCH_PROC_PARAM));
	
	para->pSaveData = esys->savedata;
	para->seq = WIFI_P2PMATCH_DPW;

	esys->comm_initialize_ok = FALSE;
	esys->sub_proc_parent_work = para;
	return esys->sub_proc_parent_work;
}

//--------------------------------------------------------------
/**
 * @brief   アドレス入力画面呼び出しパラメータ作成関数
 *
 * @param   esys		
 *
 * @retval  SubProcにParentWorkとして渡すワークのポインタ
 */
//--------------------------------------------------------------
static void ReturnFuncCall_GSProfileIDGet(EMAIL_SYSWORK *esys)
{
	WIFIP2PMATCH_PROC_PARAM *para;
	
	para = esys->sub_proc_parent_work;

    if( mydwc_checkMyGSID(esys->savedata) ){  // コード取得に成功
        SysFlag_WifiUseSet(SaveData_GetEventWork(esys->savedata));
        OS_TPrintf("友達コード取得成功\n");
    }
	
	sys_FreeMemoryEz(esys->sub_proc_parent_work);

	//サブPROCをリストに戻すためにセット
	Email_SubProcessChange(esys, EMAIL_SUBPROC_MENU, 0 );
}

