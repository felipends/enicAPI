#ifndef INSTANCEHEADER_H
#define INSTANCEHEADER_H

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <memory>

#include "../models/EventInfo.h"
#include "../models/Topic.h"
#include "../models/Author.h"
#include "../models/Paper.h"
#include "../models/Session.h"

using namespace std;

class InstanceReader {
    private:
        fstream* filestream;
        int idFirstTopicId;
        int idFirstAuthorId;
        int idFirstPaperId;

        vector<string> papersInfo;

        vector<Topic*> topics;
        vector<Author*> authors;
        vector<int> extraProfessorsIndices; 
        vector<Paper*> papers;
        vector<Session*> sessions;
        vector<vector<int>> pairs;

        void parseTopics();
        void parseAuthors();
        void parsePapers();
        void parseSessions();
        void parseAuthorConstraints();
        void readPairs();
        void parseExtraProfessors();
    public:
        vector<string> getPapersInfo();
		vector<int>* getExtraProfessorsIndices();
		vector<Author*>* getAuthors() { return &(this->authors); }
        InstanceReader(string filename);
        ~InstanceReader();
        EventInfo* readInstance();
};

#endif
