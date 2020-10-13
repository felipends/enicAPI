#include "MovementInfo.h"
#include <algorithm>
#include <sstream>
#include "utils/BenefitsUtil.h"

MovementInfo::MovementInfo(Paper *paper, Cluster *cluster, int removeValue)
{
    this->paper = paper;
    this->cluster = cluster;
    this->removeValue = removeValue;
}

int MovementInfo::getCurrentObjectiveValue()
{
    int sum = 0;
    for (auto cluster : *ClustersInfo::clusters)
        sum += cluster->getClusterValue();
    return sum;
}

void MovementInfo::calculateInsertMovements()
{
    ClustersInfo::deleteInsertMovement(paper->index, cluster->index);
    // ClustersInfo::insertMovements[paper->index][cluster->index] = NULL;
    vector<Cluster *> *clustersInfo = ClustersInfo::clusters;
    for (auto c : *clustersInfo)
    {
        if (c->index != cluster->index)
        {
            int benefit = c->calculateBenefitOfInsertPaper(paper, false);
            // ClustersInfo::insertMovements[paper->index][c->index] = benefit;
            ClustersInfo::setInsertMovement(paper->index, c->index, benefit);
        }
    }
    calculateBestPaperMovement();
}

void MovementInfo::recalculateInsertMovementsToClusters(vector<Cluster *> *modifiedClusters, vector<MovementInfo *> *movementsInfo)
{
    // atualiza o valores de inserção dos papers para os clusters envolvidos no move
    for (auto c : *modifiedClusters)
    {
        vector<int> paperIndexes;
        auto papers = c->getPapers();
        for (auto p : *papers)
            paperIndexes.push_back(p->index);

        for (auto m : *movementsInfo)
        {
            if (find(paperIndexes.begin(), paperIndexes.end(), m->paper->index) == paperIndexes.end())
            {
                // ClustersInfo::insertMovements[m->paper->index][c->index] =
                //     c->calculateBenefitOfInsertPaper(m->paper, false);
                int benefit = c->calculateBenefitOfInsertPaper(m->paper, false);
                ClustersInfo::setInsertMovement(m->paper->index, c->index, benefit);
            }
        }
    }
}

