#include "ContiguousCluster.h"

ContiguousCluster::ContiguousCluster(vector<Paper*>& clusterPapers)
{
    this->papers = clusterPapers;
    initializeData();
    setTopics();
}

ContiguousCluster::ContiguousCluster(vector<Author*>& allProfesors, vector<int>* professorsIndices, vector<Paper*>& clusterPapers, int prevClusterSize)
{
    this->prevClusterSize = prevClusterSize;
    this->papers = clusterPapers;
    this->extraProfessorsIndices = professorsIndices;

    for (int prof: *professorsIndices) {
        extraProfessors.push_back(allProfesors.at(prof));
    }

    initializeData();
    setTopics();
    if (numProfessors == 2) {
        chooseExtraProfessors();
    }
    else if (numProfessors == 1) {
        chooseExtraProfessors();
        chooseExtraProfessors();
    }
}

void ContiguousCluster::initializeData()
{
    this->numPapers = this->papers.size();

    for (Paper* paper: this->papers) {
        vector<Author*>* authorsVec = paper->getAuthors();
        if(find(professorsInCluster.begin(), professorsInCluster.end(), (*authorsVec)[1]->getId()) != professorsInCluster.end()){
            continue;
        } 
        else {
            professors.push_back((*authorsVec)[1]);
            professorsInCluster.push_back((*authorsVec)[1]->getId());
        }
    }

    this->numProfessors = professorsInCluster.size();
}

void ContiguousCluster::chooseExtraProfessors()
{
    int bestBenefit = INT32_MIN;
    int benefit = 0;
    bool professorFound = false;
    Author* bestProfessor = nullptr;
	vector<int> auxVec; //topics in cluster
	for (Topic* topic: topics) {
		auxVec.push_back(topic->getId());
	}

    for (Author* prof: extraProfessors) {
		if (prof->getAuthorTopics()->size() == 0) continue;
        for (int i = 0; i < prof->getAuthorTopics()->size(); i++) {
            Topic* auxTopic = prof->getAuthorTopics()->at(i);
            if (find(auxVec.begin(), auxVec.end(), auxTopic->getId()) != auxVec.end()) {
                benefit += 10;
            }
        }
        if (benefit > bestBenefit){
            professorFound = true;
            bestBenefit = benefit;
            bestProfessor = prof;
        }
		benefit = 0;
    }
    
    if (professorFound) {
        professors.push_back(bestProfessor);
        numProfessors++;
        extraProfessors.erase(remove(extraProfessors.begin(), extraProfessors.end(), bestProfessor), extraProfessors.end());
        extraProfessorsIndices->erase(remove(extraProfessorsIndices->begin(), extraProfessorsIndices->end(), bestProfessor->getId()), extraProfessorsIndices->end());
    }
}

void ContiguousCluster::setTopics()
{
    for (Paper* paper: papers) {
        vector<Topic*>* auxTopics = paper->getTopics();
        for (Topic* topic: (*auxTopics)) {
            if (find(topics.begin(), topics.end(), topic) != topics.end()) {
                continue;
            } 
            else {
                topics.push_back(topic);
            }
        }
        auxTopics->clear();
    }
}

