##################################################################
#	evcvcmactall.rb
#	やぶれたせかい　イベントデータ、
#	カメラアクションデータファイル全てのコンバートと一覧生成
#	evflist.rb 生成する一覧ファイル名
#	外部 "exec" "system"
##################################################################

#ライブラリ
require "evcmactcv.rb"	#データコンバート

#定数

#=================================================================
#	イベントジャンプファイルコンバート
#	list 書き込み先ファイル
#	TW_EVDATALIST_CMACT
#=================================================================
def ev_make_cmactdata( list )
	ret = 0
	count = 1
	convert = "../../../convert/exceltool/excelcommaconv"
	dir = Dir.open( Dir.pwd + "/tw_cmact/" )			# tw_event path
	winpath = dir.path								
	winpath = winpath.sub( /\/cygdrive\/c\//, "c:/" )	# win path
	
	list.printf( "TW_EVDATA_CMACTLIST =" )				# TW_EVDATA_CMACTLIST
	
	dir.each{ |name|									# xls conv write list
		if( name.include?("\.xls") )
			binname = name.sub( /\.xls/, "\.bin" )
			txtname = name.sub( /\.xls/, "\.txt" )
			list.printf( " \\\n" )
			list.printf( "\t%s", winpath+binname )		# フルパス
			
			system( convert, winpath+name )				# xls -> txt
			
			ret = evcmactcv_txtfileconv( winpath+txtname )	# バイナリ変換
			if( ret == 1 )
				break
			end
			
			count = count + 1
		end
	}
	
	#ディレクトリを閉じる
	dir.close
	
	#ファイル数出力
	printf( "tw cmactfile count %d\n", count-1 )
	
	return ret
end

#=================================================================
#	イベントファイルコンバート
#=================================================================
path = ARGV[0]
if( path == @nil )
	printf( "ERROR evcvcmactall.rb \"ListFileName\"\n" )
	exit 1
end

list = File.open( path, "w" )

ret = ev_make_cmactdata( list )

list.close

if( ret == 1 )			#エラー
	File.delete( path )
end
