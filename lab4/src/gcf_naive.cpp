#include "functions.h"
#include <algorithm>
#include <iostream>

extern "C" {
    int GCF(int A, int B) {
        int gcd = 1;
        int limit = std::min(A, B);

        for (int i = 1; i <= limit; ++i) {
            if (A % i == 0 && B % i == 0) {
                gcd = i;
            }
        }
        return gcd;
    }
}