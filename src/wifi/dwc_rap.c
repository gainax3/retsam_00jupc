//=============================================================================
/**
 * @file	dwc_rap.c
 * @bfief	DWCラッパー。オーバーレイモジュールに置くもの
 * @author	kazuki yoshihara
 * @date	06/02/23
 */
//=============================================================================

#include "common.h"
#include "communication/communication.h"
#include "communication/comm_def.h"
#include "communication/comm_state.h"
#include "communication/comm_local.h"
#include "communication/comm_system.h"
#include "wifi/dwc_rap.h"
#include "wifi/vchat.h"
#include "wifi/dwc_lobbylib.h"
#include "system/pm_debug_wifi.h"
#include "savedata/frontier_savedata.h"
#include <vct.h>


// 何秒間通信が届かないとタイムアウトと判断するか
#define MYDWC_TIMEOUTSEC (10)

// ランダムマッチにおいて、指定人数に届かない場合に進行を進める為の時間
//#define _RANDOMMATCH_TIMEOUT (90)	// 外からしていできるようにした

// 何フレーム送信がないと、KEEP_ALIVEトークンを送るか。
#define KEEPALIVE_TOKEN_TIME 120

// ボイスチャットを利用する場合は定義する。
#define MYDWC_USEVCHA


//#define YOSHIHARA_VCHAT_ONOFFTEST


// ヒープ領域の最大使用量を監視する場合定義
//#define CHEAK_HEAPSPACE

// デバッグ出力を大量に吐き出す場合定義
#if defined(DEBUG_ONLY_FOR_ohno) | defined(DEBUG_ONLY_FOR_tomoya_takahashi)
#define DEBUGPRINT_ON
#endif


#ifdef DEBUGPRINT_ON
#define MYDWC_DEBUGPRINT(...) \
    (void) ((OS_TPrintf(__VA_ARGS__)))
#else
#define MYDWC_DEBUGPRINT(...)           ((void) 0)
#endif



// テスト用サーバに繋ぐときに定義
#ifdef PM_DEBUG
#define USE_AUTHSERVER_DEVELOP
#endif

// フレンドリストのサイズ
#define FRIENDLIST_MAXSIZE 32

// １フレームに何人分のデータを更新するか。
//#define FRIENDINFO_UPDATA_PERFRAME (4)
#define FRIENDINFO_UPDATA_PERFRAME (1)	// 080708 処理不可軽減のため

// WiFiで使うHeapのサイズ(128Kバイト、仮）+7000
//-----#if TESTOHNO 
//#define MYDWC_HEAPSIZE (0x2B000)
//#define MYDWC_HEAPSIZE (0x2C000)
//#define MYDWC_HEAPSIZE (0x2B000)

//#define MYDWC_HEAPID HEAPID_WIFIMENU
//-----#endif //TESTOHNO

// この辺はテスト用。正式に割り当てられたら、指定する。
#define GAME_NAME        "pokemondpds"  // 使用するゲーム名
#define GAME_SECRET_KEY  "1vTlwb"  // 使用するシークレットキー
#define GAME_PRODUCTID   10727         // 使用するプロダクトID

#define SIZE_RECV_BUFFER (4 * 1024)
#define SIZE_SEND_BUFFER 256

#define _MATCHSTRINGNUM (128)

// ４人用WIFI通信時に臨時で確保するHEAP
#define  _EXTRA_HEAPSIZE		(0xf000)
#define  _EXTRA_HEAP_GROUPID	(16)


typedef struct
{
	u8 sendBuffer[ SIZE_SEND_BUFFER ];
//	u8* recvBuffer;
	DWCFriendData *keyList;  // DWC形式の友達リスト	
	DWCFriendsMatchControl stDwcCnt;    // DWC制御構造体	
    DWCUserData *myUserData;		// DWCのユーザデータ（自分のデータ）
	DWCInetControl stConnCtrl;
    SAVEDATA* pSaveData;   // 2006.04.07 k.ohno  セーブデータを保持
	
	void *orgPtr;  //32バイトアライメントしていない自分のポインタ
    void *recvPtr[COMM_MODE_CLUB_WIFI_NUM_MAX+1];  //受信バッファの32バイトアライメントしていないポインタ
    void *heapPtr;
	NNSFndHeapHandle headHandle;
    void *heapPtrEx;
	NNSFndHeapHandle headHandleEx;
    u32 heapSizeEx;
    
	MYDWCReceiverFunc serverCallback;
	MYDWCReceiverFunc clientCallback;
	MYDWCDisconnectFunc disconnectCallback;
    void* pDisconnectWork;
    MYDWCConnectFunc connectCallback;
    void* pConnectWork;
    MYDWCConnectModeCheckFunc connectModeCheck;
    
	void (*fetalErrorCallback)(int);
	u8 randommatch_query[_MATCHSTRINGNUM];
	u8 friend_status[FRIENDLIST_MAXSIZE];
	void *friendinfo;	
	int infosize;	
	u32 friendupdate_index;	
	
	int state;
	int matching_type;
	
	int sendbufflag;
	
//	int op_aid;			// 相手のaid
    int maxConnectNum;  //最大接続人数
    u32 backupBitmap;
	
	int heapID;
    int recvHeapID;   //受信バッファ用HEAPID
//-----#if TESTOHNO 
    int heapSize;
#ifdef CHEAK_HEAPSPACE
    int _heapspace;
    int _heapmaxspace;
#endif
//-----#endif //TESTOHNO
	int isvchat;
	int friendindex;   // 今から接続するフレンドのリストインデックス
	int newFriendConnect;  // 接続してきたらTRUE 2006.05.24 k.ohno
    BOOL bVChat;     // VCHATONOFF
	BOOL bConnectCallback;  
	int vchatcodec;
	
	int timeoutflag;
	
	int sendintervaltime[COMM_MACHINE_MAX];		// 前回データを送信してからのフレーム数。
    int setupErrorCount;  //エラーした数をカウント
	
	int opvchaton;				// 相手のボイスチャットがオンかオフか
	u16 myvchaton;				// 自分のボイスチャットがオンかオフか
	u16 myvchat_send;			// 音声データ送信フラグ
	
    u32 BlockUse_BackupBitmap;
	
	u8 myseqno;				// 自分が送信するパケットのシーケンスNo
	u8 opseqno;				// 相手が最後に送ってきたパケットのシーケンスNo
    u8 bHeapError;  // HEAP確保失敗の場合
    u8 pausevchat; //vchat一時停止
    u8 blockClient;   // クライアントを接続禁止にする
    u8 bRecvPtrWorld[COMM_MODE_CLUB_WIFI_NUM_MAX+1];  // HEAPをどこから確保したのかを記憶

	u8 closedflag;		// ConnectionClosedCallback でホスト数が1になったら切断処理に遷移するのか　TRUEで切断処理に遷移　080602 tomoya
    u8 saveing;  //セーブ中に1
    
} MYDWC_WORK;

// 親機のAID
#define _WIFI_PARENT_AID (0)

enum  _blockStatus{
    _BLOCK_NONE,
    _BLOCK_START,
    _BLOCK_CALLBACK,
};

// ボイスチャットのトークンと混合しないようにするため、
// ボイスチャットと違う３２バイトの数字をパケットの先頭につける。
#define MYDWC_PACKETYPE_MASK 0x000000ff
#define MYDWC_PACKET_VCHAT_MASK 0x00000100
#define MYDWC_PACKET_VCHAT_SHIFT 8
#define MYDWC_PACKET_SEQNO_POS 2

#define MYDWC_GAME_PACKET 0x0001

// タイムアウト処理を防ぐため、一定時間送信がない場合、
// からのデータを送る。そのときの数字
#define MYDWC_KEEPALIVE_PACKET 0x0002

enum{
	MDSTATE_INIT,	
	MDSTATE_CONNECTING,
	MDSTATE_CONNECTED,
	MDSTATE_TRYLOGIN,
	MDSTATE_LOGIN,
	
	MDSTATE_MATCHING,
	MDSTATE_CANCEL,	
	MDSTATE_MATCHED,	
	MDSTATE_PLAYING,	
	MDSTATE_CANCELFINISH,	
	
	MDSTATE_FAIL,			// 2006.7.4 yoshihara追加
	MDSTATE_FAILFINISH,	
	
	MDSTATE_ERROR,
	MDSTATE_ERROR_FM,
	MDSTATE_ERROR_DISCONNECT,
	MDSTATE_ERROR_FETAL,
	
	MDSTATE_DISCONNECTTING,	
	MDSTATE_DISCONNECT,	
	
	MDSTATE_TIMEOUT,
	
	MDSTATE_LAST
};

enum {
	MDTYPE_RANDOM,
	MDTYPE_PARENT,
	MDTYPE_CHILD
};
	

static MYDWC_WORK *_dWork = NULL;


static void LoginCallback(DWCError error, int profileID, void *param);
static void UpdateServersCallback(DWCError error, BOOL isChanged, void* param);
static void FriendStatusCallback(int index, u8 status, const char* statusString, void* param);
static void DeleteFriendListCallback(int deletedIndex, int srcIndex, void* param);
static void BuddyFriendCallback(int index, void* param);

static void ConnectToAnybodyCallback(DWCError error, BOOL cancel, void* param);
static void SendDoneCallback( int size, u8 aid );
static void UserRecvCallback( u8 aid, u8* buffer, int size );
static void ConnectionClosedCallback(DWCError error, BOOL isLocal, BOOL isServer, u8  aid, int index, void* param);
//static int handleError();
static int EvaluateAnybodyCallback(int index, void* param);
static int mydwc_step(void);

static void recvTimeoutCallback(u8 aid);
static BOOL _isSendableReliable(void);
static void mydwc_releaseRecvBuff(int aid);
static void mydwc_releaseRecvBuffAll(void);
static void mydwc_allocRecvBuff(int i);

static void mydwc_updateFriendInfo( );

#ifdef _WIFI_DEBUG_TUUSHIN

#define _USER_PARAM_MAGIC (55)

static void _NNSFndHeapVisitor(void* memBlock, NNSFndHeapHandle heap, u32 userParam)
{
    if(userParam == _USER_PARAM_MAGIC){
        GF_ASSERT(NNS_FndCheckExpHeap(heap, NNS_FND_HEAP_ERROR_PRINT));
        GF_ASSERT(NNS_FndCheckForMBlockExpHeap(memBlock, heap, NNS_FND_HEAP_ERROR_PRINT));
    }
}

#endif


//==============================================================================
/**
 * インターネットへ接続開始
 * @param   pSaveData      セーブデータへのポインタ
 * @param   heapID         WIFI用HEAPID
 * @param   maxConnectNum  最大接続人数  
 * @retval  MYDWC_STARTCONNECT_OK … OK
 * @retval  MYDWC_STARTCONNECT_FIRST … 初めて接続する場合。（メッセージ表示の必要有
 * @retval  MYDWC_STARTCONNECT_DIFFERENTDS … 異なるＤＳで接続しようしてる場合。（要警告）
 */
