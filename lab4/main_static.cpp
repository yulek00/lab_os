#include <iostream>
#include "functions.h"

int main() {
    int cmd;
    std::cout << "Lab 4: Static Linking (Euclid + Rectangle)\n";
    std::cout << "1 <A> <B> - Calculate GCF\n";
    std::cout << "2 <A> <B> - Calculate Area\n";
    std::cout << "0         - Exit\n";

    while (std::cin >> cmd) {
        if (cmd == 1) {
            int a, b;
            std::cin >> a >> b;
            std::cout << "GCF(" << a << ", " << b << ") = " << GCF(a, b) << std::endl;
        } 
        else if (cmd == 2) {
            float a, b;
            std::cin >> a >> b;
            std::cout << "Area: " << Square(a, b) << std::endl;
        }
        else if (cmd == 0) {
            std::cout << "Exiting...\n";
            break;
        }
        else {
            std::cout << "Unknown command.\n";
        }
    }
    return 0;
}