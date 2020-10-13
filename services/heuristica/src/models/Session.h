#ifndef CLASS_SESSION_H
#define CLASS_SESSION_H

#include <iostream>
#include "../utils/DateTime.h"
#include "../models/Paper.h"
#include <memory>
#include <sstream>
#include <fstream>

using namespace std;

class Session {
    private:
        int id;
        int numPapers;
		int numGaps;
		int numExtraProfs;
        int presentationTime;
        DateTime *dateTimeInit;

        vector<int> papersInSolution;
        string getSlotTime(int hour, int slot);
    public:
        Session(int id, int numPapers, int presentationTime, DateTime *dateTimeInit);        
        ~Session();

		void setNumExtraProfs(int numExtraProfs){ this->numExtraProfs = numExtraProfs; }
		void setNumGaps(int numGaps){ this->numGaps = numGaps; }
		int getNumExtraProfs(){ return this->numExtraProfs; }
		int getNumGaps(){ return this->numGaps; }

        void setPapersInSolution(int paper);

        string toStringSolution(string file_dict, vector<Paper*>& papersVec);
        int getId();
        int getNumPapers();
        int getPresentationTime();
        DateTime* getDateTimeInit();
        int getNumMinPapers();
        static int getMinSize(int sessionSize);
        string toString();
        bool isSameDate(DateTime* dateTime);
        bool isSameTime(DateTime* dateTime);
};
#endif
