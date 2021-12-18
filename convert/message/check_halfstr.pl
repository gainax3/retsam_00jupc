
#半角英数字をチェックするシステム
#実行したディレクトリ以下のファイルすべてのチェックする

system ("find src|grep gmm > tmp.dat");
if( open( FILE, "tmp.dat") )
{
	@files = <FILE>;
	close(FILE);
	system "rm tmp.dat";

	foreach $filename (@files)
	{
		$filename =~ s/\r\n//g;

		if( $filename =~ /svn/ ){
			print "skip $filename\n";
		}else{
			print "check $filename\n";

			&check( "$filename" );
			print " ...done\n";
		}
	}
}

exit(0);


#チェック
sub check{
	my( $filename ) = @_;
	my( @filebuff, $onestr, $check_flag, $dummystr );

	#チェックするファイル読み込み
	open( FILEIN, $filename );
	@filebuff = <FILEIN>;
	close( FILEIN );
	
	
	#チェックしやすいように変更
	foreach $onestr (@filebuff){
		
#		print $onestr;

		#\r\nをなくす
		$onestr =~ s/\r\n/\n/g;

		
		#<language>の最後と</language>の前に改行を入れて文字列の部分を切り出す
		$onestr =~ s/\<language name=[^\>]*\>/HALFCHECK_START\n\@RT\@/g;
		$onestr =~ s/\<\/language\>/\n\@RT\@HALFCHECK_END/g;

		#タグの情報部分までを削除する
		$onestr =~ s/\[[^\]]*\]//g;



#		print $onestr;
	}

	#\nで区切る形に再構成
	$dummystr = join( "\@RT\@", @filebuff );
	
	@filebuff = undef;
	@filebuff = split( /\@RT\@/, $dummystr );

	#文字の部分に半角英数字があるかチェック
	$check_flag = 0;
	foreach $onestr (@filebuff){


		if( $onestr =~ /HALFCHECK_END/ ){
			$check_flag = 0;
		}

#		print $check_flag."=".$onestr;

		#チェック
		if( $check_flag == 1 ){
			if( $onestr =~ /[a-z]/ ){
				print "		hit a-z ".$onestr;
			}

			if( $onestr =~ /[A-Z]/ ){
				print "		hit A-Z ".$onestr;
			}

			if( $onestr =~ /[0-9]/ ){
				print "		hit 0-9 ".$onestr;
			}
		}

		if( $onestr =~ /HALFCHECK_START/ ){
			$check_flag = 1;
		}

	}
};

