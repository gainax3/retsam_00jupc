/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - g3d - src - binres
  File:     res_print.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.26 $
 *---------------------------------------------------------------------------*/

//
// AUTHOR: Kenji Nishida
//

#include "./res_print_internal.h"

#include <nnsys/g3d/binres/res_struct.h>
#include <nnsys/g3d/binres/res_print.h>
#include <nnsys/g3d/binres/res_struct_accessor.h>
#include <nitro/fx/fx.h>

#include <math.h>

//
// このファイルの内容はデバックビルドであるか、コンバータにおいてのみ
// コンパイルされる
//
#if defined(SDK_DEBUG) || defined(NNS_FROM_TOOL)


/*---------------------------------------------------------------------------*
    NNS_G3dPrintResName

 *---------------------------------------------------------------------------*/
void
NNS_G3dPrintResName(const NNSG3dResName* resName)
{
    u32 i;
    NNS_G3D_NULL_ASSERT(resName);

    for (i = 0; i < NNS_G3D_RESNAME_SIZE; ++i)
    {
        if (resName->name[i] == '\0')
            return;
        RES_PRINTF("%c", resName->name[i]);
    }
}


/*---------------------------------------------------------------------------*
    NNS_G3dPrintFileHeader

    ファイルサイズ、シグナチャ、バージョンを表示します。    
 *---------------------------------------------------------------------------*/
void
NNS_G3dPrintFileHeader(const NNSG3dResFileHeader* header)
{
    NNS_G3D_NULL_ASSERT(header);

    tabPrint_(); RES_PRINTF("[FileHeader]\n");
    tabPlus_();
    {
        tabPrint_(); RES_PRINTF("file size, %d bytes\n",
                                header->fileSize);
        tabPrint_(); RES_PRINTF("Signature, %c%c%c%c\n",
                                header->signature[0],
                                header->signature[1],
                                header->signature[2],
                                header->signature[3]);
        tabPrint_(); RES_PRINTF("Version,   %d.%d\n",
                                header->version >> 8,
                                header->version & 0xff);
    }
    tabMinus_();
}


/*---------------------------------------------------------------------------*
    NNS_G3dPrintDataBlockHeader

    [データブロック種別], データブロックのサイズ
    を出力します
 *---------------------------------------------------------------------------*/
void
NNS_G3dPrintDataBlockHeader(const NNSG3dResDataBlockHeader* header)
{
    const char* kind;
    NNS_G3D_NULL_ASSERT(header);
    
    kind = (const char*)&header->kind;
    tabPrint_(); RES_PRINTF("[%c%c%c%c]\n",
                            kind[0], kind[1], kind[2], kind[3]);
    tabPlus_(); tabPrint_();
    RES_PRINTF("block size, %d bytes\n", header->size);
    tabMinus_();
}


/*---------------------------------------------------------------------------*
    NNS_G3dPrintDictEntryItem_TEX

    個々のテクスチャ情報を出力します。出力は、

    テクスチャのインデックス, テクスチャ名, オフセット, Width, Height,
    フォーマット名, Pltt0の扱い, Width(オリジナルデータ), Height(オリジナルデータ)

    のように並びます
 *---------------------------------------------------------------------------*/
void
NNS_G3dPrintDictEntryItem_TEX(u32 idx,
                              const NNSG3dResName* name,
                              const NNSG3dResDictTexData* data)
{
    u32 texAddr;
    int width, height;
    u32 fmt;
    u32 pltt0;

    NNS_G3D_NULL_ASSERT(name);
    NNS_G3D_NULL_ASSERT(data);
    
    tabPrint_();
    RES_PRINTF(" %d, ", idx);
    NNS_G3dPrintResName(name);
    RES_PRINTF(", ");

    texAddr = (data->texImageParam & 0xffff) << 3;
    // GX_TEXSIZE_Sxの値を利用
    width = 8 * (int)(pow(2.0, (double)((data->texImageParam >> 20) & 7)));
    // GX_TEXSIZE_Txの値を利用
    height = 8 * (int)(pow(2.0, (double)((data->texImageParam >> 23) & 7)));

    RES_PRINTF("%d, %d, %d, ", texAddr, width, height);

    fmt = (data->texImageParam >> 26) & 7;
    pltt0 = (data->texImageParam >> 29) & 1;

    switch(fmt)
    {
    case 1:
        RES_PRINTF("a3i5");
        break;
    case 2:
        RES_PRINTF("palette4");
        break;
    case 3:
        RES_PRINTF("palette16");
        break;
    case 4:
        RES_PRINTF("palette256");
        break;
    case 5:
        RES_PRINTF("tex4x4");
        break;
    case 6:
        RES_PRINTF("a5i3");
        break;
    case 7:
        RES_PRINTF("direct");
        break;
    }

    RES_PRINTF(", ");
    if (pltt0)
    {
        RES_PRINTF("transparency");
    }
    else
    {
        RES_PRINTF("color");
    }

    RES_PRINTF(", %d, %d\n",
               data->extraParam & 0x3ff,
               (data->extraParam >> 11) & 0x3ff);
}


/*---------------------------------------------------------------------------*
    NNS_G3dPrintDictEntryItem_PLTT

    個々のテクスチャパレット情報の出力

    インデックス, パレット名, ブロック内オフセット
 *---------------------------------------------------------------------------*/
void
NNS_G3dPrintDictEntryItem_PLTT(u32 idx,
                               const NNSG3dResName* name,
                               const NNSG3dResDictPlttData* data)
{
    NNS_G3D_NULL_ASSERT(name);
    NNS_G3D_NULL_ASSERT(data);

    tabPrint_(); 
    RES_PRINTF(" %d, ", idx);
    NNS_G3dPrintResName(name);
    RES_PRINTF(", %d\n", data->offset << 3);
}


/*---------------------------------------------------------------------------*
    NNS_G3dPrintTexImage

    テクスチャイメージブロックの情報を出力します。
    パレットも含みます。
 *---------------------------------------------------------------------------*/
void
NNS_G3dPrintTex(const NNSG3dResTex* texImage)
{
    const NNSG3dResDict* dict;
    u32 numEntry;
    u32 numEntryNm;
    u32 numEntry4x4;
    u32 idx;

    NNS_G3D_NULL_ASSERT(texImage);

    dict = &texImage->dict;
    numEntry = dict->numEntry;
    numEntryNm = 0;
    numEntry4x4 = 0;

    tabPlus_();
    tabPrint_(); RES_PRINTF("# of textures, %d\n", numEntry);

    //
    // 4x4とそれ以外を別々に分けて表示
    //
    for (idx = 0; idx < numEntry; ++idx)
    {
        const NNSG3dResDictTexData* data =
            (const NNSG3dResDictTexData*) NNS_G3dGetResDataByIdx(dict, idx);

        if (((data->texImageParam >> 26) & 7) != 5)
        {
            numEntryNm++;
        }
        else
        {
            numEntry4x4++;
        }
    }

    //
    // 4x4以外のフォーマット
    //
    if (0 != numEntryNm)
    {
        {
            tabPrint_(); 
            RES_PRINTF("Image size of textures(not 4x4), %d\n", (texImage->texInfo.sizeTex << 3));
            tabPlus_();
            {
                u32 i;
                for (i = 0; i < numEntry; ++i)
                {
                    const NNSG3dResName* name = NNS_G3dGetResNameByIdx(dict, i);
                    const NNSG3dResDictTexData* data =
                        (const NNSG3dResDictTexData*)NNS_G3dGetResDataByIdx(dict, i);

                    if (((data->texImageParam >> 26) & 7) != 5)
                    {
                        // 4x4COMPでない
                        NNS_G3dPrintDictEntryItem_TEX(i, name, data);
                    }
                }
            }
            tabMinus_();
        }
    }

    //
    // 4x4フォーマット
    //
    if (0 != numEntry4x4)
    {
        {
            tabPrint_();
            RES_PRINTF("Image size of textures(4x4), %d\n", (texImage->tex4x4Info.sizeTex << 3));
            tabPlus_();
            {
                u32 i;
                for (i = 0; i < numEntry; ++i)
                {
                    const NNSG3dResName* name = NNS_G3dGetResNameByIdx(dict, i);
                    const NNSG3dResDictTexData* data =
                        (const NNSG3dResDictTexData*) NNS_G3dGetResDataByIdx(dict, i);
                    
                    if (((data->texImageParam >> 26) & 7) == 5)
                    {
                        // 4x4COMP
                        NNS_G3dPrintDictEntryItem_TEX(i, name, data);
                    }
                }
            }
            tabMinus_();
        }
    }

    //
    // Texture palettes
    //
    {
        const NNSG3dResDict* dict = (const NNSG3dResDict*)((u8*)texImage + texImage->plttInfo.ofsDict);
        u32 numEntry = dict->numEntry;

        tabPrint_(); RES_PRINTF("# of palettes, %d\n", numEntry);
        tabPrint_(); RES_PRINTF("Image size of palettes,  %d\n", texImage->plttInfo.sizePltt << 3);

        tabPlus_();
        {
            u32 i;
            for (i = 0; i < numEntry; ++i)
            {
                const NNSG3dResName* name = NNS_G3dGetResNameByIdx(dict, i);
                const NNSG3dResDictPlttData* data =
                    (const NNSG3dResDictPlttData*)NNS_G3dGetResDataByIdx(dict, i);

                NNS_G3dPrintDictEntryItem_PLTT(i, name, data);
            }
        }
        tabMinus_();
    }

    tabMinus_();
}


