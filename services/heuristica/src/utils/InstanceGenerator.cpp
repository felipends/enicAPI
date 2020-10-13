#include "InstanceGenerator.h"

EventInfo* InstanceGenerator::generate() {
    vector<Session*>* sessions = generateSessions();
    vector<Paper*>* papers = generatePapers();
    vector<vector<int>> dummyValue;
    return new EventInfo(0, *sessions, *papers, topics, authors, dummyValue);
}

vector<Topic*>* InstanceGenerator::generateTopics(int numTopics) {
    vector<Topic*> *topics = new vector<Topic*>();
    int startIndex = this->topics.size();
    int stopIndex = startIndex + numTopics;
    for (int i = startIndex; i < stopIndex; i++) {
        string name = "Topic " + i;
        topics->push_back(new Topic(i, name));
    }
    for (Topic* t : *topics) {
        this->topics.push_back(t);
    }

    return topics;
}

vector<Author*>* InstanceGenerator::generateAuthors(int numAuthors) {
    vector<Author*>* authors = new vector<Author*>();
    int startIndex = this->authors.size();
    int stopIndex = startIndex + numAuthors;
    for (int i = startIndex; i < stopIndex; i++) {
        string name = "Author " + i;
        authors->push_back(new Author(i, name, "UFPB"));
    }
    for (Author* a : *authors) {
        this->authors.push_back(a);
    }
    return authors;
}

vector<Session*>* InstanceGenerator::generateSessions() {
    vector<Session*>* sessions = new vector<Session*>();

    DateTime* dateTime = new DateTime(1, 1, 2018, 8, 0, 0);
    sessions->push_back(new Session(0, 3, 20, dateTime));

    dateTime = new DateTime(1, 1, 2018, 9, 0, 0);
    sessions->push_back(new Session(1, 3, 20, dateTime));

    return sessions;
}

vector<Paper*>* InstanceGenerator::generatePapers() {
    vector<Paper*>* papers = new vector<Paper*>();    
    vector<Topic*> *topics1 = generateTopics(3);

    int id = 0;
    papers->push_back(makePaper(id++, generateAuthors(2), topics1));    
    papers->push_back(makePaper(id++, generateAuthors(2), topics1));
    papers->push_back(makePaper(id++, generateAuthors(2), topics1));

    vector<Topic*> *topics2 = generateTopics(3);    
    
    papers->push_back(makePaper(id++, generateAuthors(2), topics2));
    papers->push_back(makePaper(id++, generateAuthors(2), topics2));    
    papers->push_back(makePaper(id++, generateAuthors(2), topics2));

    return papers;
}

Paper* InstanceGenerator::makePaper(int index, vector<Author*>* authors, vector<Topic*>* topics) {
    string title = "Title " + index;
    string jemsId = "" + index;
    return new Paper(index, jemsId, title, topics, authors);
}