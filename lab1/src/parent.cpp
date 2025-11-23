#include "parent.h"
#include "os.h"
#include <iostream>
#include <string>
#include <cstdlib>

namespace parent {
    Parent::Parent() : pipe_write_end(-1), pipe_read_end(-1) {}

    void Parent::CreateChildProcess(const std::string& filename) {
        os::PipeHandle read_pipe, write_pipe;
        
        if (!os::CreatePipe(read_pipe, write_pipe)) {
            std::cerr << "Parent: Ошибка создания pipe" << std::endl;
            exit(1);
        }

        pipe_read_end = read_pipe;
        pipe_write_end = write_pipe;

        os::StartProcess child_info;
        child_info.path = "./child";
        child_info.filename = "child";
        child_info.stdin_pipe = pipe_read_end;

        if (os::CreateChildProcess(child_info) == -1) {
            std::cerr << "Parent: Ошибка создания дочернего процесса" << std::endl;
            exit(1);
        }

        os::ClosePipe(pipe_read_end);
        pipe_read_end = -1;
        std::string filename_with_newline = filename + "\n";
        int written = os::WritePipe(pipe_write_end, filename_with_newline.c_str(), filename_with_newline.size());
        if (written == -1) {
            std::cerr << "Parent: Ошибка записи в pipe" << std::endl;
        }
    }

    void Parent::Input() {
        std::cout << "Введите числа для деления (формат: 'делимое делитель1 делитель2 ...')." << std::endl;
        std::cout << "Для выхода введите 'exit'." << std::endl;
        std::string input;

        while (std::getline(std::cin, input)) {
            if (input == "exit") {
                break;
            }
            if (input.empty()) continue;

            input += "\n";
            int written = os::WritePipe(pipe_write_end, input.c_str(), input.size());
            if (written == -1) {
                std::cerr << "Parent: Ошибка записи в pipe" << std::endl;
                break;
            }
        }
    }

    void Parent::EndChild() {
        os::ClosePipe(pipe_write_end);
        pipe_write_end = -1;
        if (os::WaitForChild() == -1) {
            std::cerr << "Parent: Ошибка ожидания дочернего процесса" << std::endl;
        }
    }

    Parent::~Parent() {
        if (pipe_write_end != -1) {
            os::ClosePipe(pipe_write_end);
        }
        if (pipe_read_end != -1) {
            os::ClosePipe(pipe_read_end);
        }
    }
}