/*---------------------------------------------------------------------------*
    NNS_G3dPrintMdlInfo

    NNSG3dResMdlInfo構造体のサマリを表示する
 *---------------------------------------------------------------------------*/
void
NNS_G3dPrintMdlInfo(const NNSG3dResMdlInfo* info)
{
    NNS_G3D_NULL_ASSERT(info);

    tabPlus_();
    {
        {
            tabPrint_(); RES_PRINTF("Sbc type, ");
            switch(info->sbcType)
            {
            case NNS_G3D_SBCTYPE_NORMAL:
                RES_PRINTF("normal\n");
                break;
            default:
                RES_PRINTF("unknown\n");
                break;
            }
        }

        {
            tabPrint_(); RES_PRINTF("Scaling Rule, ");
            switch(info->scalingRule)
            {
            case NNS_G3D_SCALINGRULE_STANDARD:
                RES_PRINTF("standard\n");
                break;
            case NNS_G3D_SCALINGRULE_MAYA:
                RES_PRINTF("maya\n");
                break;
            case NNS_G3D_SCALINGRULE_SI3D:
                RES_PRINTF("si3d\n");
                break;
            default:
                RES_PRINTF("unknown\n");
                break;
            }
        }

        {
            tabPrint_(); RES_PRINTF("Texture Matrix Mode, ");
            switch(info->texMtxMode)
            {
            case NNS_G3D_TEXMTXMODE_MAYA:
                RES_PRINTF("maya\n");
                break;
            case NNS_G3D_TEXMTXMODE_SI3D:
                RES_PRINTF("si3d\n");
                break;
            default:
                RES_PRINTF("unknown\n");
                break;
            }
        }

        {
            tabPrint_(); RES_PRINTF("PosScale, ");
            printFx32_(info->posScale); RES_PRINTF("\n");
        }

        {
            tabPrint_();
            RES_PRINTF("FirstUnusedMtxStackID, %d\n", info->firstUnusedMtxStackID);
        }

        {
            tabPrint_(); RES_PRINTF("# of vectices, ");
            RES_PRINTF("%d\n", info->numVertex);

            tabPrint_(); RES_PRINTF("# of polygons, ");
            RES_PRINTF("%d\n", info->numPolygon);

            tabPrint_(); RES_PRINTF("# of triangles, ");
            RES_PRINTF("%d\n", info->numTriangle);

            tabPrint_(); RES_PRINTF("# of quads, ");
            RES_PRINTF("%d\n", info->numQuad);
        }

        {
            tabPrint_(); RES_PRINTF("Box(xyzwhd), ");
            printFx32_(info->boxX); RES_PRINTF(", ");
            printFx32_(info->boxY); RES_PRINTF(", ");
            printFx32_(info->boxZ); RES_PRINTF(", ");
            printFx32_(info->boxW); RES_PRINTF(", ");
            printFx32_(info->boxH); RES_PRINTF(", ");
            printFx32_(info->boxD); RES_PRINTF("\n");
        }
    }
    tabMinus_();
}


//
// +1/-1要素の存在によって0になる行列要素の場所を格納している
//
static u8 pivotUtil_[9][4] =
{
    {4, 5, 7, 8},
    {3, 5, 6, 8},
    {3, 4, 6, 7},
    
    {1, 2, 7, 8},
    {0, 2, 6, 8},
    {0, 1, 6, 7},

    {1, 2, 4, 5},
    {0, 2, 3, 5},
    {0, 1, 3, 4}
};


/*---------------------------------------------------------------------------*
    NNS_G3dPrintNodeSRTInfo

    個々のノードのSRT情報のサマリを出力
 *---------------------------------------------------------------------------*/
