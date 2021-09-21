
infile = ARGV[0]
outfile = ARGV[1]

inf = File.open(infile, "r")
outf = File.open(outfile, "w")
puts "\>\>#{outf.path}"
inf.gets	#ˆês“Ç‚İ”ò‚Î‚µ
lists = []
while line = inf.gets
	cl = line.split ","
	if cl[0] == "" then break end
	lists << cl[0].to_i
	lists << cl[6].to_i
end
len = [lists.length / 2]
outf.syswrite len.pack("i")
outf.syswrite lists.pack("S*")
inf.close
outf.close
