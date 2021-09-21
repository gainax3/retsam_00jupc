//=============================================================================
/**
 * @file	comm_command_contest_ol.c
 * @brief	comm_command_contest.cからコンテスト領域にオーバーレイしても問題ないものを
 *			抜き出したファイルです。
 * @author	matsuda
 * @date    2007.12.04(火)
 */
//=============================================================================
#ifndef __COMM_COMMAND_CONTEST_OL_H__
#define __COMM_COMMAND_CONTEST_OL_H__


//==============================================================================
//	外部関数宣言
//==============================================================================
extern void CommContestRecvNormal(int id_no,int size,void *pData,void *work);
extern void CommContestRecvDance(int id_no,int size,void *pData,void *work);



#endif	//__COMM_COMMAND_CONTEST_OL_H__
