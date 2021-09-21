/*---------------------------------------------------------------------------*
  Project:  NitroSDK - SND - libraries
  File:     snd_bank.c

  Copyright 2004-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: snd_bank.c,v $
  Revision 1.11  2006/01/18 02:12:29  kitase_hirotake
  do-indent

  Revision 1.10  2005/05/10 01:52:16  ida
  SND_BANK_DISABLE_RELEASEの追加

  Revision 1.9  2005/05/10 00:55:10  ida
  リリース255の時ノート長を無限とするようにした。

  Revision 1.8  2005/04/12 06:16:12  ida
  SNDをNITRO-SDKへ移設

  Revision 1.21  2005/03/17 04:25:36  ida
  SND_INST_NULL追加

  Revision 1.20  2005/02/21 02:37:17  ida
  警告対策

  Revision 1.19  2005/02/02 05:09:53  ida
  for SDK_FROM_TOOL

  Revision 1.18  2005/01/24 05:52:19  ida
  波形アーカイブ編集後のキャッシュストア追加

  Revision 1.17  2005/01/19 23:45:33  ida
  コンパイルエラー対処

  Revision 1.16  2005/01/19 06:19:58  ida
  外部参照波形アーカイブ実装関連の関数追加

  Revision 1.15  2004/11/22 04:30:17  ida
  workaround thread safe

  Revision 1.14  2004/07/16 06:42:49  ida
  fix Project name

  Revision 1.13  2004/07/13 05:42:55  ida
  change API name

  Revision 1.12  2004/07/09 06:50:48  ida
  change SND_NoteOn interface

  Revision 1.11  2004/07/07 05:06:11  ida
  add SND_WriteInstData

  Revision 1.10  2004/07/06 07:46:44  ida
  workaround multi wave archive for one bank

  Revision 1.9  2004/07/05 06:30:05  ida
  add assert

  Revision 1.8  2004/06/23 06:26:17  ida
  channel -> exchannel

  Revision 1.7  2004/06/01 00:19:14  ida
  add warning

  Revision 1.6  2004/05/26 04:18:38  ida
  check multiple assign wave archive

  Revision 1.5  2004/05/13 01:03:01  ida
  fix comments

  Revision 1.4  2004/05/12 06:39:58  ida
  modify data header structure

  Revision 1.3  2004/04/30 00:40:08  ida
  fix infinity loop
  add comments

  Revision 1.2  2004/04/26 02:54:55  ida
  add SND_Disposer*()

  Revision 1.1  2004/03/26 06:25:52  ida
  create snd_drv

  Revision 1.6  2004/03/24 06:21:43  ida
  add envelope command

  Revision 1.5  2004/03/12 02:17:06  ida
  untabify

  Revision 1.4  2004/03/11 01:52:13  ida
  use SND_CopyMemory16

  Revision 1.3  2004/03/10 06:32:08  ida
  SND_Snd -> SND_

  Revision 1.2  2004/03/08 05:40:27  ida
  rename files

  Revision 1.1  2004/03/08 03:02:50  ida
  ARM7/src -> common/src

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro/snd/common/bank.h>

#ifndef SDK_FROM_TOOL

#include <nitro/misc.h>
#include <nitro/mi.h>
#include <nitro/snd/common/main.h>

#else

#define SDK_ASSERT(exp)           ((void) 0)
#define SDK_NULL_ASSERT(exp)           ((void) 0)
#define SDK_MINMAX_ASSERT(exp, min, max)           ((void) 0)

#define SNDi_LockMutex() ((void) 0)
#define SNDi_UnlockMutex() ((void) 0)

static u8 MI_ReadByte(const void *address)
{
    return *(u8 *)address;
}

#define HW_MAIN_MEM             0x02000000

#ifdef _MSC_VER
#pragma warning( disable : 4018 )      // warning: signed/unsigned mismatch
#pragma warning( disable : 4311 )      // warning: pointer truncation from 'type' to 'type'
#pragma warning( disable : 4312 )      // warning: conversion from 'type' to 'type' of greater size
#endif

#endif /* SDK_FROM_TOOL */

