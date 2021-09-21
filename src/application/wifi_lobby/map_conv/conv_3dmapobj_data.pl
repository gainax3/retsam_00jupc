##################################################
#		
#	wflby_3dmapobj_data.datから
#	グラフィックデータのテーブルを作成する
#
##################################################

@MODATA_FILE		= undef;		#データファイル
@GRA_NAIX_FILE		= undef;		#グラフィックNAIXファイル
@OUTPUT_BUFF		= undef;		#書き出す数字の配列
$OUTPUT_BUFF_NUM	= 0;			#書き出しバッファ数

$OUTPUT_STRUCT_MEMBER_NUM	= 109;	#書き出す構造体のメンバ数

######読み込みファイル
$INPUT_MODATA		= "wflby_3dmapobj_data.c";
$INPUT_NAIX			= "wifi_lobby.naix";
$OUTPUT_FILE		= "wflby_3dmapobj_data";



#ファイルを開く
&file_open();

#NARC定義部分の配列を作成する
$return = &makeoutput_buff();
if( $return == -1 ){
	exit(1);
}


#順番に書き出す
&output_data();

exit(0);


#ファイルを開く
sub file_open{
	open( FILEIN, $INPUT_MODATA );
	@MODATA_FILE = <FILEIN>;
	close( FILEIN );
	
	open( FILEIN, $INPUT_NAIX );
	@GRA_NAIX_FILE = <FILEIN>;
	close( FILEIN );
}

#OUTPUT_BUFFを作成する
sub makeoutput_buff{

	my( $dat );
	
	$OUTPUT_BUFF_NUM = 0;
	
	foreach $one ( @MODATA_FILE ){
		#スペース、タブ、,\r\nを排除
		$one =~ s/[\s\t,\r\n]//g;

		#NARCが文字内に入っているかチェック
		if( $one =~ /NARC/ ){
			#数字に変換して配列に格納する
			$dat = &naix_idx_get( $one );
			if( $dat >= 0 ){
				$OUTPUT_BUFF[ $OUTPUT_BUFF_NUM ] = $dat;
				$OUTPUT_BUFF_NUM ++;
			}else{
				return -1;	#定義がない
			}
		#数字が文字列内にはいっていないかチェック
		}elsif( $one =~ /^([0-9]+)/ ){
			$OUTPUT_BUFF[ $OUTPUT_BUFF_NUM ] = $1;
			$OUTPUT_BUFF_NUM ++;
		}
	}
}

#NARCの定義の数字を返す
sub naix_idx_get{
	my( $word ) = @_;
	my( $count );

	$count = 0;
	while( $count < @GRA_NAIX_FILE ){

		if( $GRA_NAIX_FILE[$count] =~ /$word[^0-9]*([0-9]*)/ ){
#			print( $word." to ".$1."\n" );
			return $1;
		}
		
		$count ++;
	}

	print( "$wordがありません\n" );
	return -1;
}

#ファイルを書き出す
#１つのデータは６つで出来ているので、それごとに書き出す
sub output_data{

	my($i, $j, $idx, $count, $outfile_no, $outfile_num);

	$count			= 0;
	$outfile_no		= 0;
	$outfile_num	= $OUTPUT_BUFF_NUM / $OUTPUT_STRUCT_MEMBER_NUM;

	for( $i=0; $i<$outfile_num; $i++ ){
		open( FILEOUT, ">".$OUTPUT_FILE."_".$i.".dat" );
		binmode( FILEOUT );
		for( $j=0; $j<$OUTPUT_STRUCT_MEMBER_NUM; $j++ ){

			$idx = ($OUTPUT_STRUCT_MEMBER_NUM*$i)+$j;
			print( FILEOUT pack("I", $OUTPUT_BUFF[$idx]) );
		}
		close( FILEOUT );
	}
	close( FILEOUT );
}
