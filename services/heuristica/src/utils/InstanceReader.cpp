#include "InstanceReader.h"
#include "DateTime.h"
#include <vector>

InstanceReader::InstanceReader(string filename) {
    filestream = new fstream(filename, ios_base::in);
}

InstanceReader::~InstanceReader() {
    filestream->close();
    delete filestream;
}

EventInfo* InstanceReader::readInstance() {
    parseTopics();
    parseAuthors();
    parsePapers();
    parseSessions();
    parseAuthorConstraints();
    readPairs();
    parseExtraProfessors();
    return new EventInfo(0, sessions, papers, topics, authors, pairs);
}

void InstanceReader::parseTopics() {
    int numTopics;
    *filestream >> numTopics;
    *filestream >> idFirstTopicId;

    for (int i = 0; i < numTopics; i++) {
        topics.push_back(new Topic(i, "Topic " + to_string(i)));
    }
}

void InstanceReader::parseAuthors() {
    int numAuthors;
    *filestream >> numAuthors;
    *filestream >> idFirstAuthorId;

    for (int i = 0; i < numAuthors; i++) {
        authors.push_back(new Author(i, "Author" + to_string(i), ""));
    }
}

void InstanceReader::parsePapers() {
    int numPapers;
    *filestream >> numPapers;
    *filestream >> idFirstPaperId;

    int numTopics = 0;
    int numAuthors = 0;
    int topicIndex = 0;
    int authorIndex = 0;    
    for (int i = 0; i < numPapers; i++) { 
        stringstream paperInfo;       
        Paper* paper = new Paper(i);

        *filestream >> numTopics;
        paperInfo << numTopics <<" ";
        vector<Topic*>* topicsInPaper = new vector<Topic*>();
        for (int t = 0; t < numTopics; t++) {
            *filestream >> topicIndex;
            paperInfo << topicIndex <<" ";
            topicsInPaper->push_back(topics.at(topicIndex));
        }
        paper->setTopics(topicsInPaper);
        
        *filestream >> numAuthors;
        paperInfo << numAuthors <<" ";
        vector<Author*>* authorsInPaper = new vector<Author*>();
        for (int a = 0; a < numAuthors; a++) {
            *filestream >> authorIndex;
            paperInfo << authorIndex <<" ";
            authorsInPaper->push_back(authors.at(authorIndex));
        }
        paper->setAuthors(authorsInPaper);

        papersInfo.push_back(paperInfo.str());
        papers.push_back(paper);
    }
}

vector<string> InstanceReader::getPapersInfo(){
    return papersInfo;
}

vector<int>* InstanceReader::getExtraProfessorsIndices()
{
	return &(this->extraProfessorsIndices);
}

void InstanceReader::parseSessions() {
    int numSessions;
    *filestream >> numSessions;
    int numPapers, presentationTime, day, month, year, hour, minutes;
    for (int i = 0; i < numSessions; i++) {        
        *filestream >> numPapers;
        *filestream >> presentationTime;
        *filestream >> day;
        *filestream >> month;
        *filestream >> year;
        *filestream >> hour;
        *filestream >> minutes;

        DateTime *dateTime = new DateTime(day, month, year, hour, minutes);
        sessions.push_back(new Session(i, numPapers, presentationTime, dateTime));
    }
}

void InstanceReader::parseAuthorConstraints() {
    int numConstraints;
    *filestream >> numConstraints;
    int authorId;
    int numAuthorConstraints;    
    int day, slot;
    for (int i = 0; i < numConstraints; i++) {
        *filestream >> authorId;
        *filestream >> numAuthorConstraints;
        cout << "Author " << authorId << " com restrição nos slots ";
        for (int k = 0; k < numAuthorConstraints; k++) {
            *filestream >> day;
            *filestream >> slot;
            authors.at(authorId)->addConstraint(day-1, slot-1); // no arquivo começa de 1, mas no checker começa em 0
            cout << "(" << day << "," << slot << ") ";
        }
        cout << endl;
    }
}

void InstanceReader::readPairs() {
    int numPairs, paper1, paper2;
    *filestream >> numPairs;
    for (int i = 0; i < numPairs; i++) {
        vector<int> pair;
        *filestream >> paper1;
        *filestream >> paper2;
        pair.push_back(paper1);
        pair.push_back(paper2);
        pairs.push_back(pair);
    }
}

void InstanceReader::parseExtraProfessors() {
    int numExtraProfessors = 0;
    int numTopics = 0; 
    int professorIndex = 0; 
    int topicIndex = 0; 

    *filestream >> numExtraProfessors;
    for (int i = 0; i < numExtraProfessors; i++) {
        *filestream >> professorIndex;
        extraProfessorsIndices.push_back(professorIndex);

        *filestream >> numTopics;
        vector<Topic*>* auxTopics = new vector<Topic*>();
        if (numTopics > 0) { 
            for (int j = 0; j < numTopics; j++) {
                *filestream >> topicIndex;
                auxTopics->push_back(topics.at(topicIndex));
            }
            authors.at(professorIndex)->setTopics(*auxTopics);
        }
    }
}
