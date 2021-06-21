//============================================================================================
/**
 * @file	fushigi_data.c
 * @date	2006.04.28
 * @author	tamada / mitsuhara
 * @brief	ふしぎ通信用セーブデータ関連
 */
//============================================================================================

#include "common.h"
#include "savedata/savedata.h"
#include "savedata/fushigi_data.h"

#include "gflib/assert.h"

//============================================================================================
//============================================================================================

extern FUSHIGI_DATA * SaveData_GetFushigiData(SAVEDATA * sv);

#define UNIQ_TYPE			0xEDB88320L
#define FUSHIGI_DATA_NO_USED		0x00000000
#define FUSHIGI_DATA_MAX_EVENT		2048

#define FUSHIGI_MENU_FLAG		(FUSHIGI_DATA_MAX_EVENT - 1)

//------------------------------------------------------------------
/**
 * @brief	ふしぎデータの定義
 */
//------------------------------------------------------------------
struct FUSHIGI_DATA{
  u8 recv_flag[FUSHIGI_DATA_MAX_EVENT / 8];		//256 * 8 = 2048 bit
  GIFT_DELIVERY delivery[GIFT_DELIVERY_MAX];		// 配達員８つ
  GIFT_CARD card[GIFT_CARD_MAX];			// カード情報３つ
};


//============================================================================================
//
//		主にセーブシステムから呼ばれる関数
//
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	セーブデータサイズの取得
 * @return	int		ふしぎセーブデータのサイズ
 */
//------------------------------------------------------------------
int FUSHIGIDATA_GetWorkSize(void)
{
  return sizeof(FUSHIGI_DATA);
}
//------------------------------------------------------------------
/**
 * @brief	セーブデータ初期化
 * @param	fd		ふしぎセーブデータへのポインタ
 */
//------------------------------------------------------------------
void FUSHIGIDATA_Init(FUSHIGI_DATA * fd)
{
#ifdef DEBUG_ONLY_FOR_mituhara
  OS_TPrintf("ふしぎデータ初期化\n");
  // この処理はsaveload_system.cのSVDT_Initで行われているので何もしない
  MI_CpuClearFast(fd, sizeof(FUSHIGI_DATA));
#endif
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FUSHIGIDATA)
	SVLD_SetCrc(GMDATA_ID_FUSHIGIDATA);
#endif //CRC_LOADCHECK
}

//============================================================================================
//
//		利用するために呼ばれるアクセス関数
//
//============================================================================================


//------------------------------------------------------------------
/**
 * @brief	データが有効かどうか
 * @param	gift_type
 * @return	BOOL =TRUE 有効
 */
//------------------------------------------------------------------

static BOOL FUSHIGIDATA_IsIn(u16 gift_type)
{
    if((gift_type > MYSTERYGIFT_TYPE_NONE) &&
       (gift_type < MYSTERYGIFT_TYPE_MAX)){
        return TRUE;
    }
    return FALSE;
}

//------------------------------------------------------------------
/**
 * @brief	配達員データの取得
 * @param	fd		ふしぎセーブデータへのポインタ
 * @param	index		配達員データのインデックス（０オリジン）
 * @return	GIFT_DATA	配達員データへのポインタ
 */
//------------------------------------------------------------------
GIFT_DELIVERY * FUSHIGIDATA_GetDeliData(FUSHIGI_DATA * fd, int index)
{
    if((index >= 0) && (index < GIFT_DELIVERY_MAX)){
        if(FUSHIGIDATA_IsIn(fd->delivery[index].gift_type)){
            // データが有効なのでポインタを返す
            return &fd->delivery[index];
        }
    }
    return (GIFT_DELIVERY *)NULL;
}

//------------------------------------------------------------------
/**
 * @brief	カードデータの取得
 * @param	fd		ふしぎセーブデータへのポインタ
 * @param	index		贈り物データのインデックス（０オリジン）
 * @return	GIFT_CARD	カードデータへのポインタ
 */
//------------------------------------------------------------------
GIFT_CARD *FUSHIGIDATA_GetCardData(FUSHIGI_DATA *fd, int index)
{
    if((index >= 0) && (index < GIFT_CARD_MAX)){
        if(FUSHIGIDATA_IsIn(fd->card[index].gift_type)){
            return &fd->card[index];
        }
    }
    return NULL;
}

