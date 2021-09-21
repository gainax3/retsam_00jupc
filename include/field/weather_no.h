//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *@file		weatheru_no.h
 *@brief	お天気	ナンバー
 *@author	tomoya takahashi
 *@data		2006.06.08
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]



#ifndef __WEATHER_NO_H__
#define	__WEATHER_NO_H__

//-------------------------------------
//	天気データ列挙	(no)
//
//	企画　天候ナンバー順
//=====================================

#define		WEATHER_SYS_SUNNY			(0)				// 晴れ
#define		WEATHER_SYS_CLOUDINESS		(1)				// 曇り		BG	
#define		WEATHER_SYS_RAIN			(2)				// 雨		OAM	FOG
#define		WEATHER_SYS_STRAIN			(3)				// 大雨		OAM FOG 
#define		WEATHER_SYS_SPARK			(4)				// 大雨		OAM FOG BG
#define		WEATHER_SYS_SNOW			(5)				// 雪		OAM FOG
#define		WEATHER_SYS_SNOWSTORM		(6)				// 吹雪		OAM FOG
#define		WEATHER_SYS_SNOWSTORM_H		(7)				// 猛吹雪	OAM FOG BG
#define		WEATHER_SYS_FOG				(8)				// 霧大		FOG	
#define		WEATHER_SYS_VOLCANO			(9)				// 火山灰	OAM FOG BG
#define		WEATHER_SYS_SANDSTORM		(10)			// BG使用砂嵐	OAM FOG BG
#define		WEATHER_SYS_DIAMONDDUST		(11)			// スノーダスト	OAM FOG
#define		WEATHER_SYS_SPIRIT			(12)			// 気合		OAM
#define		WEATHER_SYS_MYSTIC			(13)			// 神秘		OAM FOG
#define		WEATHER_SYS_MIST1			(14)			// 霧払いの霧	FOG	BG
#define		WEATHER_SYS_MIST2			(15)			// 霧払いの霧	FOG	BG
#define		WEATHER_SYS_FLASH			(16)			// フラッシュ		BG

#define		WEATHER_SYS_SPARK_EFF		(17)			// 雷		BG
#define		WEATHER_SYS_FOGS			(18)			// 霧		FOG
#define		WEATHER_SYS_FOGM			(19)			// 霧中		FOG

#define		WEATHER_SYS_RAINBOW			(20)			// 虹		BG
#define		WEATHER_SYS_SNOW_STORM_BG	(21)			// BG使用吹雪	OAM FOG BG
#define		WEATHER_SYS_STORM			(22)			// 砂嵐		OAM FOG

#define		WEATHER_SYS_KOGOREBI		(23)			// 凝れ美	BG
#define		WEATHER_SYS_DOUKUTU00		(24)			// 洞窟	FOG
#define		WEATHER_SYS_DOUKUTU01		(25)			// 洞窟	FOG
#define		WEATHER_SYS_GINGA00			(26)			// 銀河団がいて薄暗い
#define		WEATHER_SYS_LOSTTOWER00		(27)			// LostTower薄暗い
#define		WEATHER_SYS_HARDMOUNTAIN	(28)			// ハードマウンテン
#define		WEATHER_SYS_DOUKUTU02		(29)			// 洞窟　FOG　青
#define		WEATHER_SYS_USUGURAI		(30)			// 薄暗い　FOG

#define		WEATHER_SYS_NUM				(31)			// 天気数

	// 戦闘フィールド状態していよう
	// バトルファクトリーのルーレット施設で
	// 実際のフィールドおきる天気以外の状態を
	// バトルに渡さなくてはいけなくなったため作成。
	//
	// PLとGSの通信でも使用される定数なので、
	// 値は一定にしてください。
#define		WEATHER_SYS_BTONLYSTART (1000)	// バトルオンリーの状態開始定数
#define		WEATHER_SYS_HIGHSUNNY	(1001)	// 日本晴れ
#define		WEATHER_SYS_TRICKROOM	(1002)	// トリックルーム



#if  0
enum{
	WEATHER_SYS_SUNNY,			// 晴れ
	WEATHER_SYS_CLOUDINESS,		// 曇り		BG	
	WEATHER_SYS_RAIN,			// 雨		OAM	FOG
	WEATHER_SYS_STRAIN,			// 大雨		OAM FOG 
	WEATHER_SYS_SPARK,			// 大雨		OAM FOG BG
	WEATHER_SYS_SNOW,			// 雪		OAM FOG
	WEATHER_SYS_SNOWSTORM,		// 吹雪		OAM FOG
	WEATHER_SYS_SNOWSTORM_H,	// 猛吹雪	OAM FOG BG
	WEATHER_SYS_FOG,			// 霧大		FOG	
	WEATHER_SYS_VOLCANO,		// 火山灰	OAM FOG BG
	WEATHER_SYS_SANDSTORM,		// BG使用砂嵐	OAM FOG BG
	WEATHER_SYS_DIAMONDDUST,	// スノーダスト	OAM FOG
	WEATHER_SYS_SPIRIT,			// 気合		OAM
	WEATHER_SYS_MYSTIC,			// 神秘		OAM FOG
	WEATHER_SYS_MIST1,			// 霧払いの霧	FOG	BG
	WEATHER_SYS_MIST2,			// 霧払いの霧	FOG	BG
	WEATHER_SYS_FLASH,			// フラッシュ		BG

	WEATHER_SYS_SPARK_EFF,		// 雷		BG
	WEATHER_SYS_FOGS,			// 霧		FOG
	WEATHER_SYS_FOGM,		// 霧中		FOG

	WEATHER_SYS_RAINBOW,	// 虹		BG
	WEATHER_SYS_SNOW_STORM_BG,// BG使用吹雪	OAM FOG BG
	WEATHER_SYS_STORM,		// 砂嵐		OAM FOG
	WEATHER_SYS_KOGOREBI,		// 凝れ美	BG
	WEATHER_SYS_DOUKUTU00,		// 洞窟	FOG
	WEATHER_SYS_DOUKUTU01,		// 洞窟	FOG
	WEATHER_SYS_GINGA00,		// 銀河団がいて薄暗い
	WEATHER_SYS_LOSTTOWER00,	// LostTower薄暗い
	WEATHER_SYS_HARDMOUNTAIN,	// ハードマウンテン
	WEATHER_SYS_DOUKUTU02,		// 洞窟　FOG　青
	WEATHER_SYS_USUGURAI,		// 薄暗い　FOG

	WEATHER_SYS_NUM,// 天気数

	// 戦闘フィールド状態していよう
	// バトルファクトリーのルーレット施設で
	// 実際のフィールドおきる天気以外の状態を
	// バトルに渡さなくてはいけなくなったため作成。
	//
	// PLとGSの通信でも使用される定数なので、
	// 値は一定にしてください。
	WEATHER_SYS_BTONLYSTART = 1000,	// バトルオンリーの状態開始定数
	WEATHER_SYS_HIGHSUNNY,	// 日本晴れ
	WEATHER_SYS_TRICKROOM,	// トリックルーム
};
#endif


#endif // __WEATHER_NO_H__
