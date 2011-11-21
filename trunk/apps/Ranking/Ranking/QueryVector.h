#ifndef QUERYVECTOR_H
#define QUERYVECTOR_H

// Feature vector class for a query-document pair
//
// Shaobo Hou
// 20.11.2011

#include <vector>
#include <string>

class QueryVector
{
public:
    QueryVector(const std::string &line)
    {
        m_features.reserve(64);
        parse(line);
    }

    double relevance() const
    {
        return m_relevance;
    };

    const std::string& queryID() const
    {
        return m_queryname;
    }

    const std::string& docID() const
    {
        return m_docname;
    }

    int nfeature() const
    {
        return m_features.size();
    }

    double features(const int i) const
    {
        return m_features[i];
    }

private:
    double m_relevance;
    std::string m_queryname;
    std::vector<double> m_features;
    std::string m_docname;

    void parse(const std::string &line);
};

#endif
