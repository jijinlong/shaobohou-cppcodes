#ifndef TREE_REGRESSION_RANKER_H
#define TREE_REGRESSION_RANKER_H

// A Simple Tree Based Regression Ranking algorithm.
//
// At each node, if not a leaf, then partition the data along
// the feature dimension that most evenly splits the associated 
// data points and recursively construct two child nodes.
//
// Shaobo Hou 
// 20.11.2011

#include "LinearRegressionRanker.h"

class TreeRegressionRanker : public Ranker
{
public:
    using Ranker::rank;

    // Node class
    class Node
    {
    public:
        Node() : leftChild(NULL), rightChild(NULL), ranker(NULL) {};
        Node(const QueryData::Query &data, const Metric *metric, const int level);
        ~Node();

        // rank documents associated with a single query
        RankingList rank(const QueryData::Query &data) const;

    private:
        int bestFeature;
        double bestValue;

        Node *leftChild;
        Node *rightChild;
        LinearRegressionRanker *ranker;

        // rank a query-document feature point
        double rank(const QueryVector &data) const;
    };

    TreeRegressionRanker(const Metric &metric, int maxLevel=2) : Ranker(metric), m_maxLevel(maxLevel), root(NULL) {}
    virtual ~TreeRegressionRanker();

    // learn from a set of queries
    virtual void learn(const QueryData &data);

    // rank documents associated with a single query
    virtual RankingList rank(const QueryData::Query &data) const;

private:
    int m_maxLevel;
    Node *root;
};

#endif
