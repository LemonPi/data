#pragma once
#include <queue>
#include <stack>
#include <unordered_map>
#include "adjacency_list.h"

namespace sal {

// assumes non-negative weighted edges, O((V+E)lgV) with binary heap (priority queue)
template <typename Graph, typename V>
size_t dijkstra(const Graph& g, V s, V x) {
	return 0;
}

// assumes unweighted graph, and assumes V is simple type (name of vertex)
// works with directed and undirected
enum class Status {UNEXPLORED, EXPLORING, EXPLORED};

template <typename V>
struct Vertex_bfs_property {
	// distance estimate of s to vertex, always >= distance
	// after completion, estimate is == distance
	size_t distance;
	// flattened tree to trace back path, unnecessary if only path weight needed
	V parent;
};

template <typename V>
struct Vertex_dfs_property {
	// time stamps
	// discovery and finish time, from 1 to |V|
	size_t start;
	size_t finish;
	V parent;
};

template <typename V>
using Vertex_bfs_property_map = std::unordered_map<V, Vertex_bfs_property<V>>;
template <typename Graph>
using VBP = Vertex_bfs_property_map<typename Graph::vertex_type>;

template <typename V>
using Vertex_dfs_property_map = std::unordered_map<V, Vertex_dfs_property<V>>;
template <typename Graph>
using VDP = Vertex_dfs_property_map<typename Graph::vertex_type>;

// O(V + E) time
// creates a vertex property map relative to source (breadth first tree)
/* expects Graph to have: 
	vertex iterators in begin() and end()
  		* gives vertex name of type V

  	adjacent iterator pair in adjacent(V)
  		dest() gives name of destination vertex
  		weight() gives weight of edge to destination vertex
*/
constexpr size_t unsigned_infinity {std::numeric_limits<size_t>::max()};

template <typename Graph>
VBP<Graph> bfs(const Graph& g, typename Graph::vertex_type s) {
	using V = typename Graph::vertex_type;
	VBP<Graph> property;
	for (auto v = g.begin(); v != g.end(); ++v) {
		// auto& v_p = property[*v];
		// v_p.distance = std::numeric_limits<size_t>::max();
		// v_p.status = Status::UNEXPLORED;
		property[*v] = {unsigned_infinity, *v};
	}

	property[s].distance = 0;
	property[s].parent = s;

	std::queue<V> exploring;
	exploring.push(s);

	while (!exploring.empty()) {
		V cur {exploring.front()};
		// leaving exploring means fully explored
		exploring.pop();

		// explore adjacent to current
		auto edges = g.adjacent(cur);
		for (auto adj = edges.first; adj != edges.second; ++adj) {
			auto& n_p = property[adj.dest()];
			// somehow decide if it's been explored or not
			if (n_p.distance == unsigned_infinity) {
				n_p.distance = property[cur].distance + 1;
				n_p.parent = cur;
				exploring.push(adj.dest());
			}
		}
	}

	return std::move(property);
}

// depth first search, used usually in other algorithms
// explores all vertices of a graph, produces a depth-first forest
template <typename Graph>
VDP<Graph> dfs(const Graph& g) {
	using V = typename Graph::vertex_type;
	VDP<Graph> property;
	std::stack<V> exploring;
	// reverse order to have stack in order
	for (auto v = g.rbegin(); v != g.rend(); ++v) {
		// mark unexplored
		property[*v] = {unsigned_infinity, 0, *v};
		// expore every vertex
		exploring.push(*v);
	}

	size_t explore_time {0};

	while (!exploring.empty()) {
		V cur {exploring.top()};
		if (property[cur].start == unsigned_infinity)
			property[cur].start = ++explore_time;

		bool fully_explored {true};
		auto edges = g.adjacent(cur);
		for (auto adj = edges.first; adj != edges.second; ++adj) {
			// check if any neighbours haven't been explored
			if (property[adj.dest()].start == unsigned_infinity) {
				property[adj.dest()].start = ++explore_time;
				property[adj.dest()].parent = cur;
				exploring.push(adj.dest());
				fully_explored = false;	// still have unexplored neighbours
				break;		// only push 1 neighbour to achieve depth first
			}
		}
		if (fully_explored) {
			exploring.pop();
			if (property[cur].finish == 0)	// default value
				property[cur].finish = ++explore_time;
		}
	}

	return std::move(property);
}

// depth first search only on a starting vertex
template <typename Graph>
VDP<Graph> dfs(const Graph& g, typename Graph::vertex_type s) {
	using V = typename Graph::vertex_type;
	VDP<Graph> property;
	std::stack<V> exploring;
	// no need to reverse traverse now
	for (auto v = g.begin(); v != g.end(); ++v)
		property[*v] = {unsigned_infinity, 0, *v};

	exploring.push(s);
	size_t explore_time {0};

	while (!exploring.empty()) {
		V cur {exploring.top()};
		if (property[cur].start == unsigned_infinity)
			property[cur].start = ++explore_time;

		bool fully_explored {true};
		auto edges = g.adjacent(cur);
		for (auto adj = edges.first; adj != edges.second; ++adj) {
			// check if any neighbours haven't been explored
			if (property[adj.dest()].start == unsigned_infinity) {
				property[adj.dest()].start = ++explore_time;
				property[adj.dest()].parent = cur;
				exploring.push(adj.dest());
				fully_explored = false;	// still have unexplored neighbours
				break;		// only push 1 neighbour to achieve depth first
			}
		}
		if (fully_explored) {
			exploring.pop();
			if (property[cur].finish == 0)	// default value
				property[cur].finish = ++explore_time;
		}
	}

	return std::move(property);
}


}