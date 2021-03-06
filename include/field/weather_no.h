//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *@file		weatheru_no.h
 *@brief	¨VC	io[
 *@author	tomoya takahashi
 *@data		2006.06.08
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]



#ifndef __WEATHER_NO_H__
#define	__WEATHER_NO_H__

//-------------------------------------
//	VCf[^ñ	(no)
//
//	éæ@Vóio[
//=====================================

#define		WEATHER_SYS_SUNNY			(0)				// °ê
#define		WEATHER_SYS_CLOUDINESS		(1)				// Üè		BG	
#define		WEATHER_SYS_RAIN			(2)				// J		OAM	FOG
#define		WEATHER_SYS_STRAIN			(3)				// åJ		OAM FOG 
#define		WEATHER_SYS_SPARK			(4)				// åJ		OAM FOG BG
#define		WEATHER_SYS_SNOW			(5)				// á		OAM FOG
#define		WEATHER_SYS_SNOWSTORM		(6)				// á		OAM FOG
#define		WEATHER_SYS_SNOWSTORM_H		(7)				// Òá	OAM FOG BG
#define		WEATHER_SYS_FOG				(8)				// ¶å		FOG	
#define		WEATHER_SYS_VOLCANO			(9)				// ÎRD	OAM FOG BG
#define		WEATHER_SYS_SANDSTORM		(10)			// BGgp»	OAM FOG BG
#define		WEATHER_SYS_DIAMONDDUST		(11)			// Xm[_Xg	OAM FOG
#define		WEATHER_SYS_SPIRIT			(12)			// C		OAM
#define		WEATHER_SYS_MYSTIC			(13)			// _é		OAM FOG
#define		WEATHER_SYS_MIST1			(14)			// ¶¥¢Ì¶	FOG	BG
#define		WEATHER_SYS_MIST2			(15)			// ¶¥¢Ì¶	FOG	BG
#define		WEATHER_SYS_FLASH			(16)			// tbV		BG

#define		WEATHER_SYS_SPARK_EFF		(17)			// 		BG
#define		WEATHER_SYS_FOGS			(18)			// ¶		FOG
#define		WEATHER_SYS_FOGM			(19)			// ¶		FOG

#define		WEATHER_SYS_RAINBOW			(20)			// ø		BG
#define		WEATHER_SYS_SNOW_STORM_BG	(21)			// BGgpá	OAM FOG BG
#define		WEATHER_SYS_STORM			(22)			// »		OAM FOG

#define		WEATHER_SYS_KOGOREBI		(23)			// Ãêü	BG
#define		WEATHER_SYS_DOUKUTU00		(24)			// ´A	FOG
#define		WEATHER_SYS_DOUKUTU01		(25)			// ´A	FOG
#define		WEATHER_SYS_GINGA00			(26)			// âÍcª¢ÄÃ¢
#define		WEATHER_SYS_LOSTTOWER00		(27)			// LostTowerÃ¢
#define		WEATHER_SYS_HARDMOUNTAIN	(28)			// n[h}Ee
#define		WEATHER_SYS_DOUKUTU02		(29)			// ´A@FOG@Â
#define		WEATHER_SYS_USUGURAI		(30)			// Ã¢@FOG

#define		WEATHER_SYS_NUM				(31)			// VC

	// í¬tB[hóÔµÄ¢æ¤
	// ogt@Ng[Ì[bg{ÝÅ
	// ÀÛÌtB[h¨«éVCÈOÌóÔð
	// ogÉn³È­ÄÍ¢¯È­ÈÁ½½ßì¬B
	//
	// PLÆGSÌÊMÅàgp³êéèÈÌÅA
	// lÍêèÉµÄ­¾³¢B
#define		WEATHER_SYS_BTONLYSTART (1000)	// ogI[ÌóÔJnè
#define		WEATHER_SYS_HIGHSUNNY	(1001)	// ú{°ê
#define		WEATHER_SYS_TRICKROOM	(1002)	// gbN[



#if  0
enum{
	WEATHER_SYS_SUNNY,			// °ê
	WEATHER_SYS_CLOUDINESS,		// Üè		BG	
	WEATHER_SYS_RAIN,			// J		OAM	FOG
	WEATHER_SYS_STRAIN,			// åJ		OAM FOG 
	WEATHER_SYS_SPARK,			// åJ		OAM FOG BG
	WEATHER_SYS_SNOW,			// á		OAM FOG
	WEATHER_SYS_SNOWSTORM,		// á		OAM FOG
	WEATHER_SYS_SNOWSTORM_H,	// Òá	OAM FOG BG
	WEATHER_SYS_FOG,			// ¶å		FOG	
	WEATHER_SYS_VOLCANO,		// ÎRD	OAM FOG BG
	WEATHER_SYS_SANDSTORM,		// BGgp»	OAM FOG BG
	WEATHER_SYS_DIAMONDDUST,	// Xm[_Xg	OAM FOG
	WEATHER_SYS_SPIRIT,			// C		OAM
	WEATHER_SYS_MYSTIC,			// _é		OAM FOG
	WEATHER_SYS_MIST1,			// ¶¥¢Ì¶	FOG	BG
	WEATHER_SYS_MIST2,			// ¶¥¢Ì¶	FOG	BG
	WEATHER_SYS_FLASH,			// tbV		BG

	WEATHER_SYS_SPARK_EFF,		// 		BG
	WEATHER_SYS_FOGS,			// ¶		FOG
	WEATHER_SYS_FOGM,		// ¶		FOG

	WEATHER_SYS_RAINBOW,	// ø		BG
	WEATHER_SYS_SNOW_STORM_BG,// BGgpá	OAM FOG BG
	WEATHER_SYS_STORM,		// »		OAM FOG
	WEATHER_SYS_KOGOREBI,		// Ãêü	BG
	WEATHER_SYS_DOUKUTU00,		// ´A	FOG
	WEATHER_SYS_DOUKUTU01,		// ´A	FOG
	WEATHER_SYS_GINGA00,		// âÍcª¢ÄÃ¢
	WEATHER_SYS_LOSTTOWER00,	// LostTowerÃ¢
	WEATHER_SYS_HARDMOUNTAIN,	// n[h}Ee
	WEATHER_SYS_DOUKUTU02,		// ´A@FOG@Â
	WEATHER_SYS_USUGURAI,		// Ã¢@FOG

	WEATHER_SYS_NUM,// VC

	// í¬tB[hóÔµÄ¢æ¤
	// ogt@Ng[Ì[bg{ÝÅ
	// ÀÛÌtB[h¨«éVCÈOÌóÔð
	// ogÉn³È­ÄÍ¢¯È­ÈÁ½½ßì¬B
	//
	// PLÆGSÌÊMÅàgp³êéèÈÌÅA
	// lÍêèÉµÄ­¾³¢B
	WEATHER_SYS_BTONLYSTART = 1000,	// ogI[ÌóÔJnè
	WEATHER_SYS_HIGHSUNNY,	// ú{°ê
	WEATHER_SYS_TRICKROOM,	// gbN[
};
#endif


#endif // __WEATHER_NO_H__