//------------------------------------------------------------------
/**
 * @brief	配達員データをセーブデータへ登録
 * @param	fd		ふしぎセーブデータへのポインタ
 * @param	p		データへのポインタ
 * @return	TRUE: セーブできた : FALSE: 空きスロットが無かった
 */
//------------------------------------------------------------------
BOOL FUSHIGIDATA_SetDeliData(FUSHIGI_DATA *fd, const void *p, int link)
{
  int i;
    BOOL bRet=FALSE;

  // セーブできる領域が無ければセーブ失敗
  if(FUSHIGIDATA_CheckDeliDataSpace(fd) == FALSE)	return FALSE;

  // ↓これ以降は容量的にはセーブに成功するはず
  
  for(i = 0; i < GIFT_DELIVERY_MAX; i++){
    if(!FUSHIGIDATA_IsIn(fd->delivery[i].gift_type)){
      MI_CpuCopy8(p, &fd->delivery[i], sizeof(GIFT_DELIVERY));
      fd->delivery[i].link = link;
      bRet = TRUE;
        break;
    }
  }
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FUSHIGIDATA)
	SVLD_SetCrc(GMDATA_ID_FUSHIGIDATA);
#endif //CRC_LOADCHECK
  return bRet;
}

//------------------------------------------------------------------
/**
 * @brief	カードデータをセーブデータへ登録
 * @param	fd		ふしぎセーブデータへのポインタ
 * @param	p		データへのポインタ
 * @return	TRUE: セーブできた : FALSE: 空きスロットが無かった
 */
//------------------------------------------------------------------
BOOL FUSHIGIDATA_SetCardData(FUSHIGI_DATA *fd, const void *p)
{
    int i;
    GIFT_CARD *gc = (GIFT_CARD *)p;
    BOOL bRet = FALSE;

    // セーブできる領域が無ければセーブ失敗
    if(FUSHIGIDATA_CheckCardDataSpace(fd) == FALSE)	return FALSE;
    // 配達員を含むデータの場合は配達員側もチェック
    if(gc->beacon.delivery_flag == TRUE &&
       FUSHIGIDATA_CheckDeliDataSpace(fd) == FALSE)	return FALSE;

  // ↓これ以降は容量的にはセーブに成功するはず
  
  // カードをセーブする
    for(i = 0; i < GIFT_CARD_MAX; i++){
        if(!FUSHIGIDATA_IsIn(fd->card[i].gift_type)){
#ifdef DEBUG_ONLY_FOR_mituhara
            OS_TPrintf("カードをセーブしました [%d]\n", i);
#endif
            MI_CpuCopy8(gc, &fd->card[i], sizeof(GIFT_CARD));

      // 配達員をセーブする
            if(gc->beacon.delivery_flag == TRUE){
                FUSHIGIDATA_SetDeliData(fd, (const void *)&gc->gift_type, i);
            }
            bRet = TRUE;
            break;
        }
    }
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FUSHIGIDATA)
	SVLD_SetCrc(GMDATA_ID_FUSHIGIDATA);
#endif //CRC_LOADCHECK
    return bRet;
}

//------------------------------------------------------------------
/**
 * @brief	配達員データを抹消する
 * @param	fd		ふしぎセーブデータへのポインタ
 * @param	index		配達員データのインデックス
 * @return	TRUE: セーブできた : FALSE: 空きスロットが無かった
 */
//------------------------------------------------------------------
BOOL FUSHIGIDATA_RemoveDeliData(FUSHIGI_DATA *fd, int index)
{
    GF_ASSERT(index < GIFT_DELIVERY_MAX);
    fd->delivery[index].gift_type = MYSTERYGIFT_TYPE_NONE;
    fd->delivery[index].link = 0;
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FUSHIGIDATA)
	SVLD_SetCrc(GMDATA_ID_FUSHIGIDATA);
#endif //CRC_LOADCHECK
    return TRUE;
}

//------------------------------------------------------------------
/**
 * @brief	カードデータを抹消する 配達員も  BITもおとす
 * @param	fd		ふしぎセーブデータへのポインタ
 * @return	TRUE: セーブできた : FALSE: 空きスロットが無かった
 */
