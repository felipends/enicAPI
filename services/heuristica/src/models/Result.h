#ifndef HEURISTIC_RESULT_H
#define HEURISTIC_RESULT_H

class Result {
    private:
        double value;
        bool solutionFound;

    public:
        Result(bool solutionFound, double value);
        bool isSolutionFound();
        double getValue();
};

#endif