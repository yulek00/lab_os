#pragma once
#include "exceptions.h"
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>


class POSIXPipe {
private:
    std::string path;
    int fd;

public:
    POSIXPipe(std::string _path);
    ~POSIXPipe();

    bool create();

    void unlinkFile();
    
    bool openWrite();

    bool openRead();
    void closePipe();

    bool send(const void* data, size_t size);

    bool receive(void* data, size_t size);
};