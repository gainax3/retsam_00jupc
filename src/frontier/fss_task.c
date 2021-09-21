//==============================================================================
/**
 * @file	fss_task.c
 * @brief	フロンティア用スクリプト仮想マシン
 * @author	matsuda
 * @date	2007.03.29(木)
 */
//==============================================================================
#include "common.h"
#include "frontier_types.h"
#include "fss_task.h"

//==============================================================================
//	定数定義
//==============================================================================
// 仮想マシンの動作状態定義
enum{
	FSSTSTAT_READY,		// 停止(動作終了）
	FSSTSTAT_RUN,		// 動作中
	FSSTSTAT_WAIT,		// 待ち状態（チェックルーチン呼び出し）
};


//==============================================================================
//	プロトタイプ宣言
//==============================================================================
void FSST_Init( FSS_TASK * core, const FRSCR_CMD * cmd_tbl, u32 cmd_max);
u8 FSST_Start( FSS_TASK * core, const FSS_CODE * start );
void FSST_SetWait( FSS_TASK * core, FSST_WAIT_FUNC func );
void FSST_End( FSS_TASK * core );
void FSST_SetWork( FSS_TASK * core, void * work );
u8 FSST_Control( FSS_TASK * core );
u8 FSSTStackPush( FSS_TASK * core, const FSS_CODE * val );
const FSS_CODE * FSSTStackPop( FSS_TASK * core );
void FSSTJump( FSS_TASK * core, FSS_CODE * adrs );
void FSSTCall( FSS_TASK * core, FSS_CODE * adrs );
void FSSTRet( FSS_TASK * core );
u16 FSSTGetU16( FSS_TASK * core );
u32 FSSTGetU32( FSS_TASK * core );



//--------------------------------------------------------------------------------------------
/**
 * 仮想マシン初期化
 *
 * @param	core		仮想マシン制御構造体へのポインタ
 * @param	cmd_tbl		命令テーブル開始アドレス
 * @param	cmd_max		命令テーブルの大きさ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void FSST_Init( FSS_TASK * core, const FRSCR_CMD * cmd_tbl, u32 cmd_max)
{
	u32	i;

	core->status = FSSTSTAT_READY;
	core->PC = NULL;
	core->SP = 0;
	core->routine = NULL;
	core->command_table = cmd_tbl;
	core->cmd_max = cmd_max;
	for( i=0; i<FSW_REG_WORK_MAX; i++ ){
		core->reg[i] = 0;
	}
	for( i=0; i<FSST_STACK_MAX; i++ ){
		core->array[i] = NULL;
	}

//	core->event_work = NULL;		//(06.07.20)
}

//--------------------------------------------------------------------------------------------
/**
 * 仮想マシンにコードを設定
 *
 * @param	core		仮想マシン制御構造体へのポインタ
 * @param	start		実行コードの開始アドレス
 *
 * @return	常にTRUE
 */
