#ifndef __WIFI_P2PMATCHFOUR_LOCAL_H__
#define __WIFI_P2PMATCHFOUR_LOCAL_H__

#define WIFI_P2PMATCH_NUM_MAX	(4)	// 最大４人マッチング

enum{
	WFP2PMF_CON_NG,		// 通信NG
	WFP2PMF_CON_OK,		// 通信OK
	WFP2PMF_CON_LOCK,	// 今親が忙しい
};

typedef struct _WFP2P_WK WFP2P_WK;


// 通信データ
typedef struct {
	u16 netID;	// ネットID
	u32 flag;	// WFP2PMF_CON_～
} WFP2PMF_COMM_RESULT;

// VCHATデータ
typedef struct {
	u8 vchat_now[WIFI_P2PMATCH_NUM_MAX];
} WFP2PMF_COMM_VCHAT;

extern void WFP2PMF_CommResultRecv( WFP2P_WK* p_wk, const WFP2PMF_COMM_RESULT* cp_data );
extern void WFP2PMF_CommStartRecv( WFP2P_WK* p_wk );
extern void WFP2PMF_CommVchatRecv( WFP2P_WK* p_wk, const WFP2PMF_COMM_VCHAT* cp_data );



#endif		// __WIFI_P2PMATCHFOUR_LOCAL_H__
