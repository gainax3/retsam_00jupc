PL_ROOT		= ../..
ARCDIR		= $(PL_ROOT)/src/arc

EXCELTOOL	= ../exceltool/ExcelSeetConv.exe
#SRC		= tvdata060530.xls
SRC			= tvdata080514.xls
POKEITEMSRC	= pokeitemTV060624b.xls

TARGET		= tv.narc
LISTDATA	= list.csv
LISTBIN		= $(LISTDATA:.csv=.bin)
CONTDATA	= contents.csv
CONBIN		= $(CONTDATA:.csv=.bin)
POKEITEMDATA	= pokeitem.csv
POKEITEMBIN	= $(POKEITEMDATA:.csv=.bin)


PARTS		= $(LISTBIN) $(CONBIN) $(POKEITEMBIN)

build:	$(TARGET)

clean:
	-rm $(TARGET) $(PARTS)

copy:
	mv $(TARGET) $(ARCDIR)
	mv $(TARGET:.narc=.naix) $(ARCDIR)

all:
	make clean
	make build
	make copy

$(TARGET):	$(PARTS)
	nnsarc -i -c -l -n $(TARGET) $(PARTS) > arc_result.txt


$(CONTDATA):	$(SRC)
	$(EXCELTOOL) -o $(CONTDATA) -p 1 -s csv $(SRC)

$(CONBIN):	$(CONTDATA)
	ruby contents.rb $(CONTDATA) $(CONBIN)


$(LISTDATA):	$(SRC)
	$(EXCELTOOL) -o $(LISTDATA) -p 2 -s csv $(SRC)

$(LISTBIN):	$(LISTDATA)
	ruby list.rb $(LISTDATA) $(LISTBIN)

$(POKEITEMDATA):	$(POKEITEMSRC)
	$(EXCELTOOL) -o $(POKEITEMDATA) -p 1 -s csv $(POKEITEMSRC)

$(POKEITEMBIN):	$(POKEITEMDATA)
	ruby pokeitem.rb $(POKEITEMDATA) $(POKEITEMBIN)
