//==============================================================================
/**
 * @file	frontier_tool.c
 * @brief	バトルフロンティア施設で共通で使えるようなツール
 * @author	matsuda
 * @date	2007.03.20(火)
 */
//==============================================================================
#include "common.h"
#include "system/procsys.h"
#include "system/pm_str.h"
#include "poketool/poke_tool.h"
#include "poketool/pokeparty.h"
#include "frontier_tool.h"

#include "itemtool/itemsym.h"

#include "gflib/strbuf_family.h"
#include "system/msgdata.h"
#include "poketool/poke_memo.h"
#include "msgdata\msg.naix"
#include "battle/wazano_def.h"
#include "communication/comm_system.h"
#include "communication/comm_info.h"
#include "communication/comm_wifihistory.h"
#include "savedata/frontier_savedata.h"

#include "battle/trtype_def.h"
#include "field/fieldobj_code.h"
#include "../field/zonedata.h"

#include "wifi/dwc_rap.h"
#include "../fielddata/maptable/zone_id.h"


//==============================================================================
//	データ
//==============================================================================
///タワーに出現するトレーナータイプ←→OBJコード
static const u16 btower_trtype2objcode[][2] = {
	{TRTYPE_BTFIVE1,SEVEN1},	///<五人衆1
	{TRTYPE_BTFIVE2,SEVEN2},	///<五人衆1
	{TRTYPE_BTFIVE3,SEVEN3},	///<五人衆1
	{TRTYPE_BTFIVE4,SEVEN4},	///<五人衆1
	{TRTYPE_BTFIVE5,SEVEN5},	///<五人衆1
	{TRTYPE_TANPAN,	BOY2},	///<たんパンこぞう
	{TRTYPE_MINI,	GIRL1},	///<ミニスカート
	{TRTYPE_SCHOOLB,	BOY1},	///<じゅくがえり
	{TRTYPE_SCHOOLG,	GIRL3},	///<じゅくがえり
	{TRTYPE_PRINCE,	GORGGEOUSM},	///<おぼっちゃま
	{TRTYPE_PRINCESS,	GORGGEOUSW},	///<おじょうさま
	{TRTYPE_CAMPB,	CAMPBOY},	///<キャンプボーイ
	{TRTYPE_PICNICG,	PICNICGIRL},	///<ピクニックガール
	{TRTYPE_UKIWAB,	BABYBOY1},	///<うきわボーイ
	{TRTYPE_UKIWAG,	BABYGIRL1},	///<うきわガール
	{TRTYPE_DAISUKIM,	MIDDLEMAN1},	///<だいすきクラブ
	{TRTYPE_DAISUKIW,	MIDDLEWOMAN1},	///<だいすきクラブ
	{TRTYPE_WAITER,	WAITER},	///<ウエーター
	{TRTYPE_WAITRESS,	WAITRESS},	///<ウエートレス
	{TRTYPE_BREEDERM,	MAN1},	///<ポケモンブリーダー
	{TRTYPE_BREEDERW,	WOMAN1},	///<ポケモンブリーダー
	{TRTYPE_CAMERAMAN,	CAMERAMAN},	///<カメラマン
	{TRTYPE_REPORTER,	REPORTER},	///<レポーター
	{TRTYPE_FARMER,	FARMER},	///<ぼくじょうおじさん
	{TRTYPE_COWGIRL,	COWGIRL},	///<カウガール
	{TRTYPE_CYCLINGM,	CYCLEM},	///<サイクリング♂
	{TRTYPE_CYCLINGW,	CYCLEW},	///<サイクリング♀
	{TRTYPE_KARATE,	FIGHTER},	///<からておう
	{TRTYPE_BATTLEG,	GIRL2},	///<バトルガール
	{TRTYPE_VETERAN,	OLDMAN1},	///<ベテラントレーナー
	{TRTYPE_MADAM,	LADY},	///<マダム
	{TRTYPE_ESPM,	MYSTERY},	///<サイキッカー
	{TRTYPE_ESPW,	MYSTERY},	///<サイキッカー
	{TRTYPE_RANGERM,	MAN3},	///<ポケモンレンジャー
	{TRTYPE_RANGERW,	WOMAN3},	///<ポケモンレンジャー
	{TRTYPE_ELITEM,	MAN3},	///<エリートトレーナー
	{TRTYPE_ELITEW,	WOMAN3},	///<エリートトレーナー
	{TRTYPE_COLDELITEM,	MAN5},	///<エリートトレーナー♂（厚着）
	{TRTYPE_COLDELITEW,	WOMAN5},	///<エリートトレーナー♀（厚着）
	{TRTYPE_DRAGON,	MAN3},	///<ドラゴンつかい
	{TRTYPE_MUSHI,	BOY3},	///<むしとりしょうねん
	{TRTYPE_SHINOBI,	BABYBOY1},	///<にんじゃごっこ
	{TRTYPE_JOGGER,	SPORTSMAN},	///<ジョギング♂
	{TRTYPE_FISHING,	FISHING},	///<つりびと
	{TRTYPE_SAILOR,	SEAMAN},	///<ふなのり
	{TRTYPE_MOUNT,	MOUNT},	///<やまおとこ
	{TRTYPE_ISEKI,	EXPLORE},	///<いせきマニア
	{TRTYPE_GUITARIST,	MAN2},	///<ギタリスト
	{TRTYPE_COLLECTOR,	BIGMAN},	///<ポケモンコレクター
	{TRTYPE_HEADS,	BADMAN},	///<スキンヘッズ
	{TRTYPE_SCIENTIST,	ASSISTANTM},	///<けんきゅういん♂
	{TRTYPE_GENTLE,	GENTLEMAN},	///<ジェントルマン
	{TRTYPE_WORKER,	WORKMAN},	///<さぎょういん
	{TRTYPE_PIERROT,	CLOWN},	///<ピエロ
	{TRTYPE_POLICE,	POLICEMAN},	///<おまわりさん
	{TRTYPE_GAMBLER,	GORGGEOUSM},	///<ギャンブラー
	{TRTYPE_BIRD,	WOMAN3},	///<とりつかい
	{TRTYPE_PARASOL,	AMBRELLA},	///<パラソルおねえさん
	{TRTYPE_SISTER,	WOMAN2},	///<おとなのおねえさん
	{TRTYPE_AROMA,	WOMAN1},	///<アロマなおねえさん
	{TRTYPE_IDOL,	IDOL},	///<アイドル
	{TRTYPE_ARTIST,	ARTIST},	///<げいじゅつか
	{TRTYPE_POKEGIRL,	PIKACHU},	///<ポケモンごっこ♀
};
#define TRTYPE2OBJCODE_MAX	(NELEMS(btower_trtype2objcode))


