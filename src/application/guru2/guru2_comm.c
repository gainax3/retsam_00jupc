//******************************************************************************
/**
 * @file	guru2_comm.c
 * @brief	ぐるぐる交換　通信
 * @author	kagaya
 * @data	05.07.13
 */
//******************************************************************************
#include "common.h"
#include "communication/communication.h"
#include "communication/comm_state.h"
#include "system/procsys.h"
#include "system/clact_tool.h"
#include "system/msgdata_util.h"
#include "system/wordset.h"
#include "system/bmp_menu.h"
#include "application/record_corner.h"
#include "system/snd_tool.h"
#include "communication/wh.h"

#include "guru2_local.h"
#include "guru2_receipt.h"

#ifdef DEBUG_ONLY_FOR_kagaya
#define D_GURU2_PRINTF_ON	//定義でOS_Printf有効
#endif

//==============================================================================
//	typedef
//==============================================================================
//--------------------------------------------------------------
///	WUSE_SENDWORK
///	通信転送用汎用ワーク。
///	サイズはGURU2_WIDEUSE_SENDWORK_SIZEで固定
//--------------------------------------------------------------
typedef struct
{
	u32 cmd;
	u8 buf[GURU2_WIDEUSE_SENDWORK_SIZE-4];	//-4=cmd size
}WUSE_SENDWORK;

//==============================================================================
//	proto
//==============================================================================
static BOOL guru2Comm_WideUseSendWorkSend(
	GURU2COMM_WORK *g2c, u32 code, const void *buf, u32 size );

static void guru2Comm_DataSend( GURU2COMM_WORK *g2c, int no );

static void Guru2Comm_PokeSelOyaEndTcbSet( GURU2COMM_WORK *g2c );

static const CommPacketTbl DATA_CommPacketTbl[CF_COMMAND_MAX];
static const PTRCommRecvFunc DATA_Guru2CommPacketTbl[G2COMM_MAX];

//==============================================================================
//	ぐるぐる交換　通信コマンド
//==============================================================================
//--------------------------------------------------------------
/**
 * 通信コマンドワーク初期化
 * @param	g2p	GURU2PROC_WORK
 * @retval	g2c	GURU2COMM_WORK
 */
//--------------------------------------------------------------
GURU2COMM_WORK * Guru2Comm_WorkInit( GURU2PROC_WORK *g2p, u32 heap_id )
{
	GURU2COMM_WORK *g2c;
	
	g2c = sys_AllocMemory( heap_id, sizeof(GURU2COMM_WORK) );
	GF_ASSERT( g2c != NULL );
	memset( g2c, 0, sizeof(GURU2COMM_WORK) );
	
	g2c->g2p = g2p;
	
	g2c->recv_poke_party_buf = sys_AllocMemory(
			heap_id, G2MEMBER_MAX*POKEPARTY_SEND_ONCE_SIZE );
	g2c->friend_poke_party_buf = sys_AllocMemory(
			heap_id, G2MEMBER_MAX*POKEPARTY_SEND_ONCE_SIZE );
	
	return( g2c );
}

//--------------------------------------------------------------
/**
 * 通信コマンドワーク削除
 * @param	g2c	GURU2COMM_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
void Guru2Comm_WorkDelete( GURU2COMM_WORK *g2c )
{
	sys_FreeMemoryEz( g2c->recv_poke_party_buf );
	sys_FreeMemoryEz( g2c->friend_poke_party_buf );
	sys_FreeMemoryEz( g2c );
}

//--------------------------------------------------------------
/**
 * 通信コマンド初期化 受信コールバック設定
 * @param	g2c	GURU2COMM_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
void Guru2Comm_CommandInit( GURU2COMM_WORK *g2c )
{
    CommCommandInitialize( DATA_CommPacketTbl, CF_COMMAND_MAX, g2c );
}

//--------------------------------------------------------------
/**
 * ユニオンルーム通信へ戻す
 * @param	g2c	GURU2COMM_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
void Guru2Comm_CommUnionRoomChange( GURU2COMM_WORK *g2c )
{
	CommStateSetLimitNum(2);
	CommStateUnionBconCollectionRestart();
	UnionRoomView_ObjInit( g2c->g2p->param.union_view );
	Union_BeaconChange( UNION_PARENT_MODE_FREE );
}

//==============================================================================
//	送信用汎用ワーク
//==============================================================================
//--------------------------------------------------------------
/**
 * 汎用ワーク送信
 * @param
 * @retval
 */
