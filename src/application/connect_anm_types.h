//==============================================================================
/**
 * @file	connect_anm_types.h
 * @brief	Wifi接続画面のBGアニメの構造体定義など
 * @author	matsuda
 * @date	2007.12.26(水)
 */
//==============================================================================
#ifndef __CONNECT_ANM_TYPES_H__
#define __CONNECT_ANM_TYPES_H__


//==============================================================================
//	定数定義
//==============================================================================
///nclファイル上のパレットアニメを開始させるパレット開始位置
#define CBP_PAL_START_NUMBER	(0)
///パレットアニメ対象パレット本数
#define CBP_PAL_NUM			(4)
///アニメ転送パレット本数
#define CBP_TRANS_PAL_NUM	(1)

///パレットアニメ対象パレットの中に含まれている実際にアニメ対象のカラー開始位置
#define CBP_PAL_COLOR_START	(1)	//CBP_PAL_COLOR_START ～ (CBP_PAL_COLOR_START + CBP_PAL_COLOR_NUM)
///パレットアニメ対象パレットの中に含まれている実際にアニメ対象のカラー数
#define CBP_PAL_COLOR_NUM	(15)

///EVY加算値(下位8ビット小数)
#define CBP_ADD_EVY			(0x0300)

///EVYパターン数
#define CBP_EVY_ANM_NUM		((16<<8) / CBP_ADD_EVY + 2)	// +1 = 端数分 +1=0の分
///EVYパターン数(全アニメパターン数)
#define CBP_EVY_TBL_ALL		(CBP_EVY_ANM_NUM * (CBP_PAL_NUM - 1))


//==============================================================================
//	構造体定義
//==============================================================================
///Wifi接続BGのパレットアニメ制御構造体
typedef struct{
	TCB_PTR tcb;
	BOOL occ;							///<TRUE:有効　FALSE:無効
	u16 src_color[CBP_PAL_NUM][16];		///<元パレット
	u16 dest_color[CBP_EVY_TBL_ALL][16];	///<転送パレット
	
	s16 trans_pos;
	s8 trans_dir;
	u8 intr_count;
}CONNECT_BG_PALANM;


#endif	//__CONNECT_ANM_TYPES_H__
