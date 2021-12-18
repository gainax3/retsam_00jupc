#!/usr/bin/perl
#---------------------------------------------------------------------------
#  Project:  NitroSDK - tools
#  File:     pdic2c.pl
#
#  Copyright 2004-2006 Nintendo.  All rights reserved.
#
#  These coded instructions, statements, and computer programs contain
#  proprietary information of Nintendo of America Inc. and/or Nintendo
#  Company Ltd., and are protected by Federal copyright law.  They may
#  not be disclosed to third parties or copied or duplicated in any form,
#  in whole or in part, without the prior written consent of Nintendo.
#---------------------------------------------------------------------------

use strict;
use vars qw($VERSION);
use vars qw($HEADER_FILE_NAME $PREFIX);

$VERSION = do { my @r = q$Revision: 1.10 $ =~ /\d+/g; sprintf '%d.'.('%02d'x$#r), @r };

use Fcntl qw(:DEFAULT);
use Getopt::Std;
$Getopt::Std::STANDARD_HELP_VERSION = 1;
use File::Basename;

sub HELP_MESSAGE
{
    my $handler = shift || \*STDOUT;
    print $handler "Usage: pdic2c.pl [-ch] [-C OUTPUT_SOURCE] [-H OUTPUT_HEADER] [-i INCLUDE_FILE] [-p PREFIX] NORMALIZE_SIZE PATTERNS.TXT\n";
}

sub VERSION_MESSAGE
{
    my $handler = shift || \*STDOUT;
    print $handler "pdic2c.pl $VERSION\n";
}

my %opts;
getopts('chC:H:i:p:', \%opts);

if (!$opts{c} && !$opts{h})
{
    $opts{c} = 1;
    $opts{h} = 1;
}

if ($opts{i})
{
    $HEADER_FILE_NAME = $opts{i};
}
elsif ($opts{H})
{
    $HEADER_FILE_NAME = basename($opts{H});
}
else
{
    $HEADER_FILE_NAME = "patternDict.h";
}

if ($opts{p})
{
    $PREFIX = $opts{p};
}
else
{
    $PREFIX = "";
}

if ( $#ARGV < 0 )
{
    HELP_MESSAGE();
    exit 1;
}

my $outputNormalizeSize = shift @ARGV;
if ( $outputNormalizeSize == 0 )
{
    print STDERR "error: normalizeSize must be > 0\n";
    exit 1;
}

binmode STDIN;

my %nameMap = ();
my @nameArray = ();
my @patterns = ();
my $wholePointNum = 0;
my $wholeStrokeNum = 0;

while (<>)
{
    s/\x0D?\x0A?$//; # chomp
    my ($data, $comment) = /^([^#]*?)\s*(?:#\s*(.*))?$/;
    my ($name, $kind, $correction, $normalizeSize, $pointsString) = $data =~ /^"([^"]+)" (\d+) (-?\d+) (\d+) (.*)|\s*$/; #"
    my $index = $wholePointNum;
    my $pointCount = 0;
    $pointsString =~ s/^|//;
    if ( not exists $nameMap{$name} )
    {
        push @nameArray, $name;
        $nameMap{$name} = $#nameArray;
    }
    my @strokesString = split /\|/, $pointsString;
    my @strokes = ();
    foreach my $strokeString ( @strokesString )
    {
        my @points = map {/\(\s*(\d+)\s*,\s*(\d+)\s*\)/; [$1, $2]} ($strokeString =~ /\(\s*\d+\s*,\s*\d+\s*\)/g);
        next if (!@points);
        push @strokes, \@points;
#print "$points[0]->[0] $points[0]->[1]\n";
        $pointCount += $#points + 1;
        $pointCount ++ ; # PenUpMarker
    }
    push @patterns,
    {
        name => $name,
        code => $nameMap{$name},
        correction => $correction,
        kind => $kind,
        normalizeSize => $normalizeSize,
        index => $index,
        pointCount => $pointCount,
        strokes => \@strokes,
        comment => $comment,
    };
    $wholePointNum += $pointCount;
    $wholeStrokeNum += $#strokes + 1;
}

my $header_text = '';
my $source_text = '';

$header_text .= <<"EOT";
/*---------------------------------------------------------------------------*
  Header File
 *---------------------------------------------------------------------------*/

#ifndef ${PREFIX}PATTERN_DICT_H_
#define ${PREFIX}PATTERN_DICT_H_

#ifdef __cplusplus
extern "C" {
#endif

EOT

my $patternNum = $#patterns + 1;
my $codeNum    = $#nameArray + 1;

$header_text .= <<"EOT";
#include <nitro/prc/types.h>


#define ${PREFIX}PDIC_WHOLE_POINT_NUM             $wholePointNum
#define ${PREFIX}PDIC_WHOLE_STROKE_NUM            $wholeStrokeNum
#define ${PREFIX}PDIC_PATTERN_NUM                 $patternNum
#define ${PREFIX}PDIC_CODE_NUM                    $codeNum
#define ${PREFIX}PDIC_NORMALIZE_SIZE              $outputNormalizeSize


extern PRCPrototypeList ${PREFIX}PrototypeList;
extern char *${PREFIX}PatternName[PDIC_CODE_NUM];


#ifdef __cplusplus
} /* extern "C" */
#endif

/* ${PREFIX}PATTERN_DICT_H_ */
#endif

/*---------------------------------------------------------------------------*
  End of File
 *---------------------------------------------------------------------------*/
EOT

$source_text .= <<"EOT";
/*---------------------------------------------------------------------------*
  Source File
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include <nitro/prc/types.h>

#include "$HEADER_FILE_NAME"

PRCPoint ${PREFIX}PrototypeListPointArray[${PREFIX}PDIC_WHOLE_POINT_NUM] =
{
EOT

foreach my $pattern (@patterns)
{
    my $strokes = $pattern->{strokes};
    my $normalizeSize = $pattern->{normalizeSize};
    foreach my $points (@$strokes)
    {
        foreach my $point (@$points)
        {
            my ($x, $y);
            $x = int($point->[0] * $outputNormalizeSize / $normalizeSize);
            $y = int($point->[1] * $outputNormalizeSize / $normalizeSize);
            $source_text .= <<"EOT";
    {$x, $y},
EOT
        }
        $source_text .= <<"EOT";
    {-1, -1},
EOT
    }
}

$source_text .= <<"EOT";
};

PRCPrototypeEntry ${PREFIX}PrototypeListEntries[${PREFIX}PDIC_PATTERN_NUM] =
{
EOT

foreach my $pattern (@patterns)
{
    my $strokeCount = $#{$pattern->{strokes}}+1;
    $source_text .= <<"EOT";
    {
        TRUE, $pattern->{kind}, $pattern->{code}, $pattern->{correction}, NULL,        // $pattern->{comment}
        $pattern->{index}, $pattern->{pointCount}, $strokeCount
    },
EOT
}

$source_text .= <<"EOT";
};

PRCPrototypeList ${PREFIX}PrototypeList =
{
    ${PREFIX}PrototypeListEntries,              /* entries */
    ${PREFIX}PDIC_PATTERN_NUM,                  /* entrySize */
    ${PREFIX}PrototypeListPointArray,           /* pointArray */
    ${PREFIX}PDIC_WHOLE_POINT_NUM,              /* pointArraySize */

    ${PREFIX}PDIC_NORMALIZE_SIZE,               /* normalizeSize */
};

char *${PREFIX}PatternName[${PREFIX}PDIC_CODE_NUM] =
{
EOT

foreach my $name (@nameArray)
{
    $source_text .= <<"EOT";
    "$name",
EOT
}

$source_text .= <<'EOT';
};

/*---------------------------------------------------------------------------*
  End of File
 *---------------------------------------------------------------------------*/
EOT

if ($opts{h})
{
    output_source($header_text, $opts{H});
}
if ($opts{c})
{
    output_source($source_text, $opts{C});
}

exit 0;

sub output_source
{
    my $text = shift;
    my $file = shift;
    my $handle;
    if ($file)
    {
        sysopen OFH, $file, O_CREAT|O_TRUNC|O_WRONLY or die "error: can't open $file.\n";
        $handle = \*OFH;
    }
    else
    {
        $handle = \*STDOUT;
    }
    binmode $handle;
    $text =~ s/\x0D\x0A|\x0D|\x0A/\x0D\x0A/g;
    print $handle $text;
    if ($file)
    {
        close $handle;
    }
}

#---------------------------------------------------------------------------
#  $Log: pdic2c.pl,v $
#  Revision 1.10  2007/11/19 05:12:04  seiki_masashi
#  コメントが必須になっていたのを修正
#
#  Revision 1.9  2006/01/18 09:50:56  okubata_ryoma
#  copyrightの修正
#
#  Revision 1.8  2006/01/18 09:09:38  seiki_masashi
#  perl の起動オプションの -W を除去
#
#  Revision 1.7  2005/10/04 04:34:37  seiki_masashi
#  プレフィックス指定の追加
#
#  Revision 1.6  2005/10/04 03:56:06  seiki_masashi
#  include するファイル名を指定できるように変更
#
#  Revision 1.5  2005/10/04 02:54:51  seiki_masashi
#  .c と .h を個別に出力できるように変更
#
#
#---------------------------------------------------------------------------
