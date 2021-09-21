##################################################################
#	fstonecv.rb
#	やぶれたせかい　飛び石データコンバート
##################################################################

$KCODE = "SJIS"

#=================================================================
#	エラー終了
#=================================================================
def	fstonecv_error_end( file0, file1, delfile )
	file0.close
	file1.close
	File.delete( delfile )
end

#=================================================================
#	飛び石　配置数書き込み
#=================================================================
def fstonecv_write_putmax( wfile, count )
	pos = wfile.pos
	wfile.pos = 0
	ary = Array( count )
	wfile.write( ary.pack("i*") )
	wfile.pos = pos
end

#=================================================================
#	飛び石　フラグ操作座標データ数書き込み
#=================================================================
def fstonecv_write_flagposmax( wfile, count )
	pos = wfile.pos
	wfile.pos = 4
	ary = Array( count )
	wfile.write( ary.pack("i*") )
	wfile.pos = pos
end

#=================================================================
#	飛び石　フラグ初期状態書き込み
#=================================================================
def fstonecv_write_flaginit( wfile, flag )
	pos = wfile.pos
	wfile.pos = 8
	ary = Array( flag )
	wfile.write( ary.pack("I*") )
	wfile.pos = pos
end

#=================================================================
#	飛び石　データ領域先頭位置を返す
#=================================================================
def fstonecv_seek_topdata()
	return 12
end

#=================================================================
#	飛び石フラグ初期状態コンバート
#	format 0-3 表示フラグ
#=================================================================
def fstonecv_conv_initflag( rfile, wfile )
	no = 0
	bit = 0
	flag = 0
	ary = Array.new
	
	rfile.gets			#"表示フラグNo"飛ばし
	line = rfile.gets	#表示フラグ番号リスト
	
	line.scan( /\d+/ ){ |match|		#数字抜き出し
		flag = match.to_i
		bit |= 1 << flag
	}
	
	#	ary = Array( bit )
	#	wfile.write( ary.pack("I*") )	#表示ビット u32
	fstonecv_write_flaginit( wfile, bit )
end

#=================================================================
#	飛び石配置　一行コンバート
#	return 1 == ERROR
#=================================================================
def fstonecv_conv_put_line( str, wfile )
	num = str[0].to_i				#参照フラグ番号 u32
	ary = Array( num )
	wfile.write( ary.pack("I*") )
	
	case str[1]						#飛び石種類 u16
	when "STEP"
		num = 0
	when "PUT"
		num = 1
	when "STEP3A"
		num = 2
	when "STEP3B"
		num = 3
	when "STEP3C"
		num = 4
	when "STEP4"
		num = 5
	when "STEP5"
		num = 6
	when "STEP6"
		num = 7
	when "STEP7"
		num = 8
	when "STEP8"
		num = 9
	when "STEP9"
		num = 10
	when "STEP10"
		num = 11
	when "STEP11"
		num = 12
	when "STEP12"
		num = 13
	when "STEP13"
		num = 14
	when "STEP14"
		num = 15
	when "STEP15"
		num = 16
	when "STEP16"
		num = 17
	when "STEP17"
		num = 18
	when "STEP18"
		num = 19
	when "LAND_TREE"
		num = 22
	when "LAND_ROCK"
		num = 23
	else
		printf( "飛び石タイプ異常\n" )
		return 1
	end
	ary = Array( num )
	wfile.write( ary.pack("S*") )
	
	num = str[2].to_i				#飛び位置座標 X s16
	ary = Array( num )
	wfile.write( ary.pack("s*") )
	num = str[3].to_i				#飛び位置座標 Y s16
	ary = Array( num )
	wfile.write( ary.pack("s*") )
	num = str[4].to_i				#飛び位置座標 Z s16
	ary = Array( num )
	wfile.write( ary.pack("s*") )
	
	return 0
end

