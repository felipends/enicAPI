#ifndef CONTIGUOUS_CLUSTERING_ORTOOLS_H
#define CONTIGUOUS_CLUSTERING_ORTOOLS_H

#include <vector> 
#include <ortools/linear_solver/linear_solver.h>
#include "Cluster.h"
#include "models/Paper.h"
#include "models/EventInfo.h"
#include "models/ContiguousCluster.h"

using namespace std;
using namespace operations_research;

class ContiguousClusteringOrTools {
	private:
		EventInfo* event;
		Cluster* cluster;
		vector<Cluster*>* solutionClusters;
		int numClustersInSolution;

		vector<int>* professorsIndices;
		vector<Author*> allProfesors;

		void setSolutionClusters(vector<vector<vector<MPVariable*>>>& w, int numPapers, vector<Paper*>& papers);

	public:
		ContiguousClusteringOrTools(vector<Author*>& allProfesors, vector<int>* professorsIndices, EventInfo* event, Cluster* cluster, int numClustersInSolution);
		~ContiguousClusteringOrTools();

		void splitCluster();
};

#endif