void MovementInfo::calculateBestPaperMovement()
{
    /* Procura nos cluters que ainda não estão cheios o melhor benefício
    * de remover do cluster atual e inserir em algum outro cluster. */    
    bestClusterIndexToMove = -1;
    if (cluster->canRemoveFromCluster()) {
        for (auto c : *ClustersInfo::clusters)
        {
            // Considera o primeiro cluster não cheio como o melhor
            if (bestClusterIndexToMove == -1)
            {
                if (cluster->index != c->index && c->isNotFull())
                {
                    bestClusterIndexToMove = c->index;                    
                }
                continue;
            }            
            /* Se o cluster não estiver cheio e apresentar um benefício maior, é escolhido
            O removeValue tem que está na comparação pois pode ocorrer de um movimento
            ter um aumento de 10 mas ao remover se perde 20 */
            if (cluster->index != c->index && c->isNotFull() 
                && ClustersInfo::getInsertMovement(paper->index, bestClusterIndexToMove) - removeValue < 
                    ClustersInfo::getInsertMovement(paper->index, c->index) - removeValue)
            {

                // cout << "bestClusterIndexToMove2 : " << bestClusterIndexToMove << endl;
                bestClusterIndexToMove = c->index;
            }            
        }
    }
        
    bestMovementValue = bestClusterIndexToMove >= 0
        ? ClustersInfo::getInsertMovement(paper->index, bestClusterIndexToMove) - removeValue
        : 0;
        // if (bestClusterIndexToMove >= 0)
        //     cout << ClustersInfo::getInsertMovement(paper->index, bestClusterIndexToMove) << " - " << removeValue << " = " << bestMovementValue << endl;
        // cout << cluster->index << ". bestMovementValue: " << bestMovementValue << endl;
    }

    void MovementInfo::makeMoveMovement(vector<MovementInfo *> * movementsInfo)
    {
        makeMoveMovement(movementsInfo, -1);
    }

    void MovementInfo::makeMoveMovement(vector<MovementInfo *> * movementsInfo, int clusterIndex)
    {
        // Troca os cluster corrente pelo cluster de destino
        auto oldClusterIndex = cluster->index;
        int resIndex = clusterIndex >= 0 ? clusterIndex : bestClusterIndexToMove;
        movePaperToCluster(resIndex);
        updateSwapMovementsInsideCluster();

        // Atualiza valores de remover o cluster (o index), o RemoveValue deste movementInfo
        // já foi calculado em MovePaperToCluster
        // vector<MovementInfo*> movementsWithClustersInTheMove;
        /* OBSERVAÇÃO: talvez poderia ser acessado em O(1) utilzando uma estrutura de dados diferente,
       um array 2D por exemplo */
        for (auto m : *movementsInfo)
        {
            if ((m->cluster->index == oldClusterIndex || m->cluster->index == cluster->index) && m->paper->index != paper->index)
            {
                m->updateRemoveValue();
            }
        }

        // Atualiza o movimento de inserção dos outros clusters para os clusteres
        // que foram atualizados (o que sofreu remoção e o que sofreu um inserção)
        vector<Cluster *> modifiedClusters;
        modifiedClusters.push_back(cluster);
        modifiedClusters.push_back(ClustersInfo::clusters->at(oldClusterIndex));
        recalculateInsertMovementsToClusters(&modifiedClusters, movementsInfo);

        // Atualiza a vizinhança de swap
        for (auto m : *movementsInfo)
        {
            if (m->cluster->index == oldClusterIndex || m->cluster->index == cluster->index)
            {
                m->recalculateSwapMovents(movementsInfo);
            }
        }
    }

    /* Move um paper para um novo cluster */
    void MovementInfo::movePaperToCluster(int clusterIndex)
    {
        if (cluster->isDummyCluster())
        {
            removeValue = 0;
        }
        // O benefício desse paper voltar para este cluster é igual ao de remoção
        ClustersInfo::setInsertMovement(paper->index, cluster->index, removeValue);

        cluster->removePaper(paper);
        ClustersInfo::clusters->at(clusterIndex)->add(paper);
        // Faz o cluster de destino ser o novo cluster desta classe
        cluster = ClustersInfo::clusters->at(clusterIndex);

        // O valor do paper sair do novo cluster é igual ao dele entrar
        removeValue = ClustersInfo::getInsertMovement(paper->index, cluster->index);

        // Remove o benefíco já que o paper já se encontra no cluster corrente
        ClustersInfo::deleteInsertMovement(paper->index, cluster->index);

        //// Recalcula o melhor cluster para mover o paper (necessário na vizinhança mover)
        //CalculateBestPaperMovement(); Obs: por enquanto vou calcular todos juntos
    }

    // Atualiza o Movimento de Swap para não conter benefício entre os papers do mesmo cluster
    void MovementInfo::updateSwapMovementsInsideCluster()
    {
        auto clusterPapers = cluster->getPapers();
        for (auto p : *clusterPapers)
        {
            if (p->index == paper->index)
                continue;

            if (paper->index < p->index)
                ClustersInfo::swapMovements[paper->index][p->index] = NULL;
            else
                ClustersInfo::swapMovements[p->index][paper->index] = NULL;
        }
    }

    void MovementInfo::updateRemoveValue()
    {
        removeValue = cluster->calculatePaperRemoveValue(paper);
    }

    void MovementInfo::calculateSwapMovement(MovementInfo * movement)
    {
        int index1, index2;

        index1 = min(paper->index, movement->paper->index);
        index2 = max(paper->index, movement->paper->index);

        vector<int> clusterPaperIndex;
        auto clusterPapers = cluster->getPapers();
        for (auto p : *clusterPapers)
            clusterPaperIndex.push_back(p->index);

        // if (Cluster.Papers.Contains(movement.Paper))
        if (find(clusterPaperIndex.begin(), clusterPaperIndex.end(), movement->paper->index) != clusterPaperIndex.end())
        {
            ClustersInfo::swapMovements[index1][index2] = NULL;
            return;
        }

        auto paper1 = paper;
        auto paper2 = movement->paper;
        auto cluster1 = cluster;
        auto cluster2 = movement->cluster;

        int benefitOfPaper1inCluster1 = removeValue;
        int benefitOfPaper2inCluster2 = movement->removeValue;

        int benefitOfPaper1inCluter2 = ClustersInfo::getInsertMovement(paper1->index, cluster2->index);
        int benefitOfPaper2inCluster1 = ClustersInfo::getInsertMovement(paper2->index, cluster1->index);

        /* Esta operação é realizada pois o benefício é calculado considerando que o cluster de destino possui todos os papers.
       Por exemplo, ao calcular o benefício do paper 1 ir para o cluster 2, no cálculo leva-se em consideração o valor
       do paper 2 já que esse entontra-se dentro do cluster 2. E portando esse valor deve ser subtraído
       ao se calcular o benefício de trocar o paper de cluster.
    */
        int benefitOfPaper1AndPaper2 = BenefitsUtil::getBenefit(paper1->index, paper2->index);

        int swapBenefitP1ToC2 = benefitOfPaper1inCluter2 - benefitOfPaper1inCluster1 - benefitOfPaper1AndPaper2;
        int swapBenefitP2ToC1 = benefitOfPaper2inCluster1 - benefitOfPaper2inCluster2 - benefitOfPaper1AndPaper2;

        int swapBenefit = swapBenefitP1ToC2 + swapBenefitP2ToC1;

        ClustersInfo::setSwapMovement(index1, index2, new SwapMovementInfo(this, movement, swapBenefit));
    }

    /* Calcula o melhor movimento de swap entre esta classe e os MovementInfo passados como parâmetro */
    void MovementInfo::calculateSwapMovements(vector<MovementInfo *> * movements)
    {
        vector<MovementInfo *> movementsToCompare;
        for (MovementInfo *m : *movements)
        {
            if (m->cluster->index > cluster->index)
            {
                movementsToCompare.push_back(m);
            }
        }

        int index1, index2;
        for (MovementInfo *m : movementsToCompare)
        {

            index1 = min(paper->index, m->paper->index);
            index2 = max(paper->index, m->paper->index);

            Paper *paper1 = paper;
            Paper *paper2 = m->paper;
            Cluster *cluster1 = cluster;
            Cluster *cluster2 = m->cluster;

            int benefitOfPaper1inCluster1 = removeValue;
            int benefitOfPaper2inCluster2 = m->getRemoveValue();

            int benefitOfPaper1inCluster2 = ClustersInfo::getInsertMovement(paper1->index, cluster2->index);
            int benefitOfPaper2inCluster1 = ClustersInfo::getInsertMovement(paper2->index, cluster1->index);

            int benefitOfPaper1AndPaper2 = BenefitsUtil::getBenefit(paper1->index, paper2->index);

            int swapBenefitP1ToC2 = benefitOfPaper1inCluster2 - benefitOfPaper1inCluster1 - benefitOfPaper1AndPaper2;
            int swapbenefitP2ToC1 = benefitOfPaper2inCluster1 - benefitOfPaper2inCluster2 - benefitOfPaper1AndPaper2;

            int swapBenefit = swapBenefitP1ToC2 + swapbenefitP2ToC1;
            ClustersInfo::setSwapMovement(index1, index2, new SwapMovementInfo(this, m, swapBenefit));
        }
    }

    /* Calcula o melhor movimento de swap entre esta classe e os MovementInfo passados como parâmetro */
    void MovementInfo::recalculateSwapMovents(vector<MovementInfo *> * movements)
    {
        int index1, index2;
        for (auto m : *movements)
        {
            if (m->cluster->index == cluster->index)
                continue;

            index1 = min(paper->index, m->paper->index);
            index2 = max(paper->index, m->paper->index);

            auto paper1 = paper;
            auto paper2 = m->paper;
            auto cluster1 = cluster;
            auto cluster2 = m->cluster;

            int benefitOfPaper1inCluster1 = removeValue;
            int benefitOfPaper2inCluster2 = m->removeValue;

            int benefitOfPaper1inCluter2 = ClustersInfo::getInsertMovement(paper1->index, cluster2->index);
            int benefitOfPaper2inCluster1 = ClustersInfo::getInsertMovement(paper2->index, cluster1->index);

            int benefitOfPaper1AndPaper2 = BenefitsUtil::getBenefit(paper1->index, paper2->index); //paper1.CalculateBenefit(paper2);

            int swapBenefitP1ToC2 = benefitOfPaper1inCluter2 - benefitOfPaper1inCluster1 - benefitOfPaper1AndPaper2;
            int swapBenefitP2ToC1 = benefitOfPaper2inCluster1 - benefitOfPaper2inCluster2 - benefitOfPaper1AndPaper2;

            int swapBenefit = swapBenefitP1ToC2 + swapBenefitP2ToC1;

            ClustersInfo::setSwapMovement(index1, index2, new SwapMovementInfo(this, m, swapBenefit));
        }
    }

    // Procura em toda a tabela pelo melhor movimento de troca
    SwapMovementInfo *MovementInfo::findBestSwapMovement()
    {
        SwapMovementInfo *bestSwapMovement = NULL;
        for (int i = 0; i < ClustersInfo::swapMovementsSize; i++)
        {
            for (int j = i + 1; j < ClustersInfo::swapMovementsSize; j++)
            {
                SwapMovementInfo *swapMove = ClustersInfo::swapMovements[i][j];
                if (swapMove == NULL)
                {
                    continue;
                }

                // Escolhe um primeiro swap que encontrar como o melhor
                if (bestSwapMovement == NULL)
                {
                    bestSwapMovement = swapMove;
                    continue;
                }

                // Se encontrar um movimento de troca melhor atualiza o melhor movimento
                if (swapMove->getValue() > bestSwapMovement->getValue())
                {
                    bestSwapMovement = swapMove;
                }
            }
        }

        return bestSwapMovement;
    }

    void MovementInfo::makeSwap(SwapMovementInfo * swapMovement, vector<MovementInfo *> * movementsInfo)
    {
        vector<MovementInfo *> &movements = swapMovement->getMovements();
        movements[0]->clusterToSwap = movements[1]->cluster->index;
        movements[1]->clusterToSwap = movements[0]->cluster->index;

        // Move os papers para seus novos clusters
        vector<Paper *> papers;
        for (auto m : *movementsInfo)
        {
            vector<Paper *> *papersToInsert = m->cluster->getPapers();
            for (auto p : *papersToInsert)
            {
                papers.push_back(p);
            }
        }
        makeSwapMovement(&swapMovement->getMovements(), &papers);

        // Autualiza os valores de remoção de todos os papers nos dois clusters envolvidos no swap
        vector<MovementInfo *> aux = swapMovement->getMovements();
        for (auto movement : aux)
        {
            movement->updateSwapMovementsInsideCluster();
            for (auto m : *movementsInfo)
            {
                if (m->cluster->index == movement->cluster->index)
                {
                    m->updateRemoveValue();
                }
            }
        }
        // Recalcula os movimentos de swap entre os dois movimentos envolvidos e os demais clusters
        for (auto movement : aux)
        {
            for (auto m : *movementsInfo)
            {
                if (m->cluster->index == movement->cluster->index)
                {
                    m->recalculateSwapMovents(movementsInfo);
                }
            }
        }
    }

    void MovementInfo::makeSwapMovement(vector<MovementInfo *> * movements, vector<Paper *> * papers)
    {
        vector<MovementInfo *> &move = *movements;
        auto paper1 = move[0]->paper;
        auto cluster1 = move[0]->cluster;

        auto paper2 = move[1]->paper;
        auto cluster2 = move[1]->cluster;

        cluster1->removePaper(paper1);
        cluster2->removePaper(paper2);

        cluster1->add(paper2);
        cluster2->add(paper1);

        ClustersInfo::deleteInsertMovement(paper1->index, cluster2->index);
        ClustersInfo::deleteInsertMovement(paper2->index, cluster1->index);

        move[0]->cluster = cluster2;
        move[1]->cluster = cluster1;

        // custo dos papers voltarem ao cluster original
        vector<int> papersC1;
        auto clusterPapers = cluster1->getPapers();
        for (auto p : *clusterPapers)
            papersC1.push_back(p->index);

        vector<int> papersC2;
        auto clusterPapers2 = cluster2->getPapers();
        for (auto p : *clusterPapers2)
            papersC2.push_back(p->index);

        for (auto paper : *papers)
        {
            if (find(papersC1.begin(), papersC1.end(), paper->index) != papersC1.end())
            {
                ClustersInfo::deleteInsertMovement(paper->index, cluster1->index);
            }
            else
            {
                int benefit = cluster1->calculateBenefitOfInsertPaper(paper, false);
                ClustersInfo::setInsertMovement(paper->index, cluster1->index, benefit);
            }

            if (find(papersC2.begin(), papersC2.end(), paper->index) != papersC2.end())
            {
                ClustersInfo::deleteInsertMovement(paper->index, cluster2->index);
            }
            else
            {
                int benefit = cluster2->calculateBenefitOfInsertPaper(paper, false);
                ClustersInfo::setInsertMovement(paper->index, cluster2->index, benefit);
            }
        }
        move[0]->updateRemoveValue();
        move[1]->updateRemoveValue();
    }

    // getters and setters
    Paper *MovementInfo::getPaper()
    {
        return paper;
    }

    void MovementInfo::setPaper(Paper * paper)
    {
        this->paper = paper;
    }

    Cluster *MovementInfo::getCluster()
    {
        return cluster;
    }

    void MovementInfo::setCluster(Cluster * cluster)
    {
        this->cluster = cluster;
    }

    int MovementInfo::getRemoveValue()
    {
        return removeValue;
    }

    void MovementInfo::setRemoveValue(int removeValue)
    {
        this->removeValue = removeValue;
    }

    int MovementInfo::getBestMovementValue()
    {
        return bestMovementValue;
    }

    int MovementInfo::getBestSwapMovement()
    {
        return bestSwapMovement;
    }

    void MovementInfo::setBestSwapMovement(int bestSwapMovement)
    {
        this->bestClusterIndexToMove = bestSwapMovement;
    }

    int MovementInfo::getBestClusterIndexToMove() {
        return bestClusterIndexToMove;
    }

    int MovementInfo::getClusterToSwap()
    {
        return clusterToSwap;
    }

    void MovementInfo::setClusterToSwap(int clusterToSwap)
    {
        this->clusterToSwap = clusterToSwap;
    }

    string MovementInfo::toString()
    {
        return "p" + to_string(paper->index) + " - rm " + to_string(removeValue);
    }

    string MovementInfo::removeValueToString(vector<MovementInfo *> * movements)
    {
        stringstream text;
        vector<int> rm(movements->size());
        for (MovementInfo *m : *movements)
        {
            rm[m->getPaper()->index] = m->getRemoveValue();
        }
        for (int i = 0; i < rm.size(); i++)
        {
            text << "(" << i << ", " << rm[i] << ")"
                 << " ";
        }
        text << "\n";
        return text.str();
    }

    string MovementInfo::swapTableToString()
    {
        stringstream text;
        auto swapMovements = ClustersInfo::swapMovements;
        for (int i = 0; i < swapMovements.size(); i++)
        {
            for (int j = 0; j < swapMovements[i].size(); j++)
            {
                if (i > j)
                {
                    text << "x ";
                    continue;
                }
                if (swapMovements[i][j] != NULL)
                    text << swapMovements[i][j]->getValue();
                else
                    text << "n";
                text << " ";
            }
            text << endl;
        }
        return text.str();
    }

    string MovementInfo::insertTableToString()
    {
        // stringstream text;
        // auto insertMovements = ClustersInfo::insertMovements;
        // for (int i = 0; i < insertMovements.size(); i++) {
        //     for (int j = 0; j < insertMovements[i].size(); j++) {
        //         text << insertMovements[i][j];
        //         text << " ";
        //     }
        //     text << endl;
        // }
        // return text.str();
        return "";
    }