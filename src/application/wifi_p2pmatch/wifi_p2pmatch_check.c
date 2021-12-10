//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wifi_p2pmatch_check.c
 *	@brief		Wi-Fiクラブ	モードチェック
 *	@author		tomoya takahashi
 *	@data		2008.06.25
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "common.h"

#include "communication/comm_state.h"

#include "wifi_p2pmatch_local.h"
#include "wifi_p2pmatch_check.h"

//-----------------------------------------------------------------------------
/**
 *					コーディング規約
 *		●関数名
 *				１文字目は大文字それ以降は小文字にする
 *		●変数名
 *				・変数共通
 *						constには c_ を付ける
 *						staticには s_ を付ける
 *						ポインタには p_ を付ける
 *						全て合わさると csp_ となる
 *				・グローバル変数
 *						１文字目は大文字
 *				・関数内変数
 *						小文字と”＿”と数字を使用する 関数の引数もこれと同じ
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------

//--------------------------------

//------------------------------------------------------------------
/**
 * $brief   マッチングしてよいかどうかの判定
 * @param   friendIndex判定する友人のindex
 * @retval  マッチングokならTRUE
 */
//------------------------------------------------------------------

BOOL WIFIP2PModeCheck( int friendIndex )
{
    int mySt;
    int targetSt;
    TEST_MATCH_WORK* pMatch = CommStateGetMatchWork();

    mySt = pMatch->myMatchStatus.status;
    targetSt = pMatch->friendMatchStatus[ friendIndex ].status;
    
    if((mySt == WIFI_STATUS_DBATTLE50_WAIT)&&(targetSt == WIFI_STATUS_DBATTLE50)){
        return TRUE;
    }
    else if((mySt == WIFI_STATUS_DBATTLE100_WAIT)&&(targetSt == WIFI_STATUS_DBATTLE100)){
        return TRUE;
    }
    else if((mySt == WIFI_STATUS_DBATTLE_FREE_WAIT)&&(targetSt == WIFI_STATUS_DBATTLE_FREE)){
        return TRUE;
    }
#ifdef WFP2P_DEBUG_EXON 
    else if((mySt == WIFI_STATUS_MBATTLE_FREE_WAIT)&&(targetSt == WIFI_STATUS_MBATTLE_FREE)){
        return TRUE;
    }
#endif
    else if((mySt == WIFI_STATUS_SBATTLE50_WAIT)&&(targetSt == WIFI_STATUS_SBATTLE50)){
        return TRUE;
    }
    else if((mySt == WIFI_STATUS_SBATTLE100_WAIT)&&(targetSt == WIFI_STATUS_SBATTLE100)){
        return TRUE;
    }
    else if((mySt == WIFI_STATUS_SBATTLE_FREE_WAIT)&&(targetSt == WIFI_STATUS_SBATTLE_FREE)){
        return TRUE;
    }
    else if((mySt == WIFI_STATUS_TRADE_WAIT)&&(targetSt == WIFI_STATUS_TRADE)){
        return TRUE;
    }
    else if((mySt == WIFI_STATUS_POFIN_WAIT)&&(targetSt == WIFI_STATUS_POFIN)){
        return TRUE;
    }
    else if((mySt == WIFI_STATUS_FRONTIER_WAIT)&&(targetSt == WIFI_STATUS_FRONTIER)){
        return TRUE;
    }
#ifdef WFP2P_DEBUG_EXON 
    else if((mySt == WIFI_STATUS_BATTLEROOM_WAIT)&&(targetSt == WIFI_STATUS_BATTLEROOM)){
        return TRUE;
    }
#endif
    else if((mySt == WIFI_STATUS_BUCKET_WAIT)&&(targetSt == WIFI_STATUS_BUCKET)){
        return TRUE;
    }
    else if((mySt == WIFI_STATUS_BALANCEBALL_WAIT)&&(targetSt == WIFI_STATUS_BALANCEBALL)){
        return TRUE;
    }
    else if((mySt == WIFI_STATUS_BALLOON_WAIT)&&(targetSt == WIFI_STATUS_BALLOON)){
        return TRUE;
    }
    else if((mySt == WIFI_STATUS_LOGIN_WAIT)&&(targetSt == WIFI_STATUS_VCT)){
        return TRUE;
    }
    return FALSE;
}


