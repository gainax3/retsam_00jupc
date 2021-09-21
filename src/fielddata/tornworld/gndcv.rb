##################################################################
#	gndcv.rb
#	やぶれたせかい　地形ファイルコンバート
##################################################################

#=================================================================
#	エラー終了
#=================================================================
def	gndcv_error_end( file0, file1, file2, file3, dfile0, dfile1 )
	file0.close
	file1.close
	file2.close
	file3.close
	File.delete( dfile0 )
	File.delete( dfile1 )
end

#=================================================================
#	アトリビュートアーカイブインデックスを検索
#	str	検索するアトリビュートファイル名
#	アトリビュートインデックスファイル
#	return	インデックス番号	-1=エラー
#=================================================================
def gndcv_attrfile_idxget( str, idx )
	enum = str.sub( /\.dat/, "\_dat" )		# *.dat -> *_dat
	
	if( str.include?("\.dat") )
		str = str.sub( /\.dat/, "" )
	end
	
	no = 0
	idx.pos = 0								#先頭行
	
	loop{									#enum行まで進行
		line = idx.gets
		
		if( line == @nil )
			return -1
		end
		
		if( line.include?("enum {") )
			break
		end
	}
	
	count = 0
	
	loop{
		line = idx.gets
		
		if( line == @nil )
			return -1
		end
		
		if( line.include?(str) )
			return count
		end
		
		count = count + 1
	}
	
	return -1
end

#=================================================================
#	一行コンバート
#	line	一行文字
#	wfile	書き込み先ファイル
#	head	地形ヘッダーファイル
#	idx		インデックスファイル
#	heads	ヘッダー文字
#	no		何番目のデータか
#	return	0=正常終了 1=異常終了 2=終端 3=データ無し行である
#=================================================================
def gndcv_conv_line( line, wfile, hfile, ifile, no, heads )
	num = 0
	ary = Array.new
	str = line.split( "," )					#","区切り配列
	
	if( str == @nil || str[0] =~ /#end/ )	# 終了チェック
		return 2
	end
	
	#if( str[1] == @nil )					# 空行チェック
	#return 3
	#end
	
	if( str[1] == @nil )
		return 1
	end
	
	hfile.printf( "\t%s_%s = %d,\n", heads, str[0], no-1 );	#番号をヘッダに
	
	if( str[1] == "NORMAL" )				#通常地形
		num = 4
		ary = Array( num.to_i )
		wfile.write( ary.pack("S*") )
		
		num = 0								#残りをダミーで埋める
		ary = Array( num.to_i )
		wfile.write( ary.pack("s*") )
		
		wfile.write( ary.pack("i*") )
		wfile.write( ary.pack("i*") )
		wfile.write( ary.pack("i*") )
	end
	
	case str[1]								# 地形タイプ s16
	when "GROUND"
		num = 0
	when "SIDE_L"
		num = 1
	when "SIDE_R"
		num = 2
	when "ROOF"
		num = 3
	else
		printf( "着地面異常 No=%d\n", no )
		return 1
	end
	ary = Array( num.to_i )
	wfile.write( ary.pack("s*") )
	
	ret = gndcv_attrfile_idxget( str[10], ifile ) #アトリビュートindex u16
	if( ret == -1 )
		printf( "アトリビュートファイル名異常 No=%d\n", no )
		return 1
	end
	ary = Array( ret )
	wfile.write( ary.pack("S*") )
	
	if( str[2] == @nil )					# X座標　u16
		printf( "X座標異常 No=%d\n", no )
		return 1
	end
	ary = Array( str[2].to_i )
	wfile.write( ary.pack("S*") )
	
	if( str[3] == @nil )					# Y座標　u16
		printf( "Y座標異常 No=%d\n", no )
		return 1
	end
	ary = Array( str[3].to_i )
	wfile.write( ary.pack("S*") )
	
	if( str[4] == @nil )					# Z座標　u16
		printf( "Z座標異常 No=%d\n", no )
		return 1
	end
	ary = Array( str[4].to_i )
	wfile.write( ary.pack("S*") )
	
	//
	if( str[5] == @nil )					# X範囲　u16
		printf( "地形サイズX異常 No=%d\n", no )
		return 1
	end
	ary = Array( str[5].to_i )
	wfile.write( ary.pack("S*") )
	
	if( str[6] == @nil )					# Y範囲　u16
		printf( "地形サイズY異常 No=%d\n", no )
		return 1
	end
	ary = Array( str[6].to_i )
	wfile.write( ary.pack("S*") )
	
	if( str[7] == @nil )					# Z範囲　u16
		printf( "地形サイズZ異常 No=%d\n", no )
		return 1
	end
	ary = Array( str[7].to_i )
	wfile.write( ary.pack("S*") )
	
	//
	if( str[8] == @nil )					# X範囲　u16
		printf( "Xアトリビュート範囲異常 No=%d\n", no )
		return 1
	end
	ary = Array( str[8].to_i )
	wfile.write( ary.pack("S*") )
	
	if( str[9] == @nil )					# Y範囲　u16
		printf( "Yアトリビュート範囲異常 No=%d\n", no )
		return 1
	end
	ary = Array( str[9].to_i )
	wfile.write( ary.pack("S*") )
	
	return 0								# 正常終了
end

#=================================================================
#	evjファイルコンバート
#	txtname		コンバートするジャンプデータテキストファイル
#	idxname		参照するアトリビュートインデックスファイル名
#	戻り値　0=正常終了 1=異常終了
#=================================================================
def gndcv_txtfileconv( txtname, idxname )
	if( txtname == @nil )
		printf( "ERROR gndcv.rb \"filename\"\n" )
		return 1
	end
	
	if( idxname == @nil )
		printf( "ERROR gndcv.rb \"attr_idxname\n" )
		return 1
	end
	
	binname = txtname.sub( /\.txt/, "\.bin" )	#output name.txt -> name.bin
	headname = txtname.sub( /\.txt/, "\.h" )	#output name.txt -> name.h
	
	tfile = File.open( txtname, "r" )
	wfile = File.open( binname, "wb" )
	hfile = File.open( headname, "w" )
	ifile = File.open( idxname, "r" )
	
	printf( "Tw GroundFile Convert %s -> %s\n",
		File.basename(txtname), File.basename(binname) )
	
	heads = txtname.sub( /\.txt/, "" )
	
	#ヘッダー num書き込み
	hfile.printf( "#ifndef %s_H_FILE_\n#define %s_HFILE_\nenum {\n", File.basename(heads), File.basename(heads) )
	
	data_no = 0									#データ番号
	tfile.gets 									#一行目を飛ばす
	ary = Array( data_no.to_i )					#データ総数を予め書き込み
	wfile.write( ary.pack("i*") )
	
	loop{										#コンバート
		line = tfile.gets
		ret = gndcv_conv_line(
			line, wfile, hfile, ifile, data_no+1, File.basename(heads) )
		
		case ret
		when 1		#エラー
			gndcv_error_end( tfile, wfile, hfile, ifile, binname, headname )
			return 1
		when 2		#終端
			break
		when 3		#空行
			next
		end
	
		data_no = data_no + 1
	}
	
	hfile.printf( "};\n#endif" )
	
	wfile.pos = 0								#先頭に戻す
	ary = Array( data_no.to_i )
	wfile.write( ary.pack("i*") )				#データ総数を書き込み
	
	tfile.close
	wfile.close
	hfile.close
	ifile.close
	return 0
end
