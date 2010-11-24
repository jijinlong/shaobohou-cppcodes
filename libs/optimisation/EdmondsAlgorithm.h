#ifndef EDMONDS_ALGORITHM_H
#define EDMONDS_ALGORITHM_H


// Edmond's algorithm for directed MST
//
// REFERENCES
// [1] http://www.ce.rit.edu/~sjyeec/dmst.html
// [2] http://www.scribd.com/doc/6853671/Tarjan-Finding-optimum-branchings
// 
// Shaobo Hou
// 1/11/2010


#include <string>
#include <numeric>

#include "EdmondsAlgorithmDense.h"


namespace EdmondsAlgorithm
{


// Graph Edge type
template <typename T>
struct Edge
{
	int source;
	int target;
	T weight;

	Edge()
	{
		source = -1;
		target = -1;
	}

	Edge(const int s, const int t, const T &w)
	{
		source = s;
		target = t;
		weight = w;
	}
};

// some typedefs
typedef std::vector<std::vector<int> > Cycles;
typedef std::vector<int> Mapping;


/** 
 * \brief find cycles from the parent list of vertices
 *
 * \param parents the ith element denotes the parent of the ith vertex
 *
 * \return a list of cycles in terms of vertex indices.
 */
std::vector<std::vector<int> > FindCycles(const std::vector<int> &parents);


/**
 * \brief contract each cycle in a graph into a pseudo node and compute vertex and edge mappings to the new graph
 *
 * \param vertMapping mapping from vertices in the original graph to vertices in the contracted graph
 * \param edgeMapping mapping from edges in the original graph to edges in the contracted graph
 * \param edges edges of the original graph
 * \param cycles a list of cycles in the original graph in terms of vertex indices
 * \param minInEdges a list of edge indices for the minimum cost incoming edge for each node
 *
 * \return edges of the contracted graph
 */
template <typename T>
std::vector<Edge<T> > ContractGraph(Mapping &vertMapping, Mapping &edgeMapping, const std::vector<Edge<T> > &edges, const std::vector<std::vector<int> > &cycles, const Mapping &minInEdges)
{
	const int nverts = vertMapping.size();
	const int nedges = edgeMapping.size();
	const int ncycles = cycles.size();

	// mark all contracting nodes, first label all cycles
	for(unsigned int i = 0; i < cycles.size(); i++)
		for(unsigned int j = 0; j < cycles[i].size(); j++)
			vertMapping[cycles[i][j]] = i;

	// vertex mapping, then label all remaining nodes
	int vertCount = cycles.size();
	for(int v = 0; v < nverts; v++)
		if(vertMapping[v] < 0)
		{
			vertMapping[v] = vertCount;
			vertCount++;
		}

	// edge mapping
	std::vector<Edge<T> > newEdges;
	for(int e = 0; e < nedges; e++)
	{
		const int old_source = edges[e].source;
		const int old_target = edges[e].target;
		const int new_source = vertMapping[old_source];
		const int new_target = vertMapping[old_target];

		if(new_source != new_target)	// edge is not in a cycle
		{
			edgeMapping[e] = newEdges.size();
			newEdges.push_back(Edge<T>(new_source, new_target, edges[e].weight));
		}
	}

	// compute min in-cycle cost
	std::vector<T> min_incycle_costs(cycles.size(), std::numeric_limits<T>::max());
	for(unsigned int i = 0; i < cycles.size(); i++)
		for(unsigned int j = 0; j < cycles[i].size(); j++)
		{
			const int e = minInEdges[cycles[i][j]];
			const int old_source = edges[e].source;
			const int new_source = vertMapping[old_source];
			if(edges[e].weight < min_incycle_costs[new_source])
				min_incycle_costs[new_source] = edges[e].weight;
		}

	// recompute weights for edges go into a cycle from outside
	for(int e = 0; e < nedges; e++)
	{
		const int old_edge = e;
		const int new_edge = edgeMapping[e];
		const int old_source = edges[e].source;
		const int old_target = edges[e].target;
		const int new_source = vertMapping[old_source];		// new source and new_target cannot be the same due to previous step
		const int new_target = vertMapping[old_target];

		if(new_edge >= 0 && new_target < ncycles && minInEdges[old_target] >= 0)
			newEdges[new_edge].weight = edges[old_edge].weight - (edges[minInEdges[old_target]].weight - min_incycle_costs[new_target]);
	}

	return newEdges;
}

/**
 * \brief reconstruct the MST in the original graph from a contracted graph
 *
 * \param prevGraph edges in the original graph
 * \param prevMinInEdges a list of edge indices for the minimum cost incoming edge for each node in the original graph
 * \param vertMapping mapping from vertices in the original graph to vertices in the contracted graph
 * \param edgeMapping mapping from edges in the original graph to edges in the contracted graph
 * \param currGraph edges in the contract graph
 * \param currMinInEdges a list of edge indices for the minimum cost incoming edge for each node in the contracted graph
 *
 * \return modified list of edge indices for the minimum cost incoming edge for each node in the original graph with cycles removed
 */
template <typename T>
std::vector<int> ReconstructGraph(const std::vector<Edge<T> > &prevGraph, const Mapping &prevMinInEdges, const Mapping &vertMapping, const Mapping &edgeMapping, const std::vector<Edge<T> > &currGraph, const Mapping &currMinInEdges)
{
	const int nprevverts = vertMapping.size();
	const int nprevedges = edgeMapping.size();
	const int ncurrverts = *std::max_element(vertMapping.begin(), vertMapping.end())+1;
	const int ncurredges = *std::max_element(edgeMapping.begin(), edgeMapping.end())+1;


	// compute reverse mapping to determine pseudo nodes
	std::vector<std::vector<int> > reverseVertMapping(ncurrverts);
	for(int v = 0; v < nprevverts; v++)
		reverseVertMapping[vertMapping[v]].push_back(v);
	std::vector<int> reverseEdgeMapping(ncurredges, -1);
	for(int e = 0; e < nprevedges; e++)
		if(edgeMapping[e] >= 0)
			reverseEdgeMapping[edgeMapping[e]] = e;


	// rewire old graph for each pseudo node
	Mapping modifiedPrevMinInEdges(prevMinInEdges.size(), -1);
	for(int v = 0; v < ncurrverts; v++)
	{
		const int curr_edge = currMinInEdges[v];
		if(curr_edge >= 0)
		{
			const int prev_edge = reverseEdgeMapping[curr_edge];
			modifiedPrevMinInEdges[prevGraph[prev_edge].target] = prev_edge;

			// add other nodes in the circuit
			const int cycle_entry = prevGraph[prev_edge].target;
			for(unsigned int i = 0; i < reverseVertMapping[v].size(); i++)
            {
				const int in_edge = prevMinInEdges[reverseVertMapping[v][i]];
				if(prevGraph[in_edge].target != cycle_entry)
					modifiedPrevMinInEdges[prevGraph[in_edge].target] = in_edge;
			}
		}
        else if(reverseVertMapping[v].size() > 1)   // no incoming edge to this contracted node, add all but the most expensive edge in the circuit
        {
            int max_in_edge = prevMinInEdges[reverseVertMapping[v][0]];
            for(unsigned int i = 0; i < reverseVertMapping[v].size(); i++)
            {
				const int in_edge = prevMinInEdges[reverseVertMapping[v][i]];
				if(prevGraph[in_edge].weight > prevGraph[max_in_edge].weight)
                    max_in_edge = in_edge;
			}
            for(unsigned int i = 0; i < reverseVertMapping[v].size(); i++)
            {
				const int in_edge = prevMinInEdges[reverseVertMapping[v][i]];
				if(in_edge != max_in_edge)
                    modifiedPrevMinInEdges[prevGraph[in_edge].target] = in_edge;
            }
        }
	}

	return modifiedPrevMinInEdges;
}

/**
 * \brief find the minimum incoming edge for each node in a graph
 *
 * \param nverts number of vertices in the graph
 * \param edges edges in the graph
 *
 * \return edges indices of minimum cost incoming edges
 */
template <typename T>
std::vector<int> FindMinInEdges(const int nverts, const std::vector<Edge<T> > &edges)
{
	const int nedges = edges.size();

	std::vector<int> min_in_edges(nverts, -1);
	for(int e = 0; e < nedges; e++)
	{
		const int source = edges[e].source;
		const int target = edges[e].target;
		if(min_in_edges[target] < 0 || edges[e].weight < edges[min_in_edges[target]].weight)
			min_in_edges[target] = e;
	}

	return min_in_edges;
}

/**
 * \brief main function for the Edmonds algorithm for the minimum spanning tree (MST) in a SPARSE directed graph
 *
 * \param nvertices number of vertices in the graph
 * \param edges edges in the graph
 * \param roots indices of root nodes
 *
 * \return indices of edges in the MST
 */
template <typename T>
std::vector<int> EdmondsAlgorithmSparse(const int nvertices, const std::vector<Edge<T> > &edges, const std::vector<int> &roots)
{
	// root flags
	std::vector<bool> is_root(nvertices, false);
	for(unsigned int r = 0; r < roots.size(); r++)
    {
        assert(roots[r]>=0 && roots[r]<nvertices);
		is_root[roots[r]] = true;
    }

	// bookkeeping variables for graph contraction
	Mapping initEdgeMapping;
	std::vector<std::vector<Edge<T> > > graphs(1);
	for(unsigned int e = 0; e < edges.size(); e++)
		if(!is_root[edges[e].target])	// only add if target is not a root
		{
			graphs.back().push_back(edges[e]);
			initEdgeMapping.push_back(e);
		}
	std::vector<Mapping> vertMappings(1, std::vector<int>(nvertices, -1));
	std::vector<Mapping> edgeMappings(1, std::vector<int>(graphs.back().size(), -1));
	std::vector<Mapping> minInEdges;
	std::vector<Cycles> cycles;

	// contract cycles into pseudo node and no cycles remain
	while(true)
	{
		// find min in edges for each node
		const int ngraphs = graphs.size();
		minInEdges.push_back(FindMinInEdges(vertMappings.back().size(), graphs.back()));

		// compute parents
		std::vector<int> parents(vertMappings.back().size(), -1);
		for(unsigned int i = 0; i < minInEdges.back().size(); i++)
		{
			const int e = minInEdges.back()[i];
			if(e >= 0)
				parents[graphs.back()[e].target] = graphs.back()[e].source;
		}

		// find cycles on contracted graph
		cycles.push_back(FindCycles(parents));
		if(cycles.back().size() == 0)
			break;
		//else
		//	cycles.back() = Cycles(1, cycles.back()[0]);

		assert(edgeMappings.back().size() == graphs.back().size());
		graphs.push_back(ContractGraph(vertMappings.back(), edgeMappings.back(), graphs.back(), cycles.back(), minInEdges.back()));
		const int nverts = *std::max_element(vertMappings.back().begin(), vertMappings.back().end())+1;
		vertMappings.push_back(std::vector<int>(nverts, -1));
		const int nedges = *std::max_element(edgeMappings.back().begin(), edgeMappings.back().end())+1;
		edgeMappings.push_back(std::vector<int>(nedges, -1));
	}

	// reconstruct branching in reverse and uncontract nodes
	int ngraphs = graphs.size();
	for(int g = ngraphs-1; g > 0; g--)
	{
		const int ncurrverts = *std::max_element(vertMappings[g-1].begin(), vertMappings[g-1].end())+1;
		minInEdges[g-1] = ReconstructGraph(graphs[g-1], minInEdges[g-1], vertMappings[g-1], edgeMappings[g-1], graphs[g], minInEdges[g]);
	}

	// map back to original input edges
	std::vector<int> inEdges(nvertices, -1);
	for(unsigned int i = 0; i < minInEdges.front().size(); i++)
		if(minInEdges.front()[i] >= 0)
			inEdges[i] = initEdgeMapping[minInEdges.front()[i]];

	return inEdges;
}

/**
 * \brief main function for the Edmonds algorithm for the minimum spanning tree (MST) in a DENSE directed graph
 *
 * \param nvertices number of vertices in the graph
 * \param edges edges in the graph
 * \param roots indices of root nodes
 * \param big_val a big value for type T used to fill non-edge entries in the cost matrix
 *
 * \return indices of edges in the MST
 */
template <typename T>
std::vector<int> EdmondsAlgorithmDense(const int nverts, const std::vector<Edge<T> > &init_edges, const std::vector<int> &roots, const T &big_val)
{
    const int nedges = init_edges.size();
    std::vector<std::vector<T> > costs(nverts, std::vector<T>(nverts, big_val));
    std::vector<std::vector<int> > edgeMapping(nverts, std::vector<int>(nverts, -1));
    for(int e = 0; e < nedges; e++)
    {
        costs[init_edges[e].source][init_edges[e].target] = init_edges[e].weight;
        edgeMapping[init_edges[e].source][init_edges[e].target] = e;
    }

    std::vector<int> parents = EdmondsAlgorithmForDenseGraph(costs, roots);

    std::vector<int> inEdges(nverts, -1);
    for(int v = 0; v < nverts; v++)
        if(parents[v] >= 0)
            inEdges[v] = edgeMapping[parents[v]][v];

    return inEdges;
}

/**
 * \brief exhaustive search for the minimum spanning tree in a directed
 *
 *	N.B. this will not return a empty vector if there are some nodes that cannot be reached from the root
 *
 * \param nvertices number of vertices in the graph
 * \param edges edges in the graph
 * \param roots indices of root nodes
 *
 * \return indices of edges in the MST
 */
template <typename T>
std::vector<int> ExhaustiveSearch(const int nverts, const std::vector<Edge<T> > &init_edges, const std::vector<int> &roots)
{
	// root flags
	std::vector<bool> is_root(nverts, false);
	for(unsigned int r = 0; r < roots.size(); r++)
    {
        assert(roots[r]>=0 && roots[r]<nverts);
		is_root[roots[r]] = true;
    }

	// bookkeeping variables for graph contraction
	Mapping initEdgeMapping;
	std::vector<Edge<T> > edges;
	for(unsigned int e = 0; e < init_edges.size(); e++)
		if(!is_root[init_edges[e].target])	// only add if target is not a root
		{
			edges.push_back(init_edges[e]);
			initEdgeMapping.push_back(e);
		}

	const int nedges = edges.size();
	const int ncases = 1 << nedges;

	std::vector<int> rootFlags(nverts, 0);
	for(unsigned int i = 0; i < roots.size(); i++)
		rootFlags[i] = 1;
	
	// test all possible cases
	T bestScore = std::numeric_limits<T>::max();
	std::vector<int> bestMinInEdges;
	for(int i = 0; i < ncases; i++)
	{
		int nth = i;
		std::vector<int> edgeFlags(nedges, 0);
		for(int j = nedges-1; j >= 0; j--)
		{
			edgeFlags[j] = nth % 2;
			nth /= 2;	
		}

		// build parent list
		bool isValid = true;
		std::vector<int> minInEdges(nverts, -1);
		for(int j = 0; j < nedges; j++)
			if(edgeFlags[j] > 0)
			{
				const int p = edges[j].source;
				const int c = edges[j].target;

				if(minInEdges[c] < 0)
					minInEdges[c] = j;
				else
				{
					isValid = false;
					break;
				}
			}

		if(!isValid)
			continue;

		for(int j = 0; j < nverts; j++)
			if(rootFlags[j] == 0)			// not root
			{
				if(minInEdges[j] < 0)
				{
					isValid = false;
					break;
				}
			}
			else							// root
			{
				if(minInEdges[j] >= 0)
				{
					isValid = false;
					break;
				}
			}

		if(!isValid)
			continue;

		// check for no cycles
		std::vector<int> parents(nverts, -1);
		for(unsigned int j = 0; j < minInEdges.size(); j++)
		{
			const int e = minInEdges[j];
			if(e >= 0)
				parents[edges[e].target] = edges[e].source;
		}
		if(FindCycles(parents).size() > 0)
			continue;

		// compute score
		T newScore(0);
		for(int e = 0; e < nedges; e++)
			if(edgeFlags[e] == 1)
				newScore += edges[e].weight;

		if(newScore < bestScore)
		{
			bestScore = newScore;
			bestMinInEdges = minInEdges;
		}
	}

	// map back to original input edges
	std::vector<int> inEdges;
	if(bestMinInEdges.size() > 0)
	{
		inEdges = std::vector<int>(nverts, -1);
		for(unsigned int i = 0; i < bestMinInEdges.size(); i++)
			if(bestMinInEdges[i] >= 0)
				inEdges[i] = initEdgeMapping[bestMinInEdges[i]];
	}

	return inEdges;
}

/**
 * \brief compute the total cost of a tree
 *
 * \param edges edges of the graph
 * \param minInEdges indices of incoming edges for each node
 *
 * \return score of tree
 */
template <typename T>
T EvaluateTree(const std::vector<Edge<T> > &edges, const std::vector<int> &minInEdges)
{
	T score(0);
	for(unsigned int i = 0; i < minInEdges.size(); i++)
		if(minInEdges[i] >= 0)
			score += edges[minInEdges[i]].weight;

	return score;
}


// test function
bool EdmondsTest();


}


#endif
