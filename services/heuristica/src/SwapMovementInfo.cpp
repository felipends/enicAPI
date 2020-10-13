#include "SwapMovementInfo.h"
#include "MovementInfo.h"
SwapMovementInfo::SwapMovementInfo() {}

SwapMovementInfo::SwapMovementInfo(MovementInfo *movement1, MovementInfo *movement2, int value) {
    movements.push_back(movement1);
    movements.push_back(movement2);
    this->value = value;
}

vector<MovementInfo*> &SwapMovementInfo::getMovements() {
    // cout << "SwapMovement " << movements[0]->getCluster()->index << endl;
    // cout << "SwapMovement " << movements[1]->getCluster()->index << endl;
    // cout << "SwapMovement value: " << value;
    return movements;
}

int SwapMovementInfo::getValue() {
    return value;
}
