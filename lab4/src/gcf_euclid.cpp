#include "functions.h"
#include <iostream>

extern "C" {
    int GCF(int A, int B) {
        if (B == 0) {
            return A;
        }
        return GCF(B, A % B);
    }
}