##################################################################
#	fstonecvall.rb
#	やぶれたせかい　飛び石データ
#	飛び石データファイル全てのコンバートと一覧生成
#	fstonecvall.rb 生成する一覧ファイル名
##################################################################

#ライブラリ
require "fstonecv.rb"

#=================================================================
#	飛び石ファイルディレクトリコンバート
#	list 書き込み先ファイル
#	return 1 == ERROR
#=================================================================
def fstonecv_dir( list )
	ret = 0
	count = 1
	convert = "../../../convert/exceltool/excelcommaconv"
	dir = Dir.open( Dir.pwd + "/tw_fstone/" )			# dir path
	winpath = dir.path								
	winpath = winpath.sub( /\/cygdrive\/c\//, "c:/" )	# win path
	
	list.printf( "TW_DATA_FSTONE_LIST=" )				# label
	
	dir.each{ |name|									# xls cv write list
		if( name.include?("\.xls") )
			binname = name.sub( /\.xls/, "\.bin" )
			txtname = name.sub( /\.xls/, "\.txt" )
			list.printf( " \\\n" )
			list.printf( "\t%s", winpath+binname )		# フルパス
			
			system( convert, winpath+name )				# xls -> txt
			
			ret = fstonecv_conv( winpath+txtname )		# バイナリ変換
			if( ret == 1 )
				break
			end
			
			count = count + 1
		end
	}
	
	#ディレクトリを閉じる
	dir.close
	
	#ファイル数出力
	printf( "tw fstonefile count %d\n", count-1 )
	
	return ret
end

#=================================================================
#	飛び石ファイル全てコンバート
#=================================================================
name = ARGV[0]
if( name == @nil )
	printf( "ERROR fstonecvall.rb \"ListFileName\"\n" )
	exit 1
end

list = File.open( name, "w" )
ret = fstonecv_dir( list )
list.close
if( ret == 1 )
	File.delete( name )
end
