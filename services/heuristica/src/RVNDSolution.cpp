#include "./RVNDSolution.h"
#include <algorithm>
#include <sstream>

RVNDSolution::RVNDSolution(vector<Cluster*> *clusters, int solutionValue) {
    this->solutionValue = solutionValue;
    this->hash = RVNDSolution::generateHash(clusters);
}

string RVNDSolution::generateHash(vector<Cluster*> *clusters) {
    vector<string> hashes;
    for(Cluster* c : *clusters) {
        hashes.push_back(c->generateHash());
    }
    sort(hashes.begin(), hashes.end());
    
    stringstream ss;
    for (auto it = hashes.begin(); it != hashes.end(); it++) {
       if (it != hashes.end()) {           
           ss << *it << ":";
       } else {
           ss << *it;
       }
    }
    return ss.str();
}