/*---------------------------------------------------------------------------*
  Project:  NitroSDK - GX - 
  File:     g3c.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g3c.c,v $
  Revision 1.45  2006/01/18 02:11:21  kitase_hirotake
  do-indent

  Revision 1.44  2005/08/10 08:57:17  yasu
  g3b.c g3c.c を PC 上でコンパイルできるように修正

  Revision 1.43  2005/05/31 11:01:32  takano_makoto
  G3C_UpdateGXDLInfoのASSERT条件を変更

  Revision 1.42  2005/03/01 01:57:00  yosizaki
  copyright の年を修正.

  Revision 1.41  2005/02/28 05:26:13  yosizaki
  do-indent.

  Revision 1.40  2004/11/02 15:06:02  takano_makoto
  fix comments

  Revision 1.39  2004/09/21 03:26:46  takano_makoto
  fix a bug in G3C_UpdateGXDLInfo().

  Revision 1.38  2004/09/03 05:17:03  takano_makoto
  fix code for hardware bug in G3CS_Direct0 .

  Revision 1.37  2004/08/30 11:34:12  takano_makoto
  Add code for hardware bug in packed geometry command.

  Revision 1.36  2004/08/02 07:30:11  takano_makoto
  Add G3*_MultTransMtx33

  Revision 1.35  2004/03/17 07:25:27  yasu
  do sdk_indent

  Revision 1.34  2004/02/18 01:15:27  yasu
  workaround for avoiding byte access

  Revision 1.33  2004/02/17 12:55:16  nishida_kenji
  small fix.

  Revision 1.32  2004/02/17 10:43:52  nishida_kenji
  small fix.

  Revision 1.31  2004/02/12 07:06:27  nishida_kenji
  Avoid generating STRB in THUMB mode.

  Revision 1.30  2004/02/10 06:12:15  kitani_toshikazu
  Add codes to check the DL buffer's overflow.

  Revision 1.29  2004/02/10 06:03:15  nishida_kenji
  Move block copy functions to MI.

  Revision 1.28  2004/02/09 11:09:56  nishida_kenji
  Use functions at gxasm.h.

  Revision 1.27  2004/02/09 06:34:24  nishida_kenji
  G3CS_Direct[0-3] not inlined to avoid byte write problem in thumb mode.

  Revision 1.26  2004/02/05 07:09:02  yasu
  change SDK prefix iris -> nitro

  Revision 1.25  2004/02/04 06:28:57  nishida_kenji
  Add comments.

  Revision 1.23  2004/01/28 00:28:18  nishida_kenji
  G3C_xxxx not inlined.

  Revision 1.22  2003/12/17 09:00:20  nishida_kenji
  Totally revised APIs.
  build/buildtools/GX_APIChangeFrom031212.pl would help you change your program to some extent.

  Revision 1.21  2003/12/17 08:53:17  nishida_kenji
  revise comments

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
//
//      -DSDK_WIN32 -DSDK_HAS_LONG_LONG_INT for GCC
//      -DSDK_WIN32                         for VC or BCB
//
#ifdef   SDK_WIN32
#include <nitro_win32.h>
#else
#include <nitro/gx/g3c.h>
#include <nitro/mi/dma.h>
#include <nitro/mi/memory.h>
#include "../include/gxasm.h"
#endif

#include <nitro/code32.h>              // includes byte access
/*---------------------------------------------------------------------------*
  Name:         G3CS_Direct0

  Description:  Makes a geometry command with no parameters on a display list.
                Note that the command is packed.

  Arguments:    info       a pointer to GXDLInfo
                op         a geometry command(GXOP_xxxxxxx)

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3CS_Direct0(GXDLInfo *info, int op)
{
    SDK_NULL_ASSERT(info);
    GX_VALID_OPCODE_ASSERT(op);

    if (op != G3OP_NOP && ((u32)info->curr_cmd & 0x3) != 0)
    {
        info->param0_cmd_flg = TRUE;
    }

    *info->curr_cmd = (u8)(op);        // byte access
}


/*---------------------------------------------------------------------------*
  Name:         G3CS_Direct1

  Description:  Makes a geometry command with a parameter on a display list.
                Note that the command is packed.

  Arguments:    info       a pointer to GXDLInfo
                op         a geometry command(GXOP_xxxxxxx)
                param0     a parameter

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3CS_Direct1(GXDLInfo *info, int op, u32 param0)
{
    SDK_NULL_ASSERT(info);
    GX_VALID_OPCODE_ASSERT(op);

    info->param0_cmd_flg = FALSE;

    *info->curr_cmd = (u8)(op);        // byte access
    *info->curr_param = param0;
}


/*---------------------------------------------------------------------------*
  Name:         G3CS_Direct2

  Description:  Makes a geometry command with two parameters on a display list.
                Note that the command is packed.

  Arguments:    info       a pointer to GXDLInfo
                op         a geometry command(GXOP_xxxxxxx)
                param0     a parameter
                param1     a parameter

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3CS_Direct2(GXDLInfo *info, int op, u32 param0, u32 param1)
{
    SDK_NULL_ASSERT(info);
    GX_VALID_OPCODE_ASSERT(op);

    info->param0_cmd_flg = FALSE;

    *info->curr_cmd = (u8)(op);        // byte access
    *(info->curr_param + 0) = param0;
    *(info->curr_param + 1) = param1;
}


/*---------------------------------------------------------------------------*
  Name:         G3CS_Direct3

  Description:  Makes a geometry command with three parameters on a display list.
                Note that the command is packed.

  Arguments:    info       a pointer to GXDLInfo
                op         a geometry command(GXOP_xxxxxxx)
                param0     a parameter
                param1     a parameter
                param2     a parameter


  Returns:      none
 *---------------------------------------------------------------------------*/
