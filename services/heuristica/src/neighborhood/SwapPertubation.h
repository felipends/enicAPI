#ifndef SWAPPERTUBATION_H
#define SWAPPERTUBATION_H

#include "./IPertubation.h"

class SwapPertubation : public IPertubation {
    void perturb(vector<Cluster*> *clusters, int numPertubations);
};

#endif