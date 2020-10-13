#include "MovePertubation.h"

void MovePertubation::perturb(vector<Cluster*> *clusters, int numPertubations) {
    for (int i = 0; i < numPertubations; i++) {
        vector<Cluster*> clustersNotEmpty;
        vector<Cluster*> clustersNotFull;
        for (auto c : *clusters) {
            if (!c->isEmpty() && c->canRemoveFromCluster()) {
                clustersNotEmpty.push_back(c);
            }
            if (c->isNotFull() || c->isDummyCluster()) {
                clustersNotFull.push_back(c);
            }
        }        
        int numClustersNotEmpty = clustersNotEmpty.size();
        int numClustersNotFull = clustersNotFull.size();

        int randomClusterIndexNotEmpty, randomClusterIndexNotFull;
        Cluster* cluster1;
        Cluster *cluster2;
        if (numClustersNotEmpty > 0) {
            do {
                // selects two random clusters
                // the fist one is from a cluster's list that isn't empty
                // and the second one from a cluster's list that isn't full
                randomClusterIndexNotEmpty = rand() % numClustersNotEmpty;
                randomClusterIndexNotFull =  rand() % numClustersNotFull;

                cluster1 = clustersNotEmpty[randomClusterIndexNotEmpty];
                cluster2 = clustersNotFull[randomClusterIndexNotFull];            
            } while (cluster1->index == cluster2->index);

            // select a random paper from the first selected cluster and remove it
            
            int paperIndex =  rand() % clustersNotEmpty[randomClusterIndexNotEmpty]->getPapers()->size();
            Paper* selectedPaper = clustersNotEmpty[randomClusterIndexNotEmpty]->getPapers()->at(paperIndex);
            clustersNotEmpty[randomClusterIndexNotEmpty]->removePaper(selectedPaper);

            // insert the selected cluster in the second cluster
            clustersNotFull[randomClusterIndexNotFull]->add(selectedPaper);

        } else {      
            for (auto c : *clusters) {
                if (!c->isEmpty() && !c->isDummyCluster()) {
                    clustersNotEmpty.push_back(c);
                }
            }          
            // Selects a random paper and insert it into the dummy cluster            
            randomClusterIndexNotEmpty = rand() % clustersNotEmpty.size();
            // select the dummy cluster as destination
            int dummyClusterIndex = clusters->size() - 1;           
            randomClusterIndexNotFull = clusters->at(dummyClusterIndex)->index;

            vector<Paper*> *papers = clustersNotEmpty[randomClusterIndexNotEmpty]->getPapers();
            for (auto paper : *papers)
            {
                clustersNotEmpty[randomClusterIndexNotEmpty]->removePaper(paper);
                clusters->at(randomClusterIndexNotFull)->add(paper);
            }
        }
    }
}