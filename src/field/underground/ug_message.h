//=============================================================================
/**
 * @file	comm_message.h
 * @brief	通信用 メッセージを簡単に扱うためのクラス
 * @author	k.ohno
 * @date    2006.02.05
 */
//=============================================================================

#ifndef __UG_MESSAGE_H__
#define __UG_MESSAGE_H__

#include "savedata/mystatus.h"
#include "gflib/msg_print.h"	// STRCODE 定義のため
#include "system/msgdata.h" //MSGDATA_MANAGER
#include "gflib/bg_system.h" //GF_BGL_INI
#include "gflib/strbuf.h" //STRBUF
#include "system/bmp_list.h" //BMPLISTHEADER

//--------------------------------------------------------------
//   typedef
//--------------------------------------------------------------

typedef struct _COMM_MESSAGE COMM_MESSAGE;

typedef void (*PTRCommMsgCallback)(int num);


#define COMM_MESFRAME_PAL     ( 10 )         //  メッセージウインドウ
#define COMM_MENUFRAME_PAL    ( 11 )         //  メニューウインドウ
#define COMM_MESFONT_PAL      ( 12 )         //  メッセージフォント
#define COMM_SYSFONT_PAL	  ( 13 )         //  システムフォント

// 会話ウィンドウ
#define	COMM_TALK_WIN_CGX_SIZE	( 18+12 )
#define	COMM_TALK_WIN_CGX_NUM	( 1024 - COMM_TALK_WIN_CGX_SIZE)
#define	COMM_MSG_WIN_PX		( 2 )
#define	COMM_MSG_WIN_PY		( 19 )
#define	COMM_MSG_WIN_SX		( 27 )
#define	COMM_MSG_WIN_SY		( 4 )
#define	COMM_MSG_WIN_PAL		( COMM_MESFONT_PAL )
#define	COMM_MSG_WIN_CGX		( (COMM_TALK_WIN_CGX_NUM - 73) - ( COMM_MSG_WIN_SX * COMM_MSG_WIN_SY ) )

// ----------------------------------------------------------------------------
// localize_spec_mark(LANG_ALL) imatake 2006/11/29
// WORDSET の代入先インデックスの定義を ug_message.c から移動
// # アンダーバーで始まる定数をヘッダに置くのはちょっと気分が悪いけど……
#define _TALK_TARGET_INDEX_WORK (0)  ///< 会話データ　話相手のIDの位置
#define _TALK_MY_INDEX_WORK     (1)  ///< 会話データ　話しているIDの位置
#define _UGITEM_INDEX_WORK      (2)   ///< アイテム
#define _UGTRAP_INDEX_WORK      (3)   ///< trap
#define _SECRET_QUESTION_WORK   (5)  ///< 秘密の質問
#define _SECRET_ANSWER_WORK   (5)  ///< 秘密の答え
#define _UNDER_GOODS_WORK   (2)    /// グッズ
// ----------------------------------------------------------------------------



//--------------------------------------------------------------
// function
//--------------------------------------------------------------

extern COMM_MESSAGE* CommMsgInitialize(int msgManagerNo,int heapID, GF_BGL_INI* bgl,int speed, int msgSize);
extern void CommMsgSetBgl(COMM_MESSAGE* pComm,GF_BGL_INI* bgl, u16 wincgx,u16 msgcgx);
extern void CommMsgFinalize(COMM_MESSAGE* pComm);
extern void CommMsgChangeMSGMAN(COMM_MESSAGE* pComm, int msgManagerNo, int type);
extern MSGDATA_MANAGER* CommMsgGetMsgManager(COMM_MESSAGE* pComm);

extern int CommMsgTalkWindowAllWrite(COMM_MESSAGE* pComm, int msgNo, BOOL bTask, PTRCommMsgCallback pEndCallBack);
extern int CommMsgTalkWindowStart(COMM_MESSAGE* pComm, int msgNo, BOOL bTask, PTRCommMsgCallback pEndCallBack);
extern int CommMsgTalkWindowStartSendNum(COMM_MESSAGE* pComm, int msgNo, BOOL bTask, PTRCommMsgCallback pEndCallBack, int callbackNum);
//extern int CommMsgTalkWindowStartBuff(COMM_MESSAGE* pComm, STRBUF* pStrBuf, BOOL bTask,  PTRCommMsgCallback pEndCallBack);
extern void CommMsgTalkWindowEnd(COMM_MESSAGE* pComm);
extern void CommMsgTalkWindowMeWait(COMM_MESSAGE* pComm);

