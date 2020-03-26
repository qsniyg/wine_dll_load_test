#include "common.h"
#include <stdio.h>

int initialized = 0;

void dll1_func(void) {
  puts("Running dll1_func");
  dll2_func();
}

int DllMain() {
  // This should print twice, once at the beginning, once at the end
  // Currently wine doesn't run it again
  puts("Ok: Initializing DLL1");
  initialized = 1;
  return 1;
}