/******************************************************************************
	static function declarations
 ******************************************************************************/
static const SNDWaveData *GetWaveData(const SNDBankData *bank, int waveArcNo, int waveIndex);

/******************************************************************************
	public functions
 ******************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         SND_AssignWaveArc

  Description:  バンクに波形アーカイブを割り付けます(ThreadSafe)

  Arguments:    bank    - pointer to SNDBankData structure
                index   - wave archive index
                waveArc - pointer to SNDWaveArc structure

  Returns:      None.
 *---------------------------------------------------------------------------*/
void SND_AssignWaveArc(SNDBankData *bank, int index, SNDWaveArc *waveArc)
{
    SNDWaveArcLink *next;
    SNDWaveArcLink *prev;

    SDK_NULL_ASSERT(bank);
    SDK_NULL_ASSERT(waveArc);
#ifdef SDK_ARM9
    SDK_ASSERTMSG(((u32)bank & 0x1f) == 0, "bank address must be aligned 32 bytes boundary.");
    SDK_ASSERTMSG(((u32)waveArc & 0x1f) == 0, "waveArc address must be aligned 32 bytes boundary.");
#endif
    SDK_MINMAX_ASSERT(index, 0, SND_BANK_TO_WAVEARC_MAX - 1);

    SNDi_LockMutex();

    if (bank->waveArcLink[index].waveArc != NULL)
    {
        // すでに接続済みの場合

        // 接続しようとしている波形アーカイブと同じだったら、何もしない
        if (waveArc == bank->waveArcLink[index].waveArc)
        {
            SNDi_UnlockMutex();
            return;
        }

        // 違う波形アーカイブが接続されていたら、一旦接続を切断する
        if (&bank->waveArcLink[index] == bank->waveArcLink[index].waveArc->topLink)
        {
            bank->waveArcLink[index].waveArc->topLink = bank->waveArcLink[index].next;

#ifdef SDK_ARM9
            DC_StoreRange(bank->waveArcLink[index].waveArc, sizeof(SNDWaveArc));
#endif
        }
        else
        {
            prev = bank->waveArcLink[index].waveArc->topLink;
            while (prev != NULL)
            {
                if (&bank->waveArcLink[index] == prev->next)
                    break;
                prev = prev->next;
            }
            SDK_NULL_ASSERT(prev);
            prev->next = bank->waveArcLink[index].next;

#ifdef SDK_ARM9
            DC_StoreRange(prev, sizeof(SNDWaveArcLink));
#endif
        }
    }

    next = waveArc->topLink;
    waveArc->topLink = &bank->waveArcLink[index];
    bank->waveArcLink[index].next = next;
    bank->waveArcLink[index].waveArc = waveArc;

    SNDi_UnlockMutex();

#ifdef SDK_ARM9
    DC_StoreRange(bank, sizeof(SNDBankData));
    DC_StoreRange(waveArc, sizeof(SNDWaveArc));
#endif
}

/*---------------------------------------------------------------------------*
  Name:         SND_DestroyBank

  Description:  バンクを破棄します(ThreadSafe)

  Arguments:    bank - pointer to SNDBankData structure

  Returns:      None.
 *---------------------------------------------------------------------------*/
void SND_DestroyBank(SNDBankData *bank)
{
    SNDWaveArc *waveArc;
    SNDWaveArcLink *prev;
    int     i;

    SDK_NULL_ASSERT(bank);
#ifdef SDK_ARM9
    SDK_ASSERTMSG(((u32)bank & 0x1f) == 0, "bank address must be aligned 32 bytes boundary.");
#endif

    SNDi_LockMutex();

    for (i = 0; i < SND_BANK_TO_WAVEARC_MAX; i++)
    {
        waveArc = bank->waveArcLink[i].waveArc;
        if (waveArc == NULL)
            continue;

        if (&bank->waveArcLink[i] == waveArc->topLink)
        {
            waveArc->topLink = bank->waveArcLink[i].next;

#ifdef SDK_ARM9
            DC_StoreRange(waveArc, sizeof(SNDWaveArc));
#endif
        }
        else
        {
            prev = waveArc->topLink;
            while (prev != NULL)
            {
                if (&bank->waveArcLink[i] == prev->next)
                    break;
                prev = prev->next;
            }
            SDK_NULL_ASSERT(prev);
            prev->next = bank->waveArcLink[i].next;

#ifdef SDK_ARM9
            DC_StoreRange(prev, sizeof(SNDWaveArcLink));
#endif
        }
    }

    SNDi_UnlockMutex();
}