#=================================================================
#	飛び石配置　コンバート
#	format 0-3 飛び石の数 4.. 飛び石データ群
#	return 1 == ERROR 2 == END
#=================================================================
def fstonecv_conv_put( rfile, wfile )
	ret = 0
	count = 0
	rfile.gets						#項目行を飛ばす
	
	loop{
		line = rfile.gets
		str = line.split( "," )
		
		if( str == @nil || str[0] =~ /#end/ )
			ret = 2
			break
		end
		
		if( str[0] == "" || str[0] == "\r\n" )
			break
		end
		
		ret = fstonecv_conv_put_line( str, wfile )
		
		if( ret == 1 )
			return 1
		end
		
		count = count + 1
	}
	
	fstonecv_write_putmax( wfile, count )
	return ret
end

#=================================================================
#	飛び石フラグ操作座標データ　一行コンバート
#	return 1 == ERROR
#=================================================================
def fstonecv_conv_flagpos_line( str, wfile )
	num = str[0].to_i				#フラグNo u32
	ary = Array( num )
	wfile.write( ary.pack("I*") )
	
	num = 0
	case str[1]						#移動方向 s16
	when "UP"
		num = 0
	when "DOWN"
		num = 1
	when "LEFT"
		num = 2
	when "RIGHT"
		num = 3
	else
		printf( "方向異常\n" )
		return 1
	end
	ary = Array( num )
	wfile.write( ary.pack("s*") )
	
	case str[2]						#ON,OFF s16
	when "OFF"
		num = 0
	when "ON"
		num = 1
	else
		printf( "操作フラグ異常\n" )
		return 1
	end
	ary = Array( num )
	wfile.write( ary.pack("s*") )
	
	num = str[3].to_i				#X座標 s16
	ary = Array( num )
	wfile.write( ary.pack("s*") )
	num = str[4].to_i				#Y座標 s16
		eflag_flagpos = 1
	ary = Array( num )
	wfile.write( ary.pack("s*") )
	num = str[5].to_i				#Z座標 s16
	ary = Array( num )
	wfile.write( ary.pack("s*") )
	num = str[6].to_i				#X範囲 s16
	ary = Array( num )
	wfile.write( ary.pack("s*") )
	num = str[7].to_i				#Y範囲 s16
	ary = Array( num )
	wfile.write( ary.pack("s*") )
	num = str[8].to_i				#Z範囲 s16
	ary = Array( num )
	wfile.write( ary.pack("s*") )
	
	return 0
end

#=================================================================
#	飛び石フラグ操作座標　コンバート
#	format 0-3 データ数 4.. 操作座標データ
#	return 1 == ERROR 2 == end
#=================================================================
def fstonecv_conv_flagpos( rfile, wfile )
	ret = 0
	count = 0
	rfile.gets						#項目行を飛ばす
	
	loop{
		line = rfile.gets
		str = line.split( "," )
		
		if( str == @nil || str[0] =~ /#end/ )
			ret = 2
			break
		end
		
		if( str[0] == "" || str[0] == "\r\n" )
			break
		end
		
		ret = fstonecv_conv_flagpos_line( str, wfile )
		
		if( ret == 1 )
			return 1
		end
		
		count = count + 1
	}
	
	fstonecv_write_flagposmax( wfile, count )
	return ret
end

#=================================================================
#	飛び石データコンバート
#	txtname コンバートする飛び石データテキストファイル
#	戻り値 0=正常終了 1=異常終了
#
#	ファイルフォーマット
#	0-3		飛び石配置数
#	4-7		飛び石操作座標データ数
#	8-11	飛び石フラグ初期状態
#	以下総数分　飛び石データ、操作座標データが続く
#=================================================================
def fstonecv_conv( txtname )
	if( txtname == @nil )
		printf( "ERROR fstonecv.rb \"FileName\"\n" )
		return 1
	end
	
	binname = txtname.sub( /\.txt/, "\.bin" )	#output name.txt -> name.bin
	
	tfile = File.open( txtname, "r" )
	wfile = File.open( binname, "wb" )
	
	printf( "Tw FroatStoneFile Convert %s -> %s\n",
		File.basename(txtname), File.basename(binname) )
	
	eflag_flag = 0
	eflag_put = 0
	eflag_flagpos = 0
	
	fstonecv_write_putmax( wfile, 0 )				#ヘッダ初期化
	fstonecv_write_flagposmax( wfile, 0 )
	fstonecv_write_flaginit( wfile, 0 )
	
	wfile.pos = fstonecv_seek_topdata()				#データ書き込み先頭位置
	
	loop{											#飛び石フラグ初期状態
		line = tfile.gets
		str = line.split( "," )
		
		if( str == @nil || str[0] =~ /#end/ )
			break
		end
		
		if( str[0] == "●飛び石フラグ初期状態" )
			eflag_flag = 1
			fstonecv_conv_initflag( tfile, wfile )
			break
		end
	}
	
	if( eflag_flag == 0 )
		printf( "init flag error\n" )
		fstonecv_error_end( tfile, wfile, binname )
		return 1
	end
	
	tfile.pos = 0									#先頭に戻す

	loop{											#飛び石配置データ
		line = tfile.gets
		str = line.split( "," )
		
		if( str == @nil || str[0] =~ /#end/ )
			break
		end
	
		if( str[0] == "●飛び石配置" )
			eflag_put = 1
			ret = fstonecv_conv_put( tfile, wfile )
			case ret
			when 1									#error
				eflag_put = 0
				break
			when 2
				break
			end
		end
	}
	
	if( eflag_put == 0 )
		printf( "fstone putdata error\n" )
		fstonecv_error_end( tfile, wfile, binname )
		return 1
	end
	
	tfile.pos = 0									#先頭に戻す
	
	loop{											#フラグ操作データ
		line = tfile.gets
		str = line.split( "," )
		
		if( str == @nil || str[0] =~ /#end/ )
			break
		end
		
		if( str[0] == "●飛び石フラグ操作座標データ" )
			eflag_flagpos = 1
			ret = fstonecv_conv_flagpos( tfile, wfile )
			case ret
			when 1
				printf( "fstone flag pos error\n" )
				fstonecv_error_end( tfile, wfile, binname )
				return 1
			when 2
				break
			end
		end
	}
	
	tfile.close
	wfile.close
	return 0
end
