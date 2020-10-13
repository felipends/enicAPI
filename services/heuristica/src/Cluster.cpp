#include "Cluster.h"
#include <sstream>
#include "./utils/delete_vector.h"

Cluster::Cluster() {
    dummyCluster = false;
    intraClusterValue = -100;
}

Cluster::~Cluster() {
    deleteVectorElements(&clusterValues);
}

Cluster::Cluster(int intraClusterValue, int index, 
    int clusterSize, bool dummyCluster, vector<Paper* >* papers, int minSize) {
    this->intraClusterValue = intraClusterValue;
    this->index = index;
    this->clusterSize = clusterSize;
    this->dummyCluster = dummyCluster;
    this->minSize = minSize;

    // faz com que os clustervalues sejam gerados (talvez nem precise)
    for (Paper *p : *papers) {
        add(p);
    }
    // cout << "intra cluster: " << intraClusterValue << endl;
}

Cluster::Cluster(int clusterSize, int minSize, int index) {
    dummyCluster = false;
    this->clusterSize = clusterSize;
    this->minSize = minSize;
    this->index = index;
}

Cluster::Cluster(int index, vector<Paper*> *papers, bool dummyCluster, int minSize, int clusterSize) {
    this->index = index;
    this->dummyCluster = dummyCluster;
    this->minSize = minSize;
    this->clusterSize = clusterSize;
    for (Paper *p : *papers) {
        add(p);
    }
}

Cluster* Cluster::createDummyClusuter(int index, vector<Paper*> *papers) {
    return new Cluster(index, papers, true, 0, 10);
}

void Cluster::setClusterSize(int clusterSize) {
    this->clusterSize = clusterSize;
}

int Cluster::getClusterSize() {
    return clusterSize;
}

int Cluster::sumClustersValues(vector<Cluster*> *clusters) {
    int sum = 0;
    for (Cluster *c : *clusters) {
        sum += c->getClusterValue();
    }
    return sum;
}

int Cluster::calculateBenefitOfInsertPaper(Paper *paper, bool returnNegativeIfEmpty) {
    if (dummyCluster) {
        return dummyClusterValue;
    }
    if (papers.size() == 0) {
        return returnNegativeIfEmpty ? -1 : 0;
    }
    int sum = 0;
    for (Paper* p : papers) {
        sum += BenefitsUtil::getBenefit(p, paper);
    }
    return sum;
}

void Cluster::add(Paper* paper) {    
    int size = papers.size();    
    vector<Paper *> otherPapers;
    if (size >= 1) {
        for (Paper* p : papers) {
            otherPapers.push_back(p);
        }
    }
    papers.push_back(paper);
    if (size == 0)
        return;

    for (Paper *p : otherPapers) {
        if (p->index == paper->index) {
            cout << "uepa!" << endl;
            cout << "cluster: " << index << endl;
            cout << "papers no cluster: " << endl;
            for (Paper *p2 : papers) {
                cout << p2->index << " ";
            }
            cout << "tentando inserir paper: " << paper->index << endl;
            throw std::invalid_argument("Cannot insert the paper in the cluster value.");
        }
        clusterValues.push_back(new ClusterValue(paper, p));
    }
}

void Cluster::removePaper(Paper* paper) {
    // Remove os ClusterValues que contém o paper que se deseja remover
    for (auto it = clusterValues.begin(); it != clusterValues.end();) {
        auto cv = *it;
        if (cv->hasPaper(paper)) {
            it = clusterValues.erase(it);
            delete cv;
        } else {
            it++;
        }         
    }
    // clusterValues.erase(remove_if(clusterValues.begin(), clusterValues.end(), 
    //     [paper](ClusterValue *c) { return c->hasPaper(paper); }), clusterValues.end());
    papers.erase(std::remove(papers.begin(), papers.end(), paper), papers.end());
}

bool Cluster::checkLimits(int numMinPapers) {
    auto numPapers = papers.size();
    return numMinPapers <= numPapers && numPapers <= clusterSize;
}

bool Cluster::isClusterViable() {
    if (papers.size() == 0)
        return true;

    if (clusterSize <= 2)
        return checkLimits(1);
    else if (3 <= clusterSize && clusterSize <= 4)
        return checkLimits(2);
    else
        return checkLimits(3);
}

bool Cluster::canRemoveFromCluster() {
    auto numPapers = papers.size();
    return dummyCluster || numPapers - 1 >= minSize || numPapers < minSize; 
}