//==============================================================================
//	プロトタイプ宣言
//==============================================================================
u8 Frontier_PowRndGet(u16 tr_no);
BOOL Frontier_PokemonIndexCreate( B_TOWER_TRAINER_ROM_DATA* trd, const u16 check_pokeno[], const u16 check_itemno[], int check_num, int get_count, u16 get_pokeindex[], int heap_id );
void Frontier_EnemyPokeMake( u8 set_num, u16 tr_index, u16 tr_index2, u16* enemy_poke_index, B_TOWER_POKEMON* enemy_poke, u8* pow_rnd, u32* personal_rnd, u8 flag );


//==============================================================================
//
//	
//
//==============================================================================
//---------------------------------------------------------------------------------------------
/**
 *	バトルタワートレーナーロムデータの読み出し
 *
 * @param[in]	trainer_index	トレーナーナンバー
 * @param[in]	heap_id	メモリ確保するためのヒープID
 *
 * @retval		取得したトレーナーロムデータへのポインタ
 *
 * トレーナーデータが可変長な為、中でメモリ確保
 * 終了時は呼び出し側がメモリ開放する必要があります。
 *
 * ★似た処理 b_tower_fld.c BattleTowerTrainerRomDataGet
 */
//---------------------------------------------------------------------------------------------
B_TOWER_TRAINER_ROM_DATA * Frontier_TrainerRomDataGet(u16 trainer_index,int heap_id,int arcTR)
{
	return	ArchiveDataLoadMalloc(arcTR,trainer_index,heap_id);
}

//---------------------------------------------------------------------------------------------
/**
 *	バトルタワーポケモンロムデータの読み出し
 *
 * @param[in]	prd		読み出したポケモンデータの格納先
 * @param[in]	index	読み出すポケモンデータのインデックス
 *
 * トレーナーデータと違い固定長の為、外側から格納先メモリを渡す
 *
 * ★似た処理 b_tower_fld.c BattleTowerPokemonRomDataGet
 */
//---------------------------------------------------------------------------------------------
void Frontier_PokemonRomDataGet(B_TOWER_POKEMON_ROM_DATA *prd,int index,int arcID)
{
	ArchiveDataLoad(prd,arcID,index);
}

//--------------------------------------------------------------
/**
 * @brief   バトルタワートレーナーロムデータをバッファへ展開
 *
 * @param   bt_trd				トレーナーデータ格納先
 * @param   trainer_index		トレーナーIndex
 * @param   heap_id				ヒープID
 *
 * @retval  トレーナーROMデータへのポインタ
 *
 *	B_TOWER_TRAINER_ROM_DATA型をメモリ確保して返すので、
 *	呼び出し側が明示的に解放すること
 *
 * ★似た処理 b_tower_fld.c RomTrainerDataAlloc
 */
//--------------------------------------------------------------
B_TOWER_TRAINER_ROM_DATA * Frontier_TrainerDataGet(B_TOWER_TRAINER *bt_trd, int trainer_index, int heap_id, int arcTR)
{
	B_TOWER_TRAINER_ROM_DATA	*trd;
	MSGDATA_MANAGER *man = MSGMAN_Create(MSGMAN_TYPE_DIRECT, ARC_MSG, NARC_msg_btdtrname_dat, heap_id);
	STRBUF			*name;

	MI_CpuClear8(bt_trd, sizeof(B_TOWER_TRAINER));
	
	trd = Frontier_TrainerRomDataGet(trainer_index, heap_id, arcTR);

	//トレーナーIDをセット
	bt_trd->player_id=trainer_index;

	//トレーナー出現メッセージ
	bt_trd->appear_word[0] = 0xFFFF;
	bt_trd->appear_word[1] = trainer_index*3;
	//トレーナーデータをセット
	bt_trd->tr_type=trd->tr_type;
	name=MSGMAN_AllocString(man,trainer_index);
	STRBUF_GetStringCode(name,&bt_trd->name[0],BUFLEN_PERSON_NAME);
	STRBUF_Delete(name);

	MSGMAN_Delete(man);

	return trd;
}

//持ちポケモン決定はランダムでしているが無限ループ防止のため、
//ある程度まわしたら、ポケモンの不一致のみをチェックして、
//アイテムを固定で持たせるためのアイテムテーブル
static const u16 FrontierPokemonItem[]={
	ITEM_HIKARINOKONA,
	ITEM_RAMUNOMI,
	ITEM_TABENOKOSI,
	ITEM_SENSEINOTUME,
};
//============================================================================================
/**
 *	@brief	バトルタワーのポケモンパラメータ生成
 *
 *	@param[in/out]	pwd	ポケモンパラメータの展開場所
 *	@param[in]		poke_index	タワーromデータポケモンナンバー
 *	@param[in]		poke_id	ポケモンにセットするid
 *	@param[in]		poke_rnd	ポケモンにセットする個性乱数(0が引き渡されたら関数内で生成)
 *	@param[in]		pow_rnd	ポケモンにセットするpow_rnd値
 *	@param[in]		mem_idx	メンバーindex。何体目か(itemfixをTRUEにした場合固定アイテム取得に使用)
 *	@param[in]		itemfix	TRUEなら固定アイテム。FALSEならromデータのアイテム
 *	@param[in]		heapID	テンポラリメモリを確保するヒープID
 *
 *	@return	personal_rnd:生成されたポケモンの個性乱数値
 *
 * ★似た処理 tower_tool.c BattleTowerPokemonParamMake
 */
