//=============================================================================
/**
 * @file	comm_ring_buff.c
 * @bfief	リングバッファの仕組みを管理する関数
 * @author	katsumi ohno
 * @date	05/09/16
 */
//=============================================================================

#include "common.h"
#include "communication/communication.h"
#include "comm_local.h"
#include "comm_ring_buff.h"



static int _ringPos(RingBuffWork* pRing,int i);
static int _backupRingDataRestSize(RingBuffWork* pRing);

//==============================================================================
/**
 * リングバッファ管理構造体初期化
 * @param   pRing        リングバッファ管理ポインタ
 * @param   pDataArea    リングバッファメモリー
 * @param   size         リングバッファメモリーサイズ
 * @retval  none
 */
//==============================================================================
void CommRingInitialize(RingBuffWork* pRing, u8* pDataArea, int size)
{
    pRing->pWork = pDataArea;
    pRing->size = size;
    pRing->startPos = 0;
    pRing->endPos = 0;
    pRing->backupEndPos = 0;
//    pRing->backupStartPos = 0;
#ifdef PM_DEBUG
    pRing->count=0;
#endif
}

#ifdef PM_DEBUG
static void _errdisp(u8* pData, int size)
{
    int i,j;
    
    for(j=0;j < size;){
        for(i = 0;((i < 16) && (j < size));i++,j++){
            GF_ASSERT_Printf("%02x",pData[j]);
        }
        GF_ASSERT_Printf("\n");
    }
}


static void _errring(RingBuffWork* pRing)
{
    int i,j,end = pRing->endPos;
    
    for(j = 0;j < 6; j++){
        for(i = 0 ; i < 16; i++){
            end--;
            GF_ASSERT_Printf("%02x",pRing->pWork[_ringPos(pRing,end)]);
        }
        GF_ASSERT_Printf("\n");
    }

}

#endif


//==============================================================================
/**
 * リングバッファに書き込む
 * @param   pRing        リングバッファ管理ポインタ
 * @param   pDataArea    書き込むデータ
 * @param   size         書き込みサイズ
 * @retval  none
 */
//==============================================================================
void CommRingPuts(RingBuffWork* pRing, u8* pDataArea, int size,int line)
{
    int i,j;

#if PL_T0856_080710_FIX
    if(pRing==NULL){
        return;
    }
#endif //PL_T0856_080710_FIX
    // ここのASSERTに引っかかるということは
    // 処理が遅延しているのに、通信は毎syncくるので
    // ストックするバッファ分まであふれると、ここで止まります。
    // 通信のバッファを増やすか、今の部分の処理を分断するか、対処が必要です。
    // 最終的にはこのエラーがあると通信を切断します。
    
    if(_backupRingDataRestSize(pRing) <= size){
//    if(sys.trg == PAD_BUTTON_SELECT){
#ifdef PM_DEBUG
        int res = _backupRingDataRestSize(pRing);
        GF_ASSERT_Printf("RECV %d CNT %d LINE %d\n",size,pRing->count,line);
        _errdisp(pDataArea,size);
        GF_ASSERT_Printf("s%d %d %d %d %d %d\n", pRing->size,pRing->startPos,pRing->endPos,pRing->backupEndPos,CommGetCurrentID(),res);
        _errring(pRing);
        GF_ASSERT_MSG(0,"RingOVER\n");
#endif
        CommSetError();
        return;
    }
    j = 0;
    for(i = pRing->backupEndPos; i < pRing->backupEndPos + size; i++,j++){
        GF_ASSERT(pDataArea);
        pRing->pWork[_ringPos( pRing, i )] = pDataArea[j];
    }
    pRing->backupEndPos = _ringPos( pRing, i );
#ifdef PM_DEBUG
    pRing->count++;
#endif
}

//==============================================================================
/**
 * リングバッファからデータを得る
 * @param   pRing        リングバッファ管理ポインタ
 * @param   pDataArea    読み込みバッファ
 * @param   size         読み込みバッファサイズ
 * @retval  実際に読み込んだデータ
 */
//==============================================================================
int CommRingGets(RingBuffWork* pRing, u8* pDataArea, int size)
{
    int i,j;

#if PL_T0856_080710_FIX
    if(pRing==NULL){
        return 0;
    }
#endif //PL_T0856_080710_FIX
    i = CommRingChecks(pRing, pDataArea, size);
    pRing->startPos = _ringPos( pRing, pRing->startPos + i);
//    OHNO_PRINT("++++++ バッファからだした %d %d  %d byte\n", pRing->startPos, pRing->endPos, i);
    return i;
}

