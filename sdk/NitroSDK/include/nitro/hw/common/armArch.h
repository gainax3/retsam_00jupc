/*---------------------------------------------------------------------------*
  Project:  NitroSDK - HW - include
  File:     armArch.h

  Copyright 2003-2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: armArch.h,v $
  Revision 1.12  2005/03/01 01:57:00  yosizaki
  copyright の年を修正.

  Revision 1.11  2005/02/28 05:26:01  yosizaki
  do-indent.

  Revision 1.10  2004/05/03 04:50:46  yada
  add HW_CPU_CLOCK*

  Revision 1.9  2004/04/07 02:03:17  yada
  fix header comment

  Revision 1.8  2004/03/16 11:53:06  yasu
  move HW_READ32/WRITE32 -> MI_ReadWord/WriteWord

  Revision 1.7  2004/02/25 11:28:59  yada
  HW_SYSTEM_CLOCK 追加

  Revision 1.6  2004/02/14 06:32:31  yasu
  add HW_READ43/WRITE32 macros

  Revision 1.5  2004/02/05 07:09:03  yasu
  change SDK prefix iris -> nitro

  Revision 1.4  2003/12/18 01:52:47  yada
  データキャッシュ、命令キャッシュイネーブルフラグのシフト量追加

  Revision 1.3  2003/12/17 11:38:48  yada
  キャッシュ定義追加

  Revision 1.2  2003/12/12 01:29:00  yada
  HW_PSR_ARM_STATE 追加

  Revision 1.1  2003/12/12 01:24:46  yada
  hw/systemControl.h → hw/armArch.h と名称変更

  Revision 1.2  2003/11/28 01:55:16  yada
  REDSDKの03-11-27反映

  Revision 1.1  2003/11/04 09:11:24  yada
  初版(整合性がうまくとれていないかもしれません)


  $NoKeywords: $

 *---------------------------------------------------------------------------*/

#ifndef NITRO_HW_SYSTEMCONTROL_H_
#define NITRO_HW_SYSTEMCONTROL_H_

