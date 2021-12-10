/*---------------------------------------------------------------------------*
  Project:  NitroSDK - FX - 
  File:     fx_atan.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: fx_atan.c,v $
  Revision 1.10  2006/01/18 02:11:20  kitase_hirotake
  do-indent

  Revision 1.9  2005/07/12 02:25:40  yosizaki
  fix about size of atan-table.

  Revision 1.8  2005/03/01 01:57:00  yosizaki
  copyright ‚Ì”N‚ðC³.

  Revision 1.7  2005/02/28 05:26:13  yosizaki
  do-indent.

  Revision 1.6  2004/02/12 02:50:21  nishida_kenji
  Do not force ARM binary.

  Revision 1.5  2004/02/05 07:09:02  yasu
  change SDK prefix iris -> nitro

  Revision 1.4  2004/02/03 06:43:03  nishida_kenji
  Add comments.

  Revision 1.3  2004/01/26 07:20:45  nishida_kenji
  include code32.h

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

//worst case
//atan(-1.0 -> 1.0) x = -0.085693                                0.007604
//atan(1.0 -> 100.0) x = 11.636475                               0.007850
//atan(-1.0 -> -100.0) x = -11.636475                            0.007850
//atan2(-1.0 -> 1.0, -1.0 -> 1.0) (x = -0.636719, y = -0.054688) 0.007807

#include <nitro/fx/fx_trig.h>
#include <nitro/fx/fx_cp.h>
#include <nitro/fx/fx_const.h>

const fx16 FX_AtanTable_[128 + 1] = {
    (fx16)0x0000,                      // 0.000
    (fx16)0x0020,                      // 0.008
    (fx16)0x0040,                      // 0.016
    (fx16)0x0060,                      // 0.023
    (fx16)0x0080,                      // 0.031
    (fx16)0x00a0,                      // 0.039
    (fx16)0x00c0,                      // 0.047
    (fx16)0x00e0,                      // 0.055
    (fx16)0x0100,                      // 0.062
    (fx16)0x0120,                      // 0.070
    (fx16)0x013f,                      // 0.078
    (fx16)0x015f,                      // 0.086
    (fx16)0x017f,                      // 0.094
    (fx16)0x019f,                      // 0.101
    (fx16)0x01be,                      // 0.109
    (fx16)0x01de,                      // 0.117
    (fx16)0x01fd,                      // 0.124
    (fx16)0x021d,                      // 0.132
    (fx16)0x023c,                      // 0.140
    (fx16)0x025c,                      // 0.147
    (fx16)0x027b,                      // 0.155
    (fx16)0x029a,                      // 0.163
    (fx16)0x02b9,                      // 0.170
    (fx16)0x02d8,                      // 0.178
    (fx16)0x02f7,                      // 0.185
    (fx16)0x0316,                      // 0.193
    (fx16)0x0335,                      // 0.200
    (fx16)0x0354,                      // 0.208
    (fx16)0x0372,                      // 0.215
    (fx16)0x0391,                      // 0.223
    (fx16)0x03af,                      // 0.230
    (fx16)0x03cd,                      // 0.238
    (fx16)0x03eb,                      // 0.245
    (fx16)0x0409,                      // 0.252
    (fx16)0x0427,                      // 0.260
    (fx16)0x0445,                      // 0.267
    (fx16)0x0463,                      // 0.274
    (fx16)0x0481,                      // 0.281
    (fx16)0x049e,                      // 0.289
    (fx16)0x04bb,                      // 0.296
    (fx16)0x04d9,                      // 0.303
    (fx16)0x04f6,                      // 0.310
    (fx16)0x0513,                      // 0.317
    (fx16)0x052f,                      // 0.324
    (fx16)0x054c,                      // 0.331
    (fx16)0x0569,                      // 0.338
    (fx16)0x0585,                      // 0.345
    (fx16)0x05a1,                      // 0.352
    (fx16)0x05be,                      // 0.359
    (fx16)0x05da,                      // 0.366
    (fx16)0x05f5,                      // 0.372
    (fx16)0x0611,                      // 0.379
    (fx16)0x062d,                      // 0.386
    (fx16)0x0648,                      // 0.393
    (fx16)0x0663,                      // 0.399
    (fx16)0x067e,                      // 0.406
    (fx16)0x0699,                      // 0.412
    (fx16)0x06b4,                      // 0.419
    (fx16)0x06cf,                      // 0.426
    (fx16)0x06e9,                      // 0.432
    (fx16)0x0703,                      // 0.438
    (fx16)0x071e,                      // 0.445
    (fx16)0x0738,                      // 0.451
    (fx16)0x0751,                      // 0.457
    (fx16)0x076b,                      // 0.464
    (fx16)0x0785,                      // 0.470
    (fx16)0x079e,                      // 0.476
    (fx16)0x07b7,                      // 0.482
    (fx16)0x07d0,                      // 0.488
    (fx16)0x07e9,                      // 0.494
    (fx16)0x0802,                      // 0.500
    (fx16)0x081a,                      // 0.506
    (fx16)0x0833,                      // 0.512
    (fx16)0x084b,                      // 0.518
    (fx16)0x0863,                      // 0.524
    (fx16)0x087b,                      // 0.530
    (fx16)0x0893,                      // 0.536
    (fx16)0x08aa,                      // 0.542
    (fx16)0x08c2,                      // 0.547
    (fx16)0x08d9,                      // 0.553
    (fx16)0x08f0,                      // 0.559
    (fx16)0x0907,                      // 0.564
    (fx16)0x091e,                      // 0.570
    (fx16)0x0934,                      // 0.575
    (fx16)0x094b,                      // 0.581
    (fx16)0x0961,                      // 0.586
    (fx16)0x0977,                      // 0.592
    (fx16)0x098d,                      // 0.597
    (fx16)0x09a3,                      // 0.602
    (fx16)0x09b9,                      // 0.608
    (fx16)0x09ce,                      // 0.613
    (fx16)0x09e3,                      // 0.618
    (fx16)0x09f9,                      // 0.623
    (fx16)0x0a0e,                      // 0.628
    (fx16)0x0a23,                      // 0.634
    (fx16)0x0a37,                      // 0.638
    (fx16)0x0a4c,                      // 0.644
    (fx16)0x0a60,                      // 0.648
    (fx16)0x0a74,                      // 0.653
    (fx16)0x0a89,                      // 0.658
    (fx16)0x0a9c,                      // 0.663
    (fx16)0x0ab0,                      // 0.668
    (fx16)0x0ac4,                      // 0.673
    (fx16)0x0ad7,                      // 0.677
    (fx16)0x0aeb,                      // 0.682
    (fx16)0x0afe,                      // 0.687
    (fx16)0x0b11,                      // 0.692
    (fx16)0x0b24,                      // 0.696
    (fx16)0x0b37,                      // 0.701
    (fx16)0x0b49,                      // 0.705
    (fx16)0x0b5c,                      // 0.710
    (fx16)0x0b6e,                      // 0.714
    (fx16)0x0b80,                      // 0.719
    (fx16)0x0b92,                      // 0.723
    (fx16)0x0ba4,                      // 0.728
    (fx16)0x0bb6,                      // 0.732
    (fx16)0x0bc8,                      // 0.736
    (fx16)0x0bd9,                      // 0.740
    (fx16)0x0beb,                      // 0.745
    (fx16)0x0bfc,                      // 0.749
    (fx16)0x0c0d,                      // 0.753
    (fx16)0x0c1e,                      // 0.757
    (fx16)0x0c2f,                      // 0.761
    (fx16)0x0c3f,                      // 0.765
    (fx16)0x0c50,                      // 0.770
    (fx16)0x0c60,                      // 0.773
    (fx16)0x0c71,                      // 0.778
    (fx16)0x0c81,                      // 0.781
    (fx16)0x0c91                       // 0.785
};


/*---------------------------------------------------------------------------*
  Name:         FX_Atan

  Description:  Computes an arc tangent of the input value, making use of
                a table.

  Arguments:    x            Y/X in fx32 format

  Returns:      result in fx16 format
 *---------------------------------------------------------------------------*/
