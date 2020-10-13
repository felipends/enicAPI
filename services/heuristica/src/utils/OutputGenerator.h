#ifndef OUTPUT_GENERATOR_H
#define OUTPUT_GENERATOR_H

#include "../models/EventInfo.h"
#include "../models/Session.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

class OutputGenerator {
    private:
        string dictionaryFilename;
        vector<Session*> sessionsOutput;
        vector<string> papersInfoOutput;
		vector<double> ilsTimes;
		int numSaltos;
		int numExtraProfs;
		int numUsedSessions;
    public:
        OutputGenerator(string dictname, vector<Session*> sess, vector<string> papersInfo, vector<double> &ilsTimes, int numSaltos, int numExtraProfs, int numUsedSessions);
        void printSessions(EventInfo& event);
        void saveOutputIntoFile(string file, EventInfo& event);
};

#endif
