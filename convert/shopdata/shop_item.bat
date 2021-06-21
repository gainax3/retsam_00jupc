..\exceltool\exceltabconv shop_item.xls
awk -f shop_item.awk shop_item.txt
copy /Y shop_item.dat ..\..\src\fielddata\shopdata
