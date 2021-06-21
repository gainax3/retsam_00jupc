/**
 *	@file	mail.c
 *	@brief	メールセーブデータ　制御
 *	@author	Miyuki Iwasawa
 *	@date	06.02.09
 *
 *	メールセーブデータ反映手順
 *	1,MAIL_SearchNullID()で空き領域を検索、データID取得
 *	2,MailData_CreateWork()でダミーデータ作成用ワークエリアを取得
 *	3,MailData_CreateDataFromSave()などを使ってワークにデータを構築
 *	4,MAIL_AddDataFromWork()と取得済みのデータID,ワークのデータを使って、
 *	　セーブ領域にデータを反映させる
 *	5,ダミーワークエリアを開放する
 */

#include "common.h"
#include "savedata/savedata.h"
#include "savedata/mail.h"
#include "savedata/mail_local.h"
#include "savedata/mystatus.h"
#include "poketool/poke_tool.h"
#include "poketool/pokeparty.h"
#include "poketool/pokeicon.h"
#include "system/pms_data.h"
#include "system/buflen.h"
#include "system/pm_str.h"
#include "poketool/icongra/poke_icon.naix"
#include "poketool/monsno.h"


///ポケモンアイコンの最大CGXID(金銀でアイコンが増えるならここも変える必要がある)
#define ICON_CGXID_MAX		(NARC_poke_icon_poke_icon_519_05_NCGR)

///プラチナ以降で追加されたポケモンのcgx_id変換テーブル
///(金銀でアイコンが増えるならこのテーブルも増やす必要がある)
static const struct{
	u16 normal_cgx_id;		///<フォルム0の時のcgxID
	u16 form_cgx_id;		///<フォルムが変わっている時のcgxID
	u16 monsno;				///<ポケモン番号
	u8 form_no;				///<form_cgx_idはフォルムNoいくつのcgxIDなのか
	u8 padding;			//ダミー
} MailIcon_CgxID_ConvTbl[] = {
	{
		NARC_poke_icon_poke_icon_509_NCGR,
		NARC_poke_icon_poke_icon_509_01_NCGR,
		MONSNO_KIMAIRAN,
		1,
	},
	{
		NARC_poke_icon_poke_icon_516_NCGR,
		NARC_poke_icon_poke_icon_516_01_NCGR,
		MONSNO_EURISU,
		1,
	},
	{
		NARC_poke_icon_poke_icon_519_NCGR,
		NARC_poke_icon_poke_icon_519_01_NCGR,
		MONSNO_PURAZUMA,
		1,
	},
	{
		NARC_poke_icon_poke_icon_519_NCGR,
		NARC_poke_icon_poke_icon_519_02_NCGR,
		MONSNO_PURAZUMA,
		2,
	},
	{
		NARC_poke_icon_poke_icon_519_NCGR,
		NARC_poke_icon_poke_icon_519_03_NCGR,
		MONSNO_PURAZUMA,
		3,
	},
	{
		NARC_poke_icon_poke_icon_519_NCGR,
		NARC_poke_icon_poke_icon_519_04_NCGR,
		MONSNO_PURAZUMA,
		4,
	},
	{
		NARC_poke_icon_poke_icon_519_NCGR,
		NARC_poke_icon_poke_icon_519_05_NCGR,
		MONSNO_PURAZUMA,
		5,
	},
};


/**
 *	@brief	メールデータサイズ取得
 *
 *	＊メールデータ一通のサイズ
 */
int MailData_GetDataWorkSize(void)
{
	return sizeof(MAIL_DATA);
}
/**
 *	@brief	メールデータクリア(初期データセット)
 */
