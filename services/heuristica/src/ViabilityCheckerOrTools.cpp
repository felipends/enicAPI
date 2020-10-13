#include "ViabilityCheckerOrTools.h"
#include <algorithm>
#include <ctime>
#include <string>
#include <cmath>
#include "models/Session.h"
#include "models/Paper.h"
#include "utils/delete_vector.h"

int ViabilityCheckerOrTools::maxParallelSessions = 2;

ViabilityCheckerOrTools::ViabilityCheckerOrTools(vector<Cluster*>* clusters, EventInfo* eventInfo, bool clearTimeSlots /*= true*/) {
    this->clusters = clusters;
    this->eventInfo = eventInfo;
    timeSlots = new vector<TimeSlot*>();
    this->clearTimeSlots = clearTimeSlots;
}

ViabilityCheckerOrTools::~ViabilityCheckerOrTools() {
    if (clearTimeSlots) {
        for (TimeSlot *t: *timeSlots) {
            deleteTimeSlots();
        }
        deleteTimeSlots();
    }

    int numDays = eventInfo->getNumDays();    
    for (int d = 0; d < numDays; d++) {
        delete dayTimeSlots[d];
    }
}

void ViabilityCheckerOrTools::extractTimeSlotsFromSessions() {
    vector<Session*> sessions = eventInfo->getSessions();
    int numDays = eventInfo->getNumDays();    

    DateTime* lastDay = sessions.at(0)->getDateTimeInit();
    DateTime* lastDateTime = sessions.at(0)->getDateTimeInit();

    int dayIndex = 0;
    int index = 0;
    int slotIndex = 0;
    timeSlots = new vector<TimeSlot*>();
    timeSlots->push_back(new TimeSlot(index++, dayIndex, slotIndex, sessions.at(0)));
    int size = sessions.size();
    for (int i = 1; i < size; i++) {
        Session* session = sessions.at(i);
        if (session->isSameDate(lastDay)) {
            if (session->isSameTime(lastDateTime)) {
                int lastTimeSlotIndex = timeSlots->size()-1;
                timeSlots->at(lastTimeSlotIndex)->addSession(session);
            } else {
                slotIndex++;
                lastDateTime = session->getDateTimeInit();
                timeSlots->push_back(new TimeSlot(index++, dayIndex, slotIndex, session));    
            }
        } else {
            slotIndex = 0;
            lastDay = lastDateTime = session->getDateTimeInit();
            dayIndex++;
            timeSlots->push_back(new TimeSlot(index++, dayIndex, slotIndex, session));
        }
    }
    for (int d = 0; d < numDays; d++) {
        dayTimeSlots[d] = new vector<TimeSlot*>();
    }
    for (TimeSlot *t : *timeSlots) {
        dayTimeSlots[t->getDay()]->push_back(t);
    }
}

int ViabilityCheckerOrTools::getMaxTopicsPerDay(Topic *topic) {
    int numPapersInTopic = eventInfo->getPapersTopic()[topic->index].size();
    if (eventInfo->getNumDays() == 1)
        return numPapersInTopic;

    return (numPapersInTopic > 20)
        ? (int) ceil((double)numPapersInTopic / 2)
        : numPapersInTopic;
}

