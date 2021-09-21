/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - libraries
  File:     mb_common.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mb_common.c,v $
  Revision 1.18  2006/01/18 02:11:30  kitase_hirotake
  do-indent

  Revision 1.17  2005/11/07 01:21:50  okubata_ryoma
  SDK_STATIC_ASSERTからSDK_COMPILER_ASSERTに変更

  Revision 1.16  2005/02/28 05:26:24  yosizaki
  do-indent.

  Revision 1.15  2005/02/21 00:39:34  yosizaki
  replace prefix MBw to MBi.

  Revision 1.14  2005/02/18 11:56:03  yosizaki
  fix around hidden warnings.

  Revision 1.13  2004/11/22 12:41:18  takano_makoto
  デバッグプリントのバグ修正

  Revision 1.12  2004/11/09 12:02:50  yosizaki
  fix MB_Get*SystemBufSize().

  Revision 1.11  2004/11/02 10:11:43  yosizaki
  fix TYPE-string.

  Revision 1.10  2004/09/16 12:49:40  sato_masaki
  mb.hにて、MBi_***で定義していた関数、型を、MB_***に変更。

  Revision 1.9  2004/09/15 06:51:20  sato_masaki
  USE_POLLBMPの定義を削除

  Revision 1.8  2004/09/14 13:19:44  sato_masaki
  リクエストデータを断片化して送るように変更。

  Revision 1.7  2004/09/13 13:09:10  sato_masaki
  remove member word from MB_CommBlockHeader

  Revision 1.6  2004/09/13 02:59:47  sato_masaki
  MB_GetParentSystemBufSize を MBi_GetParentSystemBufSize
  MB_GetChildSystemBufSize を MBi_GetChildSystemBufSize にそれぞれ変更。

  Revision 1.5  2004/09/11 11:20:05  sato_masaki
  - MBi_BlockHeaderBegin(), MBi_BlockHeaderEnd()の引数にsendbufを追加。
  - MB_GetParentSystemBufSize(), MB_GetChildSystemBufSize()追加。
  - MBi_DebugPrint()追加(for debug)

  Revision 1.4  2004/09/10 02:52:50  sato_masaki
  PRINT_DEBUG定義に関する変更。

  Revision 1.3  2004/09/09 02:50:50  sato_masaki
  デバッグ関数、MBi_comm_type_output()の引数タイプをu16に変更。

  Revision 1.2  2004/09/06 09:52:11  sato_masaki
  small fix

  Revision 1.1  2004/09/03 07:04:36  sato_masaki
  ファイルを機能別に分割。


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include "mb_private.h"

// ----------------------------------------------------------------------------
// definition
#define MY_ROUND(n, a)      (((u32) (n) + (a) - 1) & ~((a) - 1))

#define MB_PARENT_WORK_SIZE_MIN (32 + sizeof(MBiParam) + 32 + sizeof(MB_CommPWork) + 32 + WM_SYSTEM_BUF_SIZE)
#define MB_CHILD_WORK_SIZE_MIN  (32 + sizeof(MBiParam) + 32 + sizeof(MB_CommCWork) + 32 + WM_SYSTEM_BUF_SIZE)
/*
 * 要求しているサイズで容量が正しいか判定.
 */
SDK_COMPILER_ASSERT(MB_PARENT_WORK_SIZE_MIN <= MB_SYSTEM_BUF_SIZE);
SDK_COMPILER_ASSERT(MB_CHILD_WORK_SIZE_MIN <= MB_CHILD_SYSTEM_BUF_SIZE);


/*
 * ブロックヘッダを初期化し, タイプだけ設定する.
 * このあと MBi_BlockHeaderEnd() で送信するまでの間に
 * こまごまとしたフィールドを埋めていく.
 * 引数がなければそのままで良い.
 */
void MBi_BlockHeaderBegin(u8 type, u32 *sendbuf)
{
    u8     *p = (u8 *)sendbuf;
    p[0] = type;                       /* typeは親子共用 */
}

