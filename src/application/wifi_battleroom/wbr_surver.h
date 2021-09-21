//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wbr_surver.h
 *	@brief		サーバーシステム
 *	@author		tomoya takahashi
 *	@data		2007.02.21
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WBR_SURVER_H__
#define __WBR_SURVER_H__

#undef GLOBAL
#ifdef	__WBR_SURVER_H_GLOBAL
#define	GLOBAL	/* */
#else
#define	GLOBAL	extern
#endif

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	シーケンス
//=====================================
enum {
	WBR_SURVER_SEQ_START_SEND,
	WBR_SURVER_SEQ_GETCOMMON_DATA,
	WBR_SURVER_SEQ_MAIN_START,
	WBR_SURVER_SEQ_MAIN,
	WBR_SURVER_SEQ_END_SEND,
	WBR_SURVER_SEQ_END_WAIT,
	WBR_SURVER_SEQ_NUM,
} ;

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	サーバーワーク
//=====================================
typedef struct _WBR_SURVER WBR_SURVER;


//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
GLOBAL WBR_SURVER* WBR_SurverInit( u32 heap );
GLOBAL void WBR_SurverExit( WBR_SURVER* p_sys );

// 管理して必要ならコマンドを送信
GLOBAL void WBR_SurverMain( WBR_SURVER* p_sys );

// データ設定関数
GLOBAL void WBR_SurverSeqSet( WBR_SURVER* p_sys, u32 seq );
GLOBAL u32 WBR_SurverSeqGet( const WBR_SURVER* cp_sys );
GLOBAL void WBR_SurverKoGameDataSet( WBR_SURVER* p_sys, const WBR_KO_COMM_GAME* cp_ko, u32 id );
GLOBAL void WBR_SurverKoTalkDataSet( WBR_SURVER* p_sys, const WBR_KO_COMM_TALK* cp_ko, u32 id );
GLOBAL void WBR_SurverKoTalkReqSet( WBR_SURVER* p_sys, const WBR_TALK_REQ* cp_ko, u32 id );

GLOBAL BOOL WBR_SurverKoCommDataRecv( WBR_SURVER* p_sys, u32 id );

// 今の状態を子に強制転送
GLOBAL void WBR_SurverDataSend( WBR_SURVER* p_sys );


#undef	GLOBAL
#endif		// __WBR_SURVER_H__

