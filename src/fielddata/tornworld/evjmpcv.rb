##################################################################
#	evjmpcv.rb
#	やぶれたせかい　イベントジャンプデータコンバート
##################################################################

#=================================================================
#	エラー終了
#=================================================================
def	evjmpcv_error_end( file0, file1, file )
	file0.close
	file1.close
	File.delete( file )
end

#=================================================================
#	地名を検索
#	str	検索するアトリビュートファイル名
#	idx	アトリビュートインデックスファイル名
#	return	インデックス番号	-1=エラー
#=================================================================
#def evjmpcv_attrfile_idxget( str, idx )
#end

#=================================================================
#	一行コンバート
#	line	一行文字
#	wfile	書き込み先ファイルp
#	no		何番目のデータか
#	return	0=正常終了 1=異常終了 2=終端 3=データ無し行である
#=================================================================
def evjmpcv_conv_line( line, wfile, no )
	num = 0
	ary = Array.new
	str = line.split( "," )					#","区切り配列
	
	if( str == @nil || str[0] =~ /#end/ )	# 終了チェック
		return 2
	end
	
	#if( str[1] == @nil )					# 空行チェック
	#return 3
	#end
	
	case str[1]								# イベントタイプ u16
	when "WALLJUMP"
		num = 0
	else
		printf( "イベントタイプ異常 No=%d\n", no )
		return 1
	end
	ary = Array( num.to_i )
	wfile.write( ary.pack("S*") )
	
	case str[2] 							# キー入力方向　s16
	when "UP"
		num = 0
	when "DOWN"
		num = 1
	when "LEFT"
		num = 2
	when "RIGHT"
		num = 3
	else
		printf( "入力キー異常 No=%d\n", no )
		return 1
	end
	ary = Array( num.to_i )
	wfile.write( ary.pack("s*") )
	
	if( str[3] == @nil )					# パラメタ　s32
		printf( "パラメタ異常 No=%d\n", no )
	end
	ary = Array( str[3].to_i )
	wfile.write( ary.pack("i*") )
	
	if( str[4] == @nil )					# X座標　s16
		printf( "X座標異常 No=%d\n", no )
		return 1
	end
	ary = Array( str[4].to_i )
	wfile.write( ary.pack("s*") )
	
	if( str[5] == @nil )					# Y座標　s16
		printf( "Y座標異常 No=%d\n", no )
		return 1
	end
	ary = Array( str[5].to_i )
	wfile.write( ary.pack("s*") )
	
	if( str[6] == @nil )					# Z座標　s16
		printf( "Z座標異常 No=%d\n", no )
		return 1
	end
	ary = Array( str[6].to_i )
	wfile.write( ary.pack("s*") )
	
	if( str[7] == @nil )					# X範囲　s16
		printf( "X範囲異常 No=%d\n", no )
		return 1
	end
	ary = Array( str[7].to_i )
	wfile.write( ary.pack("s*") )
	
	if( str[8] == @nil )					# Y範囲　s16
		printf( "Y範囲異常 No=%d\n", no )
		return 1
	end
	ary = Array( str[8].to_i )
	wfile.write( ary.pack("s*") )
	
	if( str[9] == @nil )					# Z範囲　s16
		printf( "Z範囲異常 No=%d\n", no )
		return 1
	end
	ary = Array( str[9].to_i )
	wfile.write( ary.pack("s*") )
	
	if( str[10] == @nil )					# X移動量　s16
		printf( "X移動量異常 No=%d\n", no )
		return 1
	end
	ary = Array( str[10].to_i )
	wfile.write( ary.pack("s*") )
	
	if( str[11] == @nil )					# Y移動量　s16
		printf( "Y移動量異常 No=%d\n", no )
		return 1
	end
	ary = Array( str[11].to_i )
	wfile.write( ary.pack("s*") )
	
	if( str[12] == @nil )					# Z移動量　s16
		printf( "Z移動量異常 No=%d\n", no )
		return 1
	end
	ary = Array( str[12].to_i )
	wfile.write( ary.pack("s*") )
	
	if( str[13] == @nil )					# 回転量 s16
		printf( "回転量異常 No=%d\n", no )
		return 1
	end
	ary = Array( str[13].to_i )
	wfile.write( ary.pack("s*") )
	
	if( str[14] == @nil )					# フレーム s16
		printf( "回転量異常 No=%d\n", no )
		return 1
	end
	ary = Array( str[14].to_i )
	wfile.write( ary.pack("s*") )
	
	case str[15]							#ベクトルオフセット u16
	when "x"
	when "X"
		num = 0
	when "y"
	when "Y"
		num = 1
	when "z"
	when "Z"
		num = 2
	else
		printf( "ベクトルオフセット異常 No=%d\n", no )
		return 1
	end
	ary = Array( num.to_i )
	wfile.write( ary.pack("S*") )
	
	case str[16]							# オフセット反転 u16
	when "ON"
		num = 1
	when "OFF"
		num = 0
	else
		printf( "オフセット反転異常 No=%d\n", no )
		return 1
	end
	ary = Array( num.to_i )
	wfile.write( ary.pack("S*") )
	
	case str[17]							# 終了方向 s16
	when "UP"
		num = 0
	when "DOWN"
		num = 1
	when "LEFT"
		num = 2
	when "RIGHT"
		num = 3
	else
		printf( "終了方向異常 No=%d\n", no )
		return 1
	end
	ary = Array( num.to_i )
	wfile.write( ary.pack("s*") )
	
	case str[18]							# 着地面 s16
	when "GROUND"
		num = 0
	when "SIDE_L"
		num = 1
	when "SIDE_R"
		num = 2
	when "ROOF"
		num = 3
	when "NORMAL"
		num = 4
	else
		printf( "着地面異常 No=%d\n", no )
		return 1
	end
	ary = Array( num.to_i )
	wfile.write( ary.pack("s*") )
	
	if( str[19] == @nil )					# 着地番号 u16
		printf( "着地番号異常 No=%d\n", no )
		return 1
	end
	ary = Array( str[19].to_i )
	ary[0] = ary[0] - 1;					#1org -> 0org
	wfile.write( ary.pack("S*") )
	
	return 0								# 正常終了
end

#=================================================================
#	evjファイルコンバート
#	txtname コンバートするジャンプデータテキストファイル
#	戻り値　0=正常終了 1=異常終了
#=================================================================
def evjmpcv_txtfileconv( txtname )
	#txtname = ARGV[0] 								#filepath

	if( txtname == @nil )
		printf( "ERROR evjmpcv.rb \"filename\"\n" )
		return 1
	end
	
	#filepath = Dir.pwd							#現在のディレクトリ
	
	binname = txtname
	binname = txtname.sub( /\.txt/, "\.bin" )	#output name.txt -> name.bin
	
	tfile = File.open( txtname, "r" )
	wfile = File.open( binname, "wb" )

	printf( "Tw EventJumpFile Convert %s -> %s\n",
		File.basename(txtname), File.basename(binname) )
	
	data_no = 0									#データ番号
	tfile.gets 									#一行目を飛ばす
	ary = Array( data_no.to_i )					#データ総数を予め書き込み
	wfile.write( ary.pack("i*") )
	
	loop{										#コンバート
		line = tfile.gets
		ret = evjmpcv_conv_line( line, wfile, data_no+1 )
		
		case ret
		when 1		#エラー
			evjmpcv_error_end( tfile, wfile, binname )
			return 1
		when 2		#終端
			break
		when 3		#空行
			next
		end
	
		data_no = data_no + 1
	}
	
	wfile.pos = 0								#先頭に戻す
	ary = Array( data_no.to_i )
	wfile.write( ary.pack("i*") )				#データ総数を書き込み
	
	tfile.close
	wfile.close
	return 0
end
