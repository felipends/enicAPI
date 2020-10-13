#include "ClusterBenefit.h"

ClusterBenefit::ClusterBenefit(int benefit, Cluster* cluster) { 
    this->benefit = benefit;
    this->cluster = cluster;
}

bool ClusterBenefit::orderDescending(ClusterBenefit* c1, ClusterBenefit* c2) {    
    return c1->benefit > c2->benefit;
}
