/*---------------------------------------------------------------------------*
  Project:  RevolutionDWC Libraries
  File:     ./include/enc/dwci_encsession.h

  Copyright 2005-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/
/**
 * @file
 *
 * @brief HTTP通信のラッパーユーティリティー ヘッダ
 */

#ifndef DWC_ENCSESSION_H_
#define DWC_ENCSESSION_H_

#ifdef __cplusplus
extern "C"
{
#endif


    /* -------------------------------------------------------------------------
            enum
       ------------------------------------------------------------------------- */

/// セッションステータスを表す列挙型
    typedef enum{

        DWCi_ENC_SESSION_STATE_CANCELED,		//!< キャンセル
        DWCi_ENC_SESSION_STATE_ERROR,			//!< エラー発生
        DWCi_ENC_SESSION_STATE_INITIAL,			//!< 初期状態
        DWCi_ENC_SESSION_STATE_INITIALIZED,		//!< 初期化後
        DWCi_ENC_SESSION_STATE_REQUEST,			//!< リクエスト発行
        DWCi_ENC_SESSION_STATE_GETTING_TOKEN,	//!< トークン取得中
        DWCi_ENC_SESSION_STATE_GOT_TOKEN,		//!< トークン取得完了
        DWCi_ENC_SESSION_STATE_SENDING_DATA,	//!< データ送信中
        DWCi_ENC_SESSION_STATE_COMPLETED		//!< 完了

    }DWCiEncSessionState;

/// 関数返り値
    typedef enum{

        DWCi_ENC_SESSION_SUCCESS,				//!< 正常終了
        DWCi_ENC_SESSION_ERROR_NOTINITIALIZED,	//!< 未初期化
        DWCi_ENC_SESSION_ERROR_NOMEMORY,		//!< メモリー不足
        DWCi_ENC_SESSION_ERROR_INVALID_KEY		//!< 不正なキー

    }DWCiEncSessionResult;

/// 接続先サーバーを表す列挙型
    typedef enum{

        DWCi_ENC_SERVER_RELEASE,    //!< 製品用サーバーに接続します
        DWCi_ENC_SERVER_DEBUG,      //!< 開発用サーバーに接続します
        DWCi_ENC_SERVER_TEST        //!< (使用しないでください)

    }DWCiEncServer;


    /* -------------------------------------------------------------------------
            function type
       ------------------------------------------------------------------------- */

/// 受信データを検証するためのコールバック
    typedef BOOL (*DWCiEncSessionVaridateCallback)( u32* buf, int len );

    /* -------------------------------------------------------------------------
            prototype
       ------------------------------------------------------------------------- */

    void DWCi_EncSessionInitialize( DWCiEncServer server, const char* initdata );

    void DWCi_EncSessionShutdown( void );

    DWCiEncSessionResult DWCi_EncSessionGetAsync( 	const char* url,
            s32 pid,
            void* data,
            u32 size,
            DWCiEncSessionVaridateCallback cb );

    DWCiEncSessionResult DWCi_EncSessionGetReuseHashAsync(	const char* url,
        s32 pid,
        void* data,
        u32 size,
        DWCiEncSessionVaridateCallback cb );

    DWCiEncSessionState DWCi_EncSessionProcess( void );

    void* DWCi_EncSessionGetResponse( u32* size );

    void DWCi_EncSessionCancel( void );


#ifdef __cplusplus
}
#endif


#endif