#ifdef __cplusplus
extern "C" {
#endif

#define HW_ICACHE_SIZE              0x2000      //   命令キャッシュ
#define HW_DCACHE_SIZE              0x1000      // データキャッシュ
#define HW_CACHE_LINE_SIZE          32

#define HW_SYSTEM_CLOCK             33514000    // 正確には33513982?

#define HW_CPU_CLOCK_ARM7           33513982
#define HW_CPU_CLOCK_ARM9           67027964

#ifdef SDK_ARM7
#define HW_CPU_CLOCK                HW_CPU_CLOCK_ARM7
#else
#define HW_CPU_CLOCK                HW_CPU_CLOCK_ARM9
#endif

//**********************************************************************
//              ＩＲＩＳレジスタ・コントロールビット
//**********************************************************************

//----------------------------------------------------------------------
//                  プログラムステータスレジスタ
//----------------------------------------------------------------------

#define HW_PSR_CPU_MODE_MASK       0x1f // ＣＰＵモード

#define HW_PSR_USER_MODE           0x10 // ユーザ
#define HW_PSR_FIQ_MODE            0x11 // ＦＩＱ
#define HW_PSR_IRQ_MODE            0x12 // ＩＲＱ
#define HW_PSR_SVC_MODE            0x13 // スーパバイザ
#define HW_PSR_ABORT_MODE          0x17 // アボート（プリフェッチ／データ）
#define HW_PSR_UNDEF_MODE          0x1b // 未定義命令
#define HW_PSR_SYS_MODE            0x1f // システム

#define HW_PSR_ARM_STATE           0x0 // ＡＲＭステート
#define HW_PSR_THUMB_STATE         0x20 // ＴＨＵＭＢステート

#define HW_PSR_FIQ_DISABLE         0x40 // ＦＩＱ不許可
#define HW_PSR_IRQ_DISABLE         0x80 // ＩＲＱ不許可
#define HW_PSR_IRQ_FIQ_DISABLE     0xc0 // ＩＲＱ＆ＦＩＱ不許可

#define HW_PSR_Q_FLAG              0x08000000   // スティッキーオーバーフロー
#define HW_PSR_V_FLAG              0x10000000   // オーバーフロー
#define HW_PSR_C_FLAG              0x20000000   // キャリー／ボロー／拡張
#define HW_PSR_Z_FLAG              0x40000000   // ゼロ
#define HW_PSR_N_FLAG              0x80000000   // 負／未満


//----------------------------------------------------------------------
//                  システムコントロールコプロセッサ
//----------------------------------------------------------------------

// レジスタ１（マスタコントロール）

#define HW_C1_SB1_BITSET           0x00000078   // レジスタ１用１固定ビット列

#define HW_C1_ITCM_LOAD_MODE       0x00080000   //   命令ＴＣＭ ロードモード
#define HW_C1_DTCM_LOAD_MODE       0x00020000   // データＴＣＭ ロードモード
#define HW_C1_ITCM_ENABLE          0x00040000   //   命令ＴＣＭ イネーブル
#define HW_C1_DTCM_ENABLE          0x00010000   // データＴＣＭ イネーブル
#define HW_C1_LD_INTERWORK_DISABLE 0x00008000   // ロード命令によるインターワーキング ディセーブル
#define HW_C1_CACHE_ROUND_ROBIN    0x00004000   // キャッシュ置換アルゴリズム ラウンドロビン（最悪時のヒット率が安定）
#define HW_C1_CACHE_PSEUDO_RANDOM  0x00000000   //                            擬似ランダム
#define HW_C1_EXCEPT_VEC_UPPER     0x00002000   // 例外ベクタ 上位アドレス（こちらに設定して下さい）
#define HW_C1_EXCEPT_VEC_LOWER     0x00000000   //            下位アドレス
#define HW_C1_ICACHE_ENABLE        0x00001000   //   命令キャッシュ イネーブル
#define HW_C1_DCACHE_ENABLE        0x00000004   // データキャッシュ イネーブル
#define HW_C1_LITTLE_ENDIAN        0x00000000   // リトルエンディアン
#define HW_C1_BIG_ENDIAN           0x00000080   // ビッグエンディアン
#define HW_C1_PROTECT_UNIT_ENABLE  0x00000001   // プロテクションユニット イネーブル

#define HW_C1_ICACHE_ENABLE_SHIFT 12
#define HW_C1_DCACHE_ENABLE_SHIFT 2


// レジスタ２（プロテクションリージョン・キャッシュ設定）

#define HW_C2_PR0_SFT              0   // プロテクションリージョン０
#define HW_C2_PR1_SFT              1   //                 １
#define HW_C2_PR2_SFT              2   //                 ２
#define HW_C2_PR3_SFT              3   //                 ３
#define HW_C2_PR4_SFT              4   //                 ４
#define HW_C2_PR5_SFT              5   //                 ５
#define HW_C2_PR6_SFT              6   //                 ６
#define HW_C2_PR7_SFT              7   //                 ７


// レジスタ３（プロテクションリージョン・ライトバッファ設定）

#define HW_C3_PR0_SFT              0   // プロテクションリージョン０
#define HW_C3_PR1_SFT              1   //                 １
#define HW_C3_PR2_SFT              2   //                 ２
#define HW_C3_PR3_SFT              3   //                 ３
#define HW_C3_PR4_SFT              4   //                 ４
#define HW_C3_PR5_SFT              5   //                 ５
#define HW_C3_PR6_SFT              6   //                 ６
#define HW_C3_PR7_SFT              7   //                 ７


// レジスタ５（プロテクションリージョン・アクセス許可）

#define HW_C5_PERMIT_MASK          0xf // プロテクションリージョンアクセス許可マスク

#define HW_C5_PERMIT_NA            0   //                  アクセス不許可
#define HW_C5_PERMIT_RW            1   //                  リードライト許可
#define HW_C5_PERMIT_RO            5   //                  リードオンリー許可

#define HW_C5_PR0_SFT              0   // プロテクションリージョン０
#define HW_C5_PR1_SFT              4   //                 １
#define HW_C5_PR2_SFT              8   //                 ２
#define HW_C5_PR3_SFT              12  //                 ３
#define HW_C5_PR4_SFT              16  //                 ４
#define HW_C5_PR5_SFT              20  //                 ５
#define HW_C5_PR6_SFT              24  //                 ６
#define HW_C5_PR7_SFT              28  //                 ７


// レジスタ６（プロテクションリージョン・ベースアドレス／サイズ）

#define HW_C6_PR_SIZE_MASK         0x0000003e   // プロテクションリージョン サイズ
#define HW_C6_PR_BASE_MASK         0xfffff000   //                  ベースアドレス

#define HW_C6_PR_SIZE_SHIFT        1
#define HW_C6_PR_BASE_SHIFT        12

#define HW_C6_PR_ENABLE            1   // プロテクションリージョン イネーブル
#define HW_C6_PR_DISABLE           0   //                  ディセーブル

#define HW_C6_PR_4KB               0x16 // リージョンサイズ ４ＫＢｙｔｅ
#define HW_C6_PR_8KB               0x18 //                  ８ＫＢｙｔｅ
#define HW_C6_PR_16KB              0x1a //                １６ＫＢｙｔｅ
#define HW_C6_PR_32KB              0x1c //                ３２ＫＢｙｔｅ
#define HW_C6_PR_64KB              0x1e //                ６４ＫＢｙｔｅ
#define HW_C6_PR_128KB             0x20 //              １２８ＫＢｙｔｅ
#define HW_C6_PR_256KB             0x22 //              ２５６ＫＢｙｔｅ
#define HW_C6_PR_512KB             0x24 //              ５１２ＫＢｙｔｅ
#define HW_C6_PR_1MB               0x26 //                  １ＭＢｙｔｅ
#define HW_C6_PR_2MB               0x28 //                  ２ＭＢｙｔｅ
#define HW_C6_PR_4MB               0x2a //                  ４ＭＢｙｔｅ
#define HW_C6_PR_8MB               0x2c //                  ８ＭＢｙｔｅ
#define HW_C6_PR_16MB              0x2e //                １６ＭＢｙｔｅ
#define HW_C6_PR_32MB              0x30 //                ３２ＭＢｙｔｅ
#define HW_C6_PR_64MB              0x32 //                ６４ＭＢｙｔｅ
#define HW_C6_PR_128MB             0x34 //              １２８ＭＢｙｔｅ
#define HW_C6_PR_256MB             0x36 //              ２５６ＭＢｙｔｅ
#define HW_C6_PR_512MB             0x38 //              ５１２ＭＢｙｔｅ
#define HW_C6_PR_1GB               0x3a //                  １ＧＢｙｔｅ
#define HW_C6_PR_2GB               0x3c //                  ２ＧＢｙｔｅ
#define HW_C6_PR_4GB               0x3e //                  ４ＧＢｙｔｅ


// レジスタ７.１３（命令キャッシュ・プリフェッチ）

#define HW_C7_ICACHE_PREFCHP_MASK  0xffffffe0   //   命令キャッシュ プリフェッチアドレス


// レジスタ７.１０、７.１４（キャッシュインデックス操作）

#define HW_C7_ICACHE_INDEX_MASK    0x00000fe0   //   命令キャッシュ インデックス
#define HW_C7_DCACHE_INDEX_MASK    0x000003e0   // データキャッシュ インデックス
#define HW_C7_CACHE_SET_NO_MASK    0xc0000000   //       キャッシュ セットＮｏ

#define HW_C7_CACHE_INDEX_SHIFT    5
#define HW_C7_CACHE_SET_NO_SHIFT   30


// レジスタ９.０（キャッシュロックダウン）

#define HW_C9_LOCKDOWN_SET_NO_MASK 0x00000003   // キャッシュロックダウン セットＮｏ

#define HW_C9_LOCKDOWN_SET_NO_SHIFT 0

#define HW_C9_LOCKDOWN_LOAD_MODE   0x80000000   // キャッシュロックダウン ロードモード


// レジスタ９.１（ＴＣＭベースアドレス／サイズ）

#define HW_C9_TCMR_SIZE_MASK       0x0000003e   // ＴＣＭリージョン サイズ
#define HW_C9_TCMR_BASE_MASK       0xfffff000   //                  ベースアドレス

#define HW_C9_TCMR_SIZE_SHIFT      1
#define HW_C9_TCMR_BASE_SHIFT      12

#define HW_C9_TCMR_4KB             0x06 // リージョンサイズ ４ＫＢｙｔｅ
#define HW_C9_TCMR_8KB             0x08 //                  ８ＫＢｙｔｅ
#define HW_C9_TCMR_16KB            0x0a //                １６ＫＢｙｔｅ
#define HW_C9_TCMR_32KB            0x0c //                ３２ＫＢｙｔｅ
#define HW_C9_TCMR_64KB            0x0e //                ６４ＫＢｙｔｅ
#define HW_C9_TCMR_128KB           0x10 //              １２８ＫＢｙｔｅ
#define HW_C9_TCMR_256KB           0x12 //              ２５６ＫＢｙｔｅ
#define HW_C9_TCMR_512KB           0x14 //              ５１２ＫＢｙｔｅ
#define HW_C9_TCMR_1MB             0x16 //                  １ＭＢｙｔｅ
#define HW_C9_TCMR_2MB             0x18 //                  ２ＭＢｙｔｅ
#define HW_C9_TCMR_4MB             0x1a //                  ４ＭＢｙｔｅ
#define HW_C9_TCMR_8MB             0x1c //                  ８ＭＢｙｔｅ
#define HW_C9_TCMR_16MB            0x1e //                １６ＭＢｙｔｅ
#define HW_C9_TCMR_32MB            0x20 //                ３２ＭＢｙｔｅ
#define HW_C9_TCMR_64MB            0x22 //                ６４ＭＢｙｔｅ
#define HW_C9_TCMR_128MB           0x24 //              １２８ＭＢｙｔｅ
#define HW_C9_TCMR_256MB           0x26 //              ２５６ＭＢｙｔｅ
#define HW_C9_TCMR_512MB           0x28 //              ５１２ＭＢｙｔｅ
#define HW_C9_TCMR_1GB             0x2a //                  １ＧＢｙｔｅ
#define HW_C9_TCMR_2GB             0x2c //                  ２ＧＢｙｔｅ
#define HW_C9_TCMR_4GB             0x2e //                  ４ＧＢｙｔｅ

#ifdef __cplusplus
} /* extern "C" */
#endif

/* NITRO_HW_SYSTEMCONTROL_H_ */
#endif
