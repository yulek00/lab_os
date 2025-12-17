#include <iostream>
#include <dlfcn.h>
#include "functions.h" 

using GCF_Func = int (*)(int, int);
using Square_Func = float (*)(float, float);

const char* LIB_EUCLID = "./libgcf_euclid.so";
const char* LIB_NAIVE = "./libgcf_naive.so";
const char* LIB_RECT = "./libsquare_rect.so";
const char* LIB_TRIANGLE = "./libsquare_triangle.so";

int main() {
    void* handle_gcf = nullptr;
    void* handle_square = nullptr;
    
    GCF_Func CalcGCF = nullptr;
    Square_Func CalcSquare = nullptr;

    int mode = 0; 

    auto load_libs = [&](int current_mode) {
        if (handle_gcf) dlclose(handle_gcf);
        if (handle_square) dlclose(handle_square);

        const char* path_gcf = (current_mode == 0) ? LIB_EUCLID : LIB_NAIVE;
        const char* path_sq  = (current_mode == 0) ? LIB_RECT : LIB_TRIANGLE;

        handle_gcf = dlopen(path_gcf, RTLD_LAZY);
        handle_square = dlopen(path_sq, RTLD_LAZY);

        if (!handle_gcf || !handle_square) {
            std::cerr << "Error loading libs: " << dlerror() << "\n";
            exit(EXIT_FAILURE);
        }

        CalcGCF = (GCF_Func)dlsym(handle_gcf, "GCF");
        CalcSquare = (Square_Func)dlsym(handle_square, "Square");

        if (!CalcGCF || !CalcSquare) {
            std::cerr << "Error loading symbols: " << dlerror() << "\n";
            exit(EXIT_FAILURE);
        }

        std::cout << "Switched to: "
                  << ((current_mode == 0) ? "Euclid & Rectangle" : "Naive & Triangle")
                  << "\n";
    };

    load_libs(mode);

    int cmd;
    std::cout << "Lab 4: Dynamic Loading\n";
    std::cout << "0 - Exit\n";
    std::cout << "1 <A> <B> - Compute GCF\n";
    std::cout << "2 <A> <B> - Compute Area\n";
    std::cout << "3 - Switch implementation\n";

    while (std::cin >> cmd) {
        if (cmd == 0) {
            std::cout << "Exiting...\n";
            break;
        }
        else if (cmd == 3) {
            mode = !mode;
            load_libs(mode);
        }
        else if (cmd == 1) {
            int a, b;
            std::cin >> a >> b;
            std::cout << "GCF Result: " << CalcGCF(a, b) << "\n";
        }
        else if (cmd == 2) {
            float a, b;
            std::cin >> a >> b;
            std::cout << "Square Result: " << CalcSquare(a, b) << "\n";
        }
        else {
            std::cout << "Unknown command\n";
        }
    }

    if (handle_gcf) dlclose(handle_gcf);
    if (handle_square) dlclose(handle_square);

    return 0;
}