//------------------------------------------------------------------
BOOL FUSHIGIDATA_RemoveCardDataPlusBit(FUSHIGI_DATA *fd, int index)
{
    GF_ASSERT(index < GIFT_CARD_MAX);
    fd->card[index].gift_type = MYSTERYGIFT_TYPE_NONE;
    //BITもおとす
    FUSHIGIDATA_ResetEventRecvFlag(fd, fd->card[index].beacon.event_id);
    // リンクされているカードも一緒に抹消
    FUSHIGIDATA_RemoveCardLinkDeli(fd, index);

#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FUSHIGIDATA)
	SVLD_SetCrc(GMDATA_ID_FUSHIGIDATA);
#endif //CRC_LOADCHECK
    return TRUE;
}

//------------------------------------------------------------------
/**
 * @brief	カードデータを抹消する
 * @param	fd		ふしぎセーブデータへのポインタ
 * @return	TRUE: セーブできた : FALSE: 空きスロットが無かった
 */
//------------------------------------------------------------------
BOOL FUSHIGIDATA_RemoveCardData(FUSHIGI_DATA *fd, int index)
{
    GF_ASSERT(index < GIFT_CARD_MAX);
    fd->card[index].gift_type = MYSTERYGIFT_TYPE_NONE;

#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FUSHIGIDATA)
	SVLD_SetCrc(GMDATA_ID_FUSHIGIDATA);
#endif //CRC_LOADCHECK
    return TRUE;
}

//------------------------------------------------------------------
/**
 * @brief	配達員データがセーブできるかチェック
 * @param	fd		ふしぎセーブデータへのポインタ
 * @return	TRUE: 空きがある : FALSE: 空きスロットが無い
 */
//------------------------------------------------------------------
BOOL FUSHIGIDATA_CheckDeliDataSpace(FUSHIGI_DATA *fd)
{
    int i;
    for(i = 0; i < GIFT_DELIVERY_MAX; i++){
        if(!FUSHIGIDATA_IsIn(fd->delivery[i].gift_type)){
            return TRUE;
        }
    }
    return FALSE;
}

//------------------------------------------------------------------
/**
 * @brief	カードデータがセーブできるかチェック
 * @param	fd		ふしぎセーブデータへのポインタ
 * @param	size		データのサイズ
 * @return	TRUE: 空きがある : FALSE: 空きスロットが無い
 */
//------------------------------------------------------------------
BOOL FUSHIGIDATA_CheckCardDataSpace(FUSHIGI_DATA *fd)
{
  int i;
  for(i = 0; i < GIFT_CARD_MAX; i++){
      if(!FUSHIGIDATA_IsIn(fd->card[i].gift_type)){
          return TRUE;
      }
  }
  return FALSE;
}

//------------------------------------------------------------------
/**
 * @brief	贈り物データの存在チェック
 * @param	fd		ふしぎセーブデータへのポインタ
 * @param	index		贈り物データのインデックス（０オリジン）
 * @return	BOOL	TRUEの時、存在する
 */
//------------------------------------------------------------------
BOOL FUSHIGIDATA_IsExistsDelivery(const FUSHIGI_DATA * fd, int index)
{
    GF_ASSERT(index < GIFT_DELIVERY_MAX);
  
    if(FUSHIGIDATA_IsIn(fd->delivery[index].gift_type)){
        return TRUE;
    }
    return FALSE;
}


//------------------------------------------------------------------
/**
 * @brief	カードデータが存在するか返す
 * @param	fd		ふしぎセーブデータへのポインタ
 * @return	BOOL	TRUEの時、存在する
 */
//------------------------------------------------------------------
BOOL FUSHIGIDATA_IsExistsCard(const FUSHIGI_DATA * fd, int index)
{
    GF_ASSERT(index < GIFT_CARD_MAX);

    if(FUSHIGIDATA_IsIn(fd->card[index].gift_type)){
        return TRUE;
    }
    return FALSE;
}


//------------------------------------------------------------------
/**
 * @brief	セーブデータ内にカードデータが存在するか返す
 * @param	NONE
 * @return	NONE
 */
//------------------------------------------------------------------
BOOL FUSHIGIDATA_IsExistsCardAll(const FUSHIGI_DATA *fd)
{
  int i;
  for(i = 0; i < GIFT_CARD_MAX; i++)
    if(FUSHIGIDATA_IsExistsCard(fd, i) == TRUE)
      return TRUE;
  return FALSE;
}


//------------------------------------------------------------------
/**
 * @brief	指定のカードにリンクされている配達員が存在するか
 * @param	※indexは0～2が有効
 * @return	TRUE: 配達員は存在する FALSE: 存在しない
 */
