#include "child.h"

namespace child {
    Child::Child(const std::string& filename_arg) : filename(filename_arg), file(nullptr) {
        file = std::fopen(filename.c_str(), "a");
        if (!file) {
            std::cerr << "Child: Ошибка! Не удалось открыть файл: " << filename << std::endl;
        } else {
            std::fprintf(file, "Child: Запущен. Файл вывода: %s\n", filename.c_str());
        }
    }

    std::vector<int> Child::parse_numbers(const std::string& input_line) {
        std::vector<int> numbers;
        std::stringstream ss(input_line);
        int num;
        while (ss >> num) {
            numbers.push_back(num);
        }
        return numbers;
    }

    void Child::ProcessDivision() {
        if (!file) {
            return;
        }

        std::string line;
        while (std::getline(std::cin, line)) {
            if (line.empty()) continue;
            
            std::vector<int> numbers = parse_numbers(line);
            if (numbers.size() < 2) {
                std::fprintf(file, "Ошибка: Нужно минимум 2 числа, получено %zu\n", numbers.size());
                continue;
            }

            int dividend = numbers[0];
            std::fprintf(file, "Делим %d на: ", dividend);
            for (size_t i = 1; i < numbers.size(); i++) {
                std::fprintf(file, "%d", numbers[i]);
                if (i < numbers.size() - 1) std::fprintf(file, ", ");
            }
            std::fprintf(file, "\n");

            for (size_t i = 1; i < numbers.size(); i++) {
                if (numbers[i] == 0) {
                    std::fprintf(file, "КРИТИЧЕСКАЯ ОШИБКА: Деление на ноль! %d / %d\n", dividend, numbers[i]);
                    std::fprintf(file, "Child: Завершение работы из-за деления на ноль.\n");
                    std::cerr << "Обнаружено деление на ноль. Завершение процесса." << std::endl;
                    std::fclose(file);
                    exit(1);
                }
                int result = dividend / numbers[i];
                std::fprintf(file, "%d / %d = %d\n", dividend, numbers[i], result);
            }
            std::fprintf(file, "---\n");
            std::fflush(file);
        }
    }

    Child::~Child() {
        if (file) {
            std::fprintf(file, "Child: Работа завершена.\n");
            std::fclose(file);
        }
    }
}