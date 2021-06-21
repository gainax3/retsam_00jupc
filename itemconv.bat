REM cd C:\hns\conv\item_dp
cd C:\home\platinum\pl_conv\itemconv

copy C:\home\platinum\pokemon_pl\include\itemtool\itemuse_def.h

del itemequip.h
del itemsym.h
del itemindex.dat
del itemname.gmm
del iteminfo.gmm
del itemeqp.gmm
del itemdata /q
del itemtype.dat

md itemdata

REM itemconv_dp item.csv >itemconv_dp.err
REM itemcnv_gmm item.csv >itemcnv_gmm.err
REM itemconv_dp item.csv >c:\home\platinum\pokemon_pl\itemconv_dp.err
REM itemcnv_gmm item.csv >c:\home\platinum\pokemon_pl\itemcnv_gmm.err
itemconv_dp pl_item.csv >c:\home\platinum\pokemon_pl\itemconv_dp.err
itemcnv_gmm pl_item.csv >c:\home\platinum\pokemon_pl\itemcnv_gmm.err

move itemequip.h C:\home\platinum\pokemon_pl\include\itemtool
move itemsym.h C:\home\platinum\pokemon_pl\include\itemtool
move itemindex.dat C:\home\platinum\pokemon_pl\src\itemtool
move itemname.gmm C:\home\platinum\pokemon_pl\convert\message\src
move iteminfo.gmm C:\home\platinum\pokemon_pl\convert\message\src
move itemeqp.gmm C:\home\platinum\pokemon_pl\convert\message\src
move itemtype.dat C:\home\platinum\pokemon_pl\src\itemtool

del C:\home\platinum\pokemon_pl\arc\itemdata /q
move itemdata\*.dat C:\home\platinum\pokemon_pl\arc\itemdata

cd C:\home\platinum\pokemon_pl

notepad itemconv_dp.err
notepad itemcnv_gmm.err
