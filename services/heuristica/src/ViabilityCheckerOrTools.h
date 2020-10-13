#ifndef VIABILITY_CHECkER_ORTOOLS_H
#define VIABILITY_CHECkER_ORTOOLS_H

#include <iostream>
#include <vector>
#include <map>
#include "../../../..//or-tools/ortools/linear_solver/linear_solver.h"
#include "Cluster.h"
#include "models/EventInfo.h"
#include "TimeSlot.h"
#include "IViabilityChecker.h"

using namespace std;
using namespace operations_research;

class ViabilityCheckerOrTools : public IViabilityChecker {
    private:
        bool clearTimeSlots;
        vector<Cluster*> *clusters;
        EventInfo* eventInfo;
        vector<TimeSlot*> *timeSlots;
        map<int, vector<TimeSlot*>*> dayTimeSlots;
        map<MPVariable*, bool> validVarMap;
        static int maxParallelSessions;
        void extractTimeSlotsFromSessions();
        int getMaxTopicsPerDay(Topic* topic);
        void deleteTimeSlots();
        void insertClustersInTimeSlots(vector<vector<vector<MPVariable*>>> &y);
        vector<TimeSlot*> getTimeSlotsFromDay(int day);
    
    public:
        ViabilityCheckerOrTools(vector<Cluster*>* clusters, EventInfo* eventInfo, bool clearTimeSlots = true);        
        ~ViabilityCheckerOrTools();
        bool isViable();
        vector<TimeSlot*>* getTimeSlots();
};

#endif
