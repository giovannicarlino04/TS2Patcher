#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#ifndef PATCHES_H
#define PATCHES_H

bool CPKPatches(HANDLE hProcess, uintptr_t moduleBaseAddress);
bool ConfigDatPatch(HANDLE hProcess, uintptr_t moduleBaseAddress);
bool CMSPatches(HANDLE hProcess, uintptr_t moduleBaseAddress);
bool VersionStringPatch(HANDLE hProcess, uintptr_t moduleBaseAddress);
bool BacBcmPatch(HANDLE hProcess, uintptr_t moduleBaseAddress);
bool InfiniteTimerPatch(HANDLE hProcess, uintptr_t moduleBaseAddress);

#endif