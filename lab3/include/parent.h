#pragma once

#include "os.h"
#include <string>

namespace parent {
    class Parent {
    private:
        os::ProcessHandle child_pid = -1;
        os::SharedMemory shm;
        std::string shm_name = "/lab3_shm";

    public:
        Parent();
        void CreateChild(const std::string& filename);
        void Input();
        void EndChild();
        ~Parent();
    };
}