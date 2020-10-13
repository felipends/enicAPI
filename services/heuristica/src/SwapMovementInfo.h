#ifndef CLASS_SWAP_MOVEMENT_INFO_H
#define CLASS_SWAP_MOVEMENT_INFO_H

#include <iostream>
#include <vector>
class MovementInfo;

using namespace std;
class SwapMovementInfo {
    private:
        vector<MovementInfo*> movements;
        int value;
    public:
        SwapMovementInfo();
        SwapMovementInfo(MovementInfo *movement1, MovementInfo *movement2, int value);
        vector<MovementInfo *> &getMovements();
        int getValue();

};

#endif