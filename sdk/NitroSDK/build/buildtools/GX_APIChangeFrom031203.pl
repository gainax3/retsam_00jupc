#!/usr/bin/perl

use File::Find;

@dirlist = ('.');

sub replace_content {
  my $file = shift @_;
#  $file =~ s/\/(\w*)/$1/;
  $file =~ s/.*\///g;
    
#  $file =~ s/\/([~/]*)/$1/;
  
  open INFILE, "$file" or die "died $file\n";
  $intext = "";
  while(<INFILE>) {
    $intext .= $_;
  }

  close(INFILE);
  $replaced = 0;

  $replaced |= $intext =~ s/G2_BG0ControlMosaic/G2_BG0Mosaic/sg;
  $replaced |= $intext =~ s/G2_BG1ControlMosaic/G2_BG1Mosaic/sg;
  $replaced |= $intext =~ s/G2_BG2ControlMosaic/G2_BG2Mosaic/sg;
  $replaced |= $intext =~ s/G2_BG3ControlMosaic/G2_BG3Mosaic/sg;

  $replaced |= $intext =~ s/G2_BG0ControlPriority/G2_BG0Priority/sg;
  $replaced |= $intext =~ s/G2_BG1ControlPriority/G2_BG1Priority/sg;
  $replaced |= $intext =~ s/G2_BG2ControlPriority/G2_BG2Priority/sg;
  $replaced |= $intext =~ s/G2_BG3ControlPriority/G2_BG3Priority/sg;

  $replaced |= $intext =~ s/G3X_SetBG0Offset/G3X_SetHOffset/sg;

  $replaced |= $intext =~ s/G2_BG0CNT_PRIORITY_0/0/sg;
  $replaced |= $intext =~ s/G2_BG0CNT_PRIORITY_1/1/sg;
  $replaced |= $intext =~ s/G2_BG0CNT_PRIORITY_2/2/sg;
  $replaced |= $intext =~ s/G2_BG0CNT_PRIORITY_3/3/sg;
  $replaced |= $intext =~ s/G2_BG0CNT_PRIORITY /int /sg;

  $replaced |= $intext =~ s/G2_BG1CNT_PRIORITY_0/0/sg;
  $replaced |= $intext =~ s/G2_BG1CNT_PRIORITY_1/1/sg;
  $replaced |= $intext =~ s/G2_BG1CNT_PRIORITY_2/2/sg;
  $replaced |= $intext =~ s/G2_BG1CNT_PRIORITY_3/3/sg;
  $replaced |= $intext =~ s/G2_BG1CNT_PRIORITY /int /sg;

  $replaced |= $intext =~ s/G2_BG2CNT_PRIORITY_0/0/sg;
  $replaced |= $intext =~ s/G2_BG2CNT_PRIORITY_1/1/sg;
  $replaced |= $intext =~ s/G2_BG2CNT_PRIORITY_2/2/sg;
  $replaced |= $intext =~ s/G2_BG2CNT_PRIORITY_3/3/sg;
  $replaced |= $intext =~ s/G2_BG2CNT_PRIORITY /int /sg;

  $replaced |= $intext =~ s/G2_BG3CNT_PRIORITY_0/0/sg;
  $replaced |= $intext =~ s/G2_BG3CNT_PRIORITY_1/1/sg;
  $replaced |= $intext =~ s/G2_BG3CNT_PRIORITY_2/2/sg;
  $replaced |= $intext =~ s/G2_BG3CNT_PRIORITY_3/3/sg;
  $replaced |= $intext =~ s/G2_BG3CNT_PRIORITY /int /sg;

  $replaced |= $intext =~ s/G2_OAM_PRIORITY_0/0/sg;
  $replaced |= $intext =~ s/G2_OAM_PRIORITY_1/1/sg;
  $replaced |= $intext =~ s/G2_OAM_PRIORITY_2/2/sg;
  $replaced |= $intext =~ s/G2_OAM_PRIORITY_3/3/sg;
  $replaced |= $intext =~ s/G2_OAM_PRIORITY /int /sg;

  if ($replaced == 0) {
#    print STDERR "$file skipped\n";
    next;
  }

  print STDERR "$file replaced\n";
  open OUTFILE, ">$file" or die;
  print OUTFILE $intext;
  close(OUTFILE);

}


sub process_file {
  if ($File::Find::name =~ m/\.h$/ ||
      $File::Find::name =~ m/\.c$/ ||
      $File::Find::name =~ m/\.html$/) {
    replace_content($File::Find::name);
  }
}


find(\&process_file, @dirlist);


