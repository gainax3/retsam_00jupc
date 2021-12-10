#----------------------------------------------------------------
#
# フィールドテクスチャ転送アニメデータコンバート
#
# 2007.04.05
# AkitoMori GameFreak inc.
#
#----------------------------------------------------------------

require "CSV"

# アニメ数最大定義
TEXNAME_LENGTH  = 16	#MAYA内でのテクスチャ指定名
TEXPATH_LENGTH  = 32	#実際のimdのファイル名
ANIME_MAX   	= 18
ANIME_TABLE 	= ANIME_MAX*2


#==================================================
#------------------------------------
# 転送アニメ管理クラス
#==================================================
class TransAnime
	def initialize
		@texname    = "asase"
		@texpath    = "asase.nsbmd"
		@animetable = [20]
	end

	#------------------------------------
	# テクスチャ名セット
	def set_texname( name )
		@texname = name
	end

	#------------------------------------
	# テクスチャパスセット
	def set_texpath( path )
		#拡張子をimdからnsbmdに書き換えて置く
		@texpath = path.sub(/\.imd/,"\.nsbtx")
	end

	#------------------------------------
	# アニメーションテーブルセット
	def set_animetable( idx, no )
		if no==nil then
			#データ無しの場合はFFで埋める
			@animetable[idx] = 0xff
		else
			if (idx%2)==0 then
				#偶数番目はインデックスなのでそのまま
				@animetable[idx] = no.to_i
			else
				#奇数番目はアニメカウンタなので30/1であることを考慮して半分にする
				@animetable[idx] = no.to_i/2
			end
		end
	end

	def get_texpath()
		return @texpath
	end

	#------------------------------------
	# クラス情報表示
	def putinfo
		p @texname 
		p @texpath 
		p @animetable
	end

	#------------------------------------
	# 出力バイナリに変換
	def pack

		#一旦純粋なArrayに格納しなおしてからpack関数を呼び出す
		@tmp = [@texname]
		ANIME_TABLE.times{|n|
			@tmp[1+n] = @animetable[n].to_i
		}

		#出力フォーマットの作成
		@outputstr   = sprintf( "a%dc%d",TEXNAME_LENGTH, ANIME_TABLE )

		#バイナリ出力
		return @tmp.pack( @outputstr )
	end
end

#-----------------------------------------
# メイン処理
#-----------------------------------------

#引数が３つなかったら終了
if ARGV.length<3 then
	p "3 parameters are necessary"
	p "ruby transanime.rb <input>.xls <output1>.bin <output2>.scr"
	exit
end

#グローバルなやつ
ta = []
i  = 0


#---------CSV取り込み-----------
CSV.foreach(ARGV[0]){|line|

	#---------１行目飛ばし-----------
	if line[0]=="テクスチャ名" then
		# p "１行目なので飛ばした"
		next
	end
	
	#---------テクスチャ名の入ってない行は飛ばし-----------
	if line[0]==nil then
		next
	end
	
	#---------取り込み結果をTransAnimeクラスへ-----------
	ta[i] = TransAnime.new
	ta[i].set_texname(line[0])
	ta[i].set_texpath(line[1])

	ANIME_TABLE.times{|n|
		ta[i].set_animetable(n,line[n+2])
	}
	i += 1

}

printf( "テクスチャ指定は%d個\n",i )

#p ta[0].pack
#p ta[1].pack
#p ta[2].pack


#---------アニメデータ出力-----------
File.open(ARGV[1].to_str,"wb"){|file|
	#テクスチャ登録個数を出力
	buf = []
	buf = [i.to_i]
	file.write buf.pack("V")

	#テクスチャアニメデータを出力
	i.times{|n|
		file.write ta[n].pack
	}
}

#---------アーカイブ用リスト出力-----------
File.open(ARGV[2],"wb"){|file|
	# 最初にアニメテーブル
	file.write "\""+ARGV[1]+"\"\n"

	# あとはテクスチャファイルの羅列
	i.times{|n|
		file.write "\""+ta[n].get_texpath()+"\"\n"
	}
}
