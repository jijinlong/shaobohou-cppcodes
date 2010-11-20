#include "MiscUtils.h"

#include <sstream>
#include <algorithm>

using std::vector;
using std::string;
using std::istream;
using std::stringstream;


void getlines(istream &in, vector<string> &lines)
{
    while(!in.eof())
    {
        string line;
        getline(in, line);
        lines.push_back(line);
    }
}

vector<string> tokenise(const string &line)
{
    stringstream ss(line);

    vector<string> tokens;
    while(!ss.eof())
    {
        string tok;
        ss >> tok;

        if(tok.size() > 0)
            tokens.push_back(tok);
    }

    return tokens;
}

vector<string> tokenise(const string &line, const char &delimiter)
{
    stringstream ss(line);

    string tok;
    vector<string> tokens;
    while(getline(ss, tok, delimiter))
        if(tok.size() > 0)
            tokens.push_back(tok);

    return tokens;
}

string uppercase(const string &s)
{
    string result = s;
    transform(s.begin(), s.end(), result.begin(), toupper);
    return result;
}

string lowercase(const string &s)
{
    string result = s;
    transform(s.begin(), s.end(), result.begin(), tolower);
    return result;
}

string trimSpaces(const string &s)
{
    unsigned int t;
    string str = s;
    while ((t = str.find('\t')) != string::npos) str[t] = ' ';
    while ((t = str.find('\n')) != string::npos) str[t] = ' ';
    unsigned int n = str.find_first_not_of(" ");
    unsigned int k = str.find_last_not_of(" ");
    return str.substr(n, k - n + 1);
}

#if defined _WIN64 || defined _WIN32
#include <windows.h>
double getTime()
{
    static double     queryPerfSecsPerCount = 0.0;
    LARGE_INTEGER     queryPerfCount;
    double            seconds;
    BOOL              success;

    if ( queryPerfSecsPerCount == 0.0 )
    {
        LARGE_INTEGER queryPerfCountsPerSec;

        // get ticks-per-second ratio, calc inverse ratio
        success = QueryPerformanceFrequency( &queryPerfCountsPerSec );
        if ( success && queryPerfCountsPerSec.QuadPart )
            queryPerfSecsPerCount = (double) 1.0 / (double) queryPerfCountsPerSec.QuadPart;
        else// failure (oh joy, we are running on Win9x)
            queryPerfSecsPerCount = -1.0;
    }

    if ( queryPerfSecsPerCount == -1.0 )
        seconds = 0.001 * (double) GetTickCount();// GetTickCount() is less accurate, but it is our only choice
    else
    {
        QueryPerformanceCounter( &queryPerfCount );
        seconds = queryPerfSecsPerCount * (double) queryPerfCount.QuadPart;
    }

    return seconds;
}

unsigned int getSeedFromTime()
{
    return GetTickCount();
}

#else
#include <sys/time.h>
double getTime()
{
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);

    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

unsigned int getSeedFromTime()
{
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);

    return tv.tv_usec;
}
#endif

