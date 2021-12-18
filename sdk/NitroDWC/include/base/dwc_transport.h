/*---------------------------------------------------------------------------*
  Project:  NitroDWC - base - dwc_transport
  File:     dwc_transport.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc_transport.h,v $
  Revision 1.18  2006/01/23 04:50:43  arakit
  統一用語に合わせてコメントを修正

  Revision 1.17  2006/01/17 07:22:35  arakit
  ・DWC_SetUserSendCallback(), DWC_SetUserRecvCallback(),
  　DWC_SetUserRecvTimeoutCallback(), DWC_SetSendDrop(), DWC_SetRecvDrop(),
  　DWC_SetSendDelay(), DWC_SetRecvDelay(), DWC_Ping(),
  　DWC_SetUserPingCallback(), DWC_SetSendSplitMax() の戻り値をvoidからBOOLに
  　変更し、制御変数確保前は何もせずにFALSEを返すようにした。

  Revision 1.16  2005/09/24 12:56:30  arakit
  サーバクライアント型マッチング時に、マッチング完了したホストのみを有効なAIDとして
  扱うよう、AIDやコネクション数へのアクセス関数を修正した。

  Revision 1.15  2005/09/24 08:16:26  nakatat
  送信可能判定関数DWC_IsSendableReliable追加｡

  Revision 1.14  2005/09/24 07:17:12  nakatat
  送信分割最大サイズ設定関数DWC_SetSendSplitMax追加。
  送信可能判定関数DWCi_SendReliable追加｡
  関連部分の修正｡

  Revision 1.13  2005/09/22 10:45:01  arakit
  コネクションクローズ時に、sTransInfoのconnectionを一部クリアするようにした。

  Revision 1.12  2005/09/22 08:56:21  nakatat
  コメント追加｡

  Revision 1.11  2005/09/22 07:50:45  nakatat
  受信タイムアウト通知機能を追加。
  （DWC_SetUserRecvTimeoutCallback、DWC_SetRecvTimeoutTime）
  DWCstTransportConnection、DWCstTransportInfoに関連メンバを追加。

  Revision 1.10  2005/09/13 11:10:56  arakit
  ・既にクローズされたaidに対して送信しようとしたらデータアクセス例外で
  　止まってしまう不具合を修正した。
  ・AIDビットマップ取得関数   u32 DWC_GetAIDBitmap( void )、
  　AID指定Reliable送信関数   u32 DWC_SendReliableBitmap( u32 bitmap, const void* buffer, int size )、
  　AID指定Unreliable送信関数 u32 DWC_SendUnreliableBitmap( u32 bitmap, const void* buffer, int size )
  　を用意した。

  Revision 1.9  2005/09/01 12:15:00  arakit
  マッチング完了時の同期調整処理を追加した。

  Revision 1.8  2005/08/30 10:39:45  ito
  ・DWCi_SendReliableを追加
  ・遅延設定の不具合を修正

  Revision 1.7  2005/08/26 08:15:23  arakit
  ・ASSERTとモジュール制御変数のチェックを整理した。
  ・外部関数DWC_ShutdownFriendsMatch()の引数をとらないように変更した。
  また、同関数から各モジュールの制御変数をNULLクリアするようにした。
  ・エラー処理関数を変更し、エラーセット、エラーチェックを必要な箇所に入れた。

  Revision 1.6  2005/08/20 07:01:20  sasakit
  ヘッダインクルードガードの書式を統一した。
  bm/dwc_init.h -> bm/dwc_bm_init.hに変更
  ヘッダにCopyrightをつけた。
  構造体の名前空間をできるだけルールにちかづけた。
  util_wifiidtool.hとdwc_backup.hの関係を若干修正。


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef DWC_TRANSPORT_H_
#define DWC_TRANSPORT_H_

#include <darray.h>

#ifdef  __cplusplus
extern "C" {
#endif

//#pragma warn_padding on


#define	DWC_MAGIC_STRINGS					"DT"	// Reliable送信のヘッダーに付ける文字列	   
#define	DWC_MAGIC_STRINGS_LEN				2		// Reliable送信のヘッダーに付ける文字列の長さ	   
#define	DWC_TRANSPORT_SEND_MAX				1465	// 一度に送信できる最大サイズ
#define	DWC_TRANSPORT_GT2HEADER_SIZE		7		// gt2が付けるヘッダのサイズ

/** -----------------------------------------------------------------
  Reliable送信制御に使用
  -------------------------------------------------------------------*/
