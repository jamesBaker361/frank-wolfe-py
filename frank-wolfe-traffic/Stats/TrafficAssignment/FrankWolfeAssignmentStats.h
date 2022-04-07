#pragma once
#include <any>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
namespace py = pybind11;

// Statistics about a Frank-Wolfe assignment, including times and measures of solution quality.
class FrankWolfeAssignmentStats {
  // Constructs a struct collecting statistics about a Frank-Wolfe assignment.
  public:
  FrankWolfeAssignmentStats()
      : objFunctionValue(0),
        totalTravelCost(0),
        lastLineSearchTime(0),
        lastRunningTime(0),
        totalLineSearchTime(0),
        totalRunningTime(0) {}

  // Resets the values from the last iteration.
  void startIteration() {
    totalTravelCost = 0;
  }

  // Adds the values from the last iteration to the totals.
  void finishIteration() {
    totalLineSearchTime += lastLineSearchTime;
    totalRunningTime += lastRunningTime;
  }

  /*std::map<std::string, py::object> getState() const {
			std::map<std::string, py::object> dict;
      GET_STATE(objFunctionValue,totalTravelCost,lastLineSearchTime,lastRunningTime)
			return dict;
		}
    GET_FUNCTION(objFunctionValue,totalTravelCost,lastLineSearchTime,lastRunningTime)

  void setState(std::map<std::string,py::object> dict){ 
    SET_STATE(objFunctionValue,double,
    totalTravelCost,double,
    lastLineSearchTime,int)
  }*/
  GET_FUNCTION(objFunctionValue,totalTravelCost,lastLineSearchTime,lastRunningTime)
  SET_FUNCTION(objFunctionValue,double,
    totalTravelCost,double,
    lastLineSearchTime,int)

  double objFunctionValue; // The value of the objective function resulting from current edge flows.
  double totalTravelCost;  // The total travel cost resulting from current edge flows.

  int lastLineSearchTime; // The time spent on the line search in the last iteration.
  int lastRunningTime;    // The running time for the last iteration.

  int totalLineSearchTime; // The total time spent on the line search.
  int totalRunningTime;    // The total running time.
};
