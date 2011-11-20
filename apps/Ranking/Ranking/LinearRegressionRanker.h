#ifndef LINEAR_REGRESSION_RANKER
#define LIENAR_REGRESSION_RANKER

#include "Ranker.h"

#include "tnt.h"

class LinearRegressionRanker : public Ranker
{
public:
    using Ranker::rank;

    LinearRegressionRanker(const Metric &metric) : Ranker(metric) {}

    virtual void learn(const QueryData &data);
    virtual RankingList rank(const QueryData::Query &data) const;

private:
    TNT::Array2D<double> params;

    void learn(const TNT::Array2D<double> &X, const TNT::Array2D<double> &y);
    RankingList rank(const TNT::Array2D<double> &X, const QueryData::Query &queryDocs) const;

    TNT::Array2D<double> predict(const TNT::Array2D<double> &X) const;
};

#endif
