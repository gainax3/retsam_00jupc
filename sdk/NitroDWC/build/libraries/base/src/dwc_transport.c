#include <nitro.h>
#include <base/dwc_base_gamespy.h>

#include <base/dwc_report.h>
#include <base/dwc_account.h>
#include <base/dwc_error.h>
#include <base/dwc_login.h>
#include <base/dwc_friend.h>
#include <base/dwc_match.h>
#include <base/dwc_transport.h>
#include <base/dwc_main.h>


/** -----------------------------------------------------------------
  内部変数
  -------------------------------------------------------------------*/
static DWCTransportInfo*			sTransInfo = NULL;

/** -----------------------------------------------------------------
  内部関数
  -------------------------------------------------------------------*/
static DWCTransportConnection*	DWCs_GetTransConnection	( u8 aid );

static int			DWCs_GetSendState					( u8 aid );
static int			DWCs_GetRecvState					( u8 aid );
static void			DWCs_Send							( u8 aid, const u8* buffer, int size, BOOL reliable );
static void			DWCs_HandleUnreliableMessage		( GT2Connection	connection, void* message, int size );
static void			DWCs_HandleReliableMessage			( GT2Connection connection, u8* message, int size );
static void			DWCs_RecvDataHeader					( u8 aid, DWCTransportHeader* message, int size );
static void			DWCs_RecvDataBody					( u8 aid, void* message, int size );
static void			DWCs_RecvSystemDataBody				( u8 aid, void*	message, int size );
static void			DWCs_DelayCheck						( DArray delayedMessages, OSTick now, BOOL send );
static void			DWCs_AddDelayMessage				( DArray array, u16 delayValue, GT2Connection connection, int filterID, const u8* message, int size, BOOL reliable );
static void			DWCs_DelayedMessageFree				( void* elem );
static int			DWCs_GetOutgoingBufferFreeSize		( u8 aid );
static void			DWCs_SendFilterCallbackDrop			( GT2Connection connection, int filterID, const u8* message, int size, BOOL reliable );
static void			DWCs_SendFilterCallbackDelay		( GT2Connection	connection, int	filterID, const u8* message, int size, BOOL reliable );
static void			DWCs_RecvFilterCallbackDrop			( GT2Connection connection, int	filterID, u8* message, int size, BOOL reliable );
static void			DWCs_RecvFilterCallbackDelay		( GT2Connection connection, int filterID, u8* message, int size, BOOL reliable );
static void			DWCs_EncodeHeader					( DWCTransportHeader* header, u16 type, int size );
static u16			DWCs_DecodeHeader					( const DWCTransportHeader* header );
static s32			DWCs_GetRequiredHeaderSize			( u16 type );


/** -----------------------------------------------------------------
  ユーザーに公開するReliable送信可能判定関数
  -------------------------------------------------------------------*/
BOOL
DWC_IsSendableReliable( u8 aid )
{
    return DWCi_IsSendableReliable( aid, DWC_SEND_TYPE_USERDATA );
}

/** -----------------------------------------------------------------
  ライブラリのみで使用するReliable送信可能判定関数
  -------------------------------------------------------------------*/
BOOL
DWCi_IsSendableReliable( u8 aid, u16 type )
{
	s32 freeSpace;

    if ( DWCi_IsError() ||
         ( ( type == DWC_SEND_TYPE_USERDATA ) && ! DWC_IsValidAID( aid ) ) ||
         ! DWCi_IsValidAID( aid ) )
    {
        // 既にクローズされたコネクションに送信しようとした場合
        // 自分のAIDに送信しようとした場合もここに来る
		DWC_Printf( DWC_REPORTFLAG_WARNING, "aid %d is unavailable.\n", aid );
		return FALSE;
	}
    
	// 送信中のデータがあるならFALSEを返して終了する
	if ( DWCs_GetSendState( aid ) == DWC_TRANSPORT_SEND_BUSY )
	{
		DWC_Printf( DWC_REPORTFLAG_SEND_INFO, "+++ Cannot send to %d from %d (busy)\n", aid, DWC_GetMyAID() );
		return FALSE;
	}

	// ヘッダーを送信する為のOutgoingBufferの空きがあるか確認
	freeSpace = DWCs_GetOutgoingBufferFreeSize( aid );

	if ( freeSpace < DWCs_GetRequiredHeaderSize( type ) )
	{
		DWC_Printf( DWC_REPORTFLAG_SEND_INFO, "+++ Cannot send to %d from %d (outgoing buffer is not enough) %d < %d\n", aid, DWC_GetMyAID(), freeSpace, DWCs_GetRequiredHeaderSize( type ) );
		return FALSE;
	}

	return TRUE;
}

/** -----------------------------------------------------------------
  ユーザーに公開するReliable送信
  -------------------------------------------------------------------*/
BOOL
DWC_SendReliable(
    u8						aid,
	const void*				buffer,
	int						size )
{
    return DWCi_SendReliable( DWC_SEND_TYPE_USERDATA, aid, buffer, size );
}

/** -----------------------------------------------------------------
  ライブラリのみで使用するReliable送信
  -------------------------------------------------------------------*/
