#include "os.h"
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <cstdlib>
#include <cstdio>
#include <iostream>

namespace os {

    volatile sig_atomic_t signal_received = 0;
    volatile sig_atomic_t terminated = 0;

    void DefaultSignalHandler(int) {
        signal_received = 1;
    }

    void TerminateHandler(int signum) {
        terminated = 1;
        signal_received = 1; 
    }

    int CreateChildProcess(const std::string& exe_name, const std::string& filename, const std::string& shm_name) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork failed");
            return -1;
        }

        if (pid == 0) {
            execl(exe_name.c_str(), exe_name.c_str(), filename.c_str(), shm_name.c_str(), nullptr);
            perror("execl failed");
            Exit(1);
        }
        
        return pid;
    }

    void WaitForChild(ProcessHandle process) {
        if (process > 0) {
            waitpid(process, nullptr, 0);
        }
    }

    ProcessHandle GetParentPID() {
        return getppid();
    }

    SharedMemory CreateShM(const std::string& name, size_t size) {
        shm_unlink(name.c_str());
        int fd = shm_open(name.c_str(), O_CREAT | O_RDWR, 0666);
        if (fd == -1) { 
            perror("shm create failed"); 
            Exit(1); 
        }
        if (ftruncate(fd, size) == -1) { 
            perror("ftruncate failed"); 
            Exit(1); 
        }
        
        char* ptr = (char*)mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (ptr == MAP_FAILED) { 
            perror("mmap failed"); 
            Exit(1); 
        }

        return {ptr, size, fd, name};
    }

    SharedMemory OpenShM(const std::string& name, size_t size) {
        int fd = -1;
        int attempts = 0;
        const int max_attempts = 10;
        
        while (attempts < max_attempts) {
            fd = shm_open(name.c_str(), O_RDWR, 0666);
            if (fd != -1) break;
            
            if (errno == ENOENT) {
                usleep(100000); 
                attempts++;
            } else {
                perror("shm open failed");
                Exit(1);
            }
        }
        if (fd == -1) { 
            perror("shm open failed"); 
            Exit(1); 
        }
        
        char* ptr = (char*)mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (ptr == MAP_FAILED) { 
            perror("mmap failed"); 
            Exit(1); 
        }

        return {ptr, size, fd, name};
    }

    void UnmapShM(SharedMemory& shm) {
        if (shm.ptr) {
            munmap(shm.ptr, shm.size);
        }
        if (shm.fd != -1) {
            close(shm.fd);
        }
    }

    void DestroyShM(SharedMemory& shm) {
        UnmapShM(shm);
        shm_unlink(shm.name.c_str());
    }

    void SendSignal(ProcessHandle pid, int signum) {
        kill(pid, signum);
    }

    void WaitSignal() {
        while (!signal_received && !terminated) {
            pause();
        }
        signal_received = 0;
    }
   
    void SetSignalHandler(int signum, SignalHandler handler) {
        struct sigaction sa;
        sa.sa_handler = handler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        sigaction(signum, &sa, nullptr);
    }

    void Exit(int code) {
        _exit(code);
    }
}