#include "TimeSlot.h"
#include <algorithm>
#include <sstream>
#include <unordered_map>
#include "./utils/delete_vector.h"

TimeSlot::TimeSlot(int id, int day, Session* session) {
    this->id = id;
    this->day = day;
    this->numPapers = session->getNumPapers();
    this->sessions.push_back(session);    
    this->dateTimeInit = session->getDateTimeInit();
} 

TimeSlot::TimeSlot(int id, int day, int timeSlot, Session* session) {
    this->id = id;
    this->day = day;
    this->timeSlot = timeSlot;
    this->numPapers = session->getNumPapers();
    this->sessions.push_back(session);    
    this->dateTimeInit = session->getDateTimeInit();
}

vector<int> TimeSlot::GetSessionsSize() {
    vector<int> sizes;
    for (Session* s : sessions) {
        sizes.push_back(s->getNumPapers());
    }
    return sizes;
}

bool TimeSlot::clusterCanEnter(Cluster* cluster) {
    int maxNumPapers = 0;
    for (Session* s : sessions) {
        int numPapers = s->getNumPapers();
        if (numPapers > maxNumPapers) 
            maxNumPapers = numPapers;
    }
    return cluster->getPapers()->size() <= maxNumPapers;
}

void TimeSlot::addSession(Session* session) {
    numPapers += session->getNumPapers();
    sessions.push_back(session);
}

Session* TimeSlot::getSession(int index) {
    return sessions.at(index);
}

int TimeSlot::getDay() {
    return day;
}

DateTime* TimeSlot::getDateTimeInit() {
    return dateTimeInit;
}

string TimeSlot::toString() {        
    sortSessionsOrderbyNumPapersDesc();    
    sortClustersOrderByNumPapersDesc();    
    stringstream ss;
    ss << "=== Time Slot " << id << "===" << endl;    
    ss << "Date: " << sessions.at(0)->getDateTimeInit()->toString() << endl;
    
    int size = clusters.size();
    for (int i = 0; i < size; i++) {
        Session* session = sessions.at(i);
        Cluster* cluster = clusters.at(i);
        Topic* clusterTopic = getTopicFromCluster(cluster);

        ss << "Session " << (i+1) << "." << " Capacity: " << session->getNumPapers() << " - ";
        ss << "Cluster Size: " << cluster->getPapers()->size() << " - value: " << cluster->getClusterValue() << endl;
        ss << "Topic: " << clusterTopic->getName() << endl;

        int paperIndex = 0;
        for (Paper* paper : *(cluster->getPapers())) {
            ss << ++paperIndex << ". Paper " << paper->index << "(" << paper->getJemsId() << ") -";
            Topic *lastTopic = paper->getTopics()->at(paper->getTopics()->size()-1);
            for (Topic* t : *(paper->getTopics())) {
                ss << t->getName();
                if (t != lastTopic) {
                    ss << ", ";
                } 
            }

            Author *lastAuthor = paper->getAuthors()->at(paper->getAuthors()->size()-1);
            ss << "[";
            for (Author* a : *(paper->getAuthors())) {
                ss << a->getName();
                if (a != lastAuthor) {
                    ss << ", ";
                }
            }
            ss << "]";
            ss << "\n" << endl;
        }
    }
    return ss.str();
}



void TimeSlot::sortSessionsOrderbyNumPapersDesc() {
    sort(sessions.begin(), sessions.end(), 
        [](Session* s1, Session* s2) -> bool {
            return s1->getNumPapers() < s2->getNumPapers();
        });
}

void TimeSlot::sortClustersOrderByNumPapersDesc() {
    sort(clusters.begin(), clusters.end(), 
        [](Cluster* c1, Cluster* c2) -> bool {
            return c1->getPapers()->size() < c2->getPapers()->size();
        });
}

Topic* TimeSlot::getTopicFromCluster(Cluster* cluster) {
    unordered_map<int, int> umap;
    vector<Paper*>* papers = cluster->getPapers();
    vector<Topic*> topicsInCluster;
    for (Paper* p : *papers) {
        vector<Topic*>* topics = p->getTopics();
        for (Topic* t : *topics) {
            //cout << t->getId() << ", " << t->getName() << endl;
            topicsInCluster.push_back(t);
            if (umap.find(t->getId()) == umap.end()) {
                umap[t->getId()] = 1;
            } else {
                umap[t->getId()] += 1;
            }
        }
    }
    
    int indexMostUsedTopic = 0;
    for (auto itr = umap.begin(); itr != umap.end(); itr++) {
        //cout << "topic: " << itr->first << " = " << itr->second << endl;
        if (itr->second > indexMostUsedTopic) {
            indexMostUsedTopic = itr->first;
        }            
    }
    Topic *topicMostUsed;
    for (Topic *t : topicsInCluster) {
        if (t->getId() == indexMostUsedTopic) {
            topicMostUsed = t;
            break;
        }
    }
    return topicMostUsed;
}

void TimeSlot::addCluster(Cluster *cluster) {
    clusters.push_back(cluster);
}

int TimeSlot::getNumSessions() {
    return sessions.size();
}

int TimeSlot::getNumPapers() {
    return numPapers;
}

void TimeSlot::deleteClusters() {
    deleteVectorElements(&clusters);
}

string TimeSlot::getSolution() {
    sortSessionsOrderbyNumPapersDesc();    
    sortClustersOrderByNumPapersDesc();    

    stringstream ss;
    int size = clusters.size();
    for (int i = 0; i < size; i++) {        
        ss << day << " " << timeSlot << " ";

        Cluster* cluster = clusters.at(i);    
        ss << cluster->getPapers()->size() << " ";
        
        int paperIndex = 0;
        for (Paper* paper : *(cluster->getPapers())) {
            ss << paper->index << " ";
        }

        ss << endl;
    }
    return ss.str();
}

void TimeSlot::clustersToSessions()
{
	int numClusters = clusters.size();
	for(int i = 0; i < numClusters; i++) {
		for (Paper* p: *(clusters[i]->getPapers())) {
			sessions.at(i)->setPapersInSolution(p->index);
		}
		sessions.at(i)->setNumExtraProfs(clusters[i]->getNumExtraProfs());
		sessions.at(i)->setNumGaps(clusters[i]->getNumGaps());
	}
}

void TimeSlot::updateCluster(int index, int numGaps, int numExtraProfs)
{
	int numClusters = clusters.size();
	for(int i = 0; i < numClusters; i++) {
		if(clusters[i]->index == index) {
			clusters[i]->setNumGaps(numGaps);
			clusters[i]->setNumExtraProfs(numExtraProfs);
		}
	}
}
