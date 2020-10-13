#ifndef PROFESSORS_MODEL_H
#define PROFESSORS_MODEL_H

#include "models/Paper.h"
#include "Cluster.h"
#include <vector>
#include "../../../../or-tools/ortools/linear_solver/linear_solver.h"

using namespace std;
using namespace operations_research;

class ProfessorsModel {
private:
    Cluster* cluster;
    vector<int> clusterTopics;

    int numSlots;
    int numProfessors;
    int numExtraProfessorsNeeded;
    vector<int> professorsId;
    vector<pair<int, vector<int>>> professorPapers;
    vector<Paper*> papers;
    vector<Author*> authors;

    int chooseExtraProfessor(vector<Author*>& allAuthors, vector<int>* extraIndices);
    void setClusterTopics();
    void setExaminers(vector<vector<MPVariable*>>& x, vector<vector<MPVariable*>>& y);

public:
    ProfessorsModel(Cluster* cluster, vector<Author*>& allAuthors, vector<int>* extraIndices);
    ~ProfessorsModel();

    int getNumExtraProfessorsNeeded();

    int allocProfessorsModel();
};

#endif