void
NNS_G3dPrintNodeSRTInfo(const NNSG3dResNodeInfo* info,
                        u32 idx)
{
    const NNSG3dResNodeData* srt;
    const NNSG3dResName* resName;
    NNS_G3D_NULL_ASSERT(info);

    srt = NNS_G3dGetNodeDataByIdx(info, idx);
    resName = NNS_G3dGetNodeNameByIdx(info, idx);

    tabPrint_(); 
    RES_PRINTF(" %d, ", idx);
    NNS_G3dPrintResName(resName);

    if ((srt->flag & NNS_G3D_SRTFLAG_IDENTITY) == NNS_G3D_SRTFLAG_IDENTITY)
    {
        RES_PRINTF(", Identity");
        {
            u32 mtxStackID =
                ((u32)srt->flag & NNS_G3D_SRTFLAG_IDXMTXSTACK_MASK) >> NNS_G3D_SRTFLAG_IDXMTXSTACK_SHIFT;
            if (mtxStackID != 31)
            {
                RES_PRINTF(", stackID = %d", mtxStackID);
            }
            else
            {
                RES_PRINTF(", stackID = N/A");
            }
        }
        RES_PRINTF("\n");
        return;
    }

    if (srt->flag & NNS_G3D_SRTFLAG_TRANS_ZERO)
    {
        RES_PRINTF(", TransIsZero");
    }

    if (srt->flag & NNS_G3D_SRTFLAG_ROT_ZERO)
    {
        RES_PRINTF(", RotIsZero");
    }
    else if (srt->flag & NNS_G3D_SRTFLAG_PIVOT_EXIST)
    {
        RES_PRINTF(", PivotExist");
    }

    if (srt->flag & NNS_G3D_SRTFLAG_SCALE_ONE)
    {
        RES_PRINTF(", ScaleIsOne");
    }
    
    {
        u32 mtxStackID =
            ((u32)srt->flag & NNS_G3D_SRTFLAG_IDXMTXSTACK_MASK) >> NNS_G3D_SRTFLAG_IDXMTXSTACK_SHIFT;
        if (mtxStackID != 31)
        {
            RES_PRINTF(", stackID = %d", mtxStackID);
        }
        else
        {
            RES_PRINTF(", stackID = N/A");
        }
    }
    RES_PRINTF("\n");

    tabPlus_();
    {
        u8* ptr = (u8*)srt + sizeof(NNSG3dResNodeData);
        if (!(srt->flag & NNS_G3D_SRTFLAG_TRANS_ZERO))
        {
            f32 Tx, Ty, Tz;
            Tx = fx32Tof32_(*((fx32*)ptr));
            Ty = fx32Tof32_(*((fx32*)ptr + 1));
            Tz = fx32Tof32_(*((fx32*)ptr + 2));
            tabPrint_(); RES_PRINTF("Trans, %f, %f, %f\n", Tx, Ty, Tz);
            ptr += 3 * sizeof(fx32);
        }

        if (!(srt->flag & NNS_G3D_SRTFLAG_ROT_ZERO))
        {
            f32 mtx[9];

            if (srt->flag & NNS_G3D_SRTFLAG_PIVOT_EXIST)
            {
                //
                // 圧縮されているデータを解釈して出力
                //
                fx16* p = (fx16*)ptr;
                u32 idxPivot = ((u32)srt->flag & NNS_G3D_SRTFLAG_IDXPIVOT_MASK) >> NNS_G3D_SRTFLAG_IDXPIVOT_SHIFT;
                f32 A = fx32Tof32_(*(p + 0));
                f32 B = fx32Tof32_(*(p + 1));

                mtx[0] = mtx[1] = mtx[2] =
                mtx[3] = mtx[4] = mtx[5] =
                mtx[6] = mtx[7] = mtx[8] = 0;

                mtx[idxPivot] = (srt->flag & NNS_G3D_SRTFLAG_PIVOT_MINUS) ? -1.f : 1.f;
                mtx[pivotUtil_[idxPivot][0]] = A;
                mtx[pivotUtil_[idxPivot][1]] = B;
                if (srt->flag & NNS_G3D_SRTFLAG_SIGN_REVC)
                {
                    mtx[pivotUtil_[idxPivot][2]] = -B;
                }
                else
                {
                    mtx[pivotUtil_[idxPivot][2]] = B;
                }
                
                if (srt->flag & NNS_G3D_SRTFLAG_SIGN_REVD)
                {
                    mtx[pivotUtil_[idxPivot][3]] = -A;
                }
                else
                {
                    mtx[pivotUtil_[idxPivot][3]] = A;
                }
                ptr += 2 * sizeof(fx16);
            }
            else
            {
                fx16* p = (fx16*)ptr;
                mtx[0] = fx32Tof32_(srt->_00);
                mtx[1] = fx32Tof32_(*(p + 0));
                mtx[2] = fx32Tof32_(*(p + 1));

                mtx[3] = fx32Tof32_(*(p + 2));
                mtx[4] = fx32Tof32_(*(p + 3));
                mtx[5] = fx32Tof32_(*(p + 4));

                mtx[6] = fx32Tof32_(*(p + 5));
                mtx[7] = fx32Tof32_(*(p + 6));
                mtx[8] = fx32Tof32_(*(p + 7));

                ptr += 8 * sizeof(fx16);
            }
            tabPrint_(); RES_PRINTF("Rot  , %f, %f, %f\n", mtx[0], mtx[1], mtx[2]);
            tabPrint_(); RES_PRINTF("     , %f, %f, %f\n", mtx[3], mtx[4], mtx[5]);
            tabPrint_(); RES_PRINTF("     , %f, %f, %f\n", mtx[6], mtx[7], mtx[8]);
        }

        if (!(srt->flag & NNS_G3D_SRTFLAG_SCALE_ONE))
        {
            f32 Sx, Sy, Sz;
            
            Sx = fx32Tof32_(*((fx32*)ptr));
            Sy = fx32Tof32_(*((fx32*)ptr + 1));
            Sz = fx32Tof32_(*((fx32*)ptr + 2));

            tabPrint_(); RES_PRINTF("Scale, %f %f %f\n", Sx, Sy, Sz);
        }
    }
    tabMinus_();
}


/*---------------------------------------------------------------------------*
    NNS_G3dPrintNodeInfo

    単一モデル内のノード情報を出力
 *---------------------------------------------------------------------------*/
void
NNS_G3dPrintNodeInfo(const NNSG3dResNodeInfo* info)
{
    NNS_G3D_NULL_ASSERT(info);

    tabPrint_(); RES_PRINTF("# of nodes, %d\n", info->dict.numEntry);
    
    tabPlus_();
    {
        u32 i;
        const NNSG3dResDict* dict = &info->dict;
        u32 numEntry = dict->numEntry;
        
        for (i = 0; i < numEntry; ++i)
        {
            NNS_G3dPrintNodeSRTInfo(info, i);
        }
    }
    tabMinus_();
}


//
// NNS_G3dPrintSbc用サブ関数群
//
static const NNSG3dResName*
getNodeName_(u32 idx, const NNSG3dResMdl* mdl)
{
    NNS_G3D_NULL_ASSERT(mdl);

    return NNS_G3dGetResNameByIdx(&mdl->nodeInfo.dict, idx);
}


static const NNSG3dResName*
getMatName_(u32 idx, const NNSG3dResMdl* mdl)
{
    const NNSG3dResMat* mat;
    NNS_G3D_NULL_ASSERT(mdl);

    mat = (const NNSG3dResMat*)((u8*)mdl + mdl->ofsMat);

    return NNS_G3dGetResNameByIdx(&mat->dict, idx);
}


static const NNSG3dResName*
getShpName_(u32 idx, const NNSG3dResMdl* mdl)
{
    const NNSG3dResShp* shp;
    NNS_G3D_NULL_ASSERT(mdl);

    shp = (const NNSG3dResShp*)((u8*)mdl + mdl->ofsShp);

    return NNS_G3dGetResNameByIdx(&shp->dict, idx);
}


static void
printOpNode_(u32 idx, const NNSG3dResMdl* mdl)
{
    NNS_G3D_NULL_ASSERT(mdl);

    RES_PRINTF(", %d(", idx);
    NNS_G3dPrintResName(getNodeName_(idx, mdl));
    RES_PRINTF(")");
}


static void
printOpMat_(u32 idx, const NNSG3dResMdl* mdl)
{
    NNS_G3D_NULL_ASSERT(mdl);

    RES_PRINTF(", %d(", idx);
    NNS_G3dPrintResName(getMatName_(idx, mdl));
    RES_PRINTF(")");
}


static void
printOpShp_(u32 idx, const NNSG3dResMdl* mdl)
{
    NNS_G3D_NULL_ASSERT(mdl);
    
    RES_PRINTF(", %d(", idx);
    NNS_G3dPrintResName(getShpName_(idx, mdl));
    RES_PRINTF(")");
}


static void
printOpNodeDescFlag(u32 flag)
{
    RES_PRINTF(", 0x%02x", flag);
}


/*---------------------------------------------------------------------------*
    NNS_G3dPrintSbc

    Sbcのコードを逆アセンブル(?)して表示する
 *---------------------------------------------------------------------------*/