BOOL
DWCi_SendReliable(
	u16						type,
    u8						aid,
	const void*				buffer,
	int						size )
{
	DWCTransportConnection*	transConnection = DWCs_GetTransConnection( aid );
	DWCTransportHeader		header;
	int						sendSize;
	int						freeSpace;

    SDK_ASSERT( buffer );
    SDK_ASSERT( size > 0 );

    //if ( DWCi_IsError() ) return FALSE;

    if ( ! DWCi_IsSendableReliable( aid, type ) )
    {
        return FALSE;
    }

    // 送信状態のパラメータを初期化
    transConnection->sendState			= DWC_TRANSPORT_SEND_BUSY; 
	transConnection->sendBuffer			= buffer;				
	transConnection->sendingSize		= 0;					
	transConnection->requestSendSize	= size;					

    // 送信データ全体のサイズを送信する
	DWCs_EncodeHeader( &header, type, size );
    DWCs_Send( aid, (const u8*) &header, sizeof( DWCTransportHeader ), TRUE ); 

    // 送信サイズを決める
	if( size > sTransInfo->sendSplitMax ){
		sendSize = sTransInfo->sendSplitMax;
	}else{
		sendSize = size;
	}

    // データ本体を送信する為のOutgoingBufferの空きがあるか調べる
	freeSpace = DWCs_GetOutgoingBufferFreeSize( aid );
#if 1
	DWC_Printf(
		DWC_REPORTFLAG_SEND_INFO,
		"DWCi_SendReliable:aid = %d,%d,%d,%d,%d,vcount = %d\n",
		aid,sendSize,freeSpace,size,sTransInfo->sendSplitMax,OS_GetVBlankCount()
	);
#endif
	if ( sendSize > freeSpace )
    {
		// OutgoingBufferの空きがない！
        // DWCi_TransportProcessで送信する
		DWC_Printf(
			DWC_REPORTFLAG_SEND_INFO,
			"DWCi_SendReliable:sendSize > freeSpace:aid = %d,%d,%d,vcount = %d\n",
			aid,sendSize,freeSpace,OS_GetVBlankCount()
		);
        return TRUE;
    }

    // データ本体を送信する
	DWCs_Send( aid, buffer, sendSize, TRUE ); 
    
    // 送信済みサイズを更新
    transConnection->sendingSize += sendSize;

	DWC_Printf(
		DWC_REPORTFLAG_SEND_INFO,
		"[R] aid = %d, size = %d/%d, outgoing buffer = %d, vcount = %d\n",
		aid, transConnection->sendingSize, transConnection->requestSendSize, freeSpace - sendSize, OS_GetVBlankCount()
	);

    // 送信済みサイズと送信予定サイズが一致したら完了
	if ( transConnection->sendingSize == transConnection->requestSendSize )
	{
        // [arakit] main 051026
        int reqSendSize = transConnection->requestSendSize;
        
        // 送信状態のパラメータをリセット
		transConnection->sendState			= DWC_TRANSPORT_SEND_READY;
		transConnection->sendBuffer			= NULL;
        transConnection->sendingSize		= 0;	
		transConnection->requestSendSize	= 0;
        // [arakit] main 051026
        
		// 送信完了コールバック（ユーザデータ送信時のみ）
		if ( sTransInfo->sendCallback && type == DWC_SEND_TYPE_USERDATA )
		{
            // [arakit] main 051026
            sTransInfo->sendCallback( reqSendSize, aid );
		}
	}

    return TRUE;
}

/** -----------------------------------------------------------------
  Bitmap指定Reliable送信
  -------------------------------------------------------------------*/
u32
DWC_SendReliableBitmap(
    u32						bitmap,
	const void*				buffer,
	int						size )
{
    u8	aid;
    u32 aidbit;

    for ( aid = 0; aid < DWC_MAX_CONNECTIONS; ++aid )
    {
        aidbit = aid ? ( 1 << aid ) : 1U;
        if ( ( bitmap & aidbit ) && ( aid != DWC_GetMyAID() ) )
        {
            // Reliable送信を行い、失敗したら失敗したaidのbitを下げる
            if ( ! DWC_SendReliable( aid, buffer, size ) )
                bitmap &= ~aidbit;
        }
        // 自分のAIDのビットも立てて渡された場合は、それを下げずに返す
    }

    return bitmap;
}

/** -----------------------------------------------------------------
  Unreliable送信
  -------------------------------------------------------------------*/
BOOL
DWC_SendUnreliable(
	u8						aid,
	const void*				buffer,
	int						size )
{
    SDK_ASSERT( buffer );
    SDK_ASSERT( size > 0 );

    if ( DWCi_IsError() ) return FALSE;

    if ( ! DWC_IsValidAID( aid ) )
    {
        // 既にクローズされたコネクションに送信しようとした場合
        DWC_Printf( DWC_REPORTFLAG_WARNING, "aid %d is now unavailable.\n", aid );
        return FALSE;
    }

    if ( size > sTransInfo->sendSplitMax )
    {
        DWC_Printf( DWC_REPORTFLAG_SEND_INFO, "+++ SendUnreliable size is too large ( %d > %d )\n", size, sTransInfo->sendSplitMax );
		return FALSE;
    }
    
	DWCs_Send( aid, buffer, size, FALSE );

	DWC_Printf( DWC_REPORTFLAG_SEND_INFO, "[U] aid = %d, size = %d, vcount = %d\n", aid, size, OS_GetVBlankCount() );    
    
	// 送信完了コールバック
	if ( sTransInfo->sendCallback )
	{
		sTransInfo->sendCallback( size, aid );
	}

    return TRUE;
}

/** -----------------------------------------------------------------
  Bitmap指定Unreliable送信
  -------------------------------------------------------------------*/
u32
DWC_SendUnreliableBitmap(
    u32						bitmap,
	const void*				buffer,
	int						size )
{
    u8	aid;
    u32 aidbit;

    for ( aid = 0; aid < DWC_MAX_CONNECTIONS; ++aid )
    {
        aidbit = aid ? ( 1 << aid ) : 1U;
        if ( bitmap & aidbit && ( aid != DWC_GetMyAID() ) )
        {
            // Unreliable送信を行い、失敗したら失敗したaidのbitを下げる
            if ( ! DWC_SendUnreliable( aid, buffer, size ) )
                bitmap &= ~aidbit;
        }
        // 自分のAIDのビットも立てて渡された場合は、それを下げずに返す
    }

    return bitmap;
}

/** -----------------------------------------------------------------
  受信バッファを設定
  -------------------------------------------------------------------*/
BOOL
DWC_SetRecvBuffer(
	u8						aid,
	void*					recvBuffer,
	int						size )
{
	DWCTransportConnection*	connection = DWCs_GetTransConnection( aid );

    SDK_ASSERT( recvBuffer );
    SDK_ASSERT( size > 0 );    

    // データを受信中ならFALSEを返して終了
    if ( DWCs_GetRecvState( aid ) == DWC_TRANSPORT_RECV_BODY )
    {
	    DWC_Printf( DWC_REPORTFLAG_RECV_INFO, "+++ Cannot set recv buffer\n" );

        return FALSE;
    }
         
	// 受信バッファを設定
	connection->recvBuffer		= recvBuffer;
	connection->recvBufferSize	= size;	

    // 受信情報を初期化する
	connection->recvState		= DWC_TRANSPORT_RECV_HEADER;    
    connection->recvingSize		= 0;
	connection->requestRecvSize	= 0;

	return TRUE;
}

