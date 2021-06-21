##################################################################
#	evjmpcv.rb
#	やぶれたせかい　イベントカメラアクションデータコンバート
##################################################################
	
#=================================================================
#	エラー終了
#=================================================================
def	evcmactcv_error_end( file0, file1, file )
	file0.close
	file1.close
	File.delete( file )
end

#=================================================================
#	一行コンバート
#	line	一行文字
#	wfile	書き込み先ファイルp
#	no		何番目のデータか
#	return	0=正常終了 1=異常終了 2=終端 3=データ無し行である
#=================================================================
def evcmactcv_conv_line( line, wfile, no )
	num = 0
	ary = Array.new
	str = line.split( "," )					#","区切り配列
	
	if( str == @nil || str[0] =~ /#end/ )	#終端チェック
		return 2
	end
	
	#if( str[1] == @nil )					# 空行チェック
	#return 3
	#end
	
	if( str[2] == @nil )					# X座標　s16
		printf( "X座標異常 No=%d\n", no )
		return 1
	end
	ary = Array( str[2].to_i )
	wfile.write( ary.pack("s*") )
	
	if( str[3] == @nil )					# Y座標　s16
		printf( "Y座標異常 No=%d\n", no )
		return 1
	end
	ary = Array( str[3].to_i )
	wfile.write( ary.pack("s*") )
	
	if( str[4] == @nil )					# Z座標　s16
		printf( "Z座標異常 No=%d\n", no )
		return 1
	end
	ary = Array( str[4].to_i )
	wfile.write( ary.pack("s*") )
	
	if( str[5] == @nil )					# X範囲　s16
		printf( "X範囲異常 No=%d\n", no )
		return 1
	end
	ary = Array( str[5].to_i )
	wfile.write( ary.pack("s*") )
	
	if( str[6] == @nil )					# Y範囲　s16
		printf( "Y範囲異常 No=%d\n", no )
		return 1
	end
	ary = Array( str[6].to_i )
	wfile.write( ary.pack("s*") )
	
	if( str[7] == @nil )					# Z範囲　s16
		printf( "Z範囲異常 No=%d\n", no )
		return 1
	end
	ary = Array( str[7].to_i )
	wfile.write( ary.pack("s*") )
	
	if( str[8] == @nil )					# カメラX　u16
		printf( "カメラX異常 No=%d\n", no )
		return 1
	end
	ary = Array( str[8].to_i )
	wfile.write( ary.pack("S*") )
	
	if( str[9] == @nil )					# カメラY　u16
		printf( "カメラY異常 No=%d\n", no )
		return 1
	end
	ary = Array( str[9].to_i )
	wfile.write( ary.pack("S*") )
	
	if( str[10] == @nil )					# カメラZ　u16
		printf( "カメラZ異常 No=%d\n", no )
		return 1
	end
	ary = Array( str[10].to_i )
	wfile.write( ary.pack("S*") )
	
	case str[1]								# 方向 s16
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
	wfile.write( ary.pack("S*") )
	
	if( str[11] == @nil )					# フレーム s32
		printf( "フレーム数異常 No=%d\n", no )
		return 1
	end
	ary = Array( str[11].to_i )
	wfile.write( ary.pack("i*") )
	
	return 0								# 正常終了
end

#=================================================================
#	cmactファイルコンバート
#	txtname コンバートするカメラアクションデータテキストファイル
#	戻り値　0=正常終了 1=異常終了
#=================================================================
def evcmactcv_txtfileconv( txtname )
	if( txtname == @nil )
		printf( "ERROR evcmactcv.rb \"filename\"\n" )
		return 1
	end
	
	#filepath = Dir.pwd							#現在のディレクトリ
	
	binname = txtname
	binname = txtname.sub( /\.txt/, "\.bin" )	#output name.txt -> name.bin
	
	tfile = File.open( txtname, "r" )
	wfile = File.open( binname, "wb" )

	printf( "Tw EventCameraActionFile Convert %s -> %s\n",
		File.basename(txtname), File.basename(binname) )
	
	data_no = 0									#データ番号
	tfile.gets 									#一行目を飛ばす
	ary = Array( data_no.to_i )					#データ総数を予め書き込み
	wfile.write( ary.pack("i*") )
	
	loop{										#コンバート
		line = tfile.gets
		ret = evcmactcv_conv_line( line, wfile, data_no+1 )
		
		case ret
		when 1		#エラー
			evcmactcv_error_end( tfile, wfile, binname )
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
