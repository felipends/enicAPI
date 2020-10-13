#ifndef MOVENEIGHTBORHOOD_H
#define MOVENEIGHTBORHOOD_H

#include <vector>

#include "../MovementInfo.h"
#include "./INeighborhood.h"

class MoveNeightborhood : public INeighborhood {
    private:
        vector<MovementInfo*> *movementsInfo;
        MovementInfo *bestMovementInfo;
    
    public:
        MoveNeightborhood(vector<MovementInfo*> *movementsInfo);
        void findBestNeightbor();
        bool isObjectiveFunctionBetter();
        void updateNeightboorhood();
};

#endif