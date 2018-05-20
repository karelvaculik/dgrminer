#include "OverlapGraph.hpp"
#include <algorithm>

namespace dgrminer {
	OverlapGraph::OverlapGraph(bool heuristic) : heuristic(heuristic) {}

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
	  	map<int,vector<int> > nodeList;

		gio.m_vi_Vertices.push_back(gio.m_vi_Edges.size());

		for (auto const &adjacencyList : adjacencyLists) {
		  	std::set<int> inverseResult;

		  	std::set_difference(
		  		vertices.begin(),
			    vertices.end(),
			    adjacencyList.second.begin(),
			    adjacencyList.second.end(),
			    std::inserter(inverseResult, inverseResult.end())
		  	);

		  	for (auto result : inverseResult) {
			  	if (result < adjacencyList.first) {
				  	nodeList[adjacencyList.first].push_back(result);
				  	nodeList[result].push_back(adjacencyList.first);
			  	}
		  	}
		}

	  	for(int i=0; i < vertices.size(); ++i) {
		  	gio.m_vi_Edges.insert(gio.m_vi_Edges.end(),nodeList[i].begin(),nodeList[i].end());
		  	gio.m_vi_Vertices.push_back(gio.m_vi_Edges.size());
		}

		gio.CalculateVertexDegrees();

		if (heuristic){
		  	return maxCliqueHeu(gio);
		}

		std::vector<int> max_clique_data;
		return maxClique(gio, 0, max_clique_data);
	}
}