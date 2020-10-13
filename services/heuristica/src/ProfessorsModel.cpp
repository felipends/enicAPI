#include "ProfessorsModel.h"

ProfessorsModel::ProfessorsModel(Cluster* cluster, vector<Author*>& allAuthors, vector<int>* extraIndices)
{   
    this->cluster = cluster;
    this->numSlots = cluster->getPapers()->size();
    this->papers = *(cluster->getPapers());
    this->numExtraProfessorsNeeded = 0;

    this->numProfessors = 0;
    for (Paper* p: papers) {
        if (find(professorsId.begin(), professorsId.end(), p->getAuthors()->at(1)->getId()) == professorsId.end()) {
            professorsId.push_back(p->getAuthors()->at(1)->getId());
            authors.push_back(p->getAuthors()->at(1));
            numProfessors++;
        }
    }

	cout << "cluster professors: " << professorsId.size() << endl;
    this->setClusterTopics();

    // checks if it's necessary to add extra examiners
    if (numProfessors < 3) {
        int extraProfessor;
        int numNeededProf = (numProfessors > 1) ? 1 : 2; 
        for (int i = 0; i < (numNeededProf); i++) {
            try{
                
                extraProfessor = chooseExtraProfessor(allAuthors, extraIndices);

            } catch (int e) { 
                cerr << "Exception: function chooseExtraProfessor returned " << e << endl;
                cerr << "No extra professor was found." << endl;
                abort();
            }
            professorsId.push_back(extraProfessor);
            Author* extraProfessorAux = *(find_if(allAuthors.begin(), allAuthors.end(), [&](Author* a){ return (a->index == extraProfessor) ? true : false;}));
            authors.push_back(extraProfessorAux);
            numProfessors++;
            numExtraProfessorsNeeded++;
        }
		cout << "extra professors needed: " << numNeededProf << endl;
		this->cluster->setNumExtraProfs(numNeededProf);
	} else {
		this->cluster->setNumExtraProfs(0);
	}

    for (int pId: professorsId) {
        pair<int, vector<int>> auxPair;
        auxPair.first = pId;
        for (Paper* paper: papers) {
            if (paper->getAuthors()->at(1)->getId() == pId) {
                auxPair.second.push_back(paper->getId());
            }
        }
        professorPapers.push_back(auxPair);
    }

}

void ProfessorsModel::setClusterTopics()
{
    for (Paper* p: papers) {
        for (Topic* topic: *(p->getTopics())) {
            if (find(clusterTopics.begin(), clusterTopics.end(), topic->getId()) == clusterTopics.end())
                clusterTopics.push_back(topic->getId());
        }
    }
}

int ProfessorsModel::chooseExtraProfessor(vector<Author*>& allAuthors, vector<int>* extraIndices)
{
    int professorBenefit = 0;
    int biggestBenefit = 0;
    int extraProfessorId;

    for (int prof: *(extraIndices)) {
        Author* profAux = allAuthors.at(prof)->getAuthorTopics()->empty() ? nullptr : allAuthors.at(prof);
        if(!profAux) continue;
        for (Topic* profTopic: *(profAux->getAuthorTopics())) {
            if (find(clusterTopics.begin(), clusterTopics.end(), profTopic->getId()) != clusterTopics.end()) {
                professorBenefit += 10;
            }
        }         

        if (professorBenefit > biggestBenefit) {
            biggestBenefit = professorBenefit;
            extraProfessorId = prof;
        }
        professorBenefit = 0;
    }    

    if (biggestBenefit == 0) {
        extraProfessorId = -1;
        throw -1;
    }

    extraIndices->erase(remove(extraIndices->begin(), extraIndices->end(), extraProfessorId), extraIndices->end());

    return extraProfessorId;
}

