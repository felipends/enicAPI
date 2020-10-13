#ifndef LOCALSEARCH_H
#define LOCALSEARCH_H

#include <iostream>
#include <vector>
#include "./Cluster.h"
#include "models/EventInfo.h"
#include "./MovementInfo.h"
#include "neighborhood/INeighborhood.h"
#include "RVNDSolutionStore.h"

using namespace std;

class LocalSearch {
    private:
        vector<Cluster*> *clusters;
        vector<MovementInfo*> movementsInfo;
        RVNDSolutionStore *rnvdSolutionStore;

        void initDataStructures();
        INeighborhood* chooseRandomNeighborhood(vector<INeighborhood*> *list);        

    public:
        LocalSearch(vector<Cluster*> *clusters, EventInfo *event);
        ~LocalSearch();
        int RVND(vector<Cluster*> &clusters, bool &isSoltionFromRVNDStore);

};

#endif