/*---------------------------------------------------------------------------*
  Name:         SND_DestroyWaveArc

  Description:  波形アーカイブを破棄します。(ThreadSafe)

  Arguments:    waveArc - pointer to SNDWaveArc structure

  Returns:      None.
 *---------------------------------------------------------------------------*/
void SND_DestroyWaveArc(SNDWaveArc *waveArc)
{
    SNDWaveArcLink *link;
    SNDWaveArcLink *next;

    SDK_NULL_ASSERT(waveArc);
#ifdef SDK_ARM9
    SDK_ASSERTMSG(((u32)waveArc & 0x1f) == 0, "waveArc address must be aligned 32 bytes boundary.");
#endif

    SNDi_LockMutex();

    link = waveArc->topLink;
    while (link != NULL)
    {
        next = link->next;

        link->waveArc = NULL;
        link->next = NULL;

#ifdef SDK_ARM9
        DC_StoreRange(link, sizeof(SNDWaveArcLink));
#endif
        link = next;
    }

    SNDi_UnlockMutex();
}


/*---------------------------------------------------------------------------*
  Name:         SND_ReadInstData

  Description:  バンクからインストパラメータを取得します(ThreadSafe)

  Arguments:    bank - pointer to SNDBankData structure
                prgNo - program number
                key - note key
                inst - pointer to SNDInstData structure

  Returns:      成功したかどうか
 *---------------------------------------------------------------------------*/
