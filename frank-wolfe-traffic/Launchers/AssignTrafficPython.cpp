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
#include <any>
#include <bits/stdc++.h> 
#include <iostream> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <variant>

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

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
namespace py = pybind11;

//PYBIND11_MAKE_OPAQUE(std::map<std::string, std::vector<int>>)



#define PICKLECLASS(classname, valuetype, ...) def(py::pickle([](const classname &g){ return g.getState();},[](std::map<std::string,valuetype> dict){ classname g(__VA_ARGS__); g.setState(dict); return g; }));
#define PICKLECLASS_NOCONS(classname, valuetype) def(py::pickle([](const classname &g){ return g.getState();},[](std::map<std::string,valuetype> dict){ classname g; g.setState(dict); return g; }));
#define PICKLECLASSINT(classname, ...) def(py::pickle([](const classname &g){ return g.getState();},[](std::map<std::string,int> dict){ classname g(__VA_ARGS__); g.setState(dict); return g; }));
#define PICKLECLASSDOUBLE(classname) def(py::pickle([](const classname &g){ return g.getState();},[](std::map<std::string,double> dict){ classname g; g.setState(dict); return g; }));
#define PICKLEGETSTATE(classname) [](const classname &g){ return g.getState();}

using ShortestPathAlgoT=DijkstraAdapter;
using base= FrankWolfeAssignment<UserEquilibrium,BprFunction,ShortestPathAlgoT>;
using AllOrNothing = AllOrNothingAssignment<ShortestPathAlgoT>;


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
	std::vector<ClusteredOriginDestination> ODPairs =importODPairsFrom(demand);
	bool verbose =false;
	bool elasticRebalance=false;
	FrankWolfeAssignment<UserEquilibrium,BprFunction,DijkstraAdapter> assign(graph,ODPairs,verbose,elasticRebalance);

	return assign.runPython(numIterations);
}

int add(int i, int j) {
    return i + j;
}

FrankWolfeAssignment<UserEquilibrium,BprFunction,DijkstraAdapter> getFWAssignment(std::map<std::string,std::vector<int>> demand, std::map<std::string,std::vector<int>> edges){
	Graph graph(edges,0.0,100.0);
	std::vector<ClusteredOriginDestination> ODPairs =importODPairsFrom(demand);
	bool verbose =false;
	bool elasticRebalance=false;
	FrankWolfeAssignment<UserEquilibrium,BprFunction,DijkstraAdapter> assign(graph,ODPairs,verbose,elasticRebalance);
	return assign;
}


PYBIND11_MODULE(frankwolfe, m) {
    m.doc() = "pybind11  plugin"; // optional module docstring
	m.def("importODPairsFrom",&importODPairsFrom); //importODPairsFrom(std::map<std::string,std::vector<int>> demand)
    m.def("add", &add, "A function that adds two numbers");
	m.def("flow", &flow, "calculate traffic flow");
	m.def("getFWAssignment",&getFWAssignment, "return object that calculates flow");
	py::class_<ClusteredOriginDestination> (m,"ClusteredOriginDestination")
		.def_readwrite("origin",&ClusteredOriginDestination::origin)
		.def_readwrite("destination",&ClusteredOriginDestination::destination)
		.def_readwrite("volume",&ClusteredOriginDestination::volume)
		.def_readwrite("rebalancer",&ClusteredOriginDestination::rebalancer)
		.def_readwrite("edge1",&ClusteredOriginDestination::edge1)
		.def_readwrite("edge2",&ClusteredOriginDestination::edge2)
		.def(py::init([](int o, int d, int r, int e1, int e2, int v){
			return new ClusteredOriginDestination(o,d,r,e1,e2,v);
		}))
		.PICKLECLASS(ClusteredOriginDestination,py::object,0,0,0,0,0,0);
	py::class_<Graph> (m, "Graph") //std::map<std::string,std::vector<int>> edges,const double ceParameter, const double constParameter)
		.def("updateEdges",&Graph::updateEdges)
		.def_readwrite("vertexNum",&Graph::vertexNum)
		.def_readwrite("edgeTail",&Graph::edgeTail)
		.def_readwrite("edgeHead",&Graph::edgeHead)
		.def_readwrite("edgeLength",&Graph::edgeLength)
		.def_readwrite("edgeCapacity",&Graph::edgeCapacity)
		.def_readwrite("edgeSpeed",&Graph::edgeSpeed)
		.def_readwrite("edgeWeight",&Graph::edgeWeight)
		.def_readwrite("edgeFreeTravelTime",&Graph::edgeFreeTravelTime)
		.def(py::init([](std::map<std::string,std::vector<int>> edges,const double ceParameter, const double constParameter){
			return new Graph(edges, ceParameter,constParameter);
		}))
		.PICKLECLASS_NOCONS(Graph,py::object)
		/*
		.def(py::pickle([](){ // __getstate__

		},[](){ // __setstate__

		})); */
	py::class_<base>(m, "FrankWolfeAssignment")
		.def("runPython", &base::runPython)
		.def("updateEdges",&base::updateEdges)
		.def(py::init([](Graph& graph, std::vector<ClusteredOriginDestination>& ODPairs, const bool verbose, const bool elasticRebalance) {
        	return new base(graph,ODPairs,verbose,elasticRebalance);
    	})).def(py::pickle([](const base &fwa){ // __getstate__
			return fwa.getState();
		},[](std::map<std::string,py::object> dict){ // __setstate__
			Graph g=dict["graph"].cast<Graph>();
			std::vector<ClusteredOriginDestination> ODPairs = dict["ODPairs"].cast<std::vector<ClusteredOriginDestination>>();
			bool verbose=dict["verbose"].cast<bool>();
			bool elasticRebalance=dict["elasticRebalance"].cast<bool>();
			return new base(g,ODPairs,verbose,elasticRebalance);
		}));
}
