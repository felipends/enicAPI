#include "OutputGenerator.h"

OutputGenerator::OutputGenerator(string dictname, vector<Session*> sessions, vector<string> papersInfo, vector<double> &ilsTimes, int numSaltos, int numExtraProfs, int numUsedSessions){
    for(auto paperInfo: papersInfo){
        papersInfoOutput.push_back(paperInfo);
    }

    for(auto session: sessions){
        sessionsOutput.push_back(session);
    } 

	for(double timeils: ilsTimes){
		this->ilsTimes.push_back(timeils);
	}

    dictionaryFilename = dictname;
	this->numSaltos = numSaltos;
	this->numExtraProfs = numExtraProfs;
	this->numUsedSessions = numUsedSessions;
}

void OutputGenerator::printSessions(EventInfo& event){
    cout << "{\npapersInfo: [";
    for(auto paper: papersInfoOutput){
        cout << paper << ", ";
    }
    cout << "],\n";  
    cout << "sessions: '[\n";
    for(auto sess: sessionsOutput){
        cout << sess->toStringSolution(dictionaryFilename, event.getPapers()) <<","<< endl;
    }
    cout << "]}\n";
}

void OutputGenerator::saveOutputIntoFile(string file, EventInfo& event){
    fstream outfile;
    stringstream out;
    stringstream outaux, outaux2;

    outfile.open(file, std::ios_base::out);

    out << "{\n\"objectiveFunctionValue\": ";
    out << event.getOFValue()<<",\n";
    out << "\"numUsedSessions\": ";
    out << this->numUsedSessions<<",\n";
    out << "\"timeConstruction\": ";
    out << ilsTimes.at(0)<<",\n";
    out << "\"timeRVND\": ";
    out << ilsTimes.at(1)<<",\n";
    out << "\"timePerturb\": ";
    out << ilsTimes.at(2)<<",\n";
    out << "\"timeChecker\": ";
    out << ilsTimes.at(3)<<",\n";
    out << "\"timeProfs\": ";
    out << ilsTimes.at(4)<<",\n";
    out << "\"numSaltos\": ";
    out << this->numSaltos<<",\n";
    out << "\"numExtraProfs\": ";
    out << this->numExtraProfs<<",\n";
    out << "\"sessions\": [\n";
    for(auto sess: sessionsOutput){
        outaux2 << sess->toStringSolution(dictionaryFilename, event.getPapers()) <<",\n";
    }
    out << outaux2.str().substr(0, outaux2.str().size()-2);
    out << "\n]}\n";
    
    outfile << out.str();
    outfile.close();
}
