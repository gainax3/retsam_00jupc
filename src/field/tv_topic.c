//============================================================================================
/**
 * @file	tv_topic.c
 * @brief	テレビトピック生成・放送用
 * @author	tamada GAME FREAK inc.
 * @date	2006.05.19
 */
//============================================================================================

#include "common.h"
#include "system/gamedata.h"

#include "fieldsys.h"
#include "fieldmap_work.h"

#include "savedata/tv_work.h"
#include "savedata/encount.h"

#include "tv_define.h"

#include "system/pm_rtc.h"
#include "ev_time.h"

#include "tv.h"

#include "tv_topic.h"
#include "field/tvtopic_battle.h"
#include "field/tvtopic_extern.h"

#include "msgdata/msg.naix"

#include "system/pm_rtc.h"
#include "gflib/strbuf_family.h"
#include "system/wordset.h"

#include "savedata/randomgroup.h"
#include "savedata/imageclip_data.h"
#include "savedata/seedbed.h"
#include "savedata/zukanwork.h"
#include "savedata/encount.h"
#include "generate_zone.h"
#include "field/eventflag.h"
#include "zonedata.h"
#include "itemtool/nuts.h"
#include "field/poketch.h"

#include "sysflag.h"
#include "ev_pokemon.h"

#include "mapdefine.h"
#include "weather_sys.h"
#include "mapdata_weather.h"

#include "system/arc_tool.h"
#include "../arc/tv.naix"

#include "msgdata/msg.naix"
#include "msgdata/msg_tv_program.h"
#include "msgdata/msg_tvcm.h"
#include "msgdata/msg_msg_tv_01.h"
#include "msgdata/msg_msg_tv_02.h"
#include "msgdata/msg_msg_tv_03.h"
#include "msgdata/msg_msg_tv_04.h"
#include "msgdata/msg_msg_tv_05.h"
#include "msgdata/msg_status.h"

#include "../frontier/frontier_def.h"
#include "application/p_status/ribbon.h"

// ----------------------------------------------------------------------------
// localize_spec_mark(LANG_ENGLISH) imatake 2006/12/28
// 英語版のみポケモンの大きさをインチ系に変換
#include "localize.h"
// ----------------------------------------------------------------------------

//============================================================================================
//============================================================================================
static void MakeTopic(FIELDSYS_WORK * fsys, int prog_type, int topic_id, const void * data);
static void MakeTopic2(SAVEDATA * savedata, int prog_type, int topic_id, const void * data);

//--------------------------------------------------------------------
///	ダミー定義
//--------------------------------------------------------------------
typedef struct {
	u8 dummy[TOPIC_DATA_SIZE];
}TOPIC_DUMMY_DATA;


//============================================================================================
//	監視型(★は新規、■は削除)
//============================================================================================

//--------------------------------------------------------------------
/**
 * @brief	バトルでのテレビトピック用情報ワーク定義
 *
 *
 * 監視型１、２：捕獲
 */
//--------------------------------------------------------------------
struct TVWATCH_BATTLE_WORK{
	int win_lose_flag;
	u16 ball_count;
	u16 monsno;
	u8 mons_sex;
	u8 mons_region;
	u8 mons_rom;
	u8 nickname_flag;
	STRCODE nickname[MONS_NAME_SIZE + EOM_SIZE];
	u16 ball_item_no;
};

//--------------------------------------------------------------------
///	監視型３：つり
//--------------------------------------------------------------------
typedef struct {
	u16 monsno;
	u8 mons_sex;
	u8 mons_region;
	u8 mons_rom;
	u16 itemno;
	BOOL success_flag;
}WATCH_FISHING_WORK;

//--------------------------------------------------------------------
///	監視型４グループ乗り換え，２０グループ作成用定義
//--------------------------------------------------------------------
typedef struct {
	STRCODE groupname[MONS_NAME_SIZE + EOM_SIZE];
}WATCH_GROUP_WORK;

//--------------------------------------------------------------------
///	監視型６：隠しアイテム発見用定義
//--------------------------------------------------------------------
typedef struct {
	u16 item_no;
	u16 place_name_id;
}WATCH_HIDEITEM_WORK;

//--------------------------------------------------------------------
///	監視型７：買い物チェック用定義★
//--------------------------------------------------------------------
typedef struct {
	u16 item_no;				//アイテムナンバー
	u8	item_num;				//個数
}WATCH_SHOPPING_WORK;

//--------------------------------------------------------------------
///	監視型８：タマゴ孵化用定義
//--------------------------------------------------------------------
typedef struct {
	u16 monsno;
	u8 mons_sex;
	u8 mons_region;
	u8 mons_rom;
	u16 place_name_id;
}WATCH_HATCHEGG_WORK;
//--------------------------------------------------------------------
///	監視１０：姓名判断
//--------------------------------------------------------------------
typedef struct {
	u16 monsno;
	u8 mons_sex;
	u8 mons_region;
	u8 mons_rom;
	STRCODE newname[MONS_NAME_SIZE + EOM_SIZE];
}WATCH_MONSNAMECHG_WORK;

//--------------------------------------------------------------------
///	監視１３：地下アイテム
//--------------------------------------------------------------------
typedef struct {
	u16 fossil_type;
	u16 fossil_num;
}WATCH_DIG_FOSSIL_WORK;

//--------------------------------------------------------------------
///	監視１５：サファリ
//--------------------------------------------------------------------
typedef struct {
	u16 monsno;
	u8 mons_sex;
	u8 mons_region;
	u8 mons_rom;
	u8 get_count;
}WATCH_SAFARI_WORK;

//--------------------------------------------------------------------
///	監視型１６：オスだけ！メスだけ！用定義★
//--------------------------------------------------------------------
typedef struct {
	u16	monsno;					//先頭のポケモンナンバー
	u8	mons_sex;				//オスかメスか
	u8	mons_region;
	u8	mons_rom;
}WATCH_OSUMESU_WORK;

//--------------------------------------------------------------------
///	監視１７：漢方薬■
//--------------------------------------------------------------------
typedef struct {
	u16 monsno;
	u8 mons_sex;
	u8 mons_region;
	u8 mons_rom;
	u16 itemno;
}WATCH_KANPO_WORK;

//--------------------------------------------------------------------
///	監視１８，１９、ギネス０６：きのみ豊作、不作、名人
//--------------------------------------------------------------------
typedef struct {
	u16 nuts_item_no;
	u8	hp;
	u16 nuts_count;
}COMMON_TAKE_NUTS_WORK;

//--------------------------------------------------------------------
///	監視２１：カスタムカプセル
//--------------------------------------------------------------------
typedef struct {
	u16 monsno;
	u8 mons_sex;
	u8 mons_region;
	u8 mons_rom;
	u8 rand;
	u8 seal_no;
}WATCH_CUSTOMCAPSULE_WORK;

//--------------------------------------------------------------------
///	監視２２、２３：旗を取った・旗を取られた
//--------------------------------------------------------------------
typedef struct {
	int size;
	u8 data[TOPIC_DATA_SIZE - sizeof(int)];
}WATCH_UG_FLAG_WORK;

//--------------------------------------------------------------------
///	監視２５：別荘購入★
//--------------------------------------------------------------------
typedef struct {
	u8 buy;						//別荘を購入したか
	//u8 broadcast_flag;		//放送したか
}WATCH_VILLA_WORK;

//--------------------------------------------------------------------
///	監視２６：別荘改築★
//--------------------------------------------------------------------
typedef struct {
	u8 furniture;				//購入した家具
}WATCH_RECONSTRUCTION_WORK;


//============================================================================================
//	ギネス型(★は新規、■は削除)
//============================================================================================

//--------------------------------------------------------------------
///	ギネス型０１：バトルタワー連勝数
//--------------------------------------------------------------------
typedef struct {
	u16 renshou;
	u16 monsno;
	u8 mons_sex;
	u8 mons_region;
	u8 mons_rom;
	u8 IsSingleFlag;
}RECORD_BTOWER_WORK;

//--------------------------------------------------------------------
///	ギネス型０３：大きさ比べ記録
//--------------------------------------------------------------------
typedef struct {
	u16 monsno;
	u8 mons_sex;
	u8 mons_region;
	u8 mons_rom;
	u32 size;
}RECORD_OOKISA_WORK;

//--------------------------------------------------------------------
///	ギネス型０４：スロット達人
//--------------------------------------------------------------------
typedef struct {
	u32 start_coin;
	u32 end_coin;
	u32 playtime;
}RECORD_SLOT_WORK;

//--------------------------------------------------------------------
///	ギネス型０５：リボンコレクター★
//--------------------------------------------------------------------
typedef struct {
	STRCODE nickname[MONS_NAME_SIZE + EOM_SIZE];
	u8	ribbon_id;				//最後に獲得したリボン
	u8	ribbon_total;			//総リボン数
	u8	nickname_flag;
	u8	mons_sex;
	u8	mons_region;
	u8	mons_rom;
	u16 monsno;
}RECORD_RIBBON_WORK;

//木の実育ての名人？？？■

//--------------------------------------------------------------------
///	ギネス型０８：ワナ解除名人
//--------------------------------------------------------------------
typedef struct {
	u16	trap_no;
	u16 remove_count;
}RECORD_REMOVE_TRAP_WORK;

//--------------------------------------------------------------------
///	ギネス型０９：ハタトリ名人
//--------------------------------------------------------------------
typedef struct {
	u16 flag_count;
}RECORD_GET_FLAG_WORK;

//--------------------------------------------------------------------
///	ギネス型１０：バトルポイント名人★
//--------------------------------------------------------------------
typedef struct {
	TOPICTEMP_BP bp;
}RECORD_BP_WORK;

//--------------------------------------------------------------------
///	ギネス型１１：GTS名人★
//--------------------------------------------------------------------
typedef struct {
	TOPICTEMP_GTS gts;
}RECORD_GTS_WORK;


//============================================================================================
//	質問型(★は新規、■は削除)
//============================================================================================

//--------------------------------------------------------------------
///	質問型０２：バトルタワー
//--------------------------------------------------------------------
typedef struct {
	TOPICTEMP_BTOWER btower;
	PMS_WORD word;
}QANDA_BTOWER_WORK;

//--------------------------------------------------------------------
///	質問型０４：ポケモン
//--------------------------------------------------------------------
typedef struct {
	u16 monsno;
	u8 mons_sex;
	u8 mons_region;
	u8 mons_rom;
	u8 nickname_flag;
	STRCODE nickname[MONS_NAME_SIZE + EOM_SIZE];
	PMS_WORD word;
}QANDA_POKEMON_WORK;

//--------------------------------------------------------------------
///	質問型０６：ポケッチ
//--------------------------------------------------------------------
typedef struct {
	int poketch_id;
	PMS_WORD word;
}QANDA_POKETCH_WORK;

//--------------------------------------------------------------------
///	質問型０７：コンテスト
//--------------------------------------------------------------------
typedef struct {
	TOPICTEMP_CONTEST contest;
	PMS_WORD word;
}QANDA_CONTEST_WORK;

//--------------------------------------------------------------------
///	質問型０９：クリップ
//--------------------------------------------------------------------
typedef struct {
	u16 monsno;
	PMS_WORD word;
}QANDA_CLIP_WORK;

//--------------------------------------------------------------------
///	質問型１０：タイプ
//--------------------------------------------------------------------
typedef struct {
	int type;
}QANDA_POKETYPE_WORK;

//--------------------------------------------------------------------
///	質問型１１：ポルト
//--------------------------------------------------------------------
typedef struct {
	TOPICTEMP_PORUTO poruto;
	PMS_WORD word;
}QANDA_PORUTO_WORK;

//--------------------------------------------------------------------
///	質問型１３：連れ歩き
//--------------------------------------------------------------------
typedef struct {
	TOPICTEMP_PARK park;
	PMS_WORD word;
}QANDA_PARK_WORK;

//--------------------------------------------------------------------
///	質問型１４：バトルステージ★
//--------------------------------------------------------------------
typedef struct {
	TOPICTEMP_STAGE stage;
	PMS_WORD word;
}QANDA_STAGE_WORK;

//--------------------------------------------------------------------
///	質問型１５：へんなアンケート★
//--------------------------------------------------------------------
typedef struct {
	PMS_WORD word;
}QANDA_STRANGE_WORK1;

//--------------------------------------------------------------------
///	質問型１６：へんなアンケート★
//--------------------------------------------------------------------
typedef struct {
	PMS_WORD word;
}QANDA_STRANGE_WORK2;

//--------------------------------------------------------------------
///	質問型１７：へんなアンケート★
//--------------------------------------------------------------------
typedef struct {
	PMS_WORD word;
}QANDA_STRANGE_WORK3;

//--------------------------------------------------------------------
///	質問型１８：へんなアンケート★
//--------------------------------------------------------------------
typedef struct {
	PMS_WORD word;
}QANDA_STRANGE_WORK4;

//--------------------------------------------------------------------
///	質問型１９：フロンティアで仲良し★
//--------------------------------------------------------------------
typedef struct {
	TOPICTEMP_FRIEND friend;
	PMS_WORD word;
}QANDA_FRIEND_WORK;


//--------------------------------------------------------------------
/**
 * @brief	トピック保持データ用共有体定義
 */
//--------------------------------------------------------------------
typedef union {
//ダミー定義
	TOPIC_DUMMY_DATA DummyData;								//このデータが40byte(TOPIC_DATA_SIZE)

//監視型用
	TVWATCH_BATTLE_WORK WatchCaptureWork;
	WATCH_FISHING_WORK WatchFishingWork;
	WATCH_GROUP_WORK WatchGroupWork;
	WATCH_HIDEITEM_WORK WatchHideItemWork;
	WATCH_SHOPPING_WORK WatchShoppingWork;					//★
	WATCH_HATCHEGG_WORK WatchHatchEggWork;
	WATCH_MONSNAMECHG_WORK WatchMonsNameChgWork;
	WATCH_DIG_FOSSIL_WORK WatchDigFossilWork;
	WATCH_SAFARI_WORK WatchSafariWork;
	WATCH_OSUMESU_WORK WatchOsuMesuWork;					//★
	WATCH_KANPO_WORK WatchKanpoWork;						//■
	COMMON_TAKE_NUTS_WORK TakeNutsWork;
	WATCH_CUSTOMCAPSULE_WORK WatchCustomCapsuleWork;
	WATCH_UG_FLAG_WORK WatchUgFlagWork;
	WATCH_VILLA_WORK WatchVillaWork;						//★
	WATCH_RECONSTRUCTION_WORK WatchReconstructionWork;		//★

//ギネス型用
	RECORD_BTOWER_WORK RecordBTowerWork;
	RECORD_OOKISA_WORK RecordOokisaWork;
	RECORD_SLOT_WORK RecordSlotWork;
	RECORD_RIBBON_WORK RecordRibbonWork;					//★
	//木の実名人？											//■
	RECORD_REMOVE_TRAP_WORK RecordRemoveTrapWork;
	RECORD_GET_FLAG_WORK RecordGetFlagWork;
	RECORD_BP_WORK RecordBPWork;							//★
	RECORD_GTS_WORK RecordGTSWork;							//★

//質問型用
	QANDA_BTOWER_WORK QandABtowerWork;
	QANDA_POKEMON_WORK QandAPokemonWork;
	QANDA_POKETCH_WORK QandAPoketchWork;
	QANDA_CONTEST_WORK QandAContestWork;
	QANDA_CLIP_WORK QandAClipWork;
	QANDA_POKETYPE_WORK QandAPokeTypeWork;
	QANDA_PORUTO_WORK QandAPorutoWork;
	QANDA_PARK_WORK QandAParkWork;
	QANDA_STAGE_WORK QandAStageWork;						//★
	QANDA_STRANGE_WORK1 QandAStrangeWork1;					//★
	QANDA_STRANGE_WORK2 QandAStrangeWork2;					//★
	QANDA_STRANGE_WORK3 QandAStrangeWork3;					//★
	QANDA_STRANGE_WORK4 QandAStrangeWork4;					//★
	QANDA_FRIEND_WORK QandAFriendWork;						//★

//イベントとバラエティは保存するデータがないのでいらない！

}TOPIC_DATA;


//--------------------------------------------------------------------
/**
 * @brief	トピック生成関数の型定義
 */
//--------------------------------------------------------------------
typedef int (* MAKE_TOPIC_FUNC)(FIELDSYS_WORK *, WORDSET *, TVBCD *);

//--------------------------------------------------------------------
/**
 * @brief	トピック放送制限チェック関数の型定義
 */
//--------------------------------------------------------------------
typedef BOOL (* CHK_LIMIT_FUNC)(FIELDSYS_WORK *, TVBCD *);

typedef struct {
	MAKE_TOPIC_FUNC make_func;
	CHK_LIMIT_FUNC chk_limit_func;
}TOPIC_FUNC_DATA;

typedef struct {
	TVTOPICTYPE topic_type;
	u16 arc_id;
	u16 topic_max;
	const TOPIC_FUNC_DATA * func_data;
}TOPIC_INFO_DATA;

static const TOPIC_FUNC_DATA WatchNewsFuncTable[TVTOPIC_WATCH_MAX - 1];
static const TOPIC_FUNC_DATA RecordNewsFuncTable[TVTOPIC_RECORD_MAX - 1];
static const TOPIC_FUNC_DATA QandANewsFuncTable[TVTOPIC_INTERVIEW_TYPE_MAX - 1];
static const TOPIC_FUNC_DATA EventNewsFuncTable[TVTOPIC_EVENT_TYPE_MAX - 1];
static const TOPIC_FUNC_DATA VarietyNewsFuncTable[TVTOPIC_VARIETY_TYPE_MAX - 1];	//★

//--------------------------------------------------------------------
//	バラエティを増やした★
//--------------------------------------------------------------------
static const TOPIC_INFO_DATA TopicInfoData[] = {
	{
		TVTOPICTYPE_INTERVIEW,
		NARC_msg_msg_tv_01_dat,
		TVTOPIC_INTERVIEW_TYPE_MAX,
		QandANewsFuncTable,
	},
	{
		TVTOPICTYPE_WATCH,
		NARC_msg_msg_tv_02_dat,
		TVTOPIC_WATCH_MAX,
		WatchNewsFuncTable,
	},
	{
		TVTOPICTYPE_RECORD,
		NARC_msg_msg_tv_03_dat,
		TVTOPIC_RECORD_MAX,
		RecordNewsFuncTable,
	},
	{
		TVTOPICTYPE_EVENT,
		NARC_msg_msg_tv_04_dat,
		TVTOPIC_EVENT_TYPE_MAX,
		EventNewsFuncTable,
	},
	{
		TVTOPICTYPE_VARIETY,				//★バラエティ
		NARC_msg_msg_tv_05_dat,
		TVTOPIC_VARIETY_TYPE_MAX,
		VarietyNewsFuncTable,
	},
};

//============================================================================================
//============================================================================================
//--------------------------------------------------------------------
///	トピック定義データ取得
//--------------------------------------------------------------------
static const TOPIC_INFO_DATA * GetBCData(TVTOPICTYPE topic_type)
{
	const TOPIC_INFO_DATA * tpdata;
	GF_ASSERT(TVTOPICTYPE_NOTHING< topic_type && topic_type < TVTOPICTYPE_MAX);
	tpdata = &TopicInfoData[topic_type - 1];
	GF_ASSERT(tpdata->topic_type == topic_type);
	return tpdata;
}

//--------------------------------------------------------------------
///	トピック関数取得
//--------------------------------------------------------------------
static const TOPIC_FUNC_DATA * GetFuncData(const TOPIC_INFO_DATA * tpdata, const TVBCD * bcd)
{
	int topic_id;
	topic_id = TVBCD_GetTopicID(bcd);
	GF_ASSERT(0 < topic_id && topic_id < tpdata->topic_max);
	return &(tpdata->func_data[topic_id - 1]);
}

//--------------------------------------------------------------------
/**
 * @brief	トピック生成処理
 *
 * @param	topic_type	放送するトピックの種類（TVTOPICTYPE）
 * @param	fsys		フィールド制御ワークへのポインタ
 * @param	ws			WORDSETへのポインタ
 * @param	bcd			放送内容データへのポインタ
 * @return	int			放送するMSGのID
 *
 * MSGのIDを返すとともに、WORDSETに必要な項目登録を行う
 */
//--------------------------------------------------------------------
int TVTOPIC_Make(TVTOPICTYPE topic_type, FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd, u16 * arc_id)
{
	MAKE_TOPIC_FUNC func;
	const TOPIC_INFO_DATA * tpdata;
	const TOPIC_FUNC_DATA * funcdata;
	
	tpdata = GetBCData(topic_type);
	*arc_id = tpdata->arc_id;
	funcdata = GetFuncData(tpdata, bcd);
	func = funcdata->make_func;
	GF_ASSERT(func != NULL);
	return func(fsys, ws, bcd);
}

//--------------------------------------------------------------------
/**
 * @brief	トピック制限チェック
 *
 * @param	topic_type	放送するトピックの種類（TVTOPICTYPE）
 * @param	fsys		フィールド制御ワークへのポインタ
 * @param	bcd			放送内容データへのポインタ
 * @return	BOOL		放送できるかどうか
 */
//--------------------------------------------------------------------
BOOL TVTOPIC_CheckLimit(TVTOPICTYPE topic_type, FIELDSYS_WORK * fsys, TVBCD * bcd)
{
	CHK_LIMIT_FUNC check_func;
	const TOPIC_INFO_DATA * tpdata;
	const TOPIC_FUNC_DATA * funcdata;
	
	tpdata = GetBCData(topic_type);
	funcdata = GetFuncData(tpdata, bcd);
	if (funcdata->make_func == NULL) {
		//生成関数がない場合は、無条件で放送しない
		return FALSE;
	}
	check_func = funcdata->chk_limit_func;
	if (check_func == NULL) {
		//チェック関数がない場合は無条件に放送できる
		return TRUE;
	}
	return check_func(fsys, bcd);
}




//============================================================================================
//
//
//			共通ツール関数
//
//
//============================================================================================

//--------------------------------------------------------------------
/**
 * @brief	トピック登録呼び出し(引数をfsys→savedataに変更)
 * @param	fsys
 * @param	prog_type
 * @param	topic_id
 * @param	data
 */
//--------------------------------------------------------------------
static void MakeTopicEx(SAVEDATA * savedata, int prog_type, int topic_id, const void * data)
{
	TV_WORK * tvwk = SaveData_GetTvWork(savedata);

	//汎用データサイズは４０バイト固定でなければならない
	GF_ASSERT(sizeof(TOPIC_DATA) == TOPIC_DATA_SIZE);

	TVWORK_MakeTopic(tvwk, prog_type, topic_id, (const u8 *)data);
}

//--------------------------------------------------------------------
/**
 * @brief	トピック登録呼び出し(引数がfsys)
 * @param	fsys
 * @param	prog_type
 * @param	topic_id
 * @param	data
 */
//--------------------------------------------------------------------
static void MakeTopic(FIELDSYS_WORK * fsys, int prog_type, int topic_id, const void * data)
{
	MakeTopic2(fsys->savedata, prog_type, topic_id, data);
}

//--------------------------------------------------------------------
/**
 * @brief	トピック登録呼び出し(引数がSAVEDATA)
 * @param	savedata
 * @param	prog_type
 * @param	topic_id
 * @param	data
 */
