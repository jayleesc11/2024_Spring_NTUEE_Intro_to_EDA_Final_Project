#include <vector>
#include <limits>
#include "net.h"
#include "input.h"

#ifndef CONGESTION_H
#define CONGESTION_H

class Congestion {
public:
    Congestion(int track, int unit, pair<int,int> dieArea, vector<Net>& nets);
    float capacity;                         // Unit: tracks/coordinate unit (not um)
    float cellWidth;                        // Unit: 1 coordinate unit (not um)
    vector<vector<int>> congestionMap;      // congestionMap[x/cellWidth][y/cellWidth] = congestion at (x,y)
    vector<vector<int>> bestMap;
    tuple<float,float, pair<int,int>> bestCost = {numeric_limits<int>::max(), 0, make_pair(0,0)};           // Overflow, viacost, Maxvia info of best overflow
    
    // Methods
    vector<int> reRouteNets(const vector<Net>& nets, const int rrr_threshold, const int rrr_num);
    void updateCongestion(const Net& net, const vector<Vertex>& vertices, const vector<vector<int>>& adjList);
    void printCost();
    int printOverflow();
    void writeMap(const string& mapfile);

private:
    vector<vector<pair<int,int>>> netPaths; // All (x,y) on the path of each net
    vector<int> hpwl;                       // HPWL of each net
    vector<int> via;                        // Number of vias of each net
};

#endif