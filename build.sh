GCC=x86_64-w64-mingw32-gcc

$GCC -Iinclude -std=gnu99 -c -shared dll2.c
$GCC -shared dll2.o -o dll2.dll

$GCC -Iinclude -std=gnu99 -c -shared dll1.c
$GCC -shared dll1.o -L. -l:dll2.dll -o dll1.dll

$GCC -Iinclude -c app.c
$GCC app.o -L. -l:dll1.dll -o app.exe