void MailData_Clear(MAIL_DATA* dat)
{
	int i;
	
	dat->writerID = 0;
	dat->sex = PM_MALE;
	dat->region = CasetteLanguage;
	dat->version = CasetteVersion;
	dat->design = MAIL_DESIGN_NULL;

	PM_strclearEOM_(dat->name,BUFLEN_PERSON_NAME);

	for(i = 0;i < MAILDAT_ICONMAX;i++){
		dat->icon[i].dat = MAIL_ICON_NULL;
	}
	dat->form_bit = 0;
	for(i = 0;i < MAILDAT_MSGMAX;i++){
		PMSDAT_Clear(&dat->msg[i]);
	}
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_MAILDATA)
	SVLD_SetCrc(GMDATA_ID_MAILDATA);
#endif //CRC_LOADCHECK
}

/**
 *	@brief	メールデータが有効かどうか返す
 *	@retval	FALSE	無効
 *	@retval	TRUE	有効
 */
BOOL MailData_IsEnable(MAIL_DATA* dat)
{
	if(	dat->design >= MAIL_DESIGN_START &&
		dat->design <= MAIL_DESIGN_END){
		return TRUE;
	}
	return FALSE;
}

/**
 *	@brief	メールデータのワークを取得して返す
 *
 *	＊呼び出し側が責任もって解放すること
 *	
 */
MAIL_DATA* MailData_CreateWork(int heapID)
{
	MAIL_DATA* p;

	p = sys_AllocMemoryLo(heapID,sizeof(MAIL_DATA));
	MailData_Clear(p);

	return p;
}

/**
 *	@brief	メールデータの構造体コピー
 */
void MailData_Copy(MAIL_DATA* src,MAIL_DATA* dest)
{
	MI_CpuCopy8(src,dest,sizeof(MAIL_DATA));
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_MAILDATA)
	SVLD_SetCrc(GMDATA_ID_MAILDATA);
#endif //CRC_LOADCHECK
}

/**
 *	@brief	デザインNo,ポケモンのポジション、セーブデータ指定してメールを新規作成状態に初期化
 *	@param	dat	データを作成するMAIL_DATA構造体型へのポインタ
 *	@param	design_no	メールのイメージNo
 *	@param	pos		メールを持たせるポケモンの手持ち内のポジション
 *	@param	save	セーブデータへのポインタ
 */
void MailData_CreateFromSaveData(MAIL_DATA* dat,u8 design_no,u8 pos,SAVEDATA* save)
{
	u8	i,ct,pal,s;
	u16	monsno;
	u32 icon,egg,form;
	MYSTATUS	*my;
	POKEPARTY	*party;
	POKEMON_PARAM* pp;
	
	MailData_Clear(dat);
	dat->design = design_no;

	//セーブデータから
	party = SaveData_GetTemotiPokemon(save);
	my = SaveData_GetMyStatus(save);

	//自機の名前
	PM_strcpy(dat->name,MyStatus_GetMyName(my));
	//性別
	dat->sex = (u8)MyStatus_GetMySex(my);
	//トレーナーID
	dat->writerID = MyStatus_GetID(my);

	//ポケモンアイコン取得
	dat->form_bit = 0;
	for(i=pos,ct = 0;i < PokeParty_GetPokeCount(party);i++){
		pp = PokeParty_GetMemberPointer(party,i);
		monsno = PokeParaGet(pp,ID_PARA_monsno,NULL);
		egg = PokeParaGet(pp,ID_PARA_tamago_flag,NULL);
		form = PokeParaGet(pp,ID_PARA_form_no,NULL);
		icon = PokeIconCgxArcIndexGetByPP(pp);
//		pal = PokeIconPaletteNumberGet(monsno,egg);
		pal = PokeIconPalNumGet(monsno,form,egg);
		
		dat->icon[ct].cgxID = (u16)icon;
		dat->icon[ct].palID = pal;
		//プラチナ以降で追加されたアイコンの場合のノーマルフォルム変換(フォルム番号は別領域へ退避)
		for(s = 0; s < NELEMS(MailIcon_CgxID_ConvTbl); s++){
			if(MailIcon_CgxID_ConvTbl[s].form_cgx_id == dat->icon[ct].cgxID && 
					MailIcon_CgxID_ConvTbl[s].form_no == form){
				dat->icon[ct].cgxID = MailIcon_CgxID_ConvTbl[s].normal_cgx_id;
				dat->icon[ct].palID = PokeIconPalNumGet( monsno, 0, egg );	//フォルム0のパレット
				dat->form_bit |= MailIcon_CgxID_ConvTbl[s].form_no << (ct*5);
				break;
			}
		}

		ct++;
		if(ct >= MAILDAT_ICONMAX){
			break;
		}
	}
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_MAILDATA)
	SVLD_SetCrc(GMDATA_ID_MAILDATA);
