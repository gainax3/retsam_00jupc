#!/usr/bin/perl

#引数が１つ必要
if( @ARGV != 1 ){
	print( "pl_naix_change.pl need one param\n" );
	exit 1;
}

#ファイルオープン
if( open( FILEIN, $ARGV[0] ) == 0 ){
	print( "can't open ".$ARGV[0]."\n" );
	exit 1;
}

#データコピー
@filedata = <FILEIN>;
close( FILEIN );

#出力オープン
if( open( FILEOUT, ">".$ARGV[0] ) == 0 ){
	print( "can't open ".$ARGV[0]."\n" );
	exit 1;
}

foreach $data ( @filedata ){
	#置換して出力	
	$data =~ s/pl_//;
	print( $data );
	print( FILEOUT $data );
};

close( FILEOUT );

exit 0;