/*
 * ブロックヘッダ設定を完了し, チェックサムを計算する.
 * その後, pollbmp で指定した相手へ実際に送信する.
 */
int MBi_BlockHeaderEnd(int body_len, u16 pollbmp, u32 *sendbuf)
{
    /*
     * sendbuf が 32 アラインされていれば MY_ROUND は不要かも.
     * すでに Init() にはチェックを追加済み.
     * 最終的には多めに取って内部でアラインするようにする.
     */
    DC_FlushRange(sendbuf, MY_ROUND(body_len, 32));
    DC_WaitWriteBufferEmpty();

    MB_DEBUG_OUTPUT("SEND (BMP:%04x)", pollbmp);
    MB_COMM_TYPE_OUTPUT(((u8 *)sendbuf)[0]);

    return MBi_SendMP(sendbuf, body_len, pollbmp);
}

/*---------------------------------------------------------------------------*
  Name:         MB_GetParentSystemBufSize

  Description:  MBで使用するワークメモリのサイズを取得(親機)

  Arguments:    None.
  
  Returns:      MB親機で使用するワークメモリのサイズ
 *---------------------------------------------------------------------------*/
int MB_GetParentSystemBufSize(void)
{
    return MB_PARENT_WORK_SIZE_MIN;
}

/*---------------------------------------------------------------------------*
  Name:         MB_GetChildSystemBufSize

  Description:  MBで使用するワークメモリのサイズを取得(子機)

  Arguments:    None.
  
  Returns:      MB子機で使用するワークメモリのサイズ
 *---------------------------------------------------------------------------*/
int MB_GetChildSystemBufSize(void)
{
    return MB_CHILD_WORK_SIZE_MIN;
}


u16 MBi_calc_cksum(const u16 *buf, int length)
{
    u32     sum;
    int     nwords = length >> 1;
    for (sum = 0; nwords > 0; nwords--)
        sum += *buf++;

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    return (u16)(sum ^ 0xffff);
}

/*  ============================================================================

    for debug

    ============================================================================*/

#ifdef	PRINT_DEBUG

void MBi_DebugPrint(const char *file, int line, const char *func, const char *fmt, ...)
{
    va_list vlist;

    OS_TPrintf("func: %s [%s:%d]:\n", func, file, line);

    va_start(vlist, fmt);
    OS_TVPrintf(fmt, vlist);
    va_end(vlist);

    OS_TPrintf("\n");
}

void MBi_comm_type_output(u16 type)
{
    enum
    { MB_TYPE_STRING_NUM = 12 };
    static const char *const mb_type_string[MB_TYPE_STRING_NUM] = {
        "MB_COMM_TYPE_DUMMY",          //      0

        "MB_COMM_TYPE_PARENT_SENDSTART",        //      1
        "MB_COMM_TYPE_PARENT_KICKREQ", //      2
        "MB_COMM_TYPE_PARENT_DL_FILEINFO",      //      3
        "MB_COMM_TYPE_PARENT_DATA",    //      4
        "MB_COMM_TYPE_PARENT_BOOTREQ", //      5
        "MB_COMM_TYPE_PARENT_MEMBER_FULL",      //      6

        "MB_COMM_TYPE_CHILD_FILEREQ",  //      7
        "MB_COMM_TYPE_CHILD_ACCEPT_FILEINFO",   //      8
        "MB_COMM_TYPE_CHILD_CONTINUE", //      9
        "MB_COMM_TYPE_CHILD_STOPREQ",  //      10
        "MB_COMM_TYPE_CHILD_BOOTREQ_ACCEPTED",  //      11
    };
    if (type >= MB_TYPE_STRING_NUM)
    {
        MB_OUTPUT("TYPE: unknown\n");
        return;
    }
    MB_OUTPUT("TYPE: %s\n", mb_type_string[type]);
}

