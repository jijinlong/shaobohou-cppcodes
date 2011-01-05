#include <iostream>
#include <cstdlib>
#include <cassert>

template <typename T, unsigned short MAX_OBJECTS>
class GenericArrayList
{
private:
    // storage
    T m_objects[MAX_OBJECTS];
    T* m_pointers[MAX_OBJECTS+1];
    unsigned short m_prevs[MAX_OBJECTS+1];
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

//------------------------------------------------

int main(int argc, char *argv[])
{
    assert(false);

    PathGenerator bah;
}
