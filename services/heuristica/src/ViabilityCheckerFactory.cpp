#include "ViabilityCheckerFactory.h"
#include "ViabilityCheckerOrTools.h"

IViabilityChecker* ViabilityCheckerFactory::create(vector<Cluster*>* clusters, EventInfo* eventInfo, bool clearTimeSlots /*= true*/) {

    return new ViabilityCheckerOrTools(clusters, eventInfo, clearTimeSlots);
}

void ViabilityCheckerFactory::printCheckerVersion() {
    cout << "Using OrTools Checker" << endl;
}

