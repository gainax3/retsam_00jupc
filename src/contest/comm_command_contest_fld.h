//==============================================================================
/**
 * @file	comm_command_contest_fld.h
 * @brief	comm_command_contest.cからフィールドマップ領域にオーバーレイしても問題ないものを
 *			抜き出したファイルです。
 * @author	matsuda
 * @date    2007.12.04(火)
 */
//==============================================================================
#ifndef __COMM_COMMAND_CONTEST_FLD_H__
#define __COMM_COMMAND_CONTEST_FLD_H__


//==============================================================================
//	外部関数宣言
//==============================================================================
extern void CommContestRecvFastParam(int id_no,int size,void *pData,void *work);
extern void CommContestRecvPokemonParam(int id_no,int size,void *pData,void *work);
extern void CommContestRecvPokemonParamAll(int id_no,int size,void *pData,void *work);
extern void CommContestRecvBreederData(int id_no,int size,void *pData,void *work);
extern void CommContestRecvJudgeData(int id_no,int size,void *pData,void *work);
extern void CommContestRecvNameData(int id_no,int size,void *pData,void *work);
extern void CommContestRecvPerap(int id_no,int size,void *pData,void *work);
extern void CommContestRecvEtcData(int id_no,int size,void *pData,void *work);



#endif	//__COMM_COMMAND_CONTEST_FLD_H__
