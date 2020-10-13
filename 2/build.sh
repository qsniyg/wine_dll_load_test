GCC=x86_64-w64-mingw32-gcc

builddll() {
  rm -f dll1.o
  $GCC -Iinclude -c -shared -DDLLID=D$1 -DDLLSTR=\"D$1\" dll1.c
  # these are the flags used by skse64
  #$GCC -Wl,--image-base=0x180000000,--subsystem,windows,--major-os-version=6,--minor-os-version=0,--major-subsystem-version=6,--minor-subsystem-version=0,--nxcompat,--enable-reloc-section,--dynamicbase,--high-entropy-va -shared dll1.o -o dll$1.dll
  # this is what is needed (--dynamicbase)
  $GCC -Wl,--image-base=0x180000000,--dynamicbase -shared dll1.o -o dll$1.dll
  # testing to ensure the other flags don't have an impact (same as 1st command, but without --dynamicbase)
  #$GCC -Wl,--image-base=0x180000000,--subsystem,windows,--major-os-version=6,--minor-os-version=0,--major-subsystem-version=6,--minor-subsystem-version=0,--nxcompat,--enable-reloc-section,--high-entropy-va -shared dll1.o -o dll$1.dll
}

builddll 1
builddll 2
#builddll 3

$GCC -Iinclude -c main.c
$GCC main.o -L. -l:dll1.dll -l:dll2.dll -o main.exe