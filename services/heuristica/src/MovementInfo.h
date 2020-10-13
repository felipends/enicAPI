#ifndef CLASS_MOVEMENT_INFO_H
#define CLASS_MOVEMENT_INFO_H

class Cluster;
#include "SwapMovementInfo.h"
#include <iostream>
#include <algorithm>
#include "models/Paper.h"
#include "ClustersInfo.h"

using namespace std;
/**
 * Esta classe representa os dados de um movimento de vizinhança, como benefício do movimento, melhor troca e etc.
 * O objeto Paper está no objeto Cluster em um dado momento.
 * */
class MovementInfo {
    private:
        Paper *paper;
        Cluster *cluster;
        int removeValue;
        int bestClusterIndexToMove; // Melhor índice do cluster (coluna) para o paper ser movido
        int bestMovementValue; // Benefício do melhor movimento de um paper para um cluster
        int bestSwapMovement; // Melhor benefício de troca
        int clusterToSwap; // Índice do cluster para o qual o paper deve ser movido na troca

    public:
        MovementInfo(Paper *paper, Cluster *cluster, int removeValue);
        static int getCurrentObjectiveValue();
        void calculateInsertMovements();
        void recalculateInsertMovementsToClusters(vector<Cluster*> *modifiedClusters, vector<MovementInfo*> *movementsInfo);
        void calculateBestPaperMovement();
        void makeMoveMovement(vector<MovementInfo*> *movementsInfo);
        void makeMoveMovement(vector<MovementInfo*> *movementsInfo, int clusterIndex);
        void movePaperToCluster(int clusterIndex);
        void updateSwapMovementsInsideCluster();
        void calculateSwapMovement(MovementInfo* movement);
        void calculateSwapMovements(vector<MovementInfo*> *movements);
        void recalculateSwapMovents(vector<MovementInfo*> *movements);
        static SwapMovementInfo* findBestSwapMovement();
        static void makeSwap(SwapMovementInfo* swapMovement, vector<MovementInfo*> *movementsInfo);
        static void makeSwapMovement(vector<MovementInfo*> *movements, vector<Paper*> *papers);

        // métodos para depuração
        static string removeValueToString(vector<MovementInfo*> *movements);
        static string swapTableToString();
        static string insertTableToString();
        // getters ad setters
        Paper *getPaper();
        void setPaper(Paper* paper);
        Cluster *getCluster();
        void setCluster(Cluster* cluster);
        int getRemoveValue();
        void setRemoveValue(int removeValue);                
        int getBestMovementValue();        
        int getBestSwapMovement();
        void setBestSwapMovement(int bestSwapMovement);
        int getClusterToSwap();
        void setClusterToSwap(int clusterToSwap);
        void updateRemoveValue();
        int getBestClusterIndexToMove();

        string toString();
};

#endif