//--------------------------------------------------------------------
static void MakeTopic2(SAVEDATA * savedata, int prog_type, int topic_id, const void * data)
{
	//TV_WORK * tvwk = SaveData_GetTvWork(fsys->savedata);
	TV_WORK * tvwk = SaveData_GetTvWork(savedata);

	//汎用データサイズは４０バイト固定でなければならない
	GF_ASSERT(sizeof(TOPIC_DATA) == TOPIC_DATA_SIZE);

	TVWORK_MakeTopic(tvwk, prog_type, topic_id, (const u8 *)data);
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
static void WORDSET_RegisterSTRCODE(WORDSET * ws, int buf_id,
		const STRCODE * str, int sex, int region, int single)
{
	STRBUF * strbuf = STRBUF_Create(64, HEAPID_FIELD);
	STRBUF_SetStringCode(strbuf, str);
	WORDSET_RegisterWord(ws, buf_id, strbuf, sex, single, region);
	STRBUF_Delete(strbuf);
}

//--------------------------------------------------------------------
/**
 * @brief	放送データの製作者名を登録する
 */
//--------------------------------------------------------------------
static void WORDSET_RegisterOwnerName(WORDSET * ws, int buf_id, const TVBCD * bcd)
{
	WORDSET_RegisterSTRCODE(ws, buf_id, TVBCD_GetName(bcd),
			TVBCD_GetSex(bcd), TVBCD_GetRegionCode(bcd), TRUE);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
static void GetNickNameSTRCODE(int heapID, STRCODE * namebuf, POKEMON_PARAM * pp)
{
	STRBUF * strbuf = STRBUF_Create(64, heapID);
	PokeParaGet(pp, ID_PARA_nickname_buf, strbuf);
	STRBUF_GetStringCode(strbuf, namebuf, MONS_NAME_SIZE + EOM_SIZE);
	STRBUF_Delete(strbuf);

}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
static void SetMonsParams(POKEMON_PARAM * pp, u16 * monsno, u8 * sex, u8 * region, u8 * rom)
{
	*monsno = PokeParaGet(pp, ID_PARA_monsno, NULL);
	*sex = PokeParaGet(pp, ID_PARA_sex, NULL);
	*region = PokeParaGet(pp, ID_PARA_country_code, NULL);
	*rom = PokeParaGet(pp, ID_PARA_get_cassette, NULL);
}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
static void WORDSET_RegisterMonsName(WORDSET * ws, int buf_id, u16 monsno,
		u8 sex, u8 region, u8 rom)
{
	STRCODE monsname[MONS_NAME_SIZE + EOM_SIZE];
	MSGDAT_MonsNameGet(monsno, HEAPID_FIELD, monsname);
	WORDSET_RegisterSTRCODE(ws, buf_id, monsname, sex, region, TRUE);

}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
static void WORDSET_RegisterMonsNameEasy(WORDSET * ws, int buf_id, u16 monsno)
{
	STRCODE monsname[MONS_NAME_SIZE + EOM_SIZE];
	MSGDAT_MonsNameGet(monsno, HEAPID_FIELD, monsname);
	WORDSET_RegisterSTRCODE(ws, buf_id, monsname, PM_MALE, PM_LANG, TRUE);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
static void StoreNickName(int heapID, POKEMON_PARAM * pp, u8 * nickname_flag, STRCODE * nickname)
{
	*nickname_flag = PokeParaGet(pp, ID_PARA_nickname_flag, NULL);
	if (*nickname_flag) {
		STRBUF * strbuf = STRBUF_Create(64, heapID);
		PokeParaGet(pp, ID_PARA_nickname_buf, strbuf);
		STRBUF_GetStringCode(strbuf, nickname, MONS_NAME_SIZE + EOM_SIZE);
		STRBUF_Delete(strbuf);
	}
}

//============================================================================================
//
//
//				トピック生成用状況保持
//
//
//============================================================================================
//--------------------------------------------------------------------
/**
 * @brief	コンテスト終了状況の保持
 * @param	tvwk		テレビセーブデータへのポインタ
 * @param	pp			参加したポケモンのPOKEMON_PARAMへのポインタ
 * @param	type		コンテストの種類
 * @param	rank		コンテストランク
 * @param	result		コンテスト順位
 *
 * コンテストが終了したときによびだしてください
 */
//--------------------------------------------------------------------
void TVTOPIC_ContestTemp_Set(TV_WORK * tvwk, POKEMON_PARAM * pp,
		int type, int rank, int result)
{
	TOPICTEMP_CONTEST * pcnt = TVWORK_GetContestTemp(tvwk);

	pcnt->exist_flag = TRUE;
	SetMonsParams(pp, &pcnt->monsno, &pcnt->mons_sex, &pcnt->mons_region, &pcnt->mons_rom);
	pcnt->contest_type = type;
	pcnt->contest_rank = rank;
	pcnt->contest_result = result;
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_TVWORK)
	SVLD_SetCrc(GMDATA_ID_TVWORK);
#endif //CRC_LOADCHECK
}

//--------------------------------------------------------------------
/**
 * @brief	連れ歩き公園状況の保持：連れて行くポケモン
 * @param	tvwk		テレビセーブデータへのポインタ
 * @param	pp			連れて行くポケモンのPOKEMON_PARAM
 *
 * 連れて行くポケモンを決定したときに呼び出してください
 */
//--------------------------------------------------------------------
void TVTOPIC_ParkTemp_Init(TV_WORK * tvwk, POKEMON_PARAM * pp, int heapID)
{
	TOPICTEMP_PARK * ptemp = TVWORK_GetParkTemp(tvwk);

	ptemp->exist_flag = TRUE;
	ptemp->pick_type = PARKPICK_ID_NOTHING;
	ptemp->poke_character = PokeSeikakuGet(pp);
	SetMonsParams(pp, &ptemp->monsno, &ptemp->mons_sex, &ptemp->mons_region, &ptemp->mons_rom);
	ptemp->nickname_flag = PokeParaGet(pp, ID_PARA_nickname_flag, NULL);
	StoreNickName(heapID, pp, &ptemp->nickname_flag, ptemp->nickname);
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_TVWORK)
	SVLD_SetCrc(GMDATA_ID_TVWORK);
#endif //CRC_LOADCHECK
}

//--------------------------------------------------------------------
/**
 * @brief	連れ歩き公園状況の保持：アクセサリ取得
 * @param	tvwk		テレビセーブデータへのポインタ
 * @param	accessories		拾ったアクセサリのID
 *
 *
 * アクセサリを拾ったときに呼び出してください
 */
//--------------------------------------------------------------------
void TVTOPIC_ParkTemp_SetAccessories(TV_WORK * tvwk, int accessories)
{
	TOPICTEMP_PARK * ptemp = TVWORK_GetParkTemp(tvwk);

	ptemp->pick_type = PARKPICK_ID_ACCESSORIES;
	ptemp->accessories = accessories;
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_TVWORK)
	SVLD_SetCrc(GMDATA_ID_TVWORK);
#endif //CRC_LOADCHECK
}

//--------------------------------------------------------------------
/**
 * @brief	連れ歩き公園状況の保持：アイテム取得
 * @param	tvwk		テレビセーブデータへのポインタ
 * @param	item_no		拾ったアイテム
 *
 *
 * アイテムを拾ったときに呼び出してください
 */
//--------------------------------------------------------------------
void TVTOPIC_ParkTemp_SetKinomi(TV_WORK * tvwk, int item_no)
{
	TOPICTEMP_PARK * ptemp = TVWORK_GetParkTemp(tvwk);

	ptemp->pick_type = PARKPICK_ID_ITEM;
	ptemp->item_no = item_no;
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_TVWORK)
	SVLD_SetCrc(GMDATA_ID_TVWORK);
#endif //CRC_LOADCHECK
}

//--------------------------------------------------------------------
/**
 * @brief	ポルトミニゲーム状況の保持
 * @param	tvwk		テレビセーブデータへのポインタ
 * @param	taste_type	ポルトの種類
 *
 * ポルトを作成したときに呼び出してください
 */
//--------------------------------------------------------------------
void TVTOPIC_PorutoTemp_Set(TV_WORK * tvwk, int taste_type)
{
	TOPICTEMP_PORUTO * poruto = TVWORK_GetPorutoTemp(tvwk);

	poruto->exist_flag = TRUE;
	poruto->taste_type = taste_type;
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_TVWORK)
	SVLD_SetCrc(GMDATA_ID_TVWORK);
#endif //CRC_LOADCHECK
}

//--------------------------------------------------------------------
/**
 * @brief	バトルタワー状況の保持
 * @param	tvwk		テレビセーブデータへのポインタ
 * @param	continue_flag		勝ち抜きがつづいているかどうか
 *
 * バトルタワーの受付に戻ったときに呼び出してください
 */
//--------------------------------------------------------------------
void TVTOPIC_BTowerTemp_Set(TV_WORK * tvwk, BOOL continue_flag, u16 win_count)
{
	TOPICTEMP_BTOWER * bt = TVWORK_GetBTowerTemp(tvwk);

	bt->exist_flag = TRUE;
	bt->continue_flag = continue_flag;
	bt->win_count = win_count;
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_TVWORK)
	SVLD_SetCrc(GMDATA_ID_TVWORK);
#endif //CRC_LOADCHECK
}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
void TVTOPIC_SafariTemp_Init(TV_WORK * tvwk)
{
	TOPICTEMP_SAFARI * psaf = TVWORK_GetSafariTemp(tvwk);

	psaf->exist_flag = TRUE;
	psaf->capture_count = 0;
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_TVWORK)
	SVLD_SetCrc(GMDATA_ID_TVWORK);
#endif //CRC_LOADCHECK
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void TVTOPIC_SafariTemp_Set(TV_WORK * tvwk, POKEMON_PARAM * pp)
{
	TOPICTEMP_SAFARI * psaf = TVWORK_GetSafariTemp(tvwk);
	if (psaf->capture_count == 0) {
		SetMonsParams(pp, &psaf->monsno, &psaf->mons_sex, &psaf->mons_region, &psaf->mons_rom);
	}
	psaf->capture_count ++;
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_TVWORK)
	SVLD_SetCrc(GMDATA_ID_TVWORK);
#endif //CRC_LOADCHECK
}

//--------------------------------------------------------------------
/**
 * @brief	バトルステージ状況の保持
 * @param	tvwk		テレビセーブデータへのポインタ
 * @param	pp			POKEMON_PARAM
 *
 * シングルのみで呼ぶようにする
 */
//--------------------------------------------------------------------
void TVTOPIC_StageTemp_Set(TV_WORK * tvwk, POKEMON_PARAM* pp)
{
	TOPICTEMP_STAGE * stage = TVWORK_GetStageTemp(tvwk);

	stage->exist_flag = TRUE;
	SetMonsParams(pp, &stage->monsno, &stage->mons_sex, &stage->mons_region, &stage->mons_rom);
	stage->nickname_flag = PokeParaGet(pp, ID_PARA_nickname_flag, NULL);
	StoreNickName(HEAPID_WORLD, pp, &stage->nickname_flag, stage->nickname);
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_TVWORK)
	SVLD_SetCrc(GMDATA_ID_TVWORK);
#endif //CRC_LOADCHECK
}

//--------------------------------------------------------------------
/**
 * @brief	フロンティアで仲良し状況の保持
 * @param	tvwk		テレビセーブデータへのポインタ
 * @param	bf_no		挑戦した施設
 * @param	pair		遊んだ友達
 */
//--------------------------------------------------------------------
void TVTOPIC_FriendTemp_Set(TV_WORK * tvwk, u8 bf_no, const MYSTATUS* pair)
{
	TOPICTEMP_FRIEND * friend = TVWORK_GetFriendTemp(tvwk);

	friend->exist_flag = TRUE;
	friend->bf_no		= bf_no;
	//friend->friend_name = MyStatus_GetMyName( pair );

	//MI_CpuCopy8( MyStatus_GetMyName(pair), friend->friend_name, (PERSON_NAME_SIZE+EOM_SIZE) );
	PM_strcpy( friend->friend_name, MyStatus_GetMyName(pair) );

	friend->region		= MyStatus_GetRegionCode( pair );
	friend->rom			= MyStatus_GetRomCode( pair );
	friend->friend_sex	= MyStatus_GetMySex( pair );
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_TVWORK)
	SVLD_SetCrc(GMDATA_ID_TVWORK);
#endif //CRC_LOADCHECK
}

//--------------------------------------------------------------------
/**
 * @brief	バトルポイント名人状況の保持
 * @param	tvwk		テレビセーブデータへのポインタ
 */
//--------------------------------------------------------------------
void TVTOPIC_BPTemp_Set(TV_WORK* tvwk, u16 add_bp)
{
	TOPICTEMP_BP * bp = TVWORK_GetBPTemp(tvwk);

	bp->exist_flag = TRUE;
	bp->bp_num += add_bp;

	if( bp->bp_num > 9999 ){
		bp->bp_num = 9999;
	}

#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_TVWORK)
	SVLD_SetCrc(GMDATA_ID_TVWORK);
#endif //CRC_LOADCHECK
}

//初期化
void TVTOPIC_BPTemp_Init(TV_WORK * tvwk)
{
	TOPICTEMP_BP * bp = TVWORK_GetBPTemp(tvwk);

	bp->bp_num = 0;
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_TVWORK)
	SVLD_SetCrc(GMDATA_ID_TVWORK);
#endif //CRC_LOADCHECK
}

//--------------------------------------------------------------------
/**
 * @brief	GTS名人状況の保持
 * @param	tvwk		テレビセーブデータへのポインタ
 */
//--------------------------------------------------------------------
void TVTOPIC_GTSTemp_Set(TV_WORK * tvwk)
{
	TOPICTEMP_GTS * gts = TVWORK_GetGTSTemp(tvwk);

	gts->exist_flag = TRUE;
	gts->trade_num++;

	if( gts->trade_num > 9999 ){
		gts->trade_num = 9999;
	}

#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_TVWORK)
	SVLD_SetCrc(GMDATA_ID_TVWORK);
#endif //CRC_LOADCHECK
}

//初期化
void TVTOPIC_GTSTemp_Init(TV_WORK * tvwk)
{
	TOPICTEMP_GTS * gts = TVWORK_GetGTSTemp(tvwk);

	gts->trade_num = 0;
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_TVWORK)
	SVLD_SetCrc(GMDATA_ID_TVWORK);
#endif //CRC_LOADCHECK
}

//============================================================================================
//
//
//				監視型トピック
//
//
//============================================================================================

//============================================================================================
//	０２－０１、０２－０２	捕獲	
//============================================================================================
//--------------------------------------------------------------------
/**
 */
//--------------------------------------------------------------------
TVWATCH_BATTLE_WORK * TVWatchBattleWork_Create(int heapID)
{
	TVWATCH_BATTLE_WORK * tvwbw;
	tvwbw = sys_AllocMemory(heapID, sizeof(TVWATCH_BATTLE_WORK));
	MI_CpuClearFast(tvwbw, sizeof(TVWATCH_BATTLE_WORK));
	return tvwbw;
}

//--------------------------------------------------------------------
/**
 */
//--------------------------------------------------------------------
void TVWatchBattleWork_Delete(TVWATCH_BATTLE_WORK * tvwbw)
{
	sys_FreeMemoryEz(tvwbw);
}

//--------------------------------------------------------------------
/**
 * @brief	テレビトピック生成処理：バトル捕獲
 * @param	tvwbw			テレビトピック用情報保持ワーク
 * @param	pp				対象のポケモン
 * @param	win_lose_flag	戦闘結果
 * @param	ball_count		使用したボールの数
 * @param	heapID			利用可能なヒープ指定
 */
//--------------------------------------------------------------------
void TVBATTLE_StoreResult(TVWATCH_BATTLE_WORK * tvwbw, POKEMON_PARAM * pp,
		int win_lose_flag, int ball_count, u32 heapID)
{
	MI_CpuClear32(tvwbw, sizeof(TVWATCH_BATTLE_WORK));

	/* どうやらこの時点ではwin_lose_flagはセットされていないらしい */
	tvwbw->win_lose_flag = win_lose_flag;
	tvwbw->ball_count = ball_count;
	SetMonsParams(pp, &tvwbw->monsno, &tvwbw->mons_sex, &tvwbw->mons_region, &tvwbw->mons_rom);

	tvwbw->ball_item_no = PokeParaGet(pp, ID_PARA_get_ball, NULL);
	GF_ASSERT(tvwbw->ball_item_no);
	StoreNickName(heapID, pp, &tvwbw->nickname_flag, tvwbw->nickname);
}
//--------------------------------------------------------------------
/**
 * @brief	テレビトピック生成：捕獲
 * @param	fsys	フィールド制御ワークへのポインタ
 * @param	tvwbw			テレビトピック用情報保持ワーク
 */
//--------------------------------------------------------------------
void TVTOPIC_Entry_Watch_Capture(FIELDSYS_WORK * fsys,
		const TVWATCH_BATTLE_WORK * wcw_result, int win_lose_flag)
{
	TOPIC_DATA TopicData;
	TVWATCH_BATTLE_WORK * tvwbw = &TopicData.WatchCaptureWork;

	*tvwbw = *wcw_result;
	tvwbw->win_lose_flag = win_lose_flag;

	if (tvwbw->ball_count == 0) {
		//ボールを投げていない場合、何もしない
		return;
	}

	if (tvwbw->ball_count > 999) {
		//表示は3桁までなので制限する
		tvwbw->ball_count = 999;
	}
	
	switch (win_lose_flag) {
	case FIGHT_POKE_GET:
		MakeTopic(fsys, TVTOPICTYPE_WATCH, TVTOPIC_WATCH_POKE_GET_SUCCESS, tvwbw);
		break;
	case FIGHT_WIN:
	case FIGHT_ESCAPE:
	case FIGHT_ENEMY_ESCAPE:
		if (tvwbw->ball_count > 2) {
			MakeTopic(fsys, TVTOPICTYPE_WATCH, TVTOPIC_WATCH_POKE_GET_FAILURE, tvwbw);
		}
		break;
	}
}

//--------------------------------------------------------------------
//ポケモン捕獲成功！	ポケモン捕獲終了後
//--------------------------------------------------------------------
static int TVTOPIC_Make_Watch_SuccessCapture(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	TVWATCH_BATTLE_WORK * tvwbw = TVBCD_GetTopicData(bcd);

	if (tvwbw->nickname_flag) {
		//ニックネームをつけた場合
		WORDSET_RegisterOwnerName(ws, 0, bcd);		//トレーナー名
		WORDSET_RegisterMonsName(ws, 1, tvwbw->monsno,
				tvwbw->mons_sex, tvwbw->mons_region, tvwbw->mons_rom);
		WORDSET_RegisterItemName(ws, 2, tvwbw->ball_item_no);
		WORDSET_RegisterNumber(ws, 3, tvwbw->ball_count, 3,
				NUMBER_DISPTYPE_LEFT, NUMBER_CODETYPE_DEFAULT);
		WORDSET_RegisterSTRCODE(ws, 4, tvwbw->nickname,
				tvwbw->mons_sex, tvwbw->mons_region, TRUE);
		return msg_tv_02_01_01;
	} else {
		//ニックネームをつけなかった場合
		WORDSET_RegisterOwnerName(ws, 0, bcd);		//トレーナー名
		WORDSET_RegisterMonsName(ws, 1, tvwbw->monsno,
				tvwbw->mons_sex, tvwbw->mons_region, tvwbw->mons_rom);
		WORDSET_RegisterItemName(ws, 2, tvwbw->ball_item_no);
		WORDSET_RegisterNumber(ws, 3, tvwbw->ball_count, 3,
				NUMBER_DISPTYPE_LEFT, NUMBER_CODETYPE_DEFAULT);
		return msg_tv_02_01_02;
	}
}

//--------------------------------------------------------------------
//ポケモン捕獲失敗		ボール試投３以上で倒す、逃がす
//--------------------------------------------------------------------
static int TVTOPIC_Make_Watch_FailureCapture(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	TVWATCH_BATTLE_WORK * tvwbw = TVBCD_GetTopicData(bcd);
	WORDSET_RegisterOwnerName(ws, 0, bcd);		//トレーナー名
	WORDSET_RegisterNumber(ws, 1, tvwbw->ball_count, 3,
			NUMBER_DISPTYPE_LEFT, NUMBER_CODETYPE_DEFAULT);
	if (tvwbw->win_lose_flag == FIGHT_WIN) {
		return msg_tv_02_02_01;
	} else {
		return msg_tv_02_02_02;
	}
}
//--------------------------------------------------------------------
///	トピック制限チェック：ポケモン捕獲
//--------------------------------------------------------------------
static BOOL TVTOPIC_Limit_Watch_Capture(FIELDSYS_WORK * fsys, TVBCD * bcd)
{
	ZUKAN_WORK * zw = SaveData_GetZukanWork(fsys->savedata);
	TVWATCH_BATTLE_WORK * tvwbw = TVBCD_GetTopicData(bcd);
	return ZukanWork_GetPokeSeeFlag(zw, tvwbw->monsno);
}


//============================================================================================
//	０２－０３	つり
//============================================================================================
//--------------------------------------------------------------------
/**
 * @brief	トピック生成：世界を釣る！
 * @param	fsys	フィールド制御ワークへのポインタ
 * @param	success_flag	成功したかどうか
 * @param	itemno			使用したつりざおのアイテムナンバー
 * @param	pp				釣り上げたポケモンのPOKEMON_PARAMへのポインタ
 *
 * 釣り上げられなかった場合、success_flag == FALSEでPP==NULLでよい
 */
//--------------------------------------------------------------------
void TVTOPIC_Entry_Watch_Fishing(FIELDSYS_WORK * fsys, BOOL success_flag,
		u16 itemno, POKEMON_PARAM * pp)
{
	TOPIC_DATA TopicData;
	WATCH_FISHING_WORK * wfw = &TopicData.WatchFishingWork;

	if (success_flag) {
		SetMonsParams(pp, &wfw->monsno, &wfw->mons_sex, &wfw->mons_region, &wfw->mons_rom);
	}
	wfw->itemno = itemno;
	wfw->success_flag = success_flag;

	MakeTopic(fsys, TVTOPICTYPE_WATCH, TVTOPIC_WATCH_FISHING, wfw);
}
//--------------------------------------------------------------------
///	トピック放送：世界を釣る！
//--------------------------------------------------------------------
static int TVTOPIC_Make_Watch_Fishing(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	WATCH_FISHING_WORK * wfw = (WATCH_FISHING_WORK * )TVBCD_GetTopicData(bcd);

	WORDSET_RegisterOwnerName(ws, 0, bcd);
	if (wfw->success_flag) {
		//釣り上げた場合
		WORDSET_RegisterItemName(ws, 1, wfw->itemno);
		WORDSET_RegisterMonsName(ws, 2, wfw->monsno,
				wfw->mons_sex, wfw->mons_region, wfw->mons_rom);
		return msg_tv_02_03_01;
	} else {
		//失敗した場合
		return msg_tv_02_03_02;
	}
}
//--------------------------------------------------------------------
///	トピック制限チェック：世界を釣る！
//--------------------------------------------------------------------
static BOOL TVTOPIC_Limit_Watch_Fishing(FIELDSYS_WORK * fsys, TVBCD * bcd)
{
	WATCH_FISHING_WORK * wfw = (WATCH_FISHING_WORK * )TVBCD_GetTopicData(bcd);
	if (wfw->success_flag == FALSE) {
		//失敗した場合はポケモンの種類は関係ないので放送する
		return TRUE;
	}
	return ZukanWork_GetPokeSeeFlag(SaveData_GetZukanWork(fsys->savedata),wfw->monsno);
}


//============================================================================================
//	０２－０４	グループを入れ替える
//	０２－２０	新グループ作成
//============================================================================================
//--------------------------------------------------------------------
//--------------------------------------------------------------------
static void TV_Entry_CommonGroupTopic(FIELDSYS_WORK  * fsys, int topic_id)
{
	TOPIC_DATA TopicData;
	WATCH_GROUP_WORK * wgw = &TopicData.WatchGroupWork;

	RANDOM_GROUP * rg = SaveData_GetRandomGroup(fsys->savedata);

	GF_ASSERT(sizeof(TOPIC_DATA) == TOPIC_DATA_SIZE);
	MI_CpuClearFast(&TopicData, TOPIC_DATA_SIZE);

	PM_strncpy(wgw->groupname,
			RandomGroup_GetNamePtr(rg, RANDOMGROUP_MYUSE, RANDOMGROUP_NAME_GROUP),
			MONS_NAME_SIZE + EOM_SIZE);
	MakeTopic(fsys, TVTOPICTYPE_WATCH, topic_id, wgw);
}
//--------------------------------------------------------------------
/**
 * @brief	トピック生成：グループ作成
 */
//--------------------------------------------------------------------
void TVTOPIC_Entry_Watch_MakeGroup(FIELDSYS_WORK * fsys)
{
	TV_Entry_CommonGroupTopic(fsys, TVTOPIC_WATCH_MAKE_NEW_GROUP);
}
//--------------------------------------------------------------------
/**
 * @brief	トピック生成：グループ入れ替え
 */
//--------------------------------------------------------------------
void TVTOPIC_Entry_Watch_ChangeGroup(FIELDSYS_WORK * fsys)
{
	TV_Entry_CommonGroupTopic(fsys, TVTOPIC_WATCH_CHANGE_GROUP);
}

//--------------------------------------------------------------------
/**
 * @brief	トピック放送：グループ入れ替え
 */
