/*---------------------------------------------------------------------------*
# Project:  NitroSDK - libraries - utest
  File:     utest_assert.c

  Copyright 2005,2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: utest_assert.c,v $
  Revision 1.2  2006/01/18 02:12:38  kitase_hirotake
  do-indent

  Revision 1.1  2005/06/24 02:19:25  yasu
  ユニットテストの追加

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>
#include <nitro/utest.h>

/*---------------------------------------------------------------------------*
  通常の ASSERT
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         UTi_Assert
  Description:  exp_val の値が非0なら NG メッセージを出力する
  Arguments:    file:    __FILE__ ソースファイル名
                line:    __LINE__ ソースファイルライン数
                exp_str: 表示するメッセージフォーマット
                exp_val: 表示する値
  Returns:      None.
 *---------------------------------------------------------------------------*/
void UTi_Assert(const char *file, int line, const char *exp_str, int exp_val)
{
    // val が０なら NG とする
    if (!exp_val)
    {
        UTi_SetResult(UT_RESULT_NG);
        UTi_SetMessage(file, line, "UT_Assert: %s[=%d] is not TRUE\n", exp_str, exp_val);
    }

    // val が非０であり、これまでのテストに失敗していなければ OK とする
    else if (UTi_GetResult() != UT_RESULT_NG)
    {
        UTi_SetResult(UT_RESULT_OK);
    }
    return;
}

/*---------------------------------------------------------------------------*
  Name:         UTi_AssertEq
  Description:  exp_val1 の値と exp_val2 が等しくなければ NG メッセージを出力する
  Arguments:    file:    __FILE__ ソースファイル名
                line:    __LINE__ ソースファイルライン数
                exp_str1: 比較する値1 のテキスト
                exp_val1: 比較する値1
                exp_str2: 比較する値2 のテキスト
                exp_val2: 比較する値2
  Returns:      None.
 *---------------------------------------------------------------------------*/
void UTi_AssertEq(const char *file, int line, const char *exp_str1, int exp_val1,
                  const char *exp_str2, int exp_val2)
{
    // val1 と val2 が等しくないなら NG とする
    if (exp_val1 != exp_val2)
    {
        UTi_SetResult(UT_RESULT_NG);
        UTi_SetMessage(file, line, "UT_AssertEq: %s[=%d] is not equal to %s[=%d]\n", exp_str1,
                       exp_val1, exp_str2, exp_val2);
    }

    // val1 と val2 が等しくかつ、これまでのテストに失敗していなければ OK とする
    else if (UTi_GetResult() != UT_RESULT_NG)
    {
        UTi_SetResult(UT_RESULT_OK);
    }
    return;
}


/*---------------------------------------------------------------------------*
  Name:         UTi_AssertNe
  Description:  exp_val1 の値と exp_val2 が等しければ NG メッセージを出力する
  Arguments:    file:    __FILE__ ソースファイル名
                line:    __LINE__ ソースファイルライン数
                exp_str1: 比較する値1 のテキスト
                exp_val1: 比較する値1
                exp_str2: 比較する値2 のテキスト
                exp_val2: 比較する値2
  Returns:      None.
 *---------------------------------------------------------------------------*/
void UTi_AssertNe(const char *file, int line, const char *exp_str1, int exp_val1,
                  const char *exp_str2, int exp_val2)
{
    // val1 と val2 が等しいなら NG とする
    if (exp_val1 == exp_val2)
    {
        UTi_SetResult(UT_RESULT_NG);
        UTi_SetMessage(file, line, "UT_AssertNe: %s[=%d] is equal to %s[=%d]\n", exp_str1, exp_val1,
                       exp_str2, exp_val2);
    }

    // val1 と val2 が等しくなく、かつこれまでのテストに失敗していなければ OK とする
    else if (UTi_GetResult() != UT_RESULT_NG)
    {
        UTi_SetResult(UT_RESULT_OK);
    }
    return;
}


/*---------------------------------------------------------------------------*
  Name:         UTi_AssertMemEq
  
  Description:  exp_len バイトのメモリブロック exp_ptr1 と exp_ptr2 が
                等しくなければ NG メッセージを出力する
  
  Arguments:    file:    __FILE__ ソースファイル名
                line:    __LINE__ ソースファイルライン数
                exp_str1: 比較するブロック1 のテキスト表現
                exp_ptr1: 比較するブロック1
                exp_str2: 比較するブロック2 のテキスト表現
                exp_ptr2: 比較するブロック2
                exp_len : 比較するブロックのサイズ
  Returns:      None.
 *---------------------------------------------------------------------------*/
void UTi_AssertMemEq(const char *file, int line,
                     const char *exp_str1, const void *exp_ptr1,
                     const char *exp_str2, const void *exp_ptr2, int exp_len)
{
    // exp_ptr1 と exp_ptr2 が exp_len バイト長だけ等しくなければ NG とする
    if (UTi_MemCmp(exp_ptr1, exp_ptr2, exp_len))
    {
        UTi_SetResult(UT_RESULT_NG);
        UTi_SetMessage(file, line, "UT_AssertMemEq: %s is not equal to %s (size=%d)\n", exp_str1,
                       exp_str2, exp_len);
    }

    // val1 と val2 が等しく、かつこれまでのテストに失敗していなければ OK とする
    else if (UTi_GetResult() != UT_RESULT_NG)
    {
        UTi_SetResult(UT_RESULT_OK);
    }
    return;
}