void G3CS_Direct3(GXDLInfo *info, int op, u32 param0, u32 param1, u32 param2)
{
    SDK_NULL_ASSERT(info);
    GX_VALID_OPCODE_ASSERT(op);

    info->param0_cmd_flg = FALSE;

    *info->curr_cmd = (u8)(op);        // byte access
    *(info->curr_param + 0) = param0;
    *(info->curr_param + 1) = param1;
    *(info->curr_param + 2) = param2;
}


/*---------------------------------------------------------------------------*
  Name:         G3CS_DirectN

  Description:  Makes a geometry command with nParams parameters on a display list.
                Note that the command is packed.

  Arguments:    info       a pointer to GXDLInfo
                op         a geometry command(GXOP_xxxxxxx)
                nParams    the number of parameters
                params     parameters

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3CS_DirectN(GXDLInfo *info, int op, int nParams, const u32 *params)
{
    SDK_NULL_ASSERT(info);
    SDK_NULL_ASSERT(params);
    SDK_NULL_ASSERT(info->curr_cmd);
    SDK_NULL_ASSERT(info->curr_param);

    if (nParams == 0)
    {
        G3CS_Direct0(info, op);
        return;
    }

    info->param0_cmd_flg = FALSE;

    *info->curr_cmd = (u8)(op);        // byte access
    while (--nParams >= 0)
    {
        *(info->curr_param + nParams) = *(params + nParams);
    }
}


/*---------------------------------------------------------------------------*
  Name:         G3CS_LoadMtx44

  Description:  Make a geometry command which loads a 4x4 matrix
                to the current matrix.

  Arguments:    info       a pointer to GXDLInfo
                m          a pointer to a constant 4x4 matrix

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3CS_LoadMtx44(GXDLInfo *info, const MtxFx44 *m)
{
    SDK_NULL_ASSERT(info);
    SDK_NULL_ASSERT(info->curr_cmd);
    SDK_NULL_ASSERT(info->curr_param);
    SDK_NULL_ASSERT(m);

    info->param0_cmd_flg = FALSE;

    *info->curr_cmd = G3OP_MTX_LOAD_4x4;        // byte access
#ifndef SDK_WIN32
    MI_Copy64B(&m->_00, (void *)info->curr_param);
#else
    *(info->curr_param + 0) = (u32)m->_00;
    *(info->curr_param + 1) = (u32)m->_01;
    *(info->curr_param + 2) = (u32)m->_02;
    *(info->curr_param + 3) = (u32)m->_03;

    *(info->curr_param + 4) = (u32)m->_10;
    *(info->curr_param + 5) = (u32)m->_11;
    *(info->curr_param + 6) = (u32)m->_12;
    *(info->curr_param + 7) = (u32)m->_13;

    *(info->curr_param + 8) = (u32)m->_20;
    *(info->curr_param + 9) = (u32)m->_21;
    *(info->curr_param + 10) = (u32)m->_22;
    *(info->curr_param + 11) = (u32)m->_23;

    *(info->curr_param + 12) = (u32)m->_30;
    *(info->curr_param + 13) = (u32)m->_31;
    *(info->curr_param + 14) = (u32)m->_32;
    *(info->curr_param + 15) = (u32)m->_33;
#endif
}


/*---------------------------------------------------------------------------*
  Name:         G3CS_LoadMtx43

  Description:  Make a geometry command which loads a 4x3 matrix
                to the current matrix.

  Arguments:    info       a pointer to GXDLInfo
                m          a pointer to a constant 4x3 matrix

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3CS_LoadMtx43(GXDLInfo *info, const MtxFx43 *m)
{
    SDK_NULL_ASSERT(info);
    SDK_NULL_ASSERT(info->curr_cmd);
    SDK_NULL_ASSERT(info->curr_param);
    SDK_NULL_ASSERT(m);

    info->param0_cmd_flg = FALSE;

    *info->curr_cmd = G3OP_MTX_LOAD_4x3;        // byte access
#ifndef SDK_WIN32
    MI_Copy48B(&m->_00, (void *)info->curr_param);
#else
    *(info->curr_param + 0) = (u32)m->_00;
    *(info->curr_param + 1) = (u32)m->_01;
    *(info->curr_param + 2) = (u32)m->_02;

    *(info->curr_param + 3) = (u32)m->_10;
    *(info->curr_param + 4) = (u32)m->_11;
    *(info->curr_param + 5) = (u32)m->_12;

    *(info->curr_param + 6) = (u32)m->_20;
    *(info->curr_param + 7) = (u32)m->_21;
    *(info->curr_param + 8) = (u32)m->_22;

    *(info->curr_param + 9) = (u32)m->_30;
    *(info->curr_param + 10) = (u32)m->_31;
    *(info->curr_param + 11) = (u32)m->_32;
#endif
}


/*---------------------------------------------------------------------------*
  Name:         G3CS_MultMtx44

  Description:  Makes a geometry command which multiplies the current matrix
                by a 4x4 matrix from the left.

  Arguments:    info       a pointer to GXDLInfo
                m          a pointer to a constant 4x4 matrix

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3CS_MultMtx44(GXDLInfo *info, const MtxFx44 *m)
{
    SDK_NULL_ASSERT(info);
    SDK_NULL_ASSERT(info->curr_cmd);
    SDK_NULL_ASSERT(info->curr_param);
    SDK_NULL_ASSERT(m);

    info->param0_cmd_flg = FALSE;

    *info->curr_cmd = G3OP_MTX_MULT_4x4;        // byte access
#ifndef SDK_WIN32
    MI_Copy64B(&m->_00, (void *)info->curr_param);
#else
    *(info->curr_param + 0) = (u32)m->_00;
    *(info->curr_param + 1) = (u32)m->_01;
    *(info->curr_param + 2) = (u32)m->_02;
    *(info->curr_param + 3) = (u32)m->_03;

    *(info->curr_param + 4) = (u32)m->_10;
    *(info->curr_param + 5) = (u32)m->_11;
    *(info->curr_param + 6) = (u32)m->_12;
    *(info->curr_param + 7) = (u32)m->_13;

    *(info->curr_param + 8) = (u32)m->_20;
    *(info->curr_param + 9) = (u32)m->_21;
    *(info->curr_param + 10) = (u32)m->_22;
    *(info->curr_param + 11) = (u32)m->_23;

    *(info->curr_param + 12) = (u32)m->_30;
    *(info->curr_param + 13) = (u32)m->_31;
    *(info->curr_param + 14) = (u32)m->_32;
    *(info->curr_param + 15) = (u32)m->_33;
#endif
}


/*---------------------------------------------------------------------------*
  Name:         G3CS_MultMtx43

  Description:  Make a geometry command which multiplies the current matrix
                by a 4x3 matrix from the left.

  Arguments:    info       a pointer to GXDLInfo
                m          a pointer to a constant 4x3 matrix

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3CS_MultMtx43(GXDLInfo *info, const MtxFx43 *m)
{
    SDK_NULL_ASSERT(info);
    SDK_NULL_ASSERT(info->curr_cmd);
    SDK_NULL_ASSERT(info->curr_param);
    SDK_NULL_ASSERT(m);

    info->param0_cmd_flg = FALSE;

    *info->curr_cmd = G3OP_MTX_MULT_4x3;        // byte access
#ifndef SDK_WIN32
    MI_Copy48B(&m->_00, (void *)info->curr_param);
#else
    *(info->curr_param + 0) = (u32)m->_00;
    *(info->curr_param + 1) = (u32)m->_01;
    *(info->curr_param + 2) = (u32)m->_02;

    *(info->curr_param + 3) = (u32)m->_10;
    *(info->curr_param + 4) = (u32)m->_11;
    *(info->curr_param + 5) = (u32)m->_12;

    *(info->curr_param + 6) = (u32)m->_20;
    *(info->curr_param + 7) = (u32)m->_21;
    *(info->curr_param + 8) = (u32)m->_22;

    *(info->curr_param + 9) = (u32)m->_30;
    *(info->curr_param + 10) = (u32)m->_31;
    *(info->curr_param + 11) = (u32)m->_32;
#endif
}


/*---------------------------------------------------------------------------*
  Name:         G3CS_MultMtx33

  Description:  Makes a geometry command which multiplies the current matrix
                by a 3x3 matrix from the left.

  Arguments:    info       a pointer to GXDLInfo
                m          a pointer to a constant 3x3 matrix

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3CS_MultMtx33(GXDLInfo *info, const MtxFx33 *m)
{
    SDK_NULL_ASSERT(info);
    SDK_NULL_ASSERT(info->curr_cmd);
    SDK_NULL_ASSERT(info->curr_param);
    SDK_NULL_ASSERT(m);

    info->param0_cmd_flg = FALSE;

    *info->curr_cmd = G3OP_MTX_MULT_3x3;        // byte access
#ifndef SDK_WIN32
    MI_Copy36B(&m->_00, (void *)info->curr_param);
#else
    *(info->curr_param + 0) = (u32)m->_00;
    *(info->curr_param + 1) = (u32)m->_01;
    *(info->curr_param + 2) = (u32)m->_02;

    *(info->curr_param + 3) = (u32)m->_10;
    *(info->curr_param + 4) = (u32)m->_11;
    *(info->curr_param + 5) = (u32)m->_12;

    *(info->curr_param + 6) = (u32)m->_20;
    *(info->curr_param + 7) = (u32)m->_21;
    *(info->curr_param + 8) = (u32)m->_22;
#endif
}

/*---------------------------------------------------------------------------*
  Name:         G3CS_MultTransMtx33

  Description:  Make a geometry command which multiplies and translates matrix together.

  Arguments:    info       a pointer to GXDLInfo
                mtx        a pointer to a constant 3x3 matrix
                trans      a pointer to a constant translation vector

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3CS_MultTransMtx33(GXDLInfo *info, const MtxFx33 *mtx, const VecFx32 *trans)
{
    SDK_NULL_ASSERT(info);
    SDK_NULL_ASSERT(mtx);
    SDK_NULL_ASSERT(trans);
    SDK_NULL_ASSERT(info->curr_cmd);
    SDK_NULL_ASSERT(info->curr_param);

    info->param0_cmd_flg = FALSE;

    *info->curr_cmd = G3OP_MTX_MULT_4x3;
#ifndef SDK_WIN32
    MI_Copy36B(&mtx->_00, (void *)info->curr_param);
    MI_CpuCopy32(trans, (u32 *)info->curr_param + 9, sizeof(VecFx32));
#else
    *((u32 *)info->curr_param + 0) = (u32)mtx->_00;
    *((u32 *)info->curr_param + 1) = (u32)mtx->_01;
    *((u32 *)info->curr_param + 2) = (u32)mtx->_02;

    *((u32 *)info->curr_param + 3) = (u32)mtx->_10;
    *((u32 *)info->curr_param + 4) = (u32)mtx->_11;
    *((u32 *)info->curr_param + 5) = (u32)mtx->_12;

    *((u32 *)info->curr_param + 6) = (u32)mtx->_20;
    *((u32 *)info->curr_param + 7) = (u32)mtx->_21;
    *((u32 *)info->curr_param + 8) = (u32)mtx->_22;

    *((u32 *)info->curr_param + 9) = trans->x;
    *((u32 *)info->curr_param + 10) = trans->y;
    *((u32 *)info->curr_param + 11) = trans->z;
#endif
}



#include <nitro/codereset.h>


/*---------------------------------------------------------------------------*
  Name:         G3C_Direct0

  Description:  Makes a geometry command with a parameter on a display list.
                Note that the command is packed.
                Also, the pointers of GXDLInfo proceed.

  Arguments:    info       a pointer to GXDLInfo
                op         a geometry command(GXOP_xxxxxxx)
                param0     a parameter

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3C_Direct0(GXDLInfo *info, int op)
{
    G3CS_Direct0(info, op);
    G3C_UpdateGXDLInfo(info, 0);
}


/*---------------------------------------------------------------------------*
  Name:         G3C_Direct1

  Description:  Makes a geometry command with a parameter on a display list.
                Note that the command is packed.
                Also, the pointers of GXDLInfo proceed.

  Arguments:    info       a pointer to GXDLInfo
                op         a geometry command(GXOP_xxxxxxx)
                param0     a parameter

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3C_Direct1(GXDLInfo *info, int op, u32 param0)
{
    G3CS_Direct1(info, op, param0);

    G3C_UpdateGXDLInfo(info, 1);
}


/*---------------------------------------------------------------------------*
  Name:         G3C_Direct2

  Description:  Makes a geometry command with two parameters on a display list.
                Note that the command is packed.
                Also, the pointers of GXDLInfo proceed.

  Arguments:    info       a pointer to GXDLInfo
                op         a geometry command(GXOP_xxxxxxx)
                param0     a parameter
                param1     a parameter

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3C_Direct2(GXDLInfo *info, int op, u32 param0, u32 param1)
{
    G3CS_Direct2(info, op, param0, param1);

    G3C_UpdateGXDLInfo(info, 2);
}


/*---------------------------------------------------------------------------*
  Name:         G3C_Direct3

  Description:  Makes a geometry command with three parameters on a display list.
                Note that the command is packed.
                Also, the pointers of GXDLInfo proceed.

  Arguments:    info       a pointer to GXDLInfo
                op         a geometry command(GXOP_xxxxxxx)
                param0     a parameter
                param1     a parameter
                param2     a parameter

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3C_Direct3(GXDLInfo *info, int op, u32 param0, u32 param1, u32 param2)
{
    G3CS_Direct3(info, op, param0, param1, param2);

    G3C_UpdateGXDLInfo(info, 3);
}


/*---------------------------------------------------------------------------*
  Name:         G3C_DirectN

  Description:  Makes a geometry command with nParams parameters on a display list.
                Note that the command is packed.
                Also, the pointers of GXDLInfo proceed.

  Arguments:    info       a pointer to GXDLInfo
                op         a geometry command(GXOP_xxxxxxx)
                nParams    the number of parameters
                params     parameters

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3C_DirectN(GXDLInfo *info, int op, int nParams, const u32 *params)
{
    G3CS_DirectN(info, op, nParams, params);

    G3C_UpdateGXDLInfo(info, nParams);
}


/*---------------------------------------------------------------------------*
  Name:         G3C_UpdateGXDLInfo

  Description:  Updates a display list pointer of '*info'.

  Arguments:    info       a pointer to GXDLInfo
                n          an index to proceed

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3C_UpdateGXDLInfo(GXDLInfo *info, int n)
{
    info->curr_param += n;

    SDK_ASSERTMSG(((u32)(info->curr_param) - (u32)info->bottom <= info->length),
                  "Buffer overflow ! : Current DL buffer doesn't have enough capacity for new commands\n");

    if (((u32)(++info->curr_cmd) & 0x3) == 0)
    {
        if (info->param0_cmd_flg)
        {
            // code for hardware bug in geometry fifo.
            *(u32 *)(info->curr_param++) = 0;
            info->param0_cmd_flg = FALSE;
        }
        info->curr_cmd = (u8 *)(info->curr_param++);
    }

    SDK_ASSERTMSG(((u32)(info->curr_cmd) - (u32)info->bottom <= info->length),
                  "Buffer overflow ! : Current DL buffer doesn't have enough capacity for new commands\n");
}


/*---------------------------------------------------------------------------*
  Name:         G3C_Nop

  Description:  Makes a geometry command which does nothing on a display list.
                Note that the command is packed.
                Also, the pointers of GXDLInfo proceed.

  Arguments:    info       a pointer to GXDLInfo

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3C_Nop(GXDLInfo *info)
{
    G3CS_Nop(info);
    G3C_UpdateGXDLInfo(info, G3OP_NOP_NPARAMS);
}


/*---------------------------------------------------------------------------*
  Name:         G3C_MtxMode

  Description:  Makes a geometry command, which sets a matrix mode,
                on a display list. Note that the command is packed.
                Also, the pointers of GXDLInfo proceed.

  Arguments:    info       a pointer to GXDLInfo
                mode       a matrix mode

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3C_MtxMode(GXDLInfo *info, GXMtxMode mode)
{
    G3CS_MtxMode(info, mode);
    G3C_UpdateGXDLInfo(info, G3OP_MTX_MODE_NPARAMS);
}


/*---------------------------------------------------------------------------*
  Name:         G3C_PushMtx

  Description:  Makes a geometry command, which stores a current matrix to
                the top of the matrix stack and increments the stack pointer,
                on a display list. Note that the command is packed.
                Also, the pointers of GXDLInfo proceed.

  Arguments:    info       a pointer to GXDLInfo

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3C_PushMtx(GXDLInfo *info)
{
    G3CS_PushMtx(info);
    G3C_UpdateGXDLInfo(info, G3OP_MTX_PUSH_NPARAMS);
}


/*---------------------------------------------------------------------------*
  Name:         G3C_PopMtx

  Description:  Makes a geometry command, which pops the num'th matrix
                from the matrix stack pointer on the stack,
                and adds num to the pointer. Note that the command is packed.
                Also, the pointers of GXDLInfo proceed.

  Arguments:    info       a pointer to GXDLInfo
                num        an offset to the stack pointer

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3C_PopMtx(GXDLInfo *info, int num)
{
    G3CS_PopMtx(info, num);
    G3C_UpdateGXDLInfo(info, G3OP_MTX_POP_NPARAMS);
}


/*---------------------------------------------------------------------------*
  Name:         G3C_StoreMtx

  Description:  Makes a geometry command, which stores a current matrix
                to the num'th matrix from the matrix stack pointer on the stack,
                on a display list. Note that the command is packed.
                Also, the pointers of GXDLInfo proceed.

  Arguments:    info       a pointer to GXDLInfo
                num        an offset to the stack pointer

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3C_StoreMtx(GXDLInfo *info, int num)
{
    G3CS_StoreMtx(info, num);
    G3C_UpdateGXDLInfo(info, G3OP_MTX_STORE_NPARAMS);
}


/*---------------------------------------------------------------------------*
  Name:         G3C_RestoreMtx

  Description:  Makes a geometry command, which gets the num'th matrix from
                the matrix stack pointer on the stack, on a display list.
                Note that the command is packed.
                Also, the pointers of GXDLInfo proceed.

  Arguments:    info       a pointer to GXDLInfo
                num        an offset to the stack pointer

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3C_RestoreMtx(GXDLInfo *info, int num)
{
    G3CS_RestoreMtx(info, num);
    G3C_UpdateGXDLInfo(info, G3OP_MTX_RESTORE_NPARAMS);
}


/*---------------------------------------------------------------------------*
  Name:         G3C_Identity

  Description:  Makes a geometry command, which sets an identity matrix
                to the current matrix, on a display list.
                Note that the command is packed.
                Also, the pointers of GXDLInfo proceed.

  Arguments:    info       a pointer to GXDLInfo

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3C_Identity(GXDLInfo *info)
{
    G3CS_Identity(info);
    G3C_UpdateGXDLInfo(info, G3OP_MTX_IDENTITY_NPARAMS);
}


/*---------------------------------------------------------------------------*
  Name:         G3C_LoadMtx44

  Description:  Make a geometry command which loads a 4x4 matrix
                to the current matrix.
                Also, the pointers of GXDLInfo proceed.

  Arguments:    info       a pointer to GXDLInfo
                m          a pointer to a constant 4x4 matrix

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3C_LoadMtx44(GXDLInfo *info, const MtxFx44 *m)
{
    G3CS_LoadMtx44(info, m);
    G3C_UpdateGXDLInfo(info, G3OP_MTX_LOAD_4x4_NPARAMS);
}


/*---------------------------------------------------------------------------*
  Name:         G3C_LoadMtx43

  Description:  Make a geometry command which loads a 4x3 matrix
                to the current matrix.
                Also, the pointers of GXDLInfo proceed.

  Arguments:    info       a pointer to GXDLInfo
                m          a pointer to a constant 4x3 matrix

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3C_LoadMtx43(GXDLInfo *info, const MtxFx43 *m)
{
    G3CS_LoadMtx43(info, m);
    G3C_UpdateGXDLInfo(info, G3OP_MTX_LOAD_4x3_NPARAMS);
}


/*---------------------------------------------------------------------------*
  Name:         G3C_MultMtx44

  Description:  Makes a geometry command which multiplies the current matrix
                by a 4x4 matrix from the left.
                Also, the pointers of GXDLInfo proceed.

  Arguments:    info       a pointer to GXDLInfo
                m          a pointer to a constant 4x4 matrix

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3C_MultMtx44(GXDLInfo *info, const MtxFx44 *m)
{
    G3CS_MultMtx44(info, m);
    G3C_UpdateGXDLInfo(info, G3OP_MTX_MULT_4x4_NPARAMS);
}


/*---------------------------------------------------------------------------*
  Name:         G3C_MultMtx43

  Description:  Make a geometry command which multiplies the current matrix
                by a 4x3 matrix from the left.
                Also, the pointers of GXDLInfo proceed.

  Arguments:    info       a pointer to GXDLInfo
                m          a pointer to a constant 4x3 matrix

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3C_MultMtx43(GXDLInfo *info, const MtxFx43 *m)
{
    G3CS_MultMtx43(info, m);
    G3C_UpdateGXDLInfo(info, G3OP_MTX_MULT_4x3_NPARAMS);
}


/*---------------------------------------------------------------------------*
  Name:         G3C_MultMtx33

  Description:  Makes a geometry command which multiplies the current matrix
                by a 3x3 matrix from the left.
                Also, the pointers of GXDLInfo proceed.

  Arguments:    info       a pointer to GXDLInfo
                m          a pointer to a constant 3x3 matrix

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3C_MultMtx33(GXDLInfo *info, const MtxFx33 *m)
{
    G3CS_MultMtx33(info, m);
    G3C_UpdateGXDLInfo(info, G3OP_MTX_MULT_3x3_NPARAMS);
}

/*---------------------------------------------------------------------------*
  Name:         G3C_MultTransMtx33

  Description:  Make a geometry command which multiplies and translates matrix together.

  Arguments:    info       a pointer to GXDLInfo
                mtx        a pointer to a constant 3x3 matrix
                trans      a pointer to a constant translation vector

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3C_MultTransMtx33(GXDLInfo *info, const MtxFx33 *mtx, const VecFx32 *trans)
{
    G3CS_MultTransMtx33(info, mtx, trans);
    G3C_UpdateGXDLInfo(info, G3OP_MTX_MULT_4x3_NPARAMS);
}

/*---------------------------------------------------------------------------*
  Name:         G3C_Scale

  Description:  Makes a geometry command, which multiplies the current matrix
                by a scale matrix. Note that the command is packed.
                Also, the pointers of GXDLInfo proceed.

  Arguments:    info       a pointer to GXDLInfo
                x          X coordinate of a scale
                y          Y coordinate of a scale
                z          Z coordinate of a scale

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3C_Scale(GXDLInfo *info, fx32 x, fx32 y, fx32 z)
{
    G3CS_Scale(info, x, y, z);
    G3C_UpdateGXDLInfo(info, G3OP_MTX_SCALE_NPARAMS);
}


/*---------------------------------------------------------------------------*
  Name:         G3C_Translate

  Description:  Makes a geometry command, which multiplies the current matrix
                by a translation matrix. Note that the command is packed.
                Also, the pointers of GXDLInfo proceed.

  Arguments:    info       a pointer to GXDLInfo
                x          X coordinate of a translation vector
                y          Y coordinate of a translation vector
                z          Z coordinate of a translation vector

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3C_Translate(GXDLInfo *info, fx32 x, fx32 y, fx32 z)
{
    G3CS_Translate(info, x, y, z);
    G3C_UpdateGXDLInfo(info, G3OP_MTX_TRANS_NPARAMS);
}


/*---------------------------------------------------------------------------*
  Name:         G3C_Color

  Description:  Makes a geometry command, which sends a vertex color.
                Also, the pointers of GXDLInfo proceed.

  Arguments:    info       a pointer to GXDLInfo
                rgb        a vertex color(R:5, G:5, B:5)

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3C_Color(GXDLInfo *info, GXRgb rgb)
{
    G3CS_Color(info, rgb);
    G3C_UpdateGXDLInfo(info, G3OP_COLOR_NPARAMS);
}


/*---------------------------------------------------------------------------*
  Name:         G3C_Normal

  Description:  Makes a geometry command, which sends a normal vector.
                Also, the pointers of GXDLInfo proceed.

  Arguments:    info       a pointer to GXDLInfo
                x          X coordinate of a normal vector
                y          Y coordinate of a normal vector
                z          Z coordinate of a normal vector

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3C_Normal(GXDLInfo *info, fx16 x, fx16 y, fx16 z)
{
    G3CS_Normal(info, x, y, z);
    G3C_UpdateGXDLInfo(info, G3OP_NORMAL_NPARAMS);
}


/*---------------------------------------------------------------------------*
  Name:         G3C_TexCoord

  Description:  Makes a geometry command, which sends a texture coordinate.
                Also, the pointers of GXDLInfo proceed.

  Arguments:    info       a pointer to GXDLInfo
                s          an S of a texture coordinate
                t          a T of a texture coordinate

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3C_TexCoord(GXDLInfo *info, fx32 s, fx32 t)
{
    G3CS_TexCoord(info, s, t);
    G3C_UpdateGXDLInfo(info, G3OP_TEXCOORD_NPARAMS);
}


/*---------------------------------------------------------------------------*
  Name:         G3C_Vtx

  Description:  Makes a geometry command, which sends a vertex as a fx16 vector.
                Also, the pointers of GXDLInfo proceed.

  Arguments:    info       a pointer to GXDLInfo
                x          X coordinate of a vertex
                y          Y coordinate of a vertex
                z          Z coordinate of a vertex

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3C_Vtx(GXDLInfo *info, fx16 x, fx16 y, fx16 z)
{
    G3CS_Vtx(info, x, y, z);
    G3C_UpdateGXDLInfo(info, G3OP_VTX_16_NPARAMS);
}


/*---------------------------------------------------------------------------*
  Name:         G3C_Vtx10

  Description:  Makes a geometry command, which sends a vertex as a s3.6 vector.
                Also, the pointers of GXDLInfo proceed.

  Arguments:    info       a pointer to GXDLInfo
                x          X coordinate of a vertex
                y          Y coordinate of a vertex
                z          Z coordinate of a vertex

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3C_Vtx10(GXDLInfo *info, fx16 x, fx16 y, fx16 z)
{
    G3CS_Vtx10(info, x, y, z);
    G3C_UpdateGXDLInfo(info, G3OP_VTX_10_NPARAMS);
}


/*---------------------------------------------------------------------------*
  Name:         G3C_VtxXY

  Description:  Makes a geometry command, which sends XY components of a vertex.
                The Z coordinate is the same to the vertex sent just before.
                Also, the pointers of GXDLInfo proceed.

  Arguments:    info       a pointer to GXDLInfo
                x          X coordinate of a vertex
                y          Y coordinate of a vertex

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3C_VtxXY(GXDLInfo *info, fx16 x, fx16 y)
{
    G3CS_VtxXY(info, x, y);
    G3C_UpdateGXDLInfo(info, G3OP_VTX_XY_NPARAMS);
}


/*---------------------------------------------------------------------------*
  Name:         G3C_VtxXZ

  Description:  Makes a geometry command, which sends XZ components of a vertex.
                The Y coordinate is the same to the vertex sent just before.
                Also, the pointers of GXDLInfo proceed.

  Arguments:    info       a pointer to GXDLInfo
                x          X coordinate of a vertex
                z          Z coordinate of a vertex

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3C_VtxXZ(GXDLInfo *info, fx16 x, fx16 z)
{
    G3CS_VtxXZ(info, x, z);
    G3C_UpdateGXDLInfo(info, G3OP_VTX_XZ_NPARAMS);
}


/*---------------------------------------------------------------------------*
  Name:         G3C_VtxYZ

  Description:  Makes a geometry command, which sends YZ components of a vertex.
                The X component is the same to the vertex sent just before.
                Also, the pointers of GXDLInfo proceed.

  Arguments:    info       a pointer to GXDLInfo
                y          Y coordinate of a vertex
                z          Z coordinate of a vertex

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3C_VtxYZ(GXDLInfo *info, fx16 y, fx16 z)
{
    G3CS_VtxYZ(info, y, z);
    G3C_UpdateGXDLInfo(info, G3OP_VTX_YZ_NPARAMS);
}


/*---------------------------------------------------------------------------*
  Name:         G3C_VtxDiff

  Description:  Makes a geometry commnad, which sends a vector as an offset
                to the last vertex sent.
                Also, the pointers of GXDLInfo proceed.

  Arguments:    info       a pointer to GXDLInfo
                x          X coordinate of an offset
                y          Y coordinate of an offset
                z          Z coordinate of an offset

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3C_VtxDiff(GXDLInfo *info, fx16 x, fx16 y, fx16 z)
{
    G3CS_VtxDiff(info, x, y, z);
    G3C_UpdateGXDLInfo(info, G3OP_VTX_DIFF_NPARAMS);
}


/*---------------------------------------------------------------------------*
  Name:         G3C_PolygonAttr

  Description:  Makes a geometry command, which sends attributes for polygons.
                Also, the pointers of GXDLInfo proceed.

  Arguments:    info       a pointer to GXDLInfo
                light      a 4bits field specifying light enable/disable
                polyMode   a polygon mode
                cullMode   a cull mode
                polygonID  a polygon ID
                alpha      an alpha value
                misc       miscellaneous flags
  
  Returns:      none
 *---------------------------------------------------------------------------*/