//--------------------------------------------------------------------
static int TVTOPIC_Make_Watch_ChangeGroup(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	WATCH_GROUP_WORK * wgw = (WATCH_GROUP_WORK * )TVBCD_GetTopicData(bcd);
	WORDSET_RegisterSTRCODE(ws, 1, wgw->groupname, PM_MALE, TVBCD_GetRegionCode(bcd), TRUE);
	WORDSET_RegisterOwnerName(ws, 0, bcd);
	
	return msg_tv_02_04_01;
}
//--------------------------------------------------------------------
/**
 * @brief	トピック放送：グループ作成
 */
//--------------------------------------------------------------------
static int TVTOPIC_Make_Watch_MakeNewGroup(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	WATCH_GROUP_WORK * wgw = (WATCH_GROUP_WORK * )TVBCD_GetTopicData(bcd);
	WORDSET_RegisterSTRCODE(ws, 1, wgw->groupname, PM_MALE, TVBCD_GetRegionCode(bcd), TRUE);
	WORDSET_RegisterOwnerName(ws, 0, bcd);
	
	return msg_tv_02_20_01;
}


//============================================================================================
//	０２－０６	隠し道具発見！		隠し道具を見つけたとき
//============================================================================================
//--------------------------------------------------------------------
/**
 * @brief	トピック生成：隠しアイテム発見！
 * @param	fsys	フィールド制御ワークへのポインタ
 * @param	item_no		見つけたアイテムのナンバー
 */
//--------------------------------------------------------------------
void TVTOPIC_Entry_Watch_HideItem(FIELDSYS_WORK * fsys, u16 item_no)
{
	TOPIC_DATA TopicData;
	WATCH_HIDEITEM_WORK * whw = &TopicData.WatchHideItemWork;
	whw->item_no = item_no;
	whw->place_name_id = ZoneData_GetPlaceNameID(fsys->location->zone_id);

	MakeTopic(fsys, TVTOPICTYPE_WATCH, TVTOPIC_WATCH_GET_HIDE_ITEM, whw);
}
//--------------------------------------------------------------------
/**
 * @brief	トピック放送：隠しアイテム発見！
 */
//--------------------------------------------------------------------
static int TVTOPIC_Make_Watch_HideItem(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	WATCH_HIDEITEM_WORK * whw = (WATCH_HIDEITEM_WORK *)TVBCD_GetTopicData(bcd);
	WORDSET_RegisterPlaceName(ws, 0, whw->place_name_id);
	WORDSET_RegisterOwnerName(ws, 1, bcd);
	WORDSET_RegisterItemName(ws, 2, whw->item_no);

	return msg_tv_02_06_01;
}


//============================================================================================
//	０２－０７	買い物チェック		一度に10個以上の道具を買った時★
//============================================================================================
//--------------------------------------------------------------------
/**
 * @brief	トピック生成：買い物チェック★
 * @param	savedata	SAVEDATAへのポインタ(注意！ fsysから変更した★)
 * @param	item_no		アイテムナンバー
 * @param	item_num	個数
 */
//--------------------------------------------------------------------
//void TVTOPIC_Entry_Watch_Shopping(FIELDSYS_WORK * fsys, u16 item_no, u8 item_num)
void TVTOPIC_Entry_Watch_Shopping(SAVEDATA * savedata, u16 item_no, u8 item_num)
{
	TOPIC_DATA TopicData;
	WATCH_SHOPPING_WORK * wsw = &TopicData.WatchShoppingWork;

	if( item_num >= 10 ){
		wsw->item_no = item_no;
		wsw->item_num= item_num;
		if( wsw->item_num > 999 ){
			wsw->item_num = 999;
		}
		OS_Printf( "item_no = %d\n", wsw->item_no );
		OS_Printf( "item_num = %d\n", wsw->item_num );

		//MakeTopic(fsys, TVTOPICTYPE_WATCH, TVTOPIC_WATCH_SHOPPING, wsw);
		MakeTopic2(savedata, TVTOPICTYPE_WATCH, TVTOPIC_WATCH_SHOPPING, wsw);
	}
}
//--------------------------------------------------------------------
/**
 * @brief	トピック放送：買い物チェック★
 */
//--------------------------------------------------------------------
static int TVTOPIC_Make_Watch_Shopping(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	WATCH_SHOPPING_WORK * wsw = (WATCH_SHOPPING_WORK *)TVBCD_GetTopicData(bcd);
	WORDSET_RegisterItemName(ws, 0, wsw->item_no);
	WORDSET_RegisterNumber(ws, 1, wsw->item_num, 3,
			NUMBER_DISPTYPE_LEFT, NUMBER_CODETYPE_DEFAULT);
	WORDSET_RegisterOwnerName(ws, 2, bcd);
    WORDSET_RegisterItemNamePlural(ws, 3, wsw->item_no);
	return msg_tv_02_07_01;
}


//============================================================================================
//	０２－０８	タマゴが孵った！
//============================================================================================
//--------------------------------------------------------------------
/**
 * @brief	トピック生成：タマゴが孵った！
 * @param	fsys	フィールド制御ワークへのポインタ
 * @param	孵ったポケモンへのポインタ
 */
//--------------------------------------------------------------------
void TVTOPIC_Entry_Watch_HatchEgg(FIELDSYS_WORK * fsys, POKEMON_PARAM * pp)
{
	TOPIC_DATA TopicData;
	WATCH_HATCHEGG_WORK * whew = &TopicData.WatchHatchEggWork;

	SetMonsParams(pp, &whew->monsno, &whew->mons_sex, &whew->mons_region, &whew->mons_rom);
	whew->place_name_id = ZoneData_GetPlaceNameID(fsys->location->zone_id);

	MakeTopic(fsys, TVTOPICTYPE_WATCH, TVTOPIC_WATCH_BORN_EGG, whew);
}
//--------------------------------------------------------------------
///	トピック放送：タマゴが孵った！	タマゴが孵化したとき
//--------------------------------------------------------------------
static int TVTOPIC_Make_Watch_HatchEgg(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	WATCH_HATCHEGG_WORK * whew = (WATCH_HATCHEGG_WORK *)TVBCD_GetTopicData(bcd);
	WORDSET_RegisterPlaceName(ws, 1, whew->place_name_id);
	WORDSET_RegisterOwnerName(ws, 0, bcd);
	WORDSET_RegisterMonsName(ws, 2, whew->monsno,
			whew->mons_sex, whew->mons_region, whew->mons_rom);
	return msg_tv_02_08_01;
}
//--------------------------------------------------------------------
///	トピック制限チェック：タマゴが孵った！	タマゴが孵化したとき
//--------------------------------------------------------------------
static BOOL TVTOPIC_Limit_Watch_HatchEgg(FIELDSYS_WORK * fsys, TVBCD * bcd)
{
	WATCH_HATCHEGG_WORK * whew = (WATCH_HATCHEGG_WORK *)TVBCD_GetTopicData(bcd);
	return ZukanWork_GetPokeSeeFlag(SaveData_GetZukanWork(fsys->savedata),whew->monsno);
}


//============================================================================================
//	０２－１０	姓名判断
//============================================================================================
//--------------------------------------------------------------------
/**
 * @brief	トピック生成：姓名判断	ニックネームを変更したとき
 * @param	fsys	フィールド制御ワークへのポインタ
 * @param	pp		名前を変えたポケモンへのポインタ
 */
//--------------------------------------------------------------------
void TVTOPIC_Entry_Watch_ChangeName(FIELDSYS_WORK * fsys, POKEMON_PARAM * pp)
{
	TOPIC_DATA TopicData;
	WATCH_MONSNAMECHG_WORK * wmcw = &TopicData.WatchMonsNameChgWork;

	SetMonsParams(pp, &wmcw->monsno, &wmcw->mons_sex, &wmcw->mons_region, &wmcw->mons_rom);
	GetNickNameSTRCODE(HEAPID_FIELD, wmcw->newname, pp);

	MakeTopic(fsys, TVTOPICTYPE_WATCH, TVTOPIC_WATCH_CHANGE_NICKNAME, wmcw);
}
//--------------------------------------------------------------------
///	トピック放送：姓名判断	ニックネームを変更したとき
//--------------------------------------------------------------------
static int TVTOPIC_Make_Watch_ChangeName(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	WATCH_MONSNAMECHG_WORK * wmcw = (WATCH_MONSNAMECHG_WORK *)TVBCD_GetTopicData(bcd);
	WORDSET_RegisterOwnerName(ws, 0, bcd);
	WORDSET_RegisterMonsName(ws, 1, wmcw->monsno,
			wmcw->mons_sex, wmcw->mons_region, wmcw->mons_rom);
	WORDSET_RegisterSTRCODE(ws, 2, wmcw->newname, wmcw->mons_sex, wmcw->mons_region, TRUE);

	return msg_tv_02_10_01 + gf_p_rand(5);
}

//--------------------------------------------------------------------
///	トピック制限チェック：姓名判断	ニックネームを変更したとき
//--------------------------------------------------------------------
static BOOL TVTOPIC_Limit_Watch_ChangeName(FIELDSYS_WORK * fsys, TVBCD * bcd)
{
	ZUKAN_WORK * zw = SaveData_GetZukanWork(fsys->savedata);
	WATCH_MONSNAMECHG_WORK * wmcw = (WATCH_MONSNAMECHG_WORK *)TVBCD_GetTopicData(bcd);
	return ZukanWork_GetPokeSeeFlag(zw, wmcw->monsno);
}


//============================================================================================
//	０２－１３　化石掘り
//============================================================================================
//--------------------------------------------------------------------
/**
 * @brief	トピック生成：化石掘り	化石掘りをしたあと
 * @param	fsys	フィールド制御ワークへのポインタ
 * @param	fossil_type		化石のアイテムナンバー
 * @param	fossil_num		取れた化石の数
 */
//--------------------------------------------------------------------
void TVTOPIC_Entry_Watch_DigFossil(FIELDSYS_WORK  *fsys, int fossil_type, int fossil_num)
{
	TOPIC_DATA TopicData;
	WATCH_DIG_FOSSIL_WORK * wdfw = &TopicData.WatchDigFossilWork;
	wdfw->fossil_type = fossil_type;
	wdfw->fossil_num = fossil_num;

	MakeTopic(fsys, TVTOPICTYPE_WATCH, TVTOPIC_WATCH_DIG_FOSSIL, wdfw);
}
//--------------------------------------------------------------------
///	トピック放送：化石掘り	化石掘りをしたあと
//--------------------------------------------------------------------
static int TVTOPIC_Make_Watch_DigFossil(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	WATCH_DIG_FOSSIL_WORK * wdfw = (WATCH_DIG_FOSSIL_WORK *)TVBCD_GetTopicData(bcd);
	WORDSET_RegisterOwnerName(ws, 0, bcd);
	WORDSET_RegisterUGItemName(ws, 1, wdfw->fossil_type);
	WORDSET_RegisterNumber(ws, 2, wdfw->fossil_num, 3,
			NUMBER_DISPTYPE_LEFT, NUMBER_CODETYPE_DEFAULT);
	return msg_tv_02_13_01;
}


//============================================================================================
//	０２－１５	サファリゲーム
//============================================================================================
//--------------------------------------------------------------------
/**
 * @brief	トピック生成：サファリゲーム	サファリゲームを終了させたとき
 * @param	fsys	フィールド制御ワークへのポインタ
 */
//--------------------------------------------------------------------
void TVTOPIC_Entry_Watch_Safari(FIELDSYS_WORK * fsys)
{
	TOPIC_DATA TopicData;
	WATCH_SAFARI_WORK * wsw = &TopicData.WatchSafariWork;
	TV_WORK * tvwk = SaveData_GetTvWork(fsys->savedata);
	TOPICTEMP_SAFARI * psaf = TVWORK_GetSafariTemp(tvwk);
	if (psaf->capture_count == 0) {
		//一匹も捕まえていない場合、トピック生成されない
		return;
	}
	wsw->monsno = psaf->monsno;
	wsw->mons_sex = psaf->mons_sex;
	wsw->mons_region = psaf->mons_region;
	wsw->mons_rom = psaf->mons_rom;
	wsw->get_count = psaf->capture_count;

	MakeTopic(fsys, TVTOPICTYPE_WATCH, TVTOPIC_WATCH_SAFARI, wsw);
}
//--------------------------------------------------------------------
///	トピック放送：サファリゲーム	サファリゲームを終了させたとき
//--------------------------------------------------------------------
static int TVTOPIC_Make_Watch_Safari(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	WATCH_SAFARI_WORK * wsw = (WATCH_SAFARI_WORK *)TVBCD_GetTopicData(bcd);
	WORDSET_RegisterOwnerName(ws, 0, bcd);
	WORDSET_RegisterMonsName(ws, 1, wsw->monsno, wsw->mons_sex, wsw->mons_region, wsw->mons_rom);
	WORDSET_RegisterNumber(ws, 2, wsw->get_count, 2, NUMBER_DISPTYPE_LEFT, NUMBER_CODETYPE_DEFAULT);

	return msg_tv_02_15_01;
}
//--------------------------------------------------------------------
///	トピック制限チェック：サファリゲーム	サファリゲームを終了させたとき
//--------------------------------------------------------------------
static BOOL TVTOPIC_Limit_Watch_Safari(FIELDSYS_WORK * fsys, TVBCD * bcd)
{
	ZUKAN_WORK * zw = SaveData_GetZukanWork(fsys->savedata);
	WATCH_SAFARI_WORK * wsw = (WATCH_SAFARI_WORK *)TVBCD_GetTopicData(bcd);
	return ZukanWork_GetPokeSeeFlag(zw, wsw->monsno);
}


//============================================================================================
//	０２－１６	オスだけ！メスだけ！		パソコンから抜けた時の手持ちにより★
//============================================================================================
//--------------------------------------------------------------------
/**
 * @brief	トピック生成：オスだけ！メスだけ！★
 * @param	fsys	フィールド制御ワークへのポインタ
 */
//--------------------------------------------------------------------
void TVTOPIC_Entry_Watch_OsuMesu(FIELDSYS_WORK * fsys)
{
	u32 male,female,neutral,sex;
	u8 max,i;
	POKEMON_PARAM* pp;
	TOPIC_DATA TopicData;
	POKEPARTY* party;
	WATCH_OSUMESU_WORK * womw = &TopicData.WatchOsuMesuWork;

	male	= 0;
	female	= 0;
	neutral	= 0;
	party	= SaveData_GetTemotiPokemon( fsys->savedata );
	max		= PokeParty_GetPokeCount( party );

	//性別取得
	for( i=0; i < max; i++ ){
		pp = PokeParty_GetMemberPointer( party, i );

		//タマゴは除外
		if( PokeParaGet(pp,ID_PARA_tamago_flag,NULL) == 0 ){

			sex = PokeParaGet( pp, ID_PARA_sex, NULL );
			if( sex == PM_MALE ){
				male = 1;
			}else if( sex == PM_FEMALE ){
				female = 1;
			}else if( sex == PM_NEUTRAL ){
				neutral = 1;
			}
		}
	}

	//性別なしがいない時
	if( neutral == 0 ){

		//オスしかいない時
		if( (male == 1) && (female == 0) ){

			//先頭のポケモン
			pp = EvPoke_GetPokemonTop(SaveData_GetTemotiPokemon(fsys->savedata));
			SetMonsParams(pp, &womw->monsno, &womw->mons_sex, &womw->mons_region, &womw->mons_rom);

			MakeTopic(fsys, TVTOPICTYPE_WATCH, TVTOPIC_WATCH_OSU_MESU, womw);

		//メスしかいない時
		}else if( (male == 0) && (female == 1) ){

			//先頭のポケモン
			pp = EvPoke_GetPokemonTop(SaveData_GetTemotiPokemon(fsys->savedata));
			SetMonsParams(pp, &womw->monsno, &womw->mons_sex, &womw->mons_region, &womw->mons_rom);

			MakeTopic(fsys, TVTOPICTYPE_WATCH, TVTOPIC_WATCH_OSU_MESU, womw);
		}
	}

	return;
}

//--------------------------------------------------------------------
/**
 * @brief	トピック放送：オスだけ！メスだけ！★
 */
//--------------------------------------------------------------------
static int TVTOPIC_Make_Watch_OsuMesu(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	WATCH_OSUMESU_WORK * womw = (WATCH_OSUMESU_WORK *)TVBCD_GetTopicData(bcd);

	WORDSET_RegisterOwnerName(ws, 0, bcd);
	WORDSET_RegisterMonsName(ws, 1, womw->monsno, womw->mons_sex, 
								womw->mons_region, womw->mons_rom);

	//オスだけ
	if( womw->mons_sex == PM_MALE ){
		return msg_tv_02_16_01;
	}

	//メスだけ
	return msg_tv_02_16_02;
}

//--------------------------------------------------------------------
///	トピック制限チェック：オスだけ！メスだけ！
//--------------------------------------------------------------------
static BOOL TVTOPIC_Limit_Watch_OsuMesu(FIELDSYS_WORK * fsys, TVBCD * bcd)
{
	WATCH_OSUMESU_WORK * wow = (WATCH_OSUMESU_WORK *)TVBCD_GetTopicData(bcd);
	return ZukanWork_GetPokeSeeFlag(SaveData_GetZukanWork(fsys->savedata),wow->monsno);
}


//============================================================================================
//	０２－１７	良薬は口に苦し！	ポケモンに漢方薬をあげたとき■
//============================================================================================
//--------------------------------------------------------------------
/**
 * @brief	トピック生成：良薬は口に苦し！	ポケモンに漢方薬をあげたとき
 * @param	tvwk		テレビセーブデータへのポインタ
 * @param	pp			薬をあげたポケモンへのポインタ
 * @param	itemno		薬のアイテムナンバー
 */
//--------------------------------------------------------------------
void TVTOPIC_Entry_Watch_UseKanpo(TV_WORK * tvwk, POKEMON_PARAM * pp, u16 itemno)
{
#if 0
	TOPIC_DATA TopicData;
	WATCH_KANPO_WORK * wkw = &TopicData.WatchKanpoWork;

	SetMonsParams(pp, &wkw->monsno, &wkw->mons_sex, &wkw->mons_region, &wkw->mons_rom);
	wkw->itemno = itemno;

	TVWORK_MakeTopic(tvwk, TVTOPICTYPE_WATCH, TVTOPIC_WATCH_KANPOU, (const u8 *)wkw);
#else
	return;			//プラチナでは作成されない
#endif
}
//--------------------------------------------------------------------
///	トピック放送：良薬は口に苦し！	ポケモンに漢方薬をあげたとき
//--------------------------------------------------------------------
static int TVTOPIC_Make_Watch_UseKanpo(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	WATCH_KANPO_WORK * wkw = (WATCH_KANPO_WORK *)TVBCD_GetTopicData(bcd);
	WORDSET_RegisterOwnerName(ws, 0, bcd);
	WORDSET_RegisterMonsName(ws, 1, wkw->monsno, wkw->mons_sex, wkw->mons_region, wkw->mons_rom);
	WORDSET_RegisterItemName(ws, 2, wkw->itemno);

	return msg_tv_02_17_01;
}
//--------------------------------------------------------------------
///	トピック制限チェック：良薬は口に苦し！	ポケモンに漢方薬をあげたとき
//--------------------------------------------------------------------
static BOOL TVTOPIC_Limit_Watch_UseKanpo(FIELDSYS_WORK * fsys, TVBCD * bcd)
{
#if 0
	WATCH_KANPO_WORK * wkw = (WATCH_KANPO_WORK *)TVBCD_GetTopicData(bcd);
	return ZukanWork_GetPokeSeeFlag(SaveData_GetZukanWork(fsys->savedata),wkw->monsno);
#else
	return FALSE;	//プラチナでは放送しない
#endif
}


//============================================================================================
//	０２－１８	監視型：きのみ豊作
//	０２－１９	監視型：きのみ不作
//	０３－０６	ギネス型：きのみ名人
//============================================================================================
//--------------------------------------------------------------------
/**
 * @brief	トピック生成：きのみをとったとき
 * @param	fsys			フィールド制御ワークへのポインタ
 * @param	nuts_item_no	とったきのみのアイテムナンバー
 * @param	hp				とったときのHPの値
 * @param	nuts_count		とったきのみの数
 */
//--------------------------------------------------------------------
void TVTOPIC_Entry_TakeNuts(FIELDSYS_WORK  *fsys, u16 nuts_item_no, u8 hp, u16 nuts_count)
{
	TOPIC_DATA TopicData;
	COMMON_TAKE_NUTS_WORK * tnw = &TopicData.TakeNutsWork;
	tnw->nuts_item_no = nuts_item_no;
	tnw->hp = hp;
	tnw->nuts_count = nuts_count;
	if (hp == 5) {
		//HPが満タンのとき、ギネス型トピック生成
		//MakeTopic(fsys, TVTOPICTYPE_RECORD, TVTOPIC_RECORD_NUTS_MASTER, tnw);
	} else if (hp == 4) {
		//HPが４のとき、監視型トピック生成
		MakeTopic(fsys, TVTOPICTYPE_WATCH, TVTOPIC_WATCH_NUTS_HARVEST, tnw);
	} else if (hp == 0) {
		//HPが０のとき、監視型トピック生成
		MakeTopic(fsys, TVTOPICTYPE_WATCH, TVTOPIC_WATCH_NUTS_POOR, tnw);
	}
	/* トピックが生成されない場合もある */
}
//--------------------------------------------------------------------
///	トピック放送：豊作！木の実をたくさん収穫したとき（収穫時、木のHPが８、９だったとき）
//--------------------------------------------------------------------
static int TVTOPIC_Make_Watch_NutsHarvest(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	COMMON_TAKE_NUTS_WORK * tnw = TVBCD_GetTopicData(bcd);
	WORDSET_RegisterOwnerName(ws, 0, bcd);
	WORDSET_RegisterItemName(ws, 1, tnw->nuts_item_no);
	WORDSET_RegisterNumber(ws, 2, tnw->nuts_count, 2, NUMBER_DISPTYPE_LEFT, NUMBER_CODETYPE_DEFAULT);
	return msg_tv_02_18_01;
}
//--------------------------------------------------------------------
///	トピック放送：不作・・・木の実が１つしか収穫できなかったとき
//--------------------------------------------------------------------
static int TVTOPIC_Make_Watch_NutsPoor(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	COMMON_TAKE_NUTS_WORK * tnw = TVBCD_GetTopicData(bcd);
	WORDSET_RegisterOwnerName(ws, 0, bcd);
	WORDSET_RegisterItemName(ws, 1, tnw->nuts_item_no);
	return msg_tv_02_19_01;
}
//--------------------------------------------------------------------
///	トピック放送：ギネス型：木の実育ての名人	収穫時、木のHPが10だったとき
//--------------------------------------------------------------------
static int TVTOPIC_Make_Record_NutsMaster(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	STRBUF * strbuf;
	COMMON_TAKE_NUTS_WORK * tnw = TVBCD_GetTopicData(bcd);
	WORDSET_RegisterOwnerName(ws, 0, bcd);
	strbuf = NutsNameGetAlloc(tnw->nuts_item_no - NUTS_START_ITEMNUM, HEAPID_FIELD);
	WORDSET_RegisterWord(ws, 1, strbuf, PM_MALE, TRUE, PM_LANG);
	STRBUF_Delete(strbuf);
	return msg_tv_03_06_01;
}


//============================================================================================
//	０２－２１	カスタムカプセル
//============================================================================================
//--------------------------------------------------------------------
/**
 * @brief	トピック生成：カスタムボール作成！	カスタムカプセルをポケモンにセットしたとき
 * @param	tvwk		テレビセーブデータへのポインタ
 * @param	pp			セットしたポケモンへのポインタ
 * @param	seal_no		シールの種類（どれか一つ、適当でよい）
 */
