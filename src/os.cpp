#include "os.h"
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include <cstdio>

namespace os {

    int CreateChildProcess(const StartProcess& args) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork failed");
            return -1;
        }

        if (pid == 0) {
            if (args.stdin_pipe != -1) {
                if (dup2(args.stdin_pipe, STDIN_FILENO) == -1) {
                    perror("dup2 failed");
                    Exit(1);
                }
                close(args.stdin_pipe);
            }
            
            execl(args.path.c_str(), args.filename.c_str(), nullptr);
            perror("execl failed");
            Exit(1);
        }
        
        return pid;
    }

    bool CreatePipe(PipeHandle& readpipe, PipeHandle& writepipe) {
        int pipefd[2];
        if (pipe(pipefd) != 0) {
            perror("pipe creation failed");
            return false;
        }
        readpipe = pipefd[0];
        writepipe = pipefd[1];
        return true;
    }

    int WritePipe(PipeHandle pipe, const void* buf, int count) {
        int result = write(pipe, buf, count);
        if (result == -1) {
            perror("write pipe failed");
        }
        return result;
    }

    int ReadPipe(PipeHandle pipe, void* buf, int count) {
        int result = read(pipe, buf, count);
        if (result == -1) {
            perror("read pipe failed");
        }
        return result;
    }

    void ClosePipe(PipeHandle pipe) {
        if (pipe != -1) {
            close(pipe);
        }
    }

    void Exit(int code) {
        _exit(code);
    }

    int WaitForChild() {
        int status = wait(nullptr);
        return status;
    }

}