void G3C_PolygonAttr(GXDLInfo *info, int light, GXPolygonMode polyMode, GXCull cullMode, int polygonID, int alpha, int misc     // GXPolygonAttrMisc
    )
{
    G3CS_PolygonAttr(info, light, polyMode, cullMode, polygonID, alpha, misc);
    G3C_UpdateGXDLInfo(info, G3OP_POLYGON_ATTR_NPARAMS);
}


/*---------------------------------------------------------------------------*
  Name:         G3C_TexImageParam

  Description:  Makes a geometry command, which sends parameters for a texture.
                Also, the pointers of GXDLInfo proceed.

  Arguments:    info       a pointer to GXDLInfo
                texFmt     format of a texture
                texGen     selects the source of a texture coordinate
                s          the size of a texture in the direction of the S-axis
                t          the size of a texture in the direction of the T-axis
                repeat     repeat
                flip       flip
                pltt0      use/not use the color of pltt. 0
                addr       the offset address in the texture image slots
                           (shift 3bits internally)
  
  Returns:      none
 *---------------------------------------------------------------------------*/
void G3C_TexImageParam(GXDLInfo *info,
                       GXTexFmt texFmt,
                       GXTexGen texGen,
                       GXTexSizeS s, GXTexSizeT t, GXTexRepeat repeat, GXTexFlip flip,
                       GXTexPlttColor0 pltt0, u32 addr)
{
    G3CS_TexImageParam(info, texFmt, texGen, s, t, repeat, flip, pltt0, addr);

    G3C_UpdateGXDLInfo(info, G3OP_TEXIMAGE_PARAM_NPARAMS);
}