void
NNS_G3dPrintSbc(const u8* pCode, u32 sizeByte, const NNSG3dResMdl* mdl)
{
    u32 idx;
    NNS_G3D_NULL_ASSERT(pCode);
    NNS_G3D_NULL_ASSERT(mdl);

    idx = 0;
    tabPrint_(); RES_PRINTF("size of SBC(bytes), %d\n", sizeByte);

    tabPlus_();
    while(idx < sizeByte)
    {
        const u8* p = pCode + idx;
        u32 cmd = *p;
        u32 kind = cmd & NNS_G3D_SBCCMD_MASK;
        u32 opt  = cmd & NNS_G3D_SBCFLG_MASK;

        tabPrint_();
        switch(kind)
        {
        case NNS_G3D_SBC_NOP:
            RES_PRINTF("Nop");
            ++idx;
            break;
        case NNS_G3D_SBC_RET:
            RES_PRINTF("Ret");
            ++idx;
            break;
        case NNS_G3D_SBC_NODE:
            RES_PRINTF("Node         ");
            printOpNode_(*(p + 1), mdl);
            if (*(p + 2))
            {
                RES_PRINTF(", visible");
            }
            else
            {
                RES_PRINTF(", invisible");
            }
            idx += 3;
            break;
        case NNS_G3D_SBC_MTX:
            RES_PRINTF("Mtx          , %d", *(p + 1));
            idx += 2;
            break;
        case NNS_G3D_SBC_MAT:
            switch(opt)
            {
            case NNS_G3D_SBCFLG_000:
                RES_PRINTF("Mat[000]     ");
                break;
            case NNS_G3D_SBCFLG_001:
                RES_PRINTF("Mat[001]     ");
                break;
            case NNS_G3D_SBCFLG_010:
                RES_PRINTF("Mat[010]     ");
                break;
            default:
                goto UNKNOWN_OP;
                break;
            }
            printOpMat_(*(p + 1), mdl);
            idx += 2;
            break;
        case NNS_G3D_SBC_SHP:
            RES_PRINTF("Shp          ");
            printOpShp_(*(p + 1), mdl);
            idx += 2;
            break;
        case NNS_G3D_SBC_NODEDESC:
            switch(opt)
            {
            case NNS_G3D_SBCFLG_000:
                RES_PRINTF("NodeDesc[000]");
                printOpNode_(*(p + 1), mdl);
                printOpNode_(*(p + 2), mdl);
                printOpNodeDescFlag(*(p + 3));
                idx += 4;
                break;
            case NNS_G3D_SBCFLG_001:
                RES_PRINTF("NodeDesc[001]");
                printOpNode_(*(p + 1), mdl);
                printOpNode_(*(p + 2), mdl);
                printOpNodeDescFlag(*(p + 3));
                RES_PRINTF(", StoreStackID=%d", *(p + 4));
                idx += 5;
                break;
            case NNS_G3D_SBCFLG_010:
                RES_PRINTF("NodeDesc[010]");
                printOpNode_(*(p + 1), mdl);
                printOpNode_(*(p + 2), mdl);
                printOpNodeDescFlag(*(p + 3));
                RES_PRINTF(", RestoreStackID=%d", *(p + 4));
                idx += 5;
                break;
            case NNS_G3D_SBCFLG_011:
                RES_PRINTF("NodeDesc[011]");
                printOpNode_(*(p + 1), mdl);
                printOpNode_(*(p + 2), mdl);
                printOpNodeDescFlag(*(p + 3));
                RES_PRINTF(", StoreStackID=%d", *(p + 4));
                RES_PRINTF(", RestoreStackID=%d", *(p + 5));
                idx += 6;
                break;
            default:
                goto UNKNOWN_OP;
                break;
            }
            break;
        case NNS_G3D_SBC_BB:
            switch(opt)
            {
            case NNS_G3D_SBCFLG_000:
                RES_PRINTF("NodeDesc_BB[000]");
                printOpNode_(*(p + 1), mdl);
                idx += 2;
                break;
            case NNS_G3D_SBCFLG_001:
                RES_PRINTF("NodeDesc_BB[001]");
                printOpNode_(*(p + 1), mdl);
                RES_PRINTF(", StoreStackID=%d", *(p + 2));
                idx += 3;
                break;
            case NNS_G3D_SBCFLG_010:
                RES_PRINTF("NodeDesc_BB[010]");
                printOpNode_(*(p + 1), mdl);
                RES_PRINTF(", RestoreStackID=%d", *(p + 2));
                idx += 3;
                break;
            case NNS_G3D_SBCFLG_011:
                RES_PRINTF("NodeDesc_BB[011]");
                printOpNode_(*(p + 1), mdl);
                RES_PRINTF(", RestoreStackID=%d", *(p + 2));
                RES_PRINTF(", StoreStackID=%d", *(p + 3));
                idx += 4;
                break;
            default:
                goto UNKNOWN_OP;
            }
            break;
        case NNS_G3D_SBC_BBY:
            switch(opt)
            {
            case NNS_G3D_SBCFLG_000:
                RES_PRINTF("NodeDesc_BBY[000]");
                printOpNode_(*(p + 1), mdl);
                idx += 2;
                break;
            case NNS_G3D_SBCFLG_001:
                RES_PRINTF("NodeDesc_BBY[001]");
                printOpNode_(*(p + 1), mdl);
                RES_PRINTF(", StoreStackID=%d", *(p + 2));
                idx += 3;
                break;
            case NNS_G3D_SBCFLG_010:
                RES_PRINTF("NodeDesc_BBY[010]");
                printOpNode_(*(p + 1), mdl);
                RES_PRINTF(", RestoreStackID=%d", *(p + 2));
                idx += 3;
                break;
            case NNS_G3D_SBCFLG_011:
                RES_PRINTF("NodeDesc_BBY[011]");
                printOpNode_(*(p + 1), mdl);
                RES_PRINTF(", RestoreStackID=%d", *(p + 2));
                RES_PRINTF(", StoreStackID=%d", *(p + 3));
                idx += 4;
                break;
            default:
                goto UNKNOWN_OP;
            }
            break;
        case NNS_G3D_SBC_NODEMIX:
            {
                int i;
                const u8* pp = p + 3;
                if (opt == NNS_G3D_SBCFLG_000)
                    RES_PRINTF("NodeMix[000] ");
                else
                    RES_PRINTF("NodeMix[001] ");

                RES_PRINTF("StoreStackID=%d, numBlend=%d, [ ", *(p + 1), *(p + 2));
                idx += 3;
                for (i = 0; i < *(p + 2); ++i)
                {
                    RES_PRINTF("<Sid=%d, Mid=%d, %f> ", *(pp), *(pp + 1), *(pp + 2) / 256.f);
                    pp += 3;
                    idx += 3;
                }
                RES_PRINTF("]");
            }
            break;
        case NNS_G3D_SBC_CALLDL:
            goto UNKNOWN_OP;
            break;
        case NNS_G3D_SBC_POSSCALE:
            if (NNS_G3D_SBCFLG_000 == opt)
            {
                RES_PRINTF("PosScale[000]");
            }
            else if (NNS_G3D_SBCFLG_001 == opt)
            {
                RES_PRINTF("PosScale[001]");
            }
            else
            {
                goto UNKNOWN_OP;
            }
            idx++;
            break;

            goto UNKNOWN_OP;
            break;
        case NNS_G3D_SBC_ENVMAP:
            RES_PRINTF("EnvMap      ");
            printOpMat_(*(p + 1), mdl);
            RES_PRINTF(", flag = 0x%02x", *(p + 2));
            idx += 3;
            break;
        case NNS_G3D_SBC_PRJMAP:
            RES_PRINTF("PrjMap      ");
            printOpMat_(*(p + 1), mdl);
            RES_PRINTF(", flag = 0x%02x", *(p + 2));
            idx += 3;
            break;
        default:
            goto UNKNOWN_OP;
            break;
        }
        RES_PRINTF("\n");
    }
    tabMinus_();
    return;
UNKNOWN_OP:
    RES_PRINTF("unknown op found\n");
    tabMinus_();
    return;
}