//============================================================================================
u32 Frontier_PokemonParamMake(B_TOWER_POKEMON* pwd,
		u16 poke_index,u32 poke_id,u32 poke_rnd,u8 pow_rnd,u8 mem_idx,BOOL itemfix,int heapID,int arcID)
{
	int i;
	int	exp;
	u32	personal_rnd;
	u8	friend;
	B_TOWER_POKEMON_ROM_DATA	prd_s;
	
	MI_CpuClear8(pwd,sizeof(B_TOWER_POKEMON));
	
	//ロムデータロード
	Frontier_PokemonRomDataGet(&prd_s,poke_index,arcID);
	
	//モンスターナンバー
	pwd->mons_no=prd_s.mons_no;
	
	//フォルムナンバー(08.04.04)
	pwd->form_no=prd_s.form_no;

	//装備道具
	if(itemfix){
		//50回以上まわしていたフラグがTUREなら、装備アイテムは固定のものを持たせる
		if(mem_idx >= NELEMS(FrontierPokemonItem)){
			mem_idx %= NELEMS(FrontierPokemonItem);	//一応
		}
		pwd->item_no=FrontierPokemonItem[mem_idx];
	}else{
		//romデータに登録されたアイテムを持たせる
		pwd->item_no=prd_s.item_no;
	}

	//なつき度は255がデフォルト
	friend=255;
	for(i=0;i<WAZA_TEMOTI_MAX;i++){
		pwd->waza[i]=prd_s.waza[i];
		//やつあたりを持っているときは、なつき度を0にする
		if(prd_s.waza[i]==WAZANO_YATUATARI){
			friend=0;
		}
	}

	//IDNo
	pwd->id_no=poke_id;
	//OS_Printf( "性格 = %d\n", prd_s.chr );

	if(poke_rnd == 0){
		//個性乱数
		do{
			personal_rnd=(gf_rand()|gf_rand()<<16);
#if 0
		}while((prd_s.chr!=PokeSeikakuGetRnd(personal_rnd))&&(PokeRareGetPara(poke_id,personal_rnd)==TRUE));
#else
		//プラチナはタワーも修正する(08.03.17)(この関数ではないので注意！)
		//データの性格と一致していない"もしくは"レアの時は、ループを回す
		}while((prd_s.chr!=PokeSeikakuGetRnd(personal_rnd))||(PokeRareGetPara(poke_id,personal_rnd)==TRUE));
#endif

		//OS_Printf( "決定したpersonal_rnd = %d\n", personal_rnd );
		//OS_Printf( "PokeSeikakuGetRnd = %d\n", PokeSeikakuGetRnd(personal_rnd) );
		//OS_Printf( "レアじゃないか = %d\n", PokeRareGetPara(poke_id,personal_rnd) );
		pwd->personal_rnd=personal_rnd;
	}else{
		pwd->personal_rnd = poke_rnd;	//0でなければ引数の値を使用
		personal_rnd = poke_rnd;
	}
	
		
	//パワー乱数(0-31)
	pwd->hp_rnd=pow_rnd;
	pwd->pow_rnd=pow_rnd;
	pwd->def_rnd=pow_rnd;
	pwd->agi_rnd=pow_rnd;
	pwd->spepow_rnd=pow_rnd;
	pwd->spedef_rnd=pow_rnd;
	OS_Printf( "準備\npower_rnd = %d\n\n", pwd->power_rnd );

	//努力値
	exp=0;
	for(i=0;i<6;i++){
		if(prd_s.exp_bit&No2Bit(i)){
			exp++;
		}
	}
	if((PARA_EXP_TOTAL_MAX/exp)>255){
		exp=255;
	}else{
		exp=PARA_EXP_TOTAL_MAX/exp;
	}
	for(i = 0;i < 6;i++){
		if(prd_s.exp_bit&No2Bit(i)){
			pwd->exp[i]=exp;
		}
	}

	//技ポイント
	pwd->pp_count=0;

	//国コード
	pwd->country_code=CasetteLanguage;

	//特性
	i=PokePersonalParaGet(pwd->mons_no,ID_PER_speabi2);
	if(i){
		if(pwd->personal_rnd&1){
			pwd->tokusei=i;
		}else{
			pwd->tokusei=PokePersonalParaGet(pwd->mons_no,ID_PER_speabi1);
		}
	}else{
		pwd->tokusei=PokePersonalParaGet(pwd->mons_no,ID_PER_speabi1);
	}

	//なつき度
	pwd->natuki=friend;

	//ニックネーム
	MSGDAT_MonsNameGet(pwd->mons_no,heapID,&(pwd->nickname[0]));

	return personal_rnd;
}

//--------------------------------------------------------------
/**
 * @brief   バトルタワー用ポケモンデータからPOKEMON_PARAMを生成
 *
 * @param   src			バトルタワー用ポケモンデータ
 * @param   dest		POKEMON_PARAM代入先
 * @param   level		FRONTIER_LEVEL_50 = LV50、FRONTIER_LEVEL_OPEN = LV100、それ以外は渡された値
 *
 * ★似た処理 b_tower_fld.c BtlTower_PokeParaMake
 */
