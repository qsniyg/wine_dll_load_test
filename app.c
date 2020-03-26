#include "common.h"
#include <stdio.h>

int main() {
  if (!initialized) {
    puts("Error: Not initialized!");
  } else {
    puts("Ok: Initialized");
  }

  dll1_func();

  return 0;
}