//==============================================================================
//-----#if TESTOHNO 
int mydwc_startConnect(SAVEDATA* pSaveData, int heapID, int heapSize, int maxConnectNum)
//-----#endif //TESTOHNO
{
    void* pTemp;

    // ヒープ領域からワーク領域を確保。
	GF_ASSERT( _dWork == NULL );
#ifdef DEBUGPRINT_ON
	DWC_SetReportLevel(DWC_REPORTFLAG_ALL);
#else
	DWC_SetReportLevel(0);
#endif
    
    OHNO_PRINT("mydwc_start %d %d\n",sizeof(MYDWC_WORK) + 32,maxConnectNum);
    MYDWC_DEBUGPRINT("dwcrasp   %d %d\n",sizeof(MYDWC_WORK) + 32,maxConnectNum);
    
	pTemp = sys_AllocMemory(heapID, sizeof(MYDWC_WORK) + 32);
    MI_CpuClear8(pTemp,sizeof(MYDWC_WORK) + 32);

	_dWork = (MYDWC_WORK *)( ((u32)pTemp + 31) / 32 * 32 );

    _dWork->orgPtr = pTemp;

    _dWork->pSaveData = pSaveData;
	_dWork->serverCallback = NULL;
	_dWork->clientCallback = NULL;	
	_dWork->fetalErrorCallback = NULL;
	_dWork->state = MDSTATE_INIT;
	_dWork->heapID = heapID;
    _dWork->recvHeapID = heapID;   //受信バッファ用HEAPID
//-----#if TESTOHNO 
    _dWork->heapSize = heapSize;
#ifdef CHEAK_HEAPSPACE
    _dWork->_heapspace=0x800000;
    _dWork->_heapmaxspace=0x800000;
#endif
    _dWork->heapPtr = sys_AllocMemory(heapID, heapSize+(SIZE_RECV_BUFFER*3) + 32);  // RECVバッファ分移動
    _dWork->heapPtrEx = NULL;
//----#endif //TESTOHNO
	_dWork->headHandle = NNS_FndCreateExpHeap( (void *)( ((u32)_dWork->heapPtr + 31) / 32 * 32 ), heapSize);
	_dWork->headHandleEx = NULL;

    _dWork->vchatcodec = VCHAT_NONE;
	_dWork->friendindex = -1;
	_dWork->friendupdate_index = 0;
//    _dWork->op_aid = -1;
    _dWork->maxConnectNum = maxConnectNum;
    _dWork->backupBitmap = 0;
    _dWork->BlockUse_BackupBitmap = 0;
    _dWork->newFriendConnect = -1;
    _dWork->bVChat = TRUE;
    _dWork->bHeapError = FALSE;
    _dWork->setupErrorCount = 0;
    
    // 2006.7.22 yoshihara ここで、Seqnoをセット
    _dWork->myseqno = 0;
    _dWork->opseqno = 0;
    _dWork->myvchaton = 1;
    _dWork->opvchaton = 1;
	_dWork->myvchat_send = 1;
    
    // 2006.04.07 k.ohno 引数変更 セーブデータから得る
    if(pSaveData!=NULL){
        _dWork->myUserData = WifiList_GetMyUserInfo(SaveData_GetWifiListData(_dWork->pSaveData));
        _dWork->keyList = (DWCFriendData*)WifiList_GetDwcDataPtr(SaveData_GetWifiListData(_dWork->pSaveData),0);
    }

	// 2008.06.02 tomoya ClosedCallbackで切断処理に遷移するようにするのかをフラグできりかえれるように変更(Wi-Fiクラブ４人募集画面用)
	_dWork->closedflag = TRUE;

#ifdef PM_DEBUG
    DWC_ReportUserData(_dWork->myUserData);
#endif

	_dWork->friendinfo = NULL;
	{
		int i;
		for( i = 0; i < FRIENDLIST_MAXSIZE; i++ )
		{
			_dWork->friend_status[i] = DWC_STATUS_OFFLINE;
		}
	}
	

	// ユーザデータの状態をチェック。
	mydwc_showFriendInfo();	
	

	if( !DWC_CheckHasProfile( _dWork->myUserData ) ) 
	{
		// まだこのデータで一度もＷｉＦｉに繋いでいない。
		return MYDWC_STARTCONNECT_FIRST;
	}
	
	if( !DWC_CheckValidConsole( _dWork->myUserData ) )
	{
		// 本体情報が違う→違うＤＳで接続しようとしている。
		return 	MYDWC_STARTCONNECT_DIFFERENTDS;
	}
	


	return MYDWC_STARTCONNECT_OK;
}

//==============================================================================
/**
 * dwc_rap.cが確保した領域を開放する。
 * @param   none
 * @retval  none
 */
//==============================================================================
void mydwc_free()
{
    if(_dWork){
        mydwc_releaseRecvBuffAll();
        if(_dWork->heapPtrEx!=NULL){
            NNS_FndDestroyExpHeap( _dWork->headHandleEx );
            sys_FreeMemoryEz(_dWork->heapPtrEx);
            _dWork->heapPtrEx = NULL;
		}
        NNS_FndDestroyExpHeap( _dWork->headHandle );
        sys_FreeMemory( _dWork->heapID, _dWork->heapPtr  );
        sys_FreeMemory( _dWork->heapID, _dWork->orgPtr  );
        _dWork = NULL;
    }
	DWC_ClearError();
}

//==============================================================================
/**
 * インターネット接続中に毎フレーム呼び出される関数
 * @param   userdata 自分のログインデータへのポインタ
 * @param   list フレンドリストの先頭ポインタ
 * @retval  正…接続完了。０…接続中。負…エラー（エラーコードが返る）
 */
//==============================================================================
int mydwc_connect()
{
    switch( _dWork->state )
	{
		case MDSTATE_INIT:
		// 初期状態
		{
		    // メモリ確保関数設定
		    DWC_SetMemFunc( mydwc_AllocFunc, mydwc_FreeFunc );
		    
		    // ネット接続初期化
            DWC_InitInetEx(&_dWork->stConnCtrl,COMM_DMA_NO,COMM_POWERMODE,COMM_SSL_PRIORITY);
		    
		    DWC_SetAuthServer(GF_DWC_CONNECTINET_AUTH_TYPE);
			
			// 非同期にネットに接続
			DWC_ConnectInetAsync();
			
			_dWork->state = MDSTATE_CONNECTING;
			_dWork->isvchat = 0;
		}
		
		case MDSTATE_CONNECTING:
		{
			// 安定した状況になるまで待つ。
			if( DWC_CheckInet() )
			{
				if( DWC_GetInetStatus() == DWC_CONNECTINET_STATE_CONNECTED )
				{
					_dWork->state = MDSTATE_CONNECTED;
				}
				else
				{
					_dWork->state = MDSTATE_ERROR;
				}
			}
			else
			{
				// 自動接続処理中
		        DWC_ProcessInet();
  
				break;	
			}
		}
		
		case MDSTATE_CONNECTED:
			// エラーがおこっていないか判定
			{
				int ret = mydwc_HandleError();
                if( ret != 0 ){
                    return ret;
                }
			}

        // フレンドライブラリ初期化
        DWC_InitFriendsMatch(&(_dWork->stDwcCnt), (_dWork->myUserData), 
                             GAME_PRODUCTID, GAME_NAME,
                             GAME_SECRET_KEY, 0, 0,
                             _dWork->keyList, FRIENDLIST_MAXSIZE);
            
		    {// IPLのユーザ名を使ってログイン
		    	// 自分のユーザ名を圧縮。
				OSOwnerInfo info;
				OS_GetOwnerInfo( &info );
			    DWC_LoginAsync( &(info.nickName[0]), NULL, LoginCallback, NULL);       
		    }
		    _dWork->state = MDSTATE_TRYLOGIN;
		
		case MDSTATE_TRYLOGIN:
			// ログインが完了するまで待つ。
			DWC_ProcessFriendsMatch();
			break;
			
		case MDSTATE_LOGIN:
            DWC_ProcessFriendsMatch();   // 2006.04.07 k.ohno ログインしただけの状態を持続する為
			return STEPMATCH_CONNECT;
	}
	return mydwc_HandleError();	
}

//==============================================================================
/**
 * 受信したときのコールバックを設定する関数
 * @param client … クライアントとしてのコールバック関数
 * @param server … サーバとしてのコールバック関数
 * @retval  none
 */
//==============================================================================
void mydwc_setReceiver( MYDWCReceiverFunc server, MYDWCReceiverFunc client )
{
	_dWork->serverCallback = server;
	_dWork->clientCallback = client;	
}

//==============================================================================
/**
 * 切断を検知した時に呼ばれる関数をセットする
 * @param 	pFunc  切断時に呼ばれる関数
 * @param 	pWork  ワークエリア
 * @retval  none
 */
//==============================================================================
void mydwc_setDisconnectCallback( MYDWCDisconnectFunc pFunc, void* pWork )
{
    _dWork->disconnectCallback =pFunc;
    _dWork->pDisconnectWork =pWork;
}


//==============================================================================
/**
 * @brief   接続のコールバック内で判定する関数セット
 * @param 	pFunc  接続時に呼ばれる関数
 * @retval  none
 */
//==============================================================================
void mydwc_setConnectModeCheckCallback( MYDWCConnectModeCheckFunc pFunc )
{
    _dWork->connectModeCheck = pFunc;
}

//==============================================================================
/**
 * 接続を検知した時に呼ばれる関数をセットする
 * @param 	pFunc  接続時に呼ばれる関数
 * @param 	pWork  ワークエリア
 * @retval  none
 */
//==============================================================================
void mydwc_setConnectCallback( MYDWCConnectFunc pFunc, void* pWork )
{
    _dWork->connectCallback =pFunc;
    _dWork->pConnectWork =pWork;
}

//==============================================================================
/**
 * ランダム対戦を行う関数。ランダムマッチ中は数十フレーム処理が返ってこないことがある。
 * @retval  正…成功。０…失敗。
 */
//==============================================================================
//-----#if TESTOHNO 
const static char randommatch_query[] = "%s = \'%s\'";

int mydwc_startmatch( u8* keyStr,int numEntry, BOOL bParent, u32 timelimit )
{

    GF_ASSERT( _dWork != NULL );
    
	if( _dWork->state != MDSTATE_LOGIN ) return 0;
    mydwc_releaseRecvBuffAll();


    {
		int result;
        DWCMatchOptMinComplete moc={TRUE, 2, {0,0}, 0};
		moc.timeout = timelimit*1000;
//        DWCMatchOptMinComplete moc={TRUE, 2, {0,0}, _RANDOMMATCH_TIMEOUT*1000};
//	    if(!bParent){
//            moc.timeout = 1; // 子機に時間の主導権がないように短く設定する
//        }
        result = DWC_SetMatchingOption(DWC_MATCH_OPTION_MIN_COMPLETE,&moc,sizeof(moc));
		GF_ASSERT( result == DWC_SET_MATCH_OPT_RESULT_SUCCESS );
    }
    GF_ASSERT(DWC_AddMatchKeyString(0,PPW_LOBBY_MATCHMAKING_KEY,(const char *)keyStr)!=0);
    {
        MI_CpuClear8(_dWork->randommatch_query,_MATCHSTRINGNUM);
        sprintf((char*)_dWork->randommatch_query,randommatch_query,PPW_LOBBY_MATCHMAKING_KEY,keyStr);
        GF_ASSERT(strlen((const char*)_dWork->randommatch_query) < _MATCHSTRINGNUM);
    }
    if(bParent){
        DWC_AddMatchKeyString(1,(const char*)_dWork->randommatch_query,(const char*)_dWork->randommatch_query);
    }
#if PL_G0197_080710_FIX
	{
		int i;
		for(i=0;i<numEntry; i++){
			mydwc_allocRecvBuff(i);
		}
	}
#endif
        
    _dWork->state = MDSTATE_MATCHING;

    MYDWC_DEBUGPRINT("mydwc_startmatch %d ",numEntry);
    _dWork->maxConnectNum = numEntry;
    
    DWC_ConnectToAnybodyAsync
    (
    	numEntry,
        (const char*)_dWork->randommatch_query,
        ConnectToAnybodyCallback,
        NULL,
        EvaluateAnybodyCallback,
        NULL
    );
	_dWork->matching_type = MDTYPE_RANDOM;
    // 送信コールバックの設定	
    DWC_SetUserSendCallback( SendDoneCallback ); 

    // 受信コールバックの設定	
    DWC_SetUserRecvCallback( UserRecvCallback ); 
    
    // コネクションクローズコールバックを設定
    DWC_SetConnectionClosedCallback(ConnectionClosedCallback, NULL);
    
    // タイムアウトコールバックの設定
    DWC_SetUserRecvTimeoutCallback( recvTimeoutCallback );
    
    _dWork->sendbufflag = 0;

	_dWork->closedflag = TRUE;	// 080602 tomoya
    return 1;    
}
//----#endif //TESTOHNO

// 2006.7.4 yoshihara 追加
static void finishcancel()
{
    if( _dWork->state == MDSTATE_FAIL ){
		_dWork->state = MDSTATE_FAILFINISH;
    }
    else
    {
		_dWork->state = MDSTATE_CANCELFINISH;
    }	
}

//==============================================================================
/**
 * ランダム対戦マッチング中に毎フレーム呼ばれる関数。
 * @retval  
 	STEPMATCH_CONTINUE…マッチング中
 	STEPMATCH_SUCCESS…成功
 	STEPMATCH_CANCEL…キャンセル
 	STEPMATCH_FAIL  …相手が親をやめたため、接続を中断
 	負…エラー発生
 */
//==============================================================================

