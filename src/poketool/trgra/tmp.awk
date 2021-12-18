{
	gsub(/"/,"")
	gsub(/.NCGR/,".ncg")
}
/ncg/{
	#print $1;
	system("perl -e \"print \'$1\'\"")
}
