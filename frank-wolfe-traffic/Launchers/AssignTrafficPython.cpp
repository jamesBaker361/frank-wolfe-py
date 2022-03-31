#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <random>
#include <stack>
#include <stdexcept>
#include <string>
#include <vector>
#include <map>
#include <bits/stdc++.h> 
#include <iostream> 
#include <sys/stat.h> 
#include <sys/types.h> 

#include <boost/dynamic_bitset.hpp>
#include <routingkit/customizable_contraction_hierarchy.h>
#include <routingkit/nested_dissection.h>

#include "Algorithms/TrafficAssignment/Adapters/DijkstraAdapter.h"
#include "Algorithms/TrafficAssignment/Adapters/ConstrainedAdapter.h"
#include "Algorithms/TrafficAssignment/ObjectiveFunctions/SystemOptimum.h"
#include "Algorithms/TrafficAssignment/ObjectiveFunctions/UserEquilibrium.h"
#include "Algorithms/TrafficAssignment/ObjectiveFunctions/CombinedEquilibrium.h"
#include "Algorithms/TrafficAssignment/TravelCostFunctions/BprFunction.h"
#include "Algorithms/TrafficAssignment/TravelCostFunctions/ModifiedBprFunction.h"
#include "Algorithms/TrafficAssignment/FrankWolfeAssignment.h"
#include "DataStructures/Graph/Graph.h"
#include "DataStructures/Utilities/OriginDestination.h"
#include "Tools/CommandLine/CommandLineParser.h"
#include "Tools/Timer.h"
//#include "Launchers/Garbage.cpp"
#include "Launchers/GarbageList.cpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

//PYBIND11_MAKE_OPAQUE(std::map<std::string, std::vector<int>>)


using ShortestPathAlgoT=DijkstraAdapter;
using base= FrankWolfeAssignment<UserEquilibrium,BprFunction,ShortestPathAlgoT>;
using AllOrNothing = AllOrNothingAssignment<ShortestPathAlgoT>;
namespace py = pybind11;

/**
 * It takes in a map of demand, a map of edges, and a number of iterations. It then runs the assignment
 * algorithm on the graph with the given demand and edges.
 * 
 * @param demand a map of origin-destination pairs of the form {origin: [], destination: [], volume:[]}
 * @param edges a map of edges {edge_tail: [], "edge_head":[], "length":[], "capacity":[], "speed":[]}
 * @param numIterations the number of iterations to run the algorithm for.
 * 
 * @return A map of the form {tail: [], head: [],flow: []}
 */
std::map<std::string,std::vector<int>> flow(std::map<std::string,std::vector<int>> demand, std::map<std::string,std::vector<int>> edges, int numIterations){
	Graph graph(edges,0.0,100.0);
	std::vector<ClusteredOriginDestination> odPairs =importODPairsFrom(demand);
	bool verbose =false;
	bool elasticRebalance=false;
	FrankWolfeAssignment<UserEquilibrium,BprFunction,DijkstraAdapter> assign(graph,odPairs,verbose,elasticRebalance);

	return assign.runPython(numIterations);
}

int add(int i, int j) {
    return i + j;
}

FrankWolfeAssignment<UserEquilibrium,BprFunction,DijkstraAdapter> getFWAssignment(std::map<std::string,std::vector<int>> demand, std::map<std::string,std::vector<int>> edges){
	Graph graph(edges,0.0,100.0);
	std::vector<ClusteredOriginDestination> odPairs =importODPairsFrom(demand);
	bool verbose =false;
	bool elasticRebalance=false;
	FrankWolfeAssignment<UserEquilibrium,BprFunction,DijkstraAdapter> assign(graph,odPairs,verbose,elasticRebalance);
	return assign;
}


PYBIND11_MODULE(frankwolfe, m) {
    m.doc() = "pybind11  plugin"; // optional module docstring
	m.def("importODPairsFrom",&importODPairsFrom); //importODPairsFrom(std::map<std::string,std::vector<int>> demand)
    m.def("add", &add, "A function that adds two numbers");
	m.def("flow", &flow, "calculate traffic flow");
	m.def("getFWAssignment",&getFWAssignment, "return object that calculates flow");
	py::class_<Garbage> (m, "Garbage")
		.def(py::init([](int garbageInt){
			return new Garbage(garbageInt);
		}));
	py::class_<GarbageList> (m, "GarbageList")
		.def(py::init([](std::vector<Garbage> gList){
			return new GarbageList(gList);
		}))
		.def("add",&GarbageList::add);
	py::class_<AllOrNothing> (m, "AllOrNothingAssignment");
	py::class_<ClusteredOriginDestination> (m,"ClusteredOriginDestination")
		.def_readwrite("origin",&ClusteredOriginDestination::origin)
		.def_readwrite("destination",&ClusteredOriginDestination::destination)
		.def_readwrite("volume",&ClusteredOriginDestination::volume)
		.def_readwrite("rebalancer",&ClusteredOriginDestination::rebalancer)
		.def_readwrite("edge1",&ClusteredOriginDestination::edge1)
		.def_readwrite("edge2",&ClusteredOriginDestination::edge2)
		.def(py::init([](const int o, const int d, const int r, const int e1, const int e2, const int v){
			return new ClusteredOriginDestination(o,d,r,e1,e2,v);
		}));
	py::class_<Graph> (m, "Graph") //std::map<std::string,std::vector<int>> edges,const double ceParameter, const double constParameter)
		.def("updateEdges",&Graph::updateEdges)
		.def(py::init([](std::map<std::string,std::vector<int>> edges,const double ceParameter, const double constParameter){
			return new Graph(edges, ceParameter,constParameter);
		}));
	py::class_<base>(m, "FrankWolfeAssignment")
		.def("runPython", &base::runPython)
		.def("updateEdges",&base::updateEdges)
		.def(py::init([](Graph& graph, const std::vector<ClusteredOriginDestination>& odPairs, const bool verbose, const bool elasticRebalance) {
        	return new base(graph,odPairs,verbose,elasticRebalance);
    	}));
}
