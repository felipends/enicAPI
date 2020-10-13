#ifndef CLUSTERBENEFIT_H
#define CLUSTERBENEFIT_H

#include "../Cluster.h"

struct ClusterBenefit {
    int benefit;
    Cluster* cluster;

    ClusterBenefit(int benefit, Cluster* cluster);
    static bool orderDescending(ClusterBenefit* c1, ClusterBenefit* c2);
};

#endif