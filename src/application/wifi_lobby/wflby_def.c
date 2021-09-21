//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wflby_def.c
 *	@brief		WiFiロビー	共通通信ワークアクセス関数
 *	@author		tomoya takahashi
 *	@data		2007.09.18
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "common.h"
#include "wflby_def.h"

#include "savedata/wifihistory.h"

#include "msgdata/msg_wifi_system.h"

#include "application/wifi_country.h"

#include "communication/comm_state.h"
#include "wifi/dwc_rap.h"

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

#ifdef WFLBY_DEBUG_ROOM_WLDTIMER_AUTO
BOOL WFLBY_DEBUG_ROOM_WFLBY_TIMER_AUTO = FALSE;
#endif

#ifdef WFLBY_DEBUG_ROOM_MINIGAME_AUTO
BOOL WFLBY_DEBUG_ROOM_MINIGAME_AUTO_FLAG = FALSE;
#endif


//-----------------------------------------------------------------------------
/**
 *				時間データ
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	時間を設定
 *
 *	@param	p_time			タイム
 *	@param	cp_settime		時間
 */
//-----------------------------------------------------------------------------
void WFLBY_TIME_Set( WFLBY_TIME* p_time, const s64* cp_settime )
{
	RTCDate data;
	RTCTime time;
	RTC_ConvertSecondToDateTime( &data, &time, *cp_settime );
	p_time->hour	= time.hour;
	p_time->minute	= time.minute;
	p_time->second	= time.second;
}

//----------------------------------------------------------------------------
/**
 *	@brief	時間の足し算	
 *
 *	@param	cp_time		操作する時間
 *	@param	cp_add		足しこむ時間
 *	@param	p_ans		答え
 */
//-----------------------------------------------------------------------------
void WFLBY_TIME_Add( const WFLBY_TIME* cp_time, const WFLBY_TIME* cp_add, WFLBY_TIME* p_ans )
{
	u32 add;
	WFLBY_TIME time;
	WFLBY_TIME addtime;

	time = *cp_time;
	addtime = *cp_add;
	
	// 秒の足し算
	time.second		+= addtime.second;
	add				= time.second / 60;
	time.minute		+= add;
	p_ans->second	= time.second	% 60;

	// 分の足し算
	time.minute		+= addtime.minute;
	add				= time.minute / 60;
	time.hour		+= add;
	p_ans->minute	= time.minute	% 60;

	// 時の足し算
	time.hour	+= addtime.hour;
	p_ans->hour	= time.hour	% 24;

}

//----------------------------------------------------------------------------
/**
 *	@brief	時間の引き算	
 *
 *	@param	p_time		操作する時間
 *	@param	cp_sub		引く時間
 *	@param	p_ans		答え
 */
//-----------------------------------------------------------------------------
void WFLBY_TIME_Sub( const WFLBY_TIME* cp_time, const WFLBY_TIME* cp_sub, WFLBY_TIME* p_ans )
{
	u32 sub;
	s32 sum;
	WFLBY_TIME time;
	WFLBY_TIME subtime;

	time	= *cp_time;
	subtime	= *cp_sub;

	// 秒の引き算
	sum = ((s8)time.second - (s8)subtime.second);
	if( sum < 0  ){
		sub = ((-sum) / 60) + 1;
		time.second += 60*sub;
		time.minute -= sub;
	}
	p_ans->second = time.second - subtime.second;


	// 分の引き算
	sum = ((s8)time.minute - (s8)subtime.minute);
	if( sum < 0 ){
		sub = ((-sum) / 60) + 1;	// 最低でも１つは引くから+1
		time.minute	+= 60*sub;
		time.hour	-= sub;
	}
	p_ans->minute = time.minute - subtime.minute;

	// 時
	sum = ((s8)time.hour - (s8)subtime.hour);
	if( sum < 0 ){
		sub = ((-sum) / 24) + 1;	// 最低でも１つは引くから+1
		time.hour	+= 24*sub;
	}
	p_ans->hour = time.hour - subtime.hour;

}




//-----------------------------------------------------------------------------
/**
 *				世界地図データ
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	データ設定
 *
 *	@param	p_data		ワーク
 *	@param	nation		国ID
 *	@param	area		地域ID
 *	@param	outside		退室者かどうか
 */
