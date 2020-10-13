#ifndef INSTANCEGENERATOR_H
#define INSTANCEGENERATOR_H

#include <iostream>
#include "../models/EventInfo.h"
#include "../models/Session.h"
#include "../models/Paper.h"
#include "../models/Topic.h"
#include "../models/Author.h"
#include "DateTime.h"

using namespace std;

class InstanceGenerator {
    private:
        vector<Topic*> topics;
        vector<Author*> authors;
        
        vector<Topic*>* generateTopics(int numTopics);
        vector<Author*>* generateAuthors(int numAuthors);
        vector<Session*>* generateSessions();
        vector<Paper*>* generatePapers();
        Paper* makePaper(int index, vector<Author*>* authors, vector<Topic*>* topics);

    public:    
        EventInfo* generate();
};

#endif