bool ViabilityCheckerOrTools::isViable() {
    extractTimeSlotsFromSessions();

    MPSolver solver("ViabilityChecker", MPSolver::CBC_MIXED_INTEGER_PROGRAMMING);

    const double infinity = solver.infinity();

    vector<Session*> sessions = eventInfo->getSessions();
    int numDays = eventInfo->getNumDays(); 

    // Init y var
    int clustersSize = clusters->size();  
    MPObjective* const objective = solver.MutableObjective(); 
    objective->SetMaximization();

    vector<vector<vector<MPVariable*>>> y(clustersSize);
    for (int i = 0; i < clustersSize; i++) {
        vector<vector<MPVariable*>> aux(numDays);
        y[i] = aux;

        for (int d = 0; d < numDays; d++) {
            vector<TimeSlot*>& slotsFromDay = *(dayTimeSlots[d]);
            int slotsSize = slotsFromDay.size();
            vector<MPVariable*> aux2(slotsSize);
            y[i][d] = aux2;

            for (int h = 0; h < slotsSize; h++) {
                char var[100];
                sprintf(var, "Y(%d,%d,%d)", i, d, h);
                y[i][d][h] = solver.MakeBoolVar(var);

                TimeSlot *slot = slotsFromDay[h];

                if (slot->clusterCanEnter(clusters->at(i))) {                    
                    validVarMap[y[i][d][h]] = true;
                } else {
                    y[i][d][h] = 0;
                    validVarMap[y[i][d][h]] = false;
                }
            }
        }
    }

    // AddConstraintEveryClusterMustBeUsed
    for (int i = 0; i < clustersSize; i++) {
        MPConstraint* constraint;
        bool create = false;
        for (int d = 0; d < numDays; d++) {
            vector<TimeSlot*>& slotsFromDay = *(dayTimeSlots[d]);
            int slotsSize = slotsFromDay.size();
            for (int h = 0; h < slotsSize; h++) {
                if (validVarMap[y[i][d][h]]) {                    
                    if (!create) {
                        char name[100];
                        sprintf(name, "UseCluster_%d", i);
                        constraint = solver.MakeRowConstraint(1, 1, name);
                        create = true;
                    }
                    constraint->SetCoefficient(y[i][d][h], 1);
                }
            }
        }
    }
    
    // AddConstraintAuthorPerParallelSessions    
    vector<vector<Paper*>> papersAuthor = eventInfo->getPapersAuthor();
    vector<Author*> authors = eventInfo->getAuthors();
    for (Author* author : authors) {
        if (papersAuthor[author->index].size() <= 1) 
            continue;
        
        for (int d = 0; d < numDays; d++) { 
            vector<TimeSlot*>& slots = *(dayTimeSlots[d]);
            int slotsSize = slots.size();

            for (int h = 0; h < slotsSize; h++) {
                MPConstraint* constraint;
                bool create = false;

                for (int i = 0; i < clustersSize; i++) {
                    if (!validVarMap[y[i][d][h]])
                        continue;
                    
                    vector<Paper*> papers = papersAuthor[author->index];
                    int papersSize = papers.size();
                    Cluster* cluster = (*(clusters))[i];

                    for (int p = 0; p < papersSize; p++) {
                        if (cluster->hasPaper(papers[p])) {
                            if (!create) {
                                char name[100];
                                sprintf(name, "Author_%d_%d_%d", author->index, d, h);
                                constraint = solver.MakeRowConstraint(-infinity, 1, name);
                                create = true;
                            }
                            constraint->SetCoefficient(y[i][d][h], 1);
                            break;
                        }
                    }
                }
            }
        }
    }

    // AddConstraintClustersPerTimeSlot
     for (int d = 0; d < numDays; d++) {
        vector<TimeSlot*> &slots = *(dayTimeSlots[d]);
        int slotsSize = slots.size();
        for (int h = 0; h < slotsSize; h++) {
            MPConstraint* constraint;
            bool create = false;            
            
            for (int i = 0; i < clustersSize; i++) {
                if (!validVarMap[y[i][d][h]])
                    continue;
                
                if (!create) {
                    char name[100];
                    sprintf(name, "ClusterPerTimeSlot_%d_%d", d, h);
                    constraint = solver.MakeRowConstraint(-infinity, slots[h]->getNumSessions(), name);
                    create = true;
                }                
                constraint->SetCoefficient(y[i][d][h], 1);
            }            
        }
    }

    // AddContraintNumPapersPerTimeSlot
    for (int d = 0; d < numDays; d++) {
        vector<TimeSlot*> &slots = *(dayTimeSlots[d]);
        int slotsSize = slots.size();
        for (int h = 0; h < slotsSize; h++) {
            int maxPapersInTimeSlots = slots[h]->getNumPapers();
            MPConstraint* constraint;
            bool create = false;

            for (int i = 0; i < clustersSize; i++) {
                if (!validVarMap[y[i][d][h]])
                    continue;

                Cluster *cluster = (*(clusters))[i];
                int numPapersInCluster = cluster->getPapers()->size();
                if (!create) {
                    char name[100];
                    sprintf(name, "PapersPerTimeSlot_%d_%d", d, h);    
                    constraint = solver.MakeRowConstraint(-infinity, maxPapersInTimeSlots, name);
                    create = true;
                }
                constraint->SetCoefficient(y[i][d][h], numPapersInCluster);
            }            
        }
    }

	 //string model;
	 //solver.ExportModelAsLpFormat(false, &model); 
	 //cout << model << endl; 

    cout << "CBC solving..." << endl;
    const MPSolver::ResultStatus result_status = solver.Solve();
    // Check that the problem has an optimal solution.
    if (result_status != MPSolver::OPTIMAL) {
        cout << "CBC inviable" << endl;
        return false;
    } 
	
	insertClustersInTimeSlots(y);

    cout << "CBC viable" << endl;
	/* abort(); */
    return true;
}

vector<TimeSlot*>* ViabilityCheckerOrTools::getTimeSlots() {    
    return timeSlots;
}

void ViabilityCheckerOrTools::deleteTimeSlots() {
    deleteVector(timeSlots);
}

void ViabilityCheckerOrTools::insertClustersInTimeSlots(vector<vector<vector<MPVariable*>>> &y) {
    int numDays = eventInfo->getNumDays();
    for (int d = 0; d < numDays; d++) {
        vector<TimeSlot*> slots = getTimeSlotsFromDay(d);
        int slotsSize = slots.size();
        for (int h = 0; h < slotsSize; h++) {
            int clustersSize = clusters->size();
            for (int i = 0; i < clustersSize; i++) {
                if (!validVarMap[y[i][d][h]])
                    continue;

                double value = y[i][d][h]->solution_value();             
                if (value > 0.5) {                    
                    slots[h]->addCluster(clusters->at(i)->copy());
                }
            }
        }
    }
}

vector<TimeSlot*> ViabilityCheckerOrTools::getTimeSlotsFromDay(int day) {
    vector<TimeSlot*> result;
    for (TimeSlot* timeslot : *timeSlots) {
        if (timeslot->getDay() == day)
            result.push_back(timeslot);
    }

    sort(result.begin(), result.end(),
        [](TimeSlot *t1, TimeSlot *t2)->bool {
            time_t time1 = t1->getDateTimeInit()->getDateTime();
            time_t time2 = t2->getDateTimeInit()->getDateTime();
            return difftime(time1, time2) < 0.0;
        });
    return result;
}
