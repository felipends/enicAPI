#ifndef CLASS_CLUSTER_INFO_H
#define CLASS_CLUSTER_INFO_H

#include <iostream>
#include <vector>
#include "Cluster.h"
#include "neighborhood/SwapNeightborhood.h"
#include "utils/Integer.h"

using namespace std;

class ClustersInfo
{
    private:
        static bool created;
        static int numPapers;
        static int numClusters;
        static vector<vector<Integer*>> insertMovements;
    public:
        static vector<Cluster*> *clusters;
        static vector<vector<SwapMovementInfo*>> swapMovements;
        static int swapMovementsSize;
        static void Configure(int numPapers, int numClusters, vector<Cluster*> *clusters);
        static void setInsertMovement(int paperIndex, int clusterIndex, int value);
        static void setSwapMovement(int paperIndex1, int paperIndex2, SwapMovementInfo* swapMoveInfo);
        static void deleteInsertMovement(int paperIndex, int clusterIndex);
        static int getInsertMovement(int paperIndex, int clusterIndex);
        static void printSwapMoves();
        static void printInsertMoves();
        static void clearData();
};

#endif