/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - include
  File:     mb_wm.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mb_block.h,v $
  Revision 1.5  2006/01/18 02:11:30  kitase_hirotake
  do-indent

  Revision 1.4  2005/03/01 01:57:00  yosizaki
  copyright の年を修正.

  Revision 1.3  2005/02/28 05:26:25  yosizaki
  do-indent.

  Revision 1.2  2004/11/24 13:03:22  takano_makoto
  MBCommParentBlockHeaderとMBCommChildBlockHeaderをmb_common.hから移動

  Revision 1.1  2004/11/22 12:47:52  takano_makoto
  Initial update.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef _MB_BLOCK_H_
#define _MB_BLOCK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/types.h>
#include <nitro/mb.h>
#include <nitro/wm.h>
#include "mb_common.h"


/* ----------------------------------------------------------------------- *
 *  構造体定義
 * ----------------------------------------------------------------------- */

/*
 * 転送ブロックヘッダ(各ブロックに付加するヘッダ)
   実際にはPadding分を詰めた形で送受信します。
 */
typedef struct
{
    u8      type;                      // データタイプ                              1B
    u8      pad1[1];
    u16     fid;                       // ファイル番号値                            4B
    u16     seqno;                     // 親機使用                                  6B
}
MBCommParentBlockHeader;


typedef struct
{
    u8      type;                      // データタイプ                             1B
    u8      pad1[1];                   //                                          1B
    union                              //                              unionの合計 16B
    {
        struct
        {
            u16     req;               // ブロック番号要求値                       2B
            u8      reserved[MB_COMM_CALC_REQ_DATA_PIECE_SIZE(MB_COMM_P_RECVLEN_MAX)];  // 14B
        }
        data;

        struct
        {
            u8      piece;             //  断片化されたデータの番号                1B
            //  断片化されたデータを格納するバッファ
            u8      data[MB_COMM_CALC_REQ_DATA_PIECE_SIZE(MB_COMM_P_RECVLEN_MAX)];      //  14B
            u8      pad2[1];           //  1B
        }
        req_data;
    };
    // 計18B
}
MBCommChildBlockHeader;


// 分割リクエストの受信バッファ
typedef struct
{
    u32     data_buf[MB_MAX_CHILD][MB_COMM_CALC_REQ_DATA_BUF_SIZE(MB_COMM_P_RECVLEN_MAX) /
                                   sizeof(u32) + 1];
    u32     data_bmp[MB_MAX_CHILD];
}
MbRequestPieceBuf;

/* ----------------------------------------------------------------------- *
 *  関数宣言
 * ----------------------------------------------------------------------- */

/* 子機MP送信サイズの設定 */
void    MBi_SetChildMPMaxSize(u16 childMaxSize);
/* 親機での分割リクエスト受信用バッファの設定 */
void    MBi_SetParentPieceBuffer(MbRequestPieceBuf * buf);
/* 分割リクエスト受信用バッファをクリア */
void    MBi_ClearParentPieceBuffer(u16 child_aid);

/* 親機の送信ヘッダ情報から、実際に送信するデータを構築 */
u8     *MBi_MakeParentSendBuffer(const MBCommParentBlockHeader * hdr, u8 *sendbuf);
/* 子機から受信したデータバッファから構造体を構築 */
u8     *MBi_SetRecvBufferFromChild(const u8 *recvbuf, MBCommChildBlockHeader * hdr, u16 child_id);

/* 子機の送信ヘッダ情報から、実際に送信するデータを構築 */
u8     *MBi_MakeChildSendBuffer(const MBCommChildBlockHeader * hdr, u8 *sendbuf);
/* 子機から送信するリクエストデータを分割します。 */
u8      MBi_SendRequestDataPiece(u8 *pData, const MBCommRequestData *pReq);
/* 親機から受信したパケットからヘッダ部を取得し、データ部へのポインタを返します。 */
u8     *MBi_SetRecvBufferFromParent(MBCommParentBlockHeader * hdr, const u8 *recvbuf);


#ifdef __cplusplus
}
#endif

#endif /*  _MB_BLOCK_H_    */
