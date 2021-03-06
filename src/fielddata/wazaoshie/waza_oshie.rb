#=========================================================================================
=begin

	07.12.14 Satoshi Nohara

	技教えデータ

	waza_oshie.xlsから、waza_oshie.datを作成

	ポケモンナンバーの並びに一致しているか？
	フォルムがあるものも追加されているので、別リストにするか、
	フォルムの項目を増やしてもらって、ポケモンナンバーもデータとして、持つようにするか
	しないといけない！

=end
#=========================================================================================


#-----------------------------------------------------------------------------------------
=begin

	変数

=end
#-----------------------------------------------------------------------------------------


#-----------------------------------------------------------------------------------------
=begin

	コメント出力

=end
#-----------------------------------------------------------------------------------------
def comment_write_start( io )
	io.print "//====================================================================\n"
	io.print "//						技教えデータ\n"
	io.print "//\n"
	io.print "// 07.12.14 Satoshi Nohara\n"
	io.print "//\n"
	io.print "// ●waza_oshie.xlsをコンバートして出力しています\n"
	io.print "//\n"
	io.print "//====================================================================\n"
	io.print "\n"
	io.print "const WAZA_OSHIE_DATA waza_oshie_data[] = {\n"
end


#-----------------------------------------------------------------------------------------
=begin

	コメント出力

=end
#-----------------------------------------------------------------------------------------
def comment_write_end( io )
	io.print "};\n"
	io.print "#define WAZA_OSHIE_DATA_MAX\t( NELEMS(hide_item_data) )\n\n\n"
end


#-----------------------------------------------------------------------------------------
=begin

	waza_oshie.dat書き込み

=end
#-----------------------------------------------------------------------------------------
def waza_oshie_dat_write( file, io )

	$i
	$flag
	$num = []

	$flag = 0

	while text = file.gets do						#ファイルを１行ずつ読み込む
		text.chomp!									#改行がある場合のみ削る(破壊的)

		ary = text.split(/\t/)						#自分自身をタブで分解する

		#要素出力テスト
		#print "1:ナンバー:\t", ary[1], "\n"
		#print "2:ポケモン:\t", ary[2], "\n"
		#print "41:ポケモン:\t", ary[41], "\n"

		#クリア
		$num[0] = 0
		$num[1] = 0
		$num[2] = 0
		$num[3] = 0
		$num[4] = 0

		if( ary[2] =~ /フシギダネ/ )
			$flag = 1
		end

		if( $flag == 0 )
			next
		end

		i = 0

		#41/8=5byte必要
		while( i < 39 )
			if( ary[i+3] =~ /●/ )
				$num[i/8] |= ( 1 << (i % 8) )
			end

			i+=1
		end

		#5byte出力
		io.print "\t{ ", $num[0], ", ", $num[1], ", ", $num[2], ", ", $num[3], ", ",  $num[4], " },\t\t\/\/", ary[1], ":", ary[41], "\n"
	end
end


#-----------------------------------------------------------------------------------------
=begin

	メインの流れ

=end
#-----------------------------------------------------------------------------------------
io = open( "waza_oshie.dat", "w" )					#出力ファイル
file = open( ARGV[0] )								#waza_oshie.txt
comment_write_start( io )							#コメント
waza_oshie_dat_write( file, io )					#waza_oshie.datに書き込み
file.close

comment_write_end( io )								#コメント
io.close											#出力ファイル閉じる