int ProfessorsModel::allocProfessorsModel()
{
    int value;

    MPSolver solver("ProfessorsAllocation", MPSolver::CBC_MIXED_INTEGER_PROGRAMMING);
    //initialize variables h_is, x_is, l_is and y_is
    vector<vector<MPVariable*>> h(numProfessors);
    vector<vector<MPVariable*>> x(numProfessors);
    vector<vector<MPVariable*>> l(numProfessors);
    vector<vector<MPVariable*>> y(numProfessors);
    for (int i = 0; i < numProfessors; i++) {
        h[i] = vector<MPVariable*>(numSlots+1);  
        x[i] = vector<MPVariable*>(numSlots+1);  
        l[i] = vector<MPVariable*>(numSlots+1);  
        y[i] = vector<MPVariable*>(numSlots+1);  
        for (int s = 0; s < numSlots+1; s++) {
            char varName[100];
            sprintf(varName, "h(%d,%d)", i, s);
            h[i][s] = solver.MakeBoolVar(varName);

            sprintf(varName, "x(%d,%d)", i, s);
            x[i][s] = solver.MakeBoolVar(varName);

            sprintf(varName, "l(%d,%d)", i, s);
            l[i][s] = solver.MakeBoolVar(varName);

            sprintf(varName, "y(%d,%d)", i, s);
            y[i][s] = solver.MakeBoolVar(varName);
        }
    }

    // add constraint 1 professor must be an advisor 
    for (int s = 1; s < numSlots+1; s++) {
        MPConstraint* constraint;
        char name[100];
        for (int i = 0; i < numProfessors; i++) {
            sprintf(name, "slot(%d)_has(%d)", s, i);
            constraint = solver.MakeRowConstraint(-solver.infinity(), 0, name);
            constraint->SetCoefficient(y[i][s], 1);
            constraint->SetCoefficient(x[i][s], -1);
        }
    }

    // add constraint professors are advisor of their papers  
    for (int i = 0; i < numProfessors; i++) {
        int c = (int) professorPapers[i].second.size();
        MPConstraint* constraint;
        char name[100];
        sprintf(name, "prof(%d)_hasC", i);
        constraint = solver.MakeRowConstraint(c, c, name);
        for (int s = 1; s < numSlots+1; s++) {
            constraint->SetCoefficient(y[i][s], 1);
        }
    }

    // add constraint every slot has 1 advisor
    for (int s = 1; s < numSlots+1; s++) {
        MPConstraint* constraint;
        char name[100];
        sprintf(name, "slot(%d)_has1", s);
        constraint = solver.MakeRowConstraint(1, 1, name);
        for (int i = 0; i < numProfessors; i++) {
            constraint->SetCoefficient(y[i][s], 1);
        }
    }
    
    //add constraint every slot must have 3 professors
    for (int s = 1; s < numSlots+1; s++) {
        MPConstraint* constraint;
        char name[100];
        sprintf(name, "slot(%d)_has3", s);
        constraint = solver.MakeRowConstraint(3, 3, name);
        for (int i = 0; i < numProfessors; i++) {
            constraint->SetCoefficient(x[i][s], 1);
        }
    }

    //add constraint every professor must be in less than d slots and more than c
    int bound = ceil((2.0*numSlots)/numProfessors);
    for (int i = 0; i < numProfessors; i++) {
        int numProfessorPapers = this->professorPapers[i].second.size();
        int c = numProfessorPapers > 3 ? numProfessorPapers : 3;
        int d = max(numSlots, numProfessorPapers + bound) >= 3 ? max(numSlots, numProfessorPapers + bound) : 3;

        MPConstraint* constraint;

        char name[100];
        sprintf(name, "professor(%d)_has(%d)_avals", i, c);
        constraint = solver.MakeRowConstraint(c, d, name);

        for (int s = 1; s < numSlots+1; s++) {
            constraint->SetCoefficient(x[i][s], 1);
        }
    }

    //add constraint to count slots gap
    for (int s = 1; s < numSlots+1; s++) {
        for (int i = 0; i < numProfessors; i++) {
            MPConstraint* constraint;

            char name[100];
            sprintf(name, "count_gap_of(%d)_til(%d)", i, s);
            constraint = solver.MakeRowConstraint(-(solver.infinity()), 1, name);

            for (int sLine = 0; sLine <= s-1; sLine++) {
                constraint->SetCoefficient(x[i][sLine], (double) 1.0/numSlots);
            }
            constraint->SetCoefficient(x[i][s], -numSlots);
            constraint->SetCoefficient(l[i][s], 1);
            constraint->SetCoefficient(h[i][s-1], 1);
            constraint->SetCoefficient(h[i][s], -1);
        }
    }

    //add constraint to know if its the last slot of professor
    for (int s = 1; s < numSlots+1; s++) {
        for (int i = 0; i < numProfessors; i++) {
            MPConstraint* constraint;
            char name[100];
            for (int sLine = s+1; sLine < numSlots+1; sLine++) {
                sprintf(name, "slot(%d)_isLastOf(%d)_and(%d)isnt", s, i, sLine);
                constraint = solver.MakeRowConstraint(0, solver.infinity(), name);
                constraint->SetCoefficient(x[i][sLine], -1);
                constraint->SetCoefficient(l[i][s], 1);
            }
        }
    }

    //for every professor slot 0 has 0 gaps
    for (int i = 0; i < numProfessors; i++) {
        MPConstraint* constraint;
        char name[100];
        sprintf(name, "h(%d, 0)", i);
        constraint = solver.MakeRowConstraint(0, 0, name);
        constraint->SetCoefficient(h[i][0], 1);
    }

    // FO
    MPObjective* objective = solver.MutableObjective();
    for (int i = 0; i < numProfessors; i++) {
        for (int s = 0; s < numSlots + 1; s++) {
            objective->SetCoefficient(h[i][s], 1);
        }
    }

    objective->SetMinimization();

    const MPSolver::ResultStatus result_status = solver.Solve();
    if (result_status != MPSolver::OPTIMAL) {
        cout << "failed" << endl;
        return 0;
    }

    value = objective->Value();

    setExaminers(x, y);

    cout << "sucess" << endl;

	this->cluster->setNumGaps(value);
    return value;
}

