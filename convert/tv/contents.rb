infile = ARGV[0]
outfile = ARGV[1]

inf = File.open(infile, "r")
outf = File.open(outfile, "w")
puts "\>\>#{outf.path}"
while line = inf.gets
	cl = line.split ","
	if cl[2] == "" then break end
	if cl[1].to_i == 0 then next end
	#puts "#{cl}"

	#[スタート(5から), 何個処理するか(9個処理する)]		DP
	#newcl = cl[5, 9].map{|str| str.to_i}

	#★[スタート(5から), 何個処理するか(10個処理する)]	PL
	#★[スタート(5から), 何個処理するか(12個処理する)]	PL
	#newcl = cl[5, 10].map{|str| str.to_i}
	newcl = cl[5, 12].map{|str| str.to_i}

	#puts "#{newcl}"
	#pstr = newcl.pack("c*")
	outf.syswrite newcl.pack("c*")
end
inf.close
outf.close
