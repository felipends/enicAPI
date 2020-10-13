#ifndef CLASS_CLUSTERVALUE_H
#define CLASS_CLUSTERVALUE_H

#include <iostream>
#include <vector>
#include "models/Paper.h"

using namespace std;

class ClusterValue {
    private:
        Paper *paper1;
        Paper *paper2;
        int value;
    public:
        ClusterValue(Paper *paper1, Paper *paper2);
        bool hasPaper(Paper *paper);
        int getValue();
};

#endif
