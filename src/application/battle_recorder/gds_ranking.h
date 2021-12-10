//==============================================================================
/**
 *
 * GDSランキング
 *
 */
//==============================================================================
#ifndef __GT_GDS_RANKING_H__
#define __GT_GDS_RANKING_H__

#include "gds_define.h"
#include "gds_profile_local.h"
#include "gds_ranking_sub.h"


//--------------------------------------------------------------
/**
 *	自分のランキング登録用データ	8byte
 */
//--------------------------------------------------------------
typedef struct{
	u8 ranking_type;		///<ランキング指定ナンバー
	u8 padding[3];			//パディング
	u32 score;				///<成績
}GT_RANKING_MYDATA;

//--------------------------------------------------------------
/**
 *	先週のランキングデータ		456byte
 */
//--------------------------------------------------------------
typedef struct{
	u8 ranking_type;						///<ランキング指定ナンバー
	u8 padding[3];
	
	///トレーナータイプのランキング順位(1位から順位順にトレーナータイプが入っています)
	u8 trainer_type_ranking[GT_TRAINER_VIEW_MAX];
	///トレーナータイプランキングのスコア(trainer_type_rankingと同じ順番で得点が入っています)
	u64 trainer_type_score[GT_TRAINER_VIEW_MAX];
	
	///誕生月のランキング順位(1位から順位順に誕生月が入っています)
	u8 birthday_month_ranking[GT_YEAR_MONTH_NUM];
	///誕生月ランキングのスコア(birthday_month_rankingと同じ順番で得点が入っています)
	u64 birthday_month_score[GT_YEAR_MONTH_NUM];
	
	///好きなポケモンのランキング順位(1位から順位順にポケモン番号が入っています)
	u16 monsno_ranking[GT_MONSNO_RANKING_MAX];
	///好きなポケモンランキングのスコア(monsno_rankingと同じ順番で得点が入っています)
	u64 monsno_score[GT_MONSNO_RANKING_MAX];
}GT_LAST_WEEK_RANKING_DATA;

//--------------------------------------------------------------
/**
 *	今週のランキングデータ		72byte
 */
//--------------------------------------------------------------
typedef struct{
	u8 ranking_type;						///<ランキング指定ナンバー
	u8 padding[3];
	
	///トレーナータイプのランキング順位(1位から順位順にトレーナータイプが入っています)
	u8 trainer_type_ranking[GT_TRAINER_VIEW_MAX];
	
	///誕生月のランキング順位(1位から順位順に誕生月が入っています)
	u8 birthday_month_ranking[GT_YEAR_MONTH_NUM];
	
	///好きなポケモンのランキング順位(1位から順位順にポケモン番号が入っています)
	u16 monsno_ranking[GT_MONSNO_RANKING_MAX];
}GT_THIS_WEEK_RANKING_DATA;



//==============================================================================
//	
//==============================================================================
//--------------------------------------------------------------
/**
 *	受信データ：現在行われているランキング内容		4byte
 */
//--------------------------------------------------------------
typedef struct{
	u8 ranking_type[GT_RANKING_WEEK_NUM];		///<現在開催されているランキングの種類
	u8 padding;		//パディング
}GT_RANKING_TYPE_RECV;

//--------------------------------------------------------------
/**
 *	受信データ：先週のランキングデータ		1368byte
 */
//--------------------------------------------------------------
typedef struct{
	GT_LAST_WEEK_RANKING_DATA ranking_data[GT_RANKING_WEEK_NUM];
}GT_LAST_WEEK_RANKING_ALL_RECV;

//--------------------------------------------------------------
/**
 *	受信データ：今週のランキングデータ		216byte
 */
//--------------------------------------------------------------
typedef struct{
	GT_THIS_WEEK_RANKING_DATA ranking_data[GT_RANKING_WEEK_NUM];
}GT_THIS_WEEK_RANKING_DATA_ALL_RECV;


//==============================================================================
//	
//==============================================================================
//--------------------------------------------------------------
/**
 *	送信データ：自分のランキングデータ		40byte
 */
//--------------------------------------------------------------
typedef struct{
	GT_RANKING_PROFILE profile;				///<ランキング用プロフィール
	GT_PLAYTIME playtime;					///<プレイ時間
	GT_RANKING_MYDATA my_data[GT_RANKING_WEEK_NUM];	///<自分のランキング登録用データ
}GT_RANKING_MYDATA_SEND;


#endif	//__GT_GDS_RANKING_H__
