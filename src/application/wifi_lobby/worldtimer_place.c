//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		worldtimer_place.c
 *	@brief		世界時計	地域時間取得処理	メモリをあけるためにこの処理だけwflby_commonオーバーレイに移動
 *	@author		tomoya takahashi
 *	@data		2008.05.15
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "common.h"

#include "system/arc_util.h"

#include "application/wifi_country.h"
#include "application/wifi_earth/wifi_earth_place.naix"
#include "application/wifi_lobby/worldtimer_place.h"

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

static void WLDTIMER_EarthListGetNationData( ARCHANDLE* p_handle, u32 heapID, u16 nation, u8 area, fx32* p_y );


//----------------------------------------------------------------------------
/**
 *	@brief	地域の時間を取得する
 *
 *	@param	nation		国ID
 *	@param	area		地域ID
 *	@param	gmt			GMT
 *	@param	heapID		ヒープID
 *
 *	@return	国の時間
 */
//-----------------------------------------------------------------------------
WFLBY_TIME WFLBY_WORLDTIMER_PLACE_GetPlaceTime( u16 nation, u8 area, WFLBY_TIME gmt, u32 heapID )
{
	// 地点リスト
	fx32 gmt_y;
	fx32 place_y;
	u16 rota;
	s32 hour_dif;
	WFLBY_TIME time;
	ARCHANDLE* p_handle;

	p_handle = ArchiveDataHandleOpen( ARC_WIFI_EARCH_PLACE, heapID );
	

	// GMT地域の回転角度取得
	WLDTIMER_EarthListGetNationData( p_handle, heapID, WLDTIMER_GMT_NATIONID,  WLDTIMER_GMT_AREAID, &gmt_y );
	
	// 地域の回転角度取得
	WLDTIMER_EarthListGetNationData( p_handle, heapID, nation, area, &place_y );

	// 差分から時間の誤差を求める誤差は時間単位で行う
	rota = gmt_y - place_y;
	hour_dif = (rota*24)/0xffff;


	// マイナスの値だったり、オーバーしたりしているはずなので調整
	if( hour_dif < 0 ){
		hour_dif += 24;
	}
	if( hour_dif >= 24 ){
		hour_dif %= 24;
	}

	// 時間を求める
	memset( &time, 0, sizeof(WFLBY_TIME) );
	time.hour = hour_dif;
	WFLBY_TIME_Add( &time, &gmt, &time );

	ArchiveDataHandleClose( p_handle );

	return time;
}




//-----------------------------------------------------------------------------
/**
 *				プライベート関数
 */
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief	ある国のY回転角度だけ受け取る
 *
 *	@param	p_handle		アーカイブハンドル
 *	@param	heapID			ヒープID
 *	@param	nation			国ID
 *	@param	area			地域ID
 *	@param	p_y				ｙ回転格納先
 */
//-----------------------------------------------------------------------------
static void WLDTIMER_EarthListGetNationData( ARCHANDLE* p_handle, u32 heapID, u16 nation, u8 area, fx32* p_y )
{
	BOOL set_ok = FALSE;
	
	{//地点マーク回転初期化（国データバイナリデータロード）
		void* filep;
		EARTH_DATA_NATION* listp;
		u32	size;
		int	listcount;

		filep = ArcUtil_HDL_LoadEx( p_handle, NARC_wifi_earth_place_place_pos_wrd_dat, 
								FALSE, heapID, ALLOC_TOP, &size );

		listp = (EARTH_DATA_NATION*)filep;	//ファイル読み込み用に変換
		listcount = size/6;				//地点数取得（データ長：１地点につき６バイト）

		GF_ASSERT( nation < listcount );

		// 地域があるのかチェック
		if( listp[ nation ].flag != 2 ){
			*p_y = listp[ nation ].y;
			set_ok = TRUE;
		}
		
		sys_FreeMemoryEz(filep);
	}
	
	if( set_ok == FALSE ){//地点マーク回転初期化（地域データバイナリデータロード）
		void* filep;
		EARTH_DATA_AREA* listp;
		u32	size, data_id;
		int	index,listcount;

		index = WIFI_COUNTRY_CountryCodeToDataIndex( nation );	//1orgin

		data_id = WIFI_COUNTRY_DataIndexToPlaceDataID( index );
		filep = ArcUtil_HDL_LoadEx( p_handle, data_id, FALSE, 
								heapID, ALLOC_TOP, &size );

		listp = (EARTH_DATA_AREA*)filep;	//ファイル読み込み用に変換
		listcount = size/4;		//地点数取得（データ長：１地点につき４バイト）

		if( area < listcount ){
			*p_y = listp[ area ].y;
		}else{
			GF_ASSERT(0);
			*p_y = listp[ 0 ].y;
		}

		sys_FreeMemoryEz(filep);
	}
}