#endif //CRC_LOADCHECK
}

/**
 *	@brief	メールデータ　トレーナーID取得
 */
u32	MailData_GetWriterID(const MAIL_DATA* dat)
{
	return dat->writerID;
}
/**
 *	@brief	メールデータ　トレーナーID適用
 */
void MailData_SetWriterID(MAIL_DATA* dat,u32 id)
{
	dat->writerID = id;
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_MAILDATA)
	SVLD_SetCrc(GMDATA_ID_MAILDATA);
#endif //CRC_LOADCHECK
}

/**
 *	@brief	メールデータ　ライター名取得
 */
STRCODE* MailData_GetWriterName(MAIL_DATA* dat)
{
	return &(dat->name[0]);
}
/**
 *	@brief	メールデータ　ライター名適用
 */
void MailData_SetWriterName(MAIL_DATA* dat,STRCODE* name)
{
	PM_strcpy(dat->name,name);
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_MAILDATA)
	SVLD_SetCrc(GMDATA_ID_MAILDATA);
#endif //CRC_LOADCHECK
}

/**
 *	@brief	メールデータ　ライターの性別を取得
 */
u8	MailData_GetWriterSex(const MAIL_DATA* dat)
{
	return dat->sex;
}
/**
 *	@brief	メールデータ　ライターの性別を適用
 */
void MailData_SetWriterSex(MAIL_DATA* dat,const u8 sex)
{
	dat->sex = sex;
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_MAILDATA)
	SVLD_SetCrc(GMDATA_ID_MAILDATA);
#endif //CRC_LOADCHECK
}

/**
 *	@brief	メールデータ　デザインNo取得
 */
u8	MailData_GetDesignNo(const MAIL_DATA* dat)
{
	return dat->design;
}
/**
 *	@brief	メールデータ　デザインNo適用
 */
void MailData_SetDesignNo(MAIL_DATA* dat,const u8 design)
{
	if(design >= MAIL_DESIGN_MAX){
		return;
	}
	dat->design = design;
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_MAILDATA)
	SVLD_SetCrc(GMDATA_ID_MAILDATA);
#endif //CRC_LOADCHECK
}

/**
 *	@brief	メールデータ　国コード取得
 */
u8	MailData_GetCountryCode(const MAIL_DATA* dat)
{
	return dat->region;
}
/**
 *	@brief	メールデータ　国コード適用
 */
void MailData_SetCountryCode(MAIL_DATA* dat,const u8 code)
{
	dat->region = code;
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_MAILDATA)
	SVLD_SetCrc(GMDATA_ID_MAILDATA);
#endif //CRC_LOADCHECK
}

/**
 *	@brief	メールデータ　カセットバージョン取得
 */
u8	MailData_GetCasetteVersion(const MAIL_DATA* dat)
{
	return dat->version;
}
/**
 *	@brief	メールデータ　カセットバージョン適用
 */
void MailData_SetCasetteVersion(MAIL_DATA* dat,const u8 version)
{
	dat->version = version;
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_MAILDATA)
	SVLD_SetCrc(GMDATA_ID_MAILDATA);
#endif //CRC_LOADCHECK
}

