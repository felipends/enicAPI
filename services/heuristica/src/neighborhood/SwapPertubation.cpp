#include <cstdlib>
#include "./SwapPertubation.h"

void SwapPertubation::perturb(vector<Cluster*> *clusters, int numPertubations) {    
    for (int i = 0; i < numPertubations; i++) {
        // It's only possible make the swap between two not empty clusters (clusters with papers)        
        vector<Cluster*> clustersNotEmpty;
        for(Cluster* c : *clusters) {
            if (!c->isEmpty()) 
                clustersNotEmpty.push_back(c);
        }
        int numClustersNotEmpty = clustersNotEmpty.size();

        int randomClusterIndex1, randomClusterIndex2;
        do
        {
            // selects two random clusters
            randomClusterIndex1 = rand() % numClustersNotEmpty;
            randomClusterIndex2 = rand() % numClustersNotEmpty;
        } while (randomClusterIndex1 == randomClusterIndex2);

         // select a random paper from each cluster
        int paperIndex1 = rand() % clustersNotEmpty[randomClusterIndex1]->getPapers()->size();
        int paperIndex2 = rand() % clustersNotEmpty[randomClusterIndex2]->getPapers()->size();

        Paper* paperFromCluster1 = clustersNotEmpty[randomClusterIndex1]->getPapers()->at(paperIndex1);
        Paper* paperFromCluster2 = clustersNotEmpty[randomClusterIndex2]->getPapers()->at(paperIndex2);
        
        // remove the selected papers from the clusters
        clustersNotEmpty[randomClusterIndex1]->removePaper(paperFromCluster1);
        clustersNotEmpty[randomClusterIndex2]->removePaper(paperFromCluster2);

        // insert papers into the clusters
        clustersNotEmpty[randomClusterIndex1]->add(paperFromCluster2);
        clustersNotEmpty[randomClusterIndex2]->add(paperFromCluster1);
    }
}