#pragma once

#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace child {
    class Child {
    private:
        std::string filename;
        FILE* file;
        std::vector<int> parse_numbers(const std::string& input_line);

    public:
        explicit Child(const std::string& filename_arg);
        void ProcessDivision();
        ~Child();
    };
}