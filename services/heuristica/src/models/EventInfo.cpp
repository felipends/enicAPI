#include "EventInfo.h"
#include <ctime>
#include <algorithm>
#include "../utils/DateTime.h"
#include "../utils/delete_vector.h"

EventInfo::EventInfo(int eventId, vector<Session*> sessions, vector<Paper*> papers, 
    vector<Topic*> topics, vector<Author*> authors, vector<vector<int>> pairs) {
    this->eventId = eventId;
    this->sessions = sessions;
    this->papers = papers;
    this->topics = topics;
    this->authors = authors;
    this->pairs = pairs;

    numSessions = this->sessions.size();
    numPapers = this->papers.size();
    numTopics = this->topics.size();
    numAuthors = this->authors.size();

    papersAuthor.resize(numAuthors);
    for (int i = 0; i < numAuthors; i++) {
        this->authors[i]->index = i;
    }

    papersTopic.resize(numTopics);
    for (int i = 0; i < numTopics; i++) {
        this->topics[i]->index = i;
    }

    for (int i = 0; i < this->papers.size(); i++) {
        Paper* paper = this->papers[i];
        paper->index = i;        
        for (Author* a : *paper->getAuthors()) {
            papersAuthor[a->index].push_back(paper);
        }
        for (Topic*t : *paper->getTopics()) {
            papersTopic[t->index].push_back(paper);
        }
    }

    orderSessionsByDate();
    initNumDays();
}

void EventInfo::printSessions(){
    for(auto session: sessions){
        cout << session->toString() << endl;
    }
}

EventInfo::~EventInfo() {    
    deleteVectorElements(&sessions);    
    deleteVectorElements(&papers);
    deleteVectorElements(&topics);
    deleteVectorElements(&authors);
}

int EventInfo::getEventId() {
    return eventId;
}

vector<Session*> &EventInfo::getSessions() {
    return sessions;
}

vector<Paper*> &EventInfo::getPapers() {
    return papers;
}

vector<Topic*> &EventInfo::getTopics() {
    return topics;
}

vector<Author*> &EventInfo::getAuthors() {
    return authors;
}

vector<vector<Paper*>> &EventInfo::getPapersAuthor() {
    return papersAuthor;
}

vector<vector<Paper*>> &EventInfo::getPapersTopic() {
    return papersTopic;
}

int EventInfo::getNumSessions() {
    return numSessions;
}

int EventInfo::getNumPapers() {
    return numPapers;
}

int EventInfo::getNumTopics() {
    return numTopics;
}

int EventInfo::getNumAuthors() {
    return numAuthors;
}

int EventInfo::getNumDays() {
    return numDays;
}

void EventInfo::setOFValue(int value) { 
    this->OFValue = value;
}
int EventInfo::getOFValue() { 
    return this->OFValue;
}

void EventInfo::orderSessionsByDate() {
    sort(sessions.begin(), sessions.end(),
        [](Session *s1, Session *s2)->bool {
            time_t time1 = s1->getDateTimeInit()->getDateTime();
            time_t time2 = s2->getDateTimeInit()->getDateTime();
            return difftime(time1, time2) < 0.0;
        });
}

void EventInfo::initNumDays() {
    numDays = 1;
    int size = sessions.size();
    DateTime* lastDate = sessions[0]->getDateTimeInit();
    
    for (int i = 1; i < size; i++) {
        if (!sessions[i]->isSameDate(lastDate)) {
            ++numDays;
            lastDate = sessions[i]->getDateTimeInit();
        }
    }
}

vector<vector<int>> EventInfo::getPairs() {
    return pairs;
}