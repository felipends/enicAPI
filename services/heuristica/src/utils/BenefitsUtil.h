#ifndef CLASS_BENEFITSUTIL_H
#define CLASS_BENEFITSUTIL_H

#include <iostream>
#include <vector>
#include "../models/Paper.h"

using namespace std;

class BenefitsUtil {
    private:
        static vector<vector<int>> benefits;
        static void resizeVectors(int size);
    public:
        static int getBenefit(int paperIndex1, int paperIndex2);
        static int getBenefit(Paper* paper1, Paper* paper2);
        static void calculate(vector<Paper *> *paper);
        static void addPreferences(vector<vector<int>> pairs);
};


#endif