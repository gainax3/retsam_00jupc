##################################################################
#	gndcvall.rb
#	やぶれたせかい　イベントデータ、
#	カメラアクションデータファイル全てのコンバートと一覧生成
#	evflist.rb 生成する一覧ファイル名
#	外部 "exec" "system"
##################################################################

#ライブラリ
require "gndcv.rb"	#データコンバート

#定数

#=================================================================
#	地形ファイルコンバート
#	list 書き込み先ファイル
#	return 0=正常終了
#=================================================================
def gnd_make_gnddata( list )
	ret = 0
	count = 1
	convert = "../../../convert/exceltool/excelcommaconv"
	idx = "./tw_arc_attr.naix"
	dir = Dir.open( Dir.pwd + "/tw_ground/" )			# tw_ground path
	winpath = dir.path								
	winpath = winpath.sub( /\/cygdrive\/c\//, "c:/" )	# win path
	
	list.printf( "TW_GNDDATA_GNDLIST =" )				# TW_GNDDATA_GNDLIST
	
	dir.each{ |name|									# xls conv write list
		if( name.include?("\.xls") )
			binname = name.sub( /\.xls/, "\.bin" )
			txtname = name.sub( /\.xls/, "\.txt" )
			list.printf( " \\\n" )						# \
			list.printf( "\t%s", winpath+binname )		# フルパス
			
			system( convert, winpath+name )				# xls -> txt
			
			ret = gndcv_txtfileconv( winpath+txtname, idx )	# バイナリ変換
			if( ret == 1 )
				break
			end
			
			count = count + 1
		end
	}
	
	#ディレクトリを閉じる
	dir.close
	
	#ファイル数出力
	printf( "tw gndfile count %d\n", count-1 )
	
	return ret
end

#=================================================================
#	イベントファイルコンバート
#=================================================================
path = ARGV[0]
if( path == @nil )
	printf( "ERROR gndcvall.rb \"ListFileName\"\n" )
	exit 1
end

list = File.open( path, "w" )

ret = gnd_make_gnddata( list )

list.close

if( ret == 1 )			#エラー
	File.delete( path )
end
