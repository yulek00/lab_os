#include "os.h"
#include <windows.h>
#include <string>

namespace os {

    int CreateChildProcess(const StartProcess& args) {
        STARTUPINFOA si{};
        PROCESS_INFORMATION pi{};
        si.cb = sizeof(si);
        si.dwFlags = STARTF_USESTDHANDLES;
        
        if (args.stdin_pipe != -1) {
            si.hStdInput = reinterpret_cast<HANDLE>(args.stdin_pipe);
        } else {
            si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
        }
        
        si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
        si.hStdError = GetStdHandle(STD_ERROR_HANDLE);

        std::string command_line = args.path;
        
        BOOL success = CreateProcessA(
            nullptr,
            const_cast<char*>(command_line.c_str()),
            nullptr,
            nullptr,
            TRUE, 
            0,
            nullptr,
            nullptr,
            &si,
            &pi
        );

        if (!success) {
            return -1;
        }

        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);

        return static_cast<int>(pi.dwProcessId);
    }

    bool CreatePipe(PipeHandle& readpipe, PipeHandle& writepipe) {
        SECURITY_ATTRIBUTES sa{};
        sa.nLength = sizeof(sa);
        sa.bInheritHandle = TRUE;
        HANDLE hRead, hWrite;

        if (!CreatePipe(&hRead, &hWrite, &sa, 0)) {
            return false;
        }

        readpipe = reinterpret_cast<PipeHandle>(hRead);
        writepipe = reinterpret_cast<PipeHandle>(hWrite);
        return true;
    }

    int WritePipe(PipeHandle pipe, const void* buf, int count) {
        if (pipe == -1) {
            return -1;
        }
        HANDLE h = reinterpret_cast<HANDLE>(pipe);
        DWORD bytesWritten = 0;

        if (!WriteFile(h, buf, static_cast<DWORD>(count), &bytesWritten, nullptr)) {
            return -1;
        }

        return static_cast<int>(bytesWritten);
    }

    int ReadPipe(PipeHandle pipe, void* buf, int count) {
        if (pipe == -1) {
            return -1;
        }
        HANDLE h = reinterpret_cast<HANDLE>(pipe);
        DWORD bytesRead = 0;

        if (!ReadFile(h, buf, static_cast<DWORD>(count), &bytesRead, nullptr)) {
            return -1;
        }

        return static_cast<int>(bytesRead);
    }

    void ClosePipe(PipeHandle pipe) {
        if (pipe != -1) {
            HANDLE h = reinterpret_cast<HANDLE>(pipe);
            CloseHandle(h);
        }
    }

    void Exit(int code) {
        ExitProcess(static_cast<UINT>(code));
    }

    int WaitForChild() {
        Sleep(1000);
        return 0;
    }

}