#include "ClusterPool.h"
#include "delete_vector.h"

ClusterPool::~ClusterPool() {
    deleteVector(getAllClusters());
}

int ClusterPool::addClustersIfNotInHash(vector<Cluster*> *clusters) {
    vector<Cluster*> newClusters;
    for (auto c : *clusters) {
        newClusters.push_back(c->copy());
    }

    newClusters.pop_back(); // remove the dummy cluster

    int numAddedClusters = 0;
    for (auto c : newClusters) {
        auto res = pool.insert(c);
        if (res.second)
            numAddedClusters++;
    }

    return numAddedClusters;
}

vector<Cluster*> *ClusterPool::getAllClusters() {
    auto clusters = new vector<Cluster*>();
    clusters->insert(clusters->end(), pool.begin(), pool.end());
    return clusters;
}