/*---------------------------------------------------------------------------*
  Name:         G3C_TexPlttBase

  Description:  Makes a geometry command, which sends a base address of
                a texture palette.
                Also, the pointers of GXDLInfo proceed.

  Arguments:    info       a pointer to GXDLInfo
                addr       the offset address in the texture palette slots
                texFmt     format of a texture
  
  Returns:      none
 *---------------------------------------------------------------------------*/
void G3C_TexPlttBase(GXDLInfo *info, u32 addr, GXTexFmt texfmt)
{
    G3CS_TexPlttBase(info, addr, texfmt);

    G3C_UpdateGXDLInfo(info, G3OP_TEXPLTT_BASE_NPARAMS);
}


/*---------------------------------------------------------------------------*
  Name:         G3C_MaterialColorDiffAmb

  Description:  Makes a geometry command, which sends diffuse and ambient.
                Also, the pointers of GXDLInfo proceed.

  Arguments:    info           a pointer to GXDLInfo
                diffuse        a diffuse color
                ambient        an ambient color
                IsSetVtxColor  sets a diffuse color as a vertex color if TRUE
  
  Returns:      none
 *---------------------------------------------------------------------------*/
void G3C_MaterialColorDiffAmb(GXDLInfo *info, GXRgb diffuse, GXRgb ambient, BOOL IsSetVtxColor)
{
    G3CS_MaterialColorDiffAmb(info, diffuse, ambient, IsSetVtxColor);
    G3C_UpdateGXDLInfo(info, G3OP_DIF_AMB_NPARAMS);
}


