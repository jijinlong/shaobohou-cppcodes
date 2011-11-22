#include "TreeRegressionRanker.h"

#include <algorithm>

TreeRegressionRanker::Node::Node(const QueryData::Query &data, const Metric *metric, const int level)
    : leftChild(NULL), rightChild(NULL), ranker(NULL)
{
    if(level == 0)  // build a linear regression ranker at a leaf node
    {
        ranker = new LinearRegressionRanker(*metric);
        ranker->learn(data);
    }
    else
    {
        // find the feature that splits the data most evenly down the middle
        bestFeature = -1;
        bestValue = 0.0;
        double bestScore = 0.0;
        for(int f = 0; f < data[0]->nfeature(); f++)
        {
            double minVal = data[0]->features(f);
            double maxVal = data[0]->features(f);
            for(unsigned int i = 0; i < data.size(); i++)
            {
                minVal = std::min(minVal, data[i]->features(f));
                maxVal = std::max(maxVal, data[i]->features(f));
            }

            double midVal = (maxVal+minVal)*0.5;

            int leftCount = 0;
            int rightCount = 0;
            for(unsigned int i = 0; i < data.size(); i++)
            {
                if(data[i]->features(f) < midVal)
                {
                    ++leftCount;
                }
                else
                {
                    ++rightCount;
                }
            }

            double newScore = leftCount*leftCount + rightCount*rightCount;
            if(bestFeature < 0 || newScore < bestScore)
            {
                bestFeature = f;
                bestValue = midVal;
                bestScore = newScore;
            }
        }

        // partition the data
        QueryData::Query leftQuerys, rightQuerys;
        for(unsigned int i = 0; i < data.size(); i++)
        {
            if(data[i]->features(bestFeature) < bestValue)
            {
                leftQuerys.push_back(data[i]);
            }
            else
            {
                rightQuerys.push_back(data[i]);
            }
        }

        // recursively build child nodes
        leftChild  = new Node(leftQuerys,  metric, level-1);
        rightChild = new Node(rightQuerys, metric, level-1);
    }
}

TreeRegressionRanker::Node::~Node()
{
    if(ranker) delete ranker;
    if(leftChild) delete leftChild;
    if(rightChild) delete rightChild;
}

TreeRegressionRanker::~TreeRegressionRanker()
{
    if(root) delete root;
}

RankingList TreeRegressionRanker::Node::rank(const QueryData::Query &data) const
{
    RankingList rankings;
    for(unsigned int i = 0; i < data.size(); i++)
        rankings.push_back(RankingPair(rank(*data[i]), data[i]));

    std::sort(rankings.rbegin(), rankings.rend());

    return rankings;
}

double TreeRegressionRanker::Node::rank(const QueryVector &data) const
{
    // traverse the decision tree and rank at leaf nodes
    if(ranker != NULL)
    {
        return ranker->rank(data);
    }
    else
    {
        if(data.features(bestFeature) < bestValue)
        {
            return leftChild->rank(data);
        }
        else
        {
            return rightChild->rank(data);
        }
    }
}

void TreeRegressionRanker::learn(const QueryData &data)
{
    QueryData::Query allQuerys = data.getAllQuery();

    root = new Node(allQuerys, m_metric, m_maxLevel);
}

RankingList TreeRegressionRanker::rank(const QueryData::Query &data) const
{
    return root->rank(data);
}
