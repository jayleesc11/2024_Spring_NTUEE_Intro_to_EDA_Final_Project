#ifndef OUTPUT_H
#define OUTPUT_H

#include <fstream>
#include <string>
#include <vector>

#include "input.h"
#include "net.h"

using namespace std;

struct OutputEdge {
    pair<float, float> start;
    pair<float, float> end;

    OutputEdge(pair<float, float> s, pair<float, float> e)
        : start(s), end(e) {}
};

class Output{
public:
    vector<vector<OutputEdge>> netEdges;
    vector<vector<OutputEdge>> bestNetEdges;

    Output(const string& caseNum, const int numNets);
    void updateNetEdges(const Net& net, const vector<Vertex>& vertices, const vector<vector<int>>& adjList);
    void writeOutput();

private:
    string outputFileName;
};

#endif