/**
 *	@brief	メールデータ　メールアイコンパラメータの取得(インデックス指定版)
 *
 *	@param	mode	MAIL_ICONPRM_CGX:cgxNoの取得
 *					MAIL_ICONPRM_PAL:pltNoの取得
 *					MAIL_ICONPRM_ALL:u16型(MAIL_ICON型へキャスト可)で双方の値を返す
 *
 *	＊アイコンCgxIDとモンスターNoは同一ではありません。注意！
 */
u16	MailData_GetIconParamByIndex(const MAIL_DATA* dat,u8 index,u8 mode, u16 form_bit)
{
	MAIL_ICON mi;
	int s;
	
	if(index < MAILDAT_ICONMAX){
		mi = dat->icon[index];
		//プラチナ以降で追加されたアイコンのフォルムIndexへ変換
		for(s = 0; s < NELEMS(MailIcon_CgxID_ConvTbl); s++){
			if(MailIcon_CgxID_ConvTbl[s].normal_cgx_id == mi.cgxID && 
					MailIcon_CgxID_ConvTbl[s].form_no == ((form_bit >> (index*5)) & 0x1f)){
				mi.cgxID = MailIcon_CgxID_ConvTbl[s].form_cgx_id;
				mi.palID = PokeIconPalNumGet( 
					MailIcon_CgxID_ConvTbl[s].monsno, MailIcon_CgxID_ConvTbl[s].form_no, 0 );
				break;
			}
		}
		if(mi.cgxID > ICON_CGXID_MAX){
			mi.cgxID = NARC_poke_icon_poke_icon_000_NCGR;
			mi.palID = 0;
		}
		switch(mode){
		case MAIL_ICONPRM_CGX:
			return mi.cgxID;
		case MAIL_ICONPRM_PAL:
			return mi.palID;
		case MAIL_ICONPRM_ALL:
		default:
			return mi.dat;
		}
	}else{
		return 0;
	}
}

/**
 *	@brief	メールデータ　form_bit取得
 */
u16	MailData_GetFormBit(const MAIL_DATA* dat)
{
	return dat->form_bit;
}

/**
 *	@brief	メールデータ　簡易文取得(インデックス指定版)
 */
PMS_DATA*	MailData_GetMsgByIndex(MAIL_DATA* dat,u8 index)
{
	if(index < MAILDAT_MSGMAX){
		return &(dat->msg[index]);
	}else{
		return &(dat->msg[0]);
	}
}
/**
 *	@brief	メールデータ　簡易文セット(インデックス指定版)
 */
void MailData_SetMsgByIndex(MAIL_DATA* dat,PMS_DATA* pms,u8 index)
{
	if(index >= MAILDAT_MSGMAX){
		return;
	}
	PMSDAT_Copy(&dat->msg[index],pms);
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_MAILDATA)
	SVLD_SetCrc(GMDATA_ID_MAILDATA);
#endif //CRC_LOADCHECK
}

/**
 *	@brief	メールデータ　簡易文文字列取得(インデックス指定)
 *
 *	@param	dat	MAIL_DATA*
 *	@param	index	簡易文インデックス
 *	@param	buf		取得した文字列ポインタの格納場所
 *
 *	@retval	FALSE	文字列の取得に失敗(または簡易文が有効なデータではない)
 *	
 *	@li	bufに対して内部でメモリを確保しているので、呼び出し側が明示的に解放すること
 *	@li	FALSEが返った場合、bufはNULLクリアされる
 */
BOOL MailData_GetMsgStrByIndex(const MAIL_DATA* dat,u8 index,STRBUF* buf,int heapID)
{
	if(index >= MAILDAT_MSGMAX){
		buf = NULL;
		return FALSE;
	}
	
	if(!PMSDAT_IsEnabled(&dat->msg[index])){
		buf = NULL;
		return FALSE;
	}

	buf = PMSDAT_ToString(&dat->msg[index],heapID);
	return TRUE;
}