/** -----------------------------------------------------------------
  Ping
  -------------------------------------------------------------------*/
BOOL
DWC_Ping( u8 aid )
{
    GT2Connection connection;
    
    if ( DWCi_IsError() ) return FALSE;
    
    connection = DWCi_GetGT2Connection( aid );

    if ( ( aid == DWC_GetMyAID() ) || ( ! connection ) || ( gt2GetConnectionState( connection ) != GT2Connected ) )
    {
		DWC_Printf( DWC_REPORTFLAG_SEND_INFO, "DWC_Ping:not connected yet:%d\n",aid);
		return FALSE;
    }

//    SDK_ASSERT( connection );

    gt2Ping( connection );

    return TRUE;
}

/** -----------------------------------------------------------------
  送信コールバックを設定
  -------------------------------------------------------------------*/
BOOL
DWC_SetUserSendCallback( DWCUserSendCallback callback )
{
    if ( sTransInfo == NULL ) return FALSE;
    
	sTransInfo->sendCallback = callback;

    return TRUE;
}

/** -----------------------------------------------------------------
  受信コールバックを設定
  -------------------------------------------------------------------*/
BOOL
DWC_SetUserRecvCallback( DWCUserRecvCallback callback )
{
    if ( sTransInfo == NULL ) return FALSE;
    
	sTransInfo->recvCallback = callback;

    return TRUE;
}

/** -----------------------------------------------------------------
  受信タイムアウトコールバックを設定
  -------------------------------------------------------------------*/
BOOL
DWC_SetUserRecvTimeoutCallback( DWCUserRecvTimeoutCallback callback )
{
    if ( sTransInfo == NULL ) return FALSE;
    
	sTransInfo->recvTimeoutCallback = callback;

    return TRUE;
}

/** -----------------------------------------------------------------
  Pingコールバックを設定
  -------------------------------------------------------------------*/
BOOL
DWC_SetUserPingCallback( DWCUserPingCallback callback )
{
    if ( sTransInfo == NULL ) return FALSE;
    
	sTransInfo->pingCallback = callback;

    return TRUE;
}

/** -----------------------------------------------------------------
  送信分割最大サイズを設定
  -------------------------------------------------------------------*/
BOOL
DWC_SetSendSplitMax( u16 sendSplitMax )
{
    if ( sTransInfo == NULL ) return FALSE;
#if 1 // 2006/02/21
	SDK_ASSERT(sendSplitMax <= DWC_TRANSPORT_SEND_MAX);
#else    
	if( sendSplitMax > DWC_TRANSPORT_SEND_MAX ){
		DWC_Printf( DWC_REPORTFLAG_SEND_INFO, "DWC_SetSendSplitMax:sendSplitMax > DWC_TRANSPORT_SEND_MAX:%d,%d\n",sendSplitMax,DWC_TRANSPORT_SEND_MAX);
		sendSplitMax = DWC_TRANSPORT_SEND_MAX;
	}
#endif
	sTransInfo->sendSplitMax = sendSplitMax;

    return TRUE;
}

/** -----------------------------------------------------------------
  Reliable送信のヘッダーを作成する
  -------------------------------------------------------------------*/
static void
DWCs_EncodeHeader(
    DWCTransportHeader*	header,
	u16					type,
    int					size )
{
    // Reliable送信のヘッダーであることを識別する為に付ける文字列
    strncpy( header->magicStrings, DWC_MAGIC_STRINGS, DWC_MAGIC_STRINGS_LEN );

    header->type = type;
	header->size = size;
}

/** -----------------------------------------------------------------
  Reliable送信のヘッダーを解析する
  -------------------------------------------------------------------*/
static u16
DWCs_DecodeHeader( const DWCTransportHeader* message )
{
    DWCTransportHeader			header;

    // アライメントされたバッファにコピーする
    MI_CpuCopy8( message, (u8*) &header, sizeof( DWCTransportHeader ) );

    if ( ( memcmp( header.magicStrings, DWC_MAGIC_STRINGS, DWC_MAGIC_STRINGS_LEN ) == 0 ) )
    {
        return header.type;
    }

    return DWC_SEND_TYPE_INVALID;
}

/** -----------------------------------------------------------------
  Reliable送信のヘッダー送信に必要な容量を取得
  -------------------------------------------------------------------*/
static s32
DWCs_GetRequiredHeaderSize( u16 type )
{
    s32 size;

    switch ( type )
    {
    case DWC_SEND_TYPE_MATCH_SYN:
    case DWC_SEND_TYPE_MATCH_SYN_ACK:
    case DWC_SEND_TYPE_MATCH_ACK:
        // マッチメイク同期調整データ送信時は、空きが足りなくてTransportProcess()
        // での送信に回されると困るので、データ本体分も含めた空きが必要
        size = sizeof( DWCTransportHeader ) + DWC_MATCH_SYN_DATA_BODY_SIZE;
        break;
    default:
        // 通常はヘッダサイズ分の空きがあれば良い
        size = sizeof( DWCTransportHeader );
        break;
    }

    return size;
}

/** -----------------------------------------------------------------
  受信タイムアウト時間（単位ミリ秒）を設定
  -------------------------------------------------------------------*/
BOOL
DWC_SetRecvTimeoutTime( u8 aid, u32 time )
{
	// GT2Connectedになる前に設定できるようにした
#if 0
    GT2Connection connection = DWCi_GetGT2Connection( aid );

    if ( ( aid == DWC_GetMyAID() ) || ( ! connection ) || ( gt2GetConnectionState( connection ) != GT2Connected ) )
    {
		return;
    }
#else
    if ( !sTransInfo ) return FALSE;
#endif

	sTransInfo->connections[aid].recvTimeoutTime = time;
	sTransInfo->connections[aid].previousRecvTick = OS_GetTick();

    return TRUE;
}

