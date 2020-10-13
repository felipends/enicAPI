#ifndef RVNDSOLUTION_H
#define RVNDSOLUTION_H

#include <iostream>
#include "./Cluster.h"

using namespace std;

class RVNDSolution {
    public:
        int solutionValue;
        string hash;

        RVNDSolution(vector<Cluster*> *clusters, int solutionValue);
        static string generateHash(vector<Cluster*> *clusters);
};

#endif