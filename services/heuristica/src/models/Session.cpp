#include "Session.h"

Session::Session(int id, int numPapers, int presentationTime, DateTime *dateTimeInit) {
    this->id = id;
    this->numPapers = numPapers;
    this->presentationTime = presentationTime;
    this->dateTimeInit = dateTimeInit;
}

Session::~Session() {
    delete dateTimeInit;
}

int Session::getId() {
    return id;
}

int Session::getNumPapers() {
    return numPapers;
}

int Session::getPresentationTime() {
    return presentationTime;
}

DateTime* Session::getDateTimeInit() {
    return dateTimeInit;
}

int Session::getNumMinPapers() {
    return getMinSize(numPapers);
}

int Session::getMinSize(int sessionSize) {        
    switch(sessionSize) {
        case 1: return 1;
        case 2: return 2;
        case 3: return 2;
        case 4: return 3;
        case 5: return 3;
        default: return 3;
    }
    return 3;
}

void Session::setPapersInSolution(int paperId){
    papersInSolution.push_back(paperId);
}

string Session::toString() {
    return "{id: " + to_string(id) +
        ", numPapers: " + to_string(numPapers) + 
        ", presTime: " + to_string(presentationTime) +
        ", dateTime: " + dateTimeInit->toString()
        +"}";
}

int extractId(string str) {
    stringstream ss;
    ss << str;

    string temp;
    int found, id;
    while(!ss.eof()){
        ss >> temp;
        if(stringstream(temp) >> found)
            id = found;
        
        temp = "";
    }

    return id;
}

pair<int, string> findById(int id, vector<pair<int, string>>& pairVec) {
    vector<pair<int, string>>::iterator it = find_if(pairVec.begin(), pairVec.end(), [&](pair<int, string> item){ return (item.first == id) ? true : false; });
    return (*it);
}

string Session::toStringSolution(string file_dict, vector<Paper*>& papersVec) {
    ifstream* dict_file = new ifstream(file_dict, ios_base::in);

    vector<string> autoresTemp, topicosTemp;
    vector<pair<int, string>> autoresPair, topicosPair;
    string temp;

    int count = 0;
    while(getline(*dict_file, temp)){
       if(temp.size() <= 1){
           count++;
       } 
       if(count == 0){
           autoresTemp.push_back(temp);
       }
       else {
           topicosTemp.push_back(temp);
       }
    }

    int id;
    for(auto autor: autoresTemp) {
        id = extractId(autor);
        autor.erase(std::remove_if(std::begin(autor), std::end(autor),[](char ch){return std::isdigit(ch);}), autor.end());
        autor.erase(autor.begin(), std::find_if(autor.begin(), autor.end(), [](int ch) { return !std::isspace(ch); }));

        pair<int, string> auxPair = make_pair(id, autor);
        autoresPair.push_back(auxPair);
    }
    
    for(auto autor: autoresTemp) {
        id = extractId(autor);
        autor.erase(std::remove_if(std::begin(autor), std::end(autor),[](char ch){return std::isdigit(ch);}), autor.end());
        autor.erase(autor.begin(), std::find_if(autor.begin(), autor.end(), [](int ch) { return !std::isspace(ch); }));

        pair<int, string> auxPair = make_pair(id, autor);
        topicosPair.push_back(auxPair);
    }

    vector<Paper*> auxPaperInfo = papersVec;

    stringstream papers;
    string papersStr;
    int ppcount = 0;
    for(int paper: papersInSolution){
        vector<Author*>* autores = auxPaperInfo[paper]->getAuthors();
        pair<int, string> auxAutorPair; 
        papers << "{";
        papers << "\"id\" :" << paper << ",\n";
        papers << "\"hour\" : \"" << getSlotTime(dateTimeInit->getHour(), auxPaperInfo[paper]->getSlot()) << "\",\n";
        papers << "\"slot\" :" << auxPaperInfo[paper]->getSlot() << ",\n";
        papers << "\"aluno\" :" << "\"" << findById((*autores)[0]->getId(), autoresPair).second << "\"" << ",\n";
        papers << "\"orientador\" :" << "\"" << findById((*autores)[1]->getId(), autoresPair).second << "\"";
        if (auxPaperInfo[paper]->getAuthors()->size() > 2) {
            papers << ",\n";
            papers << "\"avaliador1\" :" << "\"" << findById((*autores)[2]->getId(), autoresPair).second << "\"" << ",\n";
            if (ppcount < papersInSolution.size() - 1)
                papers << "\"avaliador2\" :" << "\"" << findById((*autores)[3]->getId(), autoresPair).second << "\"" << "},\n";
            else
                papers << "\"avaliador2\" :" << "\"" << findById((*autores)[3]->getId(), autoresPair).second << "\"" << "}\n";
        }
        else {
            if (ppcount < papersInSolution.size() - 1)
                papers << "},\n";
            else
                papers << "}\n";
        }
        
        ppcount++;
    }

    papersStr = papers.str().substr(0, papers.str().size()-1);
    return "{\"id\": " + to_string(this->id) +
        ", \"numPapers\": " + to_string(papersInSolution.size()) + 
        ", \"presTime\": " + to_string(presentationTime) +
        ", \"dateTime\": \"" + dateTimeInit->toString() + "\"" +
        ", \"numGaps\": \"" + to_string(this->numGaps) + "\"" +
        ", \"numExtraProfs\": \"" + to_string(this->numExtraProfs) + "\"" +
        ", \"papers\": [" + papersStr + "]"
        +"}";
}

string Session::getSlotTime(int hour, int slot)
{
    switch(slot)
    {
        case 0:
            return to_string(hour)+":00";
        case 1:
            return to_string(hour)+":20";
        case 2:
            return to_string(hour)+":40";
        case 3:
            return to_string(hour+1)+":00";
        case 4:
            return to_string(hour+1)+":20";
        case 5:
            return to_string(hour+1)+":40";
        case 6:
            return to_string(hour+2)+":00";
        case 7:
            return to_string(hour+2)+":20";
        case 8:
            return to_string(hour+2)+":40";
        case 9:
            return to_string(hour+3)+":00";
        case 10:
            return to_string(hour+3)+":20";
        case 11:
            return to_string(hour+3)+":40";
        case 12:
            return to_string(hour+4)+":00";
    }
}

bool Session::isSameDate(DateTime* dateTime) {
    return (dateTimeInit->getYear() == dateTime->getYear())
        && (dateTimeInit->getMonth() == dateTime->getMonth())
        && (dateTimeInit->getDay() == dateTime->getDay());
}

bool Session::isSameTime(DateTime* dateTime) {
    return (dateTimeInit->getHour() == dateTime->getHour())
        && (dateTimeInit->getMinute() == dateTime->getMinute());
}