#ifndef SDK_FINALROM

/** -----------------------------------------------------------------
  送信時に遅延させる時間（単位: ms）を設定する
  -------------------------------------------------------------------*/
BOOL
DWC_SetSendDelay( u16 delay, u8 aid )
{
    GT2Connection connection;
    
    if ( sTransInfo == NULL ) return FALSE;
    
    connection = DWCi_GetGT2Connection( aid );

    if ( ( aid == DWC_GetMyAID() ) || ( ! connection ) || ( gt2GetConnectionState( connection ) != GT2Connected ) )
    {
		return FALSE;
    }
    
	if ( delay )
    {
		// コールバックを設定する
        // 既に遅延の値が設定されているなら、コールバックも設定されているはず
        if ( ! sTransInfo->connections[aid].sendDelay )
        {
	        if ( ! gt2AddSendFilter( connection, DWCs_SendFilterCallbackDelay ) )
            {
                DWCi_SetError( DWC_ERROR_FATAL, DWC_ECODE_SEQ_ETC + DWC_ECODE_GS_GT2 + DWC_ECODE_TYPE_ALLOC );
                return FALSE;
            }
        }
    }
    else
    {
		gt2RemoveSendFilter( connection, DWCs_SendFilterCallbackDelay );
    }

	sTransInfo->connections[aid].sendDelay = delay;

    return TRUE;
}

/** -----------------------------------------------------------------
  受信時に遅延させる時間（単位: ms）を設定する
  -------------------------------------------------------------------*/
BOOL
DWC_SetRecvDelay( u16 delay, u8 aid )
{
    GT2Connection connection;
    
    if ( sTransInfo == NULL ) return FALSE;
    
    connection = DWCi_GetGT2Connection( aid );

    if ( ( aid == DWC_GetMyAID() ) || ( ! connection ) || ( gt2GetConnectionState( connection ) != GT2Connected ) )
    {
		return FALSE;
    }
    
    if ( delay )
    {
		// コールバックを設定する
        // 既に遅延の値が設定されているなら、既にコールバックも設定されているはず
        if ( ! sTransInfo->connections[aid].recvDelay )
        {
	        if ( ! gt2AddReceiveFilter( connection, DWCs_RecvFilterCallbackDelay ) )
            {
                DWCi_SetError( DWC_ERROR_FATAL, DWC_ECODE_SEQ_ETC + DWC_ECODE_GS_GT2 + DWC_ECODE_TYPE_ALLOC );
                return FALSE;
            }
        }
    }
    else
    {
		gt2RemoveReceiveFilter( connection, DWCs_RecvFilterCallbackDelay );
    }

    sTransInfo->connections[aid].recvDelay = delay;

    return TRUE;
}

/** -----------------------------------------------------------------
  送信時のパケットロス率を設定
  -------------------------------------------------------------------*/
BOOL
DWC_SetSendDrop( u8 drop, u8 aid )
{
    GT2Connection connection;
    
    if ( sTransInfo == NULL ) return FALSE;
    
    connection = DWCi_GetGT2Connection( aid );

    if ( ( aid == DWC_GetMyAID() ) || ( ! connection ) || ( gt2GetConnectionState( connection ) != GT2Connected ) )
    {
		return FALSE;
    }
    
    sTransInfo->sendDrop = drop;	

	if ( drop )
    {
		if ( ! gt2AddSendFilter( connection, DWCs_SendFilterCallbackDrop ) )
        {
            DWCi_SetError( DWC_ERROR_FATAL, DWC_ECODE_SEQ_ETC + DWC_ECODE_GS_GT2 + DWC_ECODE_TYPE_ALLOC );
            return FALSE;
        }
    }
    else
    {
		gt2RemoveSendFilter( connection, DWCs_SendFilterCallbackDrop );
    }

    return TRUE;
}

/** -----------------------------------------------------------------
  受信時のパケットロス率を設定
  -------------------------------------------------------------------*/
BOOL
DWC_SetRecvDrop( u8 drop, u8 aid )
{
    GT2Connection connection;
    
    if ( sTransInfo == NULL ) return FALSE;
    
    connection = DWCi_GetGT2Connection( aid );

    if ( ( aid == DWC_GetMyAID() ) || ( ! connection ) || ( gt2GetConnectionState( connection ) != GT2Connected ) )
    {
		return FALSE;
    }
    
    sTransInfo->recvDrop = drop;	

	if ( drop )
    {
		if ( ! gt2AddReceiveFilter( connection, DWCs_RecvFilterCallbackDrop ) )
        {
            DWCi_SetError( DWC_ERROR_FATAL, DWC_ECODE_SEQ_ETC + DWC_ECODE_GS_GT2 + DWC_ECODE_TYPE_ALLOC );
            return FALSE;
        }
    }
    else
    {
		gt2RemoveReceiveFilter( connection, DWCs_RecvFilterCallbackDrop );
    }

    return TRUE;
}

#endif	// SDK_FINALROM

/** -----------------------------------------------------------------
  初期化
  -------------------------------------------------------------------*/
void
DWCi_InitTransport( DWCTransportInfo* info )
{
	SDK_ASSERT( info );

    sTransInfo = info;

	MI_CpuClear8( sTransInfo, sizeof( DWCTransportInfo ) );
	sTransInfo->sendSplitMax = DWC_TRANSPORT_SEND_MAX;

#ifndef	SDK_FINALROM
	// 遅延とパケットロスのエミュレートに必要な初期化
    
    // 乱数の初期化
	MATH_InitRand16( &sTransInfo->context, sTransInfo->seed );

    // 遅延発生に使用する配列を確保
	sTransInfo->delayedSend = ArrayNew( sizeof( DWCDelayedMessage ), 10, DWCs_DelayedMessageFree );
	sTransInfo->delayedRecv = ArrayNew( sizeof( DWCDelayedMessage ), 10, DWCs_DelayedMessageFree );

#endif	// SDK_FINALROM
}

