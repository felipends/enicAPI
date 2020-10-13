#include "ClusterHash.h"

size_t ClusterHash::operator()(Cluster* cluster ) const {    
    return hash<string>()(cluster->generateHash());
}

bool ClusterHash::operator()(Cluster* c1, Cluster* c2) const {
    int res = c1->generateHash().compare(c2->generateHash());
    return res == 0;
}