//--------------------------------------------------------------
static BOOL guru2Comm_WideUseSendWorkSend(
	GURU2COMM_WORK *g2c, u32 cmd, const void *buf, u32 size )
{
	GF_ASSERT( cmd < G2COMM_MAX );
	GF_ASSERT( size+4 <= GURU2_WIDEUSE_SENDWORK_SIZE );
	
	{
		BOOL ret;
		WUSE_SENDWORK *work = (void *)g2c->wideuse_sendwork;
		work->cmd = cmd;
		memcpy( work->buf, buf, size );
		ret = CommSendData( CG_GURU2_CMD, work, GURU2_WIDEUSE_SENDWORK_SIZE );
		
		#ifdef PM_DEBUG
		if( ret == FALSE ){
			#ifdef D_GURU2_PRINTF_ON
			OS_Printf( "ぐるぐる　汎用データ送信失敗 コマンド(%d)\n", cmd );
			#endif
		}
		#endif
		
		return( ret );
	}
}

//==============================================================================
//	送信
//==============================================================================
//--------------------------------------------------------------
/**
 * ぐるぐる交換データ送信
 * @param	g2c		GURU2COMM_WORK
 * @param   code	G2C
 * @param	data	data
 * @param   size	size
 * @retval	BOOL	送信キューに入ったかどうか
 * */
//--------------------------------------------------------------
BOOL Guru2Comm_SendData(
	GURU2COMM_WORK *g2c, u32 code, const void *data, u32 size )
{
	if( g2c->send_stop_flag == TRUE ){	//送信停止
		return( FALSE );
	}
	
	return( guru2Comm_WideUseSendWorkSend(g2c,code,data,size) );
}

//==============================================================================
//	コールバック関数　メイン
//==============================================================================
//--------------------------------------------------------------
/**
 * ぐるぐる交換メインコールバック
 * @param   netID		
 * @param   size		
 * @param   pBuff		
 * @param   pWork		
 * @retval	nothing
 */
//--------------------------------------------------------------
static void CommCB_Guru2Cmd(
		int netID, int size, void* pBuff, void* pWork )
{
	WUSE_SENDWORK *work = pBuff;
	
	if( work->cmd >= G2COMM_MAX ){
		#ifdef D_GURU2_PRINTF_ON
		OS_Printf( "GURU2 CALLBACK ERROR CMD cmd=%d\n", work->cmd );
		#endif
		GF_ASSERT( 0 );
		return;
	}
	
	DATA_Guru2CommPacketTbl[work->cmd]( netID, size, work->buf, pWork );
}

//--------------------------------------------------------------
/**
 * 受付　ポケモンパーティを受信
 * @param   netID		
 * @param   size		
 * @param   pBuff		
 * @param   pWork		
 * @retval  none		
 */
//--------------------------------------------------------------
static void CommCB_Guru2PokeParty(
		int netID, int size, void *pBuff, void *pWork )
{
	void *buf;
	GURU2COMM_WORK *g2c = pWork;
	
	g2c->comm_game_egg_recv_bit |= 1 << netID;
	buf = Guru2Comm_FriendPokePartyGet( g2c, netID );
	memcpy( buf, pBuff, POKEPARTY_SEND_ONCE_SIZE );
	#ifdef D_GURU2_PRINTF_ON
	OS_Printf( "タマゴ受信" );
	#endif
}

//==============================================================================
//	コールバック関数　受付
//==============================================================================
//--------------------------------------------------------------
/**
 * 受付　離脱制御フラグ到着
 * @param   netID		
 * @param   size		
 * @param   pBuff		
 * @param   pWork		
 * @retval	nothing
 */
