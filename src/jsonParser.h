#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <vector>
#include <string>
#include "block.h"
#include "net.h"

using namespace std;

class JsonParser {
    public:
        JsonParser() {};
        void parseCfgJson(const string& cfgFile, unordered_map<string, Block>& blocks);
        void parseCmJson(const string& cmFile, vector<Net>& nets);
};

#endif