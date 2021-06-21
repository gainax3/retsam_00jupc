#!/usr/bin/perl

use POSIX;

$handle = STDOUT;
$size_atan = 128;

print $handle <<ENDDOC;
/*---------------------------------------------------------------------------*
  Project:  NitroSDK - FX - 
  File:     fx_atan.c

  Copyright 2003-2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/

//margin of error

#include <nitro/fx/fx_trig.h>
#include <nitro/fx/fx_cp.h>
#include <nitro/fx/fx_const.h>

const fx16 FX_AtanTable_[$size_atan] = {
ENDDOC

    for ($i = 0; $i < $size_atan; ++$i) {
        my $val = sprintf("%.0f", atan2($i / $size_atan, 1.0) * 4096);
        if ($i == $size_atan - 1) {
            printf $handle "    (fx16)0x%04x           // %.3f\n", $val, ($val / 4096.0);
        } else {
            printf $handle "    (fx16)0x%04x,          // %.3f\n", $val, ($val / 4096.0);
        }
    }


    print $handle "};\n";
    print $handle "\n";

print $handle <<ENDDOC;
fx16 FX_Atan(fx32 x)
{
    if (x >= 0)
    {
        if (x > FX32_ONE)
        {
            return (fx16)(6434 - FX_AtanTable_[FX_Inv(x) / (4096 / $size_atan)]);
        }
        else if (x < FX32_ONE)
        {
            return FX_AtanTable_[x / (4096 / $size_atan)];
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
            return (fx16)(-6434 + FX_AtanTable_[FX_Inv(-x) / (4096 / $size_atan)]);
        }
        else if (x > -FX32_ONE)
        {
            return (fx16)-FX_AtanTable_[-x / (4096 / $size_atan)];
        }
        else
        {
            return (fx16)-3217;
        }
    }
}

fx16 FX_Atan2(fx32 y, fx32 x)
{
    fx32 a, b, c;
    int sgn;
    
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
    else // y = 0
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
        return (fx16)(c + FX_AtanTable_[FX_Div(a, b) / (4096 / $size_atan)]);
    else
        return (fx16)(c - FX_AtanTable_[FX_Div(a, b) / (4096 / $size_atan)]);
}
ENDDOC
    