/** -----------------------------------------------------------------
  受信コールバック
  -------------------------------------------------------------------*/
void
DWCi_RecvCallback(
	GT2Connection			connection,
	GT2Byte*				message,
	int						size,
	GT2Bool					reliable )
{
    if ( ! sTransInfo )
    {
		return;
    }

    DWC_Printf( DWC_REPORTFLAG_RECV_INFO, "%s aid = %d, size = %d, vcount = %d\n", reliable ? "[R]" : "[U]", DWCi_GetConnectionAID( connection ), size, OS_GetVBlankCount() );

	if( message == NULL || size == 0 )
    {
        DWC_Printf( DWC_REPORTFLAG_WARNING, "Recv NULL message %x, size = %d\n", message, size );
		return;
	}

    if ( reliable )
	{
        DWCs_HandleReliableMessage( connection, message, size );
	}
	else
	{
		DWCs_HandleUnreliableMessage( connection, message, size );
	}
}

/** -----------------------------------------------------------------
  Pingコールバック
  -------------------------------------------------------------------*/
void
DWCi_PingCallback
(
	GT2Connection			connection,
    int						latency )
{
	if ( sTransInfo->pingCallback )
    {
		u8 aid = DWCi_GetConnectionAID( connection );

        sTransInfo->pingCallback( latency, aid );
    }
}

/** -----------------------------------------------------------------
  一度に送信できなかったデータがある場合は、ここで送信する
  -------------------------------------------------------------------*/
void
DWCi_TransportProcess( void )
{
	u8* aidList;
	s32 hostCount;
	s32 i;

	if ( ! sTransInfo )
    {
		return;
    }

	hostCount = DWC_GetAIDList( &aidList );

    for ( i = 0; i < hostCount ; i++ )
    {
		u8 aid;

		aid = aidList[i];

		// 受信タイムアウト時間のチェック
		if( DWC_IsValidAID( aid ) )
		{
		    DWCTransportConnection*	transConnection;
			transConnection = DWCs_GetTransConnection( aid );

			if ( sTransInfo->recvTimeoutCallback && (transConnection->recvTimeoutTime > 0) )
			{
				u32 time;
				OSTick currentTick;

				currentTick = OS_GetTick();
				time = (u32)OS_TicksToMilliSeconds( currentTick - transConnection->previousRecvTick );
				if ( time > transConnection->recvTimeoutTime )
				{
					DWC_Printf( DWC_REPORTFLAG_RECV_INFO, "DWCi_TransportProcess:timeout aid=%d,time=%d[ms],timeout time=%d[ms]\n", aid, time, transConnection->recvTimeoutTime );
					sTransInfo->recvTimeoutCallback( aid );

					//時間をリセットする
					transConnection->previousRecvTick = currentTick;
				}
			}
		}

        // 送信中のデータがある
		if( aid != DWC_GetMyAID() && DWCs_GetSendState( aid ) == DWC_TRANSPORT_SEND_BUSY )
		{
			s32 restSize;
			s32 sendSize;
			s32 freeSpace;
		    DWCTransportConnection*	transConnection;

            transConnection = DWCs_GetTransConnection( aid );

			// 未送信サイズを求める            
            restSize = transConnection->requestSendSize - transConnection->sendingSize; 

			// 送信サイズを決める
			if( restSize > sTransInfo->sendSplitMax ){
				sendSize = sTransInfo->sendSplitMax;
			}else{
				sendSize = restSize;
			}

            // OutgoingBufferの空きサイズを確認する
			freeSpace = DWCs_GetOutgoingBufferFreeSize( aid );
			if ( freeSpace < sendSize )
            {
				DWC_Printf( DWC_REPORTFLAG_SEND_INFO, "DWCi_TransportProcess:freeSpace < sendSize:aid:%d, %d < %d\n", aid, freeSpace, sendSize );
                continue;
            }

            DWCs_Send( aid, (u8*) transConnection->sendBuffer + transConnection->sendingSize, sendSize, TRUE );

		    // 送信済みサイズを更新            
			transConnection->sendingSize += sendSize;

			DWC_Printf(
				DWC_REPORTFLAG_SEND_INFO,
				"DWCi_TransportProcess:aid = %d, size = %d/%d, outgoing buffer = %d, vcont = %d\n",
				aid, transConnection->sendingSize, transConnection->requestSendSize, freeSpace - sendSize, OS_GetVBlankCount()
			);

			if ( transConnection->sendingSize == transConnection->requestSendSize )
			{
                // [arakit] main 051026
                int reqSendSize = transConnection->requestSendSize;
                
                transConnection->sendState			= DWC_TRANSPORT_SEND_READY;
				transConnection->sendBuffer			= NULL;
                transConnection->sendingSize		= 0;	
				transConnection->requestSendSize	= 0;
                // [arakit] main 051026
                
				// 送信完了コールバック
				if ( sTransInfo->sendCallback )
				{
                    // [arakit] main 051026
                    sTransInfo->sendCallback( reqSendSize, aid );
				}
			}			
        }

#ifndef	SDK_FINALROM
//nakatat
#if 0
		{
			OSTick now = OS_GetTick();
			// 遅延発生をチェックする
			if ( sTransInfo->connections[aid].sendDelay )        
			{
				DWCs_DelayCheck( sTransInfo->delayedSend, now, TRUE );
			}

			if ( sTransInfo->connections[aid].recvDelay )        
			{
				DWCs_DelayCheck( sTransInfo->delayedRecv, now, FALSE );
			}
		}
#endif
#endif // SDK_FINALROM
    }

#ifndef	SDK_FINALROM
//nakatat
	{
		OSTick now = OS_GetTick();
		DWCs_DelayCheck( sTransInfo->delayedSend, now, TRUE );
		DWCs_DelayCheck( sTransInfo->delayedRecv, now, FALSE );
	}
#endif // SDK_FINALROM
}

/** -----------------------------------------------------------------
  コネクション管理情報をクリア
  -------------------------------------------------------------------*/
