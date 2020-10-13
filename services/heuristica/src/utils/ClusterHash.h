#ifndef CLUSTER_HASH_H
#define CLUSTER_HASH_H

#include <iostream>
#include <unordered_set>

#include "../Cluster.h"

using namespace std;

struct ClusterHash {
    public:
	    size_t operator()(Cluster* cluster) const;
        bool operator()(Cluster *c1, Cluster *c2) const;
};

#endif