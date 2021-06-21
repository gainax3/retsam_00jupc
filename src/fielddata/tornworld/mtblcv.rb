##################################################################
#	mtblcv.rb
#	やぶれたせかい　マップデータコンバート
#	登録されたマップデータを連結し一つのバイナリデータに変換
#	maptblcv.rb "マップテーブルファイル名"
#	外部 "exec" "system"
##################################################################

#ライブラリ
require "tempfile.rb"

#定数
$KCODE = "SJIS"

#=================================================================
#	ゾーンID検索
#	str	検索するゾーンID文字列
#	hfile	ゾーンIDヘッダーファイル
#	return	ゾーンIDの数値 -1=エラー
#=================================================================
def mtbl_zoneid_get( str, hfile )
	ary = Array.new
	hfile.pos = 0							#先頭行に
	
	loop{
		line = hfile.gets
		
		if( line == @nil )
			return -1
		end
		
		
		if( line.include?(str) )
			zone = line
			zone = zone.scan( /\(\d+\)/ )
			zone_id = zone[0].delete( "\(\)" )
			
			if( zone_id == @nil )
				printf( "不正なゾーンID id=%s\n", str )
				return -1
			end
			
			return zone_id
		end
	}
	
	return -1
end

#=================================================================
#	ゾーンID->データインデックス一覧ファイル、
#	ゾーン配置位置テーブルを作成
#
#	データインデックス一覧ファイル構造
#	先頭4バイト　インデックス総数
#	以下総数分[zone(u32)],[arcive index(u16)][x(s16)][y(s16][z(s16)]と続く
#	0-3 [インデックス総数]
#	4--[ZONE ID][ARC IDX][x][y][z]...
#=================================================================
def mtbl_posfile_create( wfile, tfile, hfile )
	tfile.pos = 0
	wfile.pos = 0
	
	count = 0
	ary = Array( count.to_i )
	wfile.write( ary.pack("i*") )
	
	tfile.gets 									#一行目を飛ばす
	
	loop{
		line = tfile.gets
		
		if( line == @nil )
			return 1
		end
		
		str = line.split( "," )
		
		if( str == @nil || str[0] =~ /#end/ )
			break
		end
		
		zone = mtbl_zoneid_get( str[0], hfile )
		
		if( zone == -1 )
			return 1
		end
		
		zone_num = Array( zone.to_i )			#zone id u32
		cnt = Array( count.to_i )				#arcive index u16
		wfile.write( zone_num.pack("I*") )
		wfile.write( cnt.pack("S*") )
		count = count + 1
		
		if( str[1] == "" || str[1] == @nil )	#pos x s16
			return 1
		end
		x = Array( str[1].to_i )
		wfile.write( x.pack("s*") )
		
		if( str[2] == "" || str[2] == @nil )	#pos y s16
			return 1
		end
		y = Array( str[2].to_i )
		wfile.write( y.pack("s*") )
		
		if( str[3] == "" || str[3] == @nil )	#pos z s16
			return 1
		end
		z = Array( str[3].to_i )
		wfile.write( z.pack("s*") )
	}
	
	wfile.pos = 0
	ary = Array( count.to_i )
	wfile.write( ary.pack("i*") )
	return 0
end

#=================================================================
#	デバッグ参照用ゾーン配置位置テーブルの作成
#	#define DEBUG_DATA_FIELDTORNWORLDZONEBLOCKMAX ([総数])
#	static const int DEBUG_DATA_FieldTornWorldZoneBlockPosTable[][4] =
#	{
#		{ zone id, x, y, z },
#		{ zone id, x, y, z },
#		{ zone id, x, y, z },
#		....
#		{ ZONE_ID_MAX, 0, 0, 0},	//終端
#	},
#=================================================================
def mtbl_debugfile_create( tfile, hfile, filename )
	save_pos = tfile.pos
	tfile.pos = 0
	tfile.gets 									#一行目を飛ばす
	
	d_file = File.open( filename, "w" )
	count = 0
	
	loop{
		line = tfile.gets
		str = line.split( "," )
		
		if( str == @nil || str[0] =~ /#end/ )
			break
		end
		
		count = count + 1
	}
	
	d_file.printf( "#ifndef DEBUG_DATA_FIELD_TORNWORLD_ZONE_BLOCK_MAX\n" )
	d_file.printf( "/*破れた世界 デバッグ参照用ゾーンブロック配置座標*/\n" );
	d_file.printf( "/*{ [ZoneID],[X],[Y],[Z] },*/\n" );
	
	d_file.printf(
		"#define DEBUG_DATA_FIELD_TORNWORLD_ZONE_BLOCK_MAX (%s)\n", count );
	d_file.printf(
		"static const int DEBUG_DATA_FieldtornWorldZoneBlockPosTable" );
	d_file.printf(
		"[DEBUG_DATA_FIELD_TORNWORLD_ZONE_BLOCK_MAX][4] =\n{\n" );
	
	tfile.pos = 0
	tfile.gets 									#一行目を飛ばす
	
	loop{
		line = tfile.gets
		str = line.split( "," )
		
		if( str == @nil || str[0] =~ /#end/ )
			break
		end
		
		zone = mtbl_zoneid_get( str[0], hfile )
		x = str[1]
		y = str[2]
		z = str[3]
		d_file.printf( "{ %d, %s, %s, %s },\n", zone, x, y, z );
	}
	
	d_file.printf( "};\n" )
	d_file.printf( "#endif\n" );
	d_file.close
end

#=================================================================
#	他ソース参照用ゾーン配置位置テーブルの作成
#	#define DATA_FIELDTORNWORLDZONEBLOCKMAX ([総数])
#	static const int DATA_FieldTornWorldZoneBlockPosTable[][4] =
#	{
#		{ zone id, x, y, z },
#		{ zone id, x, y, z },
#		{ zone id, x, y, z },
#		....
#		{ ZONE_ID_MAX, 0, 0, 0},	//終端
#	},
#=================================================================
def mtbl_etcsrc_readfile_create( tfile, hfile, filename )
	save_pos = tfile.pos
	tfile.pos = 0
	tfile.gets 									#一行目を飛ばす
	
	d_file = File.open( filename, "w" )
	count = 0
	
	loop{
		line = tfile.gets
		str = line.split( "," )
		
		if( str == @nil || str[0] =~ /#end/ )
			break
		end
		
		count = count + 1
	}
	
	d_file.printf( "/*破れた世界 ゾーン別ブロック配置先頭座標*/\n" );
	d_file.printf( "/*{ [ZoneID],[X],[Y],[Z] },*/\n" );
	
	d_file.printf(
		"#define DATA_FIELD_TORNWORLD_ZONE_BLOCK_MAX (%s)\n", count );
	d_file.printf(
		"static const int DATA_FieldTornWorldZoneBlockPosTable" );
	d_file.printf(
		"[DATA_FIELD_TORNWORLD_ZONE_BLOCK_MAX][4] =\n{\n" );
	
	tfile.pos = 0
	tfile.gets 									#一行目を飛ばす
	
	loop{
		line = tfile.gets
		str = line.split( "," )
		
		if( str == @nil || str[0] =~ /#end/ )
			break
		end
		
		zone = mtbl_zoneid_get( str[0], hfile )
		x = str[1]
		y = str[2]
		z = str[3]
		d_file.printf( "{ %d, %s, %s, %s },\n", zone, x, y, z );
	}
	
	d_file.printf( "};\n" )
	d_file.close
end

#=================================================================
#	バイナリファイル作成
#	no			書き込み番号
#	wpath		書き込むファイル名
#	gndname		地形ファイル読込先
#	evjmppath	ジャンプファイル読込先
#	cmapath		カメラアクションファイル読込先
#	fstonepath	飛び石ファイル読込先
#	return		0=正常終了 1=異常終了
#	
#	バイナリ構造
#	0-3		バイナリ番号
#	4-7		地形データサイズ(gnd)
#	8-11	ジャンプイベントデータサイズ(evj)
#	12-15	カメラアクションデータサイズ(cma)
#	16-19	飛び石データサイズ(fstone)
#	以下各サイズ分の地形データ、ジャンプイベントデータ、カメラアクションデータ、飛び石データと続く
#
#=================================================================
def mtbl_binfile_create( no, wpath, gndpath, ejmppath, cmapath, fstonepath )
	wfile = File.open( wpath, "wb" )
	
	if( gndpath != "" )
		gndpath = gndpath.sub( /\.xls/, ".bin" )
		gndfile = File.open( "./tw_ground/" + gndpath, "rb" )
	else
		gndpath = @nil
	end
	
	if( ejmppath != "" )
		ejmppath = ejmppath.sub( /\.xls/, ".bin" )
		ejmpfile = File.open( "./tw_evjump/" + ejmppath, "rb" )
	else
		ejmppath = @nil
	end
	
	if( cmapath != "" )
		cmapath = cmapath.sub( /\.xls/, ".bin" )
		cmafile = File.open( "./tw_cmact/" + cmapath, "rb" )
	else
		cmapath = @nil
	end
	
	if( fstonepath != "" )
		fstonepath = fstonepath.sub( /\.xls/, ".bin" )
		fstonefile = File.open( "./tw_fstone/" + fstonepath, "rb" )
	else
		fstonepath = @nil
	end
	
	
	num = Array( no.to_i )
	wfile.write( num.pack("i*") )				#番号書き出し
	
	size = 0									#gnd size
	if( gndpath != @nil )
		size = gndfile.stat.size
	end
	ary = Array( size.to_i )
	wfile.write( ary.pack("i*") )				#evjデータ位置となる
	
	size = 0
	if( ejmppath != @nil )						#evj size
		size = ejmpfile.stat.size
	end
	ary = Array( size.to_i )
	wfile.write( ary.pack("i*") )				#cmaデータ位置
	
	size = 0
	if( cmapath != @nil )						#cma size
		size = cmafile.stat.size
	end
	ary = Array( size.to_i )
	wfile.write( ary.pack("i*") )				#evj size
	
	size = 0
	if( fstonepath != @nil )					#fstone size
		size = fstonefile.stat.size
	end
	ary = Array( size.to_i )
	wfile.write( ary.pack("i*") )				#end
	
	if( gndpath != @nil )
		wfile.write( gndfile.read )
		gndfile.close
	end
	if( ejmppath != @nil )
		wfile.write( ejmpfile.read )
		ejmpfile.close
	end
	if( cmapath != @nil ) 
		wfile.write( cmafile.read )
		cmafile.close
	end
	if( fstonepath != @nil )
		wfile.write( fstonefile.read )
		fstonefile.close
	end
	
	wfile.close
	return 0
end

#=================================================================
#	マップテーブルコンバート
#	argv[0] マップテーブル.xls
#	argv[1] データリスト書き込みファイル名
#	argv[2] ゾーンIDヘッダーファイル名 
#=================================================================
convert = "../../../convert/exceltool/excelcommaconv"

xlsname = ARGV[0]
if( xlsname == @nil )
	printf( "ERROR mtblcv.rb \"MapTableFileName\"\n" )
	exit 1
end

listname = ARGV[1]
if( listname == @nil )
	printf( "ERROR mtblcv.rb \"ListFileName\"\n" )
	exit 1
end
listfile = File.open( listname, "w" )

zonename = ARGV[2]
if( zonename == @nil )
	printf( "ERROR mtblcv.rb \"ZoneHeaderFileName\"\n" )
	exit 1
end
zonefile = File.open( zonename, "r" )

xtxtname = xlsname.sub( /\.xls/, "\.txt" )		#map xls -> txt
system( convert, xlsname )
xtxtfile = File.open( xtxtname, "r" )

#MAPTABLE_LIST書き込み開始
listfile.printf( "MAPTABLE_LIST =" )

#テーブルインデックスファイル作成
idxname = File.basename( xlsname )
idxname = idxname.sub( /\.xls/, "_idx" )
printf( "mtblcv create index file %s\n", idxname )
idxfile = File.open( idxname, "wb" )

listfile.printf( " \\\n" )
listfile.printf( "\t%s", idxname )

ret = mtbl_posfile_create( idxfile, xtxtfile, zonefile )

if( ret == 1 )
	printf( "マップテーブルインデックス生成エラー\n" )
	listfile.close
	zonefile.close
	xtxtfile.close
	idxfile.close
	
	File.delete( xtxtname )
	File.delete( idxname )
	exit 1
end

#一行コンバート
count = 0
xtxtfile.pos = 0
xtxtfile.gets		#一行目を飛ばす

loop{
	line = xtxtfile.gets
	if( line == @nil )
		break
	end
	
	str = line.split( ',' )
	
	if( str == @nil || str[0] =~ /#end/ )
		break
	end
	
	mbinname = "map_" + str[0] + ".bin"
	
	ret = mtbl_binfile_create(
		count, mbinname, str[4], str[5], str[6], str[7] )
	
	if( ret == 1 )
		break
	end
	
	listfile.printf( " \\\n" )
	listfile.printf( "\t%s", mbinname )
}

mtbl_debugfile_create( xtxtfile, zonefile, "debug_tornworld.dat" )
mtbl_etcsrc_readfile_create( xtxtfile, zonefile, "field_tornworld_pos.dat" )

listfile.close
zonefile.close
xtxtfile.close
idxfile.close

if( ret == 1 )
	File.delete( xtxtname )
	File.delete( idxfile )
end
