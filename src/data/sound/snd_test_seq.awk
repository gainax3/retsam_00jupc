{
	name = $0 
	sub( "SEQ_", "", name )

	#"SE_"‚É‘Î‰ž
	sub( "_", "-", name )

	#"DP_"‚É‘Î‰ž
	sub( "_", "-", name )

	#"_"‚É‘Î‰ž
	sub( "_", "-", name )
	sub( "_", "-", name )
	sub( "_", "-", name )
	sub( "_", "-", name )

	print name
}
