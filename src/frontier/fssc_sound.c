//============================================================================================
/**
 * @file	fssc_sound.c
 * @bfief	フロンティアシステムスクリプトコマンド：サウンド
 * @author	Satoshi Nohara
 * @date	07.04.10
 */
//============================================================================================
#include "common.h"
#include "system/lib_pack.h"
#include "system/snd_tool.h"

#include "frontier_types.h"
#include "frontier_main.h"
#include "frontier_scr.h"
#include "frontier_tool.h"
#include "frontier_scrcmd.h"
#include "fss_task.h"


//============================================================================================
//
//	プロロトタイプ宣言
//
//============================================================================================
static BOOL EvWaitSe(FSS_TASK * core);
static BOOL EvWaitMe( FSS_TASK * core );

BOOL FSSC_SePlay( FSS_TASK * core );
BOOL FSSC_SeStop( FSS_TASK * core );
BOOL FSSC_SeWait(FSS_TASK * core);
BOOL FSSC_MePlay( FSS_TASK * core );
BOOL FSSC_MeWait( FSS_TASK * core );
BOOL FSSC_BgmPlay( FSS_TASK * core );
BOOL FSSC_BgmStop( FSS_TASK * core );


//============================================================================================
//
//	コマンド
//
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * ＳＥを鳴らす
 *
 * @param	core		仮想マシン制御構造体へのポインタ
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
BOOL FSSC_SePlay( FSS_TASK * core )
{
	Snd_SePlay( FSSTGetWorkValue(core) );
	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * ＳＥを止める
 *
 * @param	core		仮想マシン制御構造体へのポインタ
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
BOOL FSSC_SeStop( FSS_TASK * core )
{
	Snd_SeStopBySeqNo( FSSTGetWorkValue(core), 0 );
	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * ＳＥ終了待ち
 *
 * @param	core		仮想マシン制御構造体へのポインタ
 *
 * @return	常に1
 *
 * @li		雨などのループ音で、無限ループになってしまうので、SEナンバー指定して、それをチェック！
 */
//--------------------------------------------------------------------------------------------
BOOL FSSC_SeWait(FSS_TASK * core)
{
	//仮想マシンの汎用レジスタにSEナンバーを格納
	core->reg[0] = FSSTGetWorkValue(core);
	FSST_SetWait( core, EvWaitSe );
	return 1;
}

//return 1 = 終了
static BOOL EvWaitSe(FSS_TASK * core)
{
	//if( Snd_SePlayCheckAll() == 0 ){
	if( Snd_SePlayCheck(core->reg[0]) == 0 ){
		return TRUE;
	}

	return FALSE;
};

//--------------------------------------------------------------------------------------------
/**
 * ME再生
 *
 * @param	core		仮想マシン制御構造体へのポインタ
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
BOOL FSSC_MePlay( FSS_TASK * core )
{
	Snd_MePlay( FSSTGetU16(core) );
	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * ME終了待ち
 *
 * @param	core		仮想マシン制御構造体へのポインタ
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
BOOL FSSC_MeWait( FSS_TASK * core )
{
	FSST_SetWait( core, EvWaitMe );
	return 1;
}

//return 1 = 終了
static BOOL EvWaitMe( FSS_TASK * core )
{
	//終了待ちのみ
	//if( Snd_MePlayCheck() == 0 ){
	
	//終了待ち＋ウェイトワーク待ち
	if( Snd_MePlayCheckBgmPlay() == 0 ){ 
		return TRUE;
	}

	return FALSE;
};

//--------------------------------------------------------------------------------------------
/**
 * BGM再生
 *
 * @param	core		仮想マシン制御構造体へのポインタ
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
BOOL FSSC_BgmPlay( FSS_TASK * core )
{
	Snd_BgmPlay( FSSTGetU16(core) );
	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * BGMを止める
 *
 * @param	core		仮想マシン制御構造体へのポインタ
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
BOOL FSSC_BgmStop( FSS_TASK * core )
{
	u16 music = FSSTGetU16(core);	//"未使用"
	//Snd_BgmStop( music, 0 );
	Snd_BgmStop( Snd_NowBgmNoGet(), 0 );
	return 0;
}


