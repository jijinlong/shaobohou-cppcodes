#include "EdmondsAlgorithm.h"

#include <fstream>
#include <iostream>
#include <time.h>


namespace EdmondsAlgorithm
{

/** 
 * \brief find cycles from the parent list of vertices
 *
 * \param parents the ith element denotes the parent of the ith vertex
 *
 * \return a list of cycles in terms of vertex indices.
 */
std::vector<std::vector<int> > FindCycles(const std::vector<int> &parents)
{
	const int nverts = parents.size();
	std::vector<int> labels(nverts, -1);

	std::vector<std::vector<int> > paths(nverts);
	for(int i = 0; i < nverts; i++)
	{
		if(labels[i] >= 0)
			continue;

		std::vector<bool> visited(nverts, false);
		visited[i] = true;

		// trace the path of vertex i through its parent
		int v = i;
		while(true)
		{
			// add parent to path
			paths[i].push_back(parents[v]);

			// true if vertex should be ignored or is its own parent, both root conditions
			if(parents[v] < 0 || parents[v] == v)		
				break;

			v = parents[v];

			// eventually true if vertex i is in a cycle
			if(visited[v])
				break;

			// true if reached a previous determined cycle
			if(labels[v] >= 0)
			{
				paths[i].clear();
				break;
			}

			visited[v] = true;
		}

		// test if vertex i is in a cycle
		if(paths[i].size() > 1 && paths[i].back() == i)		// vertex i is in a cycle
		{
			for(unsigned int j = 0; j < paths[i].size(); j++)
				labels[paths[i][j]] = i;
		}
		else
			paths[i].clear();

	}

	// remove non-cycle paths
	std::vector<std::vector<int> > cycles;
	for(int i = 0; i < nverts; i++)
		if(paths[i].size() > 0)
			cycles.push_back(paths[i]);

	return cycles;
}

}


#ifndef _WINDOWS
#include <sys/time.h>
long getTime()
{
    struct timeval currtime;
    gettimeofday(&currtime, NULL);

    long mtime = currtime.tv_sec*1000000 + currtime.tv_usec;

    return mtime;
}
#else
#include <Windows.h>
long getTime()
{
    return GetTickCount();
}
#endif


