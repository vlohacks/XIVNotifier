#include "xivloginobserver.h"

#include <QDebug>

XIVLoginObserver::XIVLoginObserver()
    : proc(nullptr)
    , ptr(nullptr)
{}

bool XIVLoginObserver::hookGame()
{
    DWORD pid;
    const WCHAR *str;
    int i;

    const WCHAR *exe_names[] = {
        L"ffxiv_dx11.exe",
        L"ffxiv.exe",
        0
    };

    for (i = 0; (str = exe_names[i]) != 0; i++) {
        pid = pid_by_exename(str);
        if (pid > 0)
            break;
    }

    if (pid == 0)
        return false;

    proc = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (proc == nullptr)
        return false;

    ptr = find_mem_location(proc);
    return (ptr != 0);
}




int XIVLoginObserver::getQueue()
{
    char buffer[12];
    size_t num_read;
    uint32_t ppl_ahead;

    if (!ptr)
        return -1;

    if (ReadProcessMemory(proc, ptr, buffer, sizeof(buffer), &num_read)) {
        // if magic signature still exists, login is in progress,
        // else the user is logged in
        if (*(uint64_t*)&buffer[4] == MAGIC_SIGNATURE) {
            ppl_ahead = *(uint32_t*)buffer;
        } else {
            ppl_ahead = 0;
            CloseHandle(proc);
            proc = nullptr;
            ptr = nullptr;
        }
    }

    return (int)ppl_ahead;
}


void XIVLoginObserver::reset()
{
    ptr = nullptr;
    if (proc)
        CloseHandle(proc);
    proc = nullptr;
}

DWORD XIVLoginObserver::pid_by_exename(const WCHAR *exe_filename)
{
    PROCESSENTRY32 entry;
    DWORD ret = 0;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    entry.dwSize = sizeof(entry);

    if (Process32First(snapshot, &entry) == TRUE) {
        while (Process32Next(snapshot, &entry) == TRUE) {
            if (wcscmp(entry.szExeFile, exe_filename) == 0) {
                ret = entry.th32ProcessID;
                break;
            }
        }
    }

    CloseHandle(snapshot);
    return ret;
}


void* XIVLoginObserver::find_mem_location(HANDLE proc)
{
    uint64_t addr = 0;
    uint64_t ptr;
    SIZE_T num_read;
    MEMORY_BASIC_INFORMATION info;
    char *buffer;
    const size_t USERSPACE_LIMIT = 0x7fffffffffff;
    size_t buffer_size = 4096;

    buffer = (char*)malloc(buffer_size);

    // walk whole user address space
    while (addr < USERSPACE_LIMIT) {
        // check if region is mapped and read/write and therefore most likely
        // heap memory
        if (VirtualQueryEx(proc, (void*)addr, &info, sizeof(info))) {
            if (info.Protect == PAGE_READWRITE) {
                ptr = (uint64_t)info.BaseAddress;
                // dynamicly grow local buffer, if mem region exceeds buffer
                // size
                if (info.RegionSize > buffer_size) {
                    buffer_size = info.RegionSize;
                    buffer = (char*)realloc(buffer, buffer_size);
                }
                // read the memory region and search for the magic which is 4
                // bytes ahead the login counter.
                // Note: reading the whole region and searching locally is way
                // faster than ReadProcessMemory system calls every 4 bytes.
                ReadProcessMemory(proc, (void*)ptr, buffer,
                    info.RegionSize, &num_read);
                for (ptr = 0; ptr < info.RegionSize - 4; ptr += 4) {
                    if (*(uint64_t*)&buffer[ptr + 4] == MAGIC_SIGNATURE) {
                        free(buffer);
                        return (void*)(info.BaseAddress + ptr);
                    }
                }
            }
            addr += info.RegionSize;
        } else {
            // this won't happen most likely...
            addr += 0x10000;
        }
    }

    free(buffer);
    return 0;
}
