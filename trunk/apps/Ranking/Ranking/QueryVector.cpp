#include "QueryVector.h"

#include <sstream>
#include <cstdlib>
#include <cstdio>

// parse a line of query data
void QueryVector::parse(const std::string &line)
{
    std::stringstream ss(line);

    if(!ss.eof())
    {
        // read in relevance label
        ss >> m_relevance;

        // read everything else
        while(!ss.eof())
        {
            std::string tok;
            ss >> tok;

            if(tok.size() > 0)
            {
                if(tok == "#docid")     // read in document name
                {
                    ss >> tok;
                    ss >> m_docname;
                    break;
                }
                else                    // read in query name and features
                {
                    int pos = tok.find(':');
                    if(pos > 0)
                    {
                        std::string fieldname = tok.substr(0, pos);
                        std::string fieldval = tok.substr(pos+1, tok.size());

                        if(fieldname=="qid")    // read in query name 
                        {
                            m_queryname = fieldval;
                        }
                        else                    // read in feature values
                        {
                            int featureindex = atoi(fieldname.c_str());

//                            if((m_features.size()+1) < feaure_index)
//                                m_features.resize(feature_index-1);

                            m_features.push_back(atof(fieldval.c_str()));
                        }
                    }
                }
            }
        }
    }
}
