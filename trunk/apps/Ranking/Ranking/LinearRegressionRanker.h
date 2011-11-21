#ifndef LINEAR_REGRESSION_RANKER_H
#define LINEAR_REGRESSION_RANKER_H

// A Linear Regression Ranking algorithm
//
// Shaobo Hou 
// 20.11.2011

#include "Ranker.h"

#include "tnt.h"

class LinearRegressionRanker : public Ranker
{
public:
    using Ranker::rank;

    LinearRegressionRanker(const Metric &metric) : Ranker(metric) {}

    // learn from a set of queries
    virtual void learn(const QueryData &data);

    // rank documents associated with a single query
    virtual RankingList rank(const QueryData::Query &data) const;

    // learn from a single query
    void learn(const QueryData::Query &data);

    // rank a query-document feature point
    double rank(const QueryVector &data) const;

private:
    TNT::Array2D<double> params;

    void learn(const TNT::Array2D<double> &X, const TNT::Array2D<double> &y);
    RankingList rank(const TNT::Array2D<double> &X, const QueryData::Query &queryDocs) const;

    TNT::Array2D<double> predict(const TNT::Array2D<double> &X) const;
};

#endif