enum
{
	DWC_TRANSPORT_SEND_READY,						// データ送信可能。
	DWC_TRANSPORT_SEND_BUSY,						// データ送信中

	DWC_TRANSPORT_SEND_LAST			
};

/** -----------------------------------------------------------------
  受信制御に使用
  -------------------------------------------------------------------*/
enum
{
	DWC_TRANSPORT_RECV_NOBUF,						// 受信バッファが設定されていない
	DWC_TRANSPORT_RECV_HEADER,						// ヘッダー受信待ち
	DWC_TRANSPORT_RECV_BODY,						// データ本体待ち
    DWC_TRANSPORT_RECV_SYSTEM_DATA,					// 内部使用データ待ち
	DWC_TRANSPORT_RECV_ERROR,

	DWC_TRANSPORT_RECV_LAST			
};

/** -----------------------------------------------------------------
  送信データの識別子
  -------------------------------------------------------------------*/
enum
{
	DWC_SEND_TYPE_INVALID,
    DWC_SEND_TYPE_USERDATA,							// ユーザーデータ

    // ↓システムデータ（内部使用データ）用の識別子を以下に並べる
    DWC_SEND_TYPE_MATCH_SYN,						// マッチメイク終了同期用SYN
    DWC_SEND_TYPE_MATCH_SYN_ACK,					// マッチメイク終了同期用SYN-ACK
    DWC_SEND_TYPE_MATCH_ACK,						// マッチメイク終了同期用ACK
    // ↑ここまで

    DWC_SEND_TYPE_MAX
};

/** -----------------------------------------------------------------
  構造体のtypedef
  -------------------------------------------------------------------*/
typedef struct DWCstTransportInfo			DWCTransportInfo;
typedef struct DWCstTransportHeader			DWCTransportHeader;
typedef struct DWCstTransportConnection		DWCTransportConnection;
typedef	struct DWCstDelayedMessage			DWCDelayedMessage;

/** -----------------------------------------------------------------
  関数のtypedef
  -------------------------------------------------------------------*/
typedef void	(*DWCUserSendCallback)( int size, u8 aid );
typedef void	(*DWCUserRecvCallback)( u8 aid, u8* buffer, int size );
typedef void	(*DWCUserRecvTimeoutCallback)( u8 aid );
typedef void	(*DWCUserPingCallback)( int latency, u8 aid );

/** -----------------------------------------------------------------
  送受信を制御する為に使用する構造体
  -------------------------------------------------------------------*/
// コネクション毎に管理する情報
struct DWCstTransportConnection
{
	const u8*				sendBuffer;				// 送信バッファへのポインタ	
	u8*						recvBuffer;				// 受信バッファへのポインタ
	int						recvBufferSize;			// 受信バッファのサイズ

	int						sendingSize;			// 送信中サイズ
	int						recvingSize;			// 受信中サイズ
    int						requestSendSize;		// 送信要求サイズ
	int						requestRecvSize;		// 受信要求サイズ

    u8						sendState;				// 送信状態	
	u8						recvState;				// 受信状態

    u8						lastRecvState;			// ヘッダ受信前の最終受信状態
    u8						pads[3];
    u16						lastRecvType;			// 最終受信ヘッダタイプ
    
	OSTick					previousRecvTick;
	u32						recvTimeoutTime;

#ifndef	SDK_FINALROM

    u16						sendDelay;				// 送信時の遅延
    u16						recvDelay;				// 受信時の遅延    

#endif	// SDK_FINALROM
};

// 送受信の制御
struct DWCstTransportInfo
{
	// コネクション毎に管理する情報
    DWCTransportConnection		connections[ DWC_MAX_CONNECTIONS ];

    DWCUserSendCallback			sendCallback;			// 送信完了コールバック
	DWCUserRecvCallback			recvCallback;			// 受信完了コールバック
	DWCUserRecvTimeoutCallback	recvTimeoutCallback;	// 受信タイムアウトコールバック
	DWCUserPingCallback			pingCallback;			// Pingコールバック

	u16							sendSplitMax;			// 送信分割最大サイズ
    
#ifndef	SDK_FINALROM

    MATHRandContext16			context;				// 乱数生成
	u32							seed;					// 乱数生成

	DArray						delayedSend;			// 送信遅延をエミュレートする為に使用するバッファ
	DArray						delayedRecv;			// 受信遅延をエミュレートする為に使用するバッファ

