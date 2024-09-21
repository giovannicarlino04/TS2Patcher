#include "patches.h"

/*

{PATCH TEMPLATE}:

bool TestPatch(HANDLE hProcess, uintptr_t moduleBaseAddress) {
    const char* newBytes1 = "\x00\x00"; //Patched bytes

    LPVOID address1 = nullptr;
    SIZE_T numberOfBytesWritten;
    DWORD oldProtect;
    DWORD newProtect = PAGE_EXECUTE_READWRITE;

    if (moduleBaseAddress != 0) {
        address1 = (LPVOID)(moduleBaseAddress + 0x1193B52);  //Address of the patch
    }

    ////////////////// PATCHES GO HERE ///////////////

    if (address1 == nullptr) {
        return false;
    }

    if (!VirtualProtect(address1, strlen(newBytes1), newProtect, &oldProtect)) {
        return false;
    }

    if (!WriteProcessMemory(hProcess, address1, newBytes1, strlen(newBytes1), &numberOfBytesWritten)) {
        return false;
    }

    if (!VirtualProtect(address1, strlen(newBytes1), oldProtect, &newProtect)) {
        return false;
    }
    return true;
}

*/