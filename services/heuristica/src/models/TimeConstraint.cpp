#include "TimeConstraint.h"

TimeConstraint::TimeConstraint(int day, int slot) {
    this->day = day;
    this->slot = slot;
}

int TimeConstraint::getDay() {
    return day;
}

int TimeConstraint::getSlot() {
    return slot;
}

bool TimeConstraint::compare(int day, int slot) {
    return this->day == day && this->slot == slot;
}