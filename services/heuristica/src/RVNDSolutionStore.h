#ifndef RVNDSOLUTIONSTORE_H
#define RVNDSOLUTIONSTORE_H

#include <iostream>
#include "./RVNDSolution.h"
#include "./Cluster.h"

using namespace std;

class RVNDSolutionStore {
    private:
        static RVNDSolutionStore* instance;
        vector<RVNDSolution*> solutions;
        RVNDSolutionStore();
    public:
        static RVNDSolutionStore* getInstance();
        void storeSolution(vector<Cluster*> *clusters, int solutionValue);
        bool isSolutionStored(int solutionValue, vector<Cluster*> *clusters, int &result);
};

#endif