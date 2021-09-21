/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - g2d - src - internal
  File:     g2di_SplitChar.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.5 $
 *---------------------------------------------------------------------------*/

#include <nnsys/g2d/g2di_SplitChar.h>
#include <nnsys/g2d/g2d_config.h>
#include <nnsys/g2d/g2di_AssertUtil.h>


//****************************************************************************
// static functions
//****************************************************************************


/*---------------------------------------------------------------------------*
  Name:         IsSjisLeadByte

  Description:  SJIS のリードバイトかどうか判定します。

  Arguments:    c:  判定対象のバイトデータ。

  Returns:      c がSJISのリードバイトならTRUEを返します。
 *---------------------------------------------------------------------------*/
#define SJIS_LOW_WIDTH  0xBC
#define SJIS_LOW_BASE   0x40
#define SJIS_HIGH_BASE  0x81
#define SHIGH_AREA0_ST  0x81
#define SHIGH_AREA0_ED  0x85
#define SHIGH_AREA1     0x87
#define SHIGH_AREA2_ST  0x88
#define SHIGH_AREA2_ED  0xA0
#define SHIGH_AREA3_ST  0xE0
#define SHIGH_AREA3_ED  0x100
#define ASCII_ST    0x20
#define ASCII_ED    0x80
#define HANKANA_ST  0xA0
#define HANKANA_ED  0xE0

static NNS_G2D_INLINE BOOL IsSjisLeadByte( u8 c )
{
    return ( ((SJIS_HIGH_BASE <= c) && (c < SHIGH_AREA2_ED))
                || (SHIGH_AREA3_ST <= c)  )
                ? TRUE: FALSE;
}






//****************************************************************************
// global functions
//****************************************************************************


/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSplitChar*

  Description:  NNSiG2dGetCharCallback 型の文字コード切り出し関数です。
                バイトストリームから最初の文字の文字コードを取得するとともに
                ストリームポインタを次の文字に移動させます。

  Arguments:    ppChar: バイト配列へのポインタを格納しているバッファへのポインタ。
                        関数から返ると、このポインタの先のバッファには
                        次の文字の先頭へのポインタが格納されます。

  Returns:      *ppChar の最初の文字の文字コード。
 *---------------------------------------------------------------------------*/

// UTF-16
u16 NNSi_G2dSplitCharUTF16(const void** ppChar)
{
    u16 c;

    NNS_G2D_POINTER_ASSERT( ppChar );
    NNS_G2D_POINTER_ASSERT( *ppChar );
    NNS_G2D_ALIGN_ASSERT( *ppChar, 2 );

    c = *(const wchar_t*)*ppChar;
    ((const wchar_t*)*ppChar)++;

    return c;
}

// UTF-8
u16 NNSi_G2dSplitCharUTF8(const void** ppChar)
{
    const u8* pChar;
    u16 c;

    NNS_G2D_POINTER_ASSERT( ppChar );
    NNS_G2D_POINTER_ASSERT( *ppChar );
    NNS_G2D_ASSERT( (*(const u8*)*ppChar & 0xC0) != 0x80 );    // UTF-8の1バイト目は左記条件を満たす

    pChar = (const u8*)*ppChar;

    if( (*pChar & 0x80) == 0x00 )
        // 1バイト文字
    {
        c = *pChar;
        *(u32*)ppChar += 1;
    }
    else if( (*pChar & 0xE0) == 0xC0 )
        // 2バイト文字
    {
        c = (u16)( ((*(pChar + 0) & 0x1F) << 6) |
                   ((*(pChar + 1) & 0x3F) << 0) );
        *(u32*)ppChar += 2;
    }
    else
        // 3バイト文字
    {
    	// 4バイト以上には対応しない
        NNS_G2D_ASSERT( (*pChar & 0xF0) == 0xE0 );

        c = (u16)(  ((*(pChar + 0) & 0x1F) << 12) |
                    ((*(pChar + 1) & 0x3F) <<  6) |
                    ((*(pChar + 2) & 0x3F) <<  0) );
        *(u32*)ppChar += 3;
    }

    return c;
}

// Shift_JIS
u16 NNSi_G2dSplitCharShiftJIS(const void** ppChar)
{
    const u8* pChar;
    u16 c;

    NNS_G2D_POINTER_ASSERT( ppChar );
    NNS_G2D_POINTER_ASSERT( *ppChar );

    pChar = (const u8*)*ppChar;

    if( IsSjisLeadByte(*pChar) )
    {
        c = (u16)( ((*pChar) << 8) | (*(pChar + 1)) );
        *(u32*)ppChar += 2;
    }
    else
    {
        c = *pChar;
        *(u32*)ppChar += 1;
    }

    return c;
}

// CP1252, etc.
u16 NNSi_G2dSplitChar1Byte(const void** ppChar)
{
    const u8* pChar;
    u16 c;

    NNS_G2D_POINTER_ASSERT( ppChar );
    NNS_G2D_POINTER_ASSERT( *ppChar );

    pChar = (const u8*)*ppChar;

    c = *pChar;
    *(u32*)ppChar += 1;

    return c;
}