void
DWCi_ClearTransConnection( u8 aid )
{

    if ( !sTransInfo ) return ;

#if 1  // ひとまずバッファはクリアしない
    sTransInfo->connections[aid].sendingSize     = 0;
	sTransInfo->connections[aid].recvingSize     = 0;
    sTransInfo->connections[aid].requestSendSize = 0;
    sTransInfo->connections[aid].requestRecvSize = 0;
    sTransInfo->connections[aid].sendState       = 0;
	//sTransInfo->connections[aid].recvState       = 0;
    if ( sTransInfo->connections[aid].recvState != DWC_TRANSPORT_RECV_NOBUF )
    {
        sTransInfo->connections[aid].recvState = DWC_TRANSPORT_RECV_HEADER;
    }
    sTransInfo->connections[aid].lastRecvType    = 0;
#else
    MI_CpuClear8( &sTransInfo->connections[aid], sizeof( DWCTransportConnection ) );
#endif
}

/** -----------------------------------------------------------------
  トランスポート制御オブジェクトをクリア
  -------------------------------------------------------------------*/
void
DWCi_ShutdownTransport( void )
{
#ifndef	SDK_FINALROM
    // 遅延発生に使用する配列を開放。
	ArrayFree( sTransInfo->delayedSend );
	ArrayFree( sTransInfo->delayedRecv );
#endif	// SDK_FINALROM

    sTransInfo = NULL;
}

/** -----------------------------------------------------------------
  AidからDWCTransportConnectionを取得
  -------------------------------------------------------------------*/
static DWCTransportConnection*
DWCs_GetTransConnection( u8 aid )
{
	SDK_ASSERT( ( aid >= 0 ) && ( aid < DWC_MAX_CONNECTIONS ) );
    
    return &sTransInfo->connections[ aid ];
}

/** -----------------------------------------------------------------
  送信状態を取得
  -------------------------------------------------------------------*/
static int
DWCs_GetSendState( u8 aid )
{
	return sTransInfo->connections[ aid ].sendState;
}

/** -----------------------------------------------------------------
  受信状態を取得
  -------------------------------------------------------------------*/
static int
DWCs_GetRecvState( u8 aid )
{
	return sTransInfo->connections[ aid ].recvState;
}

/** -----------------------------------------------------------------
  gt2Sendのラップ関数　(送信先をAidで指定する)
  -------------------------------------------------------------------*/
void
DWCs_Send(
    u8						aid,
    const u8*				buffer,
    int 					size,
    BOOL					reliable )
{
    GT2Connection connection = DWCi_GetGT2Connection( aid );

    gt2Send( connection, buffer, size, reliable );
}

/** -----------------------------------------------------------------
  Reliable送信されたデータを受信
  -------------------------------------------------------------------*/
static void
DWCs_HandleReliableMessage( 
	GT2Connection			connection, 
	u8*						message, 
	int						size )
{
	u8	aid			= DWCi_GetConnectionAID( connection );
    u16 type;

    switch ( DWCs_GetRecvState( aid ) )
    {
    case DWC_TRANSPORT_RECV_NOBUF:
        type = DWCs_DecodeHeader( (DWCTransportHeader*) message );
        if ( type >= DWC_SEND_TYPE_MATCH_SYN &&
             type <= DWC_SEND_TYPE_MATCH_ACK )
        {
            // 受信バッファが設定されていないくても、システムデータヘッダなら
            // ヘッダ扱いで受信する
            DWCs_RecvDataHeader( aid, (DWCTransportHeader*) message, size );
        }
        else
        {
        	// 受信バッファが設定されていないので、受信したデータを捨てる
			DWC_Printf( DWC_REPORTFLAG_RECV_INFO, "+++ Recv buffer is not set\n" );
        }
        break;
    case DWC_TRANSPORT_RECV_HEADER:
		// ヘッダー（送信されてくる予定のサイズ）を受信
        DWCs_RecvDataHeader( aid, (DWCTransportHeader*) message, size );	
        break;
    case DWC_TRANSPORT_RECV_BODY:
		// データ本体を受信
        DWCs_RecvDataBody( aid, message, size );
        break;
    case DWC_TRANSPORT_RECV_SYSTEM_DATA:
        // システムデータ（内部使用データ）を受信
        DWCs_RecvSystemDataBody ( aid, message, size );
        break;
    case DWC_TRANSPORT_RECV_ERROR:
		// データを捨てる
		DWC_Printf( DWC_REPORTFLAG_RECV_INFO, "+++ Recv size is too large ( buffer size = %d < %d )\n", sTransInfo->connections[ aid ].recvBufferSize, size );
        // ヘッダー待ち状態に戻す		
		sTransInfo->connections[ aid ].recvState		= DWC_TRANSPORT_RECV_HEADER; 
		sTransInfo->connections[ aid ].recvingSize		= 0;
		sTransInfo->connections[ aid ].requestRecvSize	= 0;
        break;
    default:
		//OS_Panic( "[DWC] Recv error (state is %d)",  DWCs_GetRecvState( aid ) );
        DWC_Printf( DWC_REPORTFLAG_ERROR, "Recv error (state is %d).\n", DWCs_GetRecvState( aid ) );
        DWCi_SetError( DWC_ERROR_NETWORK, DWC_ECODE_SEQ_ETC + DWC_ECODE_GS_GT2 + DWC_ECODE_TYPE_TRANS_HEADER );
        break;
    }
}

/** -----------------------------------------------------------------
  Unreliable送信されたデータを受信
  -------------------------------------------------------------------*/
static void
DWCs_HandleUnreliableMessage( 
	GT2Connection			connection, 
	void*					message, 
	int						size )
{
	u8 aid = DWCi_GetConnectionAID( connection );
	DWCTransportConnection* transConnection = &sTransInfo->connections[ aid ];

	if ( transConnection->recvBuffer &&
		 transConnection->recvBufferSize >= size )
	{
		// 受信コールバック
		if ( sTransInfo->recvCallback )
		{
			sTransInfo->recvCallback( aid, message, size );
		}

		if ( sTransInfo->recvTimeoutCallback && (transConnection->recvTimeoutTime > 0) )
		{
			transConnection->previousRecvTick = OS_GetTick();
		}
	}
	else
	{
		DWC_Printf( DWC_REPORTFLAG_RECV_INFO, "Recv data size is too large (%d > %d)\n", size, transConnection->recvBufferSize );
		return;
	}
}