//--------------------------------------------------------------
void Frontier_PokeParaMake(const B_TOWER_POKEMON* src,POKEMON_PARAM* dest, u8 level)
{
	int i;
	u32 power_rnd;						//注意！u32必要
	u8	buf8,waza_pp,poke_level;
	u16	buf16;
	u32	buf32;
	
	PokeParaInit(dest);

	//OS_Printf( "monsno = %d\n", src->mons_no );

	//monsno,level,pow_rnd,idno
	if( level == FRONTIER_LEVEL_50 ){
		poke_level = 50;
	}else if( level == FRONTIER_LEVEL_OPEN ){
		poke_level = 100;
	}else{
		poke_level = level;
	}

	//OS_Printf( "power_rnd = %d\n", PokeParaGet(dest,ID_PARA_power_rnd,NULL) );
	//OS_Printf( "hp_rnd = %d\n", PokeParaGet(dest,ID_PARA_hp_rnd,NULL) );
	//OS_Printf( "pow_rnd = %d\n", PokeParaGet(dest,ID_PARA_pow_rnd,NULL) );
	//OS_Printf( "def_rnd = %d\n", PokeParaGet(dest,ID_PARA_def_rnd,NULL) );
	//OS_Printf( "agi_rnd = %d\n", PokeParaGet(dest,ID_PARA_agi_rnd,NULL) );

#if 0
	//この関数はu32でpower_rnd渡す
	PokeParaSetPowRnd(dest,src->mons_no,poke_level,(src->power_rnd & 0x3FFFFFFF),src->personal_rnd);
#else
	//レアが出ないように変更(07.08.08)
	power_rnd = (src->power_rnd & 0x3FFFFFFF);
	OS_Printf( "\nセット power_rnd = %d\n\n", power_rnd );

	//★この関数はu8(0-31)のpow_rnd渡す(あとでpower_rndに再度Putするようにした)
	PokeParaSet(dest, src->mons_no, poke_level, power_rnd, 
				RND_SET, src->personal_rnd, ID_NO_RARE, 0 );

	//PokeParaSet( dest, src->mons_no, poke_level, 0, RND_SET, src->personal_rnd, ID_NO_RARE, 0 );
	
	//★ここでu32のpower_rndを再セット！
	PokeParaPut( dest, ID_PARA_power_rnd, &power_rnd );

	PokeParaCalc( dest );
#endif

	//OS_Printf( "power_rnd = %d\n", PokeParaGet(dest,ID_PARA_power_rnd,NULL) );
	//OS_Printf( "hp_rnd = %d\n", PokeParaGet(dest,ID_PARA_hp_rnd,NULL) );
	//OS_Printf( "pow_rnd = %d\n", PokeParaGet(dest,ID_PARA_pow_rnd,NULL) );
	//OS_Printf( "def_rnd = %d\n", PokeParaGet(dest,ID_PARA_def_rnd,NULL) );
	//OS_Printf( "agi_rnd = %d\n", PokeParaGet(dest,ID_PARA_agi_rnd,NULL) );

	//form_no
	buf8 = src->form_no;
	PokeParaPut(dest,ID_PARA_form_no,&buf8);
	
	//装備アイテム設定
	PokeParaPut(dest,ID_PARA_item,&src->item_no);
	
	//技設定
	for(i = 0;i < 4;i++){
		buf16 = src->waza[i];
		PokeParaPut(dest,ID_PARA_waza1+i,&buf16);
		buf8 = (src->pp_count >> (i*2))&0x03;
		PokeParaPut(dest,ID_PARA_pp_count1+i,&buf8);

		//pp再設定
		waza_pp = (u8)PokeParaGet(dest,ID_PARA_pp_max1+i,NULL);
		PokeParaPut(dest,ID_PARA_pp1+i,&waza_pp);
	}
	
	//ID設定
	buf32 = src->id_no;	
	PokeParaPut(dest,ID_PARA_id_no,&buf32);

	//経験値設定
	buf8 = src->hp_exp;
	PokeParaPut(dest,ID_PARA_hp_exp,&buf8);
	buf8 = src->pow_exp;
	PokeParaPut(dest,ID_PARA_pow_exp,&buf8);
	buf8 = src->def_exp;
	PokeParaPut(dest,ID_PARA_def_exp,&buf8);
	buf8 = src->agi_exp;
	PokeParaPut(dest,ID_PARA_agi_exp,&buf8);
	buf8 = src->spepow_exp;
	PokeParaPut(dest,ID_PARA_spepow_exp,&buf8);
	buf8 = src->spedef_exp;
	PokeParaPut(dest,ID_PARA_spedef_exp,&buf8);

	//特性設定
	PokeParaPut(dest,ID_PARA_speabino,&src->tokusei);
	
	//なつきど設定
	PokeParaPut(dest,ID_PARA_friend,&src->natuki);
	
	//NGネームフラグをチェック
	if(src->ngname_f){
		//デフォルトネームを展開する
		MSGDATA_MANAGER* man;
		STRBUF* def_name;
	
		man = MSGMAN_Create( MSGMAN_TYPE_DIRECT, ARC_MSG, NARC_msg_monsname_dat, HEAPID_FIELD );
		def_name = MSGMAN_AllocString(man,src->mons_no);
		
		PokeParaPut(dest,ID_PARA_nickname_buf,def_name);

		STRBUF_Delete(def_name);
		MSGMAN_Delete(man);
	}else{
		//ニックネーム
		PokeParaPut(dest,ID_PARA_nickname,src->nickname);
	}

	//カントリーコード
	PokeParaPut(dest,ID_PARA_country_code,&src->country_code);
	//パラメータ再計算
	PokeParaCalc(dest);
}

//--------------------------------------------------------------
/**
 * @brief   トレーナータイプから人物OBJコードを返す
 *
 * @param   tr_type		
 *
 * @retval  
 *
 *
 */
//--------------------------------------------------------------
u16 Frontier_TrType2ObjCode(u8 tr_type)
{
	int i = 0;

	for(i = 0;i < TRTYPE2OBJCODE_MAX;i++){
		if(btower_trtype2objcode[i][0] == tr_type){
			return btower_trtype2objcode[i][1];
		}
	}
	return BOY1;
}

//--------------------------------------------------------------
/**
 * @brief	ポケモン孵化情報を格納
 *
 * @param	wk			FACTORY_SCRWORK型のポインタ
 * @param	poke		POKEMON_PARAM型のポインタ
 *
 * @retval	none
 *
 * 親名とかセットしないと、ステータス画面の表示がうまくいかないのでセット
 */
