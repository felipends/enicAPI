#include "Heuristic.h"
#include "./LocalSearch.h"
#include "IViabilityChecker.h"
#include "ViabilityCheckerFactory.h"
#include "neighborhood/Perturbation.h"
#include "utils/BenefitsUtil.h"
#include "utils/ClusterBenefit.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <stdio.h>
#include <vector>

//#include "./SetPartition.h"
#include "./utils/delete_vector.h"
#include "./utils/InstanceReader.h"
#include "models/Result.h"


Heuristic *Heuristic::instance;

Heuristic::Heuristic(EventInfo *eventInfo, string dictFileName) {
  srand(time(NULL));
  this->eventInfo = eventInfo;
  instance = this;
  this->ilsSolution = NULL;
  this->dictFileName = dictFileName;
}

Heuristic::~Heuristic() {
  deleteVectorElements(&clustersFromBestIteratonResult);
  deleteVectorElements(&clustersFromBestSolution);
  deleteVectorElements(&clusters);
}

int Heuristic::solve(InstanceReader* instanceReader) {
  printInstanceInfo();
  cout << "Precomputing papers benefits..." << endl;
  BenefitsUtil::calculate(&eventInfo->getPapers());
  BenefitsUtil::addPreferences(eventInfo->getPairs());

  ViabilityCheckerFactory::printCheckerVersion();
  cout << "Starting ILS..." << endl;
  int res = ILS(instanceReader);
  cout << "ILS result: " << res << endl;

  int total = Cluster::sumClustersValues(&clustersFromBestSolution);
  cout << ">>> Total: " << total << endl;

  saveResultToFile(ilsSolution);

  return total;
}

void Heuristic::saveResultToFile(vector<TimeSlot *> *timeSlots) {
  FILE *f = fopen("sols.txt", "w");

  if (timeSlots != NULL) {
    fprintf(f, "1\n%d\n", eventInfo->getNumSessions());
    for (TimeSlot *ts : *timeSlots) {
      fprintf(f, "%s", ts->getSolution().c_str());
      // cout << ts->getSolution() << endl;
      // cout << ts->toString() << endl;
    }
  } else {
    fprintf(f, "%d", 0);
  }

  fclose(f);
}

