#セルアクターヘッダーバイナリコンバータ
#
#引数１
#	コンバート対象ファイルパス
#		（セルアクターヘッダー）
#引数２
#	書き出しファイルパス
#
#引数３、４、、、、、
#	対応させるenumが記述されているヘッダーへのパス
#
#引数が２つ必要
if( 2 > @ARGV ){

	print( "err---引数が足りません 必要２個  現在".@ARGV."個\n" );
	print( "引数１　コンバート対象ファイルパス\n" );
	print( "引数２　書き出しファイルパス\n" );
	print( "引数３、４、、、、　対応させるヘッダーファイルパス\n" );
	exit(1);
}


open( FILEIN, $ARGV[0] );
@in_file = <FILEIN>;
close( FILEIN );

#最終行にeが6つあるかチェック
$check_listnum = @in_file;
$check_laststr = $in_file[ $check_listnum-1 ];
$check_laststr =~ s/[\t\s]//g;		#余分なスペースをはき
if( !($check_laststr =~ s/e,e,e,e,e,e,e,e,/e,e,e,e,e,e,e,e,/) ){
	printf( "最終行にe,e,e,e,e,e,e,e,がありません。" );
	exit(1);
}



#引数３以降分のヘッダーデータとファイルを対応させる
for( $i = 2; $i < @ARGV; $i++ ){
	if( $i == 2 ){
		$check = system( "c:\/cygwin\/bin\/perl enum_to_num.pl $ARGV[0] $ARGV[1] $ARGV[$i]" );
	}else{
		$check = system( "c:\/cygwin\/bin\/perl enum_to_num.pl $ARGV[1] $ARGV[1] $ARGV[$i]" );
	}

	if( $check ){
		print( "enum_to_num.pl err\n" );
		exit(1);
	}
}

#バイナリ化
$check = system( "c:\/cygwin\/bin\/perl binary_conv.pl I $ARGV[1] $ARGV[1]" );
if( $check ){
	print( "binary_conv.pl err\n" );
	exit(1);
}

exit(0);
