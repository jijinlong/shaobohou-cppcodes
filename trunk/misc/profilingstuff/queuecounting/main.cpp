//#include "Q2Geometry.h"
//#include "PathGeneratorExtensions.h"
//#include "GenericArrayList.h"

#include <iostream>
#include <execinfo.h>
#include <signal.h>
#include <cstdlib>
#include <cstdio>
#include <cassert>

template <typename T, unsigned short MAX_OBJECTS>
class GenericArrayList
{
public:
    GenericArrayList()
    {
        init();
    }

private:
    // storage
    T m_objects[MAX_OBJECTS];
    T* m_pointers[MAX_OBJECTS+1];
    unsigned short m_prevs[MAX_OBJECTS+1];
    //unsigned short m_nexts[MAX_OBJECTS+1];

    // extra bookkeeping
    unsigned short currOccupied[MAX_OBJECTS+1];

    void init()
    {
        m_prevs[0] = MAX_OBJECTS;               // link to the end node
    }
};

//------------------------------------------------

struct Point
{
  short x, y;
};

//------------------------------------------------

template <unsigned short MAX_POINTS_VALUE>
struct PointArray
{
  Point points[MAX_POINTS_VALUE];
};

typedef PointArray<100> Path;

//------------------------------------------------

struct TimePath
{
  Path path;
  unsigned long lastUpdated;
};

typedef GenericArrayList<TimePath, 500> TimePathList;

//------------------------------------------------

struct MasterPath
{
  TimePath master;
  TimePathList subPaths;  
};

typedef GenericArrayList<MasterPath, 50> MasterPathList;

//------------------------------------------------

#include <vector>
class PathGenerator
{
private:
  MasterPathList m_MasterPaths;
};

void handler(int sig)
{
    void *array[10];
    size_t size;

    // get void*'s for all entries on the stack
    size = backtrace(array, 10);

    // print out all the frames to stderr
    fprintf(stderr, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(array, size, 2);
    exit(1);
}

int main(int argc, char *argv[])
{
    signal(SIGSEGV, handler);

    assert(argc > 1);

    std::cout << "BEFORE" << std::endl;
//    PathGeneratorShaobo pathGenerator;
    PathGenerator bah;
    std::cout << "AFTER" << std::endl;
/*
    std::string filename(argv[1]);

    std::cout << "loading simulation from " << filename << std::endl;
    unsigned long beforeTime = time(NULL);
    pathGenerator.simulate(filename);
    pathGenerator.save();
    std::cout << "simulation took " << time(NULL)-beforeTime << " seconds" << std::endl;
    std::cout << std::endl;

    int count = 0;
    std::cout << pathGenerator.GetMasterPaths()->size() << " master paths" << std::endl;
    for(MasterPathList::iterator it = pathGenerator.GetMasterPaths()->begin(); it != pathGenerator.GetMasterPaths()->end(); it++)
    {
        std::cout << "master path " << count << " contains " << it->subPaths.size() << " sub-paths and is " << it->master.path.numberOfPoints << " points long" << std::endl;
        count++;
    }
*/
    exit(0);
}
