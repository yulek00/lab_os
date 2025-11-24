#include "parent.h"
#include <iostream>

int main() {
    std::string filename;
    std::cout << "Parent: Введите имя файла:" << std::endl;
    std::getline(std::cin, filename);

    if (filename.empty()) return 1;

    parent::Parent p;
    p.CreateChild(filename);
    p.Input();
    p.EndChild();
    
    return 0;
}