//--------------------------------------------------------------
void Frontier_BirthInfoSet( SAVEDATA* sv, POKEMON_PARAM* poke );
void Frontier_BirthInfoSet( SAVEDATA* sv, POKEMON_PARAM* poke )
{
	u32 id_no;
	STRBUF* str;
	MSGDATA_MANAGER* msgman;
	int placeID;
	//TRMEMO_SETID setID	= TRMEMO_EGGBIRTH_PLACESET;
	TRMEMO_SETID setID		= TRMEMO_POKE_PLACESET;					//通常取得(捕獲等)
	MYSTATUS* my			= SaveData_GetMyStatus( sv );

	//Frontier_PokeParaMakeでセットされているので退避
	id_no = PokeParaGet( poke, ID_PARA_id_no, NULL );

	PokeParaBirthInfoSet(	poke, SaveData_GetMyStatus(sv),
							//ball,        place,  ground_id,  heapID
							ITEM_MONSUTAABOORU, 0, 0, HEAPID_WORLD );

	placeID	= ZoneData_GetPlaceNameID( ZONE_ID_D32R0301 );			//「バトルファクトリー」固定

	//★ここのmyは自分のデータを渡してしまってよいのか？
	//IDNo,親の性別、親の名前がセットされる
	TrainerMemoSetPP( poke, my, setID, placeID, HEAPID_WORLD );

	msgman = MSGMAN_Create(	MSGMAN_TYPE_NORMAL, ARC_MSG, 
							NARC_msg_factory_oyaname_dat, HEAPID_WORLD );
	str = MSGMAN_AllocString( msgman, 0 );
	PokeParaPut( poke, ID_PARA_oyaname_buf, str );					//「？？？？？」固定
	PokeParaPut( poke, ID_PARA_id_no, &id_no );						//退避しておいたIDNoセット
	STRBUF_Delete(str);
	MSGMAN_Delete( msgman );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	ポケモンパーティにポケモンを追加
 *
 * @param	wk			FACTORY_SCRWORK型のポインタ
 * @param	party		POKEPARTY型のポインタ
 * @param	poke		POKEMON_PARAM型のポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------
void Frontier_PokePartyAdd( SAVEDATA* sv, POKEPARTY* party, POKEMON_PARAM* poke );
void Frontier_PokePartyAdd( SAVEDATA* sv, POKEPARTY* party, POKEMON_PARAM* poke )
{
	Frontier_BirthInfoSet( sv, poke );	//親名とかセット
	PokeParty_Add( party, poke );
	return;
}

#if 0
//--------------------------------------------------------------
/**
 * @brief	友達番号取得
 *
 * @param	wk			FACTORY_SCRWORK型のポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------
int Frontier_GetFriendIndex( u32 record_no );
int Frontier_GetFriendIndex( u32 record_no )
{
	//レコードナンバーによって分岐
	//
	//通信状態をチェックなどが必要かも。
	
	if( record_no < FRID_MAX ){
		return FRONTIER_RECORD_NOT_FRIEND;
	}

	//if( CommIsInitialize() ){
	return mydwc_getFriendIndex();
}
#endif


//==============================================================================
//
//	ポケモンアイコンアニメ
//
//==============================================================================
#include "application/app_tool.h"
#include "poketool/pokeicon.h"

void Frontier_PokeIconAnmChg( CLACT_WORK_PTR awp, u8 anm );
void Frontier_PokeIconPosSet( CLACT_WORK_PTR awp, s16 init_x, s16 init_y, u8 flag );
u8 Frontier_PokeIconAnmNoGet( u16 hp, u16 hp_max );

//--------------------------------------------------------------
/**
 * ポケモンアイコンアニメ切り替え
 *
 * @param	awk		アクターワーク
 * @param	anm		アニメ番号
 *
 * @return	none
 */
//--------------------------------------------------------------
void Frontier_PokeIconAnmChg( CLACT_WORK_PTR awp, u8 anm )
{
	if( CLACT_AnmGet(awp) == anm ){ 
		return; 
	}

	CLACT_AnmFrameSet( awp, 0 );
	CLACT_AnmChg( awp, anm );
	CLACT_AnmFrameChg( awp, FX32_ONE );
	return;
}

//--------------------------------------------------------------
/**
 * ポケモンアイコンアニメ座標セット
 *
 * @param	awk		アクターワーク
 * @param	flag	1=選択しているポケモン
 *
 * @return	none
 */
//--------------------------------------------------------------
void Frontier_PokeIconPosSet( CLACT_WORK_PTR awp, s16 init_x, s16 init_y, u8 flag )
{
	VecFx32 vec;

	vec.x = init_x * FX32_ONE;
	vec.y = init_y * FX32_ONE;
	vec.z = 0;

	if( flag == 1 ){
		if( CLACT_AnmFrameGet(awp) == 0 ){
			vec.y = (init_y - 3)  * FX32_ONE;
		}else{
			vec.y = (init_y + 1)  * FX32_ONE;
		}
	}

	CLACT_SetMatrix( awp, &vec );
	return;
}

//--------------------------------------------------------------
/**
 * ポケモンアイコンアニメNo取得
 *
 * @param	awk		アクターワーク
 * @param	flag	1=選択しているポケモン
 *
 * @return	none
 */
//--------------------------------------------------------------
u8 Frontier_PokeIconAnmNoGet( u16 hp, u16 hp_max )
{
	switch( GetHPGaugeDottoColor(hp,hp_max,48) ){
	case HP_DOTTO_MAX:
		return POKEICON_ANM_HPMAX;
	case HP_DOTTO_GREEN:	// 緑
		return POKEICON_ANM_HPGREEN;
	case HP_DOTTO_YELLOW:	// 黄
		return POKEICON_ANM_HPYERROW;
	case HP_DOTTO_RED:		// 赤
		return POKEICON_ANM_HPRED;
	};

#if 0	//デバック//////////////////////////////////////////////////////////
		if( sys.cont & PAD_BUTTON_L ){
			return POKEICON_ANM_HPMAX;
		}else if( sys.cont & PAD_BUTTON_R ){
			return POKEICON_ANM_HPGREEN;
		}else if( sys.cont & PAD_BUTTON_SELECT ){
			return POKEICON_ANM_HPYERROW;
		}else if( sys.cont & PAD_BUTTON_START ){
			return POKEICON_ANM_HPRED;
		}
#endif	////////////////////////////////////////////////////////////////////

	return POKEICON_ANM_HPMAX;
}


//==============================================================================
//
//	バトルタワートレーナーデータ生成(tower_tool.cから移動)
//
//==============================================================================
void FSRomBattleTowerTrainerDataMake2( B_TOWER_PARTNER_DATA *tr_data, u16 tr_no, int heapID, int arcTR );
void BattleParamTrainerDataSet( BATTLE_PARAM* bp, B_TOWER_TRAINER* bt_trd, int cnt, int client_no, int heapID );
u32 Frontier_PokemonParamCreate( B_TOWER_POKEMON *pwd, u16 poke_index, int position, u8 pow_rnd, u32 personal_rnd, int heap_id, int arcPM );
u32 Frontier_PokemonParamCreateStage( B_TOWER_POKEMON *pwd, u16 poke_index, int position, u8 pow_rnd, u32 personal_rnd, int heap_id );

//------------------------------------------------------------------------------
/**
 *	バトルタワートレーナー"のみ"データ生成（ロムデータをB_TOWER_PARTNER_DATA構造体に展開）
 *
 * @param[in/out]	tr_data		生成するB_TOWER_PARTNAER_DATA構造体
 * @param[in]		tr_no		生成元になるトレーナーID
 *								（NULLだとチェックなし）
 * @param[in]		heapID		ヒープID
 *
 * @retval	none
 */
//------------------------------------------------------------------------------
void FSRomBattleTowerTrainerDataMake2( B_TOWER_PARTNER_DATA *tr_data, u16 tr_no, int heapID, int arcTR )
{
	B_TOWER_TRAINER_ROM_DATA* trd;

	//トレーナーデータセット
	trd = Frontier_TrainerDataGet( &tr_data->bt_trd, tr_no, heapID, arcTR );

	sys_FreeMemoryEz(trd);
	return;
}

//--------------------------------------------------------------
/**
 *	BATTLE_PARAMにトレーナーデータセット
 *
 * @param[in/out]	bp			生成するBATTLE_PARAM構造体
 * @param[in]		tr_data		生成元になるトレーナーデータ
 * @param[in]		cnt			トレーナーの持ちポケモン数
 * @param[in]		client_no	生成するclient_no
 * @param[in]		heapID		ヒープID（POKEMON_PARAMの生成に必要）
 */
//--------------------------------------------------------------
void BattleParamTrainerDataSet( BATTLE_PARAM* bp, B_TOWER_TRAINER* bt_trd, int cnt, int client_no, int heapID )
{
	PMS_DATA		*pd;
	POKEMON_PARAM	*pp;

	//トレーナーIDをセット
	bp->trainer_id[client_no]				= bt_trd->player_id;

	//トレーナーデータをセット
	bp->trainer_data[client_no].tr_type		= bt_trd->tr_type;
	PM_strcpy( &bp->trainer_data[client_no].name[0], &bt_trd->name[0] );

	pd = (PMS_DATA*)&bt_trd->win_word[0];
	bp->trainer_data[client_no].win_word	= *pd;

	pd = (PMS_DATA*)&bt_trd->lose_word[0];
	bp->trainer_data[client_no].lose_word	= *pd;
	return;
}

//--------------------------------------------------------------
/**
 * @brief   ポケモン生成
 *
 * @param   pwd				ポケモンパラメータの展開場所
 * @param   poke_index		ポケモンIndex
 * @param   position		手持ち位置
 * @param   pow_rnd			パワー乱数
 * @param   personal_rnd	個性乱数(0の場合は内部で自動生成)
 * @param   heap_id			ヒープID
 *
 * @retval  生成されたポケモンの個性乱数値
 */
//--------------------------------------------------------------
u32 Frontier_PokemonParamCreate( B_TOWER_POKEMON *pwd, u16 poke_index, int position, u8 pow_rnd, u32 personal_rnd, int heap_id, int arcPM )
{
	u32 id, poke_rnd, set_rnd_no;
	
	id = gf_rand() | (gf_rand() << 16);

	//set_rnd_no = Frontier_PokemonParamMake(pwd, poke_index, id, 0, 
	set_rnd_no = Frontier_PokemonParamMake(pwd, poke_index, id, personal_rnd, 
		pow_rnd, position, FALSE, heap_id, arcPM);
	
	return set_rnd_no;
}

//--------------------------------------------------------------
/**
 * @brief   フロンティアで使用するポケモンを一度に複数匹生成(07.12.04ステージ以外)
 *
 * @param   pwd					ポケモンパラメータ展開場所(set_num分の要素数がある配列である事)
 * @param   range				ポケモンデータ抽出範囲データへのポインタ(パワー乱数取得に使用)
 * @param   poke_index[]		ポケモンIndexの配列へのポインタ
 * @param   personal_rnd[]		個性乱数の配列へのポインタ(NULLの場合は内部で自動生成)
 * @param   ret_personal_rnd[]	個性乱数を内部で自動生成した場合の受け取り場所(不要ならNULL)
 * @param   set_num				何体のポケモンを生成するか
 * @param   heap_id				ヒープID
 */
//--------------------------------------------------------------
void Frontier_PokemonParamCreateAll(B_TOWER_POKEMON *pwd, u16 poke_index[], 
	u8 pow_rnd[], u32 personal_rnd[], u32 ret_personal_rnd[], int set_num, int heap_id, int arcPM)
{
	int i;
	u32 per_rnd;
	u8 power_rnd;
	
	if(personal_rnd == NULL){
		per_rnd = 0;
	}
	
	for(i = 0; i < set_num; i++){
		power_rnd	= (pow_rnd == NULL) ? 0 : pow_rnd[i];
		per_rnd		= (personal_rnd == NULL) ? 0 : personal_rnd[i];
		per_rnd		= Frontier_PokemonParamCreate(	&pwd[i], poke_index[i], i, 
													power_rnd, per_rnd, heap_id, arcPM);
		if( ret_personal_rnd != NULL ){
			ret_personal_rnd[i] = per_rnd;
			OS_Printf( "per_rnd = %d\n", per_rnd );
		}
	}

	return;
}

//---------------------------------------------------------------------------------------------
/**
 *	バトルタワートレーナーの持ちポケモンのパワー乱数を決定する
 *
 * @param	tr_no	トレーナーナンバー
 *
 * @return	パワー乱数
 *
 * src/field/b_tower_fld.cと同じ
 */
//---------------------------------------------------------------------------------------------
u8 Frontier_PowRndGet(u16 tr_no)
{
	u8	pow_rnd;

	if(tr_no<100){
		pow_rnd=(0x1f/8)*1;
	}
	else if(tr_no<120){
		pow_rnd=(0x1f/8)*2;
	}
	else if(tr_no<140){
		pow_rnd=(0x1f/8)*3;
	}
	else if(tr_no<160){
		pow_rnd=(0x1f/8)*4;
	}
	else if(tr_no<180){
		pow_rnd=(0x1f/8)*5;
	}
	else if(tr_no<200){
		pow_rnd=(0x1f/8)*6;
	}
	else if(tr_no<220){
		pow_rnd=(0x1f/8)*7;
	}
	else{
		pow_rnd=0x1f;
	}
	return pow_rnd;
}

//--------------------------------------------------------------
/**
 * @brief   フロンティアで使用するポケモンのIndexを決める
 *			同じポケモン、装備道具にならないようにIndexを取得します
 *
 * @param   trd					B_TOWER_TRAINER_ROM_DATA型のポインタ
 * @param   check_pokeno[]		被り防止でチェックするポケモン番号(Indexではない事に注意)
 * @param   check_itemno[]		被り防止でチェックするアイテム番号
 * @param   check_num			check_pokenoとcheck_itemnoのデータ数
 * @param   get_count			何体のポケモンIndexを取得するか
 * @param   ret_pokeindex[]		決まったポケモンIndexの結果代入先(get_count分の配列数がある事!)
 * @param   heap_id				ヒープID
 *
 * @retval	FALSE	抽選ループが50回以内で終わった
 * @retval	TRUE	抽選ループが50回以内で終わらなかった
 *
 * 被り防止を使用しない場合はcheck_numを0にしてください。(check_pokeno, check_itemnoはNULL)
 */
//--------------------------------------------------------------
BOOL Frontier_PokemonIndexCreate( B_TOWER_TRAINER_ROM_DATA* trd, const u16 check_pokeno[], const u16 check_itemno[], int check_num, int get_count, u16 get_pokeindex[], int heap_id )
{
	int set_count,set_index,loop_count;
	B_TOWER_POKEMON_ROM_DATA prd_d[6];
	int i,poke_index;
	
	GF_ASSERT(get_count <= 6);	//prd_dの配列数を超えていればASSERT
	
	set_count	= 0;
	loop_count	= 0;
	
	OS_Printf( "use_poke_cnt = %d\n", trd->use_poke_cnt );

	while( set_count != get_count ){

		//★ランダム要確認
		poke_index = ( gf_rand() % trd->use_poke_cnt );
		//OS_Printf( "poke_index = %d\n", poke_index );
		//poke_index = btower_rand(wk)%trd->use_poke_cnt;
		
		set_index = trd->use_poke_table[poke_index];
		Frontier_PokemonRomDataGet( &prd_d[set_count], set_index, ARC_PL_BTD_PM );
		OS_Printf( "set_index = %d\n", set_index );
		
		//今まで生成してきたポケモンと被っていないかチェック
		for(i = 0; i < set_count; i++){
			if(prd_d[i].mons_no == prd_d[set_count].mons_no 
					|| prd_d[i].item_no == prd_d[set_count].item_no){
				break;	//被った
			}
		}

		OS_Printf( "i = %d\n", i );
		OS_Printf( "set_count = %d\n", set_count );

		if(i != set_count){
			OS_Printf( "continue\n" );
			continue;
		}
		
		if( loop_count < 50 ){
			//チェック用に渡されているモンスターナンバー、装備アイテムの被りチェック
			for(i = 0; i < check_num; i++){
				if(prd_d[set_count].mons_no == check_pokeno[i] 
						|| prd_d[set_count].item_no ==  check_itemno[i]){
					break;	//被った
				}
			}
			if(i != check_num){
				loop_count++;
				continue;
			}
		}

		get_pokeindex[set_count] = set_index;
		set_count++;
	}
	
	if( loop_count >= 50 ){
		return TRUE;
	}

	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	敵が出す最初のポケモン3(4)匹を決める
 *
 * @param	set_num
 * @param	tr_index
 * @param	tr_index2			ペア
 * @param	enemy_poke_index
 * @param	enemy_poke
 * @param	personal_rnd
 *
 * @return	
 *
 * 以下の順で処理していく
 * レンタル6
 * (被りチェック:レンタル6) 敵3
 * (被りチェック:手持ち3)(被りチェック:前の敵3) 敵3
 * ...
 */
//--------------------------------------------------------------
void Frontier_EnemyPokeMake( u8 set_num, u16 tr_index, u16 tr_index2, u16* enemy_poke_index, B_TOWER_POKEMON* enemy_poke, u8* pow_rnd, u32* personal_rnd, u8 flag )
{
	int i;
	B_TOWER_POKEMON poke;
	u16 poke_check_tbl[4];
	u16 item_check_tbl[4];
	B_TOWER_TRAINER bt_trd;
	B_TOWER_TRAINER_ROM_DATA* trd;
	B_TOWER_POKEMON_ROM_DATA prd_d;

	//ROMからトレーナーデータを確保
	trd = Frontier_TrainerDataGet( &bt_trd, tr_index, HEAPID_WORLD, ARC_PL_BTD_TR );

	///////////////////////////////////////////////////////////////
	//最初は、一人目のトレーナーを被りなしで選出
	//
	//その後、二人目のトレーナーを被りテーブルを代入したあとに選出
	//
	//トレーナーデータを元にしないといけない
	///////////////////////////////////////////////////////////////

	//パワー乱数代入
	for( i=0; i < set_num; i++ ){
		pow_rnd[i] = Frontier_PowRndGet( tr_index );
	}

	//シングルの時
	if( flag == 0  ){

		//敵ポケモンのインデックス取得(被りポケ、アイテム、データ数、取得する数、代入先)
		Frontier_PokemonIndexCreate(trd, poke_check_tbl, item_check_tbl, 0,
									set_num, enemy_poke_index, HEAPID_WORLD );

	//マルチの時
	}else{

		//敵ポケモンのインデックス取得(被りポケ、アイテム、データ数、取得する数、代入先)
		Frontier_PokemonIndexCreate(trd, poke_check_tbl, item_check_tbl, 0,
									(set_num / 2), enemy_poke_index, HEAPID_WORLD );

		//被りデータ作成
		for( i=0; i < (set_num / 2) ;i++ ){
			Frontier_PokemonRomDataGet( &prd_d, enemy_poke_index[i], ARC_PL_BTD_PM );
			poke_check_tbl[i] = prd_d.mons_no;
			item_check_tbl[i] = prd_d.item_no;
		}

		//ROMからトレーナーデータを確保
		sys_FreeMemoryEz( trd );
		trd = Frontier_TrainerDataGet( &bt_trd, tr_index2, HEAPID_WORLD, ARC_PL_BTD_TR );

		//敵ポケモンのインデックス取得(被りポケ、アイテム、データ数、取得する数、代入先)
		Frontier_PokemonIndexCreate(trd, poke_check_tbl, item_check_tbl, (set_num / 2),
									(set_num / 2), &enemy_poke_index[set_num / 2], HEAPID_WORLD );

		//パワー乱数代入
		for( i=0; i < (set_num / 2) ;i++ ){
			pow_rnd[i+(set_num / 2)] = Frontier_PowRndGet( tr_index2 );
		}
	}

	sys_FreeMemoryEz( trd );

	//敵ポケモンを一度に生成(personalなし)
	Frontier_PokemonParamCreateAll(	enemy_poke, enemy_poke_index, 
									pow_rnd, NULL, personal_rnd, set_num, HEAPID_WORLD,
									ARC_PL_BTD_PM );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	パートナーの名前をbufにセット
 *
 * @param	wordset		WORDSET型のポインタ
 * @param	bufID		バッファID
 *
 * @retval	none
 */
//--------------------------------------------------------------
#if 0
void Frontier_PairNameSet( WORDSET* wordset, u16 bufID )
{
	STRBUF* strbuf;
	MYSTATUS* my;						//パートナーのMyStatus

	strbuf = STRBUF_Create( (PERSON_NAME_SIZE + EOM_SIZE), HEAPID_WORLD );

	//パートナーのMyStatusを取得
	my = CommInfoGetMyStatus( 1 - CommGetCurrentID() );

	//名前取得(STRBUFにコピー)
	MyStatus_CopyNameString( my, strbuf );

	WORDSET_RegisterWord(	wordset, bufID, strbuf, 
							MyStatus_GetMySex(my), 0, PM_LANG );

	STRBUF_Delete( strbuf );
	return;
}
#endif

//--------------------------------------------------------------
/**
 * @brief	パートナーの名前をバッファにセット
 *
 * @param	wordset		WORDSET型のポインタ
 * @param	bufID		バッファID
 *
 * @return	none
 */
//--------------------------------------------------------------
void Frontier_PairNameWordSet( WORDSET* wordset, u32 bufID )
{
	WORDSET_RegisterPlayerName( wordset, bufID, 
								CommInfoGetMyStatus( (CommGetCurrentID() ^ 1) ) );
	return;
}

//--------------------------------------------------------------
/**
 * @brief   MYSTATUSから使用するOBJCODEを決定する
 * @param   my_status		MYSTATUSへのポインタ
 * @retval  OBJCODE
 */
//--------------------------------------------------------------
int FrontierTool_MyObjCodeGet(const MYSTATUS *my_status)
{
	u32 my_sex;
	int objcode, rom_code;
	
	my_sex = MyStatus_GetMySex(my_status);
	rom_code = MyStatus_GetRomCode(my_status);
	switch(rom_code){
	case VERSION_PLATINUM:
	case VERSION_GOLD:	//※check 金銀も暫定でプラチナと同じ主人公キャラを出す
	case VERSION_SILVER:
	default:
		objcode = (my_sex == PM_MALE) ? HERO : HEROINE;
		break;
	case POKEMON_DP_ROM_CODE:	//ダイヤ＆パール
		//※check DP用の主人公キャラのOBJCODEを設定する予定
		objcode = (my_sex == PM_MALE) ? DPHERO : DPHEROINE;
		break;
	}
	
	return objcode;
}

//--------------------------------------------------------------
/**
 * @brief   地球儀登録
 *
 * @param   SAVEDATA	savedata
 *
 * @retval  none
 */
//--------------------------------------------------------------
void FrontierTool_WifiHistory( SAVEDATA* savedata )
{
	WIFI_HISTORY* pHistory = SaveData_GetWifiHistory( savedata );
	Comm_WifiHistoryCheck( pHistory );
	return;
}


