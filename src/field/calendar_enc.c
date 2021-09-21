//============================================================================================
/**
 * @file	calendar_enc.c
 * @brief	カレンダーがらみのエンカウント
 * @author	Nozomu Saito
 * @date	2006.05.16
 */
//============================================================================================
#include "common.h"
#include "system/pm_rtc.h"

#include "calendar_enc.h"

#define DAY_DATA(day,prob_no)		( (day<<8) | (prob_no&0xff) )
#define GET_DAY(data)				( (data>>8)&0xff )
#define GET_PROB_NO(data)			( data&0xff )

enum {
	JAN_DAT_MAX = 4,
	FEB_DAT_MAX = 4,
	MAR_DAT_MAX = 3,
	APR_DAT_MAX = 4,
	MAY_DAT_MAX = 4,
	JUN_DAT_MAX = 1,
	JUL_DAT_MAX = 3,
	AUG_DAT_MAX = 4,
	SEP_DAT_MAX = 6,
	OCT_DAT_MAX = 3,
	NOV_DAT_MAX = 4,
	DEC_DAT_MAX = 3,
};

#define MONTH_NUM	(12)


typedef u16 CAL_ENC_DAY_DATA;

//1月
static const CAL_ENC_DAY_DATA JanData[JAN_DAT_MAX] = {
	DAY_DATA( 1,3),			//元旦		New Year's Day（北米・イギ・イタ・ス・フ・ド）
	DAY_DATA(11,2),			//鏡開き
	DAY_DATA(12,4),			//B
	DAY_DATA(29,2)			//FRLG発売
};

//2月
static const CAL_ENC_DAY_DATA FebData[FEB_DAT_MAX] = {
	DAY_DATA( 3,2),			//節分
	DAY_DATA(11,2),			//建国記念日
	DAY_DATA(14,0),			//バレンタインデー(北米)
	DAY_DATA(27,4)			//赤緑発売
};

//3月
static const CAL_ENC_DAY_DATA MarData[MAR_DAT_MAX] = {
	DAY_DATA( 3,2),			//ひな祭り
	DAY_DATA(18,2),			//ポケパーク開園
	DAY_DATA(21,3),			//春分の日
};

//4月
static const CAL_ENC_DAY_DATA AprData[APR_DAT_MAX] = {
	DAY_DATA( 1,2),			//アニメ開始
	DAY_DATA(25,2),			//ポケセンオープン
	DAY_DATA(26,1),			//GF設立
	DAY_DATA(29,2),
};

//5月
static const CAL_ENC_DAY_DATA MayData[MAY_DAT_MAX] = {
	DAY_DATA( 1,0),			//ＭＹ		メーデー(五月祭)（北米・イギ・イタ・ス・フ・ド）
	DAY_DATA( 3,2),			//憲法記念日
	DAY_DATA( 4,0),			//国民の休日
	DAY_DATA( 5,2),			//こどもの日
};

//6月
static const CAL_ENC_DAY_DATA JunData[JUN_DAT_MAX] = {
	DAY_DATA(21,2),			//DUMMY
};

//7月
static const CAL_ENC_DAY_DATA JulData[JUL_DAT_MAX] = {
	DAY_DATA( 7,4),			//七夕
	DAY_DATA(18,2),			//劇場版第一弾
	DAY_DATA(24,2)			//DUMMY
};

//8月
static const CAL_ENC_DAY_DATA AugData[AUG_DAT_MAX] = {
	DAY_DATA(13,1),			//お盆
	DAY_DATA(14,1),			//お盆
	DAY_DATA(15,2),			//お盆
	DAY_DATA(16,1),			//お盆
};

//9月
static const CAL_ENC_DAY_DATA SepData[SEP_DAT_MAX] = {
	DAY_DATA(7,2),			//十五夜
	DAY_DATA(12,2),			//十五夜
	DAY_DATA(15,2),			//十五夜
	DAY_DATA(20,2),			//
	DAY_DATA(23,3),			//秋分の日
	DAY_DATA(28,2)
};

//10月
static const CAL_ENC_DAY_DATA OctData[OCT_DAT_MAX] = {
	DAY_DATA( 5,2),			//赤青発売(欧州)
	DAY_DATA(15,2),			//青発売
	DAY_DATA(30,2),			//
};

//11月
static const CAL_ENC_DAY_DATA NovData[NOV_DAT_MAX] = {
	DAY_DATA( 3,1),			//文化の日
	DAY_DATA(12,2),			//劇場版第一弾公開(北米)
	DAY_DATA(21,2),			//金銀RS発売
	DAY_DATA(23,0)			//勤労感謝の日
};

//12月
static const CAL_ENC_DAY_DATA DecData[DEC_DAT_MAX] = {
	DAY_DATA(14,2),			//クリスタル発売
	DAY_DATA(23,2),			//天皇誕生日
	DAY_DATA(31,1),			//大晦日
};

typedef struct CAL_ENC_MONTH_DATA_tag{
	int Num;
	const CAL_ENC_DAY_DATA *DayData;
}CAL_ENC_MONTH_DATA;


static const CAL_ENC_MONTH_DATA CalEncMonthData[MONTH_NUM] = {
	{JAN_DAT_MAX,JanData},{FEB_DAT_MAX,FebData},{MAR_DAT_MAX,MarData},
	{APR_DAT_MAX,AprData},{MAY_DAT_MAX,MayData},{JUN_DAT_MAX,JunData},
	{JUL_DAT_MAX,JulData},{AUG_DAT_MAX,AugData},{SEP_DAT_MAX,SepData},
	{OCT_DAT_MAX,OctData},{NOV_DAT_MAX,NovData},{DEC_DAT_MAX,DecData}
};

static const int ProbTbl[] = 
{
	0,-5,5,-10,10
};
/**
static void DebugPrint(void);
*/
//-----------------------------------------------------------------------------
/**
 * 日にちによる確率計算
 *
 * @param	inProb			現在エンカウント確率
 * @param	inPenalty		ペナルティー
 *
 * @return	int				エンカウント確率
 */
//-----------------------------------------------------------------------------
int CalEnc_GetProb(const int inProb, const BOOL inPenalty)
{
	u8 i;
	int prob;
	RTCDate date;
	const CAL_ENC_MONTH_DATA * data;

	//時間不正変更された場合は、処理しない
	if (inPenalty){
		return inProb;
	}
	
	if (inProb == 0){
		return 0;
	}

//	DebugPrint();
	
	GF_RTC_GetDate(&date);

	GF_ASSERT(date.month>0);
	GF_ASSERT(date.month<=12);

	data = &(CalEncMonthData[date.month-1]);

	for(i=0;i<data->Num;i++){
		if ( date.day == GET_DAY(data->DayData[i]) ){
			prob = inProb + ProbTbl[ GET_PROB_NO(data->DayData[i]) ];
			if (prob < 0){
				prob = 1;
			}
			return prob;
		}
	}
	return inProb;
}

/**
static void DebugPrint(void)
{
	int i,j;
	const CAL_ENC_MONTH_DATA * data;
	for(j=0;j<MONTH_NUM;j++){
		data = &(CalEncMonthData[j]);
		OS_Printf("Month:%d DayNum:%d\n",j+1,data->Num);
		for(i=0;i<data->Num;i++){
			OS_Printf(" Day:%d Prob:%d\n",GET_DAY(data->DayData[i]),GET_PROB_NO(data->DayData[i]));
		}
	}
}
*/
