#include "Result.h"

Result::Result(bool solutionFound, double value) {
    this->solutionFound = solutionFound;
    this->value = value;
}

bool Result::isSolutionFound() {
    return solutionFound;
}

double Result::getValue() {
    return value;
}
