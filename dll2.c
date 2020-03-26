#include "common.h"
#include <stdio.h>
#include "winternl.h"
#include <stddef.h>

#define CONTAINING_RECORD(address, type, field) \
  ((type *)((PCHAR)(address) - offsetof(type, field)))

PLDR_MODULE get_dll1_ldr(void) {
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

void dll2_func(void) {
  puts("Running dll2_func");

  PLDR_MODULE mod = get_dll1_ldr();
  if (!mod) {
    puts("Failed to find dll1.dll");
  } else {
    // Flags should be 0x80000
    // SizeOfImage should be 0
    // BaseAddress should be 0
    // TlsIndex should be 5
    printf("Flags: %p\nSizeOfImage: %p\nBaseAddress: %p\nTlsIndex: %i\n", mod->Flags, mod->SizeOfImage, mod->BaseAddress, mod->TlsIndex);
  }
}

int DllMain(void) {
  puts("Running DllMain for DLL2");

  PLDR_MODULE mod = get_dll1_ldr();
  if (!mod) {
    puts("Failed to find dll1.dll");
  } else {
    puts("Found dll1.dll");
    // Neither of these will prevent dll1 from loading
    //mod->BaseDllName.Buffer[0] = 0;
    //mod->FullDllName.Buffer[0] = 0;
    //mod->EntryPoint = 0; // Will prevent dll1 from loading
    //mod->BaseAddress = 0;
    mod->Flags = 0;
    mod->SizeOfImage = 0;
    mod->TlsIndex = 5;
  }

  return 1;
}
