#pragma once

#include <csignal>
#include <string>
#include <sys/types.h>

namespace os {
    using ProcessHandle = pid_t;
    using FileHandle = int;
    using SignalHandler = void (*)(int);
    
    constexpr int WORK = SIGUSR1;    
    constexpr int CONFIRM = SIGUSR2;

    constexpr size_t SHM_SIZE = 4096;

    struct SharedMemory {
        char* ptr;
        size_t size;
        FileHandle fd;
        std::string name;
    };

    ProcessHandle CreateChildProcess(const std::string& exe_name, const std::string& filename, const std::string& shm_name);
    void WaitForChild(ProcessHandle process);
    ProcessHandle GetParentPID();

    SharedMemory CreateShM(const std::string& name, size_t size);
    SharedMemory OpenShM(const std::string& name, size_t size);
    void DestroyShM(SharedMemory& shm);
    void UnmapShM(SharedMemory& shm);

    void SetSignalHandler(int signum, SignalHandler handler);
    void DefaultSignalHandler(int signum);
    void SendSignal(ProcessHandle pid, int signum);
    void WaitSignal(); 
    void Exit(int code);

    extern volatile sig_atomic_t terminated;
    void TerminateHandler(int signum);
}