int mydwc_stepmatch( int isCancel )
{
//	MYDWC_DEBUGPRINT( "closedflag = %d\n", _dWork->closedflag );
    //OS_TPrintf("mydwc_stepmatch %d\n",_dWork->state);
	switch ( _dWork->state )
	{
		case MDSTATE_MATCHING:
			// 現在探索中
			if( isCancel ) 
			{
				_dWork->state = MDSTATE_CANCEL;
			}
			// 2006.7.4 yoshihara修正 ここから
			// 子機の場合、つなぎにいってる親が現在もサーバモードかどうかチェックする。
			if( _dWork->matching_type == MDTYPE_CHILD )
			{
				if( _dWork->friendindex >= 0 ) // このifは念のため
				{
					if( _dWork->friend_status[_dWork->friendindex] != DWC_STATUS_MATCH_SC_SV )
					{
						MYDWC_DEBUGPRINT("相手がサーバをやめてしまったので、キャンセルします。\n");
						// 既に親でなくなってしまっている。キャンセルへ移項
						_dWork->state = MDSTATE_FAIL;	
					}
				}
			}
			// 2006.7.4 yoshihara修正 ここまで
			
			break;
			
      case MDSTATE_CANCEL:
      case MDSTATE_FAIL:      
			if( _dWork->matching_type == MDTYPE_RANDOM)  //参照先が間違っていたので修正 07.12.06 k.ohno
			{
				// ランダムマッチ
				// キャンセル処理中
				MYDWC_DEBUGPRINT("ランダムマッチ	CANCEL処理＝＝＝＝＝＝\n");
				if( !DWC_CancelMatching() )
				{
				}
				break;
			} else {
				int ret;

                ret = DWC_CloseAllConnectionsHard();
                finishcancel();	  // コールバック内でSTATEを変えないように修正した為 RETURNに関係なく状態変更する
#if 0
                if( ret == 1 )
				{
					// 子機がいない→コールバック無しで終了
		            // 2006.7.4 yoshihara 修正
					finishcancel();	
				}
				else if (ret == 0)
				{
					// コールバックが帰ってきているはず
					finishcancel();	
				}
				else
				{
		            // 2006.7.4 yoshihara 修正
					finishcancel();	
//					MYDWC_DEBUGPRINT("Now unable to disconnect.\n");
				}
#endif
			}
        break;    //   06.05.12追加
		case MDSTATE_MATCHED:
			// 完了。
			{
#ifdef MYDWC_USEVCHA
                if(!CommLocalIsWiFiQuartetGroup(CommStateGetServiceNo())){   // ４人接続の時はボイスチャットを自動起動しない
                    mydwc_startvchat(_dWork->heapID);
                }
                if(_dWork->bVChat){
	                _dWork->myvchaton = 1;
                }
                else{
	                _dWork->myvchaton = 0;
                }

#endif
                _dWork->state = MDSTATE_PLAYING;
                return STEPMATCH_SUCCESS;	
			}
			break;
		case MDSTATE_CANCELFINISH:
			// ログイン直後の状態に
			_dWork->state = MDSTATE_LOGIN;
			_dWork->sendbufflag = 0;
            _dWork->newFriendConnect = -1;
            MYDWC_DEBUGPRINT("キャンセル処理完了\n");
			return STEPMATCH_CANCEL;
		case MDSTATE_FAILFINISH:
			// ログイン直後の状態に
			_dWork->state = MDSTATE_LOGIN;
			_dWork->sendbufflag = 0;
            _dWork->newFriendConnect = -1;
            MYDWC_DEBUGPRINT("フィニッシュ処理完了\n");
			return STEPMATCH_FAIL;			
		case MDSTATE_ERROR:
            return  mydwc_HandleError();
      case MDSTATE_DISCONNECTTING:
        // ボイスチャットの終了を待っている状態
        if( _dWork->isvchat == 0 ) {
            // ボイスチャットの切断完了。
            // 通信自体を切る。
            MYDWC_DEBUGPRINT("ボイスチャットの切断完了。\n");
            DWC_CloseAllConnectionsHard( );
            _dWork->state = MDSTATE_DISCONNECT;
            break;
        }

		default:
			break;		
//			return handleError();
	}
	return mydwc_step();	
		
//    DWC_ProcessFriendsMatch();  // DWC通信処理更新			
//	return handleError();
}



//==============================================================================
/**
 * サーバにデータ送信を行うことができるか
 * @retval  1 - 成功　 0 - 失敗（送信バッファが詰まっている等）
 */
//==============================================================================
BOOL mydwc_canSendToServer()
{
	return ( DWC_GetMyAID() == 0 || ( _dWork->sendbufflag == 0 && _isSendableReliable() ) );
}

//==============================================================================
/**
 * クライアントにデータ送信を行うことができるか
 * @retval  1 - 成功　 0 - 失敗（送信バッファが詰まっている等）
 */
//==============================================================================
BOOL mydwc_canSendToClient()
{
	return ( _dWork->sendbufflag == 0 && _isSendableReliable() );
}

//==============================================================================
/**
 * サーバにデータ送信を行う関数
 * @param   data - 送信するデータへのポインタ。size - 送信するデータのサイズ
 * @retval  1 - 成功　 0 - 失敗（送信バッファが詰まっている等）
 */
//==============================================================================
int mydwc_sendToServer(void *data, int size)
{
    if( !(size < SIZE_SEND_BUFFER) ){
        return 0;
    }

    
	MYDWC_DEBUGPRINT("mydwc_sendToServer(data=%d)\n", *((u32*)data));

	if( DWC_GetMyAID() == 0 )
	{
		// 自分が親
		// コールバック内で書き換えられる可能性を考え、dataをコピーしておく。
//		void *buf = mydwc_AllocFunc( NULL, size, 32 );
//		MI_CpuCopy8	( data, buf, size );
		
		// 自分自身のサーバ受信コールバックを呼び出す。
		if( _dWork->serverCallback != NULL ) _dWork->serverCallback(0, data, size);
		
		// コールバックを呼び出したらすぐに開放。
//		mydwc_FreeFunc( NULL, buf, size );
		
		return 1;
	}
	else
	{
		// 相手が親。相手に対してデータ送信。
		if( _dWork->sendbufflag || !_isSendableReliable() ) // 送信バッファをチェック。
		{
			// 送信バッファがいっぱいなどで送れない。
			return 0;
		}
		
		// 送信バッファにコピー
		*((u32*)&(_dWork->sendBuffer[0])) = MYDWC_GAME_PACKET | (_dWork->myvchaton << MYDWC_PACKET_VCHAT_SHIFT);
		_dWork->sendBuffer[MYDWC_PACKET_SEQNO_POS] = ++_dWork->myseqno;
		MI_CpuCopy8( data, &(_dWork->sendBuffer[4]), size );
		_dWork->sendbufflag = 1;
		
		// 親機に向けてのみ送信
		DWC_SendReliableBitmap( 0x01, &(_dWork->sendBuffer[0]), size + 4);
//		MYDWC_DEBUGPRINT("-");
		//OHNO_PRINT("-");
		return 1;
	}
}

//==============================================================================
/**
 * 親機が子機にデータ送信を行う関数
 * @param   data - 送信するデータへのポインタ。size - 送信するデータのサイズ
 * @retval  1 - 成功　 0 - 失敗（送信バッファが詰まっている等）
 */
//==============================================================================
int mydwc_sendToClient(void *data, int size)
{
    if( !(size < SIZE_SEND_BUFFER) ){
        return 0;
    }
	// 親機しかこの動作は行わないはず。
    if( !(DWC_GetMyAID() == 0) ){
        return 0;
    }
	
	MYDWC_DEBUGPRINT("sendToClient(data=%d)\n", *((u32*)data));
	
	{
		// 相手に対してデータ送信。
		if( _dWork->sendbufflag || !_isSendableReliable() ) // 送信バッファをチェック。
		{
            MYDWC_DEBUGPRINT("wifi failed %d %d\n",_dWork->sendbufflag,_isSendableReliable());
			// 送信バッファがいっぱいなどで送れない。
			return 0;
		}
		
		// 送信バッファにコピー
		*((u32*)&(_dWork->sendBuffer[0])) = MYDWC_GAME_PACKET | (_dWork->myvchaton << MYDWC_PACKET_VCHAT_SHIFT);
		_dWork->sendBuffer[MYDWC_PACKET_SEQNO_POS] = ++_dWork->myseqno;
		MI_CpuCopy8( data, &(_dWork->sendBuffer[4]), size );
        _dWork->sendbufflag = 1;
		
        // 子機に向けて送信
     //   MYDWC_DEBUGPRINT("wifi send %z\n",DWC_GetAIDBitmap());
        if(!DWC_SendReliableBitmap(DWC_GetAIDBitmap(), &(_dWork->sendBuffer[0]), size + 4)){
            MYDWC_DEBUGPRINT("wifi SCfailed %d\n",size);
			// 送信バッファがいっぱいなどで送れない。
            _dWork->sendbufflag = 0;
            return 0;
        }
//		MYDWC_DEBUGPRINT("-");
		//OHNO_PRINT(".");
	}	
	
	{
		// 自分に対して送信
//		// コールバック内で書き換えられる可能性を考え、dataをコピーしておく。
//		void *buf = mydwc_AllocFunc( NULL, size, 32 );
//		MI_CpuCopy8	( data, buf, size );
		
		// 自分自身のサーバ受信コールバックを呼び出す。
		if( _dWork->clientCallback != NULL ) _dWork->clientCallback(0, data, size);
		
		// コールバックを呼び出したらすぐに開放。
//		mydwc_FreeFunc( NULL, buf, size );
	}  
	
	return 1;
}


//==============================================================================
/**
 * 他の相手に送信を行う関数
 * @param   data - 送信するデータへのポインタ。size - 送信するデータのサイズ
 * @retval  1 - 成功　 0 - 失敗（送信バッファが詰まっている等）
 */
//==============================================================================
int mydwc_sendToOther(void *data, int size)
{
    u16 bitmap;
    
    if( !(size < SIZE_SEND_BUFFER) ){
        return 0;
    }
	
	
	{
		// 相手に対してデータ送信。
		if( _dWork->sendbufflag || !_isSendableReliable() ) // 送信バッファをチェック。
		{
            MYDWC_DEBUGPRINT("wifi failed %d %d\n",_dWork->sendbufflag,_isSendableReliable());
			// 送信バッファがいっぱいなどで送れない。
			return 0;
		}
		
		// 送信バッファにコピー
		*((u32*)&(_dWork->sendBuffer[0])) = MYDWC_GAME_PACKET | (_dWork->myvchaton << MYDWC_PACKET_VCHAT_SHIFT);
		_dWork->sendBuffer[MYDWC_PACKET_SEQNO_POS] = ++_dWork->myseqno;
		MI_CpuCopy8( data, &(_dWork->sendBuffer[4]), size );
        _dWork->sendbufflag = 1;
		
        bitmap = DWC_GetAIDBitmap();
        if(bitmap != DWC_SendReliableBitmap(bitmap, &(_dWork->sendBuffer[0]), size + 4)){
            MYDWC_DEBUGPRINT("wifi SOFailed %d\n",size);
			// 送信バッファがいっぱいなどで送れない。
            _dWork->sendbufflag = 0;
            return 0;
        }
		//OHNO_PRINT(":");
	}	
	
	{
        // 自分自身の受信コールバックを呼び出す。
        if( _dWork->clientCallback != NULL ) _dWork->clientCallback(DWC_GetMyAID() , data, size);
	}  
	
	return 1;
}

/*---------------------------------------------------------------------------*
  ログインコールバック関数
 *---------------------------------------------------------------------------*/
