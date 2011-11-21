#ifndef TREE_REGRESSION_RANKER_H
#define TREE_REGRESSION_RANKER_H

#include "LinearRegressionRanker.h"

class TreeRegressionRanker : public Ranker
{
public:
    using Ranker::rank;

    class Node
    {
    public:
        Node() : leftChild(NULL), rightChild(NULL), ranker(NULL) {};
        Node(const QueryData::Query &data, const Metric *metric, const int level);

        RankingList rank(const QueryData::Query &data) const;

    private:
        int bestFeature;
        double bestValue;

        Node *leftChild;
        Node *rightChild;
        LinearRegressionRanker *ranker;

        double rank(const QueryVector &data) const;
    };

    TreeRegressionRanker(const Metric &metric) : Ranker(metric) {}

    virtual void learn(const QueryData &data);
    virtual RankingList rank(const QueryData::Query &data) const;

private:
    Node *root;
};

#endif
