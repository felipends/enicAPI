
#ifndef TIMESLOT_H 
#define TIMESLOT_H

#include <iostream>
#include <vector>
#include "models/Session.h"
#include "utils/DateTime.h"
#include "Cluster.h"

using namespace std;

class TimeSlot {
    private:
        int id;
        int day;
        int timeSlot;
        DateTime* dateTimeInit;
        vector<Session*> sessions;
        vector<Cluster*> clusters;
        void sortSessionsOrderbyNumPapersDesc();
        void sortClustersOrderByNumPapersDesc();
        Topic* getTopicFromCluster(Cluster* cluster);
        int numPapers;

    public:
		
		void clustersToSessions();

        TimeSlot(int id, int day, Session* session);
        TimeSlot(int id, int day, int timeSlot, Session* session); 
        vector<int> GetSessionsSize();
        bool clusterCanEnter(Cluster* cluster);
        void addSession(Session* session);
        int getDay();
        string toString();
        DateTime* getDateTimeInit();        
        Session* getSession(int index);
        void addCluster(Cluster *cluster);
		void updateCluster(int index, int numGaps, int numExtraProfs);
        int getNumSessions();
		int getNumClusters() { return clusters.size(); }
        int getNumPapers();
        void deleteClusters();
        string getSolution();
};

#endif
