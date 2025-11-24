#include "child.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Child: Ошибка аргументов." << std::endl;
        return 1;
    }
    
    std::string filename = argv[1]; 
    std::string shm_name = argv[2];

    child::Child child_proc(filename, shm_name);
    child_proc.ProcessDivision();
    return 0;
}