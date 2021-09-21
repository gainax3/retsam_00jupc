/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - libraries
  File:     mb_block.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mb_block.c,v $
  Revision 1.4  2006/01/18 02:11:29  kitase_hirotake
  do-indent

  Revision 1.3  2005/02/28 05:26:24  yosizaki
  do-indent.

  Revision 1.2  2005/02/18 11:56:03  yosizaki
  fix around hidden warnings.

  Revision 1.1  2004/11/22 12:38:39  takano_makoto
  Initial update.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include "mb_common.h"
#include "mb_block.h"
#include "mb_private.h"


//============================================================================
//  関数プロトタイプ宣言
//============================================================================
static u8 *MBi_ReceiveRequestDataPiece(const MBCommChildBlockHeader * hdr, u16 child);
static BOOL IsGetAllRequestData(u16 child);


//============================================================================
//  変数宣言
//============================================================================

// 分割リクエストの断片サイズ、断片数
static struct
{
    int     size;
    int     num;
    int     bufSize;
}
req_data_piece;

// 子機用
// 分割リクエストの送信インデックス
static u8 req_data_piece_idx = 0;

// 親機用
// 分割リクエストの受信バッファ
static MbRequestPieceBuf *req_buf;


//============================================================================
//  関数定義
//============================================================================

/*---------------------------------------------------------------------------*
  Name:         MBi_SetChildMPMaxSize
  
  Description:  子機の送信データサイズを設定して、分割リクエストサイズを計算
                します。
  
  Arguments:    childMaxSize 子機送信サイズ
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void MBi_SetChildMPMaxSize(u16 childMaxSize)
{
    req_data_piece.size = MB_COMM_CALC_REQ_DATA_PIECE_SIZE(childMaxSize);
    req_data_piece.num = MB_COMM_CALC_REQ_DATA_PIECE_NUM(childMaxSize);
    req_data_piece.bufSize = MB_COMM_CALC_REQ_DATA_BUF_SIZE(childMaxSize);
}


/*---------------------------------------------------------------------------*
  Name:         MBi_SetParentPieceBuffer

  Description:  親機の分割リクエスト受信用バッファを設定します。
  
  Arguments:    buf 受信用バッファへのポインタ
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void MBi_SetParentPieceBuffer(MbRequestPieceBuf * buf)
{
    req_buf = buf;
    MI_CpuClear8(req_buf, sizeof(MbRequestPieceBuf));
}


/*---------------------------------------------------------------------------*
  Name:         MBi_ClearParentPieceBuffer

  Description:  分割リクエスト受信用バッファをクリアします。
  
  Arguments:    child_aid バッファをクリアする子機AID
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void MBi_ClearParentPieceBuffer(u16 child_aid)
{
    if (req_buf == NULL)
    {
        return;
    }

    MI_CpuClear8(req_buf->data_buf[child_aid - 1],
                 MB_COMM_CALC_REQ_DATA_BUF_SIZE(req_data_piece.bufSize));
    req_buf->data_bmp[child_aid - 1] = 0;
}


/*---------------------------------------------------------------------------*
  Name:         MBi_MakeParentSendBuffer

  Description:  親機の送信ヘッダ情報から、実際に送信するデータを構築します。

  Arguments:    hdr     親機送信ヘッダへのポインタ
                sendbuf 送信データを生成するバッファへのポインタ

  Returns:      送信バッファへのポインタ
 *---------------------------------------------------------------------------*/
u8     *MBi_MakeParentSendBuffer(const MBCommParentBlockHeader * hdr, u8 *sendbuf)
{
    u8     *ptr = sendbuf;

    *ptr++ = hdr->type;

    switch (hdr->type)
    {
    case MB_COMM_TYPE_PARENT_SENDSTART:        //  1
        break;
    case MB_COMM_TYPE_PARENT_KICKREQ: //  2
        break;
    case MB_COMM_TYPE_PARENT_DL_FILEINFO:      //  3
        break;
    case MB_COMM_TYPE_PARENT_DATA:    //  4
        *ptr++ = (u8)(0x00ff & hdr->fid);       // Lo
        *ptr++ = (u8)((0xff00 & hdr->fid) >> 8);        // Hi
        *ptr++ = (u8)(0x00ff & hdr->seqno);     // Lo
        *ptr++ = (u8)((0xff00 & hdr->seqno) >> 8);      // Hi
        break;
    case MB_COMM_TYPE_PARENT_BOOTREQ: //  5
        break;
    case MB_COMM_TYPE_PARENT_MEMBER_FULL:      //  6
        break;
    default:
        return NULL;
    }

    return ptr;
}


