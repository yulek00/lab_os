#include "functions.h"
#include <iostream>

extern "C" {
    float Square(float A, float B) {
        return A * B;
    }
}