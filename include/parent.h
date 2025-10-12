#pragma once

#include <iostream>
#include <string>

namespace parent {
    class Parent {
    private:
        int pipe_write_end;  
        int pipe_read_end;   

    public:
        Parent();
        void CreateChildProcess(const std::string& filename);
        void Input();
        void EndChild();
        ~Parent();
    };
}