//==============================================================================
/**
 * リングバッファから1byteデータを得る
 * @param   pRing        リングバッファ管理ポインタ
 * @retval  1byteのデータ リングにデータがないときは0(不定)
 */
//==============================================================================
u8 CommRingGetByte(RingBuffWork* pRing)
{
    u8 byte;

    CommRingGets(pRing, &byte, 1);
    return byte;
}

//==============================================================================
/**
 * リングバッファのデータ検査  読み込むだけで位置を進めない
 * @param   pRing        リングバッファ管理ポインタ
 * @param   pDataArea    読み込みバッファ
 * @param   size         読み込みバッファサイズ
 * @retval  実際に読み込んだデータ
 */
//==============================================================================
int CommRingChecks(RingBuffWork* pRing, u8* pDataArea, int size)
{
    int i,j;

#if PL_T0856_080710_FIX
    if(pRing==NULL){
        return 0;
    }
#endif //PL_T0856_080710_FIX
    j = 0;
    for(i = pRing->startPos; i < pRing->startPos + size; i++,j++){
        if(pRing->endPos == _ringPos( pRing,i )){
            return j;
        }
        pDataArea[j] = pRing->pWork[_ringPos( pRing,i )];
    }
    return j;
}

//==============================================================================
/**
 * リングバッファのデータがいくつ入っているか得る
 * @param   pRing        リングバッファ管理ポインタ
 * @retval  データサイズ
 */
//==============================================================================
int CommRingDataSize(RingBuffWork* pRing)
{
#if PL_T0856_080710_FIX
    if(pRing==NULL){
        return 0;
    }
#endif //PL_T0856_080710_FIX
    if(pRing->startPos > pRing->endPos){
        return (pRing->size + pRing->endPos - pRing->startPos);
    }
    return (pRing->endPos - pRing->startPos);
}

//==============================================================================
/**
 * リングバッファのデータがどのくらいあまっているか検査
 * @param   pRing        リングバッファ管理ポインタ
 * @retval  実際に読み込んだデータ
 */
//==============================================================================
int CommRingDataRestSize(RingBuffWork* pRing)
{
#if PL_T0856_080710_FIX
    if(pRing==NULL){
        return 0;
    }
#endif //PL_T0856_080710_FIX
    return (pRing->size - CommRingDataSize(pRing));
}

//==============================================================================
/**
 * カウンターをバックアップする
 * @param   pRing        リングバッファ管理ポインタ
 * @retval  nono
 */
//==============================================================================
//void CommRingStartPush(RingBuffWork* pRing)
//{
//    pRing->backupStartPos = pRing->startPos;
//}

//==============================================================================
/**
 * カウンターをバックアップする
 * @param   pRing        リングバッファ管理ポインタ
 * @retval  nono
 */
//==============================================================================
//void CommRingStartPop(RingBuffWork* pRing)
//{
//    pRing->startPos = pRing->backupStartPos;
//}

//==============================================================================
/**
 * リングバッファのデータがいくつ入っているか得る
 * @param   pRing        リングバッファ管理ポインタ
 * @retval  データサイズ
 */
//==============================================================================
static int _backupRingDataRestSize(RingBuffWork* pRing)
{
    if(pRing->startPos > pRing->backupEndPos){
        return (pRing->startPos - pRing->backupEndPos);
    }
    return (pRing->size - (pRing->backupEndPos - pRing->startPos));
}

//==============================================================================
/**
 * ringサイズの場所
 * @param   pRing        リングバッファ管理ポインタ
 * @retval  
 */
//==============================================================================
int _ringPos(RingBuffWork* pRing,int i)
{
    return i % pRing->size;

}

//==============================================================================
/**
 * カウンター場所をすりかえる
 * @param   pRing        リングバッファ管理ポインタ
 * @retval  nono
 */
//==============================================================================
void CommRingEndChange(RingBuffWork* pRing)
{
#if PL_T0856_080710_FIX
    if(pRing==NULL){
        return;
    }
#endif //PL_T0856_080710_FIX
    pRing->endPos = pRing->backupEndPos;
#ifdef PM_DEBUG
    pRing->count = 0;
#endif
}

