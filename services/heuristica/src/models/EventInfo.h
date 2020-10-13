#ifndef EVENT_INFO_H
#define EVENT_INFO_H

#include <iostream>
#include <vector>

#include "Session.h"
#include "Paper.h"
#include "Topic.h"
#include "Author.h"

using namespace std;

class EventInfo {
    private:
        int eventId;

        vector<Session*> sessions;
        vector<Paper*> papers;
        vector<Topic*> topics;
        vector<Author*> authors;
        vector<vector<Paper*>> papersAuthor;
        vector<vector<Paper*>> papersTopic;
        vector<vector<int>> pairs;

        int numSessions;
        int numPapers;
        int numTopics;
        int numAuthors; 
        int numDays;

        int OFValue;

        void orderSessionsByDate();
        void initNumDays();

    public: 
        EventInfo(int eventId, vector<Session*> sessions, vector<Paper*> papers, 
            vector<Topic*> topics, vector<Author*> authors, vector<vector<int>> pairs);
        ~EventInfo();
        int getEventId();
        void printSessions();
        vector<Session*> &getSessions();
        vector<Paper*> &getPapers();
        vector<Topic*> &getTopics(); 
        vector<Author*> &getAuthors();
        vector<vector<Paper*>> &getPapersAuthor();
        vector<vector<Paper*>> &getPapersTopic();

        int getNumSessions();
        int getNumPapers();
        int getNumTopics();
        int getNumAuthors();
        int getNumDays();
        vector<vector<int>> getPairs();
        
        void setOFValue(int value);
        int getOFValue();
};


#endif
