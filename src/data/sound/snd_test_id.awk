#=========================================================
#
#	snd_test_msg.awkで生成されるtmp.txtから、
#	msgedit用のメッセージIDをtmp2.txtに出力
#
#=========================================================
BEGIN{
	count = 0
}

{
	#print "msg_snd_name" count
	#count++

	tmp = $0
	#tmp2 = toupper( tmp )
	tmp2 = tolower( tmp )

	print "msg_" tmp2
}


