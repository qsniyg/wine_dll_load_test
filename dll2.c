#include "common.h"
#include <stdio.h>
#include "winternl.h"
#include <stddef.h>

#define CONTAINING_RECORD(address, type, field) \
  ((type *)((PCHAR)(address) - offsetof(type, field)))

void dll2_func(void) {
  puts("Running dll2_func");
}

int DllMain(void) {
  puts("Running DllMain for DLL2");
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
        puts("Found dll1.dll");
        mod->BaseDllName.Buffer[0] = 0;
        mod->FullDllName.Buffer[0] = 0;
        //mod->EntryPoint = 0; // Will prevent dll1 from loading
        mod->Flags = 0;
        mod->SizeOfImage = 0;
      }
    }
  return 1;
}