fx16 FX_Atan(fx32 x)
{
    if (x >= 0)
    {
        if (x > FX32_ONE)
        {
            return (fx16)(6434 - FX_AtanTable_[FX_Inv(x) >> 5]);
        }
        else if (x < FX32_ONE)
        {
            return FX_AtanTable_[x >> 5];
        }
        else
        {
            return (fx16)3217;
        }
    }
    else
    {
        if (x < -FX32_ONE)
        {
            return (fx16)(-6434 + FX_AtanTable_[FX_Inv(-x) >> 5]);
        }
        else if (x > -FX32_ONE)
        {
            return (fx16)-FX_AtanTable_[-x >> 5];
        }
        else
        {
            return (fx16)-3217;
        }
    }
}


/*---------------------------------------------------------------------------*
  Name:         FX_Atan2

  Description:  Computes an arc tangent of the input value(y/x), making use of
                a table. Note that 'y' is the first argument.

  Arguments:    y            a value in fx32 format
                x            a value in fx32 format
  
  Returns:      result in fx16 format
 *---------------------------------------------------------------------------*/
fx16 FX_Atan2(fx32 y, fx32 x)
{
    fx32    a, b, c;
    int     sgn;

    if (y > 0)
    {
        if (x > 0)
        {
            if (x > y)
            {
                a = y;
                b = x;
                c = 0;
                sgn = 1;
            }
            else if (x < y)
            {
                a = x;
                b = y;
                c = 6434;
                sgn = 0;
            }
            else
            {
                return (fx16)3217;
            }
        }
        else if (x < 0)
        {
            x = -x;
            if (x < y)
            {
                a = x;
                b = y;
                c = 6434;
                sgn = 1;
            }
            else if (x > y)
            {
                a = y;
                b = x;
                c = 12868;
                sgn = 0;
            }
            else
            {
                return (fx16)9651;
            }
        }
        else
        {
            return (fx16)6434;
        }
    }
    else if (y < 0)
    {
        y = -y;
        if (x < 0)
        {
            x = -x;
            if (x > y)
            {
                a = y;
                b = x;
                c = -12868;
                sgn = 1;
            }
            else if (x < y)
            {
                a = x;
                b = y;
                c = -6434;
                sgn = 0;
            }
            else
            {
                return (fx16)-9651;
            }
        }
        else if (x > 0)
        {
            if (x < y)
            {
                a = x;
                b = y;
                c = -6434;
                sgn = 1;
            }
            else if (x > y)
            {
                a = y;
                b = x;
                c = 0;
                sgn = 0;
            }
            else
            {
                return (fx16)-3217;
            }
        }
        else
        {
            return (fx16)-6434;
        }
    }
    else                               // y = 0
    {
        if (x >= 0)
        {
            return 0;
        }
        else
        {
            return (fx16)12868;
        }
    }

    if (b == 0)
        return 0;
    if (sgn)
        return (fx16)(c + FX_AtanTable_[FX_Div(a, b) >> 5]);
    else
        return (fx16)(c - FX_AtanTable_[FX_Div(a, b) >> 5]);
}
