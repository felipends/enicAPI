#ifndef PERTURBATION_H
#define PERTURBATION_H

#include <iostream>
#include "IPertubation.h"

using namespace std;

class Perturbation {
    private:
        vector<IPertubation*> *perturbations;
        static Perturbation* instance;
        Perturbation();

    public:        
        static Perturbation* getInstance();
        void perturb(vector<Cluster*> *clusters);
        ~Perturbation();
        
};

#endif