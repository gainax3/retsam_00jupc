#=============================================================================================
#
#	変動ショップアイテム管理表コンバータ
#
#	06.03.06 Satoshi Nohara
#
#=============================================================================================
BEGIN{

	#=================================================================
	#
	#アイテム定義ファイル読み込み
	#
	#=================================================================
	ITEMSYM_H = "../../include/itemtool/itemsym.h"

	#getline = １行読み込み
	#１は戻り値
	while ( 1 == getline < ITEMSYM_H ) {
		if ( NF == 7 && $1 =="#define" ) {
			ItemName[$7] = $2
		}
	}
	close(ITEMSYM_H)

	file1 = "shop_item.dat"

	print "//====================================================================" > file1
	print "//						変動ショップアイテムデータ" >> file1
	print "//" >> file1
	print "// 06.03.06 Satoshi Nohara" >> file1
	print "//" >> file1
	print "// ●shop_item.xlsをコンバートして出力しています" >> file1
	print "//" >> file1
	print "//====================================================================" >> file1
	print "" >> file1

	print "const SHOP_ITEM_DATA shop_item_data[] = {" >> file1
	print "\t//アイテムナンバー\t\t段階" >> file1
}

NR >= 4{

	#項目数チェック
	if( NF < 2 ) next
	if( $1 == "マップ名" )	next
	if( $1 ~ "アイテム" )	next

	print "\t{ " ItemName[$1] ",\t\t" $2 " }," >> file1
}

END{
	print "};" >> file1
	print "#define SHOP_ITEM_DATA_MAX\t( NELEMS(shop_item_data) )" >> file1
}




