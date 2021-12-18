#!/usr/bin/perl -s

##############################################################################
#
# Project:  NitroSDK - header generator - 
# File:     fx_const_generator.pl
#
# Copyright 2003-2004 Nintendo.  All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law.  They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Log: fx_const_generator.pl,v $
# Revision 1.12  2004/02/05 07:09:01  yasu
# change SDK prefix iris -> nitro
#
# Revision 1.11  2003/12/08 06:33:26  nishida_kenji
# adds FX64C_ONE,
# correct typecast: (fx64) -> (fx64c)
# adds 'LL' suffix to 64bit constants
# adds 'L' suffix to 32bit constants
#
# Revision 1.10  2003/11/26 12:25:35  nishida_kenji
# コメントを追加するようにした。
#
# Revision 1.9  2003/11/26 10:51:54  nishida_kenji
# 丸めの際に誤差が出る可能性のあるバグの修正
#
# Revision 1.8  2003/11/25 01:49:42  nishida_kenji
# バグフィックス
#
# Revision 1.7  2003/11/19 11:35:12  nishida_kenji
# FX16_ONE
#
# Revision 1.6  2003/11/18 10:51:56  nishida_kenji
# fx64c型に
#
# Revision 1.5  2003/11/17 01:28:11  nishida_kenji
# fx32の間違いなおし
#
# Revision 1.4  2003/11/17 00:33:48  nishida_kenji
# ディレクトリ変更
#
# Revision 1.3  2003/11/14 12:51:48  nishida_kenji
# Math::BigIntで大きな数が使えます
#
# Revision 1.2  2003/11/14 12:39:54  nishida_kenji
# 注意書き追加
#
# $NoKeywords: $
##############################################################################

use Text::ParseWords;
use Math::BigInt;

#
# Global Variables
#
$line_no = 0;
%name_hash = ();
@hash_array = ();
$filename = $ARGV[0];

#
# フィールドの最初と最後の空白を取り除く
#
sub trim {
  my @out = @_;
  for (@out) {
    s/^\s+//;
    s/\s+$//;
  }
  return @out;
}

#
# CSV解析ルーチン
#
sub parse_csv {
  my @fields = quotewords(",", 0, shift @_);
  @fields = trim(@fields);
  
  my $tmp;
  # 行末の,,,,を取り除く
  while(defined($tmp = pop @fields) && $tmp =~ /^$/ ) {
       ;
  }
  push @fields, $tmp;

  return @fields;
}

#
# プリプロセッサ
#
sub preprocess {
  return parse_csv(shift @_);
}

#
# 解析
#

sub analyze {
  my @fileds = @_;
  
  my $tmphash;
  my $name, $type, $value;
  $tmphash = {};
  
  $name = shift @fields;
  $type = shift @fields;
  $value = shift @fields;
  $tmphash->{"name"} = $name;
  $tmphash->{"type"} = $type;
  $tmphash->{"value"} = $value;
  return $tmphash;
}


#
# メインルーチン
#
open INPUTFILE, "$filename" or die  "ERROR: Cannot open file \'$filename\'\n";

my $line;
while($line = <INPUTFILE>) {
  $line_no++;
  $line =~ s/\"//g;
  $line =~ s/\#.*//;
  $line =~ s/\'//;
  next if ($line =~ /^[\s,]*$/);

  @fields = preprocess($line);
  push @hash_array, analyze(@fields);
}

my $output_filename = $filename;
if (($output_filename =~ s/\.csv/\.h/) == 0) {
  $output_filename .= ".h";
}
open OUT, ">$output_filename" or die "ERROR: Cannot create file \'$output_filename\'\n";
my $handle = OUT;

my $include_guard = $output_filename;
$include_guard =~ s/[.\/]/_/g;
$include_guard = "nitro_". uc($include_guard). "_";

#
# 出力
#
print $handle <<ENDDOC;
/*---------------------------------------------------------------------------*
  Project:  NitroSDK - IO Register List - 
  File:     $output_filename

  Copyright 2003-2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/
//
//  I was generated automatically, don't edit me directly!!!
//
#ifndef $include_guard
#define $include_guard

#include <nitro/fx/fx.h>

#ifdef __cplusplus
extern "C" {
#endif

ENDDOC




foreach $hash (@hash_array) {
  if ($hash->{"type"} eq "fx64c") {
    my $tmp = "#define " . uc($hash->{"type"}) . "_" . $hash->{"name"};
    my $val = sprintf("%.0f", $hash->{"value"} * 4294967296.0);
    printf $handle "%-40s ((fx64c) 0x%016xLL)    // %.9f\n", $tmp, $val, ($val / 4294967296.0);
  } elsif ($hash->{"type"} eq "fx32") {
    my $tmp = "#define " . uc($hash->{"type"}) . "_" . $hash->{"name"};
    my $val = sprintf("%.0f", $hash->{"value"} * 4096.0);
    printf $handle "%-40s ((fx32) 0x%08xL)     // %.3f\n", $tmp, ($val & 0xffffffff), $val / 4096.0;
  } elsif ($hash->{"type"} eq "fx16") {
    my $tmp = "#define " . uc($hash->{"type"}) . "_" . $hash->{"name"};
    my $val = sprintf("%.0f", $hash->{"value"} * 4096.0);
    printf $handle "%-40s ((fx16) 0x%04x)     // %.3f\n", $tmp, ($val & 0xffff), $val / 4096.0;
  } else {
    die "Sorry, $hash->{'type'} not supported\n";
  }
}



  print $handle <<ENDDOC;

#ifdef __cplusplus
} /* extern "C" */
#endif

/* $include_guard */
#endif
ENDDOC