//--------------------------------------------------------------------
void TVTOPIC_Entry_Watch_CustomBall(TV_WORK * tvwk, POKEMON_PARAM * pp, u8 seal_no)
{
	TOPIC_DATA TopicData;
	WATCH_CUSTOMCAPSULE_WORK * wccw = &TopicData.WatchCustomCapsuleWork;
	wccw->seal_no = seal_no;
	wccw->rand = gf_mtRand() % 3;
	SetMonsParams(pp, &wccw->monsno, &wccw->mons_sex, &wccw->mons_region, &wccw->mons_rom);
	//MakeTopic(fsys, TVTOPICTYPE_WATCH, TVTOPIC_WATCH_SET_CUSTOMBALL, wccw);
	TVWORK_MakeTopic(tvwk, TVTOPICTYPE_WATCH, TVTOPIC_WATCH_SET_CUSTOMBALL, (const u8 *)wccw);
}
//--------------------------------------------------------------------
///	トピック放送：カスタムボール作成！	カスタムカプセルをポケモンにセットしたとき
//--------------------------------------------------------------------
static int TVTOPIC_Make_Watch_CustomBall(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	WATCH_CUSTOMCAPSULE_WORK * wccw = (WATCH_CUSTOMCAPSULE_WORK *)TVBCD_GetTopicData(bcd);
	WORDSET_RegisterOwnerName(ws, 0, bcd);
	WORDSET_RegisterSealName(ws, 1, wccw->seal_no);
	WORDSET_RegisterMonsName(ws, 2, wccw->monsno,
			wccw->mons_sex, wccw->mons_region, wccw->mons_rom);
	return msg_tv_02_21_01 + gf_p_rand(3);
}
//--------------------------------------------------------------------
///	トピック制限チェックカスタムボール作成！	カスタムカプセルをポケモンにセットしたとき：
//--------------------------------------------------------------------
static BOOL TVTOPIC_Limit_Watch_CustomBall(FIELDSYS_WORK * fsys, TVBCD * bcd)
{
	WATCH_CUSTOMCAPSULE_WORK * wccw = (WATCH_CUSTOMCAPSULE_WORK *)TVBCD_GetTopicData(bcd);
	return ZukanWork_GetPokeSeeFlag(SaveData_GetZukanWork(fsys->savedata),wccw->monsno);
}


//============================================================================================
//	０２－２２	旗をとった！
//	０２－２３	旗を取られた！
//============================================================================================
//--------------------------------------------------------------------
//--------------------------------------------------------------------
static void Entry_Watch_FlagCommon(WATCH_UG_FLAG_WORK * wufw, const MYSTATUS * rival)
{
	wufw->size = MyStatus_GetWorkSize();
	MyStatus_Copy(rival, (MYSTATUS *)wufw->data);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
static void Make_Watch_FlagCommon(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	WATCH_UG_FLAG_WORK * wufw = (WATCH_UG_FLAG_WORK *)TVBCD_GetTopicData(bcd);
	MYSTATUS * rival = (MYSTATUS *)&wufw->data;

	GF_ASSERT(MyStatus_GetWorkSize() == wufw->size);
	WORDSET_RegisterOwnerName(ws, 0, bcd);
	WORDSET_RegisterPlayerName(ws, 1, rival);
}
//--------------------------------------------------------------------
/**
 * @brief	トピック生成：旗を取った！		人の旗を持ち帰った！
 * @param	fsys	フィールド制御ワークへのポインタ
 * @param	rival		相手のMYSTATUSへのポインタ
 */
//--------------------------------------------------------------------
void TVTOPIC_Entry_Watch_GetFlag(FIELDSYS_WORK * fsys, const MYSTATUS * rival)
{
	TOPIC_DATA TopicData;
	WATCH_UG_FLAG_WORK * wufw = &TopicData.WatchUgFlagWork;

	Entry_Watch_FlagCommon(wufw, rival);
	MakeTopic(fsys, TVTOPICTYPE_WATCH, TVTOPIC_WATCH_GET_FLAG, wufw);
}

//--------------------------------------------------------------------
/**
 * @brief	トピック生成：旗を取られた！		自分の旗を持ち帰られた！
 * @param	fsys	フィールド制御ワークへのポインタ
 * @param	rival		相手のMYSTATUSへのポインタ
 */
//--------------------------------------------------------------------
void TVTOPIC_Entry_Watch_LoseFlag(FIELDSYS_WORK * fsys, const MYSTATUS * rival)
{
	TOPIC_DATA TopicData;
	WATCH_UG_FLAG_WORK * wufw = &TopicData.WatchUgFlagWork;

	Entry_Watch_FlagCommon(wufw, rival);
	MakeTopic(fsys, TVTOPICTYPE_WATCH, TVTOPIC_WATCH_LOSE_FLAG, wufw);
}

//--------------------------------------------------------------------
///トピック放送：旗を取った！		人の旗を持ち帰った！
//--------------------------------------------------------------------
static int TVTOPIC_Make_Watch_GetFlag(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	Make_Watch_FlagCommon(fsys, ws, bcd);
	return msg_tv_02_22_01;
}

//--------------------------------------------------------------------
///トピック放送：旗を取られた！		自分の旗を持ち帰られた！
//--------------------------------------------------------------------
static int TVTOPIC_Make_Watch_LoseFlag(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	Make_Watch_FlagCommon(fsys, ws, bcd);
	return msg_tv_02_23_01;
}

//--------------------------------------------------------------------
///	トピック制限チェック：旗を取られた！		自分の旗を持ち帰られた！
///	トピック制限チェック：旗を取った！		人の旗を持ち帰った！
//--------------------------------------------------------------------
static BOOL TVTOPIC_Limit_Watch_Tankou(FIELDSYS_WORK * fsys, TVBCD * bcd)
{
	return MyItem_CheckItem(SaveData_GetMyItem(fsys->savedata), ITEM_TANKENSETTO, 1, HEAPID_EVENT);
}


//============================================================================================
//	０２－２５	別荘購入		別荘を造った時★
//============================================================================================
//--------------------------------------------------------------------
/**
 * @brief	トピック生成：別荘購入
 * @param	fsys	フィールド制御ワークへのポインタ
 */
//--------------------------------------------------------------------
void TVTOPIC_Entry_Watch_Villa(FIELDSYS_WORK * fsys)
{
	TOPIC_DATA TopicData;
	WATCH_VILLA_WORK * wvw = &TopicData.WatchVillaWork;

	wvw->buy = 1;

	MakeTopic(fsys, TVTOPICTYPE_WATCH, TVTOPIC_WATCH_VILLA, wvw);
}

//--------------------------------------------------------------------
/**
 * @brief	トピック放送：別荘購入
 */
//--------------------------------------------------------------------
static int TVTOPIC_Make_Watch_Villa(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	WORDSET_RegisterOwnerName(ws, 0, bcd);
	return msg_tv_02_25_01;
}

//--------------------------------------------------------------------
///	トピック制限	別荘購入
//--------------------------------------------------------------------
static BOOL TVTOPIC_Limit_Watch_Villa(FIELDSYS_WORK * fsys, TVBCD * bcd)
{
#if 0
	const ZUKAN_WORK* zw = SaveData_GetZukanWork( fsys->savedata );

	if( ZukanWork_CheckShinouComp(zw) == TRUE ){
		return TRUE;
	}

	return FALSE;
#else
	return SysFlag_ArriveGet(SaveData_GetEventWork(fsys->savedata), FLAG_ARRIVE_T07);
#endif
}


//============================================================================================
//	０２－２６	別荘改築		別荘で何かを購入した時★
//============================================================================================
//--------------------------------------------------------------------
/**
 * @brief	トピック生成：別荘改築	家具を購入した時
 * @param	fsys	フィールド制御ワークへのポインタ
 * @param	no		購入した家具
 */
//--------------------------------------------------------------------
void TVTOPIC_Entry_Watch_Reconstruction(FIELDSYS_WORK * fsys, u8 no)
{
	TOPIC_DATA TopicData;
	WATCH_RECONSTRUCTION_WORK * wrw = &TopicData.WatchReconstructionWork;

	wrw->furniture = no;			//購入した家具

	MakeTopic(fsys, TVTOPICTYPE_WATCH, TVTOPIC_WATCH_RECONSTRUCTION, wrw);
}

//--------------------------------------------------------------------
/**
 * @brief	トピック放送：別荘改築
 */
//--------------------------------------------------------------------
static int TVTOPIC_Make_Watch_Reconstruction(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	WATCH_RECONSTRUCTION_WORK * wrw = (WATCH_RECONSTRUCTION_WORK *)TVBCD_GetTopicData(bcd);
	WORDSET_RegisterOwnerName(ws, 0, bcd);
	WORDSET_RegisterFurniture( ws, 1, wrw->furniture );
	return msg_tv_02_26_01;
}

//--------------------------------------------------------------------
///	トピック制限	別荘改築
//--------------------------------------------------------------------
static BOOL TVTOPIC_Limit_Watch_Reconstruction(FIELDSYS_WORK * fsys, TVBCD * bcd)
{
#if 0
	const ZUKAN_WORK* zw = SaveData_GetZukanWork( fsys->savedata );

	if( ZukanWork_CheckShinouComp(zw) == TRUE ){
		return TRUE;
	}

	return FALSE;
#else
	return SysFlag_ArriveGet(SaveData_GetEventWork(fsys->savedata), FLAG_ARRIVE_T07);
#endif
}


//============================================================================================
//
//
//			ギネス型トピック
//
//
//============================================================================================

//============================================================================================
//	０３－０１：バトルタワー連勝記録更新	バトルタワー連勝記録を更新したとき
//============================================================================================
//--------------------------------------------------------------------
/**
 * @brief	トピック生成：バトルタワー連勝(引数をfsys→savedataに変更)
 * @param	fsys		フィールド制御ワークへのポインタ
 * @param	renshou		連勝数
 * @param	pp			先頭のポケモンへのポインタ
 * @param	IsSingleFlag	シングル戦かどうか
 */
//--------------------------------------------------------------------
void TVTOPIC_Entry_Record_BTowerEx(SAVEDATA * savedata, 
		u32 renshou, POKEMON_PARAM * pp, BOOL IsSingleFlag)
{
	TOPIC_DATA TopicData;
	RECORD_BTOWER_WORK * rbtw = &TopicData.RecordBTowerWork;
	SetMonsParams(pp, &rbtw->monsno, &rbtw->mons_sex, &rbtw->mons_region, &rbtw->mons_rom);
	rbtw->renshou = renshou;
	rbtw->IsSingleFlag = IsSingleFlag;
	MakeTopicEx(savedata, TVTOPICTYPE_RECORD, TVTOPIC_RECORD_BTOWER, rbtw);
}

//--------------------------------------------------------------------
/**
 * @brief	トピック生成：バトルタワー連勝
 * @param	fsys		フィールド制御ワークへのポインタ
 * @param	renshou		連勝数
 * @param	pp			先頭のポケモンへのポインタ
 * @param	IsSingleFlag	シングル戦かどうか
 */
//--------------------------------------------------------------------
void TVTOPIC_Entry_Record_BTower(FIELDSYS_WORK * fsys, 
		u32 renshou, POKEMON_PARAM * pp, BOOL IsSingleFlag)
{
	TOPIC_DATA TopicData;
	RECORD_BTOWER_WORK * rbtw = &TopicData.RecordBTowerWork;
	SetMonsParams(pp, &rbtw->monsno, &rbtw->mons_sex, &rbtw->mons_region, &rbtw->mons_rom);
	rbtw->renshou = renshou;
	rbtw->IsSingleFlag = IsSingleFlag;
	MakeTopic(fsys, TVTOPICTYPE_RECORD, TVTOPIC_RECORD_BTOWER, rbtw);
}

//--------------------------------------------------------------------
///	トピック放送：バトルタワー連勝
//--------------------------------------------------------------------
static int TVTOPIC_Make_Record_BTower(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	RECORD_BTOWER_WORK * rbtw = (RECORD_BTOWER_WORK *)TVBCD_GetTopicData(bcd);

	WORDSET_RegisterOwnerName(ws, 0, bcd);
	WORDSET_RegisterMonsName(ws, 1, rbtw->monsno,
			rbtw->mons_sex, rbtw->mons_region, rbtw->mons_rom);
	WORDSET_RegisterNumber(ws, 2, rbtw->renshou, 4,
			NUMBER_DISPTYPE_LEFT, NUMBER_CODETYPE_DEFAULT);
	if (rbtw->IsSingleFlag) {
		return msg_tv_03_01_01;
	} else {
		return msg_tv_03_01_02;
	}
}
//--------------------------------------------------------------------
///	トピック制限チェック：バトルタワー連勝
//--------------------------------------------------------------------
static BOOL TVTOPIC_Limit_Record_BTower(FIELDSYS_WORK * fsys, TVBCD * bcd)
{
	RECORD_BTOWER_WORK * rbtw = (RECORD_BTOWER_WORK *)TVBCD_GetTopicData(bcd);
	if (ZukanWork_GetPokeSeeFlag(SaveData_GetZukanWork(fsys->savedata), rbtw->monsno) == FALSE) {
		return FALSE;
	}
	return SysFlag_ArriveGet(SaveData_GetEventWork(fsys->savedata), FLAG_ARRIVE_C11);
}


//============================================================================================
//	０３－０３：大きさ測定記録更新	大きさ測定で記録を更新したとき
//============================================================================================
//--------------------------------------------------------------------
/**
 * @brief	トピック生成：大きさ測定記録更新	大きさ測定で記録を更新したとき
 * @param	fsys	フィールド制御ワークへのポインタ
 * @param	size		記録
 * @param	pp			更新したポケモンへのポインタ
 */
//--------------------------------------------------------------------
void TVTOPIC_Entry_Record_Ookisa(FIELDSYS_WORK * fsys, u32 size, POKEMON_PARAM * pp)
{
	TOPIC_DATA TopicData;
	RECORD_OOKISA_WORK * row = &TopicData.RecordOokisaWork;
	SetMonsParams(pp, &row->monsno, &row->mons_sex, &row->mons_region, &row->mons_rom);
	row->size = size;
	MakeTopic(fsys, TVTOPICTYPE_RECORD, TVTOPIC_RECORD_OOKISA, row);
}

//--------------------------------------------------------------------
///	トピック放送：大きさ測定記録更新	大きさ測定で記録を更新したとき
//--------------------------------------------------------------------
static int TVTOPIC_Make_Record_Ookisa(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	RECORD_OOKISA_WORK * row = (RECORD_OOKISA_WORK *)TVBCD_GetTopicData(bcd);

	WORDSET_RegisterOwnerName(ws, 0, bcd);
	WORDSET_RegisterMonsName(ws, 1, row->monsno,
			row->mons_sex, row->mons_region, row->mons_rom);
	// ----------------------------------------------------------------------------
	// localize_spec_mark(LANG_ENGLISH) imatake 2006/12/28
	// 英語版のみポケモンの大きさをインチ系に変換
#if (PM_LANG == LANG_ENGLISH)
	{
		u32 size_inch = PG5_CM_TO_INCH(row->size);
		WORDSET_RegisterNumber(ws, 2, size_inch / 10, 3,
				NUMBER_DISPTYPE_LEFT, NUMBER_CODETYPE_DEFAULT);
		WORDSET_RegisterNumber(ws, 3, size_inch % 10, 1,
				NUMBER_DISPTYPE_LEFT, NUMBER_CODETYPE_DEFAULT);
	}
#else
	WORDSET_RegisterNumber(ws, 2, row->size / 10, 3,
			NUMBER_DISPTYPE_LEFT, NUMBER_CODETYPE_DEFAULT);
	WORDSET_RegisterNumber(ws, 3, row->size % 10, 1,
			NUMBER_DISPTYPE_LEFT, NUMBER_CODETYPE_DEFAULT);
#endif
	// ----------------------------------------------------------------------------

	return msg_tv_03_03_01;
}
//--------------------------------------------------------------------
///	トピック制限チェック：バトルタワー連勝
//--------------------------------------------------------------------
static BOOL TVTOPIC_Limit_Record_Ookisa(FIELDSYS_WORK * fsys, TVBCD * bcd)
{
	RECORD_OOKISA_WORK * row = (RECORD_OOKISA_WORK *)TVBCD_GetTopicData(bcd);
	return ZukanWork_GetPokeSeeFlag(SaveData_GetZukanWork(fsys->savedata), row->monsno);
}


//============================================================================================
//	０３－０４：スロット達人	スロットで一定枚数以上を稼いだとき
//============================================================================================
//--------------------------------------------------------------------
/**
 * @brief	トピック生成：スロット達人	スロットで一定枚数以上を稼いだとき
 * @param	fsys	フィールド制御ワークへのポインタ
 * @param	start_coin		開始時のコイン枚数
 * @param	end_coin		終了時のコイン枚数
 * @param	playtime		プレイ時間（分単位）
 */
//--------------------------------------------------------------------
void TVTOPIC_Entry_Record_Slot(FIELDSYS_WORK * fsys, u32 start_coin, u32 end_coin, u32 playtime)
{
	TOPIC_DATA TopicData;
	RECORD_SLOT_WORK * rsw = &TopicData.RecordSlotWork;
	if (end_coin < 1000 + start_coin) return;
	rsw->start_coin = start_coin;
	rsw->end_coin = end_coin;
	rsw->playtime = playtime;
	MakeTopic(fsys, TVTOPICTYPE_RECORD, TVTOPIC_RECORD_SLOT, rsw);
}
//--------------------------------------------------------------------
///	トピック放送：スロット記録
//--------------------------------------------------------------------
static int TVTOPIC_Make_Record_Slot(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	RECORD_SLOT_WORK * rsw = TVBCD_GetTopicData(bcd);

	WORDSET_RegisterOwnerName(ws, 0, bcd);
	WORDSET_RegisterNumber(ws, 1, rsw->playtime, 10,
			NUMBER_DISPTYPE_LEFT, NUMBER_CODETYPE_DEFAULT);
	WORDSET_RegisterNumber(ws, 2, rsw->start_coin, 6,
			NUMBER_DISPTYPE_LEFT, NUMBER_CODETYPE_DEFAULT);
	WORDSET_RegisterNumber(ws, 3, rsw->end_coin, 6,
			NUMBER_DISPTYPE_LEFT, NUMBER_CODETYPE_DEFAULT);

	return msg_tv_03_04_01;
}


//============================================================================================
//	０３－０５	リボンコレクター★
//============================================================================================
static u8  get_ribbon_num( POKEMON_PARAM* pp );
//--------------------------------------------------------------------
/**
 * @brief	トピック生成：リボンコレクター
 * @param	fsys		フィールド制御ワークへのポインタ
 * @param	savedata	SAVEDATAへのポインタ(★)
 * @param	pp			リボンをゲットしたポケモン
 * @param	ribbon_id	リボンのID
 */
//--------------------------------------------------------------------
//void TVTOPIC_Entry_Record_Ribbon(FIELDSYS_WORK * fsys, POKEMON_PARAM* pp, u32 ribbon_id)
void TVTOPIC_Entry_Record_Ribbon(SAVEDATA * savedata, POKEMON_PARAM* pp, u32 ribbon_id)
{
	u8 count,total;
	TOPIC_DATA TopicData;
	RECORD_RIBBON_WORK * rrw = &TopicData.RecordRibbonWork;

	total = get_ribbon_num( pp );
	OS_Printf( "ribbon total = %d\n", total );

	//15,20,25,30,35,40個目だった時のみ
	switch( total ){
	case 15:
	case 20:
	case 25:
	case 30:
	case 35:
	case 40:
		if( RIBBON_NameGet(ribbon_id) > 0xff ){
			OS_Printf( "リボンがu8で足りません\n" );
			GF_ASSERT(0);
			return;			//作成しない
		}

		SetMonsParams(pp, &rrw->monsno, &rrw->mons_sex, &rrw->mons_region, &rrw->mons_rom);
		StoreNickName(HEAPID_EVENT, pp, &rrw->nickname_flag, rrw->nickname);

		//引数として渡されるのは、ID_PARA_リボン、なので、メッセージIDを取得
		//rrw->ribbon_id	= ribbon_id;
		rrw->ribbon_id		= RIBBON_NameGet( ribbon_id );	//mes_ribbon_name_079なのでu8で収まる
		rrw->ribbon_total	= total;

		//MakeTopic(fsys, TVTOPICTYPE_RECORD, TVTOPIC_RECORD_RIBBON, rrw);
		MakeTopic2(savedata, TVTOPICTYPE_RECORD, TVTOPIC_RECORD_RIBBON, rrw);
		break;
	};
}

static const u16 ribbon_tbl[] = {
	ID_PARA_ganba_ribbon,						//がんばリボン
	ID_PARA_sinou_champ_ribbon,					//シンオウチャンプリボン

	ID_PARA_trial_stylemedal_normal,			//かっこよさ勲章(ノーマル)トライアル
	ID_PARA_trial_stylemedal_super,				//かっこよさ勲章(スーパー)トライアル
	ID_PARA_trial_stylemedal_hyper,				//かっこよさ勲章(ハイパー)トライアル
	ID_PARA_trial_stylemedal_master,			//かっこよさ勲章(マスター)トライアル
	ID_PARA_trial_beautifulmedal_normal,		//うつくしさ勲章(ノーマル)トライアル
	ID_PARA_trial_beautifulmedal_super,			//うつくしさ勲章(スーパー)トライアル
	ID_PARA_trial_beautifulmedal_hyper,			//うつくしさ勲章(ハイパー)トライアル
	ID_PARA_trial_beautifulmedal_master,		//うつくしさ勲章(マスター)トライアル
	ID_PARA_trial_cutemedal_normal,				//かわいさ勲章(ノーマル)トライアル
	ID_PARA_trial_cutemedal_super,				//かわいさ勲章(スーパー)トライアル
	ID_PARA_trial_cutemedal_hyper,				//かわいさ勲章(ハイパー)トライアル
	ID_PARA_trial_cutemedal_master,				//かわいさ勲章(マスター)トライアル
	ID_PARA_trial_clevermedal_normal,			//かしこさ勲章(ノーマル)トライアル
	ID_PARA_trial_clevermedal_super,			//かしこさ勲章(スーパー)トライアル
	ID_PARA_trial_clevermedal_hyper,			//かしこさ勲章(ハイパー)トライアル
	ID_PARA_trial_clevermedal_master,			//かしこさ勲章(マスター)トライアル
	ID_PARA_trial_strongmedal_normal,			//たくましさ勲章(ノーマル)トライアル
	ID_PARA_trial_strongmedal_super,			//たくましさ勲章(スーパー)トライアル
	ID_PARA_trial_strongmedal_hyper,			//たくましさ勲章(ハイパー)トライアル
	ID_PARA_trial_strongmedal_master,			//たくましさ勲章(マスター)トライアル

	ID_PARA_sinou_battle_tower_ttwin_first,		//シンオウバトルタワータワータイクーン勝利1回目
	ID_PARA_sinou_battle_tower_ttwin_second,	//シンオウバトルタワータワータイクーン勝利2回目
	ID_PARA_sinou_battle_tower_2vs2_win50,		//シンオウバトルタワータワーダブル50連勝
	ID_PARA_sinou_battle_tower_aimulti_win50,	//シンオウバトルタワータワーAIマルチ50連勝
	ID_PARA_sinou_battle_tower_siomulti_win50,	//シンオウバトルタワータワー通信マルチ50連勝
	ID_PARA_sinou_battle_tower_wifi_rank5,		//シンオウバトルタワーWifiランク５入り

	ID_PARA_sinou_syakki_ribbon,				//シンオウしゃっきリボン
	ID_PARA_sinou_dokki_ribbon,					//シンオウどっきリボン
	ID_PARA_sinou_syonbo_ribbon,				//シンオウしょんぼリボン
	ID_PARA_sinou_ukka_ribbon,					//シンオウうっかリボン
	ID_PARA_sinou_sukki_ribbon,					//シンオウすっきリボン
	ID_PARA_sinou_gussu_ribbon,					//シンオウぐっすリボン
	ID_PARA_sinou_nikko_ribbon,					//シンオウにっこリボン
	ID_PARA_sinou_gorgeous_ribbon,				//シンオウゴージャスリボン
	ID_PARA_sinou_royal_ribbon,					//シンオウロイヤルリボン
	ID_PARA_sinou_gorgeousroyal_ribbon,			//シンオウゴージャスロイヤルリボン
	ID_PARA_sinou_ashiato_ribbon,				//シンオウあしあとリボン
};
#define TV_RIBBON_MAX	( NELEMS(ribbon_tbl) )

//リボン総数をカウント
static u8  get_ribbon_num( POKEMON_PARAM* pp )
{
	u8 count,i;
	count = 0;

	for( i=0; i < TV_RIBBON_MAX ;i++ ){
		if( PokeParaGet(pp,ribbon_tbl[i],NULL) == 1 ){
			count++;
		}
	}

	return count;
}

//--------------------------------------------------------------------
///	トピック放送：リボンコレクター
//--------------------------------------------------------------------
static int TVTOPIC_Make_Record_Ribbon(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	RECORD_RIBBON_WORK * rrw = TVBCD_GetTopicData(bcd);

	WORDSET_RegisterOwnerName(ws, 0, bcd);

	if (rrw->nickname_flag) {
		//ニックネームをつけた場合
		WORDSET_RegisterSTRCODE(ws, 1, rrw->nickname,
				rrw->mons_sex, rrw->mons_region, TRUE);
	} else {
		//ニックネームをつけなかった場合
		WORDSET_RegisterMonsName(ws, 1, rrw->monsno,
				rrw->mons_sex, rrw->mons_region, rrw->mons_rom);
	}

	WORDSET_RegisterRibbonName( ws, 2, rrw->ribbon_id );
	WORDSET_RegisterNumber(ws, 3, rrw->ribbon_total, 3,			//★
			NUMBER_DISPTYPE_LEFT, NUMBER_CODETYPE_DEFAULT);

	return msg_tv_03_05_01;
}


//============================================================================================
//	０３－０８	ワナ解除名人	地下に入ったとき、たくさんのワナを解除したら作成
//============================================================================================
//--------------------------------------------------------------------
/**
 * @brief	トピック生成：ワナ解除名人	地下に入ったとき、たくさんのワナを解除したら作成
 * @param	fsys	フィールド制御ワークへのポインタ
 * @param	trap_no			最初に解除したワナのナンバー
 * @param	remove_count	ワナを解除した回数
 *
 * 地下に入ってから出るまでの間の回数です。一度出たらリセットされます。
 */
//--------------------------------------------------------------------
void TVTOPIC_Entry_Record_RemoveTrap(FIELDSYS_WORK * fsys, u16 trap_no, u16 remove_count)
{
	TOPIC_DATA TopicData;
	RECORD_REMOVE_TRAP_WORK * rrtw = &TopicData.RecordRemoveTrapWork;
	rrtw->trap_no = trap_no;

	rrtw->remove_count = remove_count;
	if( rrtw->remove_count > 999 ){
		rrtw->remove_count = 999;			//放送が3桁なので
	}

	MakeTopic(fsys, TVTOPICTYPE_RECORD, TVTOPIC_RECORD_TRAP_REMOVER, rrtw);
}
//--------------------------------------------------------------------
///	トピック放送：ワナ解除名人	地下に入ったとき、たくさんのワナを解除したら作成
//--------------------------------------------------------------------
static int TVTOPIC_Make_Record_RemoveTrap(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	u16 remove_count;
	RECORD_REMOVE_TRAP_WORK * rrtw = TVBCD_GetTopicData(bcd);
	WORDSET_RegisterOwnerName(ws, 0, bcd);
	WORDSET_RegisterUGTrapName(ws, 1, rrtw->trap_no);

	remove_count = rrtw->remove_count;
	if( remove_count > 999 ){
		remove_count = 999;			//放送が3桁なので
	}

	//WORDSET_RegisterNumber(ws, 2, rrtw->remove_count, 3,
	WORDSET_RegisterNumber(ws, 2, remove_count, 3,
			NUMBER_DISPTYPE_LEFT, NUMBER_CODETYPE_DEFAULT);
	return msg_tv_03_08_01;
}


//============================================================================================
//	０３－０９	ハタトリ名人
//============================================================================================
//--------------------------------------------------------------------
/**
 * @brief	トピック生成：ハタトリ名人
 * @param	fsys	フィールド制御ワークへのポインタ
 * @param	flag_count		旗を取った回数
 *
 * 地下に入ってから出るまでの間の回数です。一度出たらリセットされます。
 */
//--------------------------------------------------------------------
void TVTOPIC_Entry_Record_GetFlag(FIELDSYS_WORK * fsys, u16 flag_count)
{
	TOPIC_DATA TopicData;
	RECORD_GET_FLAG_WORK * rgfw = &TopicData.RecordGetFlagWork;

	rgfw->flag_count = flag_count;
	if( rgfw->flag_count > 999 ){
		rgfw->flag_count = 999;			//放送が3桁なので
	}

	if (flag_count > 1) {
		MakeTopic(fsys, TVTOPICTYPE_RECORD, TVTOPIC_RECORD_FLAG_GET, rgfw);
	}
}
//--------------------------------------------------------------------
///	トピック放送：ハタトリ名人
//--------------------------------------------------------------------
static int TVTOPIC_Make_Record_GetFlag(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	u16 flag_count;
	RECORD_GET_FLAG_WORK * rgfw = TVBCD_GetTopicData(bcd);
	WORDSET_RegisterOwnerName(ws, 0, bcd);
	
	flag_count = rgfw->flag_count;
	if( flag_count > 999 ){
		flag_count = 999;			//放送が3桁なので
	}

	//WORDSET_RegisterNumber(ws, 1, rgfw->flag_count, 3,
	WORDSET_RegisterNumber(ws, 1, flag_count, 3,
			NUMBER_DISPTYPE_LEFT, NUMBER_CODETYPE_DEFAULT);

	return msg_tv_03_09_01;
}
//--------------------------------------------------------------------
///	トピック制限チェック：ハタトリ名人
//--------------------------------------------------------------------
static BOOL TVTOPIC_Limit_Record_Tankou(FIELDSYS_WORK * fsys, TVBCD * bcd)
{
	return MyItem_CheckItem(SaveData_GetMyItem(fsys->savedata), ITEM_TANKENSETTO, 1, HEAPID_EVENT);
}


//============================================================================================
//	０３－１０	バトルポイント名人★
//============================================================================================
//--------------------------------------------------------------------
/**
 * @brief	トピック生成：バトルポイント名人
 * @param	savedata		セーブデータワークへのポインタ
 * @param	add_point		足すポイント
 */
//--------------------------------------------------------------------
void TVTOPIC_Entry_Record_BP(SAVEDATA* savedata)
{
	TOPIC_DATA TopicData;
	RECORD_BP_WORK * rbw = &TopicData.RecordBPWork;
	TOPICTEMP_BP * pbw = TVWORK_GetBPTemp(SaveData_GetTvWork(savedata));

	//30以上で作成
	if( pbw->bp_num >= 30 ){
		rbw->bp = *pbw;
		pbw->exist_flag = FALSE;
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_TVWORK)
		SVLD_SetCrc(GMDATA_ID_TVWORK);
#endif //CRC_LOADCHECK
		MakeTopic2(savedata, TVTOPICTYPE_RECORD, TVTOPIC_RECORD_BP, rbw);
	}

}
//--------------------------------------------------------------------
///	トピック放送：バトルポイント名人
//--------------------------------------------------------------------
static int TVTOPIC_Make_Record_BP(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	RECORD_BP_WORK * rbw = TVBCD_GetTopicData(bcd);
	WORDSET_RegisterOwnerName(ws, 0, bcd);
	WORDSET_RegisterNumber(ws, 1, rbw->bp.bp_num, 4,
			NUMBER_DISPTYPE_LEFT, NUMBER_CODETYPE_DEFAULT);
	return msg_tv_03_10_10;
}