//--------------------------------------------------------------
static void CommCB_Receipt_BanFlag(
		int netID, int size, void* pBuff, void* pWork )
{
	GURU2COMM_WORK *g2c = pWork;
	u8 *flag = (u8*)pBuff;
	
	if( netID==0 ){					//親
		if( g2c->banFlag != *flag ){
			#ifdef D_GURU2_PRINTF_ON
			OS_Printf( "親機からの離脱禁止フラグ banFlag=%d\n", *flag );
			#endif
		}
		g2c->banFlag = *flag;
	}
}

//--------------------------------------------------------------
/**
 * 受付　データ受信コールバック
 * @param   netID		
 * @param   size		
 * @param   pBuff		
 * @param   pWork		
 * @retval	nothing
 */
//--------------------------------------------------------------
static void CommCB_Receipt_Data(
		int netID, int size, void* pBuff, void* pWork )
{
	GURU2COMM_WORK *g2c = pWork;
	g2c->recv_count++;
	#ifdef D_GURU2_PRINTF_ON
	OS_Printf( "ぐるぐる交換データ受信完了 id=%d\n", netID );
	#endif
}

//------------------------------------------------------------------
/**
 * 受付　ぐるぐる交換　ポケモンセレクト開始　親発信
 * @param   netID		
 * @param   size		
 * @param   pBuff		
 * @param   pWork		
 * @retval  nothing
 */
//------------------------------------------------------------------
static void CommCB_Receipt_Start(
		int netID, int size, void* pBuff, void* pWork )
{
	if( CommGetCurrentID() != 0 ){	//親発信 子のシーケンス変更 bug 0212 fix
		GURU2COMM_WORK *wk = pWork;
	
		wk->recv_count = 0;
		wk->record_execute = TRUE;
	
		//レコードデータ送信開始
		//Guru2Comm_DataSend( wk, CommGetCurrentID() );
		
		//画面上部を「レコードこうかんちゅう！」
		//RecordCornerTitleChange( wk );
		//Guru2Rc_MainSeqForceChange(
		//wk->g2p->g2r, RECORD_MODE_RECORD_SEND_DATA, netID );
		
		Guru2Rc_MainSeqForceChange(
			wk->g2p->g2r, RECORD_MODE_GURU2_POKESEL_START, netID );
		
	//	CommStateSetErrorCheck( TRUE,TRUE ); // ここ以降は切断＝エラー
	}
}

//--------------------------------------------------------------
/**
 * 受付　通信画面を再開する
 * @param   netID		
 * @param   size		
 * @param   pBuff		
 * @param   pWork		
 * @retval	nothing
 */
//--------------------------------------------------------------
static void CommCB_Receipt_ReStart(
		int netID, int size, void* pBuff, void* pWork)
{
	GURU2COMM_WORK *g2c = pWork;
	#ifdef D_GURU2_PRINTF_ON
	OS_Printf("親機からレコード募集再開通知\n");
	#endif
	Guru2Rc_MainSeqForceChange( g2c->g2p->g2r, RECORD_MODE_NEWMEMBER_END, 0 );
}

//--------------------------------------------------------------
/**
 * 受付　子機が離脱。
 * pBuff 子(離脱者)からの送信＝0:離脱許可確認。　1:離脱実行
 * 親からの送信＝上位4ビット：親のshareNum(0xfの場合は離脱NG)
 * @param   netID		
 * @param   size		
 * @param   pBuff		
 * @param   pWork		
 * @retval	nothing
 */