//--------------------------------------------------------------------------------------------
u8 FSST_Start( FSS_TASK * core, const FSS_CODE * start )
{
	core->PC = start;
	core->status = FSSTSTAT_RUN;

	return TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * 仮想マシンをウェイト状態に設定
 *
 * @param	core		仮想マシン制御構造体へのポインタ
 * @param	func		ウェイト関数
 *
 * @return	none
 *
 * @li	TRUEが返ってくるまでウェイト関数を毎回呼びだす
 */
//--------------------------------------------------------------------------------------------
void FSST_SetWait( FSS_TASK * core, FSST_WAIT_FUNC func )
{
	core->status = FSSTSTAT_WAIT;
	core->routine = func;
}

//--------------------------------------------------------------------------------------------
/**
 * 仮想マシン実行終了
 *
 * @param	core		仮想マシン制御構造体へのポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void FSST_End( FSS_TASK * core )
{
	core->status = FSSTSTAT_READY;
	core->PC = NULL;
}

//--------------------------------------------------------------------------------------------
/**
 * コマンドなどで参照するワークをセット
 *
 * @param	core		仮想マシン制御構造体へのポインタ
 * @param	work		ワークのポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void FSST_SetWork( FSS_TASK * core, void * work )
{
//	core->event_work = work;
}

//--------------------------------------------------------------------------------------------
/**
 * 仮想マシン制御メイン
 *
 * @param	core		仮想マシン制御構造体へのポインタ
 *
 * @retval	"TRUE = 実行中"
 * @retval	"FALSE = 停止中・実行終了"
 */
//--------------------------------------------------------------------------------------------
u8 FSST_Control( FSS_TASK * core )
{
	u16 code;

	switch( core->status ){
	case FSSTSTAT_READY:
		return FALSE;

	case FSSTSTAT_WAIT:
		if( core->routine != NULL ){
			if( core->routine(core) == TRUE ){
				core->status = FSSTSTAT_RUN;
				core->routine = NULL;
			}
			return TRUE;
		}
		core->status = FSSTSTAT_RUN;
		/* FALL THROUGH */
		//break;
		
	case FSSTSTAT_RUN:
		while( TRUE ){
			if( core->PC == NULL ){
				core->status = FSSTSTAT_READY;
				return FALSE;
			}

			code = FSSTGetU16( core );

			//OS_Printf( "*********code = %d\n", code );
			if( code >= core->cmd_max ) {
				GF_ASSERT_MSG(0, "command error %04x:%08x\n",code, core->PC - 2);
				core->status = FSSTSTAT_READY;
				return FALSE;
			}
			if( core->command_table[code]( core ) == 1 ){
				break;
			}
		}
		break;
	}
	return TRUE;
}


//============================================================================================
// 仮想マシン制御用サブルーチン
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * 仮想マシンスタックプッシュ
 *
 * @param	core		仮想マシン制御構造体へのポインタ
 * @param	val
 *
 * @retval	"0 = 正常"
 * @retval	"1 = エラー"
 */
//--------------------------------------------------------------------------------------------
u8 FSSTStackPush( FSS_TASK * core, const FSS_CODE * val )
{
	if( core->SP + 1 >= FSST_STACK_MAX ){ return 1; }
	core->array[core->SP] = val;
	core->SP ++;

	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * 仮想マシンスタックポップ
 *
 * @param	core		仮想マシン制御構造体へのポインタ
 *
 * @return
 */
//--------------------------------------------------------------------------------------------
const FSS_CODE * FSSTStackPop( FSS_TASK * core )
{
	if( core->SP == 0 ){ return NULL; }
	core->SP --;

	return core->array[core->SP];
}

//--------------------------------------------------------------------------------------------
/**
 * 仮想マシンジャンプ命令
 *
 * @param	core		仮想マシン制御構造体へのポインタ
 * @param	adrs		ジャンプ先アドレス
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void FSSTJump( FSS_TASK * core, FSS_CODE * adrs )
{
	core->PC = adrs;
}

//--------------------------------------------------------------------------------------------
/**
 * 仮想マシンコール命令
 *
 * @param	core		仮想マシン制御構造体へのポインタ
 * @param	adrs		呼び出すアドレス
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void FSSTCall( FSS_TASK * core, FSS_CODE * adrs )
{
	FSSTStackPush( core, core->PC );
	core->PC = adrs;
}

//--------------------------------------------------------------------------------------------
/**
 * 仮想マシンリターン命令
 *
 * @param	core		仮想マシン制御構造体へのポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void FSSTRet( FSS_TASK * core )
{
	core->PC = FSSTStackPop( core );
}


//--------------------------------------------------------------------------------------------
/**
 * PCのアドレスから16bit(2byte)データ取得
 *
 * @param	core		仮想マシン制御構造体へのポインタ
 *
 * @return	取得データ
 */
//--------------------------------------------------------------------------------------------
u16 FSSTGetU16( FSS_TASK * core )
{
	u16	val;

	val = (u16)FSSTGetU8( core );
	val += (u16)FSSTGetU8( core ) << 8;
	return val;
}

//--------------------------------------------------------------------------------------------
/**
 * PCのアドレスから32bit(4byte)データ取得
 *
 * @param	core		仮想マシン制御構造体へのポインタ
 *
 * @return	取得データ
 */
//--------------------------------------------------------------------------------------------
u32 FSSTGetU32( FSS_TASK * core )
{
	u32	val;
	u8	a,b,c,d;

	a = FSSTGetU8( core );
	b = FSSTGetU8( core );
	c = FSSTGetU8( core );
	d = FSSTGetU8( core );

	val = 0;
	val += (u32)d;
	val <<= 8;
	val += (u32)c;
	val <<= 8;
	val += (u32)b;
	val <<= 8;
	val += (u32)a;

	return val;
}

