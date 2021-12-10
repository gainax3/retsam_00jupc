//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wf2dmap_cmdq.c
 *	@brief		コマンドキュー
 *	@author		tomoya takahashi
 *	@data		2007.03.28
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "common.h"

#define __WF2DMAP_CMDQ_H_GLOBAL
#include "application/wifi_2dmap/wf2dmap_cmdq.h"

//-----------------------------------------------------------------------------
/**
 *					コーディング規約
 *		●関数名
 *				１文字目は大文字それ以降は小文字にする
 *		●変数名
 *				・変数共通
 *						constには c_ を付ける
 *						staticには s_ を付ける
 *						ポインタには p_ を付ける
 *						全て合わさると csp_ となる
 *				・グローバル変数
 *						１文字目は大文字
 *				・関数内変数
 *						小文字と”＿”と数字を使用する 関数の引数もこれと同じ
*/
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
/**
 *		アクションコマンドバッファ
 *		（あったら便利なので作成しました）
 */
//-----------------------------------------------------------------------------
//-------------------------------------
///	アクションコマンドバッファ
//=====================================
typedef struct _WF2DMAP_ACTCMDQ {
	WF2DMAP_ACTCMD* p_buff;
	u32 num;
	u16 top;		// 先頭
	u16	tail;		// 末尾
}WF2DMAP_ACTCMDQ;



//----------------------------------------------------------------------------
/**
 *	@brief	アクションコマンドバッファワーク作成
 *
 *	@param	buffnum		コマンド配列数
 *	@param	heapID		ヒープID
 *
 *	@return	アクションコマンドバッファワーク
 */
//-----------------------------------------------------------------------------
WF2DMAP_ACTCMDQ* WF2DMAP_ACTCMDQSysInit( u32 buffnum, u32 heapID )
{
	WF2DMAP_ACTCMDQ* p_sys;

	p_sys = sys_AllocMemory( heapID, sizeof(WF2DMAP_ACTCMDQ) );
	GF_ASSERT( p_sys );

	p_sys->num = buffnum + 1;	// キューがいっぱいになったことチェック用
	
	p_sys->p_buff = sys_AllocMemory( heapID, sizeof(WF2DMAP_ACTCMD)*p_sys->num );
	memset( p_sys->p_buff, 0, sizeof(WF2DMAP_ACTCMD)*p_sys->num );

	p_sys->top = 0;
	p_sys->tail = 0;

	return p_sys;
}

//----------------------------------------------------------------------------
/**
 *	@brief	バッファワークのクリア
 *
 *	@param	p_sys		アクションコマンドキューワーク
 */
//-----------------------------------------------------------------------------
void WF2DMAP_ACTCMDQSysExit( WF2DMAP_ACTCMDQ* p_sys )
{
	sys_FreeMemoryEz( p_sys->p_buff );
	sys_FreeMemoryEz( p_sys );
}

//----------------------------------------------------------------------------
/**
 *	@brief	バッファ数の取得
 *
 *	@param	cp_sys		アクションコマンドキューワーク
 *
 *	@return	バッファ数
 */
//-----------------------------------------------------------------------------
u32 WF2DMAP_ACTCMDQSysBuffNumGet( const WF2DMAP_ACTCMDQ* cp_sys )
{
	return cp_sys->num - 1;	// キューがいっぱいになったことチェックするためnum+1しているので-1する
}

//----------------------------------------------------------------------------
/**
 *	@brief	アクションコマンドキュー　コマンド設定
 *
 *	@param	p_sys		アクションコマンドキューワーク
 *	@param	cp_buff		設定コマンド
 */
//-----------------------------------------------------------------------------
void WF2DMAP_ACTCMDQSysCmdPush( WF2DMAP_ACTCMDQ* p_sys, const WF2DMAP_ACTCMD* cp_buff )
{
	// 末尾+1が先頭の時満杯
	if( ((p_sys->tail + 1)%p_sys->num) == p_sys->top ){
		WF2DMAP_ACTCMD tmp;	// 捨てるコマンド
		// 満杯
		// 1つデータをポップしてデータ格納
		WF2DMAP_ACTCMDQSysCmdPop( p_sys, &tmp );
	}

	// データ設定
	p_sys->p_buff[ p_sys->tail ] = *cp_buff;

	// 末尾位置を動かす
	p_sys->tail = (p_sys->tail + 1)%p_sys->num;
}

//----------------------------------------------------------------------------
/**
 *	@brief	アクションコマンドキュー　コマンド取得
 *
 *	@param	p_sys		アクションコマンドキューワーク
 *	@param	p_cmd		コマンド格納先
 *
 *	@retval	TRUE	取得成功
 *	@retval	FALSE	取得失敗
 */
