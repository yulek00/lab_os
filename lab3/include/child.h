#pragma once

#include "os.h"
#include <cstdio>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>

namespace child {
    class Child {
    private:
        std::string filename;
        std::string shm_name;
        FILE* file;
        os::SharedMemory shm;

        std::vector<int> parse_numbers(const std::string& input_line);

    public:
        Child(const std::string& filename_arg, const std::string& shm_name_arg);
        void ProcessDivision();
        ~Child();
    };
}