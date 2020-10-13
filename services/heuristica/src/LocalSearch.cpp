#include "./LocalSearch.h"
#include <ctime>
#include <cstdlib>
#include "./ClustersInfo.h"
#include "neighborhood/MoveNeightborhood.h"
#include "neighborhood/SwapNeightborhood.h"
#include "./utils/delete_vector.h"

LocalSearch::LocalSearch(vector<Cluster*> *clusters, EventInfo *event) {
    this->clusters = clusters;    
    int numPapers = event->getPapers().size();
    int numClusters = clusters->size();
    rnvdSolutionStore = RVNDSolutionStore::getInstance();    
    ClustersInfo::Configure(numPapers, numClusters, clusters); 
}

void LocalSearch::initDataStructures() { 
    for (Cluster *c : *clusters) {               
        vector<MovementInfo*>* clusterMovements = c->calculateRemoveMovement();        
        for (MovementInfo* m : *clusterMovements) {
            movementsInfo.push_back(m);
        }
        delete clusterMovements;
    }

    for (MovementInfo* m : movementsInfo) {
        m->calculateInsertMovements();
    }

    for (MovementInfo* m : movementsInfo) {
        m->calculateSwapMovements(&movementsInfo);
    } 
}

LocalSearch::~LocalSearch() {
    deleteVectorElements(&movementsInfo);
    ClustersInfo::clearData();
}

INeighborhood* LocalSearch::chooseRandomNeighborhood(vector<INeighborhood*> *list) {
    int index = rand() % list->size();
    return list->at(index);
}

int LocalSearch::RVND(vector<Cluster*> &clustersOut, bool &isSoltionFromRVNDStore) {    
    isSoltionFromRVNDStore = false;
    int bestResult = 0;
    
    initDataStructures();
    vector<INeighborhood*> neighborhoodList;    
    neighborhoodList.push_back(new MoveNeightborhood(&movementsInfo));
    neighborhoodList.push_back(new SwapNeightborhood(&movementsInfo));

    while(neighborhoodList.size() > 0) {
        INeighborhood* neighborhood = chooseRandomNeighborhood(&neighborhoodList);
        neighborhood->findBestNeightbor();
        if (neighborhood->isObjectiveFunctionBetter()) {            
            neighborhood->updateNeightboorhood();            
            int newObjValue = Cluster::sumClustersValues(this->clusters);            
            int resultFromStore;
            if (rnvdSolutionStore->isSolutionStored(newObjValue, this->clusters, resultFromStore)) {
                clustersOut = *clusters;
                isSoltionFromRVNDStore = true;

                for (auto it = neighborhoodList.begin(); it != neighborhoodList.end();) {
                    auto aux = *it;
                    it = neighborhoodList.erase(it);
                    delete aux;
                }                
                return resultFromStore;
            }

            if (newObjValue > bestResult) {
                bestResult = newObjValue;
            }
        } else {       
            neighborhoodList.erase(remove(neighborhoodList.begin(), neighborhoodList.end(), neighborhood), neighborhoodList.end());
            delete neighborhood;            
        }
    }
    clustersOut = *clusters;
    return bestResult;
}