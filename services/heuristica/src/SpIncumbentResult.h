#ifndef SP_INCUMBENT_RESULT_H
#define SP_INCUMBENT_RESULT_H

#include <iostream>

class SpIncumbentResult {
    private:
        double value;        
    
    public:
        SpIncumbentResult();
        double getValue();
        void setValue(double value);
};

#endif