//=================================================================
//
//=================================================================
//ローカル関数プロトタイプ
static int mail_GetNullData(MAIL_DATA* array,int num);
static int mail_GetNumEnable(MAIL_DATA* array,int num);
static MAIL_DATA* mail_GetAddress(MAIL_BLOCK* bloc,MAILBLOCK_ID blockID,int dataID);

/**
 *	@brief	セーブデータブロックへのポインタを取得
 */
MAIL_BLOCK* SaveData_GetMailBlock(SAVEDATA* sv)
{
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_MAILDATA)
	SVLD_CheckCrc(GMDATA_ID_MAILDATA);
#endif //CRC_LOADCHECK
	return SaveData_Get(sv,GMDATA_ID_MAILDATA);
}

/**
 *	@brief	メールセーブデータブロックサイズ取得
 *
 *	＊メールデータ一通のサイズではないので注意！
 */
int MAIL_GetBlockWorkSize(void)
{
	return sizeof(MAIL_DATA)*MAIL_STOCK_MAX;
}
/**
 *	@brief	メールセーブデータブロック初期化
 */
void MAIL_Init(MAIL_BLOCK* dat)
{
	int i = 0;

#if 0
	for(i = 0;i < MAIL_STOCK_TEMOTI;i++){
		MailData_Clear(&dat->temoti[i]);
	}
	for(i = 0;i < MAIL_STOCK_EXTRADE;i++){
		MailData_Clear(&dat->extrade[i]);
	}
	for(i = 0;i < MAIL_STOCK_SODATEYA;i++){
		MailData_Clear(&dat->sodateya[i]);
	}
#endif
	for(i = 0;i < MAIL_STOCK_PASOCOM;i++){
		MailData_Clear(&dat->paso[i]);
	}
}

/**
 *	@brief	空いているメールデータIDを取得
 *
 *	@param	id 追加したいメールブロックID
 *
 *	@return	int	データを追加できる場合は参照ID
 *				追加できない場合はマイナス値が返る
 */
int MAIL_SearchNullID(MAIL_BLOCK* block,MAILBLOCK_ID id)
{
	switch(id){
#if 0
	case MAILBLOCK_TEMOTI:
		return mail_GetNullData(block->temoti,MAIL_STOCK_TEMOTI);
	case MAILBLOCK_EXTRADE:
		return mail_GetNullData(block->extrade,MAIL_STOCK_EXTRADE);
	case MAILBLOCK_SODATEYA:
		return mail_GetNullData(block->sodateya,MAIL_STOCK_SODATEYA);
#endif
	case MAILBLOCK_PASOCOM:
		return mail_GetNullData(block->paso,MAIL_STOCK_PASOCOM);
	default:
		return MAILDATA_NULLID;
	}
	return MAILDATA_NULLID;
}

/**
 *	@brief	メールデータを削除
 *
 *	@param	blockID	ブロックのID
 *	@param	dataID	データID
 */
void MAIL_DelMailData(MAIL_BLOCK* block,MAILBLOCK_ID blockID,int dataID)
{
	MAIL_DATA* pd = NULL;
	
	pd = mail_GetAddress(block,blockID,dataID);
	if(pd != NULL){
		MailData_Clear(pd);
	}
}

/**
 *	@brief	メールデータをセーブブロックに追加
 *
 *	＊引き渡したMAIL_DATA構造体型データの中身がセーブデータに反映されるので
 *	　おかしなデータを入れないように注意！
 */
void MAIL_AddMailFormWork(MAIL_BLOCK* block,MAILBLOCK_ID blockID,int dataID,MAIL_DATA* src)
{
	MAIL_DATA* pd = NULL;
	
	pd = mail_GetAddress(block,blockID,dataID);
	if(pd != NULL){
		MailData_Copy(src,pd);
	}
}

/**
 *	@brief	指定ブロックに有効データがいくつあるか返す
 */
