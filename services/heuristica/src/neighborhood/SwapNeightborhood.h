#ifndef SWAPNEIGHTBORHOOD_H
#define SWAPNEIGHTBORHOOD_H

#include <vector>

#include "../MovementInfo.h"
#include "../SwapMovementInfo.h"
#include "./INeighborhood.h"

class SwapNeightborhood : public INeighborhood {
    private:
        vector<MovementInfo*> *movementsInfo;
        SwapMovementInfo *bestSwapMovement;
    
    public:
        SwapNeightborhood(vector<MovementInfo*>* movementsInfo);
        void findBestNeightbor();
        bool isObjectiveFunctionBetter();
        void updateNeightboorhood();

};

#endif