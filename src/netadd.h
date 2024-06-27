#ifndef NETADD_H
#define NETADD_H

#include "net.h"
#include "defParser.h"
#include <iostream>
#include <string>

class Netadd {
    private:
    public:
    Netadd() {};
    vector<Net> modify(vector<Net>& nets,std::unordered_map<std::string, std::vector<Component>>& comp, std::unordered_map<std::string, std::vector<Region>>& regs);
};




#endif