int MAIL_GetEnableDataNum(MAIL_BLOCK* block,MAILBLOCK_ID blockID)
{
	switch(blockID){
#if 0
	case MAILBLOCK_TEMOTI:
		return mail_GetNumEnable(block->temoti,MAIL_STOCK_TEMOTI);
	case MAILBLOCK_EXTRADE:
		return mail_GetNumEnable(block->extrade,MAIL_STOCK_EXTRADE);
	case MAILBLOCK_SODATEYA:
		return mail_GetNumEnable(block->sodateya,MAIL_STOCK_SODATEYA);
#endif
	case MAILBLOCK_PASOCOM:
		return mail_GetNumEnable(block->paso,MAIL_STOCK_PASOCOM);
	default:
		return 0;
	}
	return 0;

}

/**
 *	@brief	メールデータのコピーを取得
 *
 *	＊内部でメモリを確保するので、呼び出し側が責任持って領域を開放すること
 *	＊無効IDを指定した場合、空データを返す
 */
MAIL_DATA* MAIL_AllocMailData(MAIL_BLOCK* block,MAILBLOCK_ID blockID,int dataID,int heapID)
{
	MAIL_DATA* src = NULL;
	MAIL_DATA* dest;
	
	src = mail_GetAddress(block,blockID,dataID);
	dest = MailData_CreateWork(heapID);
	if(src != NULL){
		MailData_Copy(src,dest);
	}
	return dest;
}

/**
 *	@brief	メールデータのコピーを取得
 *
 *	＊あらかじめ確保したMAIL_DATA型メモリにセーブデータをコピーして取得
 */
void MAIL_GetMailData(MAIL_BLOCK* block,MAILBLOCK_ID blockID,int dataID,MAIL_DATA* dest)
{
	MAIL_DATA* src = NULL;
	
	src = mail_GetAddress(block,blockID,dataID);
	if(src == NULL){
		MailData_Clear(dest);
	}else{
		MailData_Copy(src,dest);
	}
}

//=================================================================
//
//=================================================================

/**
 *	@brief	手持ちブロックの空きを探して返す
 *
 *	@param	array	メールデータ配列へのポインタ
 *	@param	num		第一引数で渡した配列の要素数
 */
static int mail_GetNullData(MAIL_DATA* array,int num)
{
	int i = 0;

	for(i = 0;i < num;i++){
		if(!MailData_IsEnable(&array[i])){
			return i;
		}
	}
	return MAILDATA_NULLID;
}

/**
 *	@brief	有効データの数を探して返す
 */
static int mail_GetNumEnable(MAIL_DATA* array,int num)
{
	int i = 0;
	int ct = 0;
	
	for(i = 0;i < num;i++){
		if(MailData_IsEnable(&array[i])){
			ct++;
		}
	}
	return ct;
}

/**
 *	@brief	指定IDを持つブロック内のメールデータへのポインタを返す
 */
static MAIL_DATA* mail_GetAddress(MAIL_BLOCK* block,MAILBLOCK_ID blockID,int dataID)
{
	MAIL_DATA* pd = NULL;
	
	switch(blockID){
#if 0
	case MAILBLOCK_TEMOTI:
		if(dataID < MAIL_STOCK_TEMOTI){
			pd = &(block->temoti[dataID]);
		}
		break;
	case MAILBLOCK_EXTRADE:
		if(dataID < MAIL_STOCK_EXTRADE){
			pd = &(block->extrade[dataID]);
		}
		break;
	case MAILBLOCK_SODATEYA:
		if(dataID >= MAIL_STOCK_SODATEYA){
			pd = &(block->sodateya[dataID]);
		}
		break;
#endif
	case MAILBLOCK_PASOCOM:
		if(dataID < MAIL_STOCK_PASOCOM){
			pd = &(block->paso[dataID]);
		}
		break;
	default:
		break;
	}
	return pd;
}
