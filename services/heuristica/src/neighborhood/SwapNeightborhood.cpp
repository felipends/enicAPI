#include "./SwapNeightborhood.h"

SwapNeightborhood::SwapNeightborhood(vector<MovementInfo*> *movementsInfo) {
    this->movementsInfo = movementsInfo;    
}

void SwapNeightborhood::findBestNeightbor() {        
    bestSwapMovement = MovementInfo::findBestSwapMovement();    
}

bool SwapNeightborhood::isObjectiveFunctionBetter() {
    int currentObjValue = MovementInfo::getCurrentObjectiveValue();
    int newObjValue = currentObjValue + bestSwapMovement->getValue();
    return newObjValue > currentObjValue;
}

void SwapNeightborhood::updateNeightboorhood() {    
    MovementInfo::makeSwap(bestSwapMovement, movementsInfo);

    // string removeMovement = MovementInfo::removeValueToString(movementsInfo);
    // string swapMovement = MovementInfo::swapTableToString();
    // string insertMovement = MovementInfo::insertTableToString();
    // cout << insertMovement << endl;
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
    //     cout << "divergência remove (swap): " << endl;
    //     cout << removeMovement << endl;
    //     cout << removeMovement2 << endl;
    //     abort();
    // }

    // string insertMovement2 = MovementInfo::insertTableToString();
    // if (insertMovement.compare(insertMovement2) != 0) {
    //     cout << "divergência insert (swap): " << endl;
    //     cout << insertMovement << endl;
    //     cout << insertMovement2 << endl;
    //     abort();
    // }

    // string swapMovement2 = MovementInfo::swapTableToString();
    // if (swapMovement.compare(swapMovement2) != 0) {
    //     cout << swapMovement << endl;
    //     cout << swapMovement2 << endl;
    //     cout << "divergência swap (swap): " << endl;        
    //     abort();
    // }
}