BOOL SND_ReadInstData(const SNDBankData *bank, int prgNo, int key, SNDInstData *inst)
{
    u32     instOffset;

    SDK_NULL_ASSERT(bank);
    SDK_NULL_ASSERT(inst);

    if (prgNo < 0)
        return FALSE;

#ifdef SDK_FROM_TOOL
    if (bank->fileHeader.signature[0] == 'S' &&
        bank->fileHeader.signature[1] == 'B' &&
        bank->fileHeader.signature[2] == 'C' && bank->fileHeader.signature[3] == 'B')
    {
        const SNDBankDataCallback *bankcb = (const SNDBankDataCallback *)bank;
        return bankcb->readInstDataFunc(bankcb, prgNo, key, inst);
    }
#endif

    SNDi_LockMutex();

    if (prgNo >= bank->instCount)
    {                                  // Note: read from MainMemory
        SNDi_UnlockMutex();
        return FALSE;
    }

    instOffset = bank->instOffset[prgNo];       // Note: read from MainMemory
    inst->type = (u8)(instOffset & 0xff);
    instOffset >>= 8;

    // インストタイプ毎の処理
    switch (inst->type)
    {
    case SND_INST_PCM:
    case SND_INST_PSG:
    case SND_INST_NOISE:
    case SND_INST_DIRECTPCM:
    case SND_INST_NULL:
        {
            const SNDInstParam *param = (const SNDInstParam *)((u8 *)bank + instOffset);

            inst->param = *param;      // structure copy
            break;
        }

    case SND_INST_DRUM_SET:
        {
            const SNDDrumSet *drumSet = (const SNDDrumSet *)((u8 *)bank + instOffset);
            u8      min = MI_ReadByte(&drumSet->min);
            u8      max = MI_ReadByte(&drumSet->max);

            if (key < min || key > max)
            {
                SNDi_UnlockMutex();
                return FALSE;
            }

            *inst = drumSet->instOffset[key - min];     // structure copy
            break;
        }

    case SND_INST_KEY_SPLIT:
        {
            int     index = 0;
            const SNDKeySplit *keySplit = (const SNDKeySplit *)((u8 *)bank + instOffset);

            while (key > MI_ReadByte(&keySplit->key[index]))
            {
                index++;
                if (index >= SND_INST_KEYSPLIT_MAX)
                {
                    SNDi_UnlockMutex();
                    return FALSE;
                }
            }

            *inst = keySplit->instOffset[index];        // structure copy
            break;
        }

    case SND_INST_INVALID:
    default:
        SNDi_UnlockMutex();
        return FALSE;
    }

    SNDi_UnlockMutex();

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         SND_WriteInstData

  Description:  インストデータを書き込みます(ThreadSafe)

  Arguments:    bank - pointer to SNDBankData structure
                prgNo - program number
                key - note key
                inst - pointer to SNDInstData structure

  Returns:      成功したかどうかを返します
 *---------------------------------------------------------------------------*/
BOOL SND_WriteInstData(SNDBankData *bank, int prgNo, int key, const struct SNDInstData *inst)
{
    u32     instOffset;
    u8      type;

    SDK_NULL_ASSERT(bank);
    SDK_NULL_ASSERT(inst);

    if (prgNo < 0)
        return FALSE;

    SNDi_LockMutex();
    if (prgNo >= bank->instCount)
    {                                  // Note: read from MainMemory
        SNDi_UnlockMutex();
        return FALSE;
    }

    instOffset = bank->instOffset[prgNo];       // Note: read from MainMemory
    type = (u8)(instOffset & 0xff);
    instOffset >>= 8;

    // インストタイプ毎の処理
    switch (type)
    {
    case SND_INST_PCM:
    case SND_INST_PSG:
    case SND_INST_NOISE:
    case SND_INST_DIRECTPCM:
    case SND_INST_NULL:
        {
            SNDInstParam *param = (SNDInstParam *)((u8 *)bank + instOffset);

            bank->instOffset[prgNo] = (instOffset << 8) | inst->type;
            *param = inst->param;      // structure copy
            break;
        }

    case SND_INST_DRUM_SET:
        {
            SNDDrumSet *drumSet = (SNDDrumSet *)((u8 *)bank + instOffset);
            u8      min = MI_ReadByte(&drumSet->min);
            u8      max = MI_ReadByte(&drumSet->max);

            if (key < min || key > max)
            {
                SNDi_UnlockMutex();
                return FALSE;
            }

            drumSet->instOffset[key - min] = *inst;     // structure copy
            break;
        }

    case SND_INST_KEY_SPLIT:
        {
            int     index = 0;
            SNDKeySplit *keySplit = (SNDKeySplit *)((u8 *)bank + instOffset);

            while (key > MI_ReadByte(&keySplit->key[index]))
            {
                index++;
                if (index >= SND_INST_KEYSPLIT_MAX)
                {
                    SNDi_UnlockMutex();
                    return FALSE;
                }
            }

            keySplit->instOffset[index] = *inst;        // structure copy
            break;
        }

    case SND_INST_INVALID:
    default:
        SNDi_UnlockMutex();
        return FALSE;
    }

    SNDi_UnlockMutex();

#ifdef SDK_ARM9
    DC_StoreRange(bank, bank->fileHeader.fileSize);
#endif

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         SND_GetFirstInstDataPos

  Description:  インストデータ走査のためのSNDInstPos取得

  Arguments:    bank - バンクデータのポインタ

  Returns:      SNDInstPos構造体
 *---------------------------------------------------------------------------*/
SNDInstPos SND_GetFirstInstDataPos(const SNDBankData *bank)
{
    SNDInstPos pos;

    SDK_NULL_ASSERT(bank);

    (void)bank;

    pos.prgNo = 0;
    pos.index = 0;

    return pos;
}

/*---------------------------------------------------------------------------*
  Name:         SND_GetNextInstData

  Description:  次のインストデータを取得する

  Arguments:    bank - バンクデータのポインタ
                inst - 取得したインストデータが格納されるポインタ
                pos - SNDInstPos構造体のポインタ

  Returns:      取得に成功したかどうか
 *---------------------------------------------------------------------------*/
BOOL SND_GetNextInstData(const SNDBankData *bank, SNDInstData *inst, SNDInstPos *pos)
{
    SDK_NULL_ASSERT(bank);
    SDK_NULL_ASSERT(inst);
    SDK_NULL_ASSERT(pos);

    for (; pos->prgNo < bank->instCount; pos->prgNo++, pos->index = 0)
    {
        u32     instOffset = bank->instOffset[pos->prgNo];
        inst->type = (u8)(instOffset & 0xff);
        instOffset >>= 8;

        switch (inst->type)
        {
        case SND_INST_PCM:
        case SND_INST_PSG:
        case SND_INST_NOISE:
        case SND_INST_DIRECTPCM:
        case SND_INST_NULL:
            {
                const SNDInstParam *param = (const SNDInstParam *)((u8 *)bank + instOffset);
                inst->param = *param;
                pos->prgNo++;
                return TRUE;
            }

        case SND_INST_DRUM_SET:
            {
                const SNDDrumSet *drumSet = (const SNDDrumSet *)((u8 *)bank + instOffset);
                for (; pos->index < drumSet->max - drumSet->min + 1; pos->index++)
                {
                    *inst = drumSet->instOffset[pos->index];
                    pos->index++;
                    return TRUE;
                }
                break;
            }

        case SND_INST_KEY_SPLIT:
            {
                const SNDKeySplit *keySplit = (const SNDKeySplit *)((u8 *)bank + instOffset);
                for (; pos->index < SND_INST_KEYSPLIT_MAX; pos->index++)
                {
                    if (keySplit->key[pos->index] == 0)
                        break;
                    *inst = keySplit->instOffset[pos->index];
                    pos->index++;
                    return TRUE;
                }
                break;
            }

        default:
            break;
        }
    }

    return FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         SND_GetWaveDataCount

  Description:  波形アーカイブ中の波形データの個数を取得

  Arguments:    waveArc - 波形アーカイブのポインタ

  Returns:      波形データの個数
 *---------------------------------------------------------------------------*/
u32 SND_GetWaveDataCount(const struct SNDWaveArc *waveArc)
{
    SDK_NULL_ASSERT(waveArc);

    return waveArc->waveCount;
}

/*---------------------------------------------------------------------------*
  Name:         SND_SetWaveDataAddress

  Description:  波形アーカイブに波形データを登録

  Arguments:    waveArc - 波形アーカイブのポインタ
                index - 波形データのインデックス番号
                address - 波形データのアドレス

  Returns:      None.
 *---------------------------------------------------------------------------*/
void SND_SetWaveDataAddress(SNDWaveArc *waveArc, int index, const SNDWaveData *address)
{
    SDK_NULL_ASSERT(waveArc);
    SDK_MINMAX_ASSERT(index, 0, waveArc->waveCount);
    SDK_ASSERT(address == NULL || address >= (void *)HW_MAIN_MEM);

    SNDi_LockMutex();

    waveArc->waveOffset[index] = (u32)address;

#ifdef SDK_ARM9
    DC_StoreRange(&waveArc->waveOffset[index], sizeof(u32));
#endif

    SNDi_UnlockMutex();
}

/*---------------------------------------------------------------------------*
  Name:         SND_GetWaveDataAddress

  Description:  波形アーカイブに登録されている波形データを取得

  Arguments:    waveArc - 波形アーカイブのポインタ
                index - 波形データのインデックス番号

  Returns:      取得した波形データのポインタ
 *---------------------------------------------------------------------------*/
const SNDWaveData *SND_GetWaveDataAddress(const SNDWaveArc *waveArc, int index)
{
    const SNDWaveData *wave;
    u32     offset;

    SDK_NULL_ASSERT(waveArc);
    SDK_MINMAX_ASSERT(index, 0, waveArc->waveCount);

    SNDi_LockMutex();

    offset = waveArc->waveOffset[index];

    if (offset != 0)
    {
        if (offset < HW_MAIN_MEM)
            wave = (const SNDWaveData *)((u8 *)waveArc + offset);
        else
            wave = (const SNDWaveData *)offset;
    }
    else
    {
        wave = NULL;
    }

    SNDi_UnlockMutex();

    return wave;
}

//-----------------------------------------------------------------------------
// ARM7 only

#ifdef SDK_ARM7

/*---------------------------------------------------------------------------*
  Name:         SND_NoteOn

  Description:  バンクを使って、ノートオンを行います

  Arguments:    

  Returns:      pointer to SNDChannel structure
 *---------------------------------------------------------------------------*/
BOOL SND_NoteOn(SNDExChannel *ch_p,
                int key, int velocity, s32 length, const SNDBankData *bank, const SNDInstData *inst)
{
    const SNDWaveData *wave_data;
    int     release;
    BOOL    result;

    SDK_NULL_ASSERT(ch_p);
    SDK_MINMAX_ASSERT(key, 0, 127);
    SDK_MINMAX_ASSERT(velocity, 0, 127);
    SDK_NULL_ASSERT(inst);

    release = inst->param.release;
    if (inst->param.release == SND_BANK_DISABLE_RELEASE)
    {
        length = -1;
        release = 0;
    }

    switch (inst->type)
    {
    case SND_INST_PCM:
    case SND_INST_DIRECTPCM:
        if (inst->type == SND_INST_PCM)
        {
            wave_data = GetWaveData(bank, inst->param.wave[1], inst->param.wave[0]);
        }
        else
        {
            wave_data = (const SNDWaveData *)(inst->param.wave[1] << 16 | inst->param.wave[0]);
        }

        if (wave_data != NULL)
        {
            result = SND_StartExChannelPcm(ch_p, &wave_data->param, wave_data->samples, length);
        }
        else
        {
            result = FALSE;
        }
        break;

    case SND_INST_PSG:
        result = SND_StartExChannelPsg(ch_p, (SNDDuty)inst->param.wave[0], length);
        break;

    case SND_INST_NOISE:
        result = SND_StartExChannelNoise(ch_p, length);
        break;

    default:
        result = FALSE;
        break;
    }

    if (!result)
        return FALSE;

    ch_p->key = (u8)key;
    ch_p->original_key = inst->param.original_key;
    ch_p->velocity = (u8)velocity;

    SND_SetExChannelAttack(ch_p, inst->param.attack);
    SND_SetExChannelDecay(ch_p, inst->param.decay);
    SND_SetExChannelSustain(ch_p, inst->param.sustain);
    SND_SetExChannelRelease(ch_p, release);

    ch_p->init_pan = (s8)(inst->param.pan - 64);

    return TRUE;
}

#endif

/******************************************************************************
	static functions
 ******************************************************************************/

//-----------------------------------------------------------------------------
// ARM7 only

#ifdef SDK_ARM7

/*---------------------------------------------------------------------------*
  Name:         GetWaveData

  Description:  波形アーカイブから波形データを取得します

  Arguments:    bank - pointer to SNDBankData
                waveArcNo - wave archive number
                waveIndex - wave index

  Returns:      pointer to SNDWaveData structure
 *---------------------------------------------------------------------------*/
static const SNDWaveData *GetWaveData(const SNDBankData *bank, int waveArcNo, int waveIndex)
{
    const SNDWaveArc *arc;

    SDK_MINMAX_ASSERT(waveArcNo, 0, SND_BANK_TO_WAVEARC_MAX - 1);

    arc = bank->waveArcLink[waveArcNo].waveArc;

    if (arc == NULL)
        return NULL;
    if (waveIndex >= arc->waveCount)
        return NULL;

    return SND_GetWaveDataAddress(arc, waveIndex);
}

#endif

/*====== End of snd_bank.c ======*/