void MBi_comm_wmevent_output(u16 type, void *arg)
{
    enum
    { MB_CB_STRING_NUM = 43 };
    static const char *const mb_cb_string[MB_CB_STRING_NUM + 2] = {
        "MB_CALLBACK_CHILD_CONNECTED", //              0
        "MB_CALLBACK_CHILD_DISCONNECTED",       //              1
        "MB_CALLBACK_MP_PARENT_SENT",  //              2
        "MB_CALLBACK_MP_PARENT_RECV",  //              3
        "MB_CALLBACK_PARENT_FOUND",    //              4
        "MB_CALLBACK_PARENT_NOT_FOUND", //              5
        "MB_CALLBACK_CONNECTED_TO_PARENT",      //              6
        "MB_CALLBACK_DISCONNECTED",    //              7
        "MB_CALLBACK_MP_CHILD_SENT",   //              8
        "MB_CALLBACK_MP_CHILD_RECV",   //              9
        "MB_CALLBACK_DISCONNECTED_FROM_PARENT", //              10
        "MB_CALLBACK_CONNECT_FAILED",  //              11
        "MB_CALLBACK_DCF_CHILD_SENT",  //              12
        "MB_CALLBACK_DCF_CHILD_SENT_ERR",       //              13
        "MB_CALLBACK_DCF_CHILD_RECV",  //              14
        "MB_CALLBACK_DISCONNECT_COMPLETE",      //              15
        "MB_CALLBACK_DISCONNECT_FAILED",        //              16
        "MB_CALLBACK_END_COMPLETE",    //              17
        "MB_CALLBACK_MP_CHILD_SENT_ERR",        //              18
        "MB_CALLBACK_MP_PARENT_SENT_ERR",       //              19
        "MB_CALLBACK_MP_STARTED",      //              20
        "MB_CALLBACK_INIT_COMPLETE",   //              21
        "MB_CALLBACK_END_MP_COMPLETE", //              22
        "MB_CALLBACK_SET_GAMEINFO_COMPLETE",    //              23
        "MB_CALLBACK_SET_GAMEINFO_FAILED",      //              24
        "MB_CALLBACK_MP_SEND_ENABLE",  //              25
        "MB_CALLBACK_PARENT_STARTED",  //              26
        "MB_CALLBACK_BEACON_LOST",     //              27
        "MB_CALLBACK_BEACON_SENT",     //              28
        "MB_CALLBACK_BEACON_RECV",     //              29
        "MB_CALLBACK_MP_SEND_DISABLE", //              30
        "MB_CALLBACK_DISASSOCIATE",    //              31
        "MB_CALLBACK_REASSOCIATE",     //              32
        "MB_CALLBACK_AUTHENTICATE",    //              33
        "MB_CALLBACK_SET_LIFETIME",    //              34
        "MB_CALLBACK_DCF_STARTED",     //              35
        "MB_CALLBACK_DCF_SENT",        //              36
        "MB_CALLBACK_DCF_SENT_ERR",    //              37
        "MB_CALLBACK_DCF_RECV",        //              38
        "MB_CALLBACK_DCF_END",         //              39
        "MB_CALLBACK_MPACK_IND",       //              40
        "MB_CALLBACK_MP_CHILD_SENT_TIMEOUT",    //              41
        "MB_CALLBACK_SEND_QUEUE_FULL_ERR",      //              42
        "MB_CALLBACK_API_ERROR",       //              255
        "MB_CALLBACK_ERROR",           //              256
    };

    if (type == MB_CALLBACK_API_ERROR)
        type = MB_CB_STRING_NUM;
    else if (type == MB_CALLBACK_ERROR)
        type = MB_CB_STRING_NUM + 1;
    else if (type >= MB_CB_STRING_NUM)
    {
        MB_OUTPUT("EVENTTYPE: unknown\n");
        return;
    }

    MB_OUTPUT("EVENTTYPE:%s\n", mb_cb_string[type]);
    if (arg)
    {
        MB_OUTPUT("\tAPPID:%04x ERRCODE:%04x\n", ((u16 *)arg)[0], ((u16 *)arg)[1]);
        MB_OUTPUT("\twlCmd:%04x wlResult:%04x\n", ((u16 *)arg)[2], ((u16 *)arg)[3]);
    }
}

#endif