//--------------------------------------------------------------
static void CommCB_Receipt_EndChild(
		int netID, int size, void* pBuff, void* pWork)
{
	GURU2COMM_WORK *g2c = pWork;
	GURU2COMM_END_CHILD_WORK trans_work;
	GURU2COMM_END_CHILD_WORK *recieve_work = pBuff;
	
	//子機(離脱者)からの送信
	if( netID != 0 ){						//離脱します通知
		if( CommGetCurrentID()==0){			// 子機から(親機が受け取る)
			// 子機がいなくなった事を全員に通知する
			trans_work = *recieve_work;
			trans_work.ridatu_id = netID;
			trans_work.oya_share_num = g2c->shareNum;
			
			switch(recieve_work->request){
			case CREC_REQ_RIDATU_CHECK:
				if(g2c->shareNum != CommGetConnectNum() 
					|| g2c->shareNum != Guru2Comm_OnlineNumGet()
					|| g2c->shareNum != MATH_CountPopulation(WH_GetBitmap())){
					trans_work.ridatu_kyoka = FALSE;	//離脱NG！
				}else{
					g2c->ridatu_bit |= 1 << netID;
					trans_work.ridatu_kyoka = TRUE;
					//離脱OKなので参加制限をかける
					//(乱入があればそちら側で制限がはずされるはず)
					CommStateSetLimitNum( CommGetConnectNum() );
				}
				break;
			case CREC_REQ_RIDATU_EXE:
				break;
			}
		
			Guru2Comm_SendData( g2c, G2COMM_RC_END_CHILD,
				&trans_work, sizeof(GURU2COMM_END_CHILD_WORK) );
			#ifdef D_GURU2_PRINTF_ON
			OS_Printf("子機%dから離脱を受け取った→送信\n",netID);
			#endif
		}
	}else{	//親からの送信
		switch(recieve_work->request){
		case CREC_REQ_RIDATU_CHECK:
			//離脱確認なので、離脱しようとした子機にのみ結果を送る
			if(recieve_work->ridatu_id == CommGetCurrentID()){
				if(recieve_work->ridatu_kyoka == FALSE){
					Guru2Rc_MainSeqForceChange(
						g2c->g2p->g2r, RECORD_MODE_END_SELECT_ANSWER_NG,
						recieve_work->ridatu_id );
				}else{
					g2c->oya_share_num = recieve_work->oya_share_num;
					Guru2Rc_MainSeqForceChange(
						g2c->g2p->g2r, RECORD_MODE_END_SELECT_ANSWER_OK,
						recieve_work->ridatu_id );
				}
			}
			
			break;
		case CREC_REQ_RIDATU_EXE:
			#ifdef D_GURU2_PRINTF_ON
			OS_Printf("親機が子機%dの離脱通知した\n",recieve_work->ridatu_id);
			#endif
			Guru2Rc_MainSeqForceChange(
				g2c->g2p->g2r, RECORD_MODE_LOGOUT_CHILD, recieve_work->ridatu_id );
			break;
		}
	}
}

//--------------------------------------------------------------
/**
 *	受付「子機が乱入してきたので一旦絵を送るよ止まってね」
 *	と親機が送信してきた時のコールバック
 * @param   netID		
 * @param   size		
 * @param   pBuff		
 * @param   pWork		
 * @retval  nothing
 */
//--------------------------------------------------------------
static void CommCB_Receipt_Stop(
		int netID, int size, void* pBuff, void* pWork)
{
	GURU2COMM_WORK *wk = pWork;
	u8 id;

	id = *(u8*)pBuff;
	Guru2Rc_MainSeqCheckChange( wk->g2p->g2r, RECORD_MODE_NEWMEMBER, id );

	if(CommGetCurrentID()==0){	// 親機が画像データ送信を開始する
		wk->send_num = 0;
	}
	
	#ifdef D_GURU2_PRINTF_ON
	OS_Printf("親機からの「子機%dに絵を送るから止まってね」通知\n",id);
	#endif
}

//--------------------------------------------------------------
/**
 * 受付　親機がやめるので強制終了させる
 * @param   netID		
 * @param   size		
 * @param   pBuff		
 * @param   pWork		
 * @retval  none		
 */
//--------------------------------------------------------------
static void CommCB_Receipt_End(
	int netID, int size, void* pBuff, void* pWork)
{
	GURU2COMM_WORK *wk = pWork;
	
	#ifdef D_GURU2_PRINTF_ON
	OS_Printf("親機からの終了通知がきたのでやめる\n");
	#endif
	
	// 親機以外はこのコマンドで強制終了
	if( CommGetCurrentID() != 0 ){
		Guru2Rc_MainSeqForceChange( wk->g2p->g2r, RECORD_MODE_FORCE_END, 0  );
	}
}

