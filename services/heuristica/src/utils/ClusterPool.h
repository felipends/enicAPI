#ifndef CLUSTER_POOL_H
#define CLUSTER_POOL_H

#include <iostream>
#include <unordered_set>
#include <vector>

#include "../Cluster.h"
#include "ClusterHash.h"

using namespace std;

class ClusterPool {
    private:
        unordered_set<Cluster *, ClusterHash, ClusterHash> pool;
    public:
        ~ClusterPool();
        int addClustersIfNotInHash(vector<Cluster*> *clusters);
        vector<Cluster*> *getAllClusters();
};

#endif