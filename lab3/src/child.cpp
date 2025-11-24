#include "child.h"
#include <cstring>
#include <unistd.h>

namespace child {
    Child::Child(const std::string& filename_arg, const std::string& shm_name_arg) 
        : filename(filename_arg), shm_name(shm_name_arg), file(nullptr) {
        file = std::fopen(filename.c_str(), "a");
        if (!file) {
            std::cerr << "Child: Ошибка! Не удалось открыть файл: " << filename << std::endl;
            os::Exit(1);
        } else {
            std::fprintf(file, "Child: Запущен. Файл вывода: %s\n", filename.c_str());
        }

        shm = os::OpenShM(shm_name, os::SHM_SIZE);
		os::SetSignalHandler(os::WORK, os::DefaultSignalHandler);
    }

    std::vector<int> Child::parse_numbers(const std::string& input_line) {
        std::vector<int> numbers;
        std::stringstream ss(input_line);
        int num;
        while (ss >> num) numbers.push_back(num);
        return numbers;
    }

    void Child::ProcessDivision() {
        if (!file) {
            return;
        }

        while (true) {
            os::WaitSignal();

            if (os::terminated) {
                std::fprintf(file, "Child: Получен SIGTERM. Завершение работы.\n");
                std::fflush(file);
                break;
            }

            std::string line = shm.ptr;
            if (line == "exit" || line.empty()) {
                std::fprintf(file, "Child: Получена команда на завершение.\n");
                std::fflush(file);
                break;
            }
            std::vector<int> numbers = parse_numbers(line);
            if (numbers.size() < 2) {
                std::fprintf(file, "Ошибка: Нужно минимум 2 числа.\n");
            } else {
                int dividend = numbers[0];
                for (size_t i = 1; i < numbers.size(); i++) {
                    if (numbers[i] == 0) {
                        std::fprintf(file, "КРИТИЧЕСКАЯ ОШИБКА: Деление на ноль!\n");
                        std::fflush(file);
                        std::cerr << "Child: Завершение работы из-за деления на ноль." << std::endl;
                        os::SendSignal(os::GetParentPID(), SIGTERM);
                        std::fclose(file);
                        os::UnmapShM(shm);
                        os::Exit(1);
                    }
                    
                    std::fprintf(file, "%d / %d = %d\n", dividend, numbers[i], dividend / numbers[i]);
                }
            }

            std::fprintf(file, "---\n");
            std::fflush(file);

            os::SendSignal(os::GetParentPID(), os::CONFIRM);
        }
    }

    Child::~Child() {
        if (file) {
            std::fclose(file);
        }
        os::UnmapShM(shm);
    }
}