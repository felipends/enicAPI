#ifndef CONTIGUOUS_CLUSTER
#define CONTIGUOUS_CLUSTER

#include <vector>
#include <algorithm>
#include "Paper.h"

using namespace std;

class ContiguousCluster {
private:
    int numPapers;
    int numProfessors;
    int prevClusterSize;

    vector<Paper*> papers;
    vector<Author*> professors;
    vector<Topic*> topics;
    vector<int> professorsInCluster;

    //auxiliar structures
    vector<int>* extraProfessorsIndices;
    vector<Author*> extraProfessors;

    void initializeData();
    void setTopics();
    void chooseExtraProfessors();

public:    
    ContiguousCluster(vector<Paper*>& clusterPapers);
    ContiguousCluster(vector<Author*>& allProfesors, vector<int>* professorsIndices, vector<Paper*>& clusterPapers, int clusterIndice);

    void makeClusterContiguous();

    ~ContiguousCluster();
};

#endif
