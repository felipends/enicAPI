#ifndef VIABILITY_CHECKER_FACTORY_H
#define VIABILITY_CHECKER_FACTORY_H

#include <vector>
#include "IViabilityChecker.h"
#include "models/EventInfo.h"

using namespace std;

class ViabilityCheckerFactory {
    public:
        static IViabilityChecker* create(vector<Cluster*>* clusters, EventInfo* eventInfo, bool clearTimeSlots = true);
        static void printCheckerVersion();
};  

#endif