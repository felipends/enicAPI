#ifndef PAPER_H
#define PAPER_H

#include <iostream>
#include <vector>
#include <algorithm>
#include "Topic.h"
#include "Author.h"

using namespace std;

class Paper {
    private:
        int id;
        int slot;
        string jemsId;
        string title;    
        vector<Topic*>* topics;
        vector<Author*>* authors;            

    public:
        int getSlot() { return slot; }
        void setSlot(int s) { slot = s; }
        int index;
        Paper(int id);
        Paper(int id, string jemsId, string title, 
            vector<Topic*>* topics, vector<Author*>* authors);
        int getId();        
        string getJemsId();
        void setJemsId(string jemsId);
        string getTitle();
        void setTitle(string title);
        vector<Topic*>* getTopics();
        void setTopics(vector<Topic*>* topics);
        vector<Author*>* getAuthors();
        void setAuthors(vector<Author*>* authors);
        void addAuthor(Author& author);

        bool hasAuthor(Author* author);
        bool isAdvisor(int authorId);

        int calculateBenefit(Paper &paper);
        virtual bool operator==(const Paper &paper);
        string toString();
};

#endif