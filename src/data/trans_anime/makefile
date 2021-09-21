#----------------------------------------------------------------------------
#
#	フィールド転送アニメテクスチャ用Makefile
#
#	2007.04.17
#	AkitoMori GAME FREAK inc.
#----------------------------------------------------------------------------

EXCELLCONV 	   = ../../../convert/exceltool/ExcelSeetConv.exe
LIST_FILE  	   = trans_anime.xls
OUTPUT_ARC_NAME    = fldtanime.narc
TRANS_ANIME_DATA   = tanime.bin
ARC		   = nnsarc

#同じフォルダ内のimdは全てコンバート対象に
#ただし、コンバートしてもxlsファイルにリストで対象に入っていないと
#アーカイブには含まれません。
SRC      	   = $(wildcard *.imd)
TEXTURES 	   = $(SRC:.imd=.nsbtx)
LIST_TXT   	   = $(LIST_FILE:.xls=.txt)
OUTPUT_SCRIPT_NAME = $(OUTPUT_ARC_NAME:.narc=.scr)
OUTPUT_NAIX_NAME   = $(OUTPUT_ARC_NAME:.narc=.naix)


# --------依存する拡張子------------
.SUFFIXES: .narc .nsbtx .imd .xls


# --------最終ファイル------------
build: $(OUTPUT_ARC_NAME)

# --------エクセルファイル→csvファイル------------
$(LIST_TXT): $(LIST_FILE)
	$(EXCELLCONV)  -p 1 -s csv $(LIST_FILE)

# --------imd→nsbmdルールの記述------------
.imd.nsbtx :
	g3dcvtr $< -o $@ -etex

# --------最終ファイル（アーカイブデータ）の生成------------
$(OUTPUT_ARC_NAME): $(LIST_TXT) $(TEXTURES)
	ruby transanime.rb $(LIST_TXT) $(TRANS_ANIME_DATA) $(OUTPUT_SCRIPT_NAME)
	$(ARC) -c -i -n $(OUTPUT_ARC_NAME) -S $(OUTPUT_SCRIPT_NAME)
	cp $(OUTPUT_ARC_NAME) ../
	cp $(OUTPUT_NAIX_NAME) ../../field

# --------Make All------------
all:
	$(MAKE) clean
	$(MAKE)

# --------途中データ全削除------------
clean:
	-rm *.nsbtx
	-rm *.txt
	-rm *.bin
	-rm *.narc
	-rm *.naix

