#ifndef IPERTUBATION_H
#define IPERTUBATION_H

#include <vector>
#include "../Cluster.h"

using namespace std;

class IPertubation {
    public:
        void virtual perturb(vector<Cluster*> *clusters, int numPertubations) = 0;
};

#endif