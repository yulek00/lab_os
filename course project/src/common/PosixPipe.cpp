#include "PosixPipe.h"
#include <cstdio>
#include <cerrno>
#include <cstring>

POSIXPipe::POSIXPipe(std::string _path) : path(_path), fd(-1) {}

POSIXPipe::~POSIXPipe() {
    closePipe();
}

bool POSIXPipe::create() {
    if (mkfifo(path.c_str(), 0666) == -1) {
        if (errno != EEXIST) {
            fprintf(stderr, "Error creating FIFO %s: %s\n", path.c_str(), strerror(errno));
            return false;
        }
    }
    return true;
}

void POSIXPipe::unlinkFile() {
    unlink(path.c_str());
}

bool POSIXPipe::openWrite() {
    fd = open(path.c_str(), O_WRONLY);
    if (fd == -1) return false;
    return true;
}

bool POSIXPipe::openRead() {
    fd = open(path.c_str(), O_RDWR);
    if (fd == -1) return false;
    return true;
}

void POSIXPipe::closePipe() {
    if (fd != -1) {
        close(fd);
        fd = -1;
    }
}

bool POSIXPipe::send(const void* data, size_t size) {
    if (fd == -1) throw exceptions::PipeException("Cannot send: FIFO not open.");
    ssize_t written = write(fd, data, size);
    return (written == (ssize_t)size);
}

bool POSIXPipe::receive(void* data, size_t size) {
    if (fd == -1) throw exceptions::PipeException("Cannot receive: FIFO not open.");
    ssize_t bytes = read(fd, data, size);
    return (bytes == (ssize_t)size);
}