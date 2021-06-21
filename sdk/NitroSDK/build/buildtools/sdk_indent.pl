#!/usr/bin/perl

##############################################################################
#
# Project:  NitroSDK - code formatter- 
# File:     sdk_indent.pl
#
# Copyright 2003-2004 Nintendo.  All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law.  They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Log: sdk_indent.pl,v $
# Revision 1.2  2004/02/05 07:09:01  yasu
# change SDK prefix iris -> nitro
#
# Revision 1.1  2003/12/01 11:17:08  nishida_kenji
# 新規
#
# $NoKeywords: $
##############################################################################

foreach $filename (@ARGV) {
  print STDERR "processing $filename\n";
  open INFILE, "$filename" or die "cannot open infile $filename\n";
  $intext = "";
  while(<INFILE>) {
    $intext .= $_;
  }

  close(INFILE);

  next if ($intext =~ m/\Wasm\W/s); #asmがあったら何もしない！
  {
    # intext処理部
    $intext =~ s/extern\s+\"C\"[^{]*\{/extern \"C\";/s;   # extern "C" {のカッコが邪魔
    $intext =~ s/\#ifdef\s+__cplusplus\s+\}/\#ifdef __cplusplus\n\/* \} *\//s;
  }

  {
    # open2がおかしいのでベタに書くことにする
    open OUTFILE, ">indent.tmp" or die "cannot open outfile indent.tmp\n";
    print OUTFILE $intext;
    close(OUTFILE);
  }

$tpdefs = "\
-T BOOL -T f64 -T u64 -T s64 -T f32 -T u32 -T s32 -T u16 -T s16 -T u8 -T s8 \
-T VecFx16 -T VecFx32 -T OS_Thread -T OS_Mutex -T OS_MessageQueue -T OS_Context \
-T MtxFx44 -T MtxFx43 -T MtxFx33 -T MtxFx22 -T LockWord -T LockByte -T G3XMiscStatus \
-T G3DLInfo -T G3BoxTestParam -T G2OamAttr -T G2OamAffine -T fxdiv_ -T fx64Tmp_ \
-T G2ENUM_OAM_SHAPE";

system "indent -bap -bbo -bli0 -bls -c60 -cd60 -cli0 -cp8 -nbfda -i4 -l120 -lc120 -lps -nbc -ncs -npsl -nsob -nut -npcs -nprs -saf -sai -saw -ss $tdefs indent.tmp";

open INFILE2, "indent.tmp" or die "cannot open infile indent.tmp\n";

$outtext = "";
while(<INFILE2>) {
  $outtext .= $_;
}

unlink("indent.tmp");
unlink("indent.tmp~");

close(INFILE2);

{
  # outtext処理部
  $outtext =~ s/extern\s+\"C\";/extern \"C\" \{/s;   # extern "C" {のカッコをつける
  $outtext =~ s/\#ifdef\s+__cplusplus\s+\/\* \} \*\//\#ifdef __cplusplus\n\}/s;
}

open OUTFILE2, ">$filename" or die "cannot open outfile $filename\n";
print OUTFILE2 $outtext;
close(OUTFILE2);
}