void Heuristic::printInstanceInfo() {
  cout << "Papers: " << eventInfo->getNumPapers() << endl;
  cout << "Topics: " << eventInfo->getNumTopics() << endl;
  cout << "Sessions: " << eventInfo->getNumSessions() << endl;
  eventInfo->printSessions();
  int numSlots = 0;
  for (Session *session : eventInfo->getSessions()) {
    numSlots += session->getNumPapers();
  }
  cout << "Num Slots: " << numSlots << endl;
}
int Heuristic::ILS(InstanceReader* instanceReader) {
  using namespace std::chrono;
  int bestSolution = 0;
  int maxIterationsILS = 30;
  double t0 = 1000;
  double alpha = 0.8;
  int maxConsecutivePertubationsWithoutImprovement =
      max(100, eventInfo->getNumPapers());

  Perturbation *perturbation = Perturbation::getInstance();
  RVNDSolutionStore *rvndSolutionStore = RVNDSolutionStore::getInstance();

  double time_initial_sol = 0;
  double time_local_search = 0;
  double time_perturb = 0;
  double time_checker = 0;
  for (int i = 0; i < maxIterationsILS; i++) {
	high_resolution_clock::time_point t1_initial_sol = high_resolution_clock::now();
    greedyConstruction();
	high_resolution_clock::time_point t2_initial_sol = high_resolution_clock::now();
	time_initial_sol += duration_cast<duration<double>>(t2_initial_sol - t1_initial_sol).count();

    int bestIterationSolution = Cluster::sumClustersValues(&clusters);
    saveBestIterationResult(&clusters);

    double t = t0;
    int iterILS = 0;
    while (iterILS < maxConsecutivePertubationsWithoutImprovement) {
      bool isSolutionFromRVNDStore = false;
	  
	  high_resolution_clock::time_point t1_local_search = high_resolution_clock::now();
      int solution = RVND(isSolutionFromRVNDStore);
	  high_resolution_clock::time_point t2_local_search = high_resolution_clock::now();
	  time_local_search += duration_cast<duration<double>>(t2_local_search - t1_local_search).count();
      bool isInviable = hasInviableClusters();

      if (isInviable) {
        cout << "isInviable" << endl;
        if (!acceptanceProbability(solution, bestIterationSolution, t)) {
          copyClustersFromBestIterationResult();
        }

		high_resolution_clock::time_point t1_perturb = high_resolution_clock::now();
        perturbation->perturb(&clusters);
		high_resolution_clock::time_point t2_perturb = high_resolution_clock::now();
		time_perturb += duration_cast<duration<double>>(t2_perturb - t1_perturb).count();

        t = t * alpha;
        iterILS++;
        continue;
      }

      if (!isSolutionFromRVNDStore) {
        rvndSolutionStore->storeSolution(&clusters, solution);
        clusterPool.addClustersIfNotInHash(&clusters);
      }

      if (solution > bestIterationSolution) {
        bestIterationSolution = solution;
        saveBestIterationResult(&clusters);
        iterILS = 0;
      } else {
        if (!acceptanceProbability(solution, bestIterationSolution, t)) {
          copyClustersFromBestIterationResult();
        }
      }
	  high_resolution_clock::time_point t1_perturb = high_resolution_clock::now();
      perturbation->perturb(&clusters);
	  high_resolution_clock::time_point t2_perturb = high_resolution_clock::now();
	  time_perturb += duration_cast<duration<double>>(t2_perturb - t1_perturb).count();

      t = t * alpha;
      iterILS++;
    }

    vector<Cluster *> *bestIterationClusters =
        getClustersNotEmpty(&clustersFromBestIteratonResult);

    IViabilityChecker *checker;
    checker = ViabilityCheckerFactory::create(bestIterationClusters, eventInfo, false);
	
	high_resolution_clock::time_point t1_checker = high_resolution_clock::now();
	bool viabilityChecking = checker->isViable();
	high_resolution_clock::time_point t2_checker = high_resolution_clock::now();
	time_checker += duration_cast<duration<double>>(t2_checker - t1_checker).count();

    if (bestIterationSolution > bestSolution && viabilityChecking) {

      if (ilsSolution != NULL) {
        for (TimeSlot *t : *ilsSolution) {
          t->deleteClusters();
        }
        deleteVectorElements(ilsSolution);
      }
      ilsSolution = checker->getTimeSlots();

      bestSolution = bestIterationSolution;
      saveBestSolution(&clustersFromBestIteratonResult);
    }
    delete checker;

    cout << "bestIteratonSolution: " << bestIterationSolution << endl;
    delete bestIterationClusters;
  }

  cout << "tempo de geraçao de solução inicial: " << time_initial_sol << endl;
  cout << "tempo de busca local: " << time_local_search << endl;
  cout << "tempo de perturbação: " << time_perturb << endl;
  cout << "tempo de checador: " << time_checker << endl;

  cout << "bestSolution: " << bestSolution << endl;

  double time_profs = 0;
  int numSaltos = 0, numExtraProfs = 0, numUsedSessions = 0;
  for (auto c : clustersFromBestSolution) {
    if (c->getPapers()->empty()) continue;
	numUsedSessions++;
    cout << c->index << ": " << c->getClusterValue() << ", ";
    cout << "papers: ";
	
    for (Paper *p : *(c->getPapers())) {
      cout << p->index << " ";
    }

    cout << endl;

    ProfessorsModel* pModel = new ProfessorsModel(c,*(instanceReader->getAuthors()), instanceReader->getExtraProfessorsIndices());
	high_resolution_clock::time_point t1_profs = high_resolution_clock::now();
    int pModelValue = pModel->allocProfessorsModel();
	high_resolution_clock::time_point t2_profs = high_resolution_clock::now();
	time_profs += duration_cast<duration<double>>(t2_profs - t1_profs).count();
	numSaltos += pModelValue;

    cout << "FO professors model: " << pModelValue << endl;
    cout << "Extra professors needed: " << pModel->getNumExtraProfessorsNeeded() << endl;
	numExtraProfs += pModel->getNumExtraProfessorsNeeded();
	for (TimeSlot* ts: *ilsSolution) {
	  ts->updateCluster(c->index, pModelValue, pModel->getNumExtraProfessorsNeeded());
	}
    eventInfo->setOFValue(bestSolution);
  }
  cout << "tempo de escalonamento: " << time_profs << endl;

  for (TimeSlot* ts: *ilsSolution) {
    ts->clustersToSessions();
  }

  vector<double> ilsTimes = { time_initial_sol, time_local_search, time_perturb, time_checker, time_profs };
  output = new OutputGenerator(dictFileName, eventInfo->getSessions(), instanceReader->getPapersInfo(), ilsTimes, numSaltos, numExtraProfs, numUsedSessions);

  return bestSolution;
}

