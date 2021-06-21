//==============================================================================
/**
 * @file	bb_comm_cmd.c
 * @brief	簡単な説明を書く
 * @author	goto
 * @date	2007.09.25(火)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================

#include "common.h"
#include "communication/communication.h"


#include "bb_common.h"
#include "bb_comm_cmd.h"

//==============================================================
// Prototype
//==============================================================
static void CommBB_Start( int netID, int size, void* pBuff, void* pWork );
static void CommBB_End( int netID, int size, void* pBuff, void* pWork );
static void CommBB_State( int netID, int size, void* pBuff, void* pWork );
static void CommBB_Result( int netID, int size, void* pBuff, void* pWork );
static void CommBB_MemberResult( int netID, int size, void* pBuff, void* pWork );
static int CommBB_SizeZero_Get( void );
static int CommBB_SizeRule_Get( void );
static int CommBB_SizeState_Get( void );
static int CommBB_SizeResult_Get( void );
static int CommBB_SizeMemberResult_Get( void );


static void CommBB_Start( int netID, int size, void* pBuff, void* pWork )
{
	BB_WORK* wk = pWork;
	
	wk->rule = *( ( BB_RULE_COMM* )pBuff );
	
	gf_srand( wk->rule.rand );
	
	BB_cmd_StartSet( wk );
	
//	OS_Printf( " --- 乱数設定 = %d\n", wk->rule.rand );
//	OS_Printf( " --- start   netid = %2d\n", netID );
}


static void CommBB_End( int netID, int size, void* pBuff, void* pWork )
{
	BB_WORK* wk = pWork;
	
	BB_cmd_EndSet( wk );
	
//	OS_Printf( " --- end     netid = %2d\n", netID );
}


static void CommBB_State( int netID, int size, void* pBuff, void* pWork )
{
	BB_WORK* wk = pWork;
	BB_STATE_COMM* bsc;
	
	bsc = ( ( BB_STATE_COMM* )pBuff );
	
	wk->p_client->state[ netID ].state = bsc->state;

//	OS_Printf( " --- state   netid = %2d\n", netID );
}


static void CommBB_Result( int netID, int size, void* pBuff, void* pWork )
{
	BB_WORK* wk = pWork;
	BB_RESULT_COMM* brc;
	
	brc = ( ( BB_RESULT_COMM* )pBuff );
	wk->mem_result.result[ netID ].score = brc->score;
	
	OS_Printf( " --- result  netid = %2d %2d\n", netID, brc->score );
}


static void CommBB_MemberResult( int netID, int size, void* pBuff, void* pWork )
{
	BB_WORK* wk = pWork;
	
//	OS_Printf( " --- mresult netid = %2d\n", netID );
}



static int CommBB_SizeZero_Get( void )
{
	return 0;
}

static int CommBB_SizeRule_Get( void )
{
	return sizeof( BB_RULE_COMM );
}

static int CommBB_SizeState_Get( void )
{
	return sizeof( BB_STATE_COMM );
}

static int CommBB_SizeResult_Get( void )
{
	return sizeof( BB_RESULT_COMM );
}

static int CommBB_SizeMemberResult_Get( void )
{
	return sizeof( BB_MENBER_RESULT_COMM );
}


// -----------------------------------------
//
//
//	□　コールバック
//
//
// -----------------------------------------
static const CommPacketTbl _CommBBTbl[] = {	
	{ CommBB_Start,			CommBB_SizeRule_Get,		 NULL },
	{ CommBB_End,			CommBB_SizeZero_Get,		 NULL },
	{ CommBB_State,			CommBB_SizeState_Get,		 NULL },
	{ CommBB_Result,		CommBB_SizeResult_Get,		 NULL },
	{ CommBB_MemberResult,	CommBB_SizeMemberResult_Get, NULL },
};

//--------------------------------------------------------------
/**
 * @brief	コマンドを取得
 *
 * @param	none	
 *
 * @retval	const CommPacketTbl*	
 *
 */
//--------------------------------------------------------------
const CommPacketTbl* BB_CommCommandTclGet( void )
{
	return _CommBBTbl;
}


//--------------------------------------------------------------
/**
 * @brief	コマンドの要素数取得
 *
 * @param	none	
 *
 * @retval	int	
 *
 */
//--------------------------------------------------------------
int BB_CommCommandTblNumGet( void )
{
	return sizeof( _CommBBTbl ) / sizeof( CommPacketTbl );
}


//--------------------------------------------------------------
/**
 * @brief	通信初期化
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void  BB_CommCommandInit( BB_WORK* wk )
{
	CommCommandInitialize( BB_CommCommandTclGet(), BB_CommCommandTblNumGet(), wk );
}

