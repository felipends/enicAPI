#include "ContiguousClusteringOrTools.h"

ContiguousClusteringOrTools::ContiguousClusteringOrTools(vector<Author*>& allProfesors, vector<int>* professorsIndices, EventInfo* event, Cluster* cluster, int numClustersInSolution)
{
    this->event = event;
    this->cluster = cluster;
    this->numClustersInSolution = numClustersInSolution;

    this->allProfesors = allProfesors;
    this->professorsIndices = professorsIndices;

    this->solutionClusters = new vector<Cluster*>(numClustersInSolution, nullptr);
}

void ContiguousClusteringOrTools::splitCluster()
{
    MPSolver solver("ClusterSplit", MPSolver::CBC_MIXED_INTEGER_PROGRAMMING);

    vector<vector<int>> c(cluster->getClusterSize(), vector<int>(cluster->getClusterSize(), 0));
    vector<Paper*>* papers = cluster->getPapers();
    int numPapers = papers->size();

    for (int i = 0; i < numPapers; i++) {
        for (int j = 0; j < numPapers; j++) {
            if (i == j) continue;
            c[i][j] = papers->at(i)->calculateBenefit(*(papers->at(j)));
        }
    }

    // initializing w_ijk variable
    vector<vector<vector<MPVariable*>>> w(numPapers); 
    for (int i = 0; i < numPapers; i++) {
        w[i] = vector<vector<MPVariable*>>(numPapers);
        for (int j = 0; j < numPapers; j++) {
            w[i][j] = vector<MPVariable*>(numClustersInSolution);
            for (int k = 0; k < numClustersInSolution; k++) {
                char varName[100];
                sprintf(varName, "w(%d,%d,%d)", i, j, k);
                w[i][j][k] = solver.MakeBoolVar(varName);
            }
        }
    }

    // initialize z_ik variable
    vector<vector<MPVariable*>> z(numPapers);
    for (int i = 0; i < numPapers; i++) {
        z[i] = vector<MPVariable*>(numClustersInSolution);
        for (int k = 0; k < numClustersInSolution; k++) {
            char varName[100];
            sprintf(varName, "z(%d,%d)", i, k);
            z[i][k] = solver.MakeBoolVar(varName);
        }
    }

    // add constraint w_ijk + 1 >= z_ik + z_jk
    for (int i = 0; i < numPapers; i++) {
        MPConstraint* constraint;
        for (int j = i+1; j < numPapers; j++) {
            for (int k = 0; k < numClustersInSolution; k++){
                char name[100];
                sprintf(name, "w(%d,%d,%d)-z(%d,%d)-z(%d,%d)", i, j, k, i, k, j, k);
                constraint = solver.MakeRowConstraint(-1, solver.infinity(), name);
                constraint->SetCoefficient(w[i][j][k], 1);
                constraint->SetCoefficient(z[i][k], -1);
                constraint->SetCoefficient(z[j][k], -1);
            }
        }
    }

    // add constraint w_ijk <= zik
    for (int i = 0; i < numPapers; i++) {
        MPConstraint* constraint;
        for (int j = i+1; j < numPapers; j++) {
            for (int k = 0; k < numClustersInSolution; k++){
                char name[100];
                sprintf(name, "w(%d,%d,%d)-z(%d,%d)", i, j, k, i, k);
                constraint = solver.MakeRowConstraint(- (solver.infinity()), 0, name);
                constraint->SetCoefficient(w[i][j][k], 1);
                constraint->SetCoefficient(z[i][k], -1);
            }
        }
    }

    // add constraint w_ijk <= zjk
    for (int i = 0; i < numPapers; i++) {
        MPConstraint* constraint;
        for (int j = i+1; j < numPapers; j++) {
            for (int k = 0; k < numClustersInSolution; k++){
                char name[100];
                sprintf(name, "w(%d,%d,%d)-z(%d,%d)", i, j, k, j, k);
                constraint = solver.MakeRowConstraint(- (solver.infinity()), 0, name);
                constraint->SetCoefficient(w[i][j][k], 1);
                constraint->SetCoefficient(z[j][k], -1);
            }
        }
    }

    // add constraint every paper must be in a cluster
    for (int i = 0; i < numPapers; i++) {
        MPConstraint* constraint;
        char name[100];
        sprintf(name, "paper(%d)isInACluster", i);
        constraint = solver.MakeRowConstraint(1, 1, name);
        for (int k = 0; k < numClustersInSolution; k++) {
            constraint->SetCoefficient(z[i][k], 1);
        }
    }

    // add constraint every cluster must have between 1 and 6 papers
    for (int k = 0; k < numClustersInSolution; k++) {
        MPConstraint* constraint;
        char name[100];
        sprintf(name, "cluster(%d)Has6orLess", k);
        constraint = solver.MakeRowConstraint(1, 6, name);
        for (int i = 0; i < numPapers; i++) {
            constraint->SetCoefficient(z[i][k], 1);
        }
    }

    // extract authors form papers
    vector<Author*> authors;

    for (Paper* paper: *papers) {
        if(find(authors.begin(), authors.end(), paper->getAuthors()->at(1)) != authors.end())
            continue;
        else 
            authors.push_back(paper->getAuthors()->at(1));
    }

    vector<int> authorsNumPapers(authors.size(), 0);
    for (int i = 0; i < authors.size(); i++) {
        for (Paper* paper: *papers) {
            if(paper->hasAuthor(authors.at(i)))
                authorsNumPapers.at(i)++;
        }
    }

    vector<int> aux = authorsNumPapers;
    sort(aux.begin(), aux.end(), greater<int>());
    if (!((aux.size() >= 3 && aux.at(0) == 4 && aux.at(1) == 4 && aux.at(2) == 4) || 
        (aux.size() > 3 && aux.at(0) == 4 && aux.at(1) == 4 && aux.at(2) == 3 && aux.at(3) == 1) || 
        (aux.size() > 3 && aux.at(0) == 5 && aux.at(1) == 3 && aux.at(2) == 2 && aux.at(3) == 2) || 
        (aux.size() >= 3 && aux.at(0) == 5 && aux.at(1) == 5 && aux.at(2) == 2) || 
        (aux.size() >= 3 && aux.at(0) == 5 && aux.at(1) == 4 && aux.at(2) == 3))) {
        // add constraint every paper of author i must be in the same cluster
        for (int k = 0; k < numClustersInSolution; k++) {
            for (int o = 0; o < authors.size(); o++) {
                if (authorsNumPapers.at(o) > 6 || authorsNumPapers.at(o) <= 1) continue;
                MPConstraint* constraint;
                for (int i = 0; i < numPapers; i++) {
                    if (!papers->at(i)->hasAuthor(authors.at(o))) 
                        continue;
                    for (int j = i+1; j < numPapers; j++) {
                        if (i == j || !papers->at(j)->hasAuthor(authors.at(o)))
                            continue;

                        char name[100];
                        sprintf(name, "papers(%d,%d)fromAuthor(%d)areIn_cluster(%d)", i, j, authors.at(o)->getId(), k);
                        constraint = solver.MakeRowConstraint(0, 0, name);
                        constraint->SetCoefficient(z[i][k], 1);
                        constraint->SetCoefficient(z[j][k], -1);
                    }
                }
            }
        }
    }

    // objective fuction
    MPObjective* objective = solver.MutableObjective();
    for (int i = 0; i < numPapers; i++) {
        for (int j = i+1; j < numPapers; j++) {
            for (int k = 0; k < numClustersInSolution; k++) {
               objective->SetCoefficient(w[i][j][k], c[i][j]); 
            }
        }
    }
    objective->SetMaximization();

    const MPSolver::ResultStatus result_status = solver.Solve();
    if (result_status != MPSolver::OPTIMAL) {
        cout << "failed" << endl;
        return;
    }

    cout << "sucess" << endl;
    setSolutionClusters(w, numPapers, *papers);
}