//------------------------------------------------------------------
BOOL FUSHIGIDATA_GetCardLinkDeli(const FUSHIGI_DATA *fd, int index)
{
    int i;

    for(i = 0; i < GIFT_DELIVERY_MAX; i++){
        if(FUSHIGIDATA_IsIn(fd->delivery[i].gift_type)){
            if(fd->delivery[i].link == index){
                return TRUE;
            }
        }
    }
    return FALSE;
}


//------------------------------------------------------------------
/**
 * @brief	指定カードにリンクされている配達員を削除
 * @param	※indexは0～2が有効(無ければ何もしない)
 * @return	NONE
 */
//------------------------------------------------------------------
void FUSHIGIDATA_RemoveCardLinkDeli(const FUSHIGI_DATA *fd, int index)
{
    int i;

    for(i = 0; i < GIFT_DELIVERY_MAX; i++){
        if(FUSHIGIDATA_IsIn(fd->delivery[i].gift_type)){
            if(fd->delivery[i].link == index){
#if (DEBUG_ONLY_FOR_mituhara | DEBUG_ONLY_FOR_ohno)
                OS_TPrintf("カードと一緒に %d 番のおくりものも消しました\n", i );
#endif
                FUSHIGIDATA_RemoveDeliData((FUSHIGI_DATA *)fd, i);
                return;
            }
        }
    }
}

//------------------------------------------------------------------
/**
 * @brief	指定のイベントはすでにもらったか返す
 * @param	fd		ふしぎセーブデータへのポインタ
 * @param	num		イベント番号
 * @return	BOOL	TRUEの時、すでにもらっている
 */
//------------------------------------------------------------------
BOOL FUSHIGIDATA_IsEventRecvFlag(FUSHIGI_DATA * fd, int num)
{
  GF_ASSERT(num < FUSHIGI_DATA_MAX_EVENT);
  return !!(fd->recv_flag[num / 8] & (1 << (num & 7)));
}


//------------------------------------------------------------------
/**
 * @brief	指定のイベントもらったよフラグを立てる
 * @param	fd		ふしぎセーブデータへのポインタ
 * @param	num		イベント番号
 * @return	NONE
 */
//------------------------------------------------------------------
void FUSHIGIDATA_SetEventRecvFlag(FUSHIGI_DATA * fd, int num)
{
  GF_ASSERT(num < FUSHIGI_DATA_MAX_EVENT);
  fd->recv_flag[num / 8] |= (1 << (num & 7));
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FUSHIGIDATA)
	SVLD_SetCrc(GMDATA_ID_FUSHIGIDATA);
#endif //CRC_LOADCHECK
}

//------------------------------------------------------------------
/**
 * @brief	指定のイベントもらったよフラグを落す カードと配達員を一緒に消す時だけ使用する
 * @param	fd		ふしぎセーブデータへのポインタ
 * @param	num		イベント番号
 * @return	NONE
 */
//------------------------------------------------------------------
void FUSHIGIDATA_ResetEventRecvFlag(FUSHIGI_DATA * fd, int num)
{
    u8 notbit = (u8)~(1 << (num & 7));
    GF_ASSERT(num < FUSHIGI_DATA_MAX_EVENT);
    
#if (DEBUG_ONLY_FOR_mituhara | DEBUG_ONLY_FOR_ohno)
    OS_TPrintf("FUSHIGIDATA_ResetEventRecvFlag %x ",  fd->recv_flag[num / 8]);
#endif
  fd->recv_flag[num / 8] &= notbit;
#if (DEBUG_ONLY_FOR_mituhara | DEBUG_ONLY_FOR_ohno)
    OS_TPrintf(" %x \n",  fd->recv_flag[num / 8]);
#endif
    
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FUSHIGIDATA)
	SVLD_SetCrc(GMDATA_ID_FUSHIGIDATA);
#endif //CRC_LOADCHECK
}

//------------------------------------------------------------------
/**
 * @brief	ふしぎなおくりものを表示出来るか？
 * @param	NONE
 * @return	TRUE: 表示　FALSE: 表示しない
 */
//------------------------------------------------------------------
BOOL FUSHIGIDATA_IsFushigiMenu(FUSHIGI_DATA *fd)
{
  return FUSHIGIDATA_IsEventRecvFlag(fd, FUSHIGI_MENU_FLAG);
}


//------------------------------------------------------------------
/**
 * @brief	ふしぎなおくりものの表示フラグをONにする
 * @param	NONE
 * @return	NONE
 */
