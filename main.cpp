#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <MinHook.h>
#include <vector>
#include <string>
#include <cstdint>
#include <iostream>
#include "Mutex.h"
#include "ts2patcher.h"
#include "common.h"

static HMODULE patched_dll;
static Mutex mutex;
HMODULE myself;
std::string myself_path;

extern "C" PUBLIC DWORD Direct3DCreate9()
{
}

extern "C" PUBLIC DWORD Direct3DShaderValidatorCreate9()
{
    // Stub, no return
}

extern "C" PUBLIC DWORD PSGPError()
{
    // Stub, no return
}

extern "C" PUBLIC DWORD PSGPSampleTexture()
{
    // Stub, no return
}

extern "C" PUBLIC DWORD D3DPERF_BeginEvent()
{
}

extern "C" PUBLIC DWORD D3DPERF_EndEvent()
{
}

extern "C" PUBLIC DWORD D3DPERF_GetStatus()
{
}

extern "C" PUBLIC DWORD D3DPERF_QueryRepeatFrame()
{
}

extern "C" PUBLIC DWORD D3DPERF_SetMarker()
{
    // Stub, no return
}

extern "C" PUBLIC DWORD D3DPERF_SetRegion()
{
    // Stub, no return
}

extern "C" PUBLIC DWORD D3DPERF_GetOptions()
{
}

extern "C" PUBLIC DWORD D3D9GetSWInfo()
{
    // Stub, no return
}

extern "C" PUBLIC DWORD D3DPERF_SetOptions(DWORD dwOptions)
{
    // Stub, no return
}

extern "C" PUBLIC DWORD D3D9GetDriverState()
{
    // Stub, no return
}

static bool load_dll(bool critical)
{
    static const std::vector<const char *> exports =
        {
            "Direct3DCreate9",
            "Direct3DShaderValidatorCreate9",
            "PSGPError",
            "PSGPSampleTexture",
            "D3DPERF_BeginEvent",
            "D3DPERF_EndEvent",
            "D3DPERF_GetStatus",
            "D3DPERF_QueryRepeatFrame",
            "D3DPERF_SetMarker",
            "D3DPERF_SetRegion",
            "D3DPERF_GetOptions",
            "D3D9GetSWInfo",
            "D3DPERF_SetOptions",
            "D3D9GetDriverState"
        };

    static char mod_path[2048];
    static char original_path[256];
    static bool loaded = false;

    MutexLocker lock(&mutex);

    if (loaded)
        return true;

    myself = GetModuleHandleW(L"d3d9.dll");

    GetModuleFileNameA(myself, mod_path, sizeof(mod_path));

    myself_path = NormalizePath(mod_path);
    myself_path = myself_path.substr(0, myself_path.rfind('/') + 1);

    if (FileExists(myself_path + "d3d9_other.dll"))
    {
        strncpy(original_path, myself_path.c_str(), sizeof(original_path));
        strncat(original_path, "d3d9_other.dll", sizeof(original_path));
    }
    else
    {
        if (GetSystemDirectoryW((wchar_t *)original_path, sizeof(original_path)) == 0)
            return false;
        strncat(original_path, "\\d3d9.dll", sizeof(original_path));
    }

    patched_dll = LoadLibraryW((wchar_t *)(original_path));
    if (!patched_dll)
    {
        return false;
    }

    for (auto &export_name : exports)
    {
        uint64_t ordinal = (uint64_t)export_name;

        uint8_t *orig_func = (uint8_t *)GetProcAddress(patched_dll, export_name);

        if (!orig_func)
        {
            if (ordinal < 0x1000)
            {
                continue;
            }
            else
            {
                return false;
            }
        }
        uint8_t *my_func = (uint8_t *)GetProcAddress(myself, export_name);
        Hook(my_func, nullptr, orig_func);
    } // MessageBoxA(NULL, "buffer", "load_dll", MB_OK);

    loaded = true;
    return true;
}

static void unload_dll()
{
    if (patched_dll)
    {
        FreeLibrary((HMODULE)patched_dll);
        patched_dll = nullptr;
    }
}

VOID WINAPI GetStartupInfoW_Patched(LPSTARTUPINFOW lpStartupInfo)
{
    static bool started = false;

    if (!started)
    {
        if (!load_dll(true))
            exit(-1);
    }

    // MessageBoxA(NULL, "buffer", "GetStartupInfoW_Patched", MB_OK);

    GetStartupInfoW(lpStartupInfo);
}

DWORD WINAPI StartThread(LPVOID)
{ // MessageBoxA(NULL, "buffer", "StartThread", MB_OK);

    load_dll(false);
    return 0;
}

// Hook for setting up the console
void SetupConsole()
{
    AllocConsole();

    FILE *fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
}

static BOOL InGameProcess(VOID)
{
    return true;
}

extern "C" BOOL EXPORT DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
        {
            if (InGameProcess())
            {
                HANDLE hProcess = GetCurrentProcess();
                uintptr_t moduleBaseAddress = GetModuleBaseAddress(L"sims2ep9rpc.exe");
                if (!load_dll)
                    return FALSE;

                if (!HookImport("KERNEL32.dll", "GetStartupInfoW", (void *)GetStartupInfoW_Patched))
                {
                    return TRUE;
                }
            }
        }
        break;

        case DLL_PROCESS_DETACH:
        {
            if (!lpvReserved)
            {
                unload_dll();
            }
                    break;

        }
    }
    return TRUE;
}