//-----------------------------------------------------------------------------
BOOL WF2DMAP_ACTCMDQSysCmdPop( WF2DMAP_ACTCMDQ* p_sys, WF2DMAP_ACTCMD* p_cmd )
{
	// 先頭=末尾	データがない
	if( p_sys->tail == p_sys->top ){
		return FALSE;
	}

	*p_cmd = p_sys->p_buff[ p_sys->top ];

	// 先頭位置を動かす
	p_sys->top = (p_sys->top + 1)%p_sys->num;

	return TRUE;
}





//-----------------------------------------------------------------------------
/**
 *		リクエストコマンドキュー
 *		（あったら便利なので作成しました）
 */
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	リクエストコマンドキュー
//=====================================
typedef struct _WF2DMAP_REQCMDQ{
	WF2DMAP_REQCMD* p_buff;
	u32 num;
	u16 top;		// 先頭
	u16	tail;		// 末尾
}WF2DMAP_REQCMDQ;

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------


//----------------------------------------------------------------------------
/**
 *	@brief	リクエストコマンドバッファワーク作成
 *
 *	@param	buffnum		コマンド配列数
 *	@param	heapID		ヒープID
 *
 *	@return	リクエストコマンドバッファワーク
 */
//-----------------------------------------------------------------------------
WF2DMAP_REQCMDQ* WF2DMAP_REQCMDQSysInit( u32 buffnum, u32 heapID )
{
	WF2DMAP_REQCMDQ* p_sys;

	p_sys = sys_AllocMemory( heapID, sizeof(WF2DMAP_REQCMDQ) );
	GF_ASSERT( p_sys );

	p_sys->num = buffnum + 1;	// キューがいっぱいになったことチェック用
	
	p_sys->p_buff = sys_AllocMemory( heapID, sizeof(WF2DMAP_REQCMD)*p_sys->num );
	memset( p_sys->p_buff, 0, sizeof(WF2DMAP_REQCMD)*p_sys->num );

	p_sys->top = 0;
	p_sys->tail = 0;

	return p_sys;
}

//----------------------------------------------------------------------------
/**
 *	@brief	リクエストコマンド　キュー　破棄
 *
 *	@param	p_sys	リクエストコマンド　キューワーク
 */
//-----------------------------------------------------------------------------
void WF2DMAP_REQCMDQSysExit( WF2DMAP_REQCMDQ* p_sys )
{
	sys_FreeMemoryEz( p_sys->p_buff );
	sys_FreeMemoryEz( p_sys );
}

//----------------------------------------------------------------------------
/**
 *	@brief	リクエストコマンド　プッシュ
 *
 *	@param	p_sys		リクエストコマンド　キューワーク
 *	@param	cp_cmd		設定コマンドデータ
 */
//-----------------------------------------------------------------------------
void WF2DMAP_REQCMDQSysCmdPush( WF2DMAP_REQCMDQ* p_sys, const WF2DMAP_REQCMD* cp_cmd )
{
	// 末尾+1が先頭の時満杯
	if( ((p_sys->tail + 1)%p_sys->num) == p_sys->top ){
		WF2DMAP_REQCMD tmp;	// 捨てるコマンド
		// 満杯
		// 1つデータをポップしてデータ格納
		WF2DMAP_REQCMDQSysCmdPop( p_sys, &tmp );
	}

	// データ設定
	p_sys->p_buff[ p_sys->tail ] = *cp_cmd;

	// 末尾位置を動かす
	p_sys->tail = (p_sys->tail + 1)%p_sys->num;
}

//----------------------------------------------------------------------------
/**
 *	@brief	リクエストコマンドキュー　コマンド取得
 *
 *	@param	p_sys		リクエストコマンドキューワーク
 *	@param	p_cmd		コマンド格納先
 *
 *	@retval	TRUE	取得成功
 *	@retval	FALSE	取得失敗
 */
//-----------------------------------------------------------------------------
BOOL WF2DMAP_REQCMDQSysCmdPop( WF2DMAP_REQCMDQ* p_sys, WF2DMAP_REQCMD* p_cmd )
{
	// 先頭=末尾	データがない
	if( p_sys->tail == p_sys->top ){
		return FALSE;
	}

	*p_cmd = p_sys->p_buff[ p_sys->top ];

	// 先頭位置を動かす
	p_sys->top = (p_sys->top + 1)%p_sys->num;

	return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	バッファ数の取得
 *
 *	@param	cp_sys		リクエストコマンドキューワーク
 *
 *	@return	バッファ数
 */
//-----------------------------------------------------------------------------
u32 WF2DMAP_REQCMDQSysBuffNumGet( const WF2DMAP_REQCMDQ* cp_sys )
{
	return cp_sys->num - 1;	// キューがいっぱいになったことチェックするためnum+1しているので-1する
}
