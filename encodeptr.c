#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <winternl.h>

typedef uint64_t ptr_t;

static NTSTATUS  (WINAPI *pNtQueryInformationProcess)(HANDLE, PROCESSINFOCLASS, PVOID, ULONG, PULONG);

#define _ROT(bits)\
static inline uint##bits##_t rotr##bits (uint##bits##_t n, unsigned int c)\
{\
  const uint##bits##_t mask = (CHAR_BIT*sizeof(n) - 1);\
\
  c &= mask;\
  return (n>>c) | (n<<( (-c)&mask ));\
}

_ROT(32)
_ROT(64)

int get_rotamt(ULONG n, int bits) {
  return n & (bits-1);
}

int main() {
  void* ptr1 = 0xdeadbeef;
  void* ptr3 = 0x1234abcd;
  void* ptr2 = 0xdeadbeef1234abcd;
  void* usd = 0x7ffe0000;

  ULONG cookie = *(ULONG*)(usd + 0x330);
  ULONG cookie_pad = *(ULONG*)(usd + 0x334);

  HANDLE hntdll = LoadLibraryA("ntdll.dll");
  pNtQueryInformationProcess = (void*)GetProcAddress(hntdll, "NtQueryInformationProcess");

  ULONG process_cookie = 0xdeadbeef;
  pNtQueryInformationProcess(GetCurrentProcess(), ProcessCookie, &process_cookie, sizeof(process_cookie), NULL);
  uint64_t process_cookie1 = process_cookie;

  printf("process cookie: %p\n", process_cookie);

  printf("EP1: %p\n", EncodePointer(ptr1));
  printf("ESP1: %p\n", EncodeSystemPointer(ptr1));
  printf("EP2: %p\n", EncodePointer(ptr2));
  printf("ESP2: %p\n", EncodeSystemPointer(ptr2));
  printf("EP3: %p\n", EncodePointer(ptr3));
  printf("ESP3: %p\n", EncodeSystemPointer(ptr3));

  printf("\n\n");

  for (int i = 0; i < 10; i++) {
    //printf("PTR: %p\n", ptr1+i);
    //printf("EP : %p\n", EncodePointer(ptr1+i));
    printf("ESP: %p %p\n", EncodeSystemPointer(ptr1+i), ((ptr_t)(ptr1+i))^(cookie));
  }

  printf("\n\n");

  ptr_t base = 0x0123abcd;

  for (int i = 0; i < 15; i++) {
    //printf("PTR: %p\n", ptr1+i);
    //printf("EP : %p\n", EncodePointer(ptr1+i));
    ptr_t newptr = base + (i<<28);
int rotamt = get_rotamt(cookie, 64);
    ptr_t rot = rotr64(newptr, rotamt);
    ptr_t newxor = rot^(uint64_t)EncodeSystemPointer((void*)newptr);
    ptr_t rotcookie = rotr64(cookie, rotamt);
// cookie: 0x9b3f659a: 6 (32-0x9a%32 = 6)
// newxor: 0xcfd966a6
// xor222: 0x54e6033c
// cookie: 0xcdf45a48: 24 (-8, 32-0x48%32 = 24)
    printf("ESP: %p %p %p %p %p %p %p %p\n", EncodeSystemPointer((void*)newptr), newptr^cookie, rot, rot^cookie, newxor, newxor^cookie, rotcookie, rot^rotcookie);
  }

  printf("\n\n");

  for (int i = 0; i < 15; i++) {
    //printf("PTR: %p\n", ptr1+i);
    //printf("EP : %p\n", EncodePointer(ptr1+i));
    ptr_t newptr = base + (i<<28);
int rotamt = get_rotamt(process_cookie1, 64);
    ptr_t rot = rotr64(newptr, rotamt);
    ptr_t newxor = rot^(uint64_t)EncodePointer((void*)newptr);
    ptr_t rotcookie = rotr64(process_cookie1, rotamt);
// cookie: 0x9b3f659a: 6 (32-0x9a%32 = 6)
// newxor: 0xcfd966a6
// xor222: 0x54e6033c
// cookie: 0xcdf45a48: 24 (-8, 32-0x48%32 = 24)
    printf("EP: %p %p %p %p %p %p %p %p\n", EncodePointer((void*)newptr), newptr^process_cookie1, rot, rot^process_cookie1, newxor, newxor^process_cookie1, rotcookie, rot^rotcookie);
  }

  printf("Cookie: %p\n", cookie);
  printf("Cookie1: %p\n", cookie_pad);

  return 0;
}