extern STRBUF* CommMsgGetExpandStrBuf(COMM_MESSAGE* pComm, int msgNo);

// レジスターに単語登録するための関数群
extern void CommMsgRegisterMyName(COMM_MESSAGE* pComm, MYSTATUS* pMy);
extern void CommMsgRegisterTargetName(COMM_MESSAGE* pComm, MYSTATUS* pTarget);
extern void CommMsgRegisterUGItemName(COMM_MESSAGE* pComm,int type);
// ----------------------------------------------------------------------------
// localize_spec_mark(LANG_ALL) imatake 2006/11/28
// 不定冠詞付きの地下アイテム名を代入する関数を追加
extern void CommMsgRegisterUGItemNameIndefinate(COMM_MESSAGE* pComm,int type);
// ----------------------------------------------------------------------------
extern void CommMsgRegisterUGTrapName(COMM_MESSAGE* pComm,int type);
// ----------------------------------------------------------------------------
// localize_spec_mark(LANG_ALL) imatake 2006/11/28
// 不定冠詞付きの地下ワナ名を代入する関数を追加
extern void CommMsgRegisterUGTrapNameIndefinate(COMM_MESSAGE* pComm,int type);
// ----------------------------------------------------------------------------
extern void CommMsgRegisterSecretQuestionName(COMM_MESSAGE* pComm,int type);
extern void CommMsgRegisterSecretAnswerNameIndex(COMM_MESSAGE* pComm,int index,int type);
extern void CommMsgRegisterSecretAnswerName(COMM_MESSAGE* pComm,int type);
extern void CommMsgRegisterUGGoodsName(COMM_MESSAGE* pComm,int type);
// ----------------------------------------------------------------------------
// localize_spec_mark(LANG_ALL) imatake 2006/11/28
// 不定冠詞付きの地下グッズ名を代入する関数を追加
extern void CommMsgRegisterUGGoodsNameIndefinate(COMM_MESSAGE* pComm,int type);
// ----------------------------------------------------------------------------
extern void CommMsgRegisterNumber1Index(COMM_MESSAGE* pComm,int index,int num);
extern void CommMsgRegisterNumber2(COMM_MESSAGE* pComm,int num);
extern void CommMsgRegisterNumber2Index(COMM_MESSAGE* pComm,int index,int num);
extern void CommMsgRegisterNumber5(COMM_MESSAGE* pComm,int num);
extern void CommMsgRegisterNumber6(COMM_MESSAGE* pComm,int num);
extern void CommMsgRegisterUGItemNameIndex(COMM_MESSAGE* pComm,int index,int type);
extern void CommMsgRegisterUGTrapNameIndex(COMM_MESSAGE* pComm,int index,int type);
extern void CommMsgRegisterUGGoodsNameIndex(COMM_MESSAGE* pComm,int index,int type);
// ----------------------------------------------------------------------------
// localize_spec_mark(LANG_ALL) imatake 2006/11/28
// 不定冠詞付きの地下アイテム・ワナ・グッズ名を代入する関数を追加
extern void CommMsgRegisterUGItemNameIndexIndefinate(COMM_MESSAGE* pComm,int index,int type);
extern void CommMsgRegisterUGTrapNameIndexIndefinate(COMM_MESSAGE* pComm,int index,int type);
extern void CommMsgRegisterUGGoodsNameIndexIndefinate(COMM_MESSAGE* pComm,int index,int type);
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// localize_spec_mark(LANG_ALL) imatake 2006/11/29
// 指定インデックスに登録された単語をキャピタライズ
extern void CommMsgCapitalizeIndex(COMM_MESSAGE* pComm,int index);
// ----------------------------------------------------------------------------
extern void CommMsgPrintStop(COMM_MESSAGE* pComm);

extern const BMPLIST_HEADER* CommMsgGetNormalBmpListHeader(void);
extern BOOL CommMsgIsOutputing(COMM_MESSAGE* pComm);
extern void CommMsgFunc(COMM_MESSAGE* pComm);



#endif //__UG_MESSAGE_H__