static void LoginCallback(DWCError error, int profileID, void *param)
{
    BOOL result;

    // stUserDataが更新されているかどうかを確認。
    if ( DWC_CheckDirtyFlag( (_dWork->myUserData)) )
    {
	    // 必ずこのタイミングでチェックして、dirty flagが有効になっていたら、
	    // DWCUserDataをDSカードのバックアップに保存するようにしてください。
	    // 2006.04.07 k.ohno  セーブエリアに入れる
	    DWCUserData *userdata = NULL;
	    DWC_ClearDirtyFlag(_dWork->myUserData);
     //   SaveData_SaveParts(_dWork->pSaveData, SVBLK_ID_NORMAL);  //セーブ中 k.ohno 06.06.05
        _dWork->saveing = 1;  //セーブ中に1
        
	//          userdata = WifiList_GetMyUserInfo(SaveData_GetWifiListData(_dWork->pSaveData));
	//          MI_CpuCopy32( &_dWork->myUserData, userdata,  sizeof(_dWork->myUserData) );	
	    MYDWC_DEBUGPRINT("自分のフレンドコードが変更\n");
    }

    if (error == DWC_ERROR_NONE){
        // 認証成功、プロファイルID取得
#if 0
        if(CommStateGetServiceNo() == COMM_MODE_LOBBY_WIFI){
            _dWork->state = MDSTATE_LOGIN;		// ログイン完了  
        }
        else{
            result = DWC_UpdateServersAsync(NULL, //（過去との互換性のため、必ずNULL)
                                            UpdateServersCallback, _dWork->myUserData,
                                            FriendStatusCallback, param,
                                            DeleteFriendListCallback, param);

            if (result == FALSE){
                // 呼んでもいい状態（ログインが完了していない状態）で呼んだ時のみ
                // FALSEが返ってくるので、普通はTRUE
                MYDWC_DEBUGPRINT("DWC_UpdateServersAsync error teminated.\n");
                CommStateSetError(COMM_ERROR_RESET_SAVEPOINT);
                return;
            }
        }
#else
            result = DWC_UpdateServersAsync(NULL, //（過去との互換性のため、必ずNULL)
                                            UpdateServersCallback, _dWork->myUserData,
                                            FriendStatusCallback, param,
                                            DeleteFriendListCallback, param);

            if (result == FALSE){
                // 呼んでもいい状態（ログインが完了していない状態）で呼んだ時のみ
                // FALSEが返ってくるので、普通はTRUE
                MYDWC_DEBUGPRINT("DWC_UpdateServersAsync error teminated.\n");
                CommStateSetError(COMM_ERROR_RESET_SAVEPOINT);
                return;
            }
#endif
        // GameSpyサーバ上バディ成立コールバックを登録する
        DWC_SetBuddyFriendCallback(BuddyFriendCallback, NULL);
    }
    else
    {
        // 認証失敗
		_dWork->state = MDSTATE_ERROR;
    }
}

/*---------------------------------------------------------------------------*
  タイムアウトコールバック関数
 *---------------------------------------------------------------------------*/
static void recvTimeoutCallback(u8 aid)
{
	MYDWC_DEBUGPRINT("DWCタイムアウト - %d",aid);
	// コネクションを閉じる
	if( _dWork->timeoutflag )
	{
		MYDWC_DEBUGPRINT("接続を切断します\n");
		DWC_CloseAllConnectionsHard( );
        _dWork->newFriendConnect = -1;
		// タイムアウト
		_dWork->state = MDSTATE_TIMEOUT;	
	}
}

/*---------------------------------------------------------------------------*
  GameSpyサーバアップデートコールバック関数
 *---------------------------------------------------------------------------*/
static void UpdateServersCallback(DWCError error, BOOL isChanged, void* param)
{
#pragma unused(param)
    if (error == DWC_ERROR_NONE){
        if (isChanged){
            // 友達リストが変更されていた
        }
       	_dWork->state = MDSTATE_LOGIN;		// ログイン完了  
    }
    else {
        // ログイン失敗扱いにしとく？
        _dWork->state = MDSTATE_ERROR;
    }
}


/*---------------------------------------------------------------------------*
  友達状態変化通知コールバック関数
 *---------------------------------------------------------------------------*/
static void FriendStatusCallback(int index, u8 status, const char* statusString, void* param)
{
#pragma unused(param)

    MYDWC_DEBUGPRINT("Friend[%d] changed status -> %d (statusString : %s).\n",
               index, status, statusString);
            
}


/*---------------------------------------------------------------------------*
  友達リスト削除コールバック関数
 *---------------------------------------------------------------------------*/
static void DeleteFriendListCallback(int deletedIndex, int srcIndex, void* param)
{
#pragma unused(param)
	// フレンドリストが削除された。
	
    MYDWC_DEBUGPRINT("friend[%d] was deleted (equal friend[%d]).\n",
               deletedIndex, srcIndex);

    MYDWC_DEBUGPRINT("friend[%d] was deleted (equal friend[%d]).\n",
               deletedIndex, srcIndex);
    // 書き戻し
    MI_CpuCopy8(_dWork->keyList,WifiList_GetDwcDataPtr(SaveData_GetWifiListData(_dWork->pSaveData), 0),FRIENDLIST_MAXSIZE * sizeof(DWCFriendData));
    WifiList_DataMarge(SaveData_GetWifiListData(_dWork->pSaveData),
                       deletedIndex, srcIndex);
	//フレンド毎に持つフロンティアデータもマージする 2008.05.24(土) matsuda
	FrontierRecord_DataMarge(SaveData_GetFrontier(_dWork->pSaveData), deletedIndex, srcIndex);
}


/*---------------------------------------------------------------------------*
  GameSpyバディ成立コールバック関数
 *---------------------------------------------------------------------------*/
static void BuddyFriendCallback(int index, void* param)
{
#pragma unused(param)

    MYDWC_DEBUGPRINT("I was authorized by friend[%d].\n", index);
}

/*---------------------------------------------------------------------------*
  タイムアウト時間のクリア
 *---------------------------------------------------------------------------*/
static void clearTimeoutBuff(void)
{
    MI_CpuClear8(_dWork->sendintervaltime, sizeof(_dWork->sendintervaltime));
}
/*---------------------------------------------------------------------------*
  タイムアウト時間の設定
 *---------------------------------------------------------------------------*/
static void setTimeoutTime(void)
{
    int i;

    for(i = 0 ;i < COMM_MODE_CLUB_WIFI_NUM_MAX+1; i++){
        DWC_SetRecvTimeoutTime( i, 0 );
    }

    
    if(DWC_GetMyAID() == _WIFI_PARENT_AID){
        for(i = 0 ;i < _dWork->maxConnectNum; i++){
            if(DWC_GetMyAID() != i){
                if(DWC_GetAIDBitmap() & (0x01<<i) ){
                    GF_ASSERT(DWC_SetRecvTimeoutTime( i, MYDWC_TIMEOUTSEC * 1000 ));
                    MYDWC_DEBUGPRINT("setTimeOut %d\n",i);
                }
            }
        }
    }
    else{
        GF_ASSERT(DWC_SetRecvTimeoutTime( _WIFI_PARENT_AID, MYDWC_TIMEOUTSEC * 1000 ));
        MYDWC_DEBUGPRINT("setTimeOut 0\n");
    }

	// 080523 tomoya takahashi
	// BTS:通信No.167	
	// 親から何か受信しないとtimeoutflagがたたないため、
	// 子はいつまでもタイムアウト状態にならない可能性がある。
	// その為、マッチングが成功したら、直ぐにtimeoutflagが立つように
	// 変更してみる。
    // _dWork->timeoutflag = 0;	<-元々
    _dWork->timeoutflag = 1;	
    clearTimeoutBuff();
}


static void setConnectionBuffer(int index)
{
    int i,j;
    
	_dWork->state = MDSTATE_MATCHED;

#if PL_G0197_080710_FIX
#else
    for(i=0,j=0;i< _dWork->maxConnectNum; i++){
        if(DWC_GetMyAID()!=i){
            mydwc_allocRecvBuff(i);
        }
    }
#endif
    setTimeoutTime();
}

/*---------------------------------------------------------------------------*
  友達無指定接続コールバック関数
 *---------------------------------------------------------------------------*/
static void ConnectToAnybodyCallback(DWCError error, BOOL cancel, void* param)
{
#pragma unused(param)

    if (error == DWC_ERROR_NONE){
        if (!cancel){
            // 見知らぬ人たちとのコネクション設立完了
            MYDWC_DEBUGPRINT("接続完了 %d\n",DWC_GetMyAID());
            OHNO_PRINT("接続完了 %d\n",DWC_GetMyAID());
            setConnectionBuffer(1 - DWC_GetMyAID() );
        }
        else {
            MYDWC_DEBUGPRINT("キャンセル完了\n");
            // ログイン後状態に戻る
            
            // 2006.7.4 yoshihara 修正
			finishcancel();	
        }
    }
    else {
        MYDWC_DEBUGPRINT("マッチング中にエラーが発生しました。 %d\n\n", error);
		_dWork->state = MDSTATE_ERROR; 
    }
    if( _dWork->connectCallback ){
        // 何人とつながっても一回しか呼ばれないので自分のIDを入れている
        _dWork->connectCallback(DWC_GetMyAID(), _dWork->pConnectWork);
    }
}

/*---------------------------------------------------------------------------*
  友達無指定マッチング時プレイヤー評価コールバック関数
 *---------------------------------------------------------------------------*/
static int  EvaluateAnybodyCallback(int index, void* param)
{
#pragma unused(index,param)

    return 1;
}


/** -------------------------------------------------------------------------
  送信完了コールバック  
  ---------------------------------------------------------------------------*/
static void SendDoneCallback( int size, u8 aid )
{
#pragma unused(size)
	// 送信バッファをあける
	_dWork->sendbufflag = 0;
	_dWork->sendintervaltime[aid] = 0;
//	MYDWC_DEBUGPRINT( "送信\n" );
//	MYDWC_DEBUGPRINT( "*" );
}



static void _setOpVchat(u32 topcode)
{

    if(!CommLocalIsWiFiQuartetGroup(CommStateGetServiceNo())){
        if( topcode & MYDWC_PACKET_VCHAT_MASK ) 
        {
            // 相手のボイスチャットはオン
            _dWork->opvchaton = 1;		
        }
        else
        {
            // 相手のボイスチャットはオン
            _dWork->opvchaton = 0;		
        }
    }
}



/** -------------------------------------------------------------------------
  受信完了コールバック  
  ---------------------------------------------------------------------------*/
static void UserRecvCallback( u8 aid, u8* buffer, int size )
{
#pragma unused( aid, buffer, size )

	u32 topcode;
	topcode = (buffer[3] << 24) | (buffer[2] << 16) | (buffer[1] << 8) | buffer[0];

    //	MYDWC_DEBUGPRINT("[%d,%d,%d,%d]", buffer[0], buffer[1], buffer[2], buffer[3]);

	// 一度受信してはじめてタイムアウトを設定する。
	_dWork->timeoutflag = 1;

	// まず、先頭の４バイトをみて、ゲームの情報かどうかを判断
	if( (topcode & MYDWC_PACKETYPE_MASK) == MYDWC_GAME_PACKET ){
		_setOpVchat( topcode );
		_dWork->opseqno = buffer[MYDWC_PACKET_SEQNO_POS];
	}
	else {
#ifdef MYDWC_USEVCHA		
		if( myvct_checkData( aid, buffer,size ) ) return;
#endif	
		// 無意味な情報（コネクションを保持するためのものと思われる）
		_setOpVchat( topcode );
		return;
	}
	MYDWC_DEBUGPRINT( "受信(%d)\n",*((s32*)buffer) );
	
	// アライメントを確実にするために、コピー
	{
		u16 *temp = (u16*)mydwc_AllocFunc( NULL, size - 4, 4);
        if(temp==NULL){
            return;
        }
        
		MI_CpuCopy8(buffer + 4, (void*)temp, size - 4);
		
		if( DWC_GetMyAID() == 0 )
		{	
			// 自分が親の場合…クライントからサーバに対して送られてきたものと判断。
			// サーバ用受信関数を呼び出す。
			if( _dWork->serverCallback != NULL ) _dWork->serverCallback(aid, temp, size-4);
		} else {
			// サーバからクライアントに対して送られてきたものと判断。	
			if( _dWork->clientCallback != NULL ) _dWork->clientCallback(aid, temp, size-4);
		}
		
		mydwc_FreeFunc(NULL, temp, size - 4);
	}
}

/*---------------------------------------------------------------------------*
  コネクションクローズコールバック関数
error  	DWCエラー種別。内容の詳細は、DWC_GetLastErrorExを参照してください。
isLocal
TRUE:自分がクローズした。
FALSE:他の誰かがクローズした。 マッチメイク中に応答のないホストを切断する時もTRUEになる。
isServer
TRUE: サーバクライアントマッチメイクのサーバDSがクローズした。自分がサーバDSでisLocal = TRUE の時もTRUEになる。
FALSE:それ以外。サーバクライアントマッチメイク以外では必ずFALSEとなる。
aid 	クローズしたプレイヤーのAID
index 	クローズしたプレイヤーの友達リストインデックス。クローズしたプレイヤーが友達でなければ-1となる。
param 	DWC_SetConnectionClosedCallback関数で指定したコールバック用パラメータ
 *---------------------------------------------------------------------------*/