//--------------------------------------------------------------------
///	トピック制限チェック：バトルポイント名人
//--------------------------------------------------------------------
static BOOL TVTOPIC_Limit_Record_BP(FIELDSYS_WORK * fsys, TVBCD * bcd)
{
	return SysFlag_ArriveGet(SaveData_GetEventWork(fsys->savedata), FLAG_ARRIVE_C11);
}


//============================================================================================
//	０３－１１	GDS名人★
//============================================================================================
//--------------------------------------------------------------------
/**
 * @brief	トピック生成：GTS名人
 * @param	savedata	セーブデータワークへのポインタ
 */
//--------------------------------------------------------------------
void TVTOPIC_Entry_Record_GTS(SAVEDATA * savedata)
{
	TOPIC_DATA TopicData;
	RECORD_GTS_WORK * rgw = &TopicData.RecordGTSWork;
	TOPICTEMP_GTS * pgw = TVWORK_GetGTSTemp(SaveData_GetTvWork(savedata));

	//10回以上で生成
	if( pgw->trade_num >= 10 ){
		rgw->gts = *pgw;
		pgw->exist_flag = FALSE;
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_TVWORK)
		SVLD_SetCrc(GMDATA_ID_TVWORK);
#endif //CRC_LOADCHECK
		MakeTopic2(savedata, TVTOPICTYPE_RECORD, TVTOPIC_RECORD_GTS, rgw);
	}
}
//--------------------------------------------------------------------
///	トピック放送：GTS名人
//--------------------------------------------------------------------
static int TVTOPIC_Make_Record_GTS(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	RECORD_GTS_WORK * rgw = TVBCD_GetTopicData(bcd);
	WORDSET_RegisterOwnerName(ws, 0, bcd);
	WORDSET_RegisterNumber(ws, 1, rgw->gts.trade_num, 4,
			NUMBER_DISPTYPE_LEFT, NUMBER_CODETYPE_DEFAULT);
	return msg_tv_03_11_01;
}

//--------------------------------------------------------------------
///	トピック制限チェック：GTS名人
//--------------------------------------------------------------------
static BOOL TVTOPIC_Limit_Record_GTS(FIELDSYS_WORK * fsys, TVBCD * bcd)
{
	return SysFlag_ArriveGet(SaveData_GetEventWork(fsys->savedata), FLAG_ARRIVE_C03);
}


//============================================================================================
//
//
//				質問型トピック
//
//
//============================================================================================

//============================================================================================
//	０１－０２	挑戦！バトルタワー	1	バトルタワー	挑戦終了後
//============================================================================================
//--------------------------------------------------------------------
/**
 * @brief	トピック生成：バトルタワーインタビュー
 * @param	fsys	フィールド制御ワークへのポインタ
 * @param	count		連勝回数
 * @param	word		簡易会話単語ID
 */
//--------------------------------------------------------------------
void TVTOPIC_Entry_QandA_BTower(FIELDSYS_WORK * fsys, PMS_WORD word)
{
	TOPIC_DATA TopicData;
	QANDA_BTOWER_WORK * qbtw = &TopicData.QandABtowerWork;
	TOPICTEMP_BTOWER * pbt = TVWORK_GetBTowerTemp(SaveData_GetTvWork(fsys->savedata));
	qbtw->btower = *pbt;
	pbt->exist_flag = FALSE;
	qbtw->word = word;
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_TVWORK)
	SVLD_SetCrc(GMDATA_ID_TVWORK);
#endif //CRC_LOADCHECK
	MakeTopic(fsys, TVTOPICTYPE_INTERVIEW, TVTOPIC_INTERVIEW_BTOWER, qbtw);
}

//--------------------------------------------------------------------
///	トピック放送：バトルタワーインタビュー
//--------------------------------------------------------------------
static int TVTOPIC_Make_QandA_BTower(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	QANDA_BTOWER_WORK* qbtw = TVBCD_GetTopicData(bcd);
	WORDSET_RegisterNumber(ws, 0, qbtw->btower.win_count, 4,
			NUMBER_DISPTYPE_LEFT, NUMBER_CODETYPE_DEFAULT);
	WORDSET_RegisterOwnerName(ws, 1, bcd);		//トレーナー名
	WORDSET_RegisterPMSWord(ws, 2, qbtw->word);
	if (qbtw->btower.continue_flag == TRUE) {
		return msg_tv_01_02_01;
	} else {
		return msg_tv_01_02_02;
	}
}
//--------------------------------------------------------------------
///	トピック制限チェック：バトルタワー連勝
//--------------------------------------------------------------------
static BOOL TVTOPIC_Limit_QandA_BTower(FIELDSYS_WORK * fsys, TVBCD * bcd)
{
	return SysFlag_ArriveGet(SaveData_GetEventWork(fsys->savedata), FLAG_ARRIVE_C11);
}


//============================================================================================
//	０１－０４	あなたのポケモン	1	ポケモン大好きクラブ	
//============================================================================================
//--------------------------------------------------------------------
/**
 * @brief	トピック生成：あなたのポケモンインタビュー
 * @param	fsys	フィールド制御ワークへのポインタ
 * @param	word		簡易会話単語ID
 */
//--------------------------------------------------------------------
void TVTOPIC_Entry_QandA_Pokemon(FIELDSYS_WORK * fsys, PMS_WORD word)
{
	TOPIC_DATA TopicData;
	QANDA_POKEMON_WORK * qpw = &TopicData.QandAPokemonWork;
	POKEMON_PARAM * pp;
	
	//先頭のポケモン
	pp = EvPoke_GetPokemonTop(SaveData_GetTemotiPokemon(fsys->savedata));
	SetMonsParams(pp, &qpw->monsno, &qpw->mons_sex, &qpw->mons_region, &qpw->mons_rom);
	StoreNickName(HEAPID_EVENT, pp, &qpw->nickname_flag, qpw->nickname);
	qpw->word = word;
	MakeTopic(fsys, TVTOPICTYPE_INTERVIEW, TVTOPIC_INTERVIEW_POKEMON, qpw);
}

//--------------------------------------------------------------------
///	トピック放送：あなたのポケモンインタビュー
//--------------------------------------------------------------------
static int TVTOPIC_Make_QandA_Pokemon(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	QANDA_POKEMON_WORK * qpw = TVBCD_GetTopicData(bcd);

	if (qpw->nickname_flag) {
		//ニックネームをつけた場合
		WORDSET_RegisterOwnerName(ws, 0, bcd);		//トレーナー名
		WORDSET_RegisterMonsName(ws, 1, qpw->monsno,
				qpw->mons_sex, qpw->mons_region, qpw->mons_rom);
		WORDSET_RegisterSTRCODE(ws, 2, qpw->nickname,
				qpw->mons_sex, qpw->mons_region, TRUE);
		WORDSET_RegisterPMSWord(ws, 3, qpw->word);
		return msg_tv_01_04_01;
	} else {
		//ニックネームをつけなかった場合
		WORDSET_RegisterOwnerName(ws, 0, bcd);		//トレーナー名
		WORDSET_RegisterMonsName(ws, 1, qpw->monsno,
				qpw->mons_sex, qpw->mons_region, qpw->mons_rom);
	//	WORDSET_RegisterSTRCODE(ws, 2, qpw->nickname,
	//			qpw->mons_sex, qpw->mons_region, TRUE);
		WORDSET_RegisterPMSWord(ws, 3, qpw->word);
		return msg_tv_01_04_02;
	}
}
//--------------------------------------------------------------------
///	トピック制限チェック：バトルタワー連勝
//--------------------------------------------------------------------
static BOOL TVTOPIC_Limit_QandA_Pokemon(FIELDSYS_WORK * fsys, TVBCD * bcd)
{
	QANDA_POKEMON_WORK * qpw = TVBCD_GetTopicData(bcd);
	return ZukanWork_GetPokeSeeFlag(SaveData_GetZukanWork(fsys->savedata), qpw->monsno);
}


//============================================================================================
//	０１－０６	お気に入りのポケッチ	2	カンパニー	
//============================================================================================
//--------------------------------------------------------------------
/**
 * @brief	トピック生成：お気に入りのポケッチ
 * @param	fsys	フィールド制御ワークへのポインタ
 * @param	word		簡易会話単語ID
 */
//--------------------------------------------------------------------
void TVTOPIC_Entry_QandA_Poketch(FIELDSYS_WORK * fsys, PMS_WORD word)
{
	TOPIC_DATA TopicData;
	QANDA_POKETCH_WORK * qptw = &TopicData.QandAPoketchWork;
	qptw->poketch_id = Poketch_GetRunningApplication(fsys->fldmap->poketch);
	qptw->word = word;
	MakeTopic(fsys, TVTOPICTYPE_INTERVIEW, TVTOPIC_INTERVIEW_POKETCH, qptw);
}
//--------------------------------------------------------------------
///	トピック放送：ポケッチインタビュー
//--------------------------------------------------------------------
static int TVTOPIC_Make_QandA_Poketch(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	QANDA_POKETCH_WORK * qptw = TVBCD_GetTopicData(bcd);
	WORDSET_RegisterOwnerName(ws, 0, bcd);		//トレーナー名
	WORDSET_RegisterPoketchAppName(ws, 1, qptw->poketch_id);
	WORDSET_RegisterPMSWord(ws, 2, qptw->word);
	return msg_tv_01_06_01;
}


//============================================================================================
//	０１－０７	挑戦！コンテスト		コンテスト会場	コンテスト終了後
//============================================================================================
//--------------------------------------------------------------------
/**
 * @brief	トピック生成：挑戦！コンテスト
 * @param	fsys	フィールド制御ワークへのポインタ
 * @param	word		簡易会話単語ID
 */
//--------------------------------------------------------------------
void TVTOPIC_Entry_QandA_Contest(FIELDSYS_WORK * fsys, PMS_WORD word)
{
	TOPIC_DATA TopicData;
	QANDA_CONTEST_WORK * qctw = &TopicData.QandAContestWork;
	TOPICTEMP_CONTEST * pcnt = TVWORK_GetContestTemp(SaveData_GetTvWork(fsys->savedata));
	qctw->contest = *pcnt;
	pcnt->exist_flag = FALSE;
	qctw->word = word;
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_TVWORK)
	SVLD_SetCrc(GMDATA_ID_TVWORK);
#endif //CRC_LOADCHECK
	MakeTopic(fsys, TVTOPICTYPE_INTERVIEW, TVTOPIC_INTERVIEW_CONTEST, qctw);
}
#include "../contest/con_tool.h"
//--------------------------------------------------------------------
//--------------------------------------------------------------------
static int TVTOPIC_Make_QandA_Contest(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	QANDA_CONTEST_WORK * qctw = TVBCD_GetTopicData(bcd);
	WORDSET_RegisterMonsName(ws, 0, qctw->contest.monsno,
			qctw->contest.mons_sex, qctw->contest.mons_region, qctw->contest.mons_rom);
	WORDSET_RegisterContestType(ws, 1, ConTool_IMCGetTypeMsgID(qctw->contest.contest_type));
	WORDSET_RegisterContestRank(ws, 2, ConTool_IMCGetRankMsgID(qctw->contest.contest_rank));
	WORDSET_RegisterNumber(ws, 3, qctw->contest.contest_result, 1,
			NUMBER_DISPTYPE_LEFT, NUMBER_CODETYPE_DEFAULT);
	WORDSET_RegisterOwnerName(ws, 4, bcd);		//トレーナー名
	WORDSET_RegisterPMSWord(ws, 5, qctw->word);
	if (qctw->contest.contest_result == 1) {
		return msg_tv_01_07_02;		//優勝用メッセージ
	} else {
		return msg_tv_01_07_01;
	}
}
//--------------------------------------------------------------------
///	トピック制限チェック：コンテスト
//--------------------------------------------------------------------
static BOOL TVTOPIC_Limit_QandA_Contest(FIELDSYS_WORK * fsys, TVBCD * bcd)
{
	QANDA_CONTEST_WORK * qctw = TVBCD_GetTopicData(bcd);
	return ZukanWork_GetPokeSeeFlag(SaveData_GetZukanWork(fsys->savedata), qctw->contest.monsno);
}


//============================================================================================
//	０１－０９	ポケモンクリップ！	2	クリップを作る場所	クリップを作ったあと
//============================================================================================
//--------------------------------------------------------------------
/**
 * @brief	トピック生成：ポケモンクリップ！
 * @param	fsys	フィールド制御ワークへのポインタ
 * @param	word		簡易会話単語ID
 */
//--------------------------------------------------------------------
void TVTOPIC_Entry_QandA_Clip(FIELDSYS_WORK * fsys, PMS_WORD word)
{
	TOPIC_DATA TopicData;
	QANDA_CLIP_WORK * qclw = &TopicData.QandAClipWork;
	qclw->word = word;
	{
		IMC_SAVEDATA* iw = SaveData_GetImcSaveData(fsys->savedata);
		IMC_TELEVISION_SAVEDATA * itv = ImcSaveData_GetTelevisionSaveData(iw, 0);
		qclw->monsno = ImcSaveData_GetTelevisionPokeMonsNo(itv);
	}
	MakeTopic(fsys, TVTOPICTYPE_INTERVIEW, TVTOPIC_INTERVIEW_CLIP, qclw);
}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
static int TVTOPIC_Make_QandA_Clip(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	QANDA_CLIP_WORK * qclw = TVBCD_GetTopicData(bcd);
	WORDSET_RegisterOwnerName(ws, 0, bcd);		//トレーナー名
	WORDSET_RegisterMonsNameEasy(ws, 1, qclw->monsno);
	WORDSET_RegisterPMSWord(ws, 2, qclw->word);
	return msg_tv_01_09_01;
}
//--------------------------------------------------------------------
///	トピック制限チェック：バトルタワー連勝
//--------------------------------------------------------------------
static BOOL TVTOPIC_Limit_QandA_Clip(FIELDSYS_WORK * fsys, TVBCD * bcd)
{
	QANDA_CLIP_WORK * qclw = TVBCD_GetTopicData(bcd);
	return ZukanWork_GetPokeSeeFlag(SaveData_GetZukanWork(fsys->savedata), qclw->monsno);
}


//============================================================================================
//	０１－１０	性格診断！	2		
//============================================================================================
//--------------------------------------------------------------------
/**
 * @brief	トピック生成：性格診断！
 * @param	fsys	フィールド制御ワークへのポインタ
 * @param	word		簡易会話単語ID
 */
//--------------------------------------------------------------------
void TVTOPIC_Entry_QandA_Character(FIELDSYS_WORK * fsys, PMS_WORD word)
{
	TOPIC_DATA TopicData;
	QANDA_POKETYPE_WORK * qptype = &TopicData.QandAPokeTypeWork;
	qptype->type = word;
	MakeTopic(fsys, TVTOPICTYPE_INTERVIEW, TVTOPIC_INTERVIEW_CHARACTOR, qptype);
}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
static int TVTOPIC_Make_QandA_Character(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	QANDA_POKETYPE_WORK * qptype = TVBCD_GetTopicData(bcd);
	WORDSET_RegisterOwnerName(ws, 0, bcd);		//トレーナー名
	return msg_tv_01_10_01 + qptype->type - 1;
}


//============================================================================================
//	０１－１１		ポルト料理		ポルトを作る場所	ポルトを作ったら
//============================================================================================
//--------------------------------------------------------------------
/**
 * @brief	トピック生成：ポルト料理
 * @param	fsys	フィールド制御ワークへのポインタ
 * @param	word		簡易会話単語ID
 */
//--------------------------------------------------------------------
void TVTOPIC_Entry_QandA_Poruto(FIELDSYS_WORK * fsys, PMS_WORD word)
{
	TOPIC_DATA TopicData;
	QANDA_PORUTO_WORK * qporuto = &TopicData.QandAPorutoWork;
	TOPICTEMP_PORUTO * poruto = TVWORK_GetPorutoTemp(SaveData_GetTvWork(fsys->savedata));
	qporuto->poruto = *poruto;
	qporuto->word = word;
	poruto->exist_flag = FALSE;
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_TVWORK)
	SVLD_SetCrc(GMDATA_ID_TVWORK);
#endif //CRC_LOADCHECK
	MakeTopic(fsys, TVTOPICTYPE_INTERVIEW, TVTOPIC_INTERVIEW_PORUTO, qporuto);
}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
static int TVTOPIC_Make_QandA_Poruto(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	QANDA_PORUTO_WORK * qporuto = TVBCD_GetTopicData(bcd);
	int taste = qporuto->poruto.taste_type;
	WORDSET_RegisterOwnerName(ws, 0, bcd);		//トレーナー名
	WORDSET_RegisterPorutoName(ws, 1, taste);
	WORDSET_RegisterPMSWord(ws, 2, qporuto->word);
	switch (taste) {
	case PORUTO_FLAVOR_HEAVY:
		return msg_tv_01_11_02;
	case PORUTO_FLAVOR_THICK:
		return msg_tv_01_11_03;
	case PORUTO_FLAVOR_NGOOD:
		return msg_tv_01_11_04;
	case PORUTO_FLAVOR_MILD:
		return msg_tv_01_11_05;
	default:
		return msg_tv_01_11_01;
	}
}


//============================================================================================
//	０１－１３		ポケモン連れ歩き
//============================================================================================
//--------------------------------------------------------------------
/**
 * @brief	トピック生成：ポケモン連れ歩き
 * @param	fsys	フィールド制御ワークへのポインタ
 * @param	word		簡易会話単語ID
 */
