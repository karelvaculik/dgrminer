#ifndef SRC_OVERLAPGRAPH_HPP
#define SRC_OVERLAPGRAPH_HPP

#include <set>
#include <map>
#include "fmc/findClique.h"
#include "fmc/graphIO.h"

namespace dgrminer {
	class OverlapGraph {
	  	bool heuristic;
	  	int newVertex = 0;
	  	std::set<int> vertices;
	  	std::map<int, std::set<int>> adjacencyLists;

	public:
	  	OverlapGraph(bool heuristic);
	  	int addVertex();
	  	void addEdge(int srcVertexId, int dstVertexId);
	  	int computeSupport();
	};
}

#endif //SRC_OVERLAPGRAPH_HPP