namespace EdmondsAlgorithm
{

bool EdmondsTest()
{
	//srand(200);
	srand(time(NULL));

    const int BIG_VAL = 1000000;

	// toy example
	const int toy_nverts = 6;
	std::vector<Edge<int> > toy_edges;
	toy_edges.push_back(Edge<int>(0, 1, 1));
	toy_edges.push_back(Edge<int>(0, 2, 10));
	toy_edges.push_back(Edge<int>(0, 5, 5));
	toy_edges.push_back(Edge<int>(1, 2, 8));
	toy_edges.push_back(Edge<int>(1, 5, 2));
	toy_edges.push_back(Edge<int>(2, 4, 6));
	toy_edges.push_back(Edge<int>(3, 1, 7));
	toy_edges.push_back(Edge<int>(3, 2, 4));
	toy_edges.push_back(Edge<int>(4, 3, 3));
	toy_edges.push_back(Edge<int>(5, 3, 9));
	toy_edges.push_back(Edge<int>(5, 4, 11));

	std::vector<int> inedges1a = EdmondsAlgorithmSparse(toy_nverts, toy_edges, std::vector<int>(1, 0));
	std::vector<int> inedges1b = ExhaustiveSearch(toy_nverts, toy_edges, std::vector<int>(1, 0));
    std::vector<int> inedges1c = EdmondsAlgorithmDense(toy_nverts, toy_edges, std::vector<int>(1, 0), BIG_VAL);
	bool toy_success = inedges1a== inedges1b;
	assert(toy_success);
    bool toy_success2 = inedges1a== inedges1c;
    assert(toy_success2);


    // toy example 2
    std::vector<Edge<int> > toy_edges2;
    toy_edges2.push_back(Edge<int>(0, 1, 100));
    toy_edges2.push_back(Edge<int>(1, 2, 300));
    toy_edges2.push_back(Edge<int>(2, 3, 200));
    toy_edges2.push_back(Edge<int>(3, 0, 150));
    std::vector<int> in_toy_edges2a = EdmondsAlgorithmSparse(4, toy_edges2, std::vector<int>());
    std::vector<int> in_toy_edges2c = EdmondsAlgorithmDense( 4, toy_edges2, std::vector<int>(), BIG_VAL);


    // timing info
    long timeSparse = 0;
    long timeExhaust = 0;
    long timeDense = 0;

	const int ntests = 100;
	std::vector<bool> successes(ntests, false);
    std::vector<bool> successes2(ntests, false);
	std::vector<int> valids(ntests, 0);
	for(int t = 0; t < ntests; t++)
	{
		//while(true)
		//{
			const int nverts = 6;
			std::vector<Edge<int> > edges;
			for(int i = 0; i < nverts; i++)
				for(int j = 0; j < nverts; j++)
				{
					const int tempval = rand()%BIG_VAL;
					if(i != j)
						if(rand() > RAND_MAX/2 && edges.size() < nverts*nverts/2)
							edges.push_back(Edge<int>(i, j, tempval));
				}

            std::vector<int> roots(1, 0);
            long time0 = getTime();

			std::vector<int> inedgesa = EdmondsAlgorithmSparse(nverts, edges, roots);
			int scorea = EvaluateTree(edges, inedgesa);
            assert(scorea >= 0);
            long time1 = getTime();

			std::vector<int> inedgesb = ExhaustiveSearch(nverts, edges, roots);
			int scoreb = EvaluateTree(edges, inedgesb);
            assert(scoreb >= 0);
            long time2 = getTime();

            std::vector<int> inedgesc = EdmondsAlgorithmDense(nverts, edges, roots, BIG_VAL*10);
            int scorec = EvaluateTree(edges, inedgesc);
            assert(scorec >= 0);
            long time3 = getTime();

            timeSparse += time1-time0;
            timeExhaust += time2-time1;
            timeDense += time3-time2;

			// test is only valid the exhaustive method found a solution of a single tree
			if(inedgesb.size() > 0)
			{
				valids[t] = 1;
				successes[t] = inedgesa==inedgesb;
                successes2[t] = inedgesa==inedgesc;

#ifdef _WINDOWS
				// debug loop
				while(!successes[t] && scorea != scoreb && inedgesb.size() > 0)
				{
					int bah = 0;
					std::vector<int> parentsc = EdmondsAlgorithmSparse(nverts, edges, std::vector<int>(1, 0));
					int scorec = EvaluateTree(edges, parentsc);
					int pah = 0;
				}
#endif

				//break;
			}
		//}
	}

	bool total_success = inedges1a==inedges1b;
    bool total_success2 = inedges1a==inedges1c;
	for(int i = 0; i < ntests; i++)
		if(valids[i] > 0)
        {
			total_success = total_success && successes[i];
            total_success2 = total_success2 && successes2[i];
        }

	const int nvalids = std::accumulate(valids.begin(), valids.end(), 0);


    // save test results to file
	std::string filename("edmondstest.txt");
	std::ofstream out(filename.c_str());
    out << "total valid tests = " << nvalids << " out of " << ntests << std::endl << std::endl;
    if(total_success)
        out << "sparse edmonds algorithm succeed" << std::endl;
    else
        out << "sparse edmonds algorithm FAILED" << std::endl;
    if(total_success2)
        out << "dense  edmonds algorithm succeed" << std::endl << std::endl;
    else
        out << "dense  edmonds algorithm FAILED" << std::endl << std::endl;
	out << "sparse time     = " << timeSparse << std::endl;
    out << "dense time      = " << timeDense << std::endl;
    out << "exhaustive time = " << timeExhaust << std::endl;
	out.close();
    exit(0);


	return total_success;
}

}