/*---------------------------------------------------------------------------*
  Name:         MBi_SetRecvBufferFromChild

  Description:  子機から受信したパケットからヘッダ部を取得し、データ部への
                ポインタを返します。

  Arguments:    hdr     ヘッダを取得するための変数へのポインタ
                recvbuf 受信バッファへのポインタ

  Returns:      データ部へのポインタ
 *---------------------------------------------------------------------------*/
u8     *MBi_SetRecvBufferFromChild(const u8 *recvbuf, MBCommChildBlockHeader * hdr, u16 child_id)
{
    u8     *ptr = (u8 *)recvbuf;

    hdr->type = *ptr++;

    switch (hdr->type)
    {
    case MB_COMM_TYPE_CHILD_FILEREQ:
        // 断片化データを復元
        if (IsGetAllRequestData(child_id))
        {
            return (u8 *)req_buf->data_buf[child_id - 1];
        }

        hdr->req_data.piece = *ptr++;
        if (hdr->req_data.piece > req_data_piece.num)
        {
            return NULL;               // error
        }
        MI_CpuCopy8(ptr, hdr->req_data.data, (u32)req_data_piece.size);
        ptr = MBi_ReceiveRequestDataPiece(hdr, child_id);
        break;
    case MB_COMM_TYPE_CHILD_ACCEPT_FILEINFO:
        hdr->data.req = (u16)(0x00ff & (*ptr++));
        hdr->data.req |= (((u16)(*ptr++) << 8) & 0xff00);
        break;
    case MB_COMM_TYPE_CHILD_CONTINUE:
        hdr->data.req = (u16)(0x00ff & (*ptr++));
        hdr->data.req |= (((u16)(*ptr++) << 8) & 0xff00);
        MI_CpuCopy8(ptr, hdr->data.reserved, (u32)req_data_piece.size);
        ptr += req_data_piece.size;
        break;
    default:
        return NULL;
    }

    return ptr;
}


/*---------------------------------------------------------------------------*
  Name:         MBi_ReceiveRequestDataPiece

  Description:  子機から受信した分割リクエストデータから、全体を構築します。

  Arguments:    hdr     ヘッダを取得するための変数へのポインタ
                recvbuf 受信バッファへのポインタ

  Returns:      分割リクエストデータがすべて揃っていた場合はデータへのポインタ
                まだ揃っていない場合は NULL
 *---------------------------------------------------------------------------*/
static u8 *MBi_ReceiveRequestDataPiece(const MBCommChildBlockHeader * hdr, u16 child)
{
    u8      piece;
    u8     *ptr;

    if (req_buf == NULL)
    {
        return NULL;
    }

    piece = hdr->req_data.piece;

    if (piece > req_data_piece.num)
    {
        return NULL;
    }

    ptr = ((u8 *)req_buf->data_buf[child - 1]) + (piece * req_data_piece.size);

    MI_CpuCopy8(&hdr->req_data.data[0], ptr, (u32)req_data_piece.size);

    req_buf->data_bmp[child - 1] |= (1 << piece);

    MB_DEBUG_OUTPUT(" %02x %02x %02x %02x %02x %02x\n", ptr[0], ptr[1], ptr[2], ptr[3], ptr[4],
                    ptr[5]);

    if (IsGetAllRequestData(child))
    {
        return (u8 *)req_buf->data_buf[child - 1];
    }

    return NULL;
}


/*---------------------------------------------------------------------------*
  Name:         

  Description:  子機から受信した分割リクエストデータから、全体を構築します。

  Arguments:    hdr     ヘッダを取得するための変数へのポインタ
                recvbuf 受信バッファへのポインタ

  Returns:      分割リクエストデータがすべて揃っていた場合は TRUE
                まだ揃っていない場合は FALSE
 *---------------------------------------------------------------------------*/
static BOOL IsGetAllRequestData(u16 child)
{
    u16     i;

    /* Pieceが集まったかを判定 */
    for (i = 0; i < req_data_piece.num; i++)
    {
        if ((req_buf->data_bmp[child - 1] & (1 << i)) == 0)
        {
            return FALSE;
        }
    }

    return TRUE;
}



