#include "ClustersInfo.h"
#include "models/Paper.h"
#include "./utils/delete_vector.h"

vector<Cluster*> *ClustersInfo::clusters;
vector<vector<Integer*>> ClustersInfo::insertMovements;
vector<vector<SwapMovementInfo*>> ClustersInfo::swapMovements;
int ClustersInfo::swapMovementsSize;
bool ClustersInfo::created = false;
int ClustersInfo::numPapers;
int ClustersInfo::numClusters;

void ClustersInfo::Configure(int numPapers, int numClusters, vector<Cluster*> *clusters) {    
    ClustersInfo::numPapers = numPapers;
    ClustersInfo::numClusters = numClusters;

    if (!created) {        
        for (int i = 0; i < numPapers; i++)
            insertMovements.push_back(vector<Integer*>(numClusters));
    }
    
    for (int i = 0; i < numPapers; i++) {
        for (int j = 0; j < numClusters; j++) {
            if (created && insertMovements[i][j] != NULL) {
                delete insertMovements[i][j];
            }
            insertMovements[i][j] = NULL;
        }
    }    
   
    for (int i = 0; i < numPapers; i++) {
        if (!created) 
            swapMovements.push_back(vector<SwapMovementInfo*>(numPapers));
        for (int j = 0; j < numPapers; j++) {
            if (created && swapMovements[i][j] != NULL)
                delete swapMovements[i][j];
            swapMovements[i][j] = NULL;
        }
    }

    if (!created)
        created = true;
   
    swapMovementsSize = numPapers;
    ClustersInfo::clusters = clusters;
}

void ClustersInfo::deleteInsertMovement(int paperIndex, int clusterIndex) {
    if (insertMovements[paperIndex][clusterIndex] != NULL) {
        delete insertMovements[paperIndex][clusterIndex];
        insertMovements[paperIndex][clusterIndex] = NULL;
    }
}

void ClustersInfo::setSwapMovement(int paperIndex1, int paperIndex2, SwapMovementInfo* swapMoveInfo) {
    if (ClustersInfo::swapMovements[paperIndex1][paperIndex2] != NULL) {
        delete ClustersInfo::swapMovements[paperIndex1][paperIndex2];
    }
    ClustersInfo::swapMovements[paperIndex1][paperIndex2] = swapMoveInfo;
}

void ClustersInfo::setInsertMovement(int paperIndex, int clusterIndex, int value) {
    if (insertMovements[paperIndex][clusterIndex] != NULL) {
        insertMovements[paperIndex][clusterIndex]->value = value;
    } else {        
        delete insertMovements[paperIndex][clusterIndex];
        insertMovements[paperIndex][clusterIndex] = new Integer(value);
    }
}

int ClustersInfo::getInsertMovement(int paperIndex, int clusterIndex) {
    if (insertMovements[paperIndex][clusterIndex] == NULL) {
        cout << "insertMovement is NULL: " << paperIndex << ", " << clusterIndex << endl;
        abort();
    }
    return insertMovements[paperIndex][clusterIndex]->value;
}

void ClustersInfo::printSwapMoves() {
    for (int i = 0; i < swapMovementsSize; i++) {
        for (int j = 0; j < swapMovementsSize; j++) {
            cout << (swapMovements[i][j] != NULL ? swapMovements[i][j]->getValue() : -1)<< " ";
        }
        cout << endl;
    }
    cout << endl;
}

void ClustersInfo::printInsertMoves() {
    for (int i = 0; i < numPapers; i++) {
        for (int j = 0; j < numClusters; j++) {
            if (insertMovements[i][j] == NULL)
                cout << "- ";
            else 
                cout << insertMovements[i][j]->value << " ";
        }
        cout << endl;
    }
    cout << endl;
}

void ClustersInfo::clearData() {
    for (int i = 0; i < numPapers; i++) {
        for (int j = 0; j < numClusters; j++) {
            if (insertMovements[i][j] != NULL) {
                delete insertMovements[i][j];
            }
            insertMovements[i][j] = NULL;
        }
    }    
   
    for (int i = 0; i < numPapers; i++) {        
        for (int j = 0; j < numPapers; j++) {
            if (swapMovements[i][j] != NULL)
                delete swapMovements[i][j];
            swapMovements[i][j] = NULL;
        }
    }
}