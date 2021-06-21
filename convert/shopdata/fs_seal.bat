..\exceltool\exceltabconv fs_seal.xls
awk -f fs_seal.awk fs_seal.txt
copy /Y fs_seal.dat ..\..\src\fielddata\shopdata
copy /Y fs_seal_def.h ..\..\src\fielddata\shopdata
