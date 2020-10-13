#include <stdio.h>

void DLLID() {}

int DllMain() {
  printf(DLLSTR ": %p\n", DLLID);
  return 1;
}
