..\exceltool\exceltabconv fs_goods.xls
awk -f fs_goods.awk fs_goods.txt
copy /Y fs_goods.dat ..\..\src\fielddata\shopdata
copy /Y fs_goods_def.h ..\..\src\fielddata\shopdata