static void ConnectionClosedCallback(DWCError error,
                                     BOOL isLocal,
                                     BOOL isServer,
                                     u8  aid,
                                     int index,
                                     void* param)
{
#pragma unused(param, index)
    MYDWC_DEBUGPRINT("ConnectionClosedCallback %d %d %d %d %d\n",error,isLocal, isServer,aid,_dWork->closedflag);

    _dWork->sendbufflag = 0;
    _dWork->setupErrorCount = 0;
    _dWork->bConnectCallback = FALSE;
    
    if (error == DWC_ERROR_NONE){

//        if((!CommLocalIsWiFiQuartetGroup(CommStateGetServiceNo()) && (DWC_GetNumConnectionHost() == 1))){
        if(((_dWork->closedflag == TRUE) && (DWC_GetNumConnectionHost() == 1))){
            // 全てのコネクションがクローズされた場合         
          
            // キャンセル中ならば、キャンセルが完了した。
			if(	_dWork->state == MDSTATE_CANCEL )
			{
                //  DWC_SetupGameServerのコールバックで呼ばれるため まだ状態がDISCONNECTできていない可能性があるため ここでFINISHにしない
                //				_dWork->state = MDSTATE_CANCELFINISH;
                MYDWC_DEBUGPRINT("MDSTATE_CANCELFINISH\n");
			}
			else {
                //ここは削除
//                OHNO_PRINT("DWC_CloseAllConnectionsHard");
//                DWC_CloseAllConnectionsHard( );
//				_dWork->state = MDSTATE_DISCONNECT;
				_dWork->state = MDSTATE_DISCONNECTTING;
                MYDWC_DEBUGPRINT("MDSTATE_DISCONNECTTING\n");
			}
			if( _dWork->isvchat )
			{
				// まだボイスチャットライブラリを解放していない場合。
				mydwc_stopvchat();
			}
			
        }
        
        if (isLocal){
            MYDWC_DEBUGPRINT("Closed connection to aid %d (friendListIndex = %d) Rest %d.\n",
                       aid, index, DWC_GetNumConnectionHost());
        }
        else {
            MYDWC_DEBUGPRINT("Connection to aid %d (friendListIndex = %d) was closed. Rest %d.\n",
                       aid, index, DWC_GetNumConnectionHost());
        }
    }
    else {
        MYDWC_DEBUGPRINT("Failed to close connections. %d\n\n", error);
    }

//    mydwc_releaseRecvBuff(aid);
    
    if( _dWork->disconnectCallback ){  //
        _dWork->disconnectCallback(aid, _dWork->pDisconnectWork);
    }
}

/*---------------------------------------------------------------------------*
  メモリ確保関数
 *---------------------------------------------------------------------------*/
//------#if TESTOHNO
void*
mydwc_AllocFunc( DWCAllocType name, u32   size, int align )
{
#pragma unused( name )
    void * ptr;
    OSIntrMode old;

#ifdef CHEAK_HEAPSPACE
#ifdef _WIFI_DEBUG_TUUSHIN
#ifdef DEBUGPRINT_ON
   MYDWC_DEBUGPRINT("HEAP取得(%d, %d) from %p %x\n", size, align, _dWork->headHandle, _dWork->headHandle->signature);
#endif
#endif
#endif
    
    old = OS_DisableInterrupts();

    ptr = NNS_FndAllocFromExpHeapEx( _dWork->headHandle, size, align );

//	OS_TPrintf("dwc_rap:heap alloc  needsize=%d  heapnokori=%d  heapnokori max=%d\n", size, NNS_FndGetTotalFreeSizeForExpHeap( _dWork->headHandle ), NNS_FndGetAllocatableSizeForExpHeap( _dWork->headHandle ) );

    if(ptr == NULL){
        if(_dWork->heapPtrEx!=NULL){
//			OS_TPrintf("**** dwc_rap:ex_heap alloc  needsize=%d  heapnokori=%d heapnokori max=%d\n", size, NNS_FndGetTotalFreeSizeForExpHeap( _dWork->headHandleEx ), NNS_FndGetAllocatableSizeForExpHeap( _dWork->headHandleEx ) );
            ptr = NNS_FndAllocFromExpHeapEx( _dWork->headHandleEx, size, align );
        }
    }

    if(ptr == NULL){
#ifdef _WIFI_DEBUG_TUUSHIN
//        GF_ASSERT_MSG(ptr,"HEAP faqiled");
#endif

        // ヒープが無い場合の修正
        CommStateSetError(COMM_ERROR_RESET_SAVEPOINT);  // エラーにする
        OS_RestoreInterrupts( old );

		OS_TPrintf("dwc_rap:heap none  needsize=%d  heapnokori=%d\n", size, NNS_FndGetTotalFreeSizeForExpHeap( _dWork->headHandle ) );
        return NULL;//sys_AllocMemory(HEAPID_COMMUNICATION,size);  // ニセモノをつかませARM9どまりを発生させない
    }

#ifdef CHEAK_HEAPSPACE
    {
		int hspace = NNS_FndGetTotalFreeSizeForExpHeap( _dWork->headHandle );
		if( hspace < _dWork->_heapspace )
		{
			OS_TPrintf("ヒープ残り：%d\n", hspace);
			_dWork->_heapspace = hspace;
		}
	}
	{
		int maxspace = NNS_FndGetAllocatableSizeForExpHeap( _dWork->headHandle );
		if( maxspace < _dWork->_heapmaxspace )
		{
			OS_TPrintf("MaxHeap残り：%d\n", maxspace);
			_dWork->_heapmaxspace = maxspace;
		}		
	}
#endif	
    OS_RestoreInterrupts( old );
#ifdef DEBUGPRINT_ON
//	MYDWC_DEBUGPRINT("dwc_rap:ヒープ取得（size = %d）：残り%d\n", size, NNS_FndGetTotalFreeSizeForExpHeap( _dWork->headHandle ) );
#endif
    return ptr;
}
//-----#endif //TESTOHNO

/*---------------------------------------------------------------------------*
  メモリ開放関数
 *---------------------------------------------------------------------------*/
void
mydwc_FreeFunc( DWCAllocType name, void* ptr,  u32 size  )
{
#pragma unused( name, size )
    OSIntrMode old;
	u16 groupid;

    if ( !ptr ) return;
//    MYDWC_DEBUGPRINT("HEAP解放(%p) to %p\n", ptr, _dWork->headHandle);    
    old = OS_DisableInterrupts();


	// GROUPIDから臨時ヒープか確認し
	// 確保先の拡張ヒープでメモリを開放する
	groupid = NNS_FndGetGroupIDForMBlockExpHeap( ptr );
//	OS_TPrintf( "dwc_rap:heap free groupid %d\n", groupid );
	if( groupid == _EXTRA_HEAP_GROUPID ){	

		if( _dWork->heapPtrEx==NULL ){
			// ヒープが無い場合のえらー
			CommStateSetError(COMM_ERROR_RESET_SAVEPOINT);  // エラーにする
			OS_TPrintf( "dwc_rap;ex_heap none\n" );
			return ;
		}
		
	    NNS_FndFreeToExpHeap( _dWork->headHandleEx, ptr );
	}else{
	    NNS_FndFreeToExpHeap( _dWork->headHandle, ptr );
	}
    OS_RestoreInterrupts( old );
}

//==============================================================================
/**
 * エラーが起こっていないかチェックし、エラーに応じて通信処理を閉じる処理をする。
 *  世界交換、世界バトルタワーで呼び出すために多少改造 k.ohno 06.06.08
 * @retval  ０…エラーなし。負…エラー  正 returnエラー
 */
//==============================================================================
int mydwc_HandleError(void)
{
	int errorCode;
	DWCErrorType myErrorType;
	int ret;
    int returnNo = 0;


    ret = DWC_GetLastErrorEx( &errorCode, &myErrorType );

    
	if( ret != 0 ){
        // 何らかのエラーが発生。
#if DEBUG_ONLY_FOR_ohno
        OHNO_PRINT("error occured!(%d, %d, %d)\n", ret, errorCode, myErrorType);
#else
        MYDWC_DEBUGPRINT("error occured!(%d, %d, %d)\n", ret, errorCode, myErrorType);
#endif

        // DWC_GetLastErrorExの説明にのっとる  2008.5.23
        // 返すものは基本的にerrorCodeであるが
        // エラーコードが 0 の場合やエラー処理タイプが DWC_ETYPE_LIGHT の場合は、ゲーム固有の表示のみなので retを返す
        // heapエラーだったら後でさらに変更
        returnNo = errorCode;
        if((errorCode == 0) || (myErrorType == DWC_ETYPE_LIGHT)){
            returnNo = ret;
        }
        switch(myErrorType)
        {
          case DWC_ETYPE_LIGHT:
            // ゲーム固有の表示のみで、エラーコード表示は必要ない。
            // DWC_ClearError()を呼び出せば、復帰可能。
            DWC_ClearError();
            
            break;
          case DWC_ETYPE_SHOW_ERROR:  // エラー表示
            DWC_ClearError();
            break;
          case DWC_ETYPE_SHUTDOWN_FM:
          case DWC_ETYPE_SHUTDOWN_GHTTP:
          case DWC_ETYPE_SHUTDOWN_ND:
            // DWC_ShutdownFriendsMatch()を呼び出して、FriendsMatch処理を終了する必要がある。
            // エラーコードの表示が必要。
            // この場合、とりあえずDWC_ETYPE_DISCONNECTと同じ処理をしておく。
          case DWC_ETYPE_DISCONNECT:
            //FriendsMatch処理中ならDWC_ShutdownFriendsMatch()を呼び出し、
            //更にDWC_CleanupInet()で通信の切断も行う必要がある。
            //エラーコードの表示が必要。 
            if(_dWork){
                switch( _dWork->state )
                {			
                  case MDSTATE_TRYLOGIN:
                  case MDSTATE_LOGIN:		
                  case MDSTATE_MATCHING:
                  case MDSTATE_CANCEL:
                  case MDSTATE_MATCHED:
                  case MDSTATE_PLAYING:
                  case MDSTATE_CANCELFINISH:
                  case MDSTATE_ERROR:
                  case MDSTATE_TIMEOUT:
                  case MDSTATE_DISCONNECTTING:
                  case MDSTATE_DISCONNECT:
                    DWC_ShutdownFriendsMatch(  );
                  case MDSTATE_INIT:	
                  case MDSTATE_CONNECTING:
                  case MDSTATE_CONNECTED:
                    // まだこの時点では、フレンドマッチライブラリは呼ばれていない。
                    // WIFI広場の場合外で呼ぶ
                    if(CommStateGetServiceNo() != COMM_MODE_LOBBY_WIFI){
                        DWC_CleanupInet( );
                    }
                }
                
                DWC_ClearError();
            }
            if(_dWork){
                _dWork->state = MDSTATE_ERROR_DISCONNECT;
            }
            break;
          case DWC_ETYPE_FATAL:
            // FatalError相当なので、電源OFFを促す必要がある。
            if(_dWork){
                _dWork->state = MDSTATE_ERROR_FETAL;
                // このコールバックから処理がかえってこないはず。
                if( _dWork->fetalErrorCallback != NULL ){
                    _dWork->fetalErrorCallback( -errorCode );
                }
            }
            break;
        }
    }
    if(_dWork->bHeapError){
        returnNo = ERRORCODE_HEAP;
    }
    if( ret != 0 ){
        CommStateSetWifiError( errorCode, myErrorType,  ret);
    }
	return returnNo;
}


//Reliable送信可能かどうかをチェックする関数です。

static BOOL _isSendableReliable(void)
{
    int i;
    BOOL bRet=FALSE;

    for(i=0;i< _dWork->maxConnectNum;i++){
        if(DWC_GetMyAID()==i){
            continue;
        }
        if(DWC_IsValidAID(i)){
            bRet=TRUE;  // 送信相手がいた場合TRUE
            if(!DWC_IsSendableReliable(i)){
                return FALSE;
            }
        }
    }
    return bRet;
}

/*
static int _buffer;

static void _sendData(int param)
{
	_buffer = param;
	if( !DWC_IsSendableReliable( _dWork->op_aid ) ) // 送信バッファをチェック。
	{
		DWC_SendReliable( _dWork->op_aid, &(_buffer), 4 );
	}	
}
*/

#ifdef YOSHIHARA_VCHAT_ONOFFTEST		

static u16 debug_trg;
static u16 debug_cont = 0;

static void vchat_onoff()
{
	{
	    u16 ReadData = PAD_Read();
	    debug_trg  = (u16)(ReadData & (ReadData ^ debug_cont));            // トリガ 入力
	    debug_cont = ReadData;      	
	}	
	
	if( debug_trg & PAD_BUTTON_R )
	{
		if( _dWork->myvchaton )
		{
			mydwc_VChatPause();
		}
		else
		{
			mydwc_VChatRestart();
		}
	}
}

