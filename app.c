#include "common.h"
#include <stdio.h>

#include <stdio.h>
#include "winternl.h"
#include <stddef.h>

#define CONTAINING_RECORD(address, type, field) \
  ((type *)((PCHAR)(address) - offsetof(type, field)))

static PLDR_MODULE get_dll1_ldr(void) {
  PLIST_ENTRY mark, entry;
  PLDR_MODULE mod;

  mark = &NtCurrentTeb()->Peb->LdrData->InMemoryOrderModuleList;
  for (entry = mark->Flink; entry != mark; entry = entry->Flink)
    {
      mod = CONTAINING_RECORD(entry, LDR_MODULE, InMemoryOrderModuleList);

      if (mod->BaseDllName.Buffer[0] == 'd' &&
          mod->BaseDllName.Buffer[1] == 'l' &&
          mod->BaseDllName.Buffer[2] == 'l' &&
          mod->BaseDllName.Buffer[3] == '1') {
        return mod;
      }
    }

  return NULL;
}

void copied_dll2_func(void) {
  puts("Running copied_dll2_func");

  PLDR_MODULE mod = get_dll1_ldr();
  if (!mod) {
    puts("Failed to find dll1.dll");
  } else {
    // Flags should be 0x80000
    // SizeOfImage should be 0
    // BaseAddress should be 0
    // TlsIndex should be 5
    printf("Flags: %p\nSizeOfImage: %p\nBaseAddress: %p\nTlsIndex: %i\nEntryPoint: %p\n", mod->Flags, mod->SizeOfImage, mod->BaseAddress, mod->TlsIndex, mod->EntryPoint);
  }
}


int main() {
  if (!initialized) {
    puts("Error: Not initialized!");
  } else {
    puts("Ok: Initialized");
  }

  dll1_func();
  copied_dll2_func();

  return 0;
}
