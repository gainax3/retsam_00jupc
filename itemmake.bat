cd C:\home\platinum\pokemon_pl\convert\message
make

cd C:\home\platinum\pokemon_pl\arc
del pl_item_data.naix
del pl_item_data.narc
REM nnsarc -c -l -n -i pl_item_data itemdata
nnsarc -c -l -n -i pl_item_data itemdata
nnsarc -c -l -n -i item_data itemdata

REM move pl_item_data.naix C:\home\platinum\pokemon_pl\src\itemtool
move item_data.naix C:\home\platinum\pokemon_pl\src\itemtool
move pl_item_data.narc C:\home\platinum\pokemon_pl\src\itemtool\itemdata

cd c:\home\platinum\pokemon_pl
