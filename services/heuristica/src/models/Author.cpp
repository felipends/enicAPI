#include "Author.h"

Author::Author(int id, string name, string university) {
    this->id = id;
    this->name = name;
    this->university = university;
}

Author::Author(int id, string name, vector<Topic*>& topics) {
    this->id = id;
    this->name = name;
    this->authorTopics = topics;
}

int Author::getId() {
    return id;
}

void Author::setId(int id) {
    this->id = id;
}

void Author::setTopics(vector<Topic*> &topics) {
    this->authorTopics = topics;
}

string Author::getName() {
    return name;
}

void Author::setName(string name) {
    this->name = name;
}

string Author::getUniversity() {
    return university;
}

void Author::setUniversity(string university) {
    this->university = university;
}

void Author::addConstraint(int day, int slot) {
    constraints.push_back(new TimeConstraint(day, slot));
}

bool Author::hasConstraint(int day, int slot) {
    if (constraints.size() == 0)
        return false;
        
    for (TimeConstraint* t : constraints) {
        if (t->compare(day, slot)) {
            return true;
        }
    }
    return false;
}