int Cluster::getClusterValue() {
    // o dummyCluster não conta para função objetiva
    if (dummyCluster) 
        return 0;

    int sum = 0;
    for (auto cv : clusterValues) {
        int value = cv->getValue();
        // cout << value << " + ";
        sum += value;
    }
    // cout << endl;
    // cout << " total: " << sum << endl;
    return sum;
}

int Cluster::clusterValueOrDummyCluster() {
    return dummyCluster ? dummyClusterValue : getClusterValue();
}

bool Cluster::isNotFull() {
    return papers.size() < clusterSize;
}

bool Cluster::isEmpty() {
    return papers.size() == 0;
}

bool Cluster::isDummyCluster() {
    return dummyCluster;
}

void Cluster::clearPapers() {
    // Remove os objetos ClusterValue do vector
    for (auto it = clusterValues.begin(); it != clusterValues.end();) {
        auto aux = *it;
        it = clusterValues.erase(it);
        delete aux;
    }
    clusterValues.clear();    
    papers.clear(); // não precisa deleta os papers do vector 
}

int Cluster::calculatePaperRemoveValue(Paper* paper) {
    if (dummyCluster) {        
        return dummyClusterValue;
    }
    // retorna o somotório das contribuições desse paper para o cluster em que se encontra
    int sum = 0;
    for (auto cv : clusterValues) {
        if (cv->hasPaper(paper))
            sum += cv->getValue();
    }        
    return sum;
}

vector<MovementInfo*> *Cluster::calculateRemoveMovement() {
    vector<MovementInfo*>* movements = new vector<MovementInfo*>();
    if (dummyCluster) {
        for (Paper *p : papers) {
            movements->push_back(new MovementInfo(p, this, dummyClusterValue));
        }
    } else {
        for (Paper *p : papers) {            
            int removeValue = 0;
            for(ClusterValue *cv : clusterValues) {
                if (cv->hasPaper(p)) {
                    removeValue += cv->getValue();
                }
            }
            movements->push_back(new MovementInfo(p, this, removeValue));
        }
    }
    return movements;
}

Cluster* Cluster::copy() {
    intraClusterValue = getClusterValue();
    // cout << "copy: " << intraClusterValue << endl;
    return new Cluster(intraClusterValue, index, 
        clusterSize, dummyCluster, &papers, minSize);
}

vector<Paper*> *Cluster::getPapers() {
    return &papers;
}

string Cluster::generateHash() {
    if (papers.size() == 0)
        return "";
    sort(papers.begin(), papers.end(), 
        [](Paper *p1, Paper *p2)->bool {
            return p1->getId() < p2->getId();
        });

    stringstream ss;
    ss << "c" << clusterSize << "-";
    for (int i = 0; i < papers.size(); i++)  {
        if(i != 0)
            ss << ",";
        ss << papers[i]->getId();
    }
    return ss.str();
}

string Cluster::toString() {
    if (papers.size() == 0)
        return "";
    sort(papers.begin(), papers.end(), 
        [](Paper *p1, Paper *p2)->bool {
            return p1->getId() < p2->getId();
        });

    stringstream ss;
   
    for (int i = 0; i < papers.size(); i++)  {
        if(i != 0)
            ss << ",";
        ss << papers[i]->getId();
    }
    return ss.str();
}

bool Cluster::hasPaper(Paper* paper) {
    int papersSize = papers.size();
    for (int i = 0; i < papersSize; i++) {
        if (papers[i]->index == paper->index)
            return true;
    }
    return false;
}

int Cluster::getNumTopicOccurences(Topic* topic) {
    int papersSize = papers.size();
    int numOccurences = 0;
    for (Paper* p : papers) {
        for (Topic* t : *p->getTopics()) {
            if (t->getId() == topic->getId()) 
                numOccurences++;
        }
    }
    return numOccurences;
}

int Cluster::getIntraClusterValue() {    
    return intraClusterValue;
}

int Cluster::hasAuthorConstraint(int day, int slot) {
    int total = 0;
    for (Paper *paper : papers) {        
        bool hasConstraint = false;
        for (Author *author : *(paper->getAuthors())) {
            if (author->hasConstraint(day, slot)) {
                hasConstraint = true;
                break;
            }
        }
        if (hasConstraint) {
            total += 1;
        }
    }
    return total;
}