/** -----------------------------------------------------------------
  Reliable送信されたデータのヘッダーを取得
  -------------------------------------------------------------------*/
static void
DWCs_RecvDataHeader( 
	u8						aid,
	DWCTransportHeader*		message,
	int						size )
{
    u16 type;
	DWCTransportConnection*	connection = &sTransInfo->connections[ aid ];
	DWCTransportHeader		header;

    // ヘッダ受信前のrecvStateを記録
    connection->lastRecvState = (u8)DWCs_GetRecvState( aid );

    // ヘッダーを解析する
    switch ( type = DWCs_DecodeHeader( message ) )
    {
    case DWC_SEND_TYPE_USERDATA:
		// DWC_SEND_TYPE_USERDATAはユーザーが送信したデータ
 		if ( size != sizeof( DWCTransportHeader ) )
 		{
			DWC_Printf( DWC_REPORTFLAG_RECV_INFO, "+++ Invalid header from aid %d\n", aid );
	        return;
	    }
        
        MI_CpuCopy8( message, &header, sizeof(DWCTransportHeader) );

        connection->requestRecvSize	= header.size;	// 送信されてくる予定のサイズを取得
        connection->recvingSize		= 0;			// 受信中サイズを0に戻す

        // 適切なサイズの受信バッファが設定されていれば受信する
        if ( connection->recvBuffer &&
             connection->recvBufferSize >= connection->requestRecvSize )
        {
            connection->recvState = DWC_TRANSPORT_RECV_BODY;
        }
        else
        {
            connection->recvState = DWC_TRANSPORT_RECV_ERROR;
        }
        break;

    case DWC_SEND_TYPE_MATCH_SYN:
    case DWC_SEND_TYPE_MATCH_SYN_ACK:
    case DWC_SEND_TYPE_MATCH_ACK:
        // マッチメイク完了同期調整ヘッダを受信
        DWC_Printf( DWC_REPORTFLAG_RECV_INFO, "Received system header.\n" );
        connection->recvState = DWC_TRANSPORT_RECV_SYSTEM_DATA;
        break;

    default:
		DWC_Printf( DWC_REPORTFLAG_RECV_INFO, "+++ Invalid header from aid %d\n", aid );
        break;
    }

    // 最後に受信したヘッダタイプを記録
    connection->lastRecvType = type;
}

/** -----------------------------------------------------------------
  Reliable送信されたデータの本体を受信
  -------------------------------------------------------------------*/
static void
DWCs_RecvDataBody( 
	u8						aid,
	void*					message,
	int						size )
{
	DWCTransportConnection*	connection;
	int						requestSize;
    
	connection = &sTransInfo->connections[ aid ];

	if ( DWCs_GetRecvState( aid ) == DWC_TRANSPORT_RECV_BODY )
	{		
	    // オーバフローのチェック
        if ( connection->recvingSize + size > connection->recvBufferSize )
        {
            //OS_Panic( "[DWC] Recv buffer over flow\n" );
            DWC_Printf( DWC_REPORTFLAG_ERROR, "Recv buffer over flow.\n" );
            DWCi_SetError( DWC_ERROR_NETWORK, DWC_ECODE_SEQ_ETC + DWC_ECODE_GS_GT2 + DWC_ECODE_TYPE_TRANS_BODY );

            return;
        }

        MI_CpuCopy8( message, (u8 *)connection->recvBuffer + connection->recvingSize, (u32) size );
    }

    connection->recvingSize += size;

    DWC_Printf( DWC_REPORTFLAG_RECV_INFO, "aid = %d size = %d/%d state = %d incoming buffer = %d\n",
                						  aid,
                						  connection->recvingSize, connection->requestRecvSize,
										  DWCs_GetRecvState( aid ),
                						  gt2GetIncomingBufferFreeSpace( DWCi_GetGT2Connection( aid ) ) );

    if ( connection->recvingSize == connection->requestRecvSize )
	{
		requestSize = connection->requestRecvSize;

        // ヘッダー待ち状態に戻す		
		connection->recvState		= DWC_TRANSPORT_RECV_HEADER; 
		connection->recvingSize		= 0;
		connection->requestRecvSize	= 0;				

        // 受信完了コールバック
		if ( sTransInfo->recvCallback )
		{
			sTransInfo->recvCallback( aid, connection->recvBuffer, requestSize );
        }
    }

	if ( sTransInfo->recvTimeoutCallback && (connection->recvTimeoutTime > 0) )
	{
		connection->previousRecvTick = OS_GetTick();
	}
}

/** -----------------------------------------------------------------
  Reliable送信されたシステムデータの本体を受信
  -------------------------------------------------------------------*/
static void
DWCs_RecvSystemDataBody( 
	u8						aid,
	void*					message,
	int						size )
{
#pragma unused( size )
    DWCTransportConnection*	transConnection = DWCs_GetTransConnection( aid );

    // recvStateを、システムデータ受信前の状態に戻す。
    // コールバック内でDWC_SetRecvBuffer()を呼ばれることもあるので、
    // ここで戻す必要がある。
    transConnection->recvState = transConnection->lastRecvState;

    switch ( transConnection->lastRecvType )
    {
    case DWC_SEND_TYPE_MATCH_SYN:
    case DWC_SEND_TYPE_MATCH_SYN_ACK:
    case DWC_SEND_TYPE_MATCH_ACK:
        // マッチメイク同期調整データ受信時
        DWCi_ProcessMatchSynPacket( aid, transConnection->lastRecvType, (u8 *)message );
        break;
    }
}

/** -----------------------------------------------------------------
  遅延を発生させたデータがあるなら送受信する
  -------------------------------------------------------------------*/
