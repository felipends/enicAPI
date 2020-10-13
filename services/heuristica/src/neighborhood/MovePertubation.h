#ifndef MOVEPERTUBATION_H
#define MOVEPERTUBATION_H

#include <cstdlib>
#include "./IPertubation.h"

class MovePertubation : public IPertubation {    
    public:
        void perturb(vector<Cluster*> *clusters, int numPertubations);
};

#endif