//--------------------------------------------------------------------
void TVTOPIC_Entry_QandA_Park(FIELDSYS_WORK * fsys, PMS_WORD word)
{
	TOPIC_DATA TopicData;
	QANDA_PARK_WORK * qpark = &TopicData.QandAParkWork;
	TOPICTEMP_PARK * ptemp = TVWORK_GetParkTemp(SaveData_GetTvWork(fsys->savedata));
	qpark->park = *ptemp;
	qpark->word = word;
	ptemp->exist_flag = FALSE;
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_TVWORK)
	SVLD_SetCrc(GMDATA_ID_TVWORK);
#endif //CRC_LOADCHECK
	MakeTopic(fsys, TVTOPICTYPE_INTERVIEW, TVTOPIC_INTERVIEW_TUREARUKI, qpark);
}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
static int TVTOPIC_Make_QandA_Park(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	QANDA_PARK_WORK * qpark = TVBCD_GetTopicData(bcd);
	WORDSET_RegisterOwnerName(ws, 0, bcd);		//トレーナー名
	WORDSET_RegisterMonsName(ws, 1, qpark->park.monsno,
			qpark->park.mons_sex, qpark->park.mons_region, qpark->park.mons_rom);
	WORDSET_RegisterSeikaku(ws, 2, qpark->park.poke_character);
	WORDSET_RegisterPMSWord(ws, 5, qpark->word);
	switch (qpark->park.pick_type) {
	case PARKPICK_ID_NOTHING:
		return msg_tv_01_13_02;
	case PARKPICK_ID_ACCESSORIES:
		WORDSET_RegisterAccessoryName(ws, 3, qpark->park.accessories);
		return msg_tv_01_13_01;
	case PARKPICK_ID_ITEM:
		WORDSET_RegisterItemName(ws, 3, qpark->park.item_no);
		return msg_tv_01_13_03;
	}
	return msg_tv_01_13_02;
}


//============================================================================================
//	０１－１４		バトルステージ
//============================================================================================
//--------------------------------------------------------------------
/**
 * @brief	トピック生成：バトルステージ
 * @param	fsys	フィールド制御ワークへのポインタ
 * @param	word	簡易会話単語ID
 */
//--------------------------------------------------------------------
void TVTOPIC_Entry_QandA_Stage(FIELDSYS_WORK * fsys, PMS_WORD word)
{
	TOPIC_DATA TopicData;
	QANDA_STAGE_WORK * qstage = &TopicData.QandAStageWork;
	TOPICTEMP_STAGE * ptemp = TVWORK_GetStageTemp(SaveData_GetTvWork(fsys->savedata));
	qstage->stage = *ptemp;
	qstage->word = word;
	ptemp->exist_flag = FALSE;
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_TVWORK)
	SVLD_SetCrc(GMDATA_ID_TVWORK);
#endif //CRC_LOADCHECK
	MakeTopic(fsys, TVTOPICTYPE_INTERVIEW, TVTOPIC_INTERVIEW_STAGE, qstage);
}
//--------------------------------------------------------------------
///	トピック放送：バトルステージ
//--------------------------------------------------------------------
static int TVTOPIC_Make_QandA_Stage(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	QANDA_STAGE_WORK * qstage = TVBCD_GetTopicData(bcd);
	WORDSET_RegisterOwnerName(ws, 0, bcd);		//トレーナー名
	WORDSET_RegisterMonsName(ws, 1, qstage->stage.monsno,
			qstage->stage.mons_sex, qstage->stage.mons_region, qstage->stage.mons_rom);

	if (qstage->stage.nickname_flag) {
		WORDSET_RegisterSTRCODE(ws, 2, qstage->stage.nickname,
					qstage->stage.mons_sex, qstage->stage.mons_region, TRUE);
	}else{
		WORDSET_RegisterMonsName(ws, 2, qstage->stage.monsno,
			qstage->stage.mons_sex, qstage->stage.mons_region, qstage->stage.mons_rom);
	}

	WORDSET_RegisterPMSWord(ws, 3, qstage->word);
	return msg_tv_01_14_01;
}

//--------------------------------------------------------------------
///	トピック制限チェック：バトルステージ
//--------------------------------------------------------------------
static BOOL TVTOPIC_Limit_QandA_Stage(FIELDSYS_WORK * fsys, TVBCD * bcd)
{
	return SysFlag_ArriveGet(SaveData_GetEventWork(fsys->savedata), FLAG_ARRIVE_C11);
}


//============================================================================================
//	０１－１５		へんなアンケート
//============================================================================================
//--------------------------------------------------------------------
/**
 * @brief	トピック生成：へんなアンケート
 * @param	fsys	フィールド制御ワークへのポインタ
 * @param	word	簡易会話単語ID
 */
//--------------------------------------------------------------------
void TVTOPIC_Entry_QandA_Strange1(FIELDSYS_WORK * fsys, PMS_WORD word)
{
	TOPIC_DATA TopicData;
	QANDA_STRANGE_WORK1 * qstrange = &TopicData.QandAStrangeWork1;
	qstrange->word = word;
	MakeTopic(fsys, TVTOPICTYPE_INTERVIEW, TVTOPIC_INTERVIEW_STRANGE1, qstrange);
}
//--------------------------------------------------------------------
///	トピック放送：へんなアンケート
//--------------------------------------------------------------------
static int TVTOPIC_Make_QandA_Strange1(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	QANDA_STRANGE_WORK1 * qstrange = TVBCD_GetTopicData(bcd);
	WORDSET_RegisterOwnerName(ws, 0, bcd);		//トレーナー名
	WORDSET_RegisterPMSWord(ws, 1, qstrange->word);
	return msg_tv_01_15_01;
}


//============================================================================================
//	０１－１６		へんなアンケート
//============================================================================================
//--------------------------------------------------------------------
/**
 * @brief	トピック生成：へんなアンケート
 * @param	fsys	フィールド制御ワークへのポインタ
 * @param	word	簡易会話単語ID
 */
//--------------------------------------------------------------------
void TVTOPIC_Entry_QandA_Strange2(FIELDSYS_WORK * fsys, PMS_WORD word)
{
	TOPIC_DATA TopicData;
	QANDA_STRANGE_WORK2 * qstrange = &TopicData.QandAStrangeWork2;
	qstrange->word = word;
	MakeTopic(fsys, TVTOPICTYPE_INTERVIEW, TVTOPIC_INTERVIEW_STRANGE2, qstrange);
}
//--------------------------------------------------------------------
///	トピック放送：へんなアンケート
//--------------------------------------------------------------------
static int TVTOPIC_Make_QandA_Strange2(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	QANDA_STRANGE_WORK2 * qstrange = TVBCD_GetTopicData(bcd);
	WORDSET_RegisterOwnerName(ws, 0, bcd);		//トレーナー名
	WORDSET_RegisterPMSWord(ws, 1, qstrange->word);
	return msg_tv_01_15_02;
}


//============================================================================================
//	０１－１７		へんなアンケート
//============================================================================================
//--------------------------------------------------------------------
/**
 * @brief	トピック生成：へんなアンケート
 * @param	fsys	フィールド制御ワークへのポインタ
 * @param	word	簡易会話単語ID
 */
//--------------------------------------------------------------------
void TVTOPIC_Entry_QandA_Strange3(FIELDSYS_WORK * fsys, PMS_WORD word)
{
	TOPIC_DATA TopicData;
	QANDA_STRANGE_WORK3 * qstrange = &TopicData.QandAStrangeWork3;
	qstrange->word = word;
	MakeTopic(fsys, TVTOPICTYPE_INTERVIEW, TVTOPIC_INTERVIEW_STRANGE3, qstrange);
}
//--------------------------------------------------------------------
///	トピック放送：へんなアンケート
//--------------------------------------------------------------------
static int TVTOPIC_Make_QandA_Strange3(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	QANDA_STRANGE_WORK3 * qstrange = TVBCD_GetTopicData(bcd);
	WORDSET_RegisterOwnerName(ws, 0, bcd);		//トレーナー名
	WORDSET_RegisterPMSWord(ws, 1, qstrange->word);
	return msg_tv_01_15_03;
}


//============================================================================================
//	０１－１８		へんなアンケート
//============================================================================================
//--------------------------------------------------------------------
/**
 * @brief	トピック生成：へんなアンケート
 * @param	fsys	フィールド制御ワークへのポインタ
 * @param	word	簡易会話単語ID
 */
//--------------------------------------------------------------------
void TVTOPIC_Entry_QandA_Strange4(FIELDSYS_WORK * fsys, PMS_WORD word)
{
	TOPIC_DATA TopicData;
	QANDA_STRANGE_WORK4 * qstrange = &TopicData.QandAStrangeWork4;
	qstrange->word = word;
	MakeTopic(fsys, TVTOPICTYPE_INTERVIEW, TVTOPIC_INTERVIEW_STRANGE4, qstrange);
}
//--------------------------------------------------------------------
///	トピック放送：へんなアンケート
//--------------------------------------------------------------------
static int TVTOPIC_Make_QandA_Strange4(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	QANDA_STRANGE_WORK4 * qstrange = TVBCD_GetTopicData(bcd);
	WORDSET_RegisterOwnerName(ws, 0, bcd);		//トレーナー名
	WORDSET_RegisterPMSWord(ws, 1, qstrange->word);
	return msg_tv_01_15_04;
}


//============================================================================================
//	０１－１９		フロンティアで仲良し
//============================================================================================
//--------------------------------------------------------------------
/**
 * @brief	トピック生成：フロンティアで仲良し
 * @param	fsys	フィールド制御ワークへのポインタ
 * @param	word	簡易会話単語ID
 */
//--------------------------------------------------------------------
void TVTOPIC_Entry_QandA_Friend(FIELDSYS_WORK * fsys, PMS_WORD word)
{
	TOPIC_DATA TopicData;
	QANDA_FRIEND_WORK * qfriend = &TopicData.QandAFriendWork;
	TOPICTEMP_FRIEND * ptemp = TVWORK_GetFriendTemp(SaveData_GetTvWork(fsys->savedata));
	qfriend->friend = *ptemp;
	qfriend->word = word;
	ptemp->exist_flag = FALSE;
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_TVWORK)
	SVLD_SetCrc(GMDATA_ID_TVWORK);
#endif //CRC_LOADCHECK
	MakeTopic(fsys, TVTOPICTYPE_INTERVIEW, TVTOPIC_INTERVIEW_FRIEND, qfriend);
}
//--------------------------------------------------------------------
///	トピック放送：フロンティアで仲良し
//--------------------------------------------------------------------
static int TVTOPIC_Make_QandA_Friend(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	int msgid;
	QANDA_FRIEND_WORK * qfriend = TVBCD_GetTopicData(bcd);
	STRBUF* strbuf = STRBUF_Create(64, HEAPID_FIELD);

	WORDSET_RegisterOwnerName(ws, 0, bcd);		//トレーナー名

	STRBUF_SetStringCode( strbuf, qfriend->friend.friend_name );

	//WORDSET_RegisterWord(ws, 1, qfriend->friend.friend_name, qfriend->friend.friend_sex, 
	WORDSET_RegisterWord(ws, 1, strbuf, qfriend->friend.friend_sex, 
						FALSE, qfriend->friend.region);
	//WORDSET_RegisterWord(ws, 1, strbuf, PM_MALE, TRUE, PM_LANG);
	STRBUF_Delete(strbuf);

	WORDSET_RegisterPMSWord(ws, 2, qfriend->word);

	//施設によってメッセージが違う
	switch( qfriend->friend.bf_no ){
	case FRONTIER_NO_TOWER:
		msgid = msg_tv_01_16_01;
		break;

	case FRONTIER_NO_CASTLE:
		msgid = msg_tv_01_16_02;
		break;

	case FRONTIER_NO_STAGE:
		msgid = msg_tv_01_16_03;
		break;

	case FRONTIER_NO_FACTORY_LV50:
	case FRONTIER_NO_FACTORY_LV100:
		msgid = msg_tv_01_16_04;
		break;

	case FRONTIER_NO_ROULETTE:
		msgid = msg_tv_01_16_05;
		break;
	};

	return msgid;
}

//--------------------------------------------------------------------
///	トピック制限チェック：フロンティアで仲良し
//--------------------------------------------------------------------
static BOOL TVTOPIC_Limit_QandA_Friend(FIELDSYS_WORK * fsys, TVBCD * bcd)
{
	return SysFlag_ArriveGet(SaveData_GetEventWork(fsys->savedata), FLAG_ARRIVE_C11);
}


//============================================================================================
//
//
//			イベント型トピック
//
//
//============================================================================================

//============================================================================================
//０４－０１	グループ紹介
//============================================================================================
static const u8 RandomGroupID[] = {
	RANDOMGROUP_MYUSE,
	RANDOMGROUP_DATA0,
	RANDOMGROUP_DATA1,
	RANDOMGROUP_DATA2,
	RANDOMGROUP_DATA3,
};
//--------------------------------------------------------------------
//--------------------------------------------------------------------
static int CountRandomGroup(RANDOM_GROUP * rg)
{
	int i,n;
	for (i = 0, n = 0; i< NELEMS(RandomGroupID); i++) {
		if (RandomGroup_ExistCheck(rg, RandomGroupID[i])) {
			//OS_TPrintf("RGROUP[%d] OK!\n",RandomGroupID[i]);
			n ++;
		}
	}
	return n;
}

//--------------------------------------------------------------------
///	トピック放送：グループ紹介
//--------------------------------------------------------------------
static int TVTOPIC_Make_Event_Group(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	int i, n, id;
	int type;
	RANDOM_GROUP * rg = SaveData_GetRandomGroup(fsys->savedata);
	n = CountRandomGroup(rg);
	GF_ASSERT(n > 0);
	if (n > 1) {
		n = gf_mtRand() % n;
	} else {
		n = 0;
	}
	for (i = 0; i < NELEMS(RandomGroupID); i++) {
		if (RandomGroup_ExistCheck(rg, RandomGroupID[i])) {
			if (n == 0) {
				//OS_TPrintf("RGROUP:choose %d\n",RandomGroupID[i]);
				id = RandomGroupID[i];
				break;
			} else {
				n --;
			}
		}
	}
	GF_ASSERT(n == 0);

	type = gf_p_rand(17);
	if (type >= HATE_TYPE) {
		type ++;
	}
	WORDSET_RegisterGroupName(ws, fsys->savedata, id, 0, RANDOMGROUP_NAME_LEADER);
	WORDSET_RegisterGroupName(ws, fsys->savedata, id, 1, RANDOMGROUP_NAME_GROUP);
	WORDSET_RegisterPokeTypeName(ws, 2, type);
	return msg_tv_04_01_01;
}
//--------------------------------------------------------------------
///	トピック制限チェック：グループ紹介
//--------------------------------------------------------------------
static BOOL TVTOPIC_Limit_Event_Group(FIELDSYS_WORK * fsys, TVBCD * bcd)
{
	RANDOM_GROUP * rg = SaveData_GetRandomGroup(fsys->savedata);
	if (CountRandomGroup(rg) != 0) {
		return TRUE;
	} else {
		return FALSE;
	}
}


//============================================================================================
//０４－０２	天気予報
//============================================================================================
static u16 WeatherTable[] = {
	ZONE_ID_R212B,
	ZONE_ID_R213,
	ZONE_ID_R216,
	ZONE_ID_L03,
	ZONE_ID_C09,
};

//--------------------------------------------------------------------
///	トピック放送：天気予報
//--------------------------------------------------------------------
static int TVTOPIC_Make_Event_Weather(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	int zone, weather;
	zone = WeatherTable[gf_p_rand(NELEMS(WeatherTable))];
	weather = WeatherData_Get(fsys, zone);
	WORDSET_RegisterPlaceName(ws, 0, ZoneData_GetPlaceNameID(zone));
	switch (weather) {
	case WEATHER_SYS_SUNNY:
		switch (gf_p_rand(4)) {
		case 0:
			return msg_tv_04_02_01;
		case 1:
			return msg_tv_04_02_02;
		case 2:
			return msg_tv_04_02_03;
		case 3:
			return msg_tv_04_02_04;
		}
	case WEATHER_SYS_CLOUDINESS:
		return msg_tv_04_02_05;
	case WEATHER_SYS_RAIN:
		return msg_tv_04_02_06;
	case WEATHER_SYS_STRAIN:
		return msg_tv_04_02_07;
	case WEATHER_SYS_SNOW:
		return msg_tv_04_02_08;
	case WEATHER_SYS_SNOWSTORM:
		return msg_tv_04_02_09;
	case WEATHER_SYS_SNOWSTORM_H:
		return msg_tv_04_02_10;
	case WEATHER_SYS_SPARK:
		return msg_tv_04_02_11;
	case WEATHER_SYS_DIAMONDDUST:
		return msg_tv_04_02_12;
	default:
		GF_ASSERT(0);
	}
	return msg_tv_04_02_01;		//ありえないはずだが、とりあえず
}
//--------------------------------------------------------------------
///	トピック制限チェック：天気予報
//--------------------------------------------------------------------
static BOOL TVTOPIC_Limit_Event_Weather(FIELDSYS_WORK * fsys, TVBCD * bcd)
{
	return TRUE;
}


//============================================================================================
//０４－０３	シンオウあの街この街
//============================================================================================
//--------------------------------------------------------------------
///	トピック放送：シンオウあの街この街★
//--------------------------------------------------------------------
static int TVTOPIC_Make_Event_TownGuide(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	MYSTATUS* my = SaveData_GetMyStatus( GameSystem_GetSaveData(fsys) );
	int id = fsys->location->zone_id;
	
	//フタバ
	if(id == ZONE_ID_T01 || (id >= ZONE_ID_T01R0101 && id <= ZONE_ID_T01R0401)){
		WORDSET_RegisterPlayerName(ws, 0, my);
		WORDSET_RegisterRivalName(ws, 1, fsys->savedata);
		return msg_tv_04_03_01;
	}

	//マサゴ
	if(id == ZONE_ID_T02 || (id >= ZONE_ID_T02R0101 && id <= ZONE_ID_T02R0301)){
		WORDSET_RegisterSupportName(ws, 1, fsys->savedata);
		return msg_tv_04_03_02;
	}

	//ソノオ
	if(id == ZONE_ID_T03 || (id >= ZONE_ID_T03R0101 && id <= ZONE_ID_T03R0301) || id == ZONE_ID_D13R0102){
		return msg_tv_04_03_03;
	}

	//ズイ
	if(id == ZONE_ID_T04 || (id >= ZONE_ID_T04R0101 && id <= ZONE_ID_T04R0501)){
		return msg_tv_04_03_04;
	}

	//カンナギ
	if(id == ZONE_ID_T05 || (id >= ZONE_ID_T05R0101 && id <= ZONE_ID_T05R0501)){
		return msg_tv_04_03_05;
	}

	//コトブキ
	if(id == ZONE_ID_C01 || (id >= ZONE_ID_C01R0101 && id <= ZONE_ID_C01R1001)){
		return msg_tv_04_03_06;
	}

	//ミオ
	if(id == ZONE_ID_C02 || (id >= ZONE_ID_C02R0101 && id <= ZONE_ID_C02R0501) || id == ZONE_ID_C02R0601){
		return msg_tv_04_03_07;
	}

	//クロガネ
	if(id == ZONE_ID_C03 || (id >= ZONE_ID_C03R0101 && id <= ZONE_ID_C03R0701)){
		return msg_tv_04_03_08;
	}

	//ハクタイ
	if(id == ZONE_ID_C04 || (id >= ZONE_ID_C04R0101 && id <= ZONE_ID_C04R0901)){
		return msg_tv_04_03_09;
	}

	//ヨスガ
	if(id == ZONE_ID_C05 || (id >= ZONE_ID_C05R0101 && id <= ZONE_ID_C05R1201)){
		return msg_tv_04_03_10;
	}

	//ノモセ
	if(id == ZONE_ID_C06 || (id >= ZONE_ID_C06R0101 && id <= ZONE_ID_C06R0601)){
		return msg_tv_04_03_11;
	}

	//トバリ
	if(id == ZONE_ID_C07 || (id >= ZONE_ID_C07R0101 && id <= ZONE_ID_C07R0901) || (id >= ZONE_ID_D26R0101 && id <= ZONE_ID_D26R0106)){
		return msg_tv_04_03_12;
	}

	//ナギサ
	if(id == ZONE_ID_C08 || (id >= ZONE_ID_C08R0101 && id <= ZONE_ID_C08R0801) || id == ZONE_ID_C08R0802){
		return msg_tv_04_03_13;
	}

	//キッサキ
	if(id == ZONE_ID_C09 || (id >= ZONE_ID_C09R0101 && id <= ZONE_ID_C09R0201)){
		return msg_tv_04_03_14;
	}

	WORDSET_RegisterPlayerName(ws, 0, my);
	WORDSET_RegisterRivalName(ws, 1, fsys->savedata);
	return msg_tv_04_03_15;
}


//============================================================================================
//０４－０５	大量発生！
//============================================================================================
//--------------------------------------------------------------------
///	トピック放送：大量発生！
//--------------------------------------------------------------------
static int TVTOPIC_Make_Event_Outbreak(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	u16 zone, poke;
	ENC_SV_PTR data = EncDataSave_GetSaveDataPtr(fsys->savedata);
	GenerateZone_GetInfo( EncDataSave_GetRandSeed(data, ENC_RND_SEED_GENERATE), &zone, &poke );
	WORDSET_RegisterPlaceName(ws, 0, ZoneData_GetPlaceNameID(zone));
	WORDSET_RegisterMonsNameEasy(ws, 1, poke);

	return msg_tv_04_05_01;
}
//--------------------------------------------------------------------
///	トピック制限チェック：大量発生！
//--------------------------------------------------------------------
static BOOL TVTOPIC_Limit_Event_Outbreak(FIELDSYS_WORK * fsys, TVBCD * bcd)
{
	ENC_SV_PTR data = EncDataSave_GetSaveDataPtr(fsys->savedata);
	return EncDataSave_IsGenerate(data);
}


//============================================================================================
//０４－０７	ポケモン相性チャンネル■
//============================================================================================
//============================================================================================
/**
 *	技とポケモンの相性チェック用相性テーブル
 */
//============================================================================================
#define	KOUKANAI	(0)
#define	IMAHITOTSU	(1)
#define	BATSUGUN	(2)
#define	HUTSUU		(3)


enum{
	WAZA_TYPE=0,
	POKE_TYPE,
	POWER
};