void
NNS_G3dPrintDL(const u32* dl, u32 szByte)
{
#define NNS_G3D_DLPARAM_CASE(opt)  \
    case (opt):                    \
        RES_PRINTF(#opt"\n");      \
        break
#define NNS_G3D_DLPARAM_DEFAULT    \
    default:                       \
        RES_PRINTF("Unknown\n");   \
        break
#define NNS_G3D_DLPARAM_FX(val)  \
    RES_PRINTF("%6f", (fx32)(val) / 4096.f)

#define NNS_G3D_DLPARAM_FX16PAIR(v)         \
    NNS_G3D_DLPARAM_FX((fx16)((v) & 0xffff)); \
    RES_PRINTF(", ");                       \
    NNS_G3D_DLPARAM_FX((fx16)((v) >> 16));    \
    RES_PRINTF("\n")

#define NNS_G3D_DLPARAM_VEC(x, y, z) \
    NNS_G3D_DLPARAM_FX(x);           \
    RES_PRINTF(", ");                \
    NNS_G3D_DLPARAM_FX(y);           \
    RES_PRINTF(", ");                \
    NNS_G3D_DLPARAM_FX(z)

#define NNS_G3D_DLPARAM_COLOR(rgb)   \
    RES_PRINTF("(%d, %d, %d) ",      \
               (rgb) & 0x1f,         \
               ((rgb) >> 5) & 0x1f,  \
               ((rgb) >> 10) & 0x1f)

#define NNS_G3D_DLPARAM_44(p)                                     \
    NNS_G3D_DLPARAM_FX(*(p +  0)); RES_PRINTF(", ");              \
    NNS_G3D_DLPARAM_FX(*(p +  1)); RES_PRINTF(", ");              \
    NNS_G3D_DLPARAM_FX(*(p +  2)); RES_PRINTF(", ");              \
    NNS_G3D_DLPARAM_FX(*(p +  3)); RES_PRINTF("\n             "); \
    NNS_G3D_DLPARAM_FX(*(p +  4)); RES_PRINTF(", ");              \
    NNS_G3D_DLPARAM_FX(*(p +  5)); RES_PRINTF(", ");              \
    NNS_G3D_DLPARAM_FX(*(p +  6)); RES_PRINTF(", ");              \
    NNS_G3D_DLPARAM_FX(*(p +  7)); RES_PRINTF("\n             "); \
    NNS_G3D_DLPARAM_FX(*(p +  8)); RES_PRINTF(", ");              \
    NNS_G3D_DLPARAM_FX(*(p +  9)); RES_PRINTF(", ");              \
    NNS_G3D_DLPARAM_FX(*(p + 10)); RES_PRINTF(", ");              \
    NNS_G3D_DLPARAM_FX(*(p + 11)); RES_PRINTF("\n             "); \
    NNS_G3D_DLPARAM_FX(*(p + 12)); RES_PRINTF(", ");              \
    NNS_G3D_DLPARAM_FX(*(p + 13)); RES_PRINTF(", ");              \
    NNS_G3D_DLPARAM_FX(*(p + 14)); RES_PRINTF(", ");              \
    NNS_G3D_DLPARAM_FX(*(p + 15)); RES_PRINTF("\n");

    const u32* cur = dl;
    NNS_G3D_ASSERT((szByte & 3) == 0);

    while(cur < dl + (szByte >> 2))
    {
        const u32  op    = *cur;
        const u32* param = cur + 1;
        int i;

        RES_PRINTF("%04x:\n", (u32)(cur - dl) << 2);

        for (i = 0; i < 4; ++i)
        {
            u32 cmd = (op >> (i * 8)) & 0xff;

            switch(cmd)
            {
            case G3OP_NOP:
                break;
            case G3OP_MTX_MODE:
                RES_PRINTF("MatrixMode ");
                switch(*param)
                {
                NNS_G3D_DLPARAM_CASE(GX_MTXMODE_PROJECTION);
                NNS_G3D_DLPARAM_CASE(GX_MTXMODE_POSITION);
                NNS_G3D_DLPARAM_CASE(GX_MTXMODE_POSITION_VECTOR);
                NNS_G3D_DLPARAM_CASE(GX_MTXMODE_TEXTURE);
                NNS_G3D_DLPARAM_DEFAULT;
                }
                param += G3OP_MTX_MODE_NPARAMS;
                break;
            case G3OP_MTX_PUSH:
                RES_PRINTF("PushMatrix\n");
                //param += G3OP_MTX_PUSH_NPARAMS;
                break;
            case G3OP_MTX_POP:
                RES_PRINTF("PopMatrix %d\n", ((s32) *param << 26) >> 26);
                param += G3OP_MTX_POP_NPARAMS;
                break;
            case G3OP_MTX_STORE:
                RES_PRINTF("StoreMatrix %d\n", *param & 0x1f);
                param += G3OP_MTX_STORE_NPARAMS;
                break;
            case G3OP_MTX_RESTORE:
                RES_PRINTF("RestoreMatrix %d\n", *param & 0x1f);
                param += G3OP_MTX_RESTORE_NPARAMS;
                break;
            case G3OP_MTX_IDENTITY:
                RES_PRINTF("Identity\n");
                //param += G3OP_MTX_IDENTITY_NPARAMS;
                break;
            case G3OP_MTX_LOAD_4x4:
                RES_PRINTF("LoadMatrix44 ");
                NNS_G3D_DLPARAM_44(param);
                param += G3OP_MTX_LOAD_4x4_NPARAMS;
                break;
            case G3OP_MTX_LOAD_4x3:
                RES_PRINTF("LoadMatrix43 ");
                NNS_G3D_DLPARAM_VEC(*(param + 0), *(param + 1), *(param + 2));
                RES_PRINTF("\n             ");
                NNS_G3D_DLPARAM_VEC(*(param + 3), *(param + 4), *(param + 5));
                RES_PRINTF("\n             ");
                NNS_G3D_DLPARAM_VEC(*(param + 6), *(param + 7), *(param + 8));
                RES_PRINTF("\n             ");
                NNS_G3D_DLPARAM_VEC(*(param + 9), *(param + 10), *(param + 11));
                RES_PRINTF("\n");
                param += G3OP_MTX_LOAD_4x3_NPARAMS;
                break;
            case G3OP_MTX_MULT_4x4:
                RES_PRINTF("MultMatrix44 ");
                NNS_G3D_DLPARAM_44(param);
                param += G3OP_MTX_MULT_4x4_NPARAMS;
                break;
            case G3OP_MTX_MULT_4x3:
                RES_PRINTF("MultMatrix43 ");
                NNS_G3D_DLPARAM_VEC(*(param + 0), *(param + 1), *(param + 2));
                RES_PRINTF("\n             ");
                NNS_G3D_DLPARAM_VEC(*(param + 3), *(param + 4), *(param + 5));
                RES_PRINTF("\n             ");
                NNS_G3D_DLPARAM_VEC(*(param + 6), *(param + 7), *(param + 8));
                RES_PRINTF("\n             ");
                NNS_G3D_DLPARAM_VEC(*(param + 9), *(param + 10), *(param + 11));
                RES_PRINTF("\n");
                param += G3OP_MTX_MULT_4x3_NPARAMS;
                break;
            case G3OP_MTX_MULT_3x3:
                RES_PRINTF("MultMatrix33 ");
                NNS_G3D_DLPARAM_VEC(*(param + 0), *(param + 1), *(param + 2));
                RES_PRINTF("\n             ");
                NNS_G3D_DLPARAM_VEC(*(param + 3), *(param + 4), *(param + 5));
                RES_PRINTF("\n             ");
                NNS_G3D_DLPARAM_VEC(*(param + 6), *(param + 7), *(param + 8));
                RES_PRINTF("\n");
                param += G3OP_MTX_MULT_3x3_NPARAMS;
                break;
            case G3OP_MTX_SCALE:
                RES_PRINTF("Scale ");
                NNS_G3D_DLPARAM_VEC(*param, *(param + 1), *(param + 2));
                RES_PRINTF("\n");
                param += G3OP_MTX_SCALE_NPARAMS;
                break;
            case G3OP_MTX_TRANS:
                RES_PRINTF("Trans ");
                NNS_G3D_DLPARAM_VEC(*param, *(param + 1), *(param + 2));
                RES_PRINTF("\n");
                param += G3OP_MTX_TRANS_NPARAMS;
                break;
            case G3OP_COLOR:
                RES_PRINTF("Color ");
                NNS_G3D_DLPARAM_COLOR(*param & 0x7fff);
                RES_PRINTF("\n");
                param += G3OP_COLOR_NPARAMS;
                break;
            case G3OP_NORMAL:
                RES_PRINTF("Normal (%f, %f, %f)\n",
                           (((s32)((*param & 0x3ff) << 22) >> 19) / 4096.f),
                           (((s32)((*param & 0xffc00) << 12) >> 19) / 4096.f),
                           (((s32)((*param & 0x3ff00000) <<  2) >> 19) / 4096.f));
                param += G3OP_NORMAL_NPARAMS;
                break;
            case G3OP_TEXCOORD:
                RES_PRINTF("TexCoord (%f, %f)\n",
                           (((s32)(*param << 16) >> 8) / 4096.f),
                           (((s32)(*param & 0xffff0000) >> 8) / 4096.f));
                param += G3OP_TEXCOORD_NPARAMS;
                break;
            case G3OP_VTX_16:
                RES_PRINTF("Vertex ");
                NNS_G3D_DLPARAM_FX(*param & 0xffff);
                RES_PRINTF(", ");
                NNS_G3D_DLPARAM_FX(*param >> 16);
                RES_PRINTF(", ");
                NNS_G3D_DLPARAM_FX(*(param + 1) & 0xffff);
                RES_PRINTF("\n");
                param += G3OP_VTX_16_NPARAMS;
                break;
            case G3OP_VTX_10:
                RES_PRINTF("VertexShort ");
                NNS_G3D_DLPARAM_FX((fx16)((*param & 0x3ff) << 6));
                RES_PRINTF(", ");
                NNS_G3D_DLPARAM_FX((fx16)(((*param >> 10) & 0x3ff) << 6));
                RES_PRINTF(", ");
                NNS_G3D_DLPARAM_FX((fx16)(((*param >> 20) & 0x3ff) << 6));
                RES_PRINTF("\n");
                param += G3OP_VTX_10_NPARAMS;
                break;
            case G3OP_VTX_XY:
                RES_PRINTF("VertexXY ");
                NNS_G3D_DLPARAM_FX16PAIR(*param);
                param += G3OP_VTX_XY_NPARAMS;
                break;
            case G3OP_VTX_XZ:
                RES_PRINTF("VertexXZ ");
                NNS_G3D_DLPARAM_FX16PAIR(*param);
                param += G3OP_VTX_XZ_NPARAMS;
                break;
            case G3OP_VTX_YZ:
                RES_PRINTF("VertexYZ ");
                NNS_G3D_DLPARAM_FX16PAIR(*param);
                param += G3OP_VTX_YZ_NPARAMS;
                break;
            case G3OP_VTX_DIFF:
                RES_PRINTF("VertexDiff (%f, %f, %f)\n",
                           (((s32)((*param & 0x3ff) << 22) >> 22) / 4096.f),
                           (((s32)((*param & 0xffc00) << 12) >> 22) / 4096.f),
                           (((s32)((*param & 0x3ff00000) <<  2) >> 22) / 4096.f));
                param += G3OP_VTX_DIFF_NPARAMS;
                break;
            case G3OP_POLYGON_ATTR:
                RES_PRINTF("PolygonAttr %08x", *param);
                param += G3OP_POLYGON_ATTR_NPARAMS;
                break;
            case G3OP_TEXIMAGE_PARAM:
                RES_PRINTF("TexImageParam %08x", *param);
                param += G3OP_TEXIMAGE_PARAM_NPARAMS;
                break;
            case G3OP_TEXPLTT_BASE:
                RES_PRINTF("TexPlttBase %08x", *param);
                param += G3OP_TEXPLTT_BASE_NPARAMS;
                break;
            case G3OP_DIF_AMB:
                RES_PRINTF("MaterialColor0 diffuse=");
                NNS_G3D_DLPARAM_COLOR(*param & 0x7fff);
                RES_PRINTF(", ambient=");
                NNS_G3D_DLPARAM_COLOR(*param >> 16);
                RES_PRINTF(", vtxColor=%d\n", (*param >> 15) & 1);
                param += G3OP_DIF_AMB_NPARAMS;
                break;
            case G3OP_SPE_EMI:
                RES_PRINTF("MaterialColor1 specular=");
                NNS_G3D_DLPARAM_COLOR(*param & 0x7fff);
                RES_PRINTF(", emission=");
                NNS_G3D_DLPARAM_COLOR(*param >> 16);
                RES_PRINTF(", shininess=%d\n", (*param >> 15) & 1);
                param += G3OP_SPE_EMI_NPARAMS;
                break;
            case G3OP_LIGHT_VECTOR:
                RES_PRINTF("LightVector %d, (%f, %f, %f)\n", *param >> 30,
                           ((s32)((*param & 0x3ff) << 22) >> 19) / 4096.f,
                           ((s32)((*param & 0x3ff) << 12) >> 19) / 4096.f,
                           ((s32)((*param & 0x3ff) <<  2) >> 19) / 4096.f);
                param += G3OP_LIGHT_VECTOR_NPARAMS;
                break;
            case G3OP_LIGHT_COLOR:
                RES_PRINTF("LightColor %d, ", *param >> 30);
                NNS_G3D_DLPARAM_COLOR(*param & 0x7fff);
                param += G3OP_LIGHT_COLOR_NPARAMS;
                RES_PRINTF("\n");
                break;
            case G3OP_SHININESS:
                RES_PRINTF("Shininess\n");
                param += G3OP_SHININESS_NPARAMS;
                break;
            case G3OP_BEGIN:
                RES_PRINTF("Begin ");
                switch(*param)
                {
                    NNS_G3D_DLPARAM_CASE(GX_BEGIN_TRIANGLES);
                    NNS_G3D_DLPARAM_CASE(GX_BEGIN_QUADS);
                    NNS_G3D_DLPARAM_CASE(GX_BEGIN_TRIANGLE_STRIP);
                    NNS_G3D_DLPARAM_CASE(GX_BEGIN_QUAD_STRIP);
                    NNS_G3D_DLPARAM_DEFAULT;
                }
                param += G3OP_BEGIN_NPARAMS;
                break;
            case G3OP_END:
                RES_PRINTF("End\n");
                //param += G3OP_END_NPARAMS;
                break;
            case G3OP_SWAP_BUFFERS:
                RES_PRINTF("SwapBuffers ");
                switch(*param)
                {
                    NNS_G3D_DLPARAM_CASE(GX_SORTMODE_AUTO   | GX_BUFFERMODE_Z << 1);
                    NNS_G3D_DLPARAM_CASE(GX_SORTMODE_MANUAL | GX_BUFFERMODE_Z << 1);
                    NNS_G3D_DLPARAM_CASE(GX_SORTMODE_AUTO   | GX_BUFFERMODE_W << 1);
                    NNS_G3D_DLPARAM_CASE(GX_SORTMODE_MANUAL | GX_BUFFERMODE_W << 1);
                    NNS_G3D_DLPARAM_DEFAULT;
                }
                param += G3OP_SWAP_BUFFERS_NPARAMS;
                break;
            case G3OP_VIEWPORT:
                RES_PRINTF("ViewPort (%d, %d)-(%d, %d)",
                            *param & 0xff, (*param >> 8) & 0xff,
                            (*param >> 16) & 0xff, (*param >> 24) & 0xff);
                param += G3OP_VIEWPORT_NPARAMS;
                break;
            case G3OP_BOX_TEST:
                RES_PRINTF("BoxTest\n");
                param += G3OP_BOX_TEST_NPARAMS;
                break;
            case G3OP_POS_TEST:
                RES_PRINTF("PositionTest\n");
                param += G3OP_POS_TEST_NPARAMS;
                break;
            case G3OP_VEC_TEST:
                RES_PRINTF("VectorTest\n");
                param += G3OP_VEC_TEST_NPARAMS;
                break;
            default:
                RES_PRINTF("Unknown command %02x\n", cmd);
                return;
                break;
            }
        }
        cur = param;
    }
#undef NNS_G3D_DLPARAM_CASE
#undef NNS_G3D_DLPARAM_DEFAULT
#undef NNS_G3D_DLPARAM_FX
#undef NNL_G3D_DLPARAM_FX16PAIR
#undef NNS_G3D_DLPARAM_VEC
#undef NNS_G3D_DLPARAM_44
}


//
// NNS_G3dPrintMatData用サブ関数群
//

// マテリアルID(idx)でテクスチャが使われていればテクスチャの名前を返す
static const NNSG3dResName*
getTexNameForMatID_(const NNSG3dResMat* mat, u32 idx)
{
    u32 i, j;
    const NNSG3dResDict* texDict;
    NNS_G3D_NULL_ASSERT(mat);

    texDict = (const NNSG3dResDict*)((u8*)mat + mat->ofsDictTexToMatList);
    for (i = 0; i < texDict->numEntry; ++i)
    {
        const NNSG3dResDictTexToMatIdxData* data =
            (const NNSG3dResDictTexToMatIdxData*)NNS_G3dGetResDataByIdx(texDict, i); 

        u8* first = (u8*)mat + data->offset;

        for (j = 0; j < data->numIdx; ++j)
        {
            if (*(first + j) == idx)
            {
                return NNS_G3dGetResNameByIdx(texDict, i);
            }
        }
    }
    return NULL;
}


// マテリアルID(idx)でパレットが使われていればパレットの名前を返す
static const NNSG3dResName*
getPlttNameForMatID_(const NNSG3dResMat* mat, u32 idx)
{
    u32 i, j;
    const NNSG3dResDict* plttDict;
    NNS_G3D_NULL_ASSERT(mat);

    plttDict = (const NNSG3dResDict*)((u8*)mat + mat->ofsDictPlttToMatList);
    for (i = 0; i < plttDict->numEntry; ++i)
    {
        const NNSG3dResDictTexToMatIdxData* data =
            (const NNSG3dResDictTexToMatIdxData*)NNS_G3dGetResDataByIdx(plttDict, i);

        u8* first = (u8*)mat + data->offset;

        for (j = 0; j < data->numIdx; ++j)
        {
            if (*(first + j) == idx)
            {
                return NNS_G3dGetResNameByIdx(plttDict, i);
            }
        }
    }
    return NULL;
}


// RGBの表示
static void
printRGB_(const char* name, u32 rgb)
{
    u32 r = rgb & 0x1f;
    u32 g = (rgb >> 5) & 0x1f;
    u32 b = (rgb >> 10) & 0x1f;

    NNS_G3D_NULL_ASSERT(name);

    RES_PRINTF("%s, %d, %d, %d\n", name, r, g, b);
}


/*---------------------------------------------------------------------------*
    NNS_G3dPrintMatData

    個々のマテリアルのサマリを出力
 *---------------------------------------------------------------------------*/
void
NNS_G3dPrintMatData(const NNSG3dResMat* mat,
                    u32 idx)
{
    const NNSG3dResMatData* matData = NNS_G3dGetMatDataByIdx(mat, idx);
    const NNSG3dResName* resName = NNS_G3dGetMatNameByIdx(mat, idx);

    const NNSG3dResDict* plttDict =
        (const NNSG3dResDict*)((u8*)mat + mat->ofsDictPlttToMatList);

    const NNSG3dResName* texName = getTexNameForMatID_(mat, idx);
    const NNSG3dResName* plttName = getPlttNameForMatID_(mat, idx);

    //
    // MatID, MatName, a texture used, a palette used
    //
    tabPrint_();
    RES_PRINTF(" %d, ", idx);
    NNS_G3dPrintResName(resName);

    if (matData->flag & NNS_G3D_MATFLAG_WIREFRAME)
    {
        RES_PRINTF(", Wireframe");
    }
    if (matData->flag & NNS_G3D_MATFLAG_VTXCOLOR)
    {
        RES_PRINTF(", VtxColor");
    }
    if (matData->flag & NNS_G3D_MATFLAG_SHININESS)
    {
        RES_PRINTF(", Shininess");
    }

    RES_PRINTF("\n");

    //
    // Parameters for a material
    //
    tabPlus_();
    {
        if (NULL != texName)
        {
            tabPrint_();
            RES_PRINTF("Texure, ");
            NNS_G3dPrintResName(texName);
            RES_PRINTF("\n");
        }

        if (NULL != plttName)
        {
            tabPrint_();
            RES_PRINTF("Palette, ");
            NNS_G3dPrintResName(plttName);
            RES_PRINTF("\n");
        }

        {
            tabPrint_();
            RES_PRINTF("PolygonAttr, val=0x%08x, mask=0x%08x\n",
                       matData->polyAttr, matData->polyAttrMask);
        }
        
        {
            tabPrint_();
            RES_PRINTF("TexImageParam, val=0x%08x, mask=0x%08x\n",
                       matData->texImageParam, matData->texImageParamMask);
        }

        if (matData->flag & NNS_G3D_MATFLAG_DIFFUSE)
        {
            tabPrint_(); printRGB_("Diffuse ", (matData->diffAmb & 0x7fff));
        }
        if (matData->flag & NNS_G3D_MATFLAG_AMBIENT)
        {
            tabPrint_(); printRGB_("Ambient ", (matData->diffAmb >> 16) & 0x7fff);
        }
        if (matData->flag & NNS_G3D_MATFLAG_SPECULAR)
        {
            tabPrint_(); printRGB_("Specular", (matData->specEmi & 0x7fff));
        }
        if (matData->flag & NNS_G3D_MATFLAG_EMISSION)
        {
            tabPrint_(); printRGB_("Emission", (matData->specEmi >> 16) & 0x7fff);
        }
    }
    tabMinus_();
}


/*---------------------------------------------------------------------------*
    NNS_G3dPrintMat

    単一モデル内のマテリアル情報を出力
 *---------------------------------------------------------------------------*/
void
NNS_G3dPrintMat(const NNSG3dResMat* mat)
{
    NNS_G3D_NULL_ASSERT(mat);

    tabPrint_(); RES_PRINTF("# of materials, %d\n", mat->dict.numEntry);
    tabPlus_();
    {
        u32 i;
        u32 numEntry = mat->dict.numEntry;

        for (i = 0; i < numEntry; ++i)
        {
            NNS_G3dPrintMatData(mat, i);
        }
    }
    tabMinus_();
}


/*---------------------------------------------------------------------------*
    NNS_G3dPrintShp

    単一モデル内のシェイプ情報を出力
 *---------------------------------------------------------------------------*/
void
NNS_G3dPrintShp(const NNSG3dResShp* shp)
{
    NNS_G3D_NULL_ASSERT(shp);

    tabPrint_(); RES_PRINTF("# of shapes, %d\n", shp->dict.numEntry);
    tabPlus_();
    {
        u32 i;
        const NNSG3dResDict* dict = &shp->dict;
        u32 numEntry = dict->numEntry;

        for (i = 0; i < numEntry; ++i)
        {
            const NNSG3dResShpData* shpData = NNS_G3dGetShpDataByIdx(shp, i);

            tabPrint_();
            RES_PRINTF(" %d, ", i);
            NNS_G3dPrintResName(NNS_G3dGetShpNameByIdx(shp, i));
            RES_PRINTF(", %d", shpData->sizeDL);

            if (shpData->flag & NNS_G3D_SHPFLAG_USE_NORMAL)
            {
                RES_PRINTF(", UseNormal");
            }

            if (shpData->flag & NNS_G3D_SHPFLAG_USE_COLOR)
            {
                RES_PRINTF(", UseColor");
            }

            if (shpData->flag & NNS_G3D_SHPFLAG_USE_TEXCOORD)
            {
                RES_PRINTF(", UseTexCoord");
            }

            if (shpData->flag & NNS_G3D_SHPFLAG_USE_RESTOREMTX)
            {
                RES_PRINTF(", UseRestoreMatrix");
            }

            RES_PRINTF("\n");
        }
    }
    tabMinus_();
}


/*---------------------------------------------------------------------------*
    NNS_G3dPrintMdl

    モデルバイナリのサマリ表示
 *---------------------------------------------------------------------------*/
void
NNS_G3dPrintMdl(const NNSG3dResName* resName,
                const NNSG3dResMdl* mdl)
{
    NNS_G3D_NULL_ASSERT(resName);
    NNS_G3D_NULL_ASSERT(mdl);

    tabPrint_(); RES_PRINTF("Model, "); NNS_G3dPrintResName(resName); RES_PRINTF("\n");
    tabPlus_();
    {
        NNS_G3dPrintMdlInfo(NNS_G3dGetMdlInfo(mdl));
        NNS_G3dPrintNodeInfo(NNS_G3dGetNodeInfo(mdl));
        NNS_G3dPrintSbc(NNS_G3dGetSbc(mdl), mdl->ofsMat - mdl->ofsSbc, mdl);
        NNS_G3dPrintMat(NNS_G3dGetMat(mdl));
        NNS_G3dPrintShp(NNS_G3dGetShp(mdl));
    }
    tabMinus_();
}


/*---------------------------------------------------------------------------*
    NNS_G3dPrintMdlSet

    モデルセットバイナリのサマリ表示
 *---------------------------------------------------------------------------*/
void
NNS_G3dPrintMdlSet(const NNSG3dResMdlSet* mdlSet)
{
    NNS_G3D_NULL_ASSERT(mdlSet);

    tabPlus_();
    {
        u32 i;
        const NNSG3dResDict* dict = &mdlSet->dict;
        u32 numEntry = dict->numEntry;

        tabPrint_(); RES_PRINTF("# of models, %d\n", numEntry);
        for (i = 0; i < numEntry; ++i)
        {
            const NNSG3dResName* name = NNS_G3dGetMdlNameByIdx(mdlSet, i);
            chkDict_(&mdlSet->dict, name, i);
            NNS_G3dPrintMdl(name, NNS_G3dGetMdlByIdx(mdlSet, i));
        }
    }
    tabMinus_();
}


/*---------------------------------------------------------------------------*
    NNS_G3dPrintNSBMD

    NSBMDバイナリのサマリ表示
 *---------------------------------------------------------------------------*/
void
NNS_G3dPrintNSBMD(const u8* binFile)
{
    u32 i;
    NNSG3dResFileHeader* header;
    u32 numBlocks;
    const u32* blks;
    NNS_G3D_NULL_ASSERT(binFile);

    header = (NNSG3dResFileHeader*)&binFile[0];
    NNS_G3dPrintFileHeader(header);
    numBlocks = header->dataBlocks;
    blks = (u32*)((u8*)header + header->headerSize);   

    for (i = 0; i < numBlocks; ++i)
    {
        const NNSG3dResDataBlockHeader* blk = NNS_G3dGetDataBlockHeaderByIdx(header, i);

        u32 kind = blk->kind;
        u8* contents = (u8*)blk + sizeof(NNSG3dResDataBlockHeader);
        NNS_G3dPrintDataBlockHeader(blk);
        switch(kind)
        {
        case NNS_G3D_DATABLK_TEX:
            NNS_G3dPrintTex(NNS_G3dCheckTex(blk));
            break;
        case NNS_G3D_DATABLK_MDLSET:
            NNS_G3dPrintMdlSet(NNS_G3dCheckMdlSet(blk));
            break;
        default:
            tabPlus_();
            tabPrint_(); RES_PRINTF("cannot display this block\n");
            tabMinus_();
            break;
        }
    }
}


/*---------------------------------------------------------------------------*
    NNS_G3dPrintNSBTX

    NSBTXバイナリのサマリ表示
 *---------------------------------------------------------------------------*/
void
NNS_G3dPrintNSBTX(const u8* binFile)
{
    u32 i;
    NNSG3dResFileHeader* header;
    u32 numBlocks;
    const u32* blks;
    NNS_G3D_NULL_ASSERT(binFile);

    header = (NNSG3dResFileHeader*)&binFile[0];
    NNS_G3dPrintFileHeader(header);
    numBlocks = header->dataBlocks;
    blks = (u32*)((u8*)header + header->headerSize);

    for (i = 0; i < numBlocks; ++i)
    {
        NNSG3dResDataBlockHeader* blk = 
            (NNSG3dResDataBlockHeader*)((u8*)header + blks[i]);
        u32 kind = blk->kind;
        u8* contents = (u8*)blk + sizeof(NNSG3dResDataBlockHeader);
        NNS_G3dPrintDataBlockHeader(blk);
        switch(kind)
        {
        case NNS_G3D_DATABLK_TEX:
            NNS_G3dPrintTex((NNSG3dResTex*)blk);
            break;
        default:
            tabPlus_();
            tabPrint_(); RES_PRINTF("cannot display this block\n");
            tabMinus_();
            break;
        }
    }
}


/*---------------------------------------------------------------------------*
    NNS_G3dPrintBinFile

    バイナリファイルのサマリを適切に表示
 *---------------------------------------------------------------------------*/
void
NNS_G3dPrintBinFile(const u8* binFile)
{
    NNS_G3D_NULL_ASSERT(binFile);

    switch(*(u32*)&binFile[0])
    {
    case NNS_G3D_SIGNATURE_NSBTX: // BTX0
        NNS_G3dPrintNSBTX(&binFile[0]);
        break;
    case NNS_G3D_SIGNATURE_NSBMD: // BMD0
        NNS_G3dPrintNSBMD(&binFile[0]);
        break;
    case NNS_G3D_SIGNATURE_NSBCA: // BCA0
        NNS_G3dPrintNSBCA(&binFile[0]);
        break;
    case NNS_G3D_SIGNATURE_NSBVA: // BVA0
        NNS_G3dPrintNSBVA(&binFile[0]);
        break;
    case NNS_G3D_SIGNATURE_NSBMA: // BMA0
        NNS_G3dPrintNSBMA(&binFile[0]);
        break;
    case NNS_G3D_SIGNATURE_NSBTP: // BTP0
        NNS_G3dPrintNSBTP(&binFile[0]);
        break;
    case NNS_G3D_SIGNATURE_NSBTA: // BTA0
        NNS_G3dPrintNSBTA(&binFile[0]);
        break;
    default:
        RES_PRINTF("unknown file signature: '%c%c%c%c' found.\n",
                   binFile[0], binFile[1], binFile[2], binFile[3]);
        RES_PRINTF("This file may not be a G3d resource file.\n");
        break;
    };
}


////////////////////////////////////////////////////////////////////////////////
//
// Misc
//
//
static u32 sNumTab = 0;

void tabPlus_(void)
{
    sNumTab++;
}

void tabMinus_(void)
{
    if (sNumTab > 0)
        sNumTab--;
}

void tabPrint_(void)
{
    u32 i;
    for (i = 0; i < sNumTab; ++i)
    {
        RES_PRINTF("  ,");
    }
}

void printFx32_(fx32 v)
{
    RES_PRINTF("%f", v / 4096.f);
}

f32 fx32Tof32_(fx32 v)
{
    return (f32)(v / 4096.f);
}

void chkDict_(const NNSG3dResDict* dict,
              const NNSG3dResName* name,
              u32 idx)
{
    NNS_G3D_NULL_ASSERT(dict);
    NNS_G3D_NULL_ASSERT(name);

    if (idx != NNS_G3dGetResDictIdxByName(dict, name))
    {
        RES_PRINTF(" ***name-index inconsistency(%d, %s)\n",
                   idx, name->name);
    }
}

//
// その他ユーティリティ(別途ファイル分けをしたほうがよいか？)
//
fx32 CvtrFX_Mul( fx32 v1, fx32 v2 )
{
    return ((fx32) (((s64) (v1) * v2 + 0x800LL) >> FX32_SHIFT));
}


#endif
