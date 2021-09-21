..\exceltool\exceltabconv fs_item.xls
awk -f fs_item.awk fs_item.txt
copy /Y fs_item.dat ..\..\src\fielddata\shopdata
copy /Y fs_item_def.h ..\..\src\fielddata\shopdata
