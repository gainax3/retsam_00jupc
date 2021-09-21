#!/usr/bin/perl

$handle = STDOUT;

print $handle <<ENDDOC;
/*---------------------------------------------------------------------------*
  Project:  NitroSDK - FX - 
  File:     fx_sincos.c

  Copyright 2003-2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/
#include <nitro/fx/fx_trig.h>

const fx16 FX_SinCosTable_[4096 * 2] = {
ENDDOC

  for ($i = 0; $i < 4096; ++$i) {
    my $deg = $i * 360 / 4096.0;
    my $valsin = sprintf("%.0f", sin($i * 6.28318530717958647692 / 4096.0) * 4096.0);
    my $valcos = sprintf("%.0f", cos($i * 6.28318530717958647692 / 4096.0) * 4096.0);
    if ($i == 4095) {
      printf $handle "    (fx16)0x%04x, (fx16)0x%04x             // %.3f, %.3f ( deg = %f )\n", ($valsin & 0xffff), ($valcos & 0xffff), ($valsin / 4096.0), ($valcos / 4096.0), $deg;
    } else {
      printf $handle "    (fx16)0x%04x, (fx16)0x%04x,            // %.3f, %.3f ( deg = %f )\n", ($valsin & 0xffff), ($valcos & 0xffff), ($valsin / 4096.0), ($valcos / 4096.0), $deg;
    }
  }

print $handle "};\n";