OutputGenerator* Heuristic::getOutput(){
  return output;
}

int Heuristic::runILSOnce(vector<Cluster *> *clustersFromSp) {
  int bestSolution = 0;
  double t0 = 1000;
  double alpha = 0.8;
  int maxConsecutivePertubationsWithoutImprovement =
      max(100, eventInfo->getNumPapers());

  Perturbation *perturbation = Perturbation::getInstance();
  RVNDSolutionStore *rvndSolutionStore = RVNDSolutionStore::getInstance();

  deleteVectorElements(&clusters);

  clusters = *clustersFromSp;
  vector<Paper *> emptyPaper;
  emptyPaper.clear();
  clusters.push_back(
      Cluster::createDummyClusuter(clusters.size(), &emptyPaper));

  // ILS
  int bestIterationSolution = Cluster::sumClustersValues(&clusters);
  saveBestIterationResult(&clusters);

  double t = t0;
  int iterILS = 0;
  while (iterILS < maxConsecutivePertubationsWithoutImprovement) {
    bool isSolutionFromRVNDStore = false;
    int solution = RVND(isSolutionFromRVNDStore);
    bool isInviable = hasInviableClusters();

    if (isInviable) {
      if (!acceptanceProbability(solution, bestIterationSolution, t)) {
        copyClustersFromBestIterationResult();
      }

      perturbation->perturb(&clusters);
      t = t * alpha;
      iterILS++;
      continue;
    }

    if (!isSolutionFromRVNDStore) {
      rvndSolutionStore->storeSolution(&clusters, solution);
      clusterPool.addClustersIfNotInHash(&clusters);
    }

    if (solution > bestIterationSolution) {
      bestIterationSolution = solution;
      saveBestIterationResult(&clusters);
      iterILS = 0;
    } else {
      if (!acceptanceProbability(solution, bestIterationSolution, t)) {
        copyClustersFromBestIterationResult();
      }
    }

    perturbation->perturb(&clusters);
    t = t * alpha;
    iterILS++;
  }
  vector<Cluster *> *bestIterationClusters =
      getClustersNotEmpty(&clustersFromBestIteratonResult);
  IViabilityChecker *checker;
  checker = ViabilityCheckerFactory::create(bestIterationClusters, eventInfo, false);
  if (bestIterationSolution > bestSolution && checker->isViable()) {
    cout << "Solution " << bestIterationSolution << " is viable.";
    bestSolution = bestIterationSolution;
    saveBestSolution(&clustersFromBestIteratonResult);
  }
  delete checker;
  cout << "bestIteratonSolution: " << bestIterationSolution << endl;
  delete bestIterationClusters;

  cout << "bestSolution: " << bestSolution << endl;
  for (auto c : clustersFromBestSolution) {
    cout << c->index << ": " << c->getClusterValue() << ", ";
    cout << "papers: ";
    for (Paper *p : *(c->getPapers())) {
      cout << p->index << " ";
    }
    cout << endl;
  }
  return bestSolution;
}

void Heuristic::greedyConstruction() {
  deleteVectorElements(&clusters);
  clusters.clear();
  selectedPapers.clear();
  initClusters();
  populateClustersWithOnePaper();
  insertPapers();
}

void Heuristic::initClusters() {
  int index = 0;
  for (Session *s : eventInfo->getSessions()) {
    clusters.push_back(
        new Cluster(s->getNumPapers(), s->getNumMinPapers(), index++));
  }
}

void Heuristic::populateClustersWithOnePaper() {
  vector<int> selectedPapersIndexes;
  int numClusters = clusters.size();

  for (Cluster *cluster : clusters) {
    int index;
    do {
      index = rand() % numClusters;
    } while (find(selectedPapersIndexes.begin(), selectedPapersIndexes.end(),
                  index) != selectedPapersIndexes.end());

    Paper *paper = eventInfo->getPapers().at(index);
    selectedPapersIndexes.push_back(index);
    cluster->add(paper);
    selectedPapers.push_back(paper->getId());
  }
}

