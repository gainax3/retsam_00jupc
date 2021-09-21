/*---------------------------------------------------------------------------*
  Project:  NitroDWC -  - 
  File:     dwc_ranksession.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc_ranksession.h,v $
  Revision 1.7  2007/08/24 00:08:13  takayama
  DWC_2_2_RC1 にロールバック。

  Revision 1.4  2006/04/13 11:41:52  hayashi
  character code fixed

  Revision 1.3  2006/04/13 11:35:19  hayashi
  Change prefix Ranking->Rnk

  Revision 1.2  2006/04/11 09:01:34  hayashi
  DWC_RnkInitializeで、接続先のサーバーを選択できるようにした

  Revision 1.1  2006/04/10 10:16:59  hayashi
  added the ranking module


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef DWC_RANKSESSION_H_
#define DWC_RANKSESSION_H_

#ifdef __cplusplus
extern "C" {
#endif


/* -------------------------------------------------------------------------
        enum
   ------------------------------------------------------------------------- */

//!< セッションステータス
typedef enum{

	DWCi_RANKING_SESSION_STATE_CANCELED,		//!< キャンセル
	DWCi_RANKING_SESSION_STATE_ERROR,			//!< エラー発生
	DWCi_RANKING_SESSION_STATE_INITIAL,			//!< 初期状態
	DWCi_RANKING_SESSION_STATE_INITIALIZED,		//!< 初期化後
	DWCi_RANKING_SESSION_STATE_REQUEST,			//!< リクエスト発行
	DWCi_RANKING_SESSION_STATE_GETTING_TOKEN,	//!< トークン取得中
	DWCi_RANKING_SESSION_STATE_GOT_TOKEN,		//!< トークン取得完了
	DWCi_RANKING_SESSION_STATE_SENDING_DATA,	//!< データ送信中
	DWCi_RANKING_SESSION_STATE_COMPLETED		//!< 完了

}DWCiRankingSessionState;

//!< 関数返り値
typedef enum{

	DWCi_RANKING_SESSION_SUCCESS,				//!< 正常終了
	DWCi_RANKING_SESSION_ERROR_NOTINITIALIZED,	//!< 未初期化
	DWCi_RANKING_SESSION_ERROR_NOMEMORY,		//!< メモリー不足
	DWCi_RANKING_SESSION_ERROR_INVALID_KEY		//!< 不正なキー

}DWCiRankingSessionResult;


/* -------------------------------------------------------------------------
        prototype
   ------------------------------------------------------------------------- */

void DWCi_RankingSessionInitialize( BOOL releaseserver );

void DWCi_RankingSessionShutdown( void );

BOOL DWCi_RankingValidateKey(	const char* gamename,
								const char* secretkey,
								u32 key1,
								u32 key2,
								u32 key3,
								u32 key4 );

DWCiRankingSessionResult DWCi_RankingSessionPutAsync( u32 category,
													s32 pid,
													DWCRnkRegion region,
													s32 score,
													void* data,
													u32 size );

DWCiRankingSessionResult DWCi_RankingSessionGetAsync( DWCRnkGetMode mode,
													u32 category,
													s32 pid,
													DWCRnkRegion region,
													DWCRnkGetParam* param );
DWCiRankingSessionState DWCi_RankingSessionProcess( void );

void* DWCi_RankingSessionGetResponse( u32* size );

void DWCi_RankingSessionCancel( void );


#ifdef __cplusplus
}
#endif


#endif
