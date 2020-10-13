#ifndef NEIGHBORHOOD_H
#define NEIGHBORHOOD_H

class INeighborhood {
    public:
        void virtual findBestNeightbor() = 0;
        bool virtual isObjectiveFunctionBetter() = 0;
        void virtual updateNeightboorhood() = 0;
};

#endif