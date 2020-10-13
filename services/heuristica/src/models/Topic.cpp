#include "Topic.h"

Topic::Topic() {    
}

Topic::Topic(int id, string name) {
    this->id = id;
    this->name = name;
}

int Topic::getId() {
    return id;
}

void Topic::setId(int id) {
    this->id = id;
}

string Topic::getName() {
    return this->name;
}

void Topic::setName(string name) {
    this->name = name;
}

string Topic::toString() {
    return "";
}