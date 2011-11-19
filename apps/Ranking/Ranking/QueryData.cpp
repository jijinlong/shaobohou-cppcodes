#include "QueryData.h"

#include <fstream>

QueryData::QueryData(const std::string &filename)
    : m_ndata(0), m_ndims(0)
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

    const int pah = ndata();
    const int bah = 0;
};

QueryData::~QueryData()
{
    for(std::map<std::string, QueryGroup>::iterator git = m_data.begin(); git != m_data.end(); ++git)
    {
        for(QueryGroup::iterator qit = git->second.begin(); qit != git->second.end(); ++qit)
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
        m_ndims = std::max(m_ndims, val->nfeature());

        std::map<std::string, QueryGroup>::iterator it = m_data.find(val->queryID());
        if(it == m_data.end())
        {
            m_data[val->queryID()] = QueryGroup(1, val);
        }
        else
        {
            it->second.push_back(val);
        }
    }
}

TNT::Array2D<double> QueryData::label2array() const
{
    TNT::Array2D<double> labels(m_ndata, 1);

    int count = 0;
    for(std::map<std::string, QueryGroup>::const_iterator git = m_data.begin(); git != m_data.end(); ++git)
    {
        for(QueryGroup::const_iterator qit = git->second.begin(); qit != git->second.end(); ++qit)
        {
            labels[count][0] = (*qit)->relevance();
            count++;
        }
    }

    return labels;
}

TNT::Array2D<double> QueryData::feature2array() const
{
    TNT::Array2D<double> features(m_ndata, m_ndims+1);

    int count = 0;
    for(std::map<std::string, QueryGroup>::const_iterator git = m_data.begin(); git != m_data.end(); ++git)
    {
        for(QueryGroup::const_iterator qit = git->second.begin(); qit != git->second.end(); ++qit)
        {
            features[count][0] = 1;
            for(int j = 0; j < (*qit)->nfeature(); j++)
            {
                features[count][j+1] = (*qit)->features(j);
            }
            count++;
        }
    }

    return features;
}