#endif

static BOOL sendKeepAlivePacket(int i)
{
	if( (_dWork->sendbufflag == 0) && _isSendableReliable() && (0xfffe & DWC_GetAIDBitmap()) ) // 送信バッファをチェック。
	{
        MYDWC_DEBUGPRINT("SEND KEEP ALIVE PACKET  %d %d %d\n",_dWork->sendbufflag,_isSendableReliable() , DWC_GetAIDBitmap());
		_dWork->sendbufflag = 1;
		*((u32*)&(_dWork->sendBuffer[0])) = MYDWC_KEEPALIVE_PACKET | (_dWork->myvchaton << MYDWC_PACKET_VCHAT_SHIFT);;

        DWC_SendReliableBitmap(DWC_GetAIDBitmap(), &(_dWork->sendBuffer[0]), 4);

		_dWork->sendintervaltime[i] = 0;
        //MYDWC_DEBUGPRINT("SEND KEEP ALIVE PACKET\n");
		//OHNO_PRINT("KEEP ALIVE %d\n",sys.vsync_counter);
        return TRUE;
	}
    return FALSE;
}


// 通信確立後、毎フレーム呼び出してください。
//==============================================================================
/**
 * DWC通信処理更新を行う
 * @param none
 * @retval 0…正常, 負…エラー発生 1…タイムアウト 2…相手から切断された
 */
//==============================================================================
static int mydwc_step(void)
{
    int i,ret;
    

	DWC_ProcessFriendsMatch();  // DWC通信処理更新
	mydwc_updateFriendInfo();	

    
#ifdef MYDWC_USEVCHA
	if( _dWork->isvchat ) 
	{
#ifdef YOSHIHARA_VCHAT_ONOFFTEST		
		vchat_onoff();
#endif
		
		if( (_dWork->myvchaton == 1) && (_dWork->opvchaton == 1) && (_dWork->myvchat_send == TRUE) )
		{
			myvct_onVchat();
		}
		else
		{
			myvct_offVchat();
		}
				
		myvct_main();

        if(_dWork->backupBitmap != DWC_GetAIDBitmap()){
            if(!_dWork->pausevchat && _dWork->bVChat){
                if(myvct_AddConference(DWC_GetAIDBitmap(), DWC_GetMyAID())){
                    _dWork->backupBitmap = DWC_GetAIDBitmap();
                }
            }
        }
	}
#endif
    if( _dWork->state == MDSTATE_TIMEOUT ){  // タイムアウトステートの時は同時にエラーも監視する
		ret = mydwc_HandleError();
        if(ret != 0){
            return ret;
        }
        return STEPMATCH_TIMEOUT;
    }
	if( _dWork->state == MDSTATE_DISCONNECT ) return STEPMATCH_DISCONNECT;
	
	if( _dWork->state == MDSTATE_MATCHED  ||  _dWork->state == MDSTATE_PLAYING) {
        for(i=0 ; i< _dWork->maxConnectNum; i++){
            if( _dWork->sendintervaltime[i]++ >= KEEPALIVE_TOKEN_TIME && _dWork->sendbufflag == 0) {
                if(sendKeepAlivePacket(i)){
                    clearTimeoutBuff();
                    break;
                }
            }
        }
	}
	
	return mydwc_HandleError();
}

//==============================================================================
/**
 * aidを返します。接続するまでは-1を返します。
 * @retval  aid。ただし接続前は-1
 */
//==============================================================================
int mydwc_getaid()
{
    if(_dWork){
        if( _dWork->state == MDSTATE_MATCHED || _dWork->state == MDSTATE_PLAYING || _dWork->state == MDSTATE_DISCONNECTTING )
        {
            return DWC_GetMyAID();
        }
    }
	return -1;
}

// ボイスチャットの切断コールバック。
static void vct_endcallback(){
	_dWork->isvchat = 0;
}

//==============================================================================
/**
 * 通信確立後、ボイスチャットを開始します。
 * @param   heapID  メモリ確保するHEAPID
 * @retval  none
 */
//==============================================================================
void mydwc_startvchat(int heapID)
{
    int late;
    int num = 1;
    BOOL bFourGame = CommLocalIsWiFiQuartetGroup(CommStateGetServiceNo());

	// デバックプリントOFF
#ifndef DEBUGPRINT_ON
	VCT_SetReportLevel( VCT_REPORTLEVEL_NONE );
#else
      VCT_SetReportLevel( VCT_REPORTLEVEL_ALL );

#endif

    if(bFourGame){
        num = CommGetConnectNum()-1;
        MYDWC_DEBUGPRINT("VCTON人数%d\n",num);
        if(num < 1){
            num = 1;
        }
        _dWork->myvchaton = 1;
        _dWork->opvchaton = 1;
		_dWork->myvchat_send = 1;
    }
    
	if( _dWork->isvchat==0 ){
        switch( _dWork->vchatcodec ){
          case VCHAT_G711_ULAW:
            late = VCT_CODEC_G711_ULAW;
            break;
          case VCHAT_2BIT_ADPCM:
            late = VCT_CODEC_2BIT_ADPCM;
            break;
          case VCHAT_3BIT_ADPCM:
            late = VCT_CODEC_3BIT_ADPCM;
            break;
          case VCHAT_4BIT_ADPCM:		
            late = VCT_CODEC_4BIT_ADPCM;
            break;
          default:
            if(!bFourGame){
                late = VCT_CODEC_4BIT_ADPCM;   //  DP
            }
            else{
                late = VCT_CODEC_3BIT_ADPCM;
            }
            break;
        }
        myvct_init( heapID, late, num );
        myvct_setDisconnectCallback( vct_endcallback );
        _dWork->isvchat = 1;
    }
}

//==============================================================================
/**
 * ボイスチャットのコーデックを指定します。
 * コネクション確立前にコーデックを指定しておくと、
 * コネクション確立時に自動的にボイスチャットを開始します。
 * @param コーデックのタイプ
 * @retval none
 */
//==============================================================================
void mydwc_setVchat(int codec){
	_dWork->vchatcodec = codec;

	if( _dWork->isvchat ){
		switch( _dWork->vchatcodec ){
			case VCHAT_G711_ULAW:
				myvct_setCodec( VCT_CODEC_G711_ULAW );
			break;
			case VCHAT_2BIT_ADPCM:
				myvct_setCodec( VCT_CODEC_2BIT_ADPCM );
			break;
			case VCHAT_3BIT_ADPCM:
				myvct_setCodec( VCT_CODEC_3BIT_ADPCM );
			break;
			case VCHAT_4BIT_ADPCM:		
				myvct_setCodec( VCT_CODEC_4BIT_ADPCM );
			break;
			case VCHAT_8BIT_RAW:
				myvct_setCodec( VCT_CODEC_8BIT_RAW );
			break;
		}
	}
}

//==============================================================================
/**
 * @brief   ボイスチャットを停止します
 * @param   void
 * @retval  void
 */
//==============================================================================

void mydwc_stopvchat(void)
{
    myvct_free();
    if(_dWork != NULL){
	    _dWork->isvchat = 0;
    	_dWork->backupBitmap = 0;
    }
}

//==============================================================================
/**
 * @brief   ボイスチャットの一時停止＋解除を行います
 * @param   bPause   一時停止=TRUE  解除=FALSE
 * @retval  void
 */
//==============================================================================

void mydwc_pausevchat(BOOL bPause)
{
    if(bPause){
        myvct_DelConference(DWC_GetMyAID());
        _dWork->pausevchat = TRUE;
    }
    else{
        _dWork->pausevchat = FALSE;
    }
}



//==============================================================================
/**
 * DWCエラー番号に対応するメッセージのIDを返します。
 * @param code - エラーコード（正の値を入れてください
 * @retval メッセージタイプ
 */
//==============================================================================
int mydwc_errorType(int code, int type)
{
	int code100 = code / 100;
	int code1000 = code / 1000;

    if( code == 20101 ) return 1;
	if( code1000 == 23 ) return 1;
	if( code == 20108 ) return 2;
	if( code == 20110 ) return 3;
	if( code100 == 512 ) return 4;
	if( code100 == 500 ) return 5;
    if( code == 51103 ) return 7;
	if( code100 == 510 ) return 6;
	if( code100 == 511 ) return 6;
	if( code100 == 513 ) return 6;
	if( code >= 52000 && code <= 52003 ) return 8;
	if( code >= 52100 && code <= 52103 ) return 8;
	if( code >= 52200 && code <= 52203 ) return 8;
	if( code == 80430 ) return 9;

    if( code1000 == 20 ) return 0;
	if( code100 == 520 ) return 0;
	if( code100 == 521 ) return 0;
	if( code100 == 522 ) return 0;
	if( code100 == 523 ) return 0;
	if( code100 == 530 ) return 0;
	if( code100 == 531 ) return 0;
	if( code100 == 532 ) return 0;

    if( code < 10000 ) return 14; // エラーコードが１００００以下の場合は、エラーコードの表示の必要がない。

	if( code1000 == 31 ) return 12; // ダウンロード失敗
    // その他のエラーはTYPEで判断

    switch(type){
      case DWC_ETYPE_NO_ERROR:
      case DWC_ETYPE_LIGHT:
      case DWC_ETYPE_SHOW_ERROR:
        return 11;
      case DWC_ETYPE_SHUTDOWN_FM:
      case DWC_ETYPE_SHUTDOWN_GHTTP:
      case DWC_ETYPE_SHUTDOWN_ND:
      case DWC_ETYPE_DISCONNECT:
        return 10;
      default:
        GF_ASSERT(0); //DWCが返すtypeが増えているなら見直しが必要
        break;
      case DWC_ETYPE_FATAL:
        return 15;
    }
    return -1;
}

//==============================================================================
/**
 * 通信を切断します。
 * @param sync … 0 = 自分から切断しにいく。1 = 相手が切断するのを待つ。
 * @retval 　　　 1 = 接続処理完了。0 = 切断処理中。
 */
//==============================================================================
int mydwc_disconnect( int sync )
{
	if( sync == 0 ){
        MYDWC_DEBUGPRINT(" mydwc_disconnect state %d \n",_dWork->state);
		switch( _dWork->state )	{
          case MDSTATE_MATCHING:   // k.ohno 06.07.08  追加
          case MDSTATE_MATCHED:
          case MDSTATE_PLAYING:
            if( _dWork->isvchat ){
                MYDWC_DEBUGPRINT("ボイスチャット稼働中 止める\n");
                myvct_endConnection();
            }
            _dWork->state = MDSTATE_DISCONNECTTING;
            break;
          case MDSTATE_LOGIN:     //親機切断時に動きを合わせるために追加 k.ohno 06.07.04
          case MDSTATE_ERROR_DISCONNECT:
          case MDSTATE_DISCONNECT:
          case MDSTATE_TIMEOUT:
            return 1;
        }
	}
    else {
		switch( _dWork->state ) {
            case MDSTATE_LOGIN:     //親機切断時に動きを合わせるために追加 k.ohno 06.07.04
			case MDSTATE_DISCONNECT:
			case MDSTATE_TIMEOUT:
				return 1;
		}		
	}
	return 0;
}

//==============================================================================
/**
 * 通信が切断した後、この関数を呼ぶことで内部状態をログイン直後の状態にします。
 * @param 	nonte
 * @retval  1 = 成功。0 = 失敗。
 */
//==============================================================================
int mydwc_returnLobby()
{
    if( _dWork->state == MDSTATE_DISCONNECT || _dWork->state == MDSTATE_TIMEOUT || _dWork->state == MDSTATE_LOGIN) {
//        _dWork->op_aid = -1;
        _dWork->state = MDSTATE_LOGIN;
        _dWork->newFriendConnect = -1;
        mydwc_ResetClientBlock();
        return 1;
	 }
	 return 0;
}

//==============================================================================
/**
 * fetal error発生時に呼ばれる関数、このコールバックの中で処理をとめて下さい。
 * @param 	nonte
 * @retval  1 = 成功。0 = 失敗。
 */
//==============================================================================
void mydwc_setFetalErrorCallback( void (*func)(int) )
{
    if(_dWork){
        _dWork->fetalErrorCallback = func;
    }
}



//-----2006.04.11 k.ohno
//==============================================================================
/**
 * ログアウトする
 * @param 	none
 * @retval  none
 */
//==============================================================================

