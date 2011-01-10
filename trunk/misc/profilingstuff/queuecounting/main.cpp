#include "PathGeneratorExtensions.h"

#include <iostream>
#include <cstdlib>
#include <cassert>

extern unsigned long getTime();

int main(int argc, char *argv[])
{
    assert(argc > 1);

    PathGeneratorConfig pathConfig;
    PathGeneratorShaobo pathGenerator(&pathConfig);

    std::string filename(argv[1]);

    std::cout << "loading simulation from " << filename << std::endl;
    double beforeTime = getTime()/1000000.0;
    pathGenerator.simulate(filename);
    double afterTime  = getTime()/1000000.0;;
    pathGenerator.save();
    std::cout << "simulation took " << afterTime-beforeTime << " seconds" << std::endl;
    std::cout << std::endl;

    int count = 0;
    std::cout << pathGenerator.GetMasterPaths().size() << " master paths" << std::endl;
    for(MasterPathList::const_iterator it = pathGenerator.GetMasterPaths().begin(); it != pathGenerator.GetMasterPaths().end(); it++)
    {
        std::cout << "master path " << count << " contains " << it->subPaths.size() << " sub-paths and is " << it->master.path.numberOfPoints << " points long" << std::endl;
        count++;
    }

    exit(0);
}
