//==============================================================================
/**
 * @file	footprint_comm.h
 * @brief	足跡ボードの通信送受信系のヘッダ
 * @author	matsuda
 * @date	2008.01.22(火)
 */
//==============================================================================
#ifndef __FOOTPRINT_COMM_H__
#define __FOOTPRINT_COMM_H__


//==============================================================================
//	構造体定義
//==============================================================================
///プレイヤー入室時に送信するパラメータ
typedef struct{
	STAMP_PARAM stamp;	//最初に表示しておくスタンプの種類を入れておく
}FOOTPRINT_IN_PARAM;



//==============================================================================
//	外部関数宣言
//==============================================================================
extern void Footprint_Comm_Init(FOOTPRINT_SYS_PTR fps);

//--------------------------------------------------------------
//	送信命令
//--------------------------------------------------------------
extern BOOL Footprint_Send_Stamp(const STAMP_PARAM *stamp);
extern BOOL Footprint_Send_PlayerIn(const FOOTPRINT_IN_PARAM *in_para);
extern BOOL Footprint_Send_PlayerOut(void);


#endif	//__FOOTPRINT_COMM_H__