void mydwc_Logout(void)
{
    MYDWC_DEBUGPRINT("----------------------ok LOGOUT  \n");
    DWC_ShutdownFriendsMatch();
    DWC_CleanupInet();
    mydwc_stopvchat();
    mydwc_free();
}


//==============================================================================
/**
 * オンラインの友達がサーバにアップしている情報をおさめる配列を指定します。
 * この配列はmydwc_step, mydwc_stepmatchを呼び出した際に更新される可能性があります。
 * @param 	array - データをおさめる配列の先頭
 * @param 	size  - 一人あたりのデータサイズ
 * @retval  none
 */
//==============================================================================
void mydwc_setFriendStateBuffer( void *array, int size )
{
	_dWork->friendinfo = array;	
	_dWork->infosize = size;
	

}

// FRIENDINFO_UPDATA_PERFRAME人分のデータを更新する。
static void mydwc_updateFriendInfo( )
{
	int i;
	if( _dWork->friendinfo == NULL ) return;
	for(i = 0; i < FRIENDINFO_UPDATA_PERFRAME; i++)
	{
		int index = _dWork->friendupdate_index % FRIENDLIST_MAXSIZE;
		int size;
	
		if( DWC_IsBuddyFriendData( &(_dWork->keyList[index]) ) ) 
		{
			_dWork->friend_status[index] = 
				DWC_GetFriendStatusData( 
					&_dWork->keyList[ index ], 
					(void *)(((u32)_dWork->friendinfo) + _dWork->infosize * index),
					&size
				);
				
			if( size > _dWork->infosize )
			{
				MYDWC_DEBUGPRINT("\n!!!!!!!!!!!!!!!size > _dWork->infosize!!!!!!!!!!!!!!!!\n");
			}
		}
			
		_dWork->friendupdate_index = (_dWork->friendupdate_index + 1) % FRIENDLIST_MAXSIZE;
	}
}

//==============================================================================
/**
 * 自分の状態をサーバにアップします。
 * @param 	data - データへのポインタ
 * @param 	size  - データサイズ
 * @retval  成功の可否
 */
//==============================================================================
BOOL mydwc_setMyInfo( const void *data, int size )
{
	MYDWC_DEBUGPRINT("upload status change(%p, %d)\n", data, size);
	return DWC_SetOwnStatusData( data, size );
}

//==============================================================================
/**
 * 友達がサーバにアップしている情報を取得します（ローカルにキャッシュしてあるものを表示します）。
 * @param 	index フレンドリスト上の番号
 * @retval  データへのポインタ。中身は書き換えないで下さい。
 */
//==============================================================================
void *mydwc_getFriendInfo( int index )
{
	return (void *)(((u32)_dWork->friendinfo) + _dWork->infosize * index);
}

//==============================================================================
/**
 * 友達がサーバにアップしている情報を取得します（ローカルにキャッシュしてあるものを表示します）。
 * @param 	index フレンドリスト上の番号
 * @retval  友達の状態。DWC_GetFriendStatusDataの返り値と同じ
 */
//==============================================================================
u8 mydwc_getFriendStatus( int index )
{
	return _dWork->friend_status[index];
}


static void SetupGameServerCallback(DWCError error, BOOL cancel, BOOL self, BOOL isServer, int  index, void* param);
static void ConnectToGameServerCallback(DWCError error, BOOL cancel, BOOL self, BOOL isServer, int  index, void* param);
static void NewClientCallback(int index, void* param);

//==============================================================================
/**
 * ゲーム募集・参加を行う関数。
 * @target   負…自分でゲームを開催。０以上…接続しに行く相手の、フレンドリスト上の位置
 * @retval  正…成功。０…失敗。
 */
//==============================================================================
int mydwc_startgame( int target,int maxnum, BOOL bVCT )
{
    int ans,num = maxnum;

    if(mydwc_getSaving()){
        return DWCRAP_STARTGAME_FIRSTSAVE;
    }
    
    if( _dWork->state != MDSTATE_LOGIN ){
        _dWork->setupErrorCount++;
        if(_dWork->setupErrorCount>120){
            return DWCRAP_STARTGAME_FAILED;
        }
        return DWCRAP_STARTGAME_NOTSTATE;
    }
    mydwc_releaseRecvBuffAll();
    _dWork->BlockUse_BackupBitmap = 0; //080703 ohno
	_dWork->closedflag = TRUE;	// 080602	tomoya
	_dWork->friendindex = target;
    _dWork->maxConnectNum = maxnum;
    OHNO_PRINT("max %d\n",_dWork->maxConnectNum);
    if(bVCT){
        num=2;
    }
    _dWork->bConnectCallback = TRUE;
    if ( target < 0 ){
        ans = DWC_SetupGameServer((u8)num, SetupGameServerCallback, NULL, NewClientCallback, NULL);
	    _dWork->matching_type = MDTYPE_PARENT;
    } else {
        ans = DWC_ConnectToGameServerAsync(target,ConnectToGameServerCallback,NULL,NewClientCallback,NULL);
        _dWork->matching_type = MDTYPE_CHILD;
    }
    if(!ans){
        _dWork->setupErrorCount++;
        if(_dWork->setupErrorCount>120){
            return DWCRAP_STARTGAME_FAILED;
        }
        return DWCRAP_STARTGAME_RETRY;
    }
    _dWork->setupErrorCount = 0;//リセットしておく
                              
#if PL_G0197_080710_FIX
	{
		int i;
		for(i=0;i<maxnum; i++){
			mydwc_allocRecvBuff(i);
		}
	}
#endif
    _dWork->state = MDSTATE_MATCHING;
  
    // 送信コールバックの設定	
    DWC_SetUserSendCallback( SendDoneCallback ); 

    // 受信コールバックの設定	
    DWC_SetUserRecvCallback( UserRecvCallback ); 
    
    // コネクションクローズコールバックを設定
    DWC_SetConnectionClosedCallback(ConnectionClosedCallback, NULL);
    
    // タイムアウトコールバックの設定
    DWC_SetUserRecvTimeoutCallback( recvTimeoutCallback );
    
    _dWork->sendbufflag = 0;
    
    return DWCRAP_STARTGAME_OK;    
}

//==============================================================================
/**
 * 今接続している友達のフレンドリスト上の位置を返します。
 * まだ、接続が完了していなくても友達番号を返すことがありますので、接続が
 * 完了したかの判定には使わないでください。
 * @retval  ０以上…友達番号。－１…まだ接続してない。
 */
//==============================================================================
int mydwc_getFriendIndex()
{
    if(_dWork){
        // 今接続している友達のフレンドリスト上の位置を返します。
        return _dWork->friendindex;
    }
    return -1;
}

/*---------------------------------------------------------------------------*
  ゲームサーバ起動コールバック関数
 *---------------------------------------------------------------------------*/
static void SetupGameServerCallback(DWCError error,
                                    BOOL cancel,
                                    BOOL self,
                                    BOOL isServer,
                                    int  index,
                                    void* param)
{
#pragma unused(isServer, param)
    BOOL bFriendOnly = FALSE;

    _dWork->bConnectCallback = FALSE;
    if (error == DWC_ERROR_NONE){
        if (!cancel){
            // ネットワークに新規クライアントが加わった
            MYDWC_DEBUGPRINT("友達が接続してきました。（インデックス＝%d）\n", index);
			// 接続が確立したときのみ、
			// 代入する形に修正
			// 080624
			// BTS通信630の対処	tomoya 
//			_dWork->friendindex = index;

            if(CommLocalIsWiFiFriendGroup(CommStateGetServiceNo()) && (index==-1)){
                bFriendOnly = TRUE;
            }

            if(_dWork->connectModeCheck){
                if(FALSE == _dWork->connectModeCheck(index)){
                    OS_TPrintf("切断WIFIP2PModeCheck \n");
                    bFriendOnly = TRUE;
                }
            }
            
            if (_dWork->blockClient || bFriendOnly){
                u32 bitmap = ~_dWork->BlockUse_BackupBitmap & DWC_GetAIDBitmap();
				u32 aidbitmap = DWC_GetAIDBitmap();	// (DWC_CloseConnectionHardBitmapのなかで、ClosedCallbackがよばれるため)
                if(bitmap){
                    DWC_CloseConnectionHardBitmap(&bitmap);
                    MYDWC_DEBUGPRINT("受け付けた子機を切断 %x %x\n",bitmap,_dWork->BlockUse_BackupBitmap);
                    if((bitmap ^ aidbitmap) == 0x01){ //自分しかいなかったら自分CANCEL
                        MYDWC_DEBUGPRINT("自分キャンセル %x %x \n",bitmap,aidbitmap);
                        _dWork->state = MDSTATE_CANCEL;
                    }
                    return;
                }
            }

			// 接続が確立したときのみ、
			// 代入する形に修正
			// 080624
			// BTS通信630の対処	tomoya 
			_dWork->friendindex = index;


            _dWork->BlockUse_BackupBitmap = DWC_GetAIDBitmap();
            if(_dWork->BlockUse_BackupBitmap==0x01){ //自分だけつながったときにはタイムアウトにする
                _dWork->state = MDSTATE_CANCEL;
                MYDWC_DEBUGPRINT("自分タイムアウト %x\n",_dWork->BlockUse_BackupBitmap);
            }
            else{
                // バッファの確保
                setConnectionBuffer(index);
            }
        }
        else 
        {
            if (self){
                // 自分がマッチングをキャンセルした
                MYDWC_DEBUGPRINT("マッチングのキャンセルが完了しました。\n\n");
	            // ログイン後状態に戻る
//				s_dwcstate = MYDWCSTATE_MATCH_CANCELFINISH;  
            }
            else {
                // 繋いでいた子機が、接続をキャンセルした。
                MYDWC_DEBUGPRINT("Client (friendListIndex = %d) canceled matching.\n\n", index);
                // 繋いできた子機がいなくなった。2006.7.3 yoshihara
                _dWork->newFriendConnect = -1;
            }
        }
    }
    else {
        // エラーが発生した。エラーコードは、step関数の中で拾う。
        MYDWC_DEBUGPRINT("SetupGame server error occured. %d\n\n", error);

//        s_dwcstate = MYDWCSTATE_MATCH_CANCELFINISH; 
    }
}

/*---------------------------------------------------------------------------*
  ゲームサーバ接続コールバック関数
 *---------------------------------------------------------------------------*/
static void ConnectToGameServerCallback(DWCError error,
                                        BOOL cancel,
                                        BOOL self,
                                        BOOL isServer,
                                        int  index,
                                        void* param)
{
#pragma unused(param)
    _dWork->bConnectCallback = FALSE;
    if (error == DWC_ERROR_NONE){
        if (!cancel){

            if (self){
                // 自分がゲームサーバとそこにできているネットワークへの接続に
                // 成功した場合
                MYDWC_DEBUGPRINT("接続に成功しました\n");
            }
            else {
                // ネットワークに新規クライアントが加わった場合。
                // 二人対戦限定なので、ここにはこないはず。
                MYDWC_DEBUGPRINT("新規につながりました\n");
            }
            // 受信バッファセット
            setConnectionBuffer(index);
        }
        else {
            if (self){
                // 自分がマッチングをキャンセルした
                MYDWC_DEBUGPRINT("キャンセルが完了しました。\n\n");
                // ログイン後状態に戻る
//                s_dwcstate = MYDWCSTATE_MATCH_CANCELFINISH;  
            }
            else {
                if (isServer){
                    // ゲームサーバがマッチングをキャンセルした
                    MYDWC_DEBUGPRINT("親が接続をキャンセルしました\n\n");
                    // マッチングを終了してログイン後状態に戻る
//                    s_dwcstate = MYDWCSTATE_MATCH_CANCELFINISH;
                }
                else {
                    MYDWC_DEBUGPRINT("子が接続をキャンセルしました\n\n");
                    // 他のクライアントがマッチングをキャンセルした。
                    // 二人対戦ならここにこないはず。
                }
            }
        }
    }
#ifdef PM_DEBUG
    else {
        // エラーが発生した。エラーコードは、step関数の中で拾う。
        MYDWC_DEBUGPRINT("ConnectGame server error occured. %d\n\n", error);
        {
            int errorCode;
            DWCErrorType myErrorType;
            int ret = DWC_GetLastErrorEx( &errorCode, &myErrorType );
            if( ret != 0 ){
                MYDWC_DEBUGPRINT("error occured!(%d, %d, %d)\n", ret, errorCode, myErrorType);
            }
        }
        // もう一度最初から。おいおい処理
//        s_dwcstate = MYDWCSTATE_MATCH_ERRORFINISH; 
    }
#endif

//    s_blocking = 0;
}

