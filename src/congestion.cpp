#include "congestion.h"
#include <algorithm>
#include <limits>
#include <cmath>
#include <fstream>

Congestion::Congestion(int track, int unit, pair<int,int> dieArea, vector<Net>& nets) {
    int maxNum = 0;
    for(int i = 0; i < nets.size(); i++) {
        if(nets[i].numTracks > maxNum) {
            maxNum = nets[i].numTracks;
        }
    }

    capacity = float(track) / float(unit);
    cellWidth = float(maxNum * unit) / float(track);
    congestionMap.resize(dieArea.first / cellWidth + 1, vector<int>(dieArea.second / cellWidth + 1, 0));
    netPaths.resize(nets.size());
    hpwl.resize(nets.size(), 0);
    via.resize(nets.size(), 0);
}

vector<int> Congestion::reRouteNets(const vector<Net>& nets, const int rrr_threshold, const int rrr_num) {
    
    vector<int> routingNets;
    vector<bool> recorded(nets.size(), false);
    
    for(int i = 0; i < congestionMap.size(); i++) {
        for(int j = 0; j < congestionMap[i].size(); j++) {
            if( float(congestionMap[i][j]) / float(capacity * cellWidth) > rrr_threshold) {
                for(int n = 0; n < netPaths.size(); n++) {
                    if(recorded[n]) {
                        continue;
                    }
                    for(int k = 0; k < netPaths[n].size(); k++) {
                        if(netPaths[n][k] == make_pair(i,j)) {
                            routingNets.push_back(n);
                            recorded[n] = true;
                            break;
                        }
                    }
                }
            }
        }
    }

    random_shuffle(routingNets.begin(), routingNets.end());
    if(routingNets.size() > rrr_num) {
        routingNets = vector<int>(routingNets.begin(), routingNets.begin() + rrr_num);
    }
    for(const auto n : routingNets) {
        // Clear along net path
        for(int m = 0; m < netPaths[n].size(); m++) {
            if(congestionMap[netPaths[n][m].first][netPaths[n][m].second] > 0) {
                congestionMap[netPaths[n][m].first][netPaths[n][m].second] -= nets[n].numTracks;
            }
        }
        netPaths[n].clear();
        hpwl[n] = 0;
        via[n] = 0;
    }

    return routingNets;
}

void Congestion::updateCongestion(const Net& net, const vector<Vertex>& vertices, const vector<vector<int>>& adjList) {
    
    int numVia = 0;
    int minx = numeric_limits<int>::max();
    int miny = numeric_limits<int>::max();
    int maxx = numeric_limits<int>::min();
    int maxy = numeric_limits<int>::min();

    for(int i = 0; i < adjList.size(); i++) {
        if(adjList[i].size() == 0) {
            continue;
        }
        
        int startx = vertices[i].coord.first / cellWidth;
        int starty = vertices[i].coord.second / cellWidth;
        if(adjList[i].size() >= 1 && !(adjList[i].size() == 1 && vertices[i].coord == vertices[adjList[i][0]].coord)) {
            // Count turning cost
            if(!vertices[i].pin) {
                numVia++;
            }
            // Update HPWL
            if(startx < minx) {
                minx = startx;
            }
            if(starty < miny) {
                miny = starty;
            }
            if(startx > maxx) {
                maxx = startx;
            }
            if(starty > maxy) {
                maxy = starty;
            }
        }
        
        // Update congestion
        for(int j = 0; j < adjList[i].size(); j++) {
            // Undirected graph, avoid double counting
            if(i < adjList[i][j]){
                // Update congestion map
                int endx = vertices[adjList[i][j]].coord.first / cellWidth;
                int endy = vertices[adjList[i][j]].coord.second / cellWidth;
                if(startx == endx) {
                    for(int k = min(starty, endy); k <= max(starty, endy); k++) {
                        netPaths[net.id].push_back({startx, k});
                        congestionMap[startx][k] += net.numTracks;
                    }
                } else {
                    for(int k = min(startx, endx); k <= max(startx, endx); k++) {
                        netPaths[net.id].push_back({k, starty});
                        congestionMap[k][starty] += net.numTracks;
                    }
                }
            }
        }
    }

    hpwl[net.id] = (maxx - minx) + (maxy - miny);
    via[net.id] = numVia;
}

int Congestion::printOverflow() {
    float overflowCost = 0;
    float turningCost = 0;
    pair<int,int> maxVia = make_pair(0, 0);

    for(int i = 0; i < netPaths.size(); i++) {
        float netCost = 0;
        for(int j = 0; j < netPaths[i].size(); j++) {
            float factor = float(congestionMap[netPaths[i][j].first][netPaths[i][j].second]) / float(capacity * cellWidth);
            if(factor > 0.7) {
                netCost += (1 + factor);
            } else {
                netCost += 1;
            }
        }

        if(hpwl[i] != 0) {
            overflowCost += float(netCost) / float(hpwl[i]);
        }
        if(via[i] > maxVia.second) {
            maxVia = make_pair(i, via[i]);
        }
        if(via[i] > 1) {
            turningCost += exp(via[i]);
        }
    }

    if(overflowCost < get<0>(bestCost)) {
        bestCost = {overflowCost, turningCost, maxVia};
        bestMap = congestionMap;
    }

    cout << "Overflow Cost = " << overflowCost << endl;

    return overflowCost;
}

void Congestion::printCost() {
    cout << "--- Best Cost ---" << endl;
    cout << "Overflow Cost = " << get<0>(bestCost) << endl;
    // cout << "Turning cost = " << get<1>(bestCost) << " when net " << get<2>(bestCost).first << " has Max # of vias = " << get<2>(bestCost).second << endl;
    // cout << "Total Cost = " << 0.55 * get<0>(bestCost) + 0.01 * get<1>(bestCost) << endl;
}

void Congestion::writeMap(const string& mapfile) {
    ofstream mapOut(mapfile);
    for(int i = 0; i < bestMap.size(); i++) {
        for(int j = 0; j < bestMap[i].size(); j++) {
            mapOut << bestMap[i][j];
            if (j < bestMap[i].size() - 1) {
                mapOut << " ";
            }
        }
        mapOut << endl;
    }
    mapOut.close();
}