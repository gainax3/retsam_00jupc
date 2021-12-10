/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS - include
  File:     spinLock.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: spinLock.h,v $
  Revision 1.31  2006/01/18 02:12:28  kitase_hirotake
  do-indent

  Revision 1.30  2005/03/01 01:57:00  yosizaki
  copyright の年を修正.

  Revision 1.29  2005/02/28 05:26:01  yosizaki
  do-indent.

  Revision 1.28  2004/09/02 06:28:15  yada
  only change comment

  Revision 1.27  2004/07/23 04:28:23  yada
  OS_UnLock* restored for compatibility

  Revision 1.26  2004/07/23 01:52:32  yada
  inline->static inline

  Revision 1.25  2004/07/23 00:58:43  yada
  rename OS_UnLock* -> OS_Unlock*

  Revision 1.24  2004/07/22 08:21:41  yada
  change some lockID's type u32->u16

  Revision 1.23  2004/06/21 11:02:32  yada
  fix comment. incorrect about return value.

  Revision 1.22  2004/04/07 02:03:17  yada
  fix header comment

  Revision 1.21  2004/03/26 10:37:49  yada
  OS_GetLockID(), OS_ReleaseLockID() 追加

  Revision 1.20  2004/03/25 07:25:21  yada
  OS_LockCard() 等カードのロックについて追加

  Revision 1.19  2004/03/04 09:48:59  yada
  カートリッジのロックに限りIRQ/FIQ禁止にした。

  Revision 1.18  2004/02/05 07:27:19  yada
  履歴文字列のIRISだったものを NITRO から IRISに戻した。

  Revision 1.17  2004/02/05 07:09:03  yasu
  change SDK prefix iris -> nitro

  Revision 1.16  2004/01/14 01:43:41  yada
  lockやunlockの途中実行関数を設定出来るようにした

  Revision 1.15  2003/12/25 07:29:56  yada
  型ルール統一による変更

  Revision 1.14  2003/12/22 13:42:13  yasu
  OS_LockWord/LockByte に struct タグを追加

  Revision 1.13  2003/12/18 07:25:26  yada
  不要コメント削除

  Revision 1.12  2003/12/18 07:18:59  yada
  MAINP, SUBP 優先度デファインを削除

  Revision 1.11  2003/12/17 06:08:12  yasu
  細かな修正

  Revision 1.10  2003/12/11 00:35:38  yasu
  target.h のコメントアウト

  Revision 1.9  2003/12/10 10:56:18  yasu
  ISDPrint で必要な SpinLock 関数のみの移植
  OS_LockWord() -> OS_LockByWord()
  volatile LockWord -> OS_LockWord などの変更

  Revision 1.8  2003/12/08 12:21:03  yada
  12/3　REDSDK への変更対応

  Revision 1.7  2003/11/28 01:54:47  yada
  REDSDKの03-11-27反映

  Revision 1.6  2003/11/14 06:50:13  yada
  システムが用意している OS_Halt() → OS_HaltA9() に。

  Revision 1.5  2003/11/06 10:35:55  yada
  IRIS_BB時とNITRO_TEG時の処理を分離

  Revision 1.4  2003/11/05 07:29:55  yada
  ファイル名をl 小文字からL大文字に

  Revision 1.2  2003/11/04 09:08:43  yada
  (none)

  Revision 1.1  2003/10/31 04:12:03  yada
  初版作成


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_OS_SPINLOCK_H_
#define NITRO_OS_SPINLOCK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/types.h>

//======================================================================
//                      ロック変数
//
//・カード／カートリッジ／ＣＰＵ間共有内部ワークＲＡＭ／ＶＲＡＭ－Ｃ／Ｄ
//  などのＣＰＵ間での共有リスースや、ＣＰＵ間送信ＦＩＦＯ 等の専有リソース
//  の排他制御ためにこの関数をご使用下さい。
//======================================================================

// ロックＩＤ

#define OS_UNLOCK_ID            0      // ロック変数がロックされていない場合のＩＤ
#define OS_MAINP_LOCKED_FLAG    0x40   // メインプロセッサによるロック確認フラグ
#define OS_MAINP_LOCK_ID_START  0x40   // メインプロセッサ用ロックＩＤの割り当て開始番号
#define OS_MAINP_LOCK_ID_END    0x6f   //                               割り当て終了番号
#define OS_MAINP_DBG_LOCK_ID    0x70   //                               デバッガ予約番号
#define OS_MAINP_SYSTEM_LOCK_ID 0x7f   //                               システム予約番号
#define OS_SUBP_LOCKED_FLAG     0x80   //   サブプロセッサによるロック確認フラグ
#define OS_SUBP_LOCK_ID_START   0x80   //   サブプロセッサ用ロックＩＤの割り当て開始番号
#define OS_SUBP_LOCK_ID_END     0xaf   //                               割り当て終了番号
#define OS_SUBP_DBG_LOCK_ID     0xb0   //                               デバッガ予約番号
#define OS_SUBP_SYSTEM_LOCK_ID  0xbf   //                               システム予約番号

#define OS_LOCK_SUCCESS         0      // ロック成功
#define OS_LOCK_ERROR           (-1)   // ロックエラー

#define OS_UNLOCK_SUCCESS       0      // ロック解除成功
#define OS_UNLOCK_ERROR         (-2)   // ロック解除エラー

#define OS_LOCK_FREE            0      // ロック解除中

#define OS_LOCK_ID_ERROR        (-3)   // ロックＩＤエラー


