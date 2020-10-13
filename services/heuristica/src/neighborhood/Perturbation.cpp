#include "Perturbation.h"
#include "MovePertubation.h"
#include "SwapPertubation.h"
#include <cstdlib>

Perturbation* Perturbation::instance = NULL;

Perturbation::Perturbation() {
    perturbations = new vector<IPertubation*>();
    perturbations->push_back(new MovePertubation());
    perturbations->push_back(new SwapPertubation());
}

Perturbation* Perturbation::getInstance() {
    if (instance == NULL) {
        instance = new Perturbation();
    }
    return instance;
}

void Perturbation::perturb(vector<Cluster*> *clusters) {
    int index = rand() % perturbations->size();
    IPertubation* selectedPerturbation = perturbations->at(index);
    int numPertubations = 2 + (rand() % 4);
    selectedPerturbation->perturb(clusters, numPertubations);
}

Perturbation::~Perturbation() {
    for (auto it = perturbations->begin(); it != perturbations->end(); it++) {
        delete(*it);
    }
    perturbations->clear();
    delete perturbations;
}