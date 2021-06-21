#=============================================================================================
#
#	固定シールアイテム管理表コンバータ
#
#	06.05.29 Satoshi Nohara
#
#=============================================================================================
BEGIN{

	#=================================================================
	#
	#アイテム定義ファイル読み込み
	#
	#=================================================================
	SEAL_ID_H = "../../include/application/seal_id.h"

	#getline = １行読み込み
	#１は戻り値
	while ( 1 == getline < SEAL_ID_H ) {
		if ( NF == 5 && $1 =="#define" ) {
			ItemName[$5] = $2
		}
	}
	close(SEAL_ID_H)

	file1 = "fs_seal.dat"
	file2 = "fs_seal_def.h"

	#データファイル
	print "//====================================================================" > file1
	print "//						固定シールアイテムデータ" >> file1
	print "//" >> file1
	print "// 06.05.29 Satoshi Nohara" >> file1
	print "//" >> file1
	print "// ●fs_seal.xlsをコンバートして出力しています" >> file1
	print "//" >> file1
	print "//====================================================================" >> file1
	print "" >> file1
	print "typedef u16 FS_SEAL_DATA;" >> file1
	print "" >> file1

	#定義ファイル
	print "//====================================================================" > file2
	print "//					固定シールアイテム呼び出しナンバー" >> file2
	print "//" >> file2
	print "// 06.05.29 Satoshi Nohara" >> file2
	print "//" >> file2
	print "// ●fs_seal.xlsをコンバートして出力しています" >> file2
	print "//" >> file2
	print "//====================================================================" >> file2
	print "#ifndef FS_SEAL_DEF_H" >> file2
	print "#define FS_SEAL_DEF_H" >> file2
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
	print "const FS_SEAL_DATA fs_seal_" $2 "[] = {" >> file1

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

		#テスト
		#print tmp
		#print ItemName[tmp]

		#アイテムデータを進める
		sub( /^[^\t]*/, "", data )	#"\t"と一致するまで削除
		sub( /[\t]/, "", data )		#"\t"と一致したら削除
	}
	print "\t0xffff," >> file1
	print "};" >> file1
	tmp = toupper( $2 )
	print "#define FS_SEAL_" tmp "_MAX\t( NELEMS(fs_seal_" $2 ") )\n" >> file1
	print "#define ID_SHOP_SEAL_" tmp "\t(" count ")" >> file2
	count++
}

END{
	#データファイル
	print "//データテーブル" >> file1
	print "const FS_SEAL_DATA* fs_seal_data[] = {" >> file1

	for( i=0; i < count ;i++ ){
		label[i]
		print "\tfs_seal_" label[i] "," >> file1
	}
	print "};" >> file1

	print "#define FS_SEAL_DATA_MAX\t( NELEMS(fs_seal_data) )\n" >> file1

	#定義ファイル
	print "\n#endif /* FS_SEAL_DEF_H */" >> file2
}


