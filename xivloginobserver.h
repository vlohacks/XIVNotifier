#ifndef XIVLOGINOBSERVER_H
#define XIVLOGINOBSERVER_H

#include <cstdint>
#include "windows.h"
#include <tlhelp32.h>

class XIVLoginObserver
{
private:
    static constexpr uint64_t MAGIC_SIGNATURE = 0x0000000000013395;

    void *ptr;
    HANDLE proc;
    DWORD pid;
    const WCHAR *exe_name;

    DWORD pid_by_exename(const WCHAR *exe_filename);
    void* find_mem_location(HANDLE proc);

public:
    XIVLoginObserver();

    bool hookGame();
    int getQueue();
    void reset();

    bool hasPid() const { return pid != 0; }
    bool hasPtr() const { return ptr != 0; }

    const void *getPtr() const { return ptr; }
    const uint64_t getPtrU64() const { return (uint64_t)ptr; }
    const DWORD getPid() const { return pid; }
    const WCHAR *getExeName() const { return exe_name; }
};

#endif // XIVLOGINOBSERVER_H
