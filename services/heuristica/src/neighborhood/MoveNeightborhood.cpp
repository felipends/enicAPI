#include "./MoveNeightborhood.h"
#include "../ClustersInfo.h"

MoveNeightborhood::MoveNeightborhood(vector<MovementInfo*> *movementsInfo) {
    this->movementsInfo = movementsInfo;
}

void MoveNeightborhood::findBestNeightbor() {
    bestMovementInfo = NULL;
    for (auto m : *movementsInfo) {
        m->calculateBestPaperMovement();
    }     
    
    bestMovementInfo = movementsInfo->at(0);
    int size = movementsInfo->size();
    for (int i = 1; i < size; i++) {
        if(movementsInfo->at(i)->getBestMovementValue() > bestMovementInfo->getBestMovementValue()) {
            bestMovementInfo = movementsInfo->at(i);
        }
    }
}

bool MoveNeightborhood::isObjectiveFunctionBetter() {
    int currentObjValue = MovementInfo::getCurrentObjectiveValue();
    int newObjValue = currentObjValue + bestMovementInfo->getBestMovementValue();
    return newObjValue > currentObjValue;
}

void MoveNeightborhood::updateNeightboorhood() {
    bestMovementInfo->makeMoveMovement(movementsInfo);

    // string removeMovement = MovementInfo::removeValueToString(movementsInfo);
    // string swapMovement = MovementInfo::swapTableToString();
    // string insertMovement = MovementInfo::insertTableToString();

    // vector<MovementInfo*> moves;
    // for (Cluster *c : *ClustersInfo::clusters) {
    //     vector<MovementInfo*>* clusterMovements = c->calculateRemoveMovement();
    //     for (MovementInfo* m : *clusterMovements) {
    //         moves.push_back(m);
    //     }
    // }

    // for (MovementInfo* m : *movementsInfo) {
    //     m->calculateInsertMovements();
    // }
    // for (MovementInfo* m : *movementsInfo) {
    //     m->calculateSwapMovements(movementsInfo);
    // }

    // string removeMovement2 = MovementInfo::removeValueToString(&moves);
    // if (removeMovement.compare(removeMovement2) != 0) {
    //     cout << "divergência remove (move): " << endl;
    //     cout << removeMovement << endl;
    //     cout << removeMovement2 << endl;
    //     abort();
    // }

    // string insertMovement2 = MovementInfo::insertTableToString();
    // if (insertMovement.compare(insertMovement2) != 0) {
    //     cout << "divergência insert (move): " << endl;
    //     cout << insertMovement << endl;
    //     cout << insertMovement2 << endl;
    //     abort();
    // }

    // string swapMovement2 = MovementInfo::swapTableToString();
    // if (swapMovement.compare(swapMovement2) != 0) {
    //     cout << swapMovement << endl;
    //     cout << swapMovement2 << endl;
    //     cout << "divergência swap (move): " << endl;
    //     abort();
    // }
}
