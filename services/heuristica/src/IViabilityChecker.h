#ifndef IVIABILITY_CHECKER
#define IVIABILITY_CHECKER

#include <vector>
#include "./TimeSlot.h"

using namespace std;

class IViabilityChecker {
    public:                
        virtual bool isViable() = 0;
        virtual vector<TimeSlot*>* getTimeSlots() = 0;
};

#endif