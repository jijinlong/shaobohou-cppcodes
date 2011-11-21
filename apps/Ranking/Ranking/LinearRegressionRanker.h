#ifndef LINEAR_REGRESSION_RANKER_H
#define LINEAR_REGRESSION_RANKER_H

#include "Ranker.h"

#include "tnt.h"

class LinearRegressionRanker : public Ranker
{
public:
    using Ranker::rank;

    LinearRegressionRanker(const Metric &metric) : Ranker(metric) {}

    virtual void learn(const QueryData &data);
    virtual RankingList rank(const QueryData::Query &data) const;
    virtual double rank(const QueryVector &data) const;

    void learn(const QueryData::Query &data);

private:
    TNT::Array2D<double> params;

    void learn(const TNT::Array2D<double> &X, const TNT::Array2D<double> &y);
    RankingList rank(const TNT::Array2D<double> &X, const QueryData::Query &queryDocs) const;

    TNT::Array2D<double> predict(const TNT::Array2D<double> &X) const;
};

#endif