static void
DWCs_DelayCheck(
    DArray				delayedMessages,
    OSTick				now,
    BOOL				send )
{
    DWCDelayedMessage*	msg;
	int					i;	
	int					num;
	int					freeSpace;
	u8					aid;
    
//nakatat
	if( delayedMessages == NULL ){
		return;
	}

	num = ArrayLength( delayedMessages );

	for ( i = ( num - 1 ); i >= 0; --i )
    {
		msg = (DWCDelayedMessage*) ArrayNth( delayedMessages, i );

        // startTimeから過ぎた時間がdelayTimeより大きくなったら送受信する
		if ( OS_TicksToMilliSeconds( now - msg->startTime ) > msg->delayTime )
		{
			if ( send )
            {
//nakatat
#if 0
				aid = DWCi_GetConnectionAID( msg->connection );
                if ( ! DWCi_IsValidAID( aid ) )
#else
				if( gt2GetConnectionState( msg->connection ) != GT2Connected )
#endif
                {
					ArrayDeleteAt( delayedMessages, i );
					continue;	
                }
                
#if 1
				aid = DWCi_GetConnectionAID( msg->connection );
#endif
                freeSpace = DWCs_GetOutgoingBufferFreeSize( aid );				

			    if ( freeSpace > msg->size )
			    {
	                // 送信
	                gt2FilteredSend( msg->connection, msg->filterID, msg->message, msg->size, msg->reliable );
			    }
                else
                {
					return;
                }
            }
            else
            {
                // 受信
                gt2FilteredReceive( msg->connection, msg->filterID, msg->message, msg->size, msg->reliable );
            }

            // 送受信したデータを削除
			ArrayDeleteAt( delayedMessages, i );
        }
        else
        {
			return;
        }
    }
}

/** -----------------------------------------------------------------
  遅延発生用のバッファに送受信データを格納する
  -------------------------------------------------------------------*/
static void
DWCs_AddDelayMessage(
	DArray				array,
    u16					delayValue,
    GT2Connection		connection,
	int					filterID,
    const u8*			message,
    int					size,
    BOOL				reliable )
{     
    DWCDelayedMessage	msg;

    MI_CpuClear8( &msg, sizeof( DWCDelayedMessage ) );

    msg.connection	= connection;
    msg.message		= gsimalloc( (u32) size );
	//SDK_ASSERT( msg.message );
    if ( msg.message == NULL )
    {
        DWCi_SetError( DWC_ERROR_FATAL, DWC_ECODE_SEQ_ETC + DWC_ECODE_GS_GT2 + DWC_ECODE_TYPE_ALLOC );
        return;
    }
    
	MI_CpuCopy8( message, msg.message, (u32) size );
	msg.filterID	= filterID;		
    msg.size		= size;
    msg.reliable	= reliable;
    msg.startTime	= OS_GetTick();
    msg.delayTime	= delayValue;

    // 配列の先頭に追加
	ArrayInsertAt( array, &msg, 0 );    
}

/** -----------------------------------------------------------------
  配列開放
  -------------------------------------------------------------------*/
static void
DWCs_DelayedMessageFree( void* elem )
{
	DWCDelayedMessage* message = (DWCDelayedMessage*) elem;

    gsifree( message->message );
}

/** -----------------------------------------------------------------
  Outgoing Bufferの空きサイズを取得
  -------------------------------------------------------------------*/
static int
DWCs_GetOutgoingBufferFreeSize( u8 aid )
{
	static const int	gamespyUseSize	= 512; // 送信以外で使用する分も確保する必要があるみたい
	GT2Connection		connection		= DWCi_GetGT2Connection( aid );
    int					free;

    SDK_ASSERT( connection );

    free = gt2GetOutgoingBufferFreeSpace( connection ) - DWC_TRANSPORT_GT2HEADER_SIZE - gamespyUseSize;

    return	( free > 0 ) ? free : 0;
}

#ifndef	SDK_FINALROM

/** -----------------------------------------------------------------
  送信パケットロスを発生させる
  -------------------------------------------------------------------*/
static void
DWCs_SendFilterCallbackDrop(
    GT2Connection		connection,
    int					filterID,
    const GT2Byte * 	message,
    int					size,
    GT2Bool 			reliable )
{
    if ( ! reliable && MATH_Rand16(  &sTransInfo->context, 100 ) < sTransInfo->sendDrop ) 
    {
		// 送信せずに終了する
        DWC_Printf( DWC_REPORTFLAG_SEND_INFO, "Packet loss\n" );
        return;				
    }

	gt2FilteredSend( connection, filterID, message, size, reliable );
}

/** -----------------------------------------------------------------
  受信パケットロスを発生させる
  -------------------------------------------------------------------*/
static void
DWCs_RecvFilterCallbackDrop(
    GT2Connection		connection,
    int					filterID,
    GT2Byte*		 	message,
    int					size,
    GT2Bool 			reliable )
{
    if ( ! reliable && MATH_Rand16(  &sTransInfo->context, 100 ) < sTransInfo->recvDrop ) 
    {
		// 受信せずに終了する
        DWC_Printf( DWC_REPORTFLAG_RECV_INFO, "Packet loss\n" );
        return;				
    }

	gt2FilteredReceive( connection, filterID, message, size, reliable );
}

/** -----------------------------------------------------------------
  送信に遅延を発生させる
  -------------------------------------------------------------------*/
static void
DWCs_SendFilterCallbackDelay(
    GT2Connection		connection,
    int					filterID,
    const u8*	 		message,
    int					size,
    BOOL				reliable )
{
	u8	aid = DWCi_GetConnectionAID( connection );

    DWCs_AddDelayMessage( sTransInfo->delayedSend, sTransInfo->connections[aid].sendDelay,  connection, filterID, message, size, reliable );
}

/** -----------------------------------------------------------------
  受信に遅延を発生させる
  -------------------------------------------------------------------*/
static void
DWCs_RecvFilterCallbackDelay(
    GT2Connection		 connection,
    int					 filterID,
    u8*					 message,
    int					 size,
    BOOL				 reliable )
{
	u8	aid = DWCi_GetConnectionAID( connection );
    
	DWCs_AddDelayMessage( sTransInfo->delayedRecv, sTransInfo->connections[aid].recvDelay, connection, filterID, message, size, reliable );
}

#endif	// SDK_FINALROM


