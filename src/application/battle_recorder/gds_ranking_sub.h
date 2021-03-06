//==============================================================================
/**
 *
 * GDSランキングのサブ構造体や定数の定義など
 *
 */
//==============================================================================
#ifndef __GT_GDS_RANKING_SUB_H__
#define __GT_GDS_RANKING_SUB_H__


//==============================================================================
//	定数定義
//==============================================================================
///一週間に開催されるランキングの種類数
#define GT_RANKING_WEEK_NUM			(3)

///トレーナータイプの総数
#define GT_TRAINER_VIEW_MAX		(16)

///1年の月数
#define GT_YEAR_MONTH_NUM		(12)

///好きなポケモンランキングで発表する順位数
#define GT_MONSNO_RANKING_MAX	(20)


//==============================================================================
//	構造体定義
//==============================================================================
//--------------------------------------------------------------
/**
 *	プレイ時間
 */
//--------------------------------------------------------------
typedef struct{
	u16 hour;			//時
	u8	minute;			//分
	u8	second;			//秒
}GT_PLAYTIME;

//--------------------------------------------------------------
/**
 *	ランキング用の自分プロフィール			8byte
 */
//--------------------------------------------------------------
typedef struct{
	u8 version_code;						///< バージョンコード
	u8 language;							///< 言語コード
	u8 birthday_month;						///< プレイヤーの誕生月
	u8 trainer_view;						///< ユニオンルーム内での見た目
	u16 monsno;								///< ポケモン番号
	u8 egg_flag:1;							///< 1=タマゴ
	u8 dummy:7;							//余り
	
	u8 padding;							//パディング
}GT_RANKING_PROFILE;


#endif	//__GT_GDS_RANKING_SUB_H__