void ContiguousCluster::makeClusterContiguous()
{
    if (numProfessors == 3) {
        int paperCount = 0;
        for(Paper* paper: papers){
            paper->setSlot(prevClusterSize+paperCount++);
            for(Author* prof: professors){
                vector<Author*>* authors = paper->getAuthors();
                if (find(authors->begin(), authors->end(), prof) != authors->end()) {
                    continue;
                }
                else {
                    paper->addAuthor(*prof);
                }
            }
        }        
    }

    else if (numPapers == 6 && numProfessors == 4) {
        // elect the professor with most papers
        vector<Paper*> auxPapers(6, nullptr);
        vector<pair<int, int>*> authorsPapers;

        for (int profId: professorsInCluster) {
            pair<int, int>* auxPairPtr = new pair<int, int>(profId, 0);
            authorsPapers.push_back(auxPairPtr);
        }

        for (pair<int, int>* profPapers: authorsPapers){
            for (Paper* paper: papers) {
                if ((*(paper->getAuthors()))[1]->getId() == profPapers->first) {
                    profPapers->second++;
                }
            }
        }

        int max = authorsPapers[0]->second;
        for (int i = 0; i < 4; i++) {
            if (authorsPapers[i]->second > max) {
                max = authorsPapers[i]->second;
            }
        }

        if (max == 2) {
            for (pair<int, int>* profPapers: authorsPapers) {
                for (Paper* paper: papers) {
                    if (profPapers->first == (*(paper->getAuthors()))[1]->getId()) {
                        if (profPapers->second == 2) {
                            if (!auxPapers[0]) {
                                auxPapers[0] = paper;
                                auxPapers[0]->setSlot(prevClusterSize+0);
                            } else if (!auxPapers[2]) {
                                auxPapers[2] = paper;
                                auxPapers[2]->setSlot(prevClusterSize+2);
                            } else if (!auxPapers[3]) {
                                auxPapers[3] = paper;
                                auxPapers[3]->setSlot(prevClusterSize+3);
                            } else if (!auxPapers[4]) {
                                auxPapers[4] = paper;
                                auxPapers[4]->setSlot(prevClusterSize+4);
                            }
                        } else {
                            if (!auxPapers[1]) {
                                auxPapers[1] = paper;
                                auxPapers[1]->setSlot(prevClusterSize+1);
                            } else {
                                auxPapers[5] = paper;
                                auxPapers[5]->setSlot(prevClusterSize+5);
                            }
                        }
                    }
                } 
            }

            auxPapers[0]->addAuthor(*(*(auxPapers[1]->getAuthors()))[1]);
            auxPapers[2]->addAuthor(*(*(auxPapers[1]->getAuthors()))[1]);

            auxPapers[3]->addAuthor(*(*(auxPapers[5]->getAuthors()))[1]);
            auxPapers[4]->addAuthor(*(*(auxPapers[5]->getAuthors()))[1]);
			
			// 0 3 1
			// 3 0 1
			// 0 3 1
			// 1 2 0
			// 1 2 0
			// 2 1 0
			if ((*(auxPapers[0]->getAuthors()))[1]->getId() == (*(auxPapers[2]->getAuthors()))[1]->getId()) {
				auxPapers[0]->addAuthor(*(*(auxPapers[3]->getAuthors()))[1]);
				auxPapers[2]->addAuthor(*(*(auxPapers[3]->getAuthors()))[1]);
				auxPapers[1]->addAuthor(*(*(auxPapers[3]->getAuthors()))[1]);
				auxPapers[5]->addAuthor(*(*(auxPapers[3]->getAuthors()))[1]);
				auxPapers[3]->addAuthor(*(*(auxPapers[0]->getAuthors()))[1]);
				auxPapers[4]->addAuthor(*(*(auxPapers[0]->getAuthors()))[1]);
				auxPapers[1]->addAuthor(*(*(auxPapers[0]->getAuthors()))[1]);
				auxPapers[5]->addAuthor(*(*(auxPapers[0]->getAuthors()))[1]);
			}
			// 0 3 1
			// 3 0 1
			// 1 3 0
			// 0 1 2
			// 1 0 2
			// 2 1 0
			else if ((*(auxPapers[0]->getAuthors()))[1]->getId() == (*(auxPapers[3]->getAuthors()))[1]->getId()) {
				auxPapers[0]->addAuthor(*(*(auxPapers[2]->getAuthors()))[1]);
				auxPapers[2]->addAuthor(*(*(auxPapers[0]->getAuthors()))[1]);
				auxPapers[1]->addAuthor(*(*(auxPapers[2]->getAuthors()))[1]);
				auxPapers[5]->addAuthor(*(*(auxPapers[2]->getAuthors()))[1]);
				auxPapers[3]->addAuthor(*(*(auxPapers[2]->getAuthors()))[1]);
				auxPapers[4]->addAuthor(*(*(auxPapers[0]->getAuthors()))[1]);
				auxPapers[1]->addAuthor(*(*(auxPapers[0]->getAuthors()))[1]);
				auxPapers[5]->addAuthor(*(*(auxPapers[0]->getAuthors()))[1]);
			}
			// 0 3 1
			// 3 0 1
			// 1 3 0
			// 1 2 0
			// 0 1 2
			// 2 1 0
            else if ((*(auxPapers[0]->getAuthors()))[1]->getId() == (*(auxPapers[4]->getAuthors()))[1]->getId()) {
				auxPapers[0]->addAuthor(*(*(auxPapers[2]->getAuthors()))[1]);
				auxPapers[2]->addAuthor(*(*(auxPapers[0]->getAuthors()))[1]);
				auxPapers[1]->addAuthor(*(*(auxPapers[2]->getAuthors()))[1]);
				auxPapers[5]->addAuthor(*(*(auxPapers[2]->getAuthors()))[1]);
				auxPapers[3]->addAuthor(*(*(auxPapers[0]->getAuthors()))[1]);
				auxPapers[4]->addAuthor(*(*(auxPapers[2]->getAuthors()))[1]);
				auxPapers[1]->addAuthor(*(*(auxPapers[0]->getAuthors()))[1]);
				auxPapers[5]->addAuthor(*(*(auxPapers[0]->getAuthors()))[1]);
			}
        }
        
        if (max == 3) {
            for (pair<int, int>* prof: authorsPapers) {
                for (Paper* paper: papers) {
                    if (prof->first == (*(paper->getAuthors()))[1]->getId()) {
                        if (prof->second == 3) {
                            if (!auxPapers[1]) {
                                auxPapers[1] = paper;
                                auxPapers[1]->setSlot(prevClusterSize+1);
                            } 
                            else if (!auxPapers[2]){
                                auxPapers[2] = paper;
                                auxPapers[2]->setSlot(prevClusterSize+2);
                            }       
                            else if (!auxPapers[5]){
                                auxPapers[5] = paper;
                                auxPapers[5]->setSlot(prevClusterSize+5);
                            }
                        }
                        else {
                            if (!auxPapers[0]) {
                                auxPapers[0] = paper;
                                auxPapers[0]->setSlot(prevClusterSize+0);
                            }
                            else if (!auxPapers[3]) {
                                auxPapers[3] = paper;
                                auxPapers[3]->setSlot(prevClusterSize+3);
                            }
                            else if (!auxPapers[4]) {
                                auxPapers[4] = paper;
                                auxPapers[4]->setSlot(prevClusterSize+4);
                            }
                        }
                    }
                }
            } 

            // 1 0 3
            // 0 1 3
            // 0 1 3
            // 2 0 1
            // 3 0 2
            // 0 2 3
            auxPapers[0]->addAuthor(*(*(auxPapers[1]->getAuthors()))[1]);
            auxPapers[0]->addAuthor(*(*(auxPapers[4]->getAuthors()))[1]);
            auxPapers[1]->addAuthor(*(*(auxPapers[0]->getAuthors()))[1]);
            auxPapers[1]->addAuthor(*(*(auxPapers[4]->getAuthors()))[1]);
            auxPapers[2]->addAuthor(*(*(auxPapers[0]->getAuthors()))[1]);
            auxPapers[2]->addAuthor(*(*(auxPapers[4]->getAuthors()))[1]);
            auxPapers[3]->addAuthor(*(*(auxPapers[1]->getAuthors()))[1]);
            auxPapers[3]->addAuthor(*(*(auxPapers[0]->getAuthors()))[1]);
            auxPapers[4]->addAuthor(*(*(auxPapers[1]->getAuthors()))[1]);
            auxPapers[4]->addAuthor(*(*(auxPapers[3]->getAuthors()))[1]);
            auxPapers[5]->addAuthor(*(*(auxPapers[3]->getAuthors()))[1]);
            auxPapers[5]->addAuthor(*(*(auxPapers[4]->getAuthors()))[1]);
        }

        papers.clear();
        papers = auxPapers;
        auxPapers.clear();
        auxPapers = vector<Paper*>(6, nullptr);
    }

    else if (numPapers == 6 && numProfessors == 5) {
        // figure out who has 2 papers
        vector<Paper*> auxPapers(6, nullptr);
        vector<pair<int, int>*> authorsPapers;

        for (int profId: professorsInCluster) {
            pair<int, int>* auxPairPtr = new pair<int, int>(profId, 0);
            authorsPapers.push_back(auxPairPtr);
        }

        for (pair<int, int>* profPapers: authorsPapers){
            for (Paper* paper: papers) {
                if ((*(paper->getAuthors()))[1]->getId() == profPapers->first) {
                    profPapers->second++;
                }
            }
        }

        for (pair<int, int>* profPapers: authorsPapers) {
            for (Paper* paper: papers) {
                if (profPapers->first == (*(paper->getAuthors()))[1]->getId()) {
                    if (profPapers->second == 2) {
                        if (!auxPapers[2]) {
                            auxPapers[2] = paper;
                            auxPapers[2]->setSlot(prevClusterSize+2);
                        }
                        else if (!auxPapers[5]) {
                            auxPapers[5] = paper;
                            auxPapers[5]->setSlot(prevClusterSize+5);
                        }
                    }
                    else {
                        if (!auxPapers[0]) {
                            auxPapers[0] = paper;
                            auxPapers[0]->setSlot(prevClusterSize+0);
                        }
                        else if (!auxPapers[1]) {
                            auxPapers[1] = paper;
                            auxPapers[1]->setSlot(prevClusterSize+1);
                        }
                        else if (!auxPapers[3]) {
                            auxPapers[3] = paper;
                            auxPapers[3]->setSlot(prevClusterSize+3);
                        }
                        else if (!auxPapers[4]) {
                            auxPapers[4] = paper;
                            auxPapers[4]->setSlot(prevClusterSize+4);
                        }
                    }
                }
            }
        }
        // 3 0 4
        // 0 3 4
        // 4 0 3
        // 1 2 4
        // 2 1 4
        // 4 1 2
        auxPapers[0]->addAuthor(*(*(auxPapers[1]->getAuthors()))[1]);
        auxPapers[0]->addAuthor(*(*(auxPapers[2]->getAuthors()))[1]);
        auxPapers[1]->addAuthor(*(*(auxPapers[0]->getAuthors()))[1]);
        auxPapers[1]->addAuthor(*(*(auxPapers[2]->getAuthors()))[1]);
        auxPapers[2]->addAuthor(*(*(auxPapers[1]->getAuthors()))[1]);
        auxPapers[2]->addAuthor(*(*(auxPapers[0]->getAuthors()))[1]);
        auxPapers[3]->addAuthor(*(*(auxPapers[4]->getAuthors()))[1]);
        auxPapers[3]->addAuthor(*(*(auxPapers[2]->getAuthors()))[1]);
        auxPapers[4]->addAuthor(*(*(auxPapers[3]->getAuthors()))[1]);
        auxPapers[4]->addAuthor(*(*(auxPapers[2]->getAuthors()))[1]);
        auxPapers[5]->addAuthor(*(*(auxPapers[3]->getAuthors()))[1]);
        auxPapers[5]->addAuthor(*(*(auxPapers[4]->getAuthors()))[1]);

        papers.clear();
        papers = auxPapers;
        auxPapers.clear();
        auxPapers = vector<Paper*>(6, nullptr);
    }

    else if (numPapers == 6 && numProfessors == 6) {
       // 4 0 3
       // 0 3 4
       // 3 0 4
       // 5 1 2
       // 1 2 5
       // 2 1 5
       papers[0]->setSlot(prevClusterSize+0);
       papers[0]->addAuthor(*(*(papers[1]->getAuthors()))[1]);
       papers[0]->addAuthor(*(*(papers[2]->getAuthors()))[1]);
       papers[1]->setSlot(prevClusterSize+1);
       papers[1]->addAuthor(*(*(papers[2]->getAuthors()))[1]);
       papers[1]->addAuthor(*(*(papers[0]->getAuthors()))[1]);
       papers[2]->setSlot(prevClusterSize+2);
       papers[2]->addAuthor(*(*(papers[1]->getAuthors()))[1]);
       papers[2]->addAuthor(*(*(papers[0]->getAuthors()))[1]);
       papers[3]->setSlot(prevClusterSize+3);
       papers[3]->addAuthor(*(*(papers[4]->getAuthors()))[1]);
       papers[3]->addAuthor(*(*(papers[5]->getAuthors()))[1]);
       papers[4]->setSlot(prevClusterSize+4);
       papers[4]->addAuthor(*(*(papers[5]->getAuthors()))[1]);
       papers[4]->addAuthor(*(*(papers[3]->getAuthors()))[1]);
       papers[5]->setSlot(prevClusterSize+5);
       papers[5]->addAuthor(*(*(papers[4]->getAuthors()))[1]);
       papers[5]->addAuthor(*(*(papers[3]->getAuthors()))[1]);
    }

    else if (numProfessors == 5 && numPapers == 5){
        // 0 2 3
        // 3 0 2
        // 2 0 3
        // 4 1 2
        // 1 2 4
        papers[0]->setSlot(prevClusterSize+0);
        papers[0]->addAuthor(*(*(papers[1]->getAuthors()))[1]);
        papers[0]->addAuthor(*(*(papers[2]->getAuthors()))[1]);
        papers[1]->setSlot(prevClusterSize+1);
        papers[1]->addAuthor(*(*(papers[2]->getAuthors()))[1]);
        papers[1]->addAuthor(*(*(papers[0]->getAuthors()))[1]);
        papers[2]->setSlot(prevClusterSize+2);
        papers[2]->addAuthor(*(*(papers[0]->getAuthors()))[1]);
        papers[2]->addAuthor(*(*(papers[1]->getAuthors()))[1]);
        papers[3]->setSlot(prevClusterSize+3);
        papers[3]->addAuthor(*(*(papers[4]->getAuthors()))[1]);
        papers[3]->addAuthor(*(*(papers[2]->getAuthors()))[1]);
        papers[4]->setSlot(prevClusterSize+4);
        papers[4]->addAuthor(*(*(papers[2]->getAuthors()))[1]);
        papers[4]->addAuthor(*(*(papers[3]->getAuthors()))[1]);
    }

    else if (numProfessors == 4 && numPapers == 5){
        // figure out who has 2 papers
        vector<Paper*> auxPapers(5, nullptr);
        vector<pair<int, int>*> authorsPapers;

        for (int profId: professorsInCluster) {
            pair<int, int>* auxPairPtr = new pair<int, int>(profId, 0);
            authorsPapers.push_back(auxPairPtr);
        }

        for (pair<int, int>* profPapers: authorsPapers){
            for (Paper* paper: papers) {
                if ((*(paper->getAuthors()))[1]->getId() == profPapers->first) {
                    profPapers->second++;
                }
            }
        }

        for (pair<int, int>* profPapers: authorsPapers) {
            for (Paper* paper: papers) {
                if (profPapers->first == (*(paper->getAuthors()))[1]->getId()) {
                    if (profPapers->second == 2) {
                        if (!auxPapers[1]){
                            auxPapers[1] = paper;
                            auxPapers[1]->setSlot(prevClusterSize+1);
                        }
                        else {
                            auxPapers[2] = paper;
                            auxPapers[2]->setSlot(prevClusterSize+0);
                        } 
                    }
                    else {
                        if (!auxPapers[0]) {
                            auxPapers[0] = paper;
                            auxPapers[0]->setSlot(prevClusterSize+2);
                        }
                        else if (!auxPapers[3]) {
                            auxPapers[3] = paper;
                            auxPapers[3]->setSlot(prevClusterSize+3);
                        }
                        else if (!auxPapers[4]) {
                            auxPapers[4] = paper;
                            auxPapers[4]->setSlot(prevClusterSize+4);
                        }
                    }
                }
            }
        }
        // 1 0 2
        // 0 1 2
        // 0 1 2
        // 3 0 2
        // 2 0 3
        auxPapers[0]->addAuthor(*(*(auxPapers[1]->getAuthors()))[1]);
        auxPapers[0]->addAuthor(*(*(auxPapers[3]->getAuthors()))[1]);
        auxPapers[1]->addAuthor(*(*(auxPapers[0]->getAuthors()))[1]);
        auxPapers[1]->addAuthor(*(*(auxPapers[4]->getAuthors()))[1]);
        auxPapers[2]->addAuthor(*(*(auxPapers[0]->getAuthors()))[1]);
        auxPapers[2]->addAuthor(*(*(auxPapers[4]->getAuthors()))[1]);
        auxPapers[3]->addAuthor(*(*(auxPapers[4]->getAuthors()))[1]);
        auxPapers[3]->addAuthor(*(*(auxPapers[1]->getAuthors()))[1]);
        auxPapers[4]->addAuthor(*(*(auxPapers[1]->getAuthors()))[1]);
        auxPapers[4]->addAuthor(*(*(auxPapers[3]->getAuthors()))[1]);

        papers.clear();
        papers = auxPapers;
        auxPapers.clear();
        auxPapers = vector<Paper*>(5, nullptr);
    }
    else if (numProfessors == 4 && numPapers == 4){
        // 2 0 3
        // 1 0 3
        // 0 1 2
        // 3 1 2
        papers[0]->setSlot(prevClusterSize+0);
        papers[0]->addAuthor(*(*(papers[2]->getAuthors()))[1]);
        papers[0]->addAuthor(*(*(papers[3]->getAuthors()))[1]);
        papers[1]->setSlot(prevClusterSize+1);
        papers[1]->addAuthor(*(*(papers[2]->getAuthors()))[1]);
        papers[1]->addAuthor(*(*(papers[3]->getAuthors()))[1]);
        papers[2]->setSlot(prevClusterSize+2);
        papers[2]->addAuthor(*(*(papers[1]->getAuthors()))[1]);
        papers[2]->addAuthor(*(*(papers[0]->getAuthors()))[1]);
        papers[3]->setSlot(prevClusterSize+3);
        papers[3]->addAuthor(*(*(papers[1]->getAuthors()))[1]);
        papers[3]->addAuthor(*(*(papers[0]->getAuthors()))[1]);
    }
}
