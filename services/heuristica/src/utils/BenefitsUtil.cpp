#include "BenefitsUtil.h"
#include <stdexcept>

vector<vector<int>> BenefitsUtil::benefits;

int BenefitsUtil::getBenefit(int paperIndex1, int paperIndex2) {

    if (paperIndex1 == paperIndex2) {
        throw std::invalid_argument("The papers have the same index: "
             + to_string(paperIndex1) + "==" + to_string(paperIndex2));
    }
    return BenefitsUtil::benefits[paperIndex1][paperIndex2];
}

int BenefitsUtil::getBenefit(Paper* paper1, Paper* paper2) {
    return getBenefit(paper1->index, paper2->index);
}

void BenefitsUtil::resizeVectors(int size) {    
    BenefitsUtil::benefits.resize(size);
    for (auto &v : BenefitsUtil::benefits) 
        v.resize(size);
}

void BenefitsUtil::calculate(vector<Paper *> *papers) {    
    int size = papers->size();
    resizeVectors(size);
    // O índex dos papers está sendo inicializado na classe EventInfo
    // Calcula os benefícios e salva em uma matriz simétrica, ou seja v[i][j] == v[j][i];
    for (int i = 0; i < size; i++) {
        Paper* paperI = papers->at(i);

        for (int j = i + 1; j < size; j++) {            
            Paper* paperJ = papers->at(j);

            int benefit = paperI->calculateBenefit(*paperJ);

            BenefitsUtil::benefits[paperI->index][paperJ->index] = benefit;
            BenefitsUtil::benefits[paperJ->index][paperI->index] = benefit;
        }
    }
}

void BenefitsUtil::addPreferences(vector<vector<int>> pairs) {
    for (int i = 0; i < pairs.size(); i++) {
        vector<int> pair = pairs[i];
        int paper1 = pair[0];
        int paper2 = pair[1];
        BenefitsUtil::benefits[paper1][paper2] = 1000;
        BenefitsUtil::benefits[paper2][paper1] = 1000;
        cout << "Benefício maximizado entre os papers " << paper1 << " e " << paper2 << endl;
    }
}