/*---------------------------------------------------------------------------*
  Name:         MBi_MakeChildSendBuffer

  Description:  子機の送信ヘッダ情報から、実際に送信するデータを構築します。

  Arguments:    hdr     子機送信ヘッダへのポインタ
                sendbuf 送信データを生成するバッファへのポインタ

  Returns:      送信バッファへのポインタ
 *---------------------------------------------------------------------------*/
u8     *MBi_MakeChildSendBuffer(const MBCommChildBlockHeader * hdr, u8 *sendbuf)
{
    u8     *pbuf = sendbuf;

    *pbuf++ = hdr->type;
    switch (hdr->type)
    {
    case MB_COMM_TYPE_CHILD_FILEREQ:
        *pbuf++ = hdr->req_data.piece;
        if (hdr->req_data.piece > req_data_piece.num)
        {
            return NULL;               // error
        }

        MI_CpuCopy8((void *)(hdr->req_data.data), (void *)pbuf, (u32)req_data_piece.size);
        pbuf += req_data_piece.size;
        break;
    case MB_COMM_TYPE_CHILD_ACCEPT_FILEINFO:
        break;
    case MB_COMM_TYPE_CHILD_CONTINUE:
        *pbuf++ = (u8)(0x00ff & hdr->data.req); // Lo
        *pbuf++ = (u8)((0xff00 & hdr->data.req) >> 8);  // Hi
        MI_CpuCopy8((void *)(hdr->data.reserved), (void *)pbuf, (u32)req_data_piece.size);
        pbuf += req_data_piece.size;
        break;
    case MB_COMM_TYPE_CHILD_STOPREQ:
        break;
    case MB_COMM_TYPE_CHILD_BOOTREQ_ACCEPTED:
        break;
    default:
        return NULL;
    }

    return pbuf;
}


/*---------------------------------------------------------------------------*
  Name:         MBi_SendRequestDataPiece

  Description:  子機から送信するリクエストデータを分割します。

  Arguments:    pData     分割データを取得するポインタ
                pReq      分割するリクエストデータ

  Returns:      取得した分割データのインデックス値
 *---------------------------------------------------------------------------*/
u8 MBi_SendRequestDataPiece(u8 *pData, const MBCommRequestData *pReq)
{
    const u8 *ptr = (u8 *)pReq;

    /* 送信Pieceを決定 */
    req_data_piece_idx = (u8)((req_data_piece_idx + 1) % req_data_piece.num);
    MB_DEBUG_OUTPUT("req_data piece : %d\n", req_data_piece_idx);

    // 送信バッファにデータをコピー
    MI_CpuCopy8((void *)&ptr[req_data_piece_idx * req_data_piece.size],
                pData, (u32)req_data_piece.size);

    MB_DEBUG_OUTPUT(" %02x %02x %02x %02x %02x %02x\n", pData[0], pData[1], pData[2], pData[3],
                    pData[4], pData[5]);

    return req_data_piece_idx;
}



/*---------------------------------------------------------------------------*
  Name:         MBi_SetRecvBufferFromParent

  Description:  親機から受信したパケットからヘッダ部を取得し、データ部への
                ポインタを返します。

  Arguments:    hdr     ヘッダを取得するための変数へのポインタ
                recvbuf 受信バッファへのポインタ

  Returns:      データ部へのポインタ
 *---------------------------------------------------------------------------*/
u8     *MBi_SetRecvBufferFromParent(MBCommParentBlockHeader * hdr, const u8 *recvbuf)
{

    hdr->type = *recvbuf++;

    switch (hdr->type)
    {
    case MB_COMM_TYPE_PARENT_SENDSTART:
        break;
    case MB_COMM_TYPE_PARENT_KICKREQ:
        break;
    case MB_COMM_TYPE_PARENT_DL_FILEINFO:
        break;
    case MB_COMM_TYPE_PARENT_DATA:
        hdr->fid = (u16)(*recvbuf++);  // Lo
        hdr->fid |= ((u16)(*recvbuf++) << 8);   // Hi
        hdr->seqno = (u16)(*recvbuf++); // Lo
        hdr->seqno |= ((u16)(*recvbuf++) << 8); // Hi
        break;
    case MB_COMM_TYPE_PARENT_BOOTREQ:
        break;
    case MB_COMM_TYPE_PARENT_MEMBER_FULL:
        break;
    default:
        return NULL;
    }
    return (u8 *)recvbuf;
}
