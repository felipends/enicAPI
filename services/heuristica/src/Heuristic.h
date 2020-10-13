#ifndef HEURISTIC_H
#define HEURISTIC_H

#include <iostream>
#include "models/EventInfo.h"
#include "models/Result.h"
#include "Cluster.h"
#include "utils/ClusterPool.h"
#include "TimeSlot.h"
#include "utils/OutputGenerator.h"
#include "utils/InstanceReader.h"
#include "models/ContiguousCluster.h"
#include "ProfessorsModel.h"
#include "ContiguousClusteringOrTools.h"

using namespace std;

class Heuristic {
    private:
        string dictFileName; // to get back the name corresponding to its id

        OutputGenerator *output;
        EventInfo *eventInfo; 
        vector<Cluster*> clusters; // current cluster
        vector<int> selectedPapers; // aux list to build a inital solution
        ClusterPool clusterPool; // data structure to save the clusters
        vector<Cluster*> clustersFromBestSolution; // store the list of cluster from the best solution found
        vector<Cluster*> clustersFromBestIteratonResult;  // store the best clusters inside a iteration
        Result *result;
        static Heuristic* instance;
        vector<TimeSlot*> *ilsSolution;

        int ILS(InstanceReader* instanceReader);
        void printInstanceInfo();
        void greedyConstruction();
        void initClusters();
        void populateClustersWithOnePaper();
        bool insertPapers();
        void alocatePapersInClustersByBestBenefit(vector<Paper*> *papers);
        void saveBestSolution(vector<Cluster*>* clusters);
        void saveBestIterationResult(vector<Cluster*>* clusters);
        int RVND(bool &isSoltionFromRVNDStore);
        bool hasInviableClusters();
        bool acceptanceProbability(int solution, int bestIterationSolution, double temp);
        void copyClustersFromBestIterationResult();
        vector<Cluster*>* getClustersNotEmpty(vector<Cluster*> *clusters);
        vector<TimeSlot*>* runSetPartition();
        void saveResultToFile(vector<TimeSlot*>* timeslots);

    public:
        Heuristic(EventInfo *eventInfo, string dictFileName);
        ~Heuristic();
        int solve(InstanceReader* instanceReader); 
        OutputGenerator *getOutput();
        static Heuristic* getInstance();
        int runILSOnce(vector<Cluster*> *clustersFromSp);
};

#endif