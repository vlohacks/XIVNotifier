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
    uint32_t ppl_ahead;


    DWORD pid_by_exename(const WCHAR *exe_filename);
    void* find_mem_location(HANDLE proc);

public:
    XIVLoginObserver();

    bool hookGame();
    int getQueue();
    void reset();
};

#endif // XIVLOGINOBSERVER_H