static	const	u8	TypeCheckTable[][3]={
	{NORMAL_TYPE,	IWA_TYPE,		IMAHITOTSU},	// NORMAL で IWA に攻撃すると、
													// 威力０．５倍
	{NORMAL_TYPE,	METAL_TYPE,		IMAHITOTSU},

	{FIRE_TYPE,		FIRE_TYPE,		IMAHITOTSU},
	{FIRE_TYPE,		WATER_TYPE,		IMAHITOTSU},
	{FIRE_TYPE,		KUSA_TYPE,		BATSUGUN},	
	{FIRE_TYPE,		KOORI_TYPE,		BATSUGUN},	
	{FIRE_TYPE,		MUSHI_TYPE,		BATSUGUN},
	{FIRE_TYPE,		IWA_TYPE,		IMAHITOTSU},
	{FIRE_TYPE,		DRAGON_TYPE,	IMAHITOTSU},
	{FIRE_TYPE,		METAL_TYPE,		BATSUGUN},

	{WATER_TYPE,	FIRE_TYPE,		BATSUGUN},
	{WATER_TYPE,	WATER_TYPE,		IMAHITOTSU},
	{WATER_TYPE,	KUSA_TYPE,		IMAHITOTSU},
	{WATER_TYPE,	JIMEN_TYPE,		BATSUGUN},
	{WATER_TYPE,	IWA_TYPE,		BATSUGUN},
	{WATER_TYPE,	DRAGON_TYPE,	IMAHITOTSU},

	{ELECTRIC_TYPE,	WATER_TYPE,		BATSUGUN},
	{ELECTRIC_TYPE,	ELECTRIC_TYPE,	IMAHITOTSU},
	{ELECTRIC_TYPE,	KUSA_TYPE,		IMAHITOTSU},
	{ELECTRIC_TYPE,	JIMEN_TYPE,		KOUKANAI},
	{ELECTRIC_TYPE,	HIKOU_TYPE,		BATSUGUN},
	{ELECTRIC_TYPE,	DRAGON_TYPE,	IMAHITOTSU},

	{KUSA_TYPE,		FIRE_TYPE,		IMAHITOTSU},
	{KUSA_TYPE,		WATER_TYPE,		BATSUGUN},
	{KUSA_TYPE,		KUSA_TYPE,		IMAHITOTSU},
	{KUSA_TYPE,		POISON_TYPE,	IMAHITOTSU},
	{KUSA_TYPE,		JIMEN_TYPE,		BATSUGUN},
	{KUSA_TYPE,		HIKOU_TYPE,		IMAHITOTSU},
	{KUSA_TYPE,		MUSHI_TYPE,		IMAHITOTSU},
	{KUSA_TYPE,		IWA_TYPE,		BATSUGUN},
	{KUSA_TYPE,		DRAGON_TYPE,	IMAHITOTSU},
	{KUSA_TYPE,		METAL_TYPE,		IMAHITOTSU},

	{KOORI_TYPE,	WATER_TYPE,		IMAHITOTSU},
	{KOORI_TYPE,	KUSA_TYPE,		BATSUGUN},
	{KOORI_TYPE,	KOORI_TYPE,		IMAHITOTSU},
	{KOORI_TYPE,	JIMEN_TYPE,		BATSUGUN},
	{KOORI_TYPE,	HIKOU_TYPE,		BATSUGUN},
	{KOORI_TYPE,	DRAGON_TYPE,	BATSUGUN},
	{KOORI_TYPE,	METAL_TYPE,		IMAHITOTSU},
	{KOORI_TYPE,	FIRE_TYPE,		IMAHITOTSU},

	{BATTLE_TYPE,	NORMAL_TYPE,	BATSUGUN},
	{BATTLE_TYPE,	KOORI_TYPE,		BATSUGUN},
	{BATTLE_TYPE,	POISON_TYPE,	IMAHITOTSU},
	{BATTLE_TYPE,	HIKOU_TYPE,		IMAHITOTSU},
	{BATTLE_TYPE,	SP_TYPE,		IMAHITOTSU},
	{BATTLE_TYPE,	MUSHI_TYPE,		IMAHITOTSU},
	{BATTLE_TYPE,	IWA_TYPE,		BATSUGUN},
	{BATTLE_TYPE,	AKU_TYPE,		BATSUGUN},
	{BATTLE_TYPE,	METAL_TYPE,		BATSUGUN},

	{POISON_TYPE,	KUSA_TYPE,		BATSUGUN},
	{POISON_TYPE,	POISON_TYPE,	IMAHITOTSU},
	{POISON_TYPE,	JIMEN_TYPE,		IMAHITOTSU},
	{POISON_TYPE,	IWA_TYPE,		IMAHITOTSU},
	{POISON_TYPE,	GHOST_TYPE,		IMAHITOTSU},
	{POISON_TYPE,	METAL_TYPE,		KOUKANAI},

	{JIMEN_TYPE,	FIRE_TYPE,		BATSUGUN},
	{JIMEN_TYPE,	ELECTRIC_TYPE,	BATSUGUN},
	{JIMEN_TYPE,	KUSA_TYPE,		IMAHITOTSU},
	{JIMEN_TYPE,	POISON_TYPE,	BATSUGUN},
	{JIMEN_TYPE,	HIKOU_TYPE,		KOUKANAI},
	{JIMEN_TYPE,	MUSHI_TYPE,		IMAHITOTSU},
	{JIMEN_TYPE,	IWA_TYPE,		BATSUGUN},
	{JIMEN_TYPE,	METAL_TYPE,		BATSUGUN},

	{HIKOU_TYPE,	ELECTRIC_TYPE,	IMAHITOTSU},
	{HIKOU_TYPE,	KUSA_TYPE,		BATSUGUN},
	{HIKOU_TYPE,	BATTLE_TYPE,	BATSUGUN},
	{HIKOU_TYPE,	MUSHI_TYPE,		BATSUGUN},
	{HIKOU_TYPE,	IWA_TYPE,		IMAHITOTSU},
	{HIKOU_TYPE,	METAL_TYPE,		IMAHITOTSU},

	{SP_TYPE,		BATTLE_TYPE,	BATSUGUN},
	{SP_TYPE,		POISON_TYPE,	BATSUGUN},
	{SP_TYPE,		SP_TYPE,		IMAHITOTSU},
	{SP_TYPE,		AKU_TYPE,		KOUKANAI},
	{SP_TYPE,		METAL_TYPE,		IMAHITOTSU},	

	{MUSHI_TYPE,	FIRE_TYPE,		IMAHITOTSU},
	{MUSHI_TYPE,	KUSA_TYPE,		BATSUGUN},
	{MUSHI_TYPE,	BATTLE_TYPE,	IMAHITOTSU},
	{MUSHI_TYPE,	POISON_TYPE,	IMAHITOTSU},
	{MUSHI_TYPE,	HIKOU_TYPE,		IMAHITOTSU},
	{MUSHI_TYPE,	SP_TYPE,		BATSUGUN},
	{MUSHI_TYPE,	GHOST_TYPE,		IMAHITOTSU},
	{MUSHI_TYPE,	AKU_TYPE,		BATSUGUN},
	{MUSHI_TYPE,	METAL_TYPE,		IMAHITOTSU},

	{IWA_TYPE,		FIRE_TYPE,		BATSUGUN},
	{IWA_TYPE,		KOORI_TYPE,		BATSUGUN},
	{IWA_TYPE,		BATTLE_TYPE,	IMAHITOTSU},
	{IWA_TYPE,		JIMEN_TYPE,		IMAHITOTSU},
	{IWA_TYPE,		HIKOU_TYPE,		BATSUGUN},
	{IWA_TYPE,		MUSHI_TYPE,		BATSUGUN},
	{IWA_TYPE,		METAL_TYPE,		IMAHITOTSU},

	{GHOST_TYPE,	NORMAL_TYPE,	KOUKANAI},
	{GHOST_TYPE,	SP_TYPE,		BATSUGUN},
	{GHOST_TYPE,	AKU_TYPE,		IMAHITOTSU},
	{GHOST_TYPE,	METAL_TYPE,		IMAHITOTSU},
	{GHOST_TYPE,	GHOST_TYPE,		BATSUGUN},	

	{DRAGON_TYPE,	DRAGON_TYPE,	BATSUGUN},
	{DRAGON_TYPE,	METAL_TYPE,		IMAHITOTSU},

	{AKU_TYPE,		BATTLE_TYPE,	IMAHITOTSU},
	{AKU_TYPE,		SP_TYPE,		BATSUGUN},
	{AKU_TYPE,		GHOST_TYPE,		BATSUGUN},
	{AKU_TYPE,		AKU_TYPE,		IMAHITOTSU},
	{AKU_TYPE,		METAL_TYPE,		IMAHITOTSU},

	{METAL_TYPE,	FIRE_TYPE,		IMAHITOTSU},
	{METAL_TYPE,	WATER_TYPE,		IMAHITOTSU},
	{METAL_TYPE,	ELECTRIC_TYPE,	IMAHITOTSU},
	{METAL_TYPE,	KOORI_TYPE,		BATSUGUN},
	{METAL_TYPE,	IWA_TYPE,		BATSUGUN},
	{METAL_TYPE,	METAL_TYPE,		IMAHITOTSU},

	{NORMAL_TYPE,	GHOST_TYPE,		KOUKANAI},
	{BATTLE_TYPE,	GHOST_TYPE,		KOUKANAI},

};
static int GetWazaKouka(int attack, int deffence)
{
	int i;
	for (i = 0; i < NELEMS(TypeCheckTable); i++) {
		if ((TypeCheckTable[i][WAZA_TYPE] == attack) && (TypeCheckTable[i][POKE_TYPE] == deffence)) {
			return TypeCheckTable[i][POWER];
		}
	}
	return HUTSUU;
}
//--------------------------------------------------------------------
///	トピック放送：
//--------------------------------------------------------------------
static int TVTOPIC_Make_Event_Aishou(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	int a_type, d_type;
	a_type = gf_p_rand(17);
	if (a_type >= HATE_TYPE) {
		a_type ++;
	}
	d_type = gf_p_rand(17);
	if (d_type >= HATE_TYPE) {
		d_type ++;
	}
	WORDSET_RegisterPokeTypeName(ws, 0, a_type);
	WORDSET_RegisterPokeTypeName(ws, 1, d_type);
	switch (GetWazaKouka(a_type, d_type)) {
	case BATSUGUN:
		return msg_tv_04_07_01;
	case HUTSUU:
	default:
		return msg_tv_04_07_02;
	case IMAHITOTSU:
		return msg_tv_04_07_03;
	case KOUKANAI:
		return msg_tv_04_07_04;
	}
}

//--------------------------------------------------------------------
///	トピック制限チェック：
//--------------------------------------------------------------------
static BOOL TVTOPIC_Limit_Event_Aishou(FIELDSYS_WORK * fsys, TVBCD * bcd)
{
#if 0
	return TRUE;
#else
	return FALSE;	//プラチナでは放送しない
#endif
}


//============================================================================================
//０４－０９	きのみじょうほう
//============================================================================================
static const u16 KinomiFlagTable[] = {
	FLAG_ARRIVE_T01,	//( 0)
	FLAG_ARRIVE_T02,	//( 1)
	FLAG_ARRIVE_T03,	//( 2)
	FLAG_ARRIVE_T04,	//( 3)
	FLAG_ARRIVE_T05,	//( 4)
	FLAG_ARRIVE_T06,	//( 5)
	FLAG_ARRIVE_T07,	//( 6)
	FLAG_ARRIVE_C01,	//( 7)
	FLAG_ARRIVE_C02,	//( 8)
	FLAG_ARRIVE_C03,	//( 9)
	FLAG_ARRIVE_C04,	//(10)
	FLAG_ARRIVE_C05,	//(11)
	FLAG_ARRIVE_C06,	//(12)
	FLAG_ARRIVE_C07,	//(13)
	FLAG_ARRIVE_C08,	//(14)
	FLAG_ARRIVE_C09,	//(15)
	FLAG_ARRIVE_C10,	//(16)
	FLAG_ARRIVE_C11,	//(17)
};
#include "../fielddata/setting/kinomi_zone.c"

//--------------------------------------------------------------------
//--------------------------------------------------------------------
static int GetNutsID(FIELDSYS_WORK * fsys)
{
	u8 flags[NELEMS(KinomiFlagTable)];
	u8 id[NELEMS(KinomiZoneTable)/2];
	int i, n;
	EVENTWORK * ev = SaveData_GetEventWork(fsys->savedata);
	for (i = 0; i < NELEMS(KinomiFlagTable); i ++) {
		flags[i] = SysFlag_ArriveGet(ev, KinomiFlagTable[i]);
		OS_TPrintf("FLAG %02d = %d\n",KinomiFlagTable[i], flags[i]);
	}
	for (i = 0, n = 0; i < NELEMS(KinomiZoneTable)/2; i++ ) {
		if (flags[KinomiZoneTable[i*2]]) {
			OS_TPrintf("ID:%02d FLAG%02d\n",i, KinomiZoneTable[i*2]);
			id[n] = i;
			n ++;
		}
	}
	return id[gf_p_rand(n)];
}
//--------------------------------------------------------------------
///	トピック放送：きのみじょうほう
//--------------------------------------------------------------------
static int TVTOPIC_Make_Event_NutsInfo(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	SEEDBED * sbed = SaveData_GetSeedBed(fsys->savedata);
	int id, zone;

	id = GetNutsID(fsys);
	zone = KinomiZoneTable[id*2+1];
	WORDSET_RegisterPlaceName(ws, 0, ZoneData_GetPlaceNameID(zone));

	switch (SEEDBED_GetSeedStatus(sbed, id)) {
	case SEEDSTAT_FRUIT:
		return msg_tv_04_09_01;
	case SEEDSTAT_FLOWER:
		return msg_tv_04_09_02;
	case SEEDSTAT_MIKI:
		return msg_tv_04_09_03;
	case SEEDSTAT_HUTABA:
		return msg_tv_04_09_04;
	case SEEDSTAT_NOTHING:
	case SEEDSTAT_UNDERGROUND:
	default:
		return msg_tv_04_09_05;
	}
}

//--------------------------------------------------------------------
///	トピック制限チェック：きのみじょうほう
//--------------------------------------------------------------------
static BOOL TVTOPIC_Limit_Event_NutsInfo(FIELDSYS_WORK * fsys, TVBCD * bcd)
{
	EVENTWORK * ev = SaveData_GetEventWork(fsys->savedata);
	//シナリオ的にもっとも近い場所なので
	return SysFlag_ArriveGet(ev, FLAG_ARRIVE_C03);
}


//============================================================================================
//０４－１１	どうぐを持ってる野生ポケモン■
//============================================================================================
//--------------------------------------------------------------------
///	トピック放送：どうぐを持ってる野生ポケモン
//--------------------------------------------------------------------
static int TVTOPIC_Make_Event_PokemonItem(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	int size, ofs, pos;
	u16 poke, item;
	ArchiveDataLoadOfs(&size, ARC_TV, NARC_tv_pokeitem_bin, 0, sizeof(int));
	pos = gf_p_rand(size);
	ofs = sizeof(int) + pos * sizeof(u16) * 2;
	ArchiveDataLoadOfs(&poke, ARC_TV, NARC_tv_pokeitem_bin, ofs, sizeof(u16));
	ArchiveDataLoadOfs(&item, ARC_TV, NARC_tv_pokeitem_bin, ofs + sizeof(u16), sizeof(u16));
	WORDSET_RegisterMonsNameEasy(ws, 0, poke);
	WORDSET_RegisterItemName(ws, 1, item);
	return msg_tv_04_11_01;
}
//--------------------------------------------------------------------
///	トピック制限チェック：どうぐを持ってる野生ポケモン
//--------------------------------------------------------------------
static BOOL TVTOPIC_Limit_Event_PokemonItem(FIELDSYS_WORK * fsys, TVBCD * bcd)
{
#if 0
	return TRUE;
#else
	return FALSE;	//プラチナでは放送しない
#endif
}


//============================================================================================
//０４－１２	せいかく診断★
//============================================================================================
//--------------------------------------------------------------------
///	トピック放送：せいかく診断
//--------------------------------------------------------------------
static int TVTOPIC_Make_Event_PokemonCharactor(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	u32 rand,i;
	u8 seikaku;
	POKEMON_PARAM* pp;
	POKEMON_PASO_PARAM* ppp;
	int taste,status;

	//初期化
	taste = 0xff;
	status= 0xff;

	//適当な性格を求める
	rand	= ( gf_rand() % 0xffff );
	seikaku = PokeSeikakuGetRnd( rand );
	WORDSET_RegisterSeikaku( ws, 0, seikaku );

	//上がりやすいステータス、好きな味が存在しない性格の時
	if( (seikaku == POKE_PERSONALITY_GANBARIYA) ||				//がんばりや
		(seikaku == POKE_PERSONALITY_SUNAO) ||					//すなお
		(seikaku == POKE_PERSONALITY_MAJIME) ||					//まじめ
		(seikaku == POKE_PERSONALITY_TEREYA) ||					//てれや
		(seikaku == POKE_PERSONALITY_KIMAGURE) ){				//きまぐれ

		return msg_tv_04_12_03;
	}

	//ランダムで上がりやすいステータスか、好きな味か決める
	if( (rand % 2) == 0 ){

		//好きな味を取得
		for( i=0; i < 5; i++ ){
			if( PokeLikeTasteCheck(rand,i) == 1 ){
				taste = i;		//好きな味を保存
				break;
			}
		}

		//「○味なポフィンが好きなんだ」
		WORDSET_RegisterSeedTasteName( ws, 2, taste );
		return msg_tv_04_12_02;
	}

	//上がりやすいステータス
	
#if 0
	//poke_tool.h(以下の5項目)
	ID_PER_basic_pow,			//基本攻撃力
	ID_PER_basic_def,			//基本防御力
	ID_PER_basic_agi,			//基本素早さ
	ID_PER_basic_spepow,		//基本特殊攻撃力
	ID_PER_basic_spedef,		//基本特殊防御力
#endif

	for( i=0; i < 5; i++ ){
		if( PokeChrAbiTableGet(seikaku,ID_PER_basic_pow+i) > 0 ){
			status = i;
			break;
		}
	}

	//「○に自信があるんだ」
	WORDSET_RegisterPokeStatusName( ws, 1, STATUS_001+status );
	return msg_tv_04_12_01;
}


//============================================================================================
//０４－１５	移動ポケモン目撃情報★
//============================================================================================
#include "move_pokemon_def.h"
#include "move_pokemon.h"
//--------------------------------------------------------------------
///	トピック放送：移動ポケモン目撃情報
//--------------------------------------------------------------------
static int TVTOPIC_Make_Event_MovePokemon(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	MPD_PTR mpd;
	ENC_SV_PTR data;
	u16 move_rand,i;
	u32 monsno,rnd;
	STRBUF* strbuf = STRBUF_Create(BUFLEN_PLACE_NAME, HEAPID_FIELD);
	MYSTATUS* my = SaveData_GetMyStatus( GameSystem_GetSaveData(fsys) );

	data = EncDataSave_GetSaveDataPtr(fsys->savedata);
	move_rand = ( gf_rand() % LOCATION_MAX );

	//ゾーンIDから地名を取得
	PNC_GetPlaceNameFromZoneID( MP_GetMovePokeZone(move_rand), HEAPID_FIELD, strbuf );

	//単語セット
	//バッファID
	//文字列
	//性別コード
	//単／複（TRUEで単数）
	//言語コード
	WORDSET_RegisterWord( ws, 0, strbuf, PM_MALE, TRUE, PM_LANG );
	STRBUF_Delete(strbuf);

	//移動ポケモン名をセット
	for( i=0; i < MOVE_POKE_MAX ;i++ ){						//3匹(+3)
		if( EncDataSave_IsMovePokeValid(data,i) ){			//0-2(0-5)

			//指定移動ポケモンデータポインタの取得
			mpd = EncDataSave_GetMovePokeDataPtr( data, i );

			monsno = EncDataSave_GetMovePokeDataParam( mpd, MP_PARAM_MONSNO );	//モンスターNo
			rnd = EncDataSave_GetMovePokeDataParam( mpd, MP_PARAM_PER_RND );	//個性乱数

			WORDSET_RegisterMonsName(	ws, 1, monsno,
										PokeSexGetMonsNo(monsno,rnd),
										MyStatus_GetRegionCode(my),
										MyStatus_GetRomCode(my) );
			break;
		}
	}

	return msg_tv_04_15_01;
}
//--------------------------------------------------------------------
///	トピック制限チェック：移動ポケモン目撃情報
//--------------------------------------------------------------------
static BOOL TVTOPIC_Limit_Event_MovePokemon(FIELDSYS_WORK * fsys, TVBCD * bcd)
{
	int i;
	ENC_SV_PTR data;

	data = EncDataSave_GetSaveDataPtr(fsys->savedata);

	//移動ポケモンがいるかをチェック
	for( i=0; i < MOVE_POKE_MAX ;i++ ){						//3匹(+3)
		if( EncDataSave_IsMovePokeValid(data,i) ){			//0-2(0-5)
			return 1;
		}
	}

	return 0;
}


//============================================================================================
//０４－１７	ポケモンファッションチェック
//============================================================================================
//--------------------------------------------------------------------
//--------------------------------------------------------------------
static int CountClipInfo(IMC_SAVEDATA * iw)
{
	int i, n;
	for (i = 0, n = 0; i < IMC_SAVEDATA_TELEVISION_SAVENUM; i++) {
		if (ImcSaveData_CheckTelevisionSaveData(iw, i) == TRUE) {
			n ++;
		}
	}
	return n;
}
//--------------------------------------------------------------------
///	トピック放送：ポケモンファッションチェック
//--------------------------------------------------------------------
static int TVTOPIC_Make_Event_ClipInfo(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	IMC_TELEVISION_SAVEDATA * itv;
	int i,n,count,id;
	IMC_SAVEDATA* iw = SaveData_GetImcSaveData(fsys->savedata);
	n = CountClipInfo(iw);
	if (n > 1) {
		count = gf_mtRand() % n;
	} else {
		count = 0;
	}
	for (i = 0; i < IMC_SAVEDATA_TELEVISION_SAVENUM; i++ ) {
		if (ImcSaveData_CheckTelevisionSaveData(iw, i) == TRUE) {
			if (count == 0) {
				id = i;
				break;
			} else {
				count --;
			}
		}
	}
	GF_ASSERT(i < IMC_SAVEDATA_TELEVISION_SAVENUM);
	itv= ImcSaveData_GetTelevisionSaveData(iw, id);
	{
		//PMS_DATA pmsdata;
		PMS_WORD pmsword;
		STRBUF * strbuf = STRBUF_Create(PERSON_NAME_SIZE + EOM_SIZE, HEAPID_FIELD);
		int sex = ImcSaveData_GetTelevisionPokeOyaSex(itv);
		ImcSaveData_GetTelevisionPokeOyaName(itv, strbuf);
		WORDSET_RegisterWord(ws, 0, strbuf, sex, TRUE, ImcSaveData_GetCountryCode(itv));
		STRBUF_Delete(strbuf);
		pmsword = ImcSaveData_GetTelevisionTitlePmsWord(itv);
		WORDSET_RegisterPMSWord(ws, 1, pmsword);
		//ImcSaveData_GetTelevisionTitle(itv, &pmsdata);
		//WORDSET_RegisterPMSWord(ws, 1, pmsdata.word[0]);
	}

	return msg_tv_04_17_01;
}
//--------------------------------------------------------------------
///	トピック制限チェック：ポケモンファッションチェック
//--------------------------------------------------------------------
static BOOL TVTOPIC_Limit_Event_ClipInfo(FIELDSYS_WORK * fsys, TVBCD * bcd)
{
	IMC_SAVEDATA* iw = SaveData_GetImcSaveData(fsys->savedata);
	if (CountClipInfo(iw) != 0) {
		return TRUE;
	} else {
		return FALSE;
	}
}


//============================================================================================
//
//
//				バラエティ型トピック
//
//
//============================================================================================
//============================================================================================
//	ポケモン捜索隊
//============================================================================================
//--------------------------------------------------------------------
///	トピック放送：ポケモン捜索隊
//--------------------------------------------------------------------
static int TVTOPIC_Make_Variety_SearchParty(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	u16 rand;

	rand = 0;

	//ゲームクリアしていたら
	if( SysFlag_GameClearCheck(SaveData_GetEventWork(fsys->savedata)) == TRUE ){
		rand = ( gf_rand() % 8 );		//1,2,3,4,5,6,7,8

	//ヨスガ到着していたら
	}else if( SysFlag_ArriveGet(SaveData_GetEventWork(fsys->savedata),FLAG_ARRIVE_C05) == 1 ){
		rand = ( gf_rand() % 5 );		//1,2,3,4,5

	//ハクタイ到着していたら
	}else if( SysFlag_ArriveGet(SaveData_GetEventWork(fsys->savedata),FLAG_ARRIVE_C04) == 1 ){
		rand = ( gf_rand() % 4 );		//1,2,3,4

	//クロガネ炭鉱到着していたら
	}else if( SysFlag_ArriveGet(SaveData_GetEventWork(fsys->savedata),FLAG_ARRIVE_D01R0101) == 1 ){
		rand = ( gf_rand() % 2 );		//1,2
	};

	//ゲーム最初から
	return msg_tv_05_01_01 + rand;		//1
}


//============================================================================================
//	マキシマム試合中継
//============================================================================================
static STRBUF * PokeNameGetAlloc( u16 id, u32 heap );
//--------------------------------------------------------------------
///	トピック放送：マキシマム試合中継
//--------------------------------------------------------------------
static int TVTOPIC_Make_Variety_Leader(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	STRBUF* buf;
	u16 rand,i,monsno;
	const ZUKAN_WORK* zw = SaveData_GetZukanWork( fsys->savedata );

	rand = ( gf_rand() % (MONSNO_END-1) ) + 1;				//0はダミーなので
	for( i=1; i <= MONSNO_END; i++ ){
		if( ZukanWork_GetPokeSeeFlag(zw,rand) == TRUE ){
			monsno = rand;									//選出されたポケモンナンバー
			break;
		}

		rand++;
		if( rand == MONSNO_END ){
			rand = 1;
		}
	}

	//単語セット
	//バッファID
	//文字列
	//性別コード
	//単／複（TRUEで単数）
	//言語コード
	buf	= PokeNameGetAlloc( monsno, HEAPID_FIELD );
	WORDSET_RegisterWord( ws, 0, buf, PM_MALE, TRUE, PM_LANG );
	STRBUF_Delete( buf );

	rand = ( gf_rand() % 3 );
	if( rand == 0 ){
		return msg_tv_05_02_01;
	}else if( rand == 1 ){
		rand = ( gf_rand() % 34 ) + 25;						//25-58のランダム
		WORDSET_RegisterNumber(ws, 1, rand, 2,
				NUMBER_DISPTYPE_LEFT, NUMBER_CODETYPE_DEFAULT);
		return msg_tv_05_02_02;
	}else{
		return msg_tv_05_02_03;
	};
}