bool Heuristic::insertPapers() {
  vector<Paper *> papers;
  for (Paper *p : eventInfo->getPapers()) {
    if (find(selectedPapers.begin(), selectedPapers.end(), p->getId()) ==
        selectedPapers.end()) {
      papers.push_back(p);
    }
  }

  alocatePapersInClustersByBestBenefit(&papers);
  papers.clear();
  vector<Cluster *> inviableClusters;
  for (Cluster *c : clusters) {
    if (!c->isClusterViable()) {
      vector<Paper *> *papersFromCluster = c->getPapers();
      for (Paper *p : *papersFromCluster) {
        papers.push_back(p);
      }
      c->clearPapers();
    }
  }

  alocatePapersInClustersByBestBenefit(&papers);

  inviableClusters.clear();
  papers.clear();

  for (Cluster *c : clusters) {
    if (!c->isClusterViable()) {
      inviableClusters.push_back(c);
      for (Paper *p : *c->getPapers()) {
        papers.push_back(p);
      }
      c->clearPapers();
    }
  }

  Cluster *dummyCluster =
      Cluster::createDummyClusuter(clusters.size(), &papers);
  clusters.push_back(dummyCluster);
  return inviableClusters.size() == 0;
}

void Heuristic::alocatePapersInClustersByBestBenefit(vector<Paper *> *papers) {
  for (Paper *paper : *papers) {
    vector<ClusterBenefit *> candidates;
    for (Cluster *c : clusters) {
      if (c->isNotFull()) {
        int benefit = c->calculateBenefitOfInsertPaper(paper, true);
        candidates.push_back(new ClusterBenefit(benefit, c));
      }
    }

    if (candidates.size() == 0) {
      throw invalid_argument("Erro, Heuristic.cpp [result.size() == 0]");
    }
    sort(candidates.begin(), candidates.end(), ClusterBenefit::orderDescending);
    candidates.at(0)->cluster->add(paper);
    deleteVectorElements(&candidates);
  }
}

void Heuristic::saveBestSolution(vector<Cluster *> *clusters) {
  deleteVectorElements(&clustersFromBestSolution);
  clustersFromBestSolution.clear();
  for (Cluster *c : *clusters) {
    clustersFromBestSolution.push_back(c->copy());
  }
}

void Heuristic::saveBestIterationResult(vector<Cluster *> *clusters) {
  deleteVectorElements(&clustersFromBestIteratonResult);
  clustersFromBestIteratonResult.clear();
  for (Cluster *c : *clusters) {
    clustersFromBestIteratonResult.push_back(c->copy());
  }
}

int Heuristic::RVND(bool &isSoltionFromRVNDStore) {
  LocalSearch localSearch(&clusters, eventInfo);
  return localSearch.RVND(clusters, isSoltionFromRVNDStore);
}

bool Heuristic::hasInviableClusters() {
  for (Cluster *c : this->clusters) {
    if (!c->isClusterViable())
      return true;
  }
  return false;
}

bool Heuristic::acceptanceProbability(int solution, int bestIterationSolution,
                                      double temp) {

  int delta = bestIterationSolution - solution;
  double x = ((double)rand()) / RAND_MAX;
  double expValue = exp(-delta / temp);
  bool res = temp > 0.00001 && x < expValue;
  // if (delta > 0) {
  //     cout << "-----" << endl;
  //     cout << "solution: " << solution << endl;
  //     cout << "bestIterationSolution: " << bestIterationSolution << endl;
  //     cout << "dela: " << delta << endl;
  //     cout << "x:" << x << endl;
  //     cout << "expValue: " << expValue << endl;
  //     cout << "temp:" << temp << endl;
  //     cout << "result: " << res << endl;
  // }
  return res;
}

void Heuristic::copyClustersFromBestIterationResult() {
  for (auto it = clusters.begin(); it != clusters.end(); it++) {
    delete (*it);
  }
  clusters.clear();

  for (Cluster *c : clustersFromBestIteratonResult) {
    clusters.push_back(c->copy());
  }
}

vector<Cluster *> *Heuristic::getClustersNotEmpty(vector<Cluster *> *clusters) {
  vector<Cluster *> *result = new vector<Cluster *>();
  for (Cluster *c : *clusters) {
    if (!c->isEmpty())
      result->push_back(c);
  }
  return result;
}

// vector<TimeSlot*>* Heuristic::runSetPartition() {
//     int value = Cluster::sumClustersValues(&clustersFromBestSolution);
//     cout << clustersFromBestSolution.size() << endl;
//     cout << "cluster value: " << value;
//     SetPartition sp(clusterPool.getAllClusters(), &clustersFromBestSolution,
//     eventInfo); result = sp.solve(); if (result->isSolutionFound()) {
//         return sp.getTimeSlots();
//     } else {
//         return NULL;
//     }
// }

Heuristic *Heuristic::getInstance() { return instance; }
