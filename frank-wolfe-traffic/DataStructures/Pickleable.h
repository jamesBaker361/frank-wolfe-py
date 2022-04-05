#pragma once
#include <any>
#include <map>

class Pickleable {
    virtual void setState(std::map<std::string, std::any> dict)=0;
    virtual void getState()=0;
};