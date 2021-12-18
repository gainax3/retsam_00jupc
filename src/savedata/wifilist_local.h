//============================================================================================
/**
 * @file	wifilist_local.h
 * @brief	Wifiともだちコードリスト定義（「しりあいグループ」は別、wifi上での情報のみ）
 * @author	mori GAME FREAK inc.
 * @date	2006.03.02
 */
//============================================================================================
#ifndef __WIFILIST_LOCAL_H__
#define __WIFILIST_LOCAL_H__

typedef struct{
    STRCODE         groupName[PERSON_NAME_SIZE + EOM_SIZE]; // 16
    STRCODE 		name[PERSON_NAME_SIZE + EOM_SIZE];		// 32
	u32				id;										// 36


//	u16		battle_count;							// 38
	u16		battle_win;								// 40
	u16		battle_lose;							// 42

	u16     trade_num;								// 44

    u16     year;    //最後に対戦した年月日 まだの場合は０００  //46
    u8     month;	                             //47								
    u8     day;
    u8     sex;
    u8     unionGra;
	u16		pofin_num;							// 52	0～9999なのでu16でOK
	u16		minigame_ballslow;					// 56
	u16		minigame_balanceball;				// 58
	u16		minigame_balloon;					// 60
}WIFI_FRIEND;

struct _WIFI_LIST{
	DWCUserData		my_dwcuser;							// 64
	DWCFriendData	friend_dwc[WIFILIST_FRIEND_MAX];	// 64+12*32 = 448

	WIFI_FRIEND	friend[WIFILIST_FRIEND_MAX];		// 448+ 56*32 = 2240 = 0x8C0
};



#endif