//------------------------------------------------------------------
void FUSHIGIDATA_SetFushigiMenu(FUSHIGI_DATA *fd)
{
  FUSHIGIDATA_SetEventRecvFlag(fd, FUSHIGI_MENU_FLAG);
}


//============================================================================================
//
//		さらに上位の関数群　主にこちらを呼んでください
//
//============================================================================================

static FUSHIGI_DATA *_fushigi_ptr = NULL;

//------------------------------------------------------------------
/**
 * @brief	これ以下の関数を使うために必要な初期化
 * @param	sv		セーブデータ構造へのポインタ
 * @param	heap_id		ワークを取得するヒープのID
 * @return	NONE
 */
//------------------------------------------------------------------
void FUSHIGIDATA_InitSlot(SAVEDATA * sv, int heap_id)
{
  LOAD_RESULT result;

  if(_fushigi_ptr == NULL){
    _fushigi_ptr = SaveData_GetFushigiData(sv);
  }
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FUSHIGIDATA)
	SVLD_SetCrc(GMDATA_ID_FUSHIGIDATA);
#endif //CRC_LOADCHECK
}

//------------------------------------------------------------------
/**
 * @brief	これ以下の関数を使い終わった後の後始末
 * @param	sv		セーブデータ構造へのポインタ
 * @param	flag		TRUE: セーブする / FALSE: セーブしない
 * @return	NONE
 */
//------------------------------------------------------------------
void FUSHIGIDATA_FinishSlot(SAVEDATA * sv, int flag)
{
  SAVE_RESULT result;
  if(_fushigi_ptr){
#if 0
    if(flag == TRUE)
      SaveData_Save(sv);
#endif
    _fushigi_ptr = NULL;
  }
}
     
//------------------------------------------------------------------
/**
 * @brief	スロットにデータがあるか返す関数
 * @param	NONE
 * @return	-1..データなし: 0以上 データindex番号
*/
//------------------------------------------------------------------
int FUSHIGIDATA_GetSlotData(void)
{
    int i;
    for(i = 0; i < GIFT_DELIVERY_MAX; i++){
        if(FUSHIGIDATA_IsExistsDelivery(_fushigi_ptr, i) == TRUE)
            return i;
    }
    return -1;
}

//------------------------------------------------------------------
/**
 * @brief	スロットにデータがあるか返す関数
 * @param	NONE
 * @return	FALSE..データなし: TRUE データあり
*/
//------------------------------------------------------------------
BOOL FUSHIGIDATA_CheckSlotData(void)
{
    if(FUSHIGIDATA_GetSlotData() == -1){
        return FALSE;
    }
    return TRUE;
}

//------------------------------------------------------------------
/**
 * @brief	指定番号のスロットデータのタイプを返す
 * @param	index		スロットのインデックス番号
 * @return	int		MYSTERYGIFT_TYPE_xxxxx
 *
 * ※インデックス番号はFUSHIGIDATA_GetSlotDataで返された値
*/
//------------------------------------------------------------------
int FUSHIGIDATA_GetSlotType(int index)
{
    GIFT_DELIVERY *dv;

    dv = FUSHIGIDATA_GetDeliData(_fushigi_ptr, index);
    if(dv){
        return (int)dv->gift_type;
    }
    return MYSTERYGIFT_TYPE_NONE;
}


//------------------------------------------------------------------
/**
 * @brief	指定番号のスロット構造体へのポインタを返す
 * @param	index		スロットのインデックス番号
 * @return	GIFT_PRESENT	構造体へのポインタ
 *
 * ※インデックス番号はFUSHIGIDATA_GetSlotDataで返された値
 */
//------------------------------------------------------------------
GIFT_PRESENT *FUSHIGIDATA_GetSlotPtr(int index)
{
    GIFT_DELIVERY *dv;

    dv = FUSHIGIDATA_GetDeliData(_fushigi_ptr, index);
    if(dv){
        return &dv->data;
    }
    return NULL;
}

//------------------------------------------------------------------
/**
 * @brief	指定のスロットを消去する
 * @param	index		スロットのインデックス番号
 * @return	NONE
 *
 * ※インデックス番号はFUSHIGIDATA_GetSlotDataで返された値
 */
//------------------------------------------------------------------
void FUSHIGIDATA_RemoveSlot(int index)
{
  FUSHIGIDATA_RemoveDeliData(_fushigi_ptr, index);
}

//------------------------------------------------------------------