//--------------------------------------------------------------
/**
 * 受付　３台目・４台目・５台目の子機が「絵をちょーだい」と言う
 * @param   netID		
 * @param   size		
 * @param   pBuff		
 * @param   pWork		
 * @retval  none		
 */
//--------------------------------------------------------------
static void CommCB_Receipt_ChildJoin(
	int netID, int size, void* pBuff, void* pWork )
{
	u8 id;
	GURU2COMM_WORK *g2c = pWork;
	
	// 親機が受け取ったら(というか親しか受け取らないけど）
	if( CommGetCurrentID() == 0 ){
		id  = netID;
		// 全台に「これから絵を送るので止まってください」と送信する
		Guru2Comm_SendData( g2c, G2COMM_RC_STOP, &id, 1 );
		#ifdef D_GURU2_PRINTF_ON
		OS_Printf("子機(%d = %d)からの乱入／絵のください通知\n",id,netID);
		#endif
	}
}

//==============================================================================
//	コールバック関数　ゲームメイン
//==============================================================================
//--------------------------------------------------------------
/**
 * ゲームメイン　シグナル受信
 * @param   netID		
 * @param   size		
 * @param   pBuff		
 * @param   pWork		
 * @retval  none		
 */
//--------------------------------------------------------------
static void CommCB_Main_Signal(
		int netID, int size, void *pBuff, void *pWork )
{
	u16 *buff = pBuff;
	GURU2COMM_WORK *wk = pWork;
	wk->comm_game_signal_bit |= *buff;
	
	#ifdef D_GURU2_PRINTF_ON
	OS_Printf("ぐるぐる シグナル(0x%x)受信 受信ID=0x%x\n",*buff,netID);
	#endif
}

//--------------------------------------------------------------
/**
 * ゲームメイン　参加
 * @param   netID		
 * @param   size		
 * @param   pBuff		
 * @param   pWork		
 * @retval  none		
 */
//--------------------------------------------------------------
static void CommCB_Main_Join(
		int netID, int size, void *pBuff, void *pWork )
{
	GURU2COMM_WORK *wk = pWork;
	wk->comm_game_join_bit |= 1 << netID;
	#ifdef D_GURU2_PRINTF_ON
	OS_Printf( "ぐるぐる ゲーム参加受け取り ID= 0x%d\n", netID );
	#endif
}

//--------------------------------------------------------------
/**
 * ゲームメイン　プレイ番号を受信
 * @param   netID		
 * @param   size		
 * @param   pBuff		
 * @param   pWork		
 * @retval  none		
 */
//--------------------------------------------------------------
static void CommCB_Main_PlayNo(
		int netID, int size, void *pBuff, void *pWork )
{
	GURU2COMM_WORK *g2c = pWork;
	GURU2COMM_PLAYNO *no = pBuff;
	Guru2Main_CommPlayNoDataSet( g2c->g2p->g2m, no );
}

//--------------------------------------------------------------
/**
 * ゲームメイン　プレイ最大数を受信
 * @param   netID		
 * @param   size		
 * @param   pBuff		
 * @param   pWork		
 * @retval  none		
 */
//--------------------------------------------------------------
static void CommCB_Main_PlayMax(
		int netID, int size, void *pBuff, void *pWork )
{
	int max = *(int *)pBuff;
	
	GURU2COMM_WORK *g2c = pWork;
	Guru2Main_CommPlayMaxSet( g2c->g2p->g2m, max );
}

//--------------------------------------------------------------
/**
 * ゲーム　ボタン情報を受信
 * @param   netID		
 * @param   size		
 * @param   pBuff		
 * @param   pWork		
 * @retval  none		
 */
//--------------------------------------------------------------
static void CommCB_Main_Button(
		int netID, int size, void *pBuff, void *pWork )
{
	u8 btn = *(u8*)pBuff;
	GURU2COMM_WORK *g2c = pWork;
	Guru2Main_CommButtonSet( g2c->g2p->g2m, btn );
}