static STRBUF * PokeNameGetAlloc( u16 id, u32 heap )
{
	MSGDATA_MANAGER* man;
	STRBUF* str;
	
	man = MSGMAN_Create( MSGMAN_TYPE_DIRECT, ARC_MSG, NARC_msg_monsname_dat, heap );
	str = MSGMAN_AllocString( man, id );

	MSGMAN_Delete( man );
	return str;
}

//--------------------------------------------------------------------
///	トピック制限チェック：マキシマム試合
//--------------------------------------------------------------------
static BOOL TVTOPIC_Limit_Variety_Leader(FIELDSYS_WORK * fsys, TVBCD * bcd)
{
	const ZUKAN_WORK* zw = SaveData_GetZukanWork( fsys->savedata );

	//図鑑持っているか
	if( ZukanWork_GetZukanGetFlag(zw) == TRUE ){
		return TRUE;
	}else{
		return FALSE;
	}
}


//============================================================================================
//	連続テレビドラマなみのりのはてに
//============================================================================================
//--------------------------------------------------------------------
///	トピック放送：連続テレビドラマなみのりのはてに
//--------------------------------------------------------------------
static int TVTOPIC_Make_Variety_Dorama1(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	u16 rand;

	rand = ( gf_rand() % 3 );

	if( rand == 0 ){
		return msg_tv_05_03_01;
	}else if( rand == 1 ){
		return msg_tv_05_03_02;
	}else {
		return msg_tv_05_03_03;
	};
}


//============================================================================================
//	連続テレビドラマポケッチデカ
//============================================================================================
//--------------------------------------------------------------------
///	トピック放送：連続テレビドラマポケッチデカ
//--------------------------------------------------------------------
static int TVTOPIC_Make_Variety_Dorama2(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	u16 rand;

	rand = ( gf_rand() % 3 );

	if( rand == 0 ){
		return msg_tv_05_04_01;
	}else if( rand == 1 ){
		return msg_tv_05_04_02;
	}else{
		return msg_tv_05_04_03;
	}
}


//============================================================================================
//	音楽番組
//============================================================================================
#include "application/imageClip/imc_itemid_define.h"	//アクセサリーナンバー
#include "battle/wazano_def.h"							//技ナンバー
#include "scr_tool.h"

//--------------------------------------------------------------------
///	トピック放送：音楽番組
//--------------------------------------------------------------------
static int TVTOPIC_Make_Variety_Dorama3(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	STRBUF* buf;
	u16 start_pos,i;
	POKEMON_PARAM* pp;
	POKEPARTY* party;
	MYSTATUS* my	= SaveData_GetMyStatus( fsys->savedata );
	ZUKAN_WORK* zw	= SaveData_GetZukanWork( fsys->savedata );

	party = SaveData_GetTemotiPokemon( fsys->savedata );
	pp = PokeParty_GetMemberPointer( party, GetFrontPokemon(fsys->savedata) );

	//手持ち先頭のポケモン名
	WORDSET_RegisterMonsName(	ws, 0, 
								PokeParaGet(pp,ID_PARA_monsno,NULL),
								PokeParaGet(pp,ID_PARA_sex,NULL),
								MyStatus_GetRegionCode(my),
								MyStatus_GetRomCode(my) );

	//アクセサリー名
	WORDSET_RegisterAccessoryName( ws, 1, (gf_rand() % IMC_ACCE_MAX) );

	//ポケモン名(見たことある)
	start_pos = ( gf_rand() % (MONSNO_END-2) + 1 );		//1オリジン

	for( i=1; i <= MONSNO_END; i++ ){

		if( ZukanWork_GetPokeSeeFlag(zw,start_pos) == TRUE ){
			buf	= PokeNameGetAlloc( start_pos, HEAPID_FIELD );
			WORDSET_RegisterWord( ws, 2, buf, PM_MALE, TRUE, PM_LANG );
			STRBUF_Delete( buf );
			break;
		}

		start_pos++;
		if( start_pos >= MONSNO_END ){
			start_pos = 1;
		}
	}

	//技名(1オリジン)
	WORDSET_RegisterWazaName( ws, 3, (gf_rand() % WAZANO_MAX-2) + 1 );

	return msg_tv_05_05_01;
}

//--------------------------------------------------------------------
///	トピック制限チェック：音楽番組
//--------------------------------------------------------------------
static BOOL TVTOPIC_Limit_Variety_Dorama3(FIELDSYS_WORK * fsys, TVBCD * bcd)
{
	const ZUKAN_WORK* zw = SaveData_GetZukanWork( fsys->savedata );

	//図鑑持っているか
	if( ZukanWork_GetZukanGetFlag(zw) == TRUE ){
		return TRUE;
	}else{
		return FALSE;
	}
}


//============================================================================================
//	テレビ映画劇場
//============================================================================================
//--------------------------------------------------------------------
///	トピック放送：テレビ映画劇場
//--------------------------------------------------------------------
static int TVTOPIC_Make_Variety_Dorama4(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	return msg_tv_05_06_01;
}


//============================================================================================
//	GDS紹介
//============================================================================================
//--------------------------------------------------------------------
///	トピック放送：GDS紹介
//--------------------------------------------------------------------
static int TVTOPIC_Make_Variety_Dorama5(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	return msg_tv_05_07_01;
}

//--------------------------------------------------------------------
///	トピック制限チェック：GDS紹介
//--------------------------------------------------------------------
static BOOL TVTOPIC_Limit_Variety_Dorama5(FIELDSYS_WORK * fsys, TVBCD * bcd)
{
	//クロガネバッジチェック
	if( MyStatus_GetBadgeFlag(SaveData_GetMyStatus(fsys->savedata),BADGE_ID_C03) == TRUE ){
		return TRUE;
	}else{
		return FALSE;
	}
}


//============================================================================================
//	連続テレビドラマポケマジ
//============================================================================================
//--------------------------------------------------------------------
///	トピック放送：連続テレビドラマポケマジ
//--------------------------------------------------------------------
static int TVTOPIC_Make_Variety_Dorama6(FIELDSYS_WORK * fsys, WORDSET * ws, TVBCD * bcd)
{
	u16 rand;

	rand = ( gf_rand() % 9 );

	if( rand == 0 ){
		return msg_tv_05_08_01;
	}else if( rand == 1 ){
		return msg_tv_05_08_02;
	}else if( rand == 2 ){
		return msg_tv_05_08_03;
	}else if( rand == 3 ){
		return msg_tv_05_08_04;
	}else if( rand == 4 ){
		return msg_tv_05_08_05;
	}else if( rand == 5 ){
		return msg_tv_05_08_06;
	}else if( rand == 6 ){
		return msg_tv_05_08_07;
	}else if( rand == 7 ){
		return msg_tv_05_08_08;
	}else{
		return msg_tv_05_08_09;
	}
}


//============================================================================================
//
//	監視型 放送関数テーブル★
//
//============================================================================================

//--------------------------------------------------------------------
/**
 * @brief	監視型トピック放送関数テーブル
 */
//--------------------------------------------------------------------
static const TOPIC_FUNC_DATA WatchNewsFuncTable[] = {
	{//TVTOPIC_WATCH_POKE_GET_SUCCESS
		TVTOPIC_Make_Watch_SuccessCapture,
		TVTOPIC_Limit_Watch_Capture,
	},
	{//TVTOPIC_WATCH_POKE_GET_FAILURE
		TVTOPIC_Make_Watch_FailureCapture,
		NULL,
	},
	{//TVTOPIC_WATCH_FISHING
		TVTOPIC_Make_Watch_Fishing,
		TVTOPIC_Limit_Watch_Fishing,
	},
	{//TVTOPIC_WATCH_CHANGE_GROUP
		TVTOPIC_Make_Watch_ChangeGroup,
		NULL,
	},
	{//TVTOPIC_WATCH_SEE_RARE_POKEMON,		///<レアポケモン発見！	3	レアポケモンとのエンカウント後
		NULL,
		NULL,
	},
	{//TVTOPIC_WATCH_GET_HIDE_ITEM,
		TVTOPIC_Make_Watch_HideItem,
		NULL,
	},
	{//TVTOPIC_WATCH_SHOPPING,				///<買い物チェック	4	一度に10個以上の道具を買ったとき
		TVTOPIC_Make_Watch_Shopping,		//★
		NULL,
	},
	{//TVTOPIC_WATCH_BORN_EGG
		TVTOPIC_Make_Watch_HatchEgg,
		TVTOPIC_Limit_Watch_HatchEgg,
	},
	{//TVTOPIC_WATCH_CHECK_SODATEYA,		///<育て屋チェック		育て屋に預けたとき
		NULL,
		NULL,
	},
	{//TVTOPIC_WATCH_CHANGE_NICKNAME
		TVTOPIC_Make_Watch_ChangeName,
		TVTOPIC_Limit_Watch_ChangeName,
	},
	{//TVTOPIC_WATCH_POISON_DEAD,			///<毒でひん死……		フィールドでポケモンが瀕死になったとき
		NULL,
		NULL,
	},
	{//TVTOPIC_WATCH_WIN_LOT,				///<ポケモンくじ当選！		ポケモンくじに当たったとき
		NULL,
		NULL,
	},
	{//TVTOPIC_WATCH_DIG_FOSSIL
		TVTOPIC_Make_Watch_DigFossil,
		TVTOPIC_Limit_Watch_Tankou,
	},
	{//TVTOPIC_WATCH_DENDOU,				///<殿堂入り！	はじめて殿堂入りしたとき
		NULL,
		NULL,
	},
	{//TVTOPIC_WATCH_SAFARI
		TVTOPIC_Make_Watch_Safari,
		TVTOPIC_Limit_Watch_Safari,
	},
	{//TVTOPIC_WATCH_OSU_MESU,				///<オスだけ！メスだけ！	パソコンから抜けたときの手持ちにより
		TVTOPIC_Make_Watch_OsuMesu,			//★
		TVTOPIC_Limit_Watch_OsuMesu,
	},
	{//TVTOPIC_WATCH_KANPOU
		//TVTOPIC_Make_Watch_UseKanpo,		//■
		NULL,								//■プラチナでは生成しない
		TVTOPIC_Limit_Watch_UseKanpo,		//■
	},
	{//TVTOPIC_WATCH_NUTS_HARVEST
		TVTOPIC_Make_Watch_NutsHarvest,
		NULL,
	},
	{//TVTOPIC_WATCH_NUTS_POOR
		TVTOPIC_Make_Watch_NutsPoor,
		NULL,
	},
	{//TVTOPIC_WATCH_MAKE_NEW_GROUP,		///<新グループ誕生！	新しくグループを作ったとき
		TVTOPIC_Make_Watch_MakeNewGroup,
		NULL,
	},
	{//TVTOPIC_WATCH_SET_CUSTOMBALL
		TVTOPIC_Make_Watch_CustomBall,
		TVTOPIC_Limit_Watch_CustomBall,
	},
	{//TVTOPIC_WATCH_GET_FLAG,
		TVTOPIC_Make_Watch_GetFlag,
		TVTOPIC_Limit_Watch_Tankou,
	},
	{//TVTOPIC_WATCH_LOSE_FLAG
		TVTOPIC_Make_Watch_LoseFlag,
		TVTOPIC_Limit_Watch_Tankou,
	},
	{//TVTOPIC_WATCH_RIBBON,				///<リボン		
		NULL,
		NULL,
	},
	{//TVTOPIC_WATCH_VILLA,					///<別荘購入★
		TVTOPIC_Make_Watch_Villa,
		TVTOPIC_Limit_Watch_Villa,
	},
	{//TVTOPIC_WATCH_RECONSTRUCTION,		///<別荘改築★
		TVTOPIC_Make_Watch_Reconstruction,
		TVTOPIC_Limit_Watch_Reconstruction,
	},
};


//============================================================================================
//
//	ギネス型 放送関数テーブル★
//
//============================================================================================

//--------------------------------------------------------------------
/**
 * @brief	ギネス型トピック放送関数テーブル
 */
//--------------------------------------------------------------------
static const TOPIC_FUNC_DATA RecordNewsFuncTable[TVTOPIC_RECORD_MAX - 1] = {
	{//TVTOPIC_RECORD_BTOWER
		TVTOPIC_Make_Record_BTower,
		TVTOPIC_Limit_Record_BTower,
	},
	{//TVTOPIC_RECORD_BFACTORY,		///<ファクトリー連勝記録更新	ファクトリー連勝記録を更新したとき
		NULL,
		NULL,
	},
	{//TVTOPIC_RECORD_OOKISA
		TVTOPIC_Make_Record_Ookisa,
		TVTOPIC_Limit_Record_Ookisa,
	},
	{//TVTOPIC_RECORD_SLOT
		TVTOPIC_Make_Record_Slot,
		NULL,
	},
	{//TVTOPIC_RECORD_RIBBON,			///<リボンコレクター	リボンを一定枚数以上獲得時に作成
		TVTOPIC_Make_Record_Ribbon,		//★
		NULL,
	},
	{//TVTOPIC_RECORD_NUTS_MASTER
		//TVTOPIC_Make_Record_NutsMaster,//■
		NULL,							//■プラチナでは作成しない
		NULL,
	},
	{//TVTOPIC_RECORD_POKE_SEARCHER,	///<揺れ草名人		揺れ草エンカウントを終了したとき連続数が10以上
		NULL,
		NULL,
	},
	{//TVTOPIC_RECORD_TRAP_REMOVER
		TVTOPIC_Make_Record_RemoveTrap,
		TVTOPIC_Limit_Record_Tankou,
	},
	{//TVTOPIC_RECORD_FLAG_GET
		TVTOPIC_Make_Record_GetFlag,
		TVTOPIC_Limit_Record_Tankou,
	},
	{//TVTOPIC_RECORD_BP
		TVTOPIC_Make_Record_BP,			//★
		TVTOPIC_Limit_Record_BP,
	},
	{//TVTOPIC_RECORD_GTS
		TVTOPIC_Make_Record_GTS,		//★
		TVTOPIC_Limit_Record_GTS,
	},
};


//============================================================================================
//
//	質問型 放送関数テーブル★
//
//============================================================================================

//--------------------------------------------------------------------
/**
 * @brief	質問型トピック放送関数テーブル
 */
//--------------------------------------------------------------------
static const TOPIC_FUNC_DATA QandANewsFuncTable[TVTOPIC_INTERVIEW_TYPE_MAX - 1] = {
	{//TVTOPIC_INTERVIEW_GYM_BADGE = 1,///<ジム制覇！	1	ジムのあるシティ	ジムリーダーを倒したあと
		NULL,
		NULL,
	},
	{//TVTOPIC_INTERVIEW_BTOWER,
		TVTOPIC_Make_QandA_BTower,
		TVTOPIC_Limit_QandA_BTower,
	},
	{//TVTOPIC_INTERVIEW_BFACTORY,		///<挑戦！ファクトリー	1	バトルファクトリー	挑戦終了後
		NULL,
		NULL,
	},
	{//TVTOPIC_INTERVIEW_POKEMON,
		TVTOPIC_Make_QandA_Pokemon,
		TVTOPIC_Limit_QandA_Pokemon,
	},
	{//TVTOPIC_INTERVIEW_ITEM,			///<おすすめのどうぐ！	2	フレンドリィショップ	
		NULL,
		NULL,
	},
	{//TVTOPIC_INTERVIEW_POKETCH,
		TVTOPIC_Make_QandA_Poketch,
		NULL,
	},
	{//TVTOPIC_INTERVIEW_CONTEST,
		TVTOPIC_Make_QandA_Contest,
		TVTOPIC_Limit_QandA_Contest,
	},
	{//TVTOPIC_INTERVIEW_TATOE,		///<ポケモンに例えたら？	2	大好きクラブ	
		NULL,
		NULL,
	},
	{//TVTOPIC_INTERVIEW_CLIP,
		TVTOPIC_Make_QandA_Clip,
		TVTOPIC_Limit_QandA_Clip,
	},
	{//TVTOPIC_INTERVIEW_CHARACTOR,
		TVTOPIC_Make_QandA_Character,
		NULL,
	},
	{//TVTOPIC_INTERVIEW_PORUTO,
		TVTOPIC_Make_QandA_Poruto,
		NULL,
	},
	{//TVTOPIC_INTERVIEW_KOUKAN,		///<交換したポケモン	1		
		NULL,
		NULL,
	},
	{//TVTOPIC_INTERVIEW_TUREARUKI,	///<連れ歩きを満喫！		自然公園ゲート	連れ歩き後
		TVTOPIC_Make_QandA_Park,
		NULL,
	},
	{//TVTOPIC_INTERVIEW_STAGE
		TVTOPIC_Make_QandA_Stage,		//★
		TVTOPIC_Limit_QandA_Stage,
	},
	{//TVTOPIC_INTERVIEW_STRANGE1
		TVTOPIC_Make_QandA_Strange1,	//★
		NULL,
	},
	{//TVTOPIC_INTERVIEW_STRANGE2
		TVTOPIC_Make_QandA_Strange2,	//★
		NULL,
	},
	{//TVTOPIC_INTERVIEW_STRANGE3
		TVTOPIC_Make_QandA_Strange3,	//★
		NULL,
	},
	{//TVTOPIC_INTERVIEW_STRANGE4
		TVTOPIC_Make_QandA_Strange4,	//★
		NULL,
	},
	{//TVTOPIC_INTERVIEW_FRIEND
		TVTOPIC_Make_QandA_Friend,		//★
		TVTOPIC_Limit_QandA_Friend,
	},
};


//============================================================================================
//
//	イベント型 放送関数テーブル★
//
//============================================================================================

//--------------------------------------------------------------------
/**
 * @brief	イベント型トピック放送関数テーブル
 */
//--------------------------------------------------------------------
static const TOPIC_FUNC_DATA EventNewsFuncTable[TVTOPIC_EVENT_TYPE_MAX - 1] = {
	{	//TVTOPIC_EVENT_RANDOMGROUP = 1,		///<グループ紹介
		TVTOPIC_Make_Event_Group,
		TVTOPIC_Limit_Event_Group,
	},
	{	//TVTOPIC_EVENT_WEATHER,				///<天気予報
		TVTOPIC_Make_Event_Weather,
		TVTOPIC_Limit_Event_Weather,
	},
	{	//TVTOPIC_EVENT_TOWNGUIDE,			///<シンオウあの街この街
		TVTOPIC_Make_Event_TownGuide,		//★
		NULL,
	},
	{	//TVTOPIC_EVENT_BARGAIN,				///<ショップ安売り情報
		NULL,
		NULL,
	},
	{	//TVTOPIC_EVENT_POKEMON_OUTBREAK,		///<大量発生！
		TVTOPIC_Make_Event_Outbreak,
		TVTOPIC_Limit_Event_Outbreak,
	},
	{	//TVTOPIC_EVENT_POKEMON_ENCOUNT,		///<ポケモン出没情報！
		NULL,
		NULL,
	},
	{	//TVTOPIC_EVENT_POKEMON_AISHOU,			///<ポケモン相性チャンネル
		//TVTOPIC_Make_Event_Aishou,			//■
		NULL,									//■プラチナでは生成しない
		TVTOPIC_Limit_Event_Aishou,
	},
	{	//TVTOPIC_EVENT_POSTMAN,				///<配達員だより
		NULL,
		NULL,
	},
	{	//TVTOPIC_EVENT_NUTS_INFOMATION,		///<きのみじょうほう
		TVTOPIC_Make_Event_NutsInfo,
		TVTOPIC_Limit_Event_NutsInfo,
	},
	{	//TVTOPIC_EVENT_WAZAMACHINE,			///<わざマシンじょうほう
		NULL,
		NULL,
	},
	{	//TVTOPIC_EVENT_POKEMON_ITEM,			///<どうぐを持ってる野生ポケモン
		//TVTOPIC_Make_Event_PokemonItem,		//■
		NULL,									//■プラチナでは生成しない
		TVTOPIC_Limit_Event_PokemonItem,
	},
	{	//TVTOPIC_EVENT_POKEMON_CHARACTOR,	///<せいかく診断
		TVTOPIC_Make_Event_PokemonCharactor,//★
		NULL,
	},
	{	//TVTOPIC_EVENT_SLOT_INFO,			///<スロットの出玉情報
		NULL,
		NULL,
	},
	{	//TVTOPIC_EVENT_TV_SHOPPING,			///<ＴＶショッピング
		NULL,
		NULL,
	},
	{	//TVTOPIC_EVENT_TV_MOVE_POKEMON,		///<移動ポケモン目撃情報！
		TVTOPIC_Make_Event_MovePokemon,			//★
		TVTOPIC_Limit_Event_MovePokemon,
	},
	{	//TVTOPIC_EVENT_TV_TAMAGO_INFO,		///<タマゴ発見報告
		NULL,
		NULL,
	},
	{	//TVTOPIC_EVENT_TV_CLIP_INFO,			///<ポケモンファッションチェック
		TVTOPIC_Make_Event_ClipInfo,
		TVTOPIC_Limit_Event_ClipInfo,
	},
};


//============================================================================================
//
//	バラエティ型 放送関数テーブル★
//
//============================================================================================

//--------------------------------------------------------------------
/**
 * @brief	バラエティ型トピック放送関数テーブル
 */
//--------------------------------------------------------------------
static const TOPIC_FUNC_DATA VarietyNewsFuncTable[TVTOPIC_VARIETY_TYPE_MAX - 1] = {
	{	//TVTOPIC_VARIETY_SEARCH_PARTY = 1,		///<ポケモン捜索隊★
		TVTOPIC_Make_Variety_SearchParty,
		NULL,
	},
	{	//TVTOPIC_VARIETY_LEADER,				///<マキシマム試合中継★
		TVTOPIC_Make_Variety_Leader,
		TVTOPIC_Limit_Variety_Leader,
	},
	{	//TVTOPIC_EVENT_VARIETY_DORAMA1,		///<連続ドラマ★
		TVTOPIC_Make_Variety_Dorama1,
		NULL,
	},
	{	//TVTOPIC_EVENT_VARIETY_DORAMA2,		///<連続ドラマ★
		TVTOPIC_Make_Variety_Dorama2,
		NULL,
	},
	{	//TVTOPIC_EVENT_VARIETY_DORAMA3,		///<音楽番組★
		TVTOPIC_Make_Variety_Dorama3,
		TVTOPIC_Limit_Variety_Dorama3,
	},
	{	//TVTOPIC_EVENT_VARIETY_DORAMA4,		///<連続ドラマ★
		TVTOPIC_Make_Variety_Dorama4,
		NULL,
	},
	{	//TVTOPIC_EVENT_VARIETY_DORAMA5,		///<連続ドラマ★
		TVTOPIC_Make_Variety_Dorama5,
		TVTOPIC_Limit_Variety_Dorama5,
	},
	{	//TVTOPIC_EVENT_VARIETY_DORAMA6,		///<連続ドラマ★
		TVTOPIC_Make_Variety_Dorama6,
		NULL,
	},
};


//============================================================================================
//
//	一日一回クリア
//
//============================================================================================

//----------------------------------------------------------
/**
 * @brief	バトルポイント名人、GTS名人のワーククリア★
 * @param	savedata
 * @return	none
 */
//----------------------------------------------------------
void TVWORK_OneDayTempClear( SAVEDATA* savedata )
{
	TV_WORK * tvwk = SaveData_GetTvWork( savedata );

	//このタイミングでTVデータ作成(08.06.02)
	TVTOPIC_Entry_Record_BP( savedata );
	TVTOPIC_Entry_Record_GTS( savedata );

	TVTOPIC_BPTemp_Init( tvwk );
	TVTOPIC_GTSTemp_Init( tvwk );
	return;
}


