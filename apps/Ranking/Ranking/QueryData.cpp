#include "QueryData.h"

#include <fstream>
#include <cassert>

QueryData::QueryData(const std::string &filename)
    : m_ndata(0), m_nfeature(0)
{
    std::ifstream fin(filename.c_str());
    assert(fin.is_open());

    while(!fin.eof())
    {
        std::string line;
        getline(fin, line);

        if(line.size() > 0)
        {
            addData(new QueryVector(line));
        }
    }
};

QueryData::~QueryData()
{
    for(std::vector<Query>::iterator git = m_data.begin(); git != m_data.end(); ++git)
    {
        for(Query::iterator qit = git->begin(); qit != git->end(); ++qit)
        {
            delete *qit;
        }
    }
}

void QueryData::addData(QueryVector *val)
{
    if(val != NULL)
    {
        ++m_ndata;
        m_nfeature = std::max(m_nfeature, val->nfeature());

        std::map<std::string, int>::iterator it = m_qid2index.find(val->queryID());
        if(it == m_qid2index.end())
        {
            m_qid2index[val->queryID()] = m_data.size();
            m_data.push_back(Query(1, val));
        }
        else
        {
            m_data[it->second].push_back(val);
        }
    }
}

const QueryData::Query& QueryData::getQuery(const int q) const
{
    return m_data[q];
}

QueryData::Query QueryData::getQueryAll() const
{
    Query queryDocs;
    queryDocs.reserve(m_ndata);

    for(std::vector<Query>::const_iterator git = m_data.begin(); git != m_data.end(); ++git)
    {
        queryDocs.insert(queryDocs.end(), git->begin(), git->end());
    }

    return queryDocs;
}
