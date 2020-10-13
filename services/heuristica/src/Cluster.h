#ifndef CLASS_CLUSTER_H
#define CLASS_CLUSTER_H

#include <iostream>
#include <vector>
#include <algorithm>
#include "MovementInfo.h"
#include "models/Paper.h"
#include "ClusterValue.h"
#include "utils/BenefitsUtil.h"

using namespace std;

class Cluster {
    private:
        int id;    
        vector<Paper *> papers;        
        int clusterSize;
        int minSize;
        vector<ClusterValue *> clusterValues;
        bool dummyCluster;
        int intraClusterValue;
		int numExtraProfs;
		int numGaps;
        
        bool checkLimits(int numMinPapers);

    public:
        Cluster();
        ~Cluster();
        Cluster(int intraClustervalue, int index, int clusterSize, bool dummyCluster, vector<Paper* >* papers, int minSize);
        Cluster(int clusterSize, int minSize, int index);
        Cluster(int index, vector<Paper*> *papers, bool dummyCluster, int minSize, int clusterSize);
        int index;
        const int dummyClusterValue = -10000;
        
        static Cluster* createDummyClusuter(int index, vector<Paper*> *papers);

        // sets and gets
        void setClusterSize(int clusterSize);
        int getClusterSize();
        static int sumClustersValues(vector<Cluster*> *clusters);

		void setNumExtraProfs(int numExtraProfs){ this->numExtraProfs = numExtraProfs; }
		void setNumGaps(int numGaps){ this->numGaps = numGaps; }
		int getNumExtraProfs(){ return this->numExtraProfs; }
		int getNumGaps(){ return this->numGaps; }

        int calculateBenefitOfInsertPaper(Paper *paper, bool returnNegativeIfEmpty);
        void add(Paper* paper);
        void removePaper(Paper* paper);
        bool isClusterViable();
        bool canRemoveFromCluster();
        int getClusterValue();
        int clusterValueOrDummyCluster();
        bool isNotFull();
        bool isEmpty();
        void clearPapers();
        vector<MovementInfo*> *calculateRemoveMovement();
        int calculatePaperRemoveValue(Paper* paper);
        Cluster* copy();
        string generateHash();
        string toString();
        bool isDummyCluster();
        vector<Paper*> *getPapers();
        bool hasPaper(Paper* paper);
        int getNumTopicOccurences(Topic* topic);
        int getIntraClusterValue();
        int hasAuthorConstraint(int day, int slot);
};


#endif 
