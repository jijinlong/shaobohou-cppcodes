#ifndef QUERY_DATA_H
#define QUERY_DATA_H

// Query related classes
//
// Shaobo Hou
// 20.11.2011

#include "QueryVector.h"

#include <map>

// a set of queries
class QueryData
{
public:
    // documents associated with a single query
    typedef std::vector<QueryVector*> Query;

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

    // query accessor
    const Query& getQuery(const int q) const;

    // group all documents into a single query
    Query getAllQuery() const;


private:
    int m_ndata, m_nfeature;
    std::map<std::string, int> m_qid2index;
    std::vector<Query> m_data;
};

#endif
