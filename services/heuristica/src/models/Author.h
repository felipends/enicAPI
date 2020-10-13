#ifndef AUTHOR_H
#define AUTHOR_H

#include <iostream>
#include "TimeConstraint.h"
#include "Topic.h"
#include <vector>

using namespace std;

class Author {
    private:
        int id;
        string name;
        string university;
        vector<TimeConstraint*> constraints;
        vector<Topic*> authorTopics;

    public:
        int index;
        Author(int id, string name, string university);
        Author(int id, string name, vector<Topic*>& topics);
        int getId();
        void setId(int id);
        string getName();
        void setName(string name);
        string getUniversity();
        void setUniversity(string university);
        void addConstraint(int day, int slot);
        bool hasConstraint(int day, int slot);
        void setTopics(vector<Topic*> &topics);
        vector<Topic*>* getAuthorTopics() {return &authorTopics;}
};

#endif