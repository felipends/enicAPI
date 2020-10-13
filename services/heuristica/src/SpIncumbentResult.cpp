#include "SpIncumbentResult.h"

SpIncumbentResult::SpIncumbentResult() {
    value = 0.0;
}

double SpIncumbentResult::getValue() {
    return value;
}

void SpIncumbentResult::setValue(double value) {
    this->value = value;
}