void ProfessorsModel::setExaminers(vector<vector<MPVariable*>>& x, vector<vector<MPVariable*>>& y)
{
    #ifdef DEBUG
        /*
            DEBUG INFO
            check professors in slots
        */
        for (int s = 1; s < numSlots+1; s++) {
            cout << "slot " << s << ": " << endl;
            for (int i = 0; i < numProfessors; i++) {
                if (x[i][s]->solution_value()) {
                    cout << i << " ";
                }
            }
            cout << endl;
        }
        //END DEBUG
    #endif
    vector<bool> freeSlots(numSlots, true); 

    for (Paper* paper: papers) {
        for (int i = 0; i < numProfessors; i++) {
            for (int s = 1; s < numSlots+1; s++) {
                if (y[i][s]->solution_value() && paper->isAdvisor(professorsId[i]) && freeSlots[s-1]) {
                    paper->setSlot(s-1);
                    freeSlots[s-1] = false;
                    break;
                }
            }
        }
    }

    for (Paper* paper: papers) {
        for (int s = 1; s < numSlots+1; s++) {
            if (paper->getSlot() != s-1) continue;
            for (int i = 0; i < numProfessors; i++) {
                if (x[i][s]->solution_value() && !(y[i][s]->solution_value())) {
                    Author* auxAuthor = *(find_if(authors.begin(), authors.end(), [&](Author* a){ return (a->index == professorsId[i]) ? true : false;}));
                    paper->addAuthor(*auxAuthor);
                }
            }
        }
    }
}

int ProfessorsModel::getNumExtraProfessorsNeeded(){
    return this->numExtraProfessorsNeeded;
}