/*---------------------------------------------------------------------------*
  Name:         UTi_AssertMemNe
  
  Description:  exp_len バイトのメモリブロック exp_ptr1 と exp_ptr2 が
                等しければ NG メッセージを出力する
  
  Arguments:    file:    __FILE__ ソースファイル名
                line:    __LINE__ ソースファイルライン数
                exp_str1: 比較するブロック1 のテキスト表現
                exp_ptr1: 比較するブロック1
                exp_str2: 比較するブロック2 のテキスト表現
                exp_ptr2: 比較するブロック2
                exp_len : 比較するブロックのサイズ
  Returns:      None.
 *---------------------------------------------------------------------------*/
void UTi_AssertMemNe(const char *file, int line,
                     const char *exp_str1, const void *exp_ptr1,
                     const char *exp_str2, const void *exp_ptr2, int exp_len)
{
    // exp_ptr1 と exp_ptr2 が exp_len バイト長だけ等しければ NG とする
    if (!UTi_MemCmp(exp_ptr1, exp_ptr2, exp_len))
    {
        UTi_SetResult(UT_RESULT_NG);
        UTi_SetMessage(file, line, "UT_AssertMemNe: %s is equal to %s (size=%d)\n", exp_str1,
                       exp_str2, exp_len);
    }

    // val1 と val2 が等しくなく、かつこれまでのテストに失敗していなければ OK とする
    else if (UTi_GetResult() != UT_RESULT_NG)
    {
        UTi_SetResult(UT_RESULT_OK);
    }
    return;
}


/*---------------------------------------------------------------------------*
  ASSERT 機能の ASSERT
 *---------------------------------------------------------------------------*/

OSContext UTi_AssertContext;

/*---------------------------------------------------------------------------*
  Name:         UTi_Terminate
  Description:  停止処理を差し替える. ASSERT 後に元に戻る
  Arguments:    None.
  Returns:      None.
 *---------------------------------------------------------------------------*/
#ifdef SDK_DEBUG
void UTi_Terminate(void)
{
    OS_LoadContext(&UTi_AssertContext);
}
#endif


/*---------------------------------------------------------------------------*
  Name:         UTi_AssertAsserted
  Description:  Assert が発生していないときのエラーメッセージを出力する
  Arguments:    file:    __FILE__ ソースファイル名
                line:    __LINE__ ソースファイルライン数
                exp_str:  表示するメッセージフォーマット
                asserted:アサートが起こっていたら 1 さもなくば 0
  Returns:      None.
 *---------------------------------------------------------------------------*/
void UTi_AssertAsserted(const char *file, int line, const char *exp_str, int asserted)
{
    // アサートが起こらないなら NG とする
    if (!asserted)
    {
        UTi_SetResult(UT_RESULT_NG);
        UTi_SetMessage(file, line, "UT_AssertAsserted: %s not asserted\n", exp_str);
    }

    // アサートが起こっており、かつこれまでのテストに失敗していなければ OK とする
    else if (UTi_GetResult() != UT_RESULT_NG)
    {
        UTi_SetResult(UT_RESULT_OK);
    }
    return;
}


/*---------------------------------------------------------------------------*
  Name:         UTi_AssertNotAsserted
  Description:  Assert が発生しているときのエラーメッセージを出力する
  Arguments:    file:    __FILE__ ソースファイル名
                line:    __LINE__ ソースファイルライン数
                exp_str:  表示するメッセージフォーマット
                asserted:アサートが起こっていたら 1 さもなくば 0
  Returns:      None.
 *---------------------------------------------------------------------------*/
void UTi_AssertNotAsserted(const char *file, int line, const char *exp_str, int asserted)
{
    // アサートが起きたなら NG とする
    if (asserted)
    {
        UTi_SetResult(UT_RESULT_NG);
        UTi_SetMessage(file, line, "UT_AssertNotAsserted: %s asserted\n", exp_str);
    }

    // アサートが起こっていない、かつこれまでのテストに失敗していなければ OK とする
    else if (UTi_GetResult() != UT_RESULT_NG)
    {
        UTi_SetResult(UT_RESULT_OK);
    }
    return;
}


/*---------------------------------------------------------------------------*
  Name:         UTi_MemCmp
  
  Description:  len バイトのメモリブロック ptr1 と ptr2 を比較する
  
  Arguments:    ptr1: 比較するブロック1
                ptr2: 比較するブロック2
                len : 比較するブロックのサイズ
  
  Returns:      0    : 等しい.
                0以外: 等しくない
 *---------------------------------------------------------------------------*/
int UTi_MemCmp(const void *ptr1, const void *ptr2, int len)
{
    int     i;

    for (i = 0; i < len; i++)
    {
        if (((unsigned char *)ptr1)[i] != ((unsigned char *)ptr2)[i])
        {
            return 1;
        }
    }
    return 0;
}