//---- structure of lock variable 
typedef volatile struct OSLockWord
{
    u32     lockFlag;
    u16     ownerID;
    u16     extension;
}
OSLockWord;

/*---------------------------------------------------------------------------*
  Name:         OS_InitLock

  Description:  initialize system lock variable 
                and privilege to access shared resources

                * cartridge exclusive control area is not cleared 
                  because debugger uses.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    OS_InitLock(void);

//----------------------------------------------------------------------
//          スピンロック
//
//・プロセッサ間やモジュール間で共有リソースを排他制御するための
//  ロック変数のスピンロックを行います。
//・ロックが成功するまで試行し続けます。
//・プロセッサ間の共有リソースは必ずロック後に使用して下さい。
//・プロセッサ専有のリソースはタイミング的に調整できるのであれば
//  ロックしなくても構いません。
//  専有リソースはデバッグ時に限定してロックすることもできます。
//
//・引数：
//  lockID              ロックID
//  lockp               ロック変数のポインタ
//
//・戻り値：
//  OS_LOCK_SUCCESS     ロック成功
//----------------------------------------------------------------------
s32     OS_LockByWord(u16 lockID, OSLockWord *lockp, void (*ctrlFuncp) (void));
s32     OS_LockCartridge(u16 lockID);
s32     OS_LockCard(u16 lockID);

//----------------------------------------------------------------------
//          ロックの解除
//
//・ロックを解除し、共有リソースのアクセス権をサブプロセッサに渡します。
//・ロックしていないモジュールが実行した場合には解除されず、
//  OS_UNLOCK_ERROR が返されます。
//
//・引数：
//  lockID              ロックID
//  lockp               ロック変数のポインタ
//
//・戻り値：
//  OS_UNLOCK_SUCCESS   ロック解除成功
//  OS_UNLOCK_ERROR     ロック解除エラー
//----------------------------------------------------------------------
s32     OS_UnlockByWord(u16 lockID, OSLockWord *lockp, void (*ctrlFuncp) (void));
s32     OS_UnlockCartridge(u16 lockID);
s32     OS_UnlockCard(u16 lockID);

//---- for compatibility to old name ('UnLock' <-> 'Unlock')
//     because isd lib calls OS_UnLock*, these cannot be inline.
s32     OS_UnLockByWord(u16 lockID, OSLockWord *lockp, void (*ctrlFuncp) (void));
s32     OS_UnLockCartridge(u16 lockID);
s32     OS_UnLockCard(u16 lockID);

//----------------------------------------------------------------------
//          ロックの試行
//
//・１回だけスピンロックを試行します。
//・プロセッサ間の共有リソースは必ずロック後に使用して下さい。
//・プロセッサ専有のリソースはタイミング的に調整できるのであれば
//  ロックしなくても構いません。
//  専有リソースはデバッグ時に限定してロックすることもできます。
//
//・引数：
//  lockID              ロックID
//  lockp               ロック変数のポインタ
//  CtrlFuncp           リソース制御関数のポインタ
//
//・戻り値：
//  正                  ロック中（直前に格納されていたID）
//  OS_LOCK_SUCCESS     ロック成功
//----------------------------------------------------------------------
s32     OS_TryLockByWord(u16 lockID, OSLockWord *lockp, void (*crtlFuncp) (void));
s32     OS_TryLockCartridge(u16 lockID);
s32     OS_TryLockCard(u16 lockID);

//----------------------------------------------------------------------
//          ロック変数の所有モジュールIDの読み込み
//
//・ロック変数の所有モジュールID を読み込みます。
//・モジュールID が非0 の場合はその時点でどちらのプロセッサ側が
//  所有権を持っているのかを確認できます。
//・共有リソースの場合は「メインプロセッサ側が所有権を持っている状態」
//  のみを割り込みを禁止することによって維持することができます。
//  その他の状態はサブプロセッサが変化させてしまう可能性があります。
//・所有モジュールID が 0 であってもロック変数が解除されているとは限りません。
//
//・引数：
//  lockp       ロック変数のポインタ
//
//・戻り値：    所有モジュールID
//
//※但し、メインメモリに対してはキャッシュ経由でなければ
//  バイトアクセスはできないことに注意して下さい。
//  ですので、メインメモリ上では基本的に OS_ReadOwnerOfLockWord() を使用して下さい。
//----------------------------------------------------------------------
u16     OS_ReadOwnerOfLockWord(OSLockWord *lockp);
#define OS_ReadOwnerOfLockCartridge()  OS_ReadOwnerOfLockWord( (OSLockWord *)HW_CTRDG_LOCK_BUF )
#define OS_ReadOwnerOfLockCard()       OS_ReadOwnerOfLockWord( (OSLockWord *)HW_CARD_LOCK_BUF  )



/*---------------------------------------------------------------------------*
  Name:         OS_GetLockID

  Description:  get lock ID

  Arguments:    None.

  Returns:      OS_LOCK_ID_ERROR, if fail to get ID

                if ARM9
                   0x40～0x6f       lockID
                else if ARM7
                   0x80～0xaf       lockID
                endif

                *Notice:  ID is allocated only 48 pattern at a highest.

 *---------------------------------------------------------------------------*/
s32     OS_GetLockID(void);


/*---------------------------------------------------------------------------*
  Name:         OS_ReleaseLockID

  Description:  release lock ID

  Arguments:    lockID : id to tend to release

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    OS_ReleaseLockID(u16 lockID);

#ifdef __cplusplus
} /* extern "C" */
#endif

/* NITRO_OS_SPINLOCK_H_ */
#endif
