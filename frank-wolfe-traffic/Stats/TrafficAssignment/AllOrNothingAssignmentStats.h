#pragma once

#include <cstdint>
#include <vector>

#include "DataStructures/Pickleable.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>


namespace py = pybind11;

class DummyObject{
  public:
  DummyObject(int dumb) : dumb(dumb){}
  DummyObject() : dumb(0) {}

  int dumb;

  void setState(std::map<std::string,py::object> dict){
    SET_1(dumb,int)
  }

  std::map<std::string,py::object> getState() const {
    std::map<std::string,py::object> dict;
    GET_1(dumb)
    return dict;
  }
};

// Statistics about an iterative all-or-nothing assignment, including checksums and running times.
class AllOrNothingAssignmentStats {
  // Constructs a struct collecting statistics about an iterative all-or-nothing assignment.
  public:
  AllOrNothingAssignmentStats(const int numODPairs)
      : lastChecksum(0),
        totalChecksum(0),
        lastDistances(numODPairs, -1),
        maxChangeInDistances(0),
        avgChangeInDistances(0),
        lastCustomizationTime(0),
        lastQueryTime(0),
        lastRoutingTime(0),
        totalPreprocessingTime(0),
        totalCustomizationTime(0),
        totalQueryTime(0),
        totalRoutingTime(0),
        numIterations(0)
       {

       }

  AllOrNothingAssignmentStats(const AllOrNothingAssignmentStats &other){
    D_COPY(lastChecksum)
    D_COPY(totalChecksum)
    D_COPY(maxChangeInDistances)
    D_COPY(avgChangeInDistances)
    D_COPY(lastCustomizationTime)
    D_COPY(lastQueryTime)
    D_COPY(lastRoutingTime)
    D_COPY(totalPreprocessingTime)
  }

  // Resets the values from the last iteration.
  void startIteration() {
    lastChecksum = 0;
    maxChangeInDistances = 0;
    avgChangeInDistances = 0;
  }

  void reset() {
    lastDistances = std::vector<int>(lastDistances.size(),-1);
    numIterations=0;
  }

  void addToDistances(int d){
    lastDistances.push_back(d);
  }

  // Adds the values from the last iteration to the totals.
  void finishIteration() {
    lastRoutingTime = lastCustomizationTime + lastQueryTime;
    totalChecksum += lastChecksum;
    totalCustomizationTime += lastCustomizationTime;
    totalQueryTime += lastQueryTime;
    totalRoutingTime += lastRoutingTime;
  }
/*
  void setState(std::map<std::string,py::object> dict){
    SET_STATE(totalChecksum,int64_t,
      lastDistances,std::vector<int>)
    //SET_1(dObj,DummyObject)
    //totalChecksum=dict["totalChecksum"].cast<int64_t>();
    //lastDistances=dict["lastDistances"].cast<std::vector<int>>();
    //std::cout<< "my self"<<std::endl;
    //dObj=py::cast<DummyObject>(dict["dObj"]);
    SET_STATE_CAST(lastChecksum,int64_t,
      totalChecksum, int64_t,
    lastDistances, std::vector<int>,
    maxChangeInDistances, double,
    avgChangeInDistances, double,
    lastCustomizationTime, int,
      lastQueryTime, int,
      lastRoutingTime, int,
      totalPreprocessingTime, int,
      totalCustomizationTime, int,
      totalQueryTime, int,
      totalRoutingTime, int,
      numIterations, int)
  }

  std::map<std::string,py::object> getState() const {
    std::map<std::string,py::object> dict;
    GET_STATE(totalChecksum,lastDistances)
    //dict["totalChecksum"]=py::cast(totalChecksum);
    //dict["lastDistances"]=py::cast(lastDistances);
    //dict["dObj"]=py::cast(dObj);
    GET_STATE(lastChecksum,totalChecksum,
    lastDistances,
    maxChangeInDistances,avgChangeInDistances,lastCustomizationTime,
      lastQueryTime, lastRoutingTime,totalPreprocessingTime,
      totalCustomizationTime, totalQueryTime,totalRoutingTime, numIterations)
      return dict;
  }*/
  GET_FUNCTION(totalChecksum,lastDistances)
  SET_FUNCTION(totalChecksum,int64_t,
      lastDistances,std::vector<int>)

  int64_t lastChecksum;  // The sum of the distances computed in the last iteration.
  int64_t totalChecksum; // The total sum of distances computed.

  std::vector<int> lastDistances; // The OD-distances from the last iteration.
  double maxChangeInDistances;    // The max change in the OD-distances between the last iterations.
  double avgChangeInDistances;    // The avg change in the OD-distances between the last iterations.

  int lastCustomizationTime; // The time spent on customization in the last iteration.
  int lastQueryTime;         // The time spent on queries in the last iteration.
  int lastRoutingTime;       // The time spent on routing in the last iteration.

  int totalPreprocessingTime; // The total time spent on preprocessing.
  int totalCustomizationTime; // The total time spent on customization.
  int totalQueryTime;         // The total time spent on queries.
  int totalRoutingTime;       // The total time spent on routing.

  int numIterations; // The number of iterations performed.
};