//--------------------------------------------------------------
/**
 * ゲーム　ゲーム情報を受信
 * @param   netID		
 * @param   size		
 * @param   pBuff		
 * @param   pWork		
 * @retval  none		
 */
//--------------------------------------------------------------
static void CommCB_Main_GameData(
		int netID, int size, void *pBuff, void *pWork )
{
	if( CommGetCurrentID() != 0 ){	//発信した親以外
		GURU2COMM_WORK *g2c = pWork;
		GURU2COMM_GAMEDATA *data = pBuff;
		Guru2Main_CommGameDataSet( g2c->g2p->g2m, data );
	}
}

//--------------------------------------------------------------
/**
 * ゲームメイン　交換位置を受信
 * @param   netID		
 * @param   size		
 * @param   pBuff		
 * @param   pWork		
 * @retval  none		
 */
//--------------------------------------------------------------
static void CommCB_Main_TradeNo(
		int netID, int size, void *pBuff, void *pWork )
{
	int no = *(int *)pBuff;
	GURU2COMM_WORK *g2c = pWork;
	Guru2Main_TradeNoSet( g2c->g2p->g2m, netID, no );
}

//--------------------------------------------------------------
/**
 * ゲームメイン　ゲーム結果を受信
 * @param   netID		
 * @param   size		
 * @param   pBuff		
 * @param   pWork		
 * @retval  none		
 */
//--------------------------------------------------------------
static void CommCB_Main_GameResult(
		int netID, int size, void *pBuff, void *pWork )
{
	GURU2COMM_WORK *g2c = pWork;
	GURU2COMM_GAMERESULT *result = pBuff;
	Guru2Main_GameResultSet( g2c->g2p->g2m, result );
}

//--------------------------------------------------------------
/**
 * ゲームメイン　卵データ異常受信
 * @param   netID		
 * @param   size		
 * @param   pBuff		
 * @param   pWork		
 * @retval  none		
 */
//--------------------------------------------------------------
static void CommCB_Main_EggDataNG(
		int netID, int size, void *pBuff, void *pWork )
{
	u32 bit = 1 << netID;
	GURU2COMM_WORK *g2c = pWork;
	g2c->comm_game_egg_check_error_bit |= bit;
}

//--------------------------------------------------------------
/**
 * ゲームメイン　卵データ正常受信
 * @param   netID
 * @param   size
 * @param   pBuff
 * @param   pWork
 * @retval  none
 */
//--------------------------------------------------------------
static void CommCB_Main_EggDataOK(
		int netID, int size, void *pBuff, void *pWork )
{
	u32 bit = 1 << netID;
	GURU2COMM_WORK *g2c = pWork;
	g2c->comm_game_egg_check_ok_bit |= bit;
}

//==============================================================================
//	サイズ
//==============================================================================
//--------------------------------------------------------------
///	GURU2_WIDEUSE_SENDWORK_SIZE
//--------------------------------------------------------------
static int _getGuru2WideUseSendWorkSize( void )
{
	return( GURU2_WIDEUSE_SENDWORK_SIZE );
}

//--------------------------------------------------------------
///	GURU2COMM_END_CHILD_WORK
//--------------------------------------------------------------
static int _getCRECW(void)
{
	return sizeof(GURU2COMM_END_CHILD_WORK);
}

//--------------------------------------------------------------
///	POKEPARTY_SEND_ONCE_SIZE
//--------------------------------------------------------------
static int _getPokePartyOnceSize( void )
{
	return POKEPARTY_SEND_ONCE_SIZE;
}

//==============================================================================
//	受信バッファ
//==============================================================================
//--------------------------------------------------------------
/**
 * ポケモンデータ受信バッファ
 * @param
 * @retval
 */
//--------------------------------------------------------------
static u8 * _getPokePartyRecvBuff( int netID, void *pWork, int size )
{
	u32 buf;
	GURU2COMM_WORK *g2c = pWork;
	
	buf = (u32)(g2c->recv_poke_party_buf);
	buf += netID * POKEPARTY_SEND_ONCE_SIZE;
	return( (u8*)buf );
}

