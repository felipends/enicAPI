#include "ClusterValue.h"
#include "utils/BenefitsUtil.h"

ClusterValue::ClusterValue(Paper *paper1, Paper *paper2) {
    this->paper1 = paper1;
    this->paper2 = paper2;
    value = BenefitsUtil::getBenefit(paper1, paper2);    
}

bool ClusterValue::hasPaper(Paper *paper) {
    return this->paper1->index == paper->index 
        || this->paper2->index == paper->index;
}

int ClusterValue::getValue() {    
    return value;
}
