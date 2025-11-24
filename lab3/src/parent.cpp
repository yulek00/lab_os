#include "parent.h"
#include <iostream>
#include <cstring>
#include <unistd.h>

namespace parent {
    Parent::Parent() {
        shm = os::CreateShM(shm_name, os::SHM_SIZE);
        os::SetSignalHandler(os::CONFIRM, os::DefaultSignalHandler);
        os::SetSignalHandler(SIGTERM, os::TerminateHandler);
    }

    void Parent::CreateChild(const std::string& filename) {
        child_pid = os::CreateChildProcess("./child", filename, shm_name);

        if (child_pid == -1) {
            std::cerr << "Parent: Ошибка создания child" << std::endl;
            os::Exit(1);
        }
        usleep(200000);
    }

    void Parent::Input() {
        std::cout << "Введите числа: 'делимое делитель...'. 'exit' для выхода." << std::endl;
        std::string input;

        while (std::getline(std::cin, input)) {
            if (os::terminated) {
                std::cerr << "Parent: Аварийное завершение по SIGTERM от ребенка. Выход." << std::endl;
                break;
            }

            if (input.empty()) {
                continue;
            }

            std::memset(shm.ptr, 0, shm.size);
            std::strncpy(shm.ptr, input.c_str(), shm.size - 1);

            os::SendSignal(child_pid, os::WORK);

            if (input == "exit") {
                break;
            }

            os::WaitSignal(); 
            if (os::terminated) {
                 std::cerr << "Parent: Аварийное завершение по SIGTERM от ребенка. Выход." << std::endl;
                 break;
            }
        }
    }

    void Parent::EndChild() {
        if (!os::terminated) {
            memset(shm.ptr, 0, shm.size);
            strncpy(shm.ptr, "exit", shm.size - 1);
            os::SendSignal(child_pid, os::WORK);
        }
        os::WaitForChild(child_pid);
    }

    Parent::~Parent() {
        os::DestroyShM(shm);
    }
}