//==============================================================================
//	パーツ
//==============================================================================
//------------------------------------------------------------------
/**
 * 現在のオンライン数を取得
 * @param   nothing
 * @retval  int		オンライン数
 */
//------------------------------------------------------------------
int Guru2Comm_OnlineNumGet( void )
{
	int i,result;
	
	for(result=0,i=0;i<G2MEMBER_MAX;i++){
		if(CommInfoGetMyStatus(i)!=NULL){
			result++;
		}
	}
	return result;
}

//------------------------------------------------------------------
/**
 * $brief   グラフィックデータを分割送信
 * @param   g2c
 * @param   no		
 * @retval  none		
 */
//------------------------------------------------------------------
static void guru2Comm_DataSend( GURU2COMM_WORK *g2c, int no )
{
#if 0
	//xor取得・格納
	{
		int i;
		u32 *p,result;
		p = (u32*)g2c->send_data.data;
		for(result=0,i=0;i<sizeof(GURU2COMM_RC_SEND_DATABUF_SIZE)/4;i++){
			result ^= p[i];
		}
		g2c->send_data._xor        = result;
	}
	
	// 分割番号を書き込む(親機の値だけが使用される）
	g2c->send_data.seed = gf_rand();
	
	// 送信開始
	CommSendHugeData(
		G2COMM_RC_CMD, &g2c->send_data, sizeof(GURU2COMM_RC_SENDDATA) );
	
	OS_Printf("送信データ no=%d, xor = %08x\n",
				g2c->send_data.seed,  g2c->send_data._xor );
#endif
}

//--------------------------------------------------------------
/**
 * ポケモンパーティ受信済みバッファを返す
 * @param
 * @retval
 */
//--------------------------------------------------------------
void * Guru2Comm_FriendPokePartyGet( GURU2COMM_WORK *g2c, int id )
{
	u32 buf;
	buf = (u32)(g2c->friend_poke_party_buf);
	buf += id * POKEPARTY_SEND_ONCE_SIZE;
	return( (void*)buf );
}

//==============================================================================
//	data
//==============================================================================
//--------------------------------------------------------------
///	通信コールバック関数
//--------------------------------------------------------------
static const CommPacketTbl DATA_CommPacketTbl[CF_COMMAND_MAX] =
{
	// comm_command_field.cで登録されているコマンドテーブルの無効化
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
	
	// レコードが使用するコマンド部分
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
	
	// おえかきのコマンドを無視するための部分
	{ CommDummyCallBack, _getZero, NULL},
	{ CommDummyCallBack, _getZero, NULL},
	{ CommDummyCallBack, _getZero, NULL},
	{ CommDummyCallBack, _getZero, NULL},
	{ CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
    { CommDummyCallBack, _getZero, NULL},
	{ CommDummyCallBack, _getZero, NULL},
	{ CommDummyCallBack, _getZero, NULL},
	
	//ここからぐるぐる
	{CommCB_Guru2Cmd, _getGuru2WideUseSendWorkSize, NULL }, //CG_GURU2_CMD
	{CommCB_Guru2PokeParty,_getPokePartyOnceSize,_getPokePartyRecvBuff}, //CG_GURU2_EGG
};

//--------------------------------------------------------------
///	ぐるぐる専用通信コールバック関数
//--------------------------------------------------------------
static const PTRCommRecvFunc DATA_Guru2CommPacketTbl[G2COMM_MAX] =
{
	CommCB_Receipt_Stop,
	CommCB_Receipt_ReStart,
	CommCB_Receipt_EndChild,
	CommCB_Receipt_End,
	CommCB_Receipt_ChildJoin,
	CommCB_Receipt_Start,
	CommCB_Receipt_Data,
	CommCB_Receipt_BanFlag,
	
	CommCB_Main_Signal,
	CommCB_Main_Join,
	CommCB_Main_Button,
	CommCB_Main_GameData,
	CommCB_Main_PlayNo,
	CommCB_Main_PlayMax,
	CommCB_Main_TradeNo,
	CommCB_Main_GameResult,
	CommCB_Main_EggDataNG,
	CommCB_Main_EggDataOK,
};

