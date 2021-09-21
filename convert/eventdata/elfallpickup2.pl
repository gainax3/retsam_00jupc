#!perl
#=========================================================================
#
#	ファイル名
#	pokemon_pl/convert/eventdata/elfallpickup2.pl
#
#	実行
#	pokemon_pl/src/fielddata/eventdata/makefile
#	elfallpickup2.pl flist all_data.elf
#
#	flist
#	zone_dummy zone_ug zone_c01 ... 全てのゾーンが出力されている
#
#	07.03.14 Satoshi Nohara
#	CW2.0で、all_data.elfの出力形式が変更になっていたので対応
#
#=========================================================================

#all_data.elf
$totalname = $ARGV[1];

open(FLIST, $ARGV[0]);
$flist = <FLIST>;
close(FLIST);

#all_data.elfを開く
open(FILE, $totalname);

$size;
$value;
$value2;
@tbl;
$tbl_num = 0;
$time;
@tbl2;
@tbl2_num;

$time = localtime(time);
print "elfallpickup2.pl start ", $time, "\n";

#必要なデータを抜き出してテーブル作成
tbl_make();
tbl_make2();

#FObj,Connect,BGTALK,PosEvに分けてテーブルを作成
tbl_make3();
tbl_make4();

#-------------------------------------------------------------------------
#
#	空白で区切る(zone_dummy zone_c01などを順番に処理)
#
#-------------------------------------------------------------------------
foreach $basename( split(/\s+/, $flist) ){

	#指定ラベルのデータをファイルから抜き出す
    &createbin();

	#zone_c01_total.binファイルを開き、出力
    $outfile = $basename . "_total.bin";
    open(WFILE, "> $outfile");
	binmode(WFILE);

	#文字列を数式として演算
	#print sprintf("pack(\"C*\", %s);", $data);
    print WFILE eval( sprintf("pack(\"C*\", %s);", $data) );
    close(WFILE);
}
close(FILE);

$time = localtime(time);
print "elfallpickup2.pl end   ", $time, "\n";

#-------------------------------------------------------------------------
#
#	指定ラベルのデータをファイルから抜き出す
#
#	BGTALK_zone_c01_numなど
#
#-------------------------------------------------------------------------
sub createbin{

    $data = '';

	#$time = localtime(time);
	#print "1ゾーン開始時間", $time, "\n";

	#evb
	&pickupbin2ex( sprintf("BGTALK_%s_num:", $basename), 2 );
	&pickupbin2ex( sprintf("BGTALK_%s:", $basename), 2 );
	#&pickupbin2( sprintf("BGTALK_%s_num:", $basename) );
	#&pickupbin2( sprintf("BGTALK_%s:", $basename) );

	#evc
    &pickupbin2ex( sprintf("FObj_%s_num:", $basename), 0 );
    &pickupbin2ex( sprintf("FObj_%s:", $basename), 0 );
	#&pickupbin2( sprintf("FObj_%s_num:", $basename) );
	#&pickupbin2( sprintf("FObj_%s:", $basename) );

	#evd
    &pickupbin2ex( sprintf("Connect_%s_num:", $basename), 1 );
    &pickupbin2ex( sprintf("Connect_%s:", $basename), 1 );
	#&pickupbin2( sprintf("Connect_%s_num:", $basename) );
	#&pickupbin2( sprintf("Connect_%s:", $basename) );

	#evp
    &pickupbin2ex( sprintf("PosEv_%s_num:", $basename), 3 );
    &pickupbin2ex( sprintf("PosEv_%s:", $basename), 3 );
	#&pickupbin2( sprintf("PosEv_%s_num:", $basename) );
	#&pickupbin2( sprintf("PosEv_%s:", $basename) );
}

#-------------------------------------------------------------------------
#
#	必要なデータを抜き出してテーブル作成
#
#-------------------------------------------------------------------------
sub tbl_make{

    seek(FILE, 0, 0);

	#FILE = all_data.elf
	while( <FILE> ){

		#データ記述部分を探す
		#if( $_ =~ /DATA \(\.rodata\)/ ){				#test
		if( $_ =~ /INITIALIZED DATA \(\.rodata\)/ ){
			#$mode = 1;
			return;
		}
	}
}
#-------------------------------------------------------------------------
#
#	必要なデータを抜き出してテーブル作成2
#
#-------------------------------------------------------------------------
sub tbl_make2{

	#FILE = all_data.elf
	while( <FILE> ){

		#ラベルでない部分を探す
		if( $_ =~ /'/ ){
			$tbl[ $tbl_num ] = $_;
			$tbl_num+=1;
		}
	}
}

