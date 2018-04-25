#include "OverlapGraph.hpp"

namespace dgrminer {
	int OverlapGraph::addVertex() {
	 	vertices.insert(newVertex);
	  	adjacencyLists[newVertex].clear();
	 	return newVertex++;
	}

	void OverlapGraph::addEdge(int srcVertexId, int dstVertexId) {
	  	adjacencyLists[srcVertexId].insert(dstVertexId);
	  	adjacencyLists[dstVertexId].insert(srcVertexId);
	}

	int OverlapGraph::computeSupport() {
		// create inverse graph and calculate maximum clique
		CGraphIO gio;

		gio.m_vi_Vertices.push_back(gio.m_vi_Edges.size());
		std::set<int> inverseResult;

		for (auto const &adjacencyList : adjacencyLists) {
		  	inverseResult.clear();

		  	std::set_difference(
		  		vertices.begin(),
			    vertices.end(),
			    adjacencyList.second.begin(),
			    adjacencyList.second.end(),
			    std::inserter(inverseResult, inverseResult.end())
		  	);

		  	gio.m_vi_Edges.insert(gio.m_vi_Edges.end(), inverseResult.begin(), inverseResult.end());
		  	gio.m_vi_Vertices.push_back(gio.m_vi_Edges.size());
		}

		gio.CalculateVertexDegrees();

		inverseResult.clear();

		std::vector<int> max_clique_data;
		return maxClique(gio, 0, max_clique_data);
	}
}