void ContiguousClusteringOrTools::setSolutionClusters(vector<vector<vector<MPVariable*>>>& w, int numPapers, vector<Paper*>& papers)
{
    bool alreadyInserd = false;
    vector<int> insertedIds;
    for (int i = 0; i < numClustersInSolution; i++) {
        solutionClusters->at(i) = new Cluster(6, 1, i); 
    }
    for (int i = 0; i < numPapers; i++) {
        for (int j = i+1; j < numPapers; j++) {
            for (int k = 0; k < numClustersInSolution; k++) {
                if (w[i][j][k]->solution_value()) {
                    if (!alreadyInserd && find(insertedIds.begin(), insertedIds.end(), papers.at(i)->getId()) == insertedIds.end()) {
                        insertedIds.push_back(papers.at(i)->getId());
                        this->solutionClusters->at(k)->add(papers.at(i));
                        alreadyInserd = true;
                    }                
                    if(find(insertedIds.begin(), insertedIds.end(), papers.at(j)->getId()) == insertedIds.end()) { 
                        this->solutionClusters->at(k)->add(papers.at(j));
                        insertedIds.push_back(papers.at(j)->getId());
                    }
                    break;
                }
            }
        }
        alreadyInserd = false;
    } 

    for (Cluster* c: *solutionClusters) {
        cout << "cluster size: " << c->getPapers()->size() << endl;
        auto cCluster = new ContiguousCluster(this->allProfesors, this->professorsIndices, *(c->getPapers()), c->index == 0 ? 0 : solutionClusters->at(0)->getPapers()->size());
        cCluster->makeClusterContiguous();
    }

    solutionClusters->clear();
}
