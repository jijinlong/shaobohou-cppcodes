#include "PathGeneratorExtensions.h"

#include <iostream>
#include <cstdlib>
#include <cassert>

int main(int argc, char *argv[])
{
    assert(argc > 1);

    PathGeneratorShaobo pathGenerator;

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

    exit(0);
}