#-------------------------------------------------------------------------
#
#	ファイルから指定のラベルデータを抜き出す
#	※データはグローバル変数$dataへ蓄積される
#
#-------------------------------------------------------------------------
sub pickupbin2{

    my $temp = $_[0];							#BGTALK_zone_c01_num
	my @temp3;
	#print "確認中\t", $temp, "\n";				#BGTALK_zone_c01_num

    seek(FILE, 0, 0);
	$size = 0;

	#コロンなし
	$temp =~ s/\://;

	#FILE = all_data.elf
	while( <FILE> ){

		#アドレスとサイズを取得

		@temp3 = split( /\s+/, $_ );

		if( $temp eq $temp3[7] ){				#test

			$value = $temp3[1];

			#10進数値に変換
			$value2 = hex( $value ) & 0xfffffff0;

			#16進文字列に変換→数値に変換
			$value2 = hex( sprintf("%x",    $value2) );

			#大文字にする
			#$value =~ tr/a-f/A-F/;
			#$value2 =~ tr/a-f/A-F/;

			$size = hex($temp3[2]);

			#print "データのアドレス\t", $value, "\n";			#アドレス
			#print "データのある境界線\t", $value2, "\n";		#アドレス(きりのいい値)
			#print "size\t", $size, "\n";		#サイズ

			search_code();
			return;
		}
    }
}

#-------------------------------------------------------------------------
#
#	データがある位置を探して取得
#
#-------------------------------------------------------------------------
sub search_code{

	my $code;
	my $count = $value2 >> 4;			# / 16

	$code = $tbl[ $count ];
	$count+=1;

	$code =~ ( s/^0x........: // );
	$code =~ s/'.*'//;

	my @temp99 = split( /\s+/, $code );
	my $i = 0;
	my $j = 0;
	my $no = hex( $value ) - $value2;

	#サイズ分取得する
	while( $i < $size ){

		if( ($no + $j) >= 16 ){

			$code = $tbl[ $count ];
			$count+=1;
			@temp99 = split( /\s+/, $code );
			$j = 0;
			$no = 0;
		}

		$data .= "0x";									#書き込み
		$data .= $temp99[ $no + $j + 1 ];				#書き込み
		$data .= ",";									#書き込み

		$i += 1;
		$j += 1;
	}

	return;
}


#=========================================================================
#=========================================================================
#=========================================================================
#=========================================================================

#-------------------------------------------------------------------------
#
#	必要なデータを抜き出してテーブル作成3
#
#-------------------------------------------------------------------------
sub tbl_make3{

    seek(FILE, 0, 0);

	#FILE = all_data.elf
	while( <FILE> ){

		#データ記述部分を探す
		if( $_ =~ /\(\.symtab\)/ ){
			return;
		}
	}
}

#-------------------------------------------------------------------------
#
#	必要なデータを抜き出してテーブル作成4
#
#-------------------------------------------------------------------------
sub tbl_make4{

	my $pos;

	#FILE = all_data.elf
	while( <FILE> ){

		#必要なデータはここまでなので終了
		if( $_ =~ /strtab/ ){
			return;
		}

		#カテゴリーで分けて格納
		if( $_ =~ /FObj/ ){
			$pos = 0;
		}elsif( $_ =~ /Connect/ ){
			$pos = 1;
		}elsif( $_ =~ /BGTALK/ ){
			$pos = 2;
		}else{
			$pos = 3;
		}

		$tbl2[$pos][ $tbl2_num[$pos] ] = $_;
		$tbl2_num[$pos]+=1;
	}
}

#-------------------------------------------------------------------------
#
#	ファイルから指定のラベルデータを抜き出す
#	※データはグローバル変数$dataへ蓄積される
#
#-------------------------------------------------------------------------
sub pickupbin2ex{

	my $i = 0;
    my $temp = $_[0];							#BGTALK_zone_c01_num
    my $num = $_[1];							#FOBJ,CONNECT,BG,POSどれか
	my @temp3;

	$size = 0;

	#コロンなし
	$temp =~ s/\://;

	while( $i < $tbl2_num[ $num ] ){

		#アドレスとサイズを取得

		@temp3 = split( /\s+/, $tbl2[ $num ][ $i ] );
		$i+=1;

		if( $temp eq $temp3[7] ){				#test

			$value = $temp3[1];

			#10進数値に変換
			$value2 = hex( $value ) & 0xfffffff0;

			#16進文字列に変換→数値に変換
			$value2 = hex( sprintf("%x",    $value2) );

			#大文字にする
			#$value =~ tr/a-f/A-F/;
			#$value2 =~ tr/a-f/A-F/;

			$size = hex($temp3[2]);

			#print "データのアドレス\t", $value, "\n";			#アドレス
			#print "データのある境界線\t", $value2, "\n";		#アドレス(きりのいい値)
			#print "size\t", $size, "\n";		#サイズ

			search_code();
			return;
		}
    }
}


