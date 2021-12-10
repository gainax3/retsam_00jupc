##################################################
#		
#	wflby_gdata.txtからグリッドデータのリストを作成する
#
##################################################
@GDATA_FILE = undef;	#ファイルデータ
@MAPDATA_FILE = undef;	#ファイルデータ
@MAPOBJID	= undef;	#マップオブジェID
@EVID		= undef;	#イベントID
%GDATANO	= undef;	#グリッドデータNOのハッシュ
$MAPGRID_X	= undef;	#マップの大きさ
$MAPGRID_Y	= undef;	#マップの大きさ
@MAPGRID_DT = undef;	#マップグリッドデータ

######読み込むファイル
$INPUT_MAPDATA		= "wflby_map.txt";
$INPUT_GDATA		= "wflby_gdata.txt";
$OUTPUT_MAPH		= "wflby_mapdata.h";
$OUTPUT_GDATANO		= "wflby_map.dat";

######予約定義
$OBJID_YOYAKU		= "NONE";
$EVID_YOYAKU		= "NONE";
$OBJID_DEF			= "WFLBY_MAPOBJID_";
$EVID_DEF			= "WFLBY_MAPEVID_";

#ファイルオープン
&file_open();

#データ作成
$return = &dataid_make();
if( $return == -1 ){
	exit(1);	#err
}

#マップデータを作成する
$return = &map_data_make();
if( $return == -1 ){
	exit(1);	#err
}

#全データを書き出す
#ヘッダー
&write_header();
#マップデータ
&write_map();

exit(0);





##################################################
#		
#	関数
#
##################################################

#ファイル読み込み
sub file_open{
	open( FILEIN, $INPUT_MAPDATA );
	@MAPDATA_FILE = <FILEIN>;
	close( FILEIN );
	open( FILEIN, $INPUT_GDATA );
	@GDATA_FILE = <FILEIN>;
	close( FILEIN );
}


#GDATAを上から検索して、MAPOBJID,EVID,GDATANOを生成
sub dataid_make{
	my( $obj_num, $ev_num, $gdatano, $gdata_obj, $gdata_ev, $zyufuku, $hit );

	$MAPOBJID[ 0 ]	= $OBJID_YOYAKU;
	$EVID[ 0 ]		= $EVID_YOYAKU;
	$obj_num = 1;
	$ev_num = 1;
	
	foreach $one ( @GDATA_FILE ){

		$one =~ s/\r\n//g;
			
		if( $one =~ /([A-Z][^,]*),[^,]*,([0-9]),([^,]*)/ ){	#アルファベットのあるぎょうならデータ

			#OBJID
			$zyufuku = &double_check( \@MAPOBJID, $1 );
			if( $zyufuku == -1 ){		
				$MAPOBJID[$obj_num] = $1;
				$gdata_obj = $obj_num;
				$obj_num++;
			}else{
				print( "mapobjid  zyuufuku $1\n" );
				return -1;	#エラー
			}

			#EVID
			if( $3 ne "0" ){
				$zyufuku = &double_check( \@EVID, $3 );
				if( $zyufuku == -1 ){
					$EVID[$ev_num] = $3;
					$gdata_ev = $ev_num;
					$ev_num ++;
				}else{
					$gdata_ev = $zyufuku;
				}
			}else{
				$gdata_ev = 0;	#なんもないとき
			}

			#HIT BIT
			if( $2 eq "1" ){
				$hit = 1;
			}else{
				$hit = 0;
			}

			#GDATANO
			$gdatano = $hit | ($gdata_ev << 1) | ($gdata_obj << 16);
			$GDATANO{ $MAPOBJID[$gdata_obj] } = $gdatano;
		}
	}
}

#重複チェック
sub double_check{
	my( $arry, $check ) = @_;
	my( $count );

	$count  = 0;

	for( $count = 0; $count <(@$arry+0); $count++ ){
		if( $$arry[$count] eq $check ){
			return $count;	#重複
		}
	}
	return -1;			#重複なし
}



#マップデータを作成する
sub map_data_make{
	
	my( @d_line, $line_c, $gdata );
	
	$MAPGRID_Y = 0;
	$MAPGRID_X = 0;


	foreach $one (@MAPDATA_FILE){
		
		if( $one =~ /[A-Z].*/ ){	#アルファベットが入ってればOK
			
			#数字の部分を抜く
			$one =~ s/^[0-9]+,//g;
		
			#最後についてる\nを抜く
			$one =~ s/\r\n//g;
			
			@d_line = split( /,/, $one );
			
			if( $MAPGRID_X == 0 ){
				$MAPGRID_X = @d_line;		#幅を格納
			}else{
				if( $MAPGRID_X != @d_line ){
					print( "mapgrid_siz x diff  line".$MAPGRID_Y."\n" );
					return -1;
				}
			}

			for( $line_c=0; $line_c<$MAPGRID_X; $line_c++ ){
				$gdata = &get_gdata( $d_line[ $line_c ] );
				if( $gdata == -1 ){
					print( "mapobj none ".$d_line[ $line_c ]."\n" );
					return -1;
				}
				$MAPGRID_DT[ ($MAPGRID_Y*$MAPGRID_X)+$line_c ] = $gdata;
			}


			$MAPGRID_Y ++;
		}
	}

	return 0;
}

#GDATANOから指定のGDATAを取得する
sub get_gdata{

	my( $sarch ) = @_;
	my( $key, $gdata, $retgdata );

	$retgdata = -1;
	
	while( ($key, $gdata) = each( %GDATANO ) ){
#		print( "hash ".$key."=$gdata  sarch=$sarch\n" );
		if( $key eq $sarch ){
			$retgdata = $gdata;
		}
	}
	return $retgdata;
}


#ヘッダーを書き出す
sub write_header{

	my( $i );
	
	open( FILEOUT, ">$OUTPUT_MAPH" );
	
	print( FILEOUT "#ifndef __WFLBY_MAP_H__\r\n" );
	print( FILEOUT "#define __WFLBY_MAP_H__\r\n" );

	#マップデータの大きさ
	print( FILEOUT "\r\n\r\n\r\n" );
	print( FILEOUT "// マップの大きさ\r\n" );
	print( FILEOUT "#define WFLBY_MAPSIZE_X		($MAPGRID_X)\r\n" );
	print( FILEOUT "#define WFLBY_MAPSIZE_Y		($MAPGRID_Y)\r\n" );
	
	#オブジェクトID
	print( FILEOUT "\r\n\r\n\r\n" );
	print( FILEOUT "// マップオブジェクトID\r\n" );
	for( $i=0; $i<@MAPOBJID; $i++ ){
		print( FILEOUT "#define $OBJID_DEF".$MAPOBJID[$i]."		($i)\r\n" );
	}

	#イベントID
	print( FILEOUT "\r\n\r\n\r\n" );
	print( FILEOUT "// イベントID\r\n" );
	for( $i=0; $i<@EVID; $i++ ){
		print( FILEOUT "#define $EVID_DEF".$EVID[$i]."		($i)\r\n" );
	}

	print( FILEOUT "\r\n\r\n\r\n" );
	print( FILEOUT "#endif //__WFLBY_MAP_H__\r\n" );
	
	close( FILEOUT );
}

#マップデータを書き出します
sub write_map{

	my( $i );
	
	open( FILEOUT, ">$OUTPUT_GDATANO" );
	binmode( FILEOUT );

	

	for( $i=0; $i<@MAPGRID_DT; $i++ ){
		print( FILEOUT pack("I", $MAPGRID_DT[$i]) );
	}
	
	close( FILEOUT );
}
