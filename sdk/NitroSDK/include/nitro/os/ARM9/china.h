/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS - include
  File:     china.h

  Copyright 2005,2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: china.h,v $
  Revision 1.4  2006/01/18 02:11:19  kitase_hirotake
  do-indent

  Revision 1.3  2005/05/27 08:57:52  yosizaki
  add OS_ShowAttentionChina.

  Revision 1.2  2005/05/14 01:11:28  terui
  OS_InitChina関数にISBN文字列を指定するパラメータを追加

  Revision 1.1  2005/04/25 08:23:53  terui
  中国語版の正式対応のため、ヘッダを追加

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_OS_ARM9_CHINA_H_
#define NITRO_OS_ARM9_CHINA_H_

#ifdef  __cplusplus
extern "C" {
#endif

/*===========================================================================*/

#define OS_BURY_STRING_FORCHINA         "[SDK+NINTENDO:FORCHINA]"
#define OS_BUSY_STRING_LEN_FORCHINA     23


/*---------------------------------------------------------------------------*
  Name:         OS_InitChina

  Description:  SDK の OS ライブラリを初期化する。
                指向地が中国であるアプリケーション専用。
                OS_Init 関数の代わりに当関数を使用する。

  Arguments:    isbn    -   ISBN 番号等に関する文字列配列を指定する。
                            {
                                char    ISBN[ 13 ] ,
                                char    合同登記号[ 12 ] ,
                                char    新出音管(左)[ 4 ] ,
                                char    新出音管(右)[ 4 ]
                            }

  Returns:      None
 *---------------------------------------------------------------------------*/
void    OS_InitChina(const char **isbn);

/*---------------------------------------------------------------------------*
  Name:         OS_ShowAttentionChina

  Description:  中国対応ロゴ、及び注意画面を所定時間表示する。
                この関数はユーザアプリケーション側で再定義することができる。

  Arguments:    isbn    -   ISBN 番号等に関する文字列配列。

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    OS_ShowAttentionChina(const char **isbn);


/*===========================================================================*/

#ifdef  __cplusplus
}       /* extern "C" */
#endif

#endif /* NITRO_OS_ARM9_CHINA_H_ */

/*---------------------------------------------------------------------------*
    End of file
 *---------------------------------------------------------------------------*/