//-----------------------------------------------------------------------------
void WFLBY_WLDTIMER_SetData( WFLBY_WLDTIMER* p_data, u16 nation, u8 area, BOOL outside )
{
	int i;
	BOOL set_data;
	u32 area_max;

	// 国データなしなら入れない
	if( nation == WFLBY_WLDDATA_NATION_NONE ){
		return ;
	}

	area_max = WIFI_COUNTRY_CountryCodeToPlaceIndexMax( nation );

	// 不明な国なら入れない
	if( area > area_max ){
		return ;
	}

	set_data = FALSE;
	for( i=0; i<WFLBY_WLDTIMER_DATA_MAX; i++ ){
		
		// データがないところまできたらそこに入れる
		if( p_data->data[ i ].flag == FALSE){
			set_data = TRUE;
		}else{
			// 国とエリアがかぶってたら、自分が退室者でないときだけ設定する
			if( (p_data->data[i].nation == nation) &&
				(p_data->data[i].area	== area)){
				if( outside == TRUE ){
					return ;
				}else{
					set_data = TRUE;
				}
			}
		}
		
		// 設定
		if( set_data ){
			p_data->data[i].nation	= nation;
			p_data->data[i].area	= area;
			p_data->data[i].outside	= outside;
			p_data->data[i].flag	= TRUE;
			return ;
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	国IDの取得
 */
//-----------------------------------------------------------------------------
u16 WFLBY_WLDTIMER_GetNation( const WFLBY_WLDTIMER* cp_data, u8 index )
{
	GF_ASSERT( index < WFLBY_WLDTIMER_DATA_MAX );
	GF_ASSERT( cp_data->data[index].flag == TRUE );
	
	return cp_data->data[index].nation;
}
//----------------------------------------------------------------------------
/**
 *	@brief	地域IDの取得
 */
//-----------------------------------------------------------------------------
u8 WFLBY_WLDTIMER_GetArea( const WFLBY_WLDTIMER* cp_data, u8 index )
{
	GF_ASSERT( index < WFLBY_WLDTIMER_DATA_MAX );
	GF_ASSERT( cp_data->data[index].flag == TRUE );
	
	return cp_data->data[index].area;
}
//----------------------------------------------------------------------------
/**
 *	@brief	退室フラグの取得
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_WLDTIMER_GetOutSide( const WFLBY_WLDTIMER* cp_data, u8 index )
{
	GF_ASSERT( index < WFLBY_WLDTIMER_DATA_MAX );
	GF_ASSERT( cp_data->data[index].flag == TRUE );
	
	return cp_data->data[index].outside;
}
//----------------------------------------------------------------------------
/**
 *	@brief	データ有無フラグの取得
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_WLDTIMER_GetDataFlag( const WFLBY_WLDTIMER* cp_data, u8 index )
{
	GF_ASSERT( index < WFLBY_WLDTIMER_DATA_MAX );
	
	return cp_data->data[index].flag;
}








//-----------------------------------------------------------------------------
/**
 *			ライトリソース操作
 *				ライトの影響を受けるモデルはここを通す
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	ライトリソース操作		ライトの影響を受けるモデルはここを通す
 */
//-----------------------------------------------------------------------------
void WFLBY_LIGHT_SetUpMdl( void* p_file )
{
	NNSG3dResMdlSet* p_mdlset;
	NNSG3dResMdl* p_res;

	p_mdlset = NNS_G3dGetMdlSet( p_file );
	p_res = NNS_G3dGetMdlByIdx( p_mdlset, 0 );
	
	NNS_G3dMdlUseGlbDiff( p_res );
	NNS_G3dMdlUseGlbAmb( p_res );
	NNS_G3dMdlUseGlbSpec( p_res );
	NNS_G3dMdlUseGlbEmi( p_res );
}



//-----------------------------------------------------------------------------
/**
 *			地域があるのかチェック
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	地域があるかチェック
 *
 *	@param	nation		国コード
 *	@param	area		地域コード
 *	
 *	@retval	TRUE	ある
 *	@retval	FALSE	ない
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_AREA_Check( u16 nation, u16 area )
{
	u32 area_max;

	// 地域もあっているのかチェック
	area_max = WIFI_COUNTRY_CountryCodeToPlaceIndexMax( nation );

	// 地域数0の場合0ならOK
	if( area_max == 0 ){
		if( area == 0 ){
			return TRUE;
		}
	}
	
	// 総地域数0より大きいときは1～その値までOK
	if( (area >= 1) && (area <= area_max) ){
		return TRUE;
	}

	return FALSE;
}





//-----------------------------------------------------------------------------
/**
 *			広場内技タイプ定数から戦闘用技タイプ定数を求める関数
 */
//-----------------------------------------------------------------------------
u32 WFLBY_BattleWazaType_Get( WFLBY_POKEWAZA_TYPE type )
{
	static const u8 sc_BATTLE_WAZA_TYPE_DATA[ WFLBY_POKEWAZA_TYPE_NUM ] = {
		HATE_TYPE,			//	WFLBY_POKEWAZA_TYPE_NONE,	// 選ばない
		NORMAL_TYPE,		//	WFLBY_POKEWAZA_TYPE_NORMAL,
		BATTLE_TYPE,		//	WFLBY_POKEWAZA_TYPE_BATTLE,
		HIKOU_TYPE,			//	WFLBY_POKEWAZA_TYPE_HIKOU,
		POISON_TYPE,		//	WFLBY_POKEWAZA_TYPE_POISON,
		JIMEN_TYPE,			//	WFLBY_POKEWAZA_TYPE_JIMEN,
		IWA_TYPE,			//	WFLBY_POKEWAZA_TYPE_IWA,
		MUSHI_TYPE,			//	WFLBY_POKEWAZA_TYPE_MUSHI,
		GHOST_TYPE,			//	WFLBY_POKEWAZA_TYPE_GHOST,
		METAL_TYPE,			//	WFLBY_POKEWAZA_TYPE_METAL,
		FIRE_TYPE,			//	WFLBY_POKEWAZA_TYPE_FIRE,
		WATER_TYPE,			//	WFLBY_POKEWAZA_TYPE_WATER,
		KUSA_TYPE,			//	WFLBY_POKEWAZA_TYPE_KUSA,
		ELECTRIC_TYPE,		//	WFLBY_POKEWAZA_TYPE_ELECTRIC,
		SP_TYPE,			//	WFLBY_POKEWAZA_TYPE_SP,
		KOORI_TYPE,			//	WFLBY_POKEWAZA_TYPE_KOORI,
		DRAGON_TYPE,		//	WFLBY_POKEWAZA_TYPE_DRAGON,
		AKU_TYPE,			//	WFLBY_POKEWAZA_TYPE_AKU,
	};
	return sc_BATTLE_WAZA_TYPE_DATA[ type ];
}




//-----------------------------------------------------------------------------
/**
 *			エラー処理
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	エラーナンバーからエラーメッセージを取得
 *
 *	@param	errno	エラーナンバー	CommStateGetWifiErrNo
 *	
 *	@return	メッセージナンバー
 */
//-----------------------------------------------------------------------------
int WFLBY_ERR_GetStrID( int errno,int errtype )
{
	int err_type;
	int msgno;

	err_type = mydwc_errorType( errno,errtype );
	
    if((err_type == 11) || (errno == ERRORCODE_0)){
        msgno = dwc_error_0015;
    }
    else if(errno == ERRORCODE_HEAP){
        msgno = dwc_error_0014;
    }
    else{
		if( err_type >= 0 ){
	        msgno = dwc_error_0001 + err_type;
		}else{
	        msgno = dwc_error_0012;
		}
    }

	return msgno;
}

//----------------------------------------------------------------------------
/**
 *	@brief	エラーナンバーからエラーメッセージを取得
 *
 *	@param	errno	エラーナンバー
 *
 *	@return	エラータイプ
 */
//-----------------------------------------------------------------------------
WFLBY_ERR_TYPE WFLBY_ERR_GetErrType( int errno,int errtype )
{
	int err_type;
	int ret;

	err_type = mydwc_errorType( errno, errtype );
	
    if( errno == ERRORCODE_0 ){
		err_type = 11;
    }
    else if(errno == ERRORCODE_HEAP){
		err_type = 12;
    }
    
    switch(err_type){
      case 1:
      case 4:
      case 5:
      case 11:
      case 6:
      case 7:
      case 8:
      case 9:
      case 10:
		ret = WFLBY_ERR_TYPE_RETRY;
		break; 

      case 0:
      case 2:
      case 3:
      default:
		ret = WFLBY_ERR_TYPE_END;
		break; 
    }

	return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief	Wi-Fiエラーチェック
 *
 *	@retval	TRUE	エラー発生
 *	@retval	FALSE	エラーなし
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_ERR_CheckError( void )
{
	if( CommStateIsWifiError() || CommStateWifiLobbyError() ){
		return TRUE;
	}
	return FALSE;
}