    u8							sendDrop;				// 送信パケットロス率
    u8							recvDrop;				// 受信パケットロス率
	u8							pads[2];
    
#endif	// SDK_FINALROM
};	

/** -----------------------------------------------------------------
  Reliable送信に使うヘッダ
  -------------------------------------------------------------------*/
struct DWCstTransportHeader
{
	int						size;					// 送信サイズ
    u16						type;					// 送信データのタイプ
    char					magicStrings[ DWC_MAGIC_STRINGS_LEN ];	// Reliable送信のヘッダー識別の為に付ける文字列	   
};

/** -----------------------------------------------------------------
  遅延のエミュレートに使用する構造体
  -------------------------------------------------------------------*/
struct DWCstDelayedMessage
{
    GT2Connection			connection;
    int						filterID;
    u8*						message;
    int						size;
    BOOL					reliable;
    OSTick					startTime;				// 送信をした時間
    OSTick					delayTime;	    		// 遅延させる時間
};

/** -----------------------------------------------------------------
  外部関数		
  -------------------------------------------------------------------*/
extern BOOL		DWC_SendReliable				( u8 aid, const void* buffer, int size );		// Relible送信
extern u32		DWC_SendReliableBitmap			( u32 bitmap, const void* buffer, int size );	// Bitmap指定Relible送信
extern BOOL		DWC_SendUnreliable				( u8 aid, const void* buffer, int size );		// Unreliable送信
extern u32		DWC_SendUnreliableBitmap		( u32 bitmap, const void* buffer, int size );	// Bitmap指定Unreliable送信
extern BOOL		DWC_Ping						( u8 aid );										// Ping値測定
extern BOOL		DWC_SetRecvBuffer				( u8 aid, void* recvBuffer, int size );			// 受信バッファを設定
extern BOOL		DWC_SetUserSendCallback			( DWCUserSendCallback callback );				// 送信コールバック
extern BOOL		DWC_SetUserRecvCallback			( DWCUserRecvCallback callback );				// 受信コールバック
extern BOOL		DWC_SetUserRecvTimeoutCallback	( DWCUserRecvTimeoutCallback callback );		// 受信タイムアウトコールバック
extern BOOL		DWC_SetRecvTimeoutTime			( u8 aid, u32 time );							// 受信タイムアウト時間（単位ミリ秒）を設定
extern BOOL		DWC_SetUserPingCallback			( DWCUserPingCallback callback );				// Ping測定完了コールバック
extern BOOL		DWC_SetSendSplitMax				( u16 sendSplitMax );							// 送信分割最大サイズを設定
extern BOOL		DWC_IsSendableReliable			( u8 aid );										// Reliable送信可能判定関数

#ifndef	SDK_FINALROM

extern BOOL		DWC_SetSendDelay				( u16 delay, u8 aid );							// 送信側に遅延を設定
extern BOOL		DWC_SetRecvDelay				( u16 delay, u8 aid );							// 受信側に遅延を設定
extern BOOL		DWC_SetSendDrop					( u8 drop, u8 aid );							// 送信側パケットロス率を設定
extern BOOL		DWC_SetRecvDrop					( u8 drop, u8 aid );							// 受信側パケットロス率を設定

#else

#define			DWC_SetSendDelay( delay,  aid )	( (void) 0 )
#define			DWC_SetRecvDelay( delay,  aid )	( (void) 0 )
#define			DWC_SetSendDrop( drop, aid )	( (void) 0 )
#define			DWC_SetRecvDrop( drop, aid )	( (void) 0 )

#endif	// SDK_FINALROM

void			DWCi_InitTransport				( DWCTransportInfo* info );
BOOL			DWCi_IsSendableReliable			( u8 aid, u16 type );
BOOL			DWCi_SendReliable				( u16 type, u8 aid, const void* buffer, int size );
void			DWCi_RecvCallback				( GT2Connection	connection, GT2Byte* message, int size, GT2Bool	reliable );
void		    DWCi_PingCallback				( GT2Connection connection, int latency );
void			DWCi_TransportProcess			( void );
void			DWCi_ShutdownTransport			( void );
void			DWCi_ClearTransConnection		( u8 aid );


//#pragma warn_padding reset


#ifdef  __cplusplus
}		/* extern "C" */
#endif


#endif // DWC_TRANSPORT_H_
