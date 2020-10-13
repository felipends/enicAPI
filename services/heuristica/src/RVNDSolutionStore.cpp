#include "./RVNDSolutionStore.h"

RVNDSolutionStore* RVNDSolutionStore::instance = NULL;

RVNDSolutionStore::RVNDSolutionStore() {}

RVNDSolutionStore* RVNDSolutionStore::getInstance() {
    if (instance == NULL) {
        instance = new RVNDSolutionStore();
    }
    return instance;
}

void RVNDSolutionStore::storeSolution(vector<Cluster*> *clusters, int solutionValue) {
    solutions.push_back(new RVNDSolution(clusters, solutionValue));
}

bool RVNDSolutionStore::isSolutionStored(int solutionValue, vector<Cluster*> *clusters, int &result) {
    string hash = RVNDSolution::generateHash(clusters);
    RVNDSolution *solution;
    for (RVNDSolution *s : solutions) {
        if (s->solutionValue == solutionValue && s->hash == hash) {
            result = s->solutionValue;
            return true;
        }
    }
    result = 0;
    return false;
}