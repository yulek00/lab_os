#include "child.h"
#include <iostream>
#include <string>

int main() {
    std::string filename;
    if (!std::getline(std::cin, filename) || filename.empty()) {
        std::cerr << "Child: Ошибка!!! Не получено имя файла от родителя." << std::endl;
        return 1;
    }

    child::Child child_process(filename);
    child_process.ProcessDivision();
    return 0;
}