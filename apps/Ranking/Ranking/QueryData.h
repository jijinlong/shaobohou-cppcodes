#ifndef QUERY_DATA_H
#define QUERY_DATA_H

#include "QueryVector.h"

#include "tnt.h"

#include <map>

class QueryData
{
public:
    typedef std::vector<QueryVector*> QueryGroup;

    QueryData() : m_ndata(0), m_nfeature(0) {}
    QueryData(const std::string &filename);

    ~QueryData();

    void addData(QueryVector *val);

    int nquery() const
    {
        return m_data.size();
    }

    int ndata() const
    {
        return m_ndata;
    }

    int nfeature() const
    {
        return m_nfeature;
    }

    TNT::Array2D<double> label2array() const;
    TNT::Array2D<double> feature2array() const;
    TNT::Array2D<double> feature2array(const int f) const;

private:
    int m_ndata, m_nfeature;
    std::map<std::string, QueryGroup> m_data;
};

#endif
