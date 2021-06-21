#=============================================================================================
#
#	固定グッズアイテム管理表コンバータ
#
#	06.05.13 Satoshi Nohara
#
#=============================================================================================
BEGIN{

	#=================================================================
	#
	#アイテム定義ファイル読み込み
	#
	#=================================================================
	ITEMSYM_H = "../../src/fielddata/base_goods/goods_id_def.h"

	#getline = １行読み込み
	#１は戻り値
	while ( 1 == getline < ITEMSYM_H ) {
		if ( NF == 5 && $1 =="#define" ) {
			ItemName[$5] = $2
		}
	}
	close(ITEMSYM_H)

	file1 = "fs_goods.dat"
	file2 = "fs_goods_def.h"

	#データファイル
	print "//====================================================================" > file1
	print "//						固定グッズアイテムデータ" >> file1
	print "//" >> file1
	print "// 06.05.13 Satoshi Nohara" >> file1
	print "//" >> file1
	print "// ●fs_goods.xlsをコンバートして出力しています" >> file1
	print "//" >> file1
	print "//====================================================================" >> file1
	print "" >> file1
	print "typedef u16 FS_GOODS_DATA;" >> file1
	print "" >> file1

	#定義ファイル
	print "//====================================================================" > file2
	print "//					固定グッズアイテム呼び出しナンバー" >> file2
	print "//" >> file2
	print "// 06.05.13 Satoshi Nohara" >> file2
	print "//" >> file2
	print "// ●fs_goods.xlsをコンバートして出力しています" >> file2
	print "//" >> file2
	print "//====================================================================" >> file2
	print "#ifndef FS_GOODS_DEF_H" >> file2
	print "#define FS_GOODS_DEF_H" >> file2
	print "" >> file2

	label[256]	#ゾーンID格納
	count = 0	#処理したデータ数
}

NR >= 4{

	#項目数チェック
	if( NF < 3 ) next
	if( $1 == "マップ名" )	next
	if( $1 ~ "アイテム" )	next

	#初期設定
	data = $0
	max = NF
	label[count] = $2
	#print max
	print "//"$1 >> file1
	print "const FS_GOODS_DATA fs_goods_" $2 "[] = {" >> file1

	#いらないデータを削除
	sub( /^[^\t]*/, "", data )		#"\t"と一致するまで削除
	sub( /[\t]/, "", data )			#"\t"と一致したら削除
	sub( /^[^\t]*/, "", data )		#"\t"と一致するまで削除
	sub( /[\t]/, "", data )			#"\t"と一致したら削除

	for( i=2; i < max ;i++ ){

		#次のアイテムデータを抜き出す
		tmp = data 
		sub( /\t.*$/, "", tmp )	#"\t"以降を削除
		print "\t" ItemName[tmp] "," >> file1
		#printf( "%s, ", ItemName[tmp] ) >> file1
		#print tmp
		#print ItemName[tmp]

		#アイテムデータを進める
		sub( /^[^\t]*/, "", data )	#"\t"と一致するまで削除
		sub( /[\t]/, "", data )		#"\t"と一致したら削除
	}
	print "\t0xffff," >> file1
	print "};" >> file1
	tmp = toupper( $2 )
	print "#define FS_GOODS_" tmp "_MAX\t( NELEMS(fs_goods_" $2 ") )\n" >> file1
	print "#define ID_SHOP_GOODS_" tmp "\t(" count ")" >> file2
	count++
}

END{
	#データファイル
	print "//データテーブル" >> file1
	print "const FS_GOODS_DATA* fs_goods_data[] = {" >> file1

	for( i=0; i < count ;i++ ){
		label[i]
		print "\tfs_goods_" label[i] "," >> file1
	}
	print "};" >> file1

	print "#define FS_GOODS_DATA_MAX\t( NELEMS(fs_goods_data) )\n" >> file1

	#定義ファイル
	print "\n#endif /* FS_GOODS_DEF_H */" >> file2
}