/*---------------------------------------------------------------------------*
  Name:         G3C_MaterialColorSpecEmi

  Description:  Makes a geometry command, which sends specular and emission.
                Also, the pointers of GXDLInfo proceed.

  Arguments:    info           a pointer to GXDLInfo
                specular       a specular color
                emission       an emission color
                IsShininess    use the shininess table to change a specular
                               color if TRUE

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3C_MaterialColorSpecEmi(GXDLInfo *info, GXRgb specular, GXRgb emission, BOOL IsShininess)
{
    G3CS_MaterialColorSpecEmi(info, specular, emission, IsShininess);
    G3C_UpdateGXDLInfo(info, G3OP_SPE_EMI_NPARAMS);
}


/*---------------------------------------------------------------------------*
  Name:         G3C_LightVector

  Description:  Makes a geometry command, which sends a light vector.
                Also, the pointers of GXDLInfo proceed.

  Arguments:    info       a pointer to GXDLInfo
                lightID    light ID
                x          X coordinate of a light vector
                y          Y coordinate of a light vector
                z          Z coordinate of a light vector

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3C_LightVector(GXDLInfo *info, GXLightId lightID, fx16 x, fx16 y, fx16 z)
{
    G3CS_LightVector(info, lightID, x, y, z);
    G3C_UpdateGXDLInfo(info, G3OP_LIGHT_VECTOR_NPARAMS);
}


/*---------------------------------------------------------------------------*
  Name:         G3C_LightColor

  Description:  Makes a geometry command, which sends a light color.
                Also, the pointers of GXDLInfo proceed.

  Arguments:    info       a pointer to GXDLInfo
                lightID    light ID
                rgb        a light color(R:5, G:5, B:5)

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3C_LightColor(GXDLInfo *info, GXLightId lightID, GXRgb rgb)
{
    G3CS_LightColor(info, lightID, rgb);
    G3C_UpdateGXDLInfo(info, G3OP_LIGHT_COLOR_NPARAMS);
}


/*---------------------------------------------------------------------------*
  Name:         G3C_Shininess

  Description:  Makes a geometry command, which sets up the shininess table.
                Also, the pointers of GXDLInfo proceed.

  Arguments:    info       a pointer to GXDLInfo
                table      a pointer to the shininess data(32 words)

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3C_Shininess(GXDLInfo *info, const u32 *table)
{
    G3CS_Shininess(info, table);
    G3C_UpdateGXDLInfo(info, G3OP_SHININESS_NPARAMS);
}


/*---------------------------------------------------------------------------*
  Name:         G3C_Begin

  Description:  Makes a geometry command, which starts sending primitives.
                Also, the pointers of GXDLInfo proceed.

  Arguments:    info       a pointer to GXDLInfo
                primitive  the type of primitives

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3C_Begin(GXDLInfo *info, GXBegin primitive)
{
    G3CS_Begin(info, primitive);
    G3C_UpdateGXDLInfo(info, G3OP_BEGIN_NPARAMS);
}


/*---------------------------------------------------------------------------*
  Name:         G3C_End

  Description:  Makes a geometry command, which ends sending primitives.
                Also, the pointers of GXDLInfo proceed.

  Arguments:    info       a pointer to GXDLInfo

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3C_End(GXDLInfo *info)
{
    G3CS_End(info);
    G3C_UpdateGXDLInfo(info, G3OP_END_NPARAMS);
}


/*---------------------------------------------------------------------------*
  Name:         G3C_SwapBuffers

  Description:  Makes a geometry command, which swaps the polygon list RAM,
                the vertex RAM, etc.
                Also, the pointers of GXDLInfo proceed.

  Arguments:    info       a pointer to GXDLInfo
                am         auto sort/manual sort
                zw         Z buffer/W buffer

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3C_SwapBuffers(GXDLInfo *info, GXSortMode am, GXBufferMode zw)
{
    G3CS_SwapBuffers(info, am, zw);
    G3C_UpdateGXDLInfo(info, G3OP_SWAP_BUFFERS_NPARAMS);
}


/*---------------------------------------------------------------------------*
  Name:         G3C_ViewPort

  Description:  Makes a geometry command, which specifies viewport.
                Also, the pointers of GXDLInfo proceed.

  Arguments:    info       a pointer to GXDLInfo
                x1         the X coordinate of the lower left
                y1         the Y coordinate of the lower left
                x2         the X coordinate of the upper right
                y2         the Y coordinate of the upper right

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3C_ViewPort(GXDLInfo *info, int x1, int y1, int x2, int y2)
{
    G3CS_ViewPort(info, x1, y1, x2, y2);
    G3C_UpdateGXDLInfo(info, G3OP_VIEWPORT_NPARAMS);
}


/*---------------------------------------------------------------------------*
  Name:         G3C_BoxTest

  Description:  Makes a geometry command, which tests if a box is
                in the frustum or not.
                Also, the pointers of GXDLInfo proceed.

  Arguments:    info       a pointer to GXDLInfo
                box        a pointer to GXBoxTestParam

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3C_BoxTest(GXDLInfo *info, const GXBoxTestParam *box)
{
    G3CS_BoxTest(info, box);
    G3C_UpdateGXDLInfo(info, G3OP_BOX_TEST_NPARAMS);
}


/*---------------------------------------------------------------------------*
  Name:         G3C_PositionTest

  Description:  Makes a geometry command, which applies a position vector
                to the current clip matrix.
                Also, the pointers of GXDLInfo proceed.

  Arguments:    info       a pointer to GXDLInfo
                x          X coordinate of a position
                y          Y coordinate of a position
                z          Z coordinate of a position

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3C_PositionTest(GXDLInfo *info, fx16 x, fx16 y, fx16 z)
{
    G3CS_PositionTest(info, x, y, z);
    G3C_UpdateGXDLInfo(info, G3OP_POS_TEST_NPARAMS);
}


/*---------------------------------------------------------------------------*
  Name:         G3C_VectorTest

  Description:  Makes a geometry command, which applies a vector
                to the current vector matrix.
                Also, the pointers of GXDLInfo proceed.

  Arguments:    info       a pointer to GXDLInfo
                x          X coordinate of a vector
                y          Y coordinate of a vector
                z          Z coordinate of a vector

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3C_VectorTest(GXDLInfo *info, fx16 x, fx16 y, fx16 z)
{
    G3CS_VectorTest(info, x, y, z);
    G3C_UpdateGXDLInfo(info, G3OP_VEC_TEST_NPARAMS);
}