/*---------------------------------------------------------------------------*
  サーバクライアント型マッチング時の新規接続クライアント通知コールバック関数
 *---------------------------------------------------------------------------*/
static void NewClientCallback(int index, void* param)
{
#pragma unused(param)
    // 新規接続クライアントの接続処理が終わるまでは、
    // コントローラ操作を禁止する
//    s_blocking = 1;

    _dWork->newFriendConnect = index;
    
    MYDWC_DEBUGPRINT("新しい人が接続してきました。\n");
    if (index != -1){
        MYDWC_DEBUGPRINT("友達[%d].\n", index);
        //VCTカンファレンス開始

    }
    else {
        MYDWC_DEBUGPRINT("友達ではない.\n");
    }
    if( _dWork->connectCallback ){  //
        _dWork->connectCallback(index, _dWork->pConnectWork);
    }
}

//==============================================================================
/**
 * 音声のノイズカットレベルを調整します（外部からアクセスしたいので、こちらに）
 * @param   d … 今より閾値を下げるか、上げるか（下げるほど拾いやすくなる）
 * @retval  none
 */
//==============================================================================
void mydwc_changeVADLevel(int d)
{
//	myvct_changeVADLevel(d);
}

//==============================================================================
/**
 * 現在のフレンドリストを表示します。（デバッグ用）
 * @param 	none
 * @retval  none
 */
//==============================================================================
void mydwc_showFriendInfo()
{
	int i;
	
	if( !DWC_CheckHasProfile( _dWork->myUserData ) ) 
	{
		DWCFriendData token;
		u32 *ptr;

		DWC_CreateExchangeToken( _dWork->myUserData, &token ); 
		ptr = (u32*)&token;
		MYDWC_DEBUGPRINT("まだ、プロファイルＩＤ取得前\nログインＩＤ:(%d, %d, %d)\n\n", ptr[0], ptr[1], ptr[2] );		
	}
	else
	{
		// 接続済み	
		DWCFriendData token;
		DWC_CreateExchangeToken( _dWork->myUserData, &token );
		MYDWC_DEBUGPRINT("プロファイルＩＤ:%d \n\n", DWC_GetGsProfileId( _dWork->myUserData, &token ) );	
	}
	
	for( i = 0; i < FRIENDLIST_MAXSIZE; i++ )
	{
		int ret = DWC_GetFriendDataType( &(_dWork->keyList[i]) );
		u32 *ptr = (u32*)(&_dWork->keyList[i]);
		switch(ret)
		{
			case DWC_FRIENDDATA_LOGIN_ID:
				MYDWC_DEBUGPRINT("%d:ログインＩＤ:(%d, %d, %d)",i, ptr[0], ptr[1], ptr[2] );
				break;

			case DWC_FRIENDDATA_FRIEND_KEY:
				MYDWC_DEBUGPRINT("%d:フレンドコード:(%d)", i, DWC_GetGsProfileId( _dWork->myUserData, &_dWork->keyList[i] ) );			
				break;

			case DWC_FRIENDDATA_GS_PROFILE_ID:
				MYDWC_DEBUGPRINT("%d:プロファイルＩＤ:(%d)", i, DWC_GetGsProfileId( _dWork->myUserData, &_dWork->keyList[ i ]) );
				break;

			case DWC_FRIENDDATA_NODATA:
			default:
				MYDWC_DEBUGPRINT("%d:空", i);
				break;
		}			
		
		if( DWC_IsBuddyFriendData( &(_dWork->keyList[i]) ) )
		{
			MYDWC_DEBUGPRINT("(両思い)");
		}
		MYDWC_DEBUGPRINT("\n");
	}
}


// 送信したかどうかを返します
BOOL mydwc_IsSendVoiceAndInc(void)
{
    return myvct_IsSendVoiceAndInc();
}


//==============================================================================
/**
 * ボイスチャット状態かどうかを返します   k.ohno 06.05.23 07.22 フラグを交換
 * @retval  TRUE…ボイスチャット   FALSE…ボイスチャットではない 
 */
//==============================================================================
BOOL mydwc_IsVChat(void)
{
    if(_dWork){
        return _dWork->bVChat;
    }
    return FALSE;
}

//==============================================================================
/**
 * 接続してきたかどうかを返します    k.ohno 06.05.24
 * @retval  TRUE…接続開始なのでキー操作をブロック   FALSE…
 */
//==============================================================================
BOOL mydwc_IsNewPlayer(void)
{
    if(_dWork){
        return _dWork->newFriendConnect;
    }
    return FALSE;
}

//==============================================================================
/**
 * 接続してきたかどうかフラグを落とします k.ohno 06.08.04
 * @retval  none
 */
//==============================================================================
void mydwc_ResetNewPlayer(void)
{
    if(_dWork){
        _dWork->newFriendConnect = -1;
    }
}

//==============================================================================
/**
 * VCHATのONOFF     k.ohno 06.05.24
 */
//==============================================================================
void mydwc_setVChat(BOOL bVChat)
{
    _dWork->bVChat = bVChat;
}

static void sendPacket()
{
    int i;
    
	if( _dWork->sendbufflag || !_isSendableReliable() ) // 送信バッファをチェック。
	{
		_dWork->sendbufflag = 1;
		*((u32*)&(_dWork->sendBuffer[0])) = MYDWC_KEEPALIVE_PACKET | (_dWork->myvchaton << MYDWC_PACKET_VCHAT_SHIFT);;

        DWC_SendReliableBitmap(DWC_GetAIDBitmap(), &(_dWork->sendBuffer[0]), 4);
        clearTimeoutBuff();
	}
}

void mydwc_VChatPause()
{
	if( _dWork->myvchaton != 0 )
	{
		_dWork->myvchaton = 0;
		sendPacket();
	}
}

void mydwc_VChatRestart()
{
	if( _dWork->myvchaton != 1 )
	{
		_dWork->myvchaton = 1;
		sendPacket();
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	VCHATデータ送信設定
 *
 *	@param	flag	TRUE:音声データを送信する	FALSE:音声データを送信しない
 */
//-----------------------------------------------------------------------------
void mydwc_VChatSetSend( BOOL flag )
{
	_dWork->myvchat_send = flag;
}
BOOL mydwc_VChatGetSend( void )
{
	return _dWork->myvchat_send;
}


static void _blockCallback(void* param)
{
   _dWork->blockClient = _BLOCK_CALLBACK;
}

//==============================================================================
/**
 * @brief   クライアント接続を一時的に中止します
 * @param   none
 * @retval  none
 */
//==============================================================================
int mydwc_SetClientBlock(void)
{
    if(_dWork->blockClient==_BLOCK_NONE){
	    _dWork->blockClient = _BLOCK_START;
        DWC_StopSCMatchingAsync(_blockCallback,NULL);
    }
    return (_dWork->blockClient==_BLOCK_CALLBACK);
}

//==============================================================================
/**
 * @brief   クライアント接続を許可に戻します
 * @param   none
 * @retval  none
 */
//==============================================================================
void mydwc_ResetClientBlock(void)
{
    _dWork->blockClient = _BLOCK_NONE;
}

//==============================================================================
/**
 * @brief   評価状態中の接続数を返す
 * @param   none
 * @retval  数
 */
//==============================================================================
int mydwc_AnybodyEvalNum(void)
{
    return DWC_GetNumConnectionHost();
}

//==============================================================================
/**
 * @brief   HEAPをワールドから確保するかどうかを切り替える
 * @param   TRUEならワールドから確保
 * @retval  none
 */
//==============================================================================

void mydwc_recvHeapChange(BOOL bWorldHeap, int heapID)
{
    if(bWorldHeap){
        _dWork->recvHeapID = HEAPID_WORLD;   //受信バッファ用HEAPID

        if(_dWork->heapPtrEx==NULL){
            _dWork->heapPtrEx = sys_AllocMemory(heapID, _EXTRA_HEAPSIZE + 32);  // 拡張HEAP
            _dWork->headHandleEx = NNS_FndCreateExpHeap( (void *)( ((u32)_dWork->heapPtrEx + 31) / 32 * 32 ), _EXTRA_HEAPSIZE);
			NNS_FndSetGroupIDForExpHeap( _dWork->headHandleEx, _EXTRA_HEAP_GROUPID );
            _dWork->heapSizeEx = NNS_FndGetTotalFreeSizeForExpHeap(_dWork->headHandleEx);
        }

    }
    else{
        _dWork->recvHeapID = _dWork->heapID; // 元に戻す
        if(_dWork->heapPtrEx!=NULL){

			// ここで開放忘れがあったら終わり
            if(NNS_FndGetTotalFreeSizeForExpHeap(_dWork->headHandleEx) != _dWork->heapSizeEx){
				CommStateSetError(COMM_ERROR_RESET_SAVEPOINT);  // エラーにする
				OS_TPrintf( "dwc_rap;ex_heap can't free\n" );
				return ;
			}

            NNS_FndDestroyExpHeap( _dWork->headHandleEx );
            sys_FreeMemoryEz(_dWork->heapPtrEx);
            _dWork->heapPtrEx = NULL;
        }

    }
}

//----------------------------------------------------------------------------
/**
 *	@brief	通信クローズ処理で、切断処理に遷移するのかのフラグ
 *
 *	@param	flag	フラグ		TRUEなら切断処理に遷移する（初期値TRUE）
 *
 * *Wi-Fiクラブ４人募集画面用に作成		
 */
//-----------------------------------------------------------------------------
void mydwc_SetClosedDisconnectFlag( BOOL flag )
{
	_dWork->closedflag = flag;
	MYDWC_DEBUGPRINT( "_dWork->closedflag = %d\n", flag );
}

//==============================================================================
/**
 * @brief   受信バッファ開放
 * @param   
 * @retval  none
 */
//==============================================================================

void mydwc_releaseRecvBuff(int aid)
{
    if(_dWork->recvPtr[aid]!=NULL){
        OHNO_PRINT("_SetRecvBufferメモリ開放 %d\n",aid);
        if(_dWork->bRecvPtrWorld[aid] == TRUE){
            sys_FreeMemoryEz(_dWork->recvPtr[aid]);
        }
        else{
            mydwc_FreeFunc( 0, _dWork->recvPtr[aid],  0 );
        }
        _dWork->recvPtr[aid]=NULL;
    }
}

//==============================================================================
/**
 * @brief   受信バッファ確保
 * @param   
 * @retval  none
 */
//==============================================================================

void mydwc_allocRecvBuff(int i)
{
    mydwc_releaseRecvBuff(i);

    if(_dWork->recvPtr[i]==NULL){
        OHNO_PRINT("_SetRecvBufferメモリ確保 %d\n",i);
        if(_dWork->recvHeapID == HEAPID_WORLD){
            _dWork->recvPtr[i] = sys_AllocMemory(_dWork->recvHeapID, SIZE_RECV_BUFFER + 32);
            _dWork->bRecvPtrWorld[i] = TRUE;
        }
        else{
            _dWork->recvPtr[i] = mydwc_AllocFunc( 0, SIZE_RECV_BUFFER, 32 );
            _dWork->bRecvPtrWorld[i] = FALSE;
        }

        DWC_SetRecvBuffer( i, (u8 *)( ((u32)_dWork->recvPtr[i] + 31) / 32 * 32 ), SIZE_RECV_BUFFER );
    }
}

//==============================================================================
/**
 * @brief   受信バッファすべて開放
 * @param   TRUEならワールドから確保
 * @retval  none
 */
//==============================================================================

void mydwc_releaseRecvBuffAll(void)
{
    int i;

    for(i=0;i<COMM_MODE_CLUB_WIFI_NUM_MAX+1;i++){
        mydwc_releaseRecvBuff(i);
    }

}

//----------------------------------------------------------------------------
/**
 *	@brief	セーブして良いかどうか確認するフラグ
 *	@param	TRUE セーブしてよい
 */
//-----------------------------------------------------------------------------
u8 mydwc_getSaving(void)
{
    return _dWork->saveing;  //セーブ中に1
}

//----------------------------------------------------------------------------
/**
 *	@brief	セーブして良いかどうか確認するフラグを消す
 */
//-----------------------------------------------------------------------------
void mydwc_resetSaving(void)
{
    _dWork->saveing = 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief	キャンセル処理を行って良いかどうか
 *	@param	FALSE キャンセルしてよい TRUEキャンセル禁止
 */
//-----------------------------------------------------------------------------
BOOL mydwc_CancelDisable(void)
{
    return _dWork->bConnectCallback;
}

