#ifndef TOPIC_H
#define TOPIC_H

#include <iostream>

using namespace std;

class Topic {
    private:
        int id;
        string name;

    public:
        int index;
        Topic();
        Topic(int id, string name);
        int getId();
        void setId(int id);
        string getName();
        void setName(string name);
        string toString();
};

#endif