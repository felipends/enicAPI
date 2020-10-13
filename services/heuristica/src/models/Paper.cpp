#include "Paper.h"
#include <sstream>

Paper::Paper(int id) {
    this->id = id;
    title = "Paper " + to_string(id);
}

Paper::Paper(int id, string jemsId, string title,
    vector<Topic*>* topics, vector<Author*>* authors) {
    this->id = id;
    this->jemsId = jemsId;
    this->title = title;
    this->topics = topics;
    this->authors = authors;
}

int Paper::getId() {
    return id;
}

string Paper::getJemsId() {
    return jemsId;
}

void Paper::setJemsId(string jemsId) {
    this->jemsId = jemsId;
}

string Paper::getTitle() {
    return title;
}

void Paper::setTitle(string title) {
    this->title = title;
}

vector<Topic* >* Paper::getTopics() {
    return topics;
}

void Paper::setTopics(vector<Topic*>* topics) {
this->topics = topics;
}

vector<Author*>* Paper::getAuthors() {
    return authors;
}

void Paper::setAuthors(vector<Author*>* authors) {
    this->authors = authors;
}

void Paper::addAuthor(Author& author)
{
    this->authors->push_back(&author);
}

int Paper::calculateBenefit(Paper &paper) {    
    // get topics id from current paper
    vector<int> myTopics(topics->size());
    transform(topics->begin(), topics->end(), myTopics.begin(),
        [](Topic *t) { return t->getId(); });
    vector<int> myAuthors(authors->size());
    transform(authors->begin(), authors->end(), myAuthors.begin(),
        [](Author *a) { return a->getId(); });

    // get topics id from parameter paper
    vector<int> topics(paper.topics->size());
    transform(paper.topics->begin(), paper.topics->end(), topics.begin(),
        [](Topic *t) {return t->getId(); });
    vector<int> authors(paper.authors->size());
    transform(paper.authors->begin(), paper.authors->end(), authors.begin(),
        [](Author *a) {return a->getId(); });

    // get intersecion
    sort(myTopics.begin(), myTopics.end());
    sort(topics.begin(), topics.end());
    sort(myAuthors.begin(), myAuthors.end());
    sort(authors.begin(), authors.end());

    vector<int> intersection;
    set_intersection(myTopics.begin(), myTopics.end(),
        topics.begin(), topics.end(), back_inserter(intersection));

    vector<int> authorsIntersection;
    set_intersection(myAuthors.begin(), myAuthors.end(),
        authors.begin(), authors.end(), back_inserter(authorsIntersection));

    bool isSameAdvisor = !(authorsIntersection.empty());

    int numTopicsInCommon = intersection.size();
    int benefit = 0;
    switch(numTopicsInCommon) {
        case 1: benefit = 1; break;
        case 2: benefit = 10; break;
        case 3: benefit = 100; break;
        case 0: benefit = -10; break;
    }    
    return isSameAdvisor ? abs(benefit*1000) : benefit;
}

bool Paper::operator==(const Paper &paper)  {
    return this->index == paper.index;
}

string Paper::toString() {
    stringstream ss;
    ss.clear();
    ss << "{";
    ss << "index: " << index << ", ";
    ss << "id: " << id << ", ";
    ss << "topics: [";

    int topicsLastIndex = topics->size() - 1;
    for (Topic *c : *topics) {
        if (c->getId() == topics->at(topicsLastIndex)->getId()) {
            ss << c->getId();
        } else {
            ss << c->getId() << ", ";
        }
    }
    ss << "], ";


    ss << "authors: [";
    int authorsLastIndex = authors->size() - 1;
    for (Author *a : *authors) {
        if (a == authors->at(authorsLastIndex)) {
            ss << a->getId();
        } else {
            ss << a->getId() << ", ";
        }
    }
    ss << "]";

    ss << "}";
    return ss.str();
}

bool Paper::hasAuthor(Author* author)
{
    return find(authors->begin(), authors->end(), author) != authors->end();
}

bool Paper::isAdvisor(int authorId) {   
    
    if (authors->at(1)->getId() == authorId) return true;

    return false;
}
