#include <algorithm>
#include <iostream>
#include "oasg.h"

using namespace std;

void OASG::printOASG() const {
    // for(int i = 0; i < adjList.size(); i++) {
    //     cout << fixed;
    //     cout.precision(0);
    //     cout << "Vertex " << i << " (" << vplist[i].coord.first << "," << vplist[i].coord.second << ") -> ";
    //     for(const auto& edge : adjList[i]) {
    //         cout << edge << "(" << vplist[edge].coord.first << "," << vplist[edge].coord.second << "),";
    //     }
    //     cout << endl;
    // }
    cout << "Edges:" << endl;
    for(int i = 0; i < adjList.size(); i++) {
        for(const auto& edge : adjList[i]) {
            if(i < edge) {
                cout << "(" << i << "," <<  edge << "), ";
            }
        }
    }
    cout << endl << endl << "Vertices: " << endl;
    for(int i = 0; i < vplist.size(); i++) {
        cout << fixed;
        cout.precision(0);
        cout << i << ": (" << vplist[i].coord.first << "," << vplist[i].coord.second << "), ";
    }
    cout << endl;
}

void OASG::buildOASG(const vector<Obstacles>& ob, const vector<Vertex>& vertices, const vector<pair<int,int>>& removeEdges, const Net& net, vector<Vertex>& addv) {
    
    obstacles = ob;
    
    // Sort obstacles by x-coordinate in increasing order
    incOlist = obstacles;
    sort(incOlist.begin(), incOlist.end(), [](const Obstacles& a, const Obstacles& b) {
        return a.start.first < b.start.first;
    });
    // Sort obstacles by x-coordinate in decreasing order
    decOlist = obstacles;
    sort(decOlist.begin(), decOlist.end(), [](const Obstacles& a, const Obstacles& b) {
        return a.end.first > b.end.first;
    });

    // Input net TX and RX (pins)
    vplist = vertices;
    int vertexid = vertices.size();
    if(addv.empty()) {
        vplist.emplace_back(vertexid++, net.txCoord, true);
        for (const auto& rx : net.rxCoord) {
            vplist.emplace_back(vertexid++, rx, true);
        }
    }
    for(const auto& v : addv) {
        vplist.emplace_back(vertexid++, v.coord, false);
    }
    adjList.clear();
    adjList.resize(vplist.size());

    
    incPlist.assign(vplist.begin() + numObVertices, vplist.end());
    decPlist = incPlist;
    // Sort pins by x-coordinate in increasing order
    sort(incPlist.begin(), incPlist.end(), [](const Vertex& a, const Vertex& b) {
        return a.coord.first < b.coord.first;
    });
    // Sort pins by x-coordinate in decreasing order
    sort(decPlist.begin(), decPlist.end(), [](const Vertex& a, const Vertex& b) {
        return a.coord.first > b.coord.first;
    });

    // Find OASG edges of each vertex
    for(auto it = vplist.begin() + numObVertices; it != vplist.end(); it++) {
        OASG_R1(*it, -1);
        OASG_R2(*it, -1);
        OASG_R3(*it, -1);
        OASG_R4(*it, -1);
    }
    for(const auto& obstacle : obstacles) {
        // c1
        OASG_R1(vplist[obstacle.c1], obstacle.obstacleid);
        OASG_R2(vplist[obstacle.c1], obstacle.obstacleid);
        OASG_R4(vplist[obstacle.c1], obstacle.obstacleid);
        // c2
        OASG_R1(vplist[obstacle.c2], obstacle.obstacleid);
        OASG_R2(vplist[obstacle.c2], obstacle.obstacleid);
        OASG_R3(vplist[obstacle.c2], obstacle.obstacleid);
        // c3
        OASG_R2(vplist[obstacle.c3], obstacle.obstacleid);
        OASG_R3(vplist[obstacle.c3], obstacle.obstacleid);
        OASG_R4(vplist[obstacle.c3], obstacle.obstacleid);
        // c4
        OASG_R1(vplist[obstacle.c4], obstacle.obstacleid);
        OASG_R3(vplist[obstacle.c4], obstacle.obstacleid);
        OASG_R4(vplist[obstacle.c4], obstacle.obstacleid);
    }

    // Remove duplicate edges and self loop
    for(int i = 0; i < adjList.size(); i++) {
        sort(adjList[i].begin(), adjList[i].end());
        adjList[i].erase(unique(adjList[i].begin(), adjList[i].end()), adjList[i].end());
        adjList[i].erase(remove(adjList[i].begin(), adjList[i].end(), i), adjList[i].end());
    }

    // Remove edges in removeEdges
    for(const auto& edge : removeEdges) {
        adjList[edge.first].erase(remove(adjList[edge.first].begin(), adjList[edge.first].end(), edge.second), adjList[edge.first].end());
        adjList[edge.second].erase(remove(adjList[edge.second].begin(), adjList[edge.second].end(), edge.first), adjList[edge.second].end());

        if(vplist[edge.first].coord.first == vplist[edge.second].coord.first) {
            int up, down;
            if(vplist[edge.first].coord.second > vplist[edge.second].coord.second) {
                up = edge.first;
                down = edge.second;
            } else {
                up = edge.second;
                down = edge.first;
            }
            for(int i = 0; i < adjList[up].size(); i++) {
                if(vplist[adjList[up][i]].coord.first == vplist[up].coord.first && vplist[adjList[up][i]].coord.second <= vplist[down].coord.second) {
                    adjList[adjList[up][i]].erase(remove(adjList[adjList[up][i]].begin(), adjList[adjList[up][i]].end(), up), adjList[adjList[up][i]].end());
                    adjList[up].erase(remove(adjList[up].begin(), adjList[up].end(), adjList[up][i]), adjList[up].end());
                    i--;
                }
            }
            for(int i = 0; i < adjList[down].size(); i++) {
                if(vplist[adjList[down][i]].coord.first == vplist[down].coord.first && vplist[adjList[down][i]].coord.second >= vplist[up].coord.second) {
                    adjList[adjList[down][i]].erase(remove(adjList[adjList[down][i]].begin(), adjList[adjList[down][i]].end(), down), adjList[adjList[down][i]].end());
                    adjList[down].erase(remove(adjList[down].begin(), adjList[down].end(), adjList[down][i]), adjList[down].end());
                    i--;
                }
            }
        }
    }
}

void OASG::OASG_R1(const Vertex& source, const int sourceobstacleid) {

    vector<int> intervals;
    vector<int> candidates;
    
    // Find candidate vertices
    int oi = 0;
    int pi = 0;
    bool olremain = true;
    bool orremain = true;
    bool premain = true;
    float sweepx = 0;
    while(olremain || premain || orremain) {

        // Find the next sweep line x-coordinate
        if(olremain && premain) {
            sweepx = min<float>(incOlist[oi].start.first, incPlist[pi].coord.first);
        }
        else if(olremain) {
            sweepx = incOlist[oi].start.first;
        }
        else if(premain) {
            sweepx = incPlist[pi].coord.first;
        }
        if(!intervals.empty()) {
            auto minR = min_element(intervals.begin(), intervals.end(), [&](const int a, const int b) {
                return obstacles[a].end.first < obstacles[b].end.first;
            });
            if( !olremain && !premain ) {
                sweepx = (float)obstacles[*minR].end.first;
            } else {
                sweepx = min<float>(sweepx, (float)obstacles[*minR].end.first);
            }
        }

        // Left boundaries of obstacles met: add blocking intervals
        vector<int> leftObstacles;
        for(auto ito = incOlist.begin() + oi; ito->start.first == sweepx && ito != incOlist.end(); ito++) {
            int id = incOlist[oi].obstacleid;
            if(id != sourceobstacleid) {
                leftObstacles.emplace_back(id);
                intervals.emplace_back(id);
            }
            oi++;
        }
        for(auto it = candidates.begin(); it != candidates.end();) {
            bool remove = false;
            for(const auto& inte : intervals) {
                // Check if any candidate is blocked by I: Iymin <= y <= IyMax || Iymin <= yc <= IyMax || yp <= Iymin <= y || yp <= IyMax <= y
                if((obstacles[inte].start.second <= source.coord.second && source.coord.second <= obstacles[inte].end.second) || 
                    (obstacles[inte].start.second <= vplist[*it].coord.second && vplist[*it].coord.second <= obstacles[inte].end.second) ||
                    (vplist[*it].coord.second <= obstacles[inte].start.second && obstacles[inte].start.second <= source.coord.second) ||
                    (vplist[*it].coord.second <= obstacles[inte].end.second && obstacles[inte].end.second <= source.coord.second)) {
                    it = candidates.erase(it);
                    remove = true;
                    break;
                }
            }
            if(!remove) {
                it++;
            }
        }

        // Right boundaries of obstacles met: remove blocking intervals
        vector<int> rightObstacles;
        for(auto iti = intervals.begin(); iti != intervals.end();) {
            if(obstacles[*iti].end.first == sweepx) {
                rightObstacles.emplace_back(*iti);
                iti = intervals.erase(iti);
            }
            else {
                iti++;
            }
        }

        // Pins met: add candidate pin vertices
        for(auto itp = incPlist.begin() + pi; itp->coord.first == sweepx && itp != incPlist.end(); itp++) {
            if(incPlist[pi].coord.first <= source.coord.first && incPlist[pi].coord.second <= source.coord.second) {
                bool add = true;
                for(const auto& inte : intervals) {
                    // Check if it's blocked by I: Iymin <= y <= IyMax || Iymin <= yp <= IyMax || yp <= Iymin <= y || yp <= IyMax <= y
                    if((obstacles[inte].start.second <= source.coord.second && source.coord.second <= obstacles[inte].end.second) ||
                        (obstacles[inte].start.second <= incPlist[pi].coord.second && incPlist[pi].coord.second <= obstacles[inte].end.second) ||
                        (incPlist[pi].coord.second <= obstacles[inte].start.second && obstacles[inte].start.second <= source.coord.second) ||
                        (incPlist[pi].coord.second <= obstacles[inte].end.second && obstacles[inte].end.second <= source.coord.second)) {
                        add = false;
                        break;
                    }
                }
                if(add && incPlist[pi].id != source.id) {
                    candidates.emplace_back(incPlist[pi].id);
                }
            }
            pi++;
        }
        
        // Add candidate vertices
        for(const auto& id : leftObstacles) {
            // c2 in R1: xmin <= x, yMax <= y
            if(obstacles[id].start.first <= source.coord.first && obstacles[id].end.second <= source.coord.second) {
                bool add = true;
                for(const auto& inte : intervals) {
                    // Check if it's blocked by I: Iymin <= y <= IyMax || Iymin <= yMax <= IyMax || yMax < Iymin < y || yMax < IyMax < y
                    if((obstacles[inte].start.second <= source.coord.second && source.coord.second <= obstacles[inte].end.second) ||
                        (obstacles[inte].start.second <= obstacles[id].end.second && obstacles[id].end.second <= obstacles[inte].end.second) ||
                        (obstacles[id].end.second < obstacles[inte].start.second && obstacles[inte].start.second < source.coord.second) ||
                        (obstacles[id].end.second < obstacles[inte].end.second && obstacles[inte].end.second < source.coord.second)) {
                        add = false;
                        break;
                    }
                }
                if(add) {
                    candidates.emplace_back(obstacles[id].c2);
                }
            }
        }
        for(const auto& id : rightObstacles) {
            // c3 in R1: xMax <= x, yMax <= y
            if(obstacles[id].end.first <= source.coord.first && obstacles[id].end.second <= source.coord.second) {
                bool add = true;
                for(const auto& inte : intervals) {
                    // Check if it's blocked by I: Iymin <= y <= IyMax || Iymin <= yMax <= IyMax || yMax <= Iymin <= y || yMax <= IyMax <= y
                    if((obstacles[inte].start.second <= source.coord.second && source.coord.second <= obstacles[inte].end.second) ||
                        (obstacles[inte].start.second <= obstacles[id].end.second && obstacles[id].end.second <= obstacles[inte].end.second) ||
                        (obstacles[id].end.second <= obstacles[inte].start.second && obstacles[inte].start.second <= source.coord.second) ||
                        (obstacles[id].end.second <= obstacles[inte].end.second && obstacles[inte].end.second <= source.coord.second)) {
                        add = false;
                        break;
                    }
                }
                if(add) {
                    candidates.emplace_back(obstacles[id].c3);
                }
            }
            // c4 in R1: xMax <= x, ymin <= y
            else if(obstacles[id].end.first <= source.coord.first && obstacles[id].start.second <= source.coord.second) {
                bool add = true;
                for(const auto& inte : intervals) {
                    // Check if it's blocked by I: Iymin <= y <= IyMax || Iymin <= ymin <= IyMax || ymin <= Iymin <= y || ymin <= IyMax <= y
                    if((obstacles[inte].start.second <= source.coord.second && source.coord.second <= obstacles[inte].end.second) ||
                        (obstacles[inte].start.second <= obstacles[id].start.second && obstacles[id].start.second <= obstacles[inte].end.second) ||
                        (obstacles[id].start.second <= obstacles[inte].start.second && obstacles[inte].start.second <= source.coord.second) ||
                        (obstacles[id].start.second <= obstacles[inte].end.second && obstacles[inte].end.second <= source.coord.second)) {
                        add = false;
                        break;
                    }
                }
                if(add) {
                    candidates.emplace_back(obstacles[id].c4);
                }
            }
        }

        if(oi == incOlist.size() ) {
            olremain = false;
        } else if (incOlist[oi].start.first >= source.coord.first) {
            olremain = false;
        }
        if(pi == incPlist.size()) {
            premain = false;
        } else if (incPlist[pi].coord.first > source.coord.first) {
            premain = false;
        }
        if(!olremain && !premain) {
            orremain = false;
            for(const auto& inte : intervals) {
                if(obstacles[inte].end.first <= source.coord.first) {
                    orremain = true;
                    break;
                }
            }
        }
    }

    // Add vertices of the same obstacle
    if(sourceobstacleid != -1) {
        if(source.id == obstacles[sourceobstacleid].c2 || source.id == obstacles[sourceobstacleid].c4) {
            candidates.emplace_back(obstacles[sourceobstacleid].c1);
        }
    }

    // Add edges to the OASG
    if(!candidates.empty()) {
        if(candidates[0] == source.id) {
            return;
        }
        for(auto itc = candidates.begin(); itc != candidates.end(); itc++) {
            adjList[source.id].emplace_back(*itc);
            adjList[*itc].emplace_back(source.id);
        }
    }
}

void OASG::OASG_R2(const Vertex& source, const int sourceobstacleid) {

    vector<int> intervals;
    vector<int> candidates;
    
    // Find candidate vertices
    int oi = 0;
    int pi = 0;
    bool olremain = true;
    bool orremain = true;
    bool premain = true;
    float sweepx = 0;
    while(olremain || premain || orremain) {

        // Find the next sweep line x-coordinate
        if(olremain && premain) {
            sweepx = min<float>(incOlist[oi].start.first, incPlist[pi].coord.first);
        }
        else if(olremain) {
            sweepx = incOlist[oi].start.first;
        }
        else if(premain) {
            sweepx = incPlist[pi].coord.first;
        }
        if(!intervals.empty()) {
            auto minR = min_element(intervals.begin(), intervals.end(), [&](const int a, const int b) {
                return obstacles[a].end.first < obstacles[b].end.first;
            });
            if(!olremain && !premain) {
                sweepx = (float)obstacles[*minR].end.first;
            } else {
                sweepx = min<float>(sweepx, (float)obstacles[*minR].end.first);
            }
        }

        // Left boundaries of obstacles met: add blocking intervals
        vector<int> leftObstacles;
        for(auto ito = incOlist.begin() + oi; ito->start.first == sweepx && ito != incOlist.end(); ito++) {
            int id = incOlist[oi].obstacleid;
            if(id != sourceobstacleid) {
                leftObstacles.emplace_back(id);
                intervals.emplace_back(id);
            }
            oi++;
        }
        for(auto it = candidates.begin(); it != candidates.end();) {
            bool remove = false;
            for(const auto& inte : intervals) {
                // Check if any candidate is blocked by I: Iymin <= y <= IyMax || Iymin <= yc <= IyMax || y <= Iymin <= yc || y <= IyMax <= yc
                if((obstacles[inte].start.second <= source.coord.second && source.coord.second <= obstacles[inte].end.second) ||
                    (obstacles[inte].start.second <= vplist[*it].coord.second && vplist[*it].coord.second <= obstacles[inte].end.second) ||
                    (source.coord.second <= obstacles[inte].start.second && obstacles[inte].start.second <= vplist[*it].coord.second) ||
                    (source.coord.second <= obstacles[inte].end.second && obstacles[inte].end.second <= vplist[*it].coord.second)) {
                    it = candidates.erase(it);
                    remove = true;
                    break;
                }
            }
            if(!remove) {
                it++;
            }
        }

        // Right boundaries of obstacles met: remove blocking intervals
        vector<int> rightObstacles;
        for(auto iti = intervals.begin(); iti != intervals.end();) {
            if(obstacles[*iti].end.first == sweepx) {
                rightObstacles.emplace_back(*iti);
                iti = intervals.erase(iti);
            }
            else {
                iti++;
            }
        }

        // Pins met: add candidate pin vertices
        for(auto itp = incPlist.begin() + pi; itp->coord.first == sweepx && itp != incPlist.end(); itp++) {
            if(incPlist[pi].coord.first <= source.coord.first && incPlist[pi].coord.second >= source.coord.second) {
                bool add = true;
                for(const auto& inte : intervals) {
                    // Check if it's blocked by I: Iymin <= y <= IyMax || Iymin <= yp <= IyMax || y <= Iymin <= yp || y <= IyMax <= yp
                    if((obstacles[inte].start.second <= source.coord.second && source.coord.second <= obstacles[inte].end.second) ||
                        (obstacles[inte].start.second <= incPlist[pi].coord.second && incPlist[pi].coord.second <= obstacles[inte].end.second) ||
                        (source.coord.second <= obstacles[inte].start.second && obstacles[inte].start.second <= incPlist[pi].coord.second) ||
                        (source.coord.second <= obstacles[inte].end.second && obstacles[inte].end.second <= incPlist[pi].coord.second)) {
                        add = false;
                        break;
                    }
                }
                if(add && incPlist[pi].id != source.id) {
                    candidates.emplace_back(incPlist[pi].id);
                }
            }
            pi++;
        }
        
        // Add candidate vertices
        for(const auto& id : leftObstacles) {
            // c1 in R2: xmin <= x, ymin >= y
            if(obstacles[id].start.first <= source.coord.first && obstacles[id].start.second >= source.coord.second) {
                bool add = true;
                for(const auto& inte : intervals) {
                    // Check if it's blocked by I: Iymin <= y <= IyMax || Iymin <= ymin <= IyMax || y < Iymin < ymin || y < IyMax < ymin
                    if((obstacles[inte].start.second <= source.coord.second && source.coord.second <= obstacles[inte].end.second) ||
                        (obstacles[inte].start.second <= obstacles[id].start.second && obstacles[id].start.second <= obstacles[inte].end.second) ||
                        (source.coord.second < obstacles[inte].start.second && obstacles[inte].start.second < obstacles[id].start.second) ||
                        (source.coord.second < obstacles[inte].end.second && obstacles[inte].end.second < obstacles[id].start.second)) {
                        add = false;
                        break;
                    }
                }
                if(add) {
                    candidates.emplace_back(obstacles[id].c1);
                }
            }
        }
        for(const auto& id : rightObstacles) {
            // c4 in R2: xMax <= x, ymin >= y
            if(obstacles[id].end.first <= source.coord.first && obstacles[id].start.second >= source.coord.second) {
                bool add = true;
                for(const auto& inte : intervals) {
                    // Check if it's blocked by I: Iymin <= y <= IyMax || Iymin <= ymin <= IyMax || y <= Iymin <= ymin || y <= IyMax <= ymin
                    if((obstacles[inte].start.second <= source.coord.second && source.coord.second <= obstacles[inte].end.second) || 
                        (obstacles[inte].start.second <= obstacles[id].start.second && obstacles[id].start.second <= obstacles[inte].end.second) ||
                        (source.coord.second <= obstacles[inte].start.second && obstacles[inte].start.second <= obstacles[id].start.second) ||
                        (source.coord.second <= obstacles[inte].end.second && obstacles[inte].end.second <= obstacles[id].start.second)) {
                        add = false;
                        break;
                    }
                }
                if(add) {
                    candidates.emplace_back(obstacles[id].c4);
                }
            }
            // c3 in R2: xMax <= x, yMax >= y
            else if(obstacles[id].end.first <= source.coord.first && obstacles[id].end.second >= source.coord.second) {
                bool add = true;
                for(const auto& inte : intervals) {
                    // Check if it's blocked by I: Iymin <= y <= IyMax || Iymin <= yMax <= IyMax || y <= Iymin <= yMax || y <= IyMax <= yMax
                    if((obstacles[inte].start.second <= source.coord.second && source.coord.second <= obstacles[inte].end.second) ||
                        (obstacles[inte].start.second <= obstacles[id].end.second && obstacles[id].end.second <= obstacles[inte].end.second) ||
                        (source.coord.second <= obstacles[inte].start.second && obstacles[inte].start.second <= obstacles[id].end.second) ||
                        (source.coord.second <= obstacles[inte].end.second && obstacles[inte].end.second <= obstacles[id].end.second)) {
                        add = false;
                        break;
                    }
                }
                if(add) {
                    candidates.emplace_back(obstacles[id].c3);
                }
            }
        }

        if(oi == incOlist.size() ) {
            olremain = false;
        } else if (incOlist[oi].start.first >= source.coord.first) {
            olremain = false;
        }
        if(pi == incPlist.size()) {
            premain = false;
        } else if (incPlist[pi].coord.first > source.coord.first) {
            premain = false;
        }
        if(!olremain && !premain) {
            orremain = false;
            for(const auto& inte : intervals) {
                if(obstacles[inte].end.first <= source.coord.first) {
                    orremain = true;
                    break;
                }
            }
        }
    }

    // Add vertices of the same obstacle
    if(sourceobstacleid != -1) {
        if(source.id == obstacles[sourceobstacleid].c1 || source.id == obstacles[sourceobstacleid].c3) {
            candidates.emplace_back(obstacles[sourceobstacleid].c2);
        }
    }

    // Add edges to the OASG
    if(!candidates.empty()) {
        if(candidates[0] == source.id) {
            return;
        }
        for(auto itc = candidates.begin(); itc != candidates.end(); itc++) {
            adjList[source.id].emplace_back(*itc);
            adjList[*itc].emplace_back(source.id);
        }
    }
}

void OASG::OASG_R3(const Vertex& source, const int sourceobstacleid) {

    vector<int> intervals;
    vector<int> candidates;
    
    // Find candidate vertices
    int oi = 0;
    int pi = 0;
    bool orremain = true;
    bool olremain = true;
    bool premain = true;
    float sweepx = 0;
    while(orremain || premain || olremain) {

        // Find the next sweep line x-coordinate
        if(orremain && premain) {
            sweepx = max<float>(decOlist[oi].end.first, decPlist[pi].coord.first);
        }
        else if(orremain) {
            sweepx = decOlist[oi].end.first;
        }
        else if(premain) {
            sweepx = decPlist[pi].coord.first;
        }
        if(!intervals.empty()) {
            auto maxR = max_element(intervals.begin(), intervals.end(), [&](const int a, const int b) {
                return obstacles[a].start.first < obstacles[b].start.first;
            });
            if( !orremain && !premain ) {
                sweepx = (float)obstacles[*maxR].start.first;
            } else {
                sweepx = max<float>(sweepx, (float)obstacles[*maxR].start.first);
            }
        }
        
        // Right boundaries of obstacles met: add blocking intervals
        vector<int> rightObstacles;
        for(auto ito = decOlist.begin() + oi; ito->end.first == sweepx && ito != decOlist.end(); ito++) {
            int id = decOlist[oi].obstacleid;
            if(id != sourceobstacleid) {
                rightObstacles.emplace_back(id);
                intervals.emplace_back(id);
            }
            oi++;
        }
        for(auto it = candidates.begin(); it != candidates.end();) {
            bool remove = false;
            for(const auto& inte : intervals) {
                // Check if any candidate is blocked by I: Iymin <= y <= IyMax || Iymin <= yc <= IyMax || y <= Iymin <= yc || y <= IyMax <= yc
                if((obstacles[inte].start.second <= source.coord.second && source.coord.second <= obstacles[inte].end.second) ||
                    (obstacles[inte].start.second <= vplist[*it].coord.second && vplist[*it].coord.second <= obstacles[inte].end.second) ||
                    (source.coord.second <= obstacles[inte].start.second && obstacles[inte].start.second <= vplist[*it].coord.second) ||
                    (source.coord.second <= obstacles[inte].end.second && obstacles[inte].end.second <= vplist[*it].coord.second)) {
                    it = candidates.erase(it);
                    remove = true;
                    break;
                }
            }
            if(!remove) {
                it++;
            }
        }
        
        // Left boundaries of obstacles met: remove blocking intervals
        vector<int> leftObstacles;
        for(auto iti = intervals.begin(); iti != intervals.end();) {
            if(obstacles[*iti].start.first == sweepx) {
                leftObstacles.emplace_back(*iti);
                iti = intervals.erase(iti);
            }
            else {
                iti++;
            }
        }

        // Pins met: add candidate pin vertices
        for(auto itp = decPlist.begin() + pi; itp->coord.first == sweepx && itp != decPlist.end(); itp++) {
            if(decPlist[pi].coord.first >= source.coord.first && decPlist[pi].coord.second >= source.coord.second) {
                bool add = true;
                for(const auto& inte : intervals) {
                    // Check if it's blocked by I: Iymin <= y <= IyMax || Iymin <= yp <= IyMax || y <= Iymin <= yp || y <= IyMax <= yp
                    if((obstacles[inte].start.second <= source.coord.second && source.coord.second <= obstacles[inte].end.second) ||
                        (obstacles[inte].start.second <= decPlist[pi].coord.second && decPlist[pi].coord.second <= obstacles[inte].end.second) ||
                        (source.coord.second <= obstacles[inte].start.second && obstacles[inte].start.second <= decPlist[pi].coord.second) ||
                        (source.coord.second <= obstacles[inte].end.second && obstacles[inte].end.second <= decPlist[pi].coord.second)) {
                        add = false;
                        break;
                    }
                }
                if(add && decPlist[pi].id != source.id) {
                    candidates.emplace_back(decPlist[pi].id);
                }
            }
            pi++;
        }
        
        // Add candidate vertices
        for(const auto& id : rightObstacles) {
            // c4 in R3: xMax >= x, ymin >= y
            if(obstacles[id].end.first >= source.coord.first && obstacles[id].start.second >= source.coord.second) {
                bool add = true;
                for(const auto& inte : intervals) {
                    // Check if it's blocked by I: Iymin <= y <= IyMax || Iymin <= ymin <= IyMax || y < Iymin < ymin || y < IyMax < ymin
                    if((obstacles[inte].start.second <= source.coord.second && source.coord.second <= obstacles[inte].end.second) ||
                        (obstacles[inte].start.second <= obstacles[id].start.second && obstacles[id].start.second <= obstacles[inte].end.second) ||
                        (source.coord.second < obstacles[inte].start.second && obstacles[inte].start.second < obstacles[id].start.second) ||
                        (source.coord.second < obstacles[inte].end.second && obstacles[inte].end.second < obstacles[id].start.second)) {
                        add = false;
                        break;
                    }
                }
                if(add) {
                    candidates.emplace_back(obstacles[id].c4);
                }
            }
        }
        for(const auto& id : leftObstacles) {
            // c1 in R3: xmin >= x, ymin >= y
            if(obstacles[id].start.first >= source.coord.first && obstacles[id].start.second >= source.coord.second) {
                bool add = true;
                for(const auto& inte : intervals) {
                    // Check if it's blocked by I: Iymin <= y <= IyMax || Iymin <= ymin <= IyMax || y <= Iymin <= ymin || y <= IyMax <= ymin
                    if((obstacles[inte].start.second <= source.coord.second && source.coord.second <= obstacles[inte].end.second) || 
                        (obstacles[inte].start.second <= obstacles[id].start.second && obstacles[id].start.second <= obstacles[inte].end.second) ||
                        (source.coord.second <= obstacles[inte].start.second && obstacles[inte].start.second <= obstacles[id].start.second) ||
                        (source.coord.second <= obstacles[inte].end.second && obstacles[inte].end.second <= obstacles[id].start.second)) {
                        add = false;
                        break;
                    }
                }
                if(add) {
                    candidates.emplace_back(obstacles[id].c1);
                }
            }
            // c2 in R3: xmin >= x, yMax >= y
            else if(obstacles[id].start.first >= source.coord.first && obstacles[id].end.second >= source.coord.second) {
                bool add = true;
                for(const auto& inte : intervals) {
                    // Check if it's blocked by I: !(Iymin <= y <= IyMax || Iymin <= yMax <= IyMax || y <= Iymin <= yMax || y <= IyMax <= yMax
                    if((obstacles[inte].start.second <= source.coord.second && source.coord.second <= obstacles[inte].end.second) ||
                        (obstacles[inte].start.second <= obstacles[id].end.second && obstacles[id].end.second <= obstacles[inte].end.second) ||
                        (source.coord.second <= obstacles[inte].start.second && obstacles[inte].start.second <= obstacles[id].end.second) ||
                        (source.coord.second <= obstacles[inte].end.second && obstacles[inte].end.second <= obstacles[id].end.second)) {
                        add = false;
                        break;
                    }
                }
                if(add) {
                    candidates.emplace_back(obstacles[id].c2);
                }
            }
        }

        if(oi == decOlist.size() ) {
            orremain = false;
        } else if (decOlist[oi].end.first <= source.coord.first) {
            orremain = false;
        }
        if(pi == decPlist.size()) {
            premain = false;
        } else if (decPlist[pi].coord.first < source.coord.first) {
            premain = false;
        }
        if(!orremain && !premain) {
            olremain = false;
            for(const auto& inte : intervals) {
                if(obstacles[inte].start.first >= source.coord.first) {
                    olremain = true;
                    break;
                }
            }
        }
    }

    // Add vertices of the same obstacle
    if(sourceobstacleid != -1) {
        if(source.id == obstacles[sourceobstacleid].c2 || source.id == obstacles[sourceobstacleid].c4) {
            candidates.emplace_back(obstacles[sourceobstacleid].c3);
        }
    }
    
    // Add edges to the OASG
    if(!candidates.empty()) {
        if(candidates[0] == source.id) {
            return;
        }
        for(auto itc = candidates.begin(); itc != candidates.end(); itc++) {
            adjList[source.id].emplace_back(*itc);
            adjList[*itc].emplace_back(source.id);
        }
    }
}

void OASG::OASG_R4(const Vertex& source, const int sourceobstacleid) {
    
    vector<int> intervals;
    vector<int> candidates;
    
    // Find candidate vertices
    int oi = 0;
    int pi = 0;
    bool orremain = true;
    bool olremain = true;
    bool premain = true;
    float sweepx = 0;
    while(orremain || premain || olremain) {

        // Find the next sweep line x-coordinate
        if(orremain && premain) {
            sweepx = max<float>(decOlist[oi].end.first, decPlist[pi].coord.first);
        }
        else if(orremain) {
            sweepx = decOlist[oi].end.first;
        }
        else if(premain) {
            sweepx = decPlist[pi].coord.first;
        }
        if(!intervals.empty()) {
            auto maxR = max_element(intervals.begin(), intervals.end(), [&](const int a, const int b) {
                return obstacles[a].start.first < obstacles[b].start.first;
            });
            if(!orremain && !premain) {
                sweepx = (float)obstacles[*maxR].start.first;
            } else {
                sweepx = max<float>(sweepx, (float)obstacles[*maxR].start.first);
            }
        }

        // Right boundaries of obstacles met: add blocking intervals
        vector<int> rightObstacles;
        for(auto ito = decOlist.begin() + oi; ito->end.first == sweepx && ito != decOlist.end(); ito++) {
            int id = decOlist[oi].obstacleid;
            if(id != sourceobstacleid) {
                rightObstacles.emplace_back(id);
                intervals.emplace_back(id);
            }
            oi++;
        }
        for(auto it = candidates.begin(); it != candidates.end();) {
            bool remove = false;
            for(const auto& inte : intervals) {
                // Check if any candidate is blocked by I: Iymin <= y <= IyMax || Iymin <= yc <= IyMax || yc <= Iymin <= y || yc <= IyMax <= y
                if((obstacles[inte].start.second <= source.coord.second && source.coord.second <= obstacles[inte].end.second) ||
                    (obstacles[inte].start.second <= vplist[*it].coord.second && vplist[*it].coord.second <= obstacles[inte].end.second) ||
                    (vplist[*it].coord.second <= obstacles[inte].start.second && obstacles[inte].start.second <= source.coord.second) ||
                    (vplist[*it].coord.second <= obstacles[inte].end.second && obstacles[inte].end.second <= source.coord.second)) {
                    it = candidates.erase(it);
                    remove = true;
                    break;
                }
            }
            if(!remove) {
                it++;
            }
        }

        // Left boundaries of obstacles met: remove blocking intervals
        vector<int> leftObstacles;
        for(auto iti = intervals.begin(); iti != intervals.end();) {
            if(obstacles[*iti].start.first == sweepx) {
                leftObstacles.emplace_back(*iti);
                iti = intervals.erase(iti);
            }
            else {
                iti++;
            }
        }

        // Pins met: add candidate pin vertices
        for(auto itp = decPlist.begin() + pi; itp->coord.first == sweepx && itp != decPlist.end(); itp++) {
            if(decPlist[pi].coord.first >= source.coord.first && decPlist[pi].coord.second <= source.coord.second) {
                bool add = true;
                for(const auto& inte : intervals) {
                    // Check if it's blocked by I: Iymin <= y <= IyMax || Iymin <= yp <= IyMax || yp <= Iymin <= y || yp <= IyMax <= y
                    if((obstacles[inte].start.second <= source.coord.second && source.coord.second <= obstacles[inte].end.second) ||
                        (obstacles[inte].start.second <= decPlist[pi].coord.second && decPlist[pi].coord.second <= obstacles[inte].end.second) ||
                        (decPlist[pi].coord.second <= obstacles[inte].start.second && obstacles[inte].start.second <= source.coord.second) ||
                        (decPlist[pi].coord.second <= obstacles[inte].end.second && obstacles[inte].end.second <= source.coord.second)) {
                        add = false;
                        break;
                    }
                }
                if(add && decPlist[pi].id != source.id) {
                    candidates.emplace_back(decPlist[pi].id);
                }
            }
            pi++;
        }
        
        // Add candidate vertices
        for(const auto& id : rightObstacles) {
            // c3 in R4: xMax >= x, yMax <= y
            if(obstacles[id].end.first >= source.coord.first && obstacles[id].end.second <= source.coord.second) {
                bool add = true;
                for(const auto& inte : intervals) {
                    // Check if it's blocked by I: Iymin <= y <= IyMax || Iymin <= yMax <= IyMax || yMax < Iymin < y || yMax < IyMax < y
                    if((obstacles[inte].start.second <= source.coord.second && source.coord.second <= obstacles[inte].end.second) || 
                        (obstacles[inte].start.second <= obstacles[id].end.second && obstacles[id].end.second <= obstacles[inte].end.second) ||
                        (obstacles[id].end.second < obstacles[inte].start.second && obstacles[inte].start.second < source.coord.second) ||
                        (obstacles[id].end.second < obstacles[inte].end.second && obstacles[inte].end.second < source.coord.second)) {
                        add = false;
                        break;
                    }
                }
                if(add) {
                    candidates.emplace_back(obstacles[id].c3);
                }
            }
        }
        for(const auto& id : leftObstacles) {
            // c2 in R4: xmin >= x, yMax <= y
            if(obstacles[id].start.first >= source.coord.first && obstacles[id].end.second <= source.coord.second) {
                bool add = true;
                for(const auto& inte : intervals) {
                    // Check if it's blocked by I: Iymin <= y <= IyMax || Iymin <= yMax <= IyMax || yMax <= Iymin <= y || yMax <= IyMax <= y
                    if((obstacles[inte].start.second <= source.coord.second && source.coord.second <= obstacles[inte].end.second) ||
                        (obstacles[inte].start.second <= obstacles[id].end.second && obstacles[id].end.second <= obstacles[inte].end.second) ||
                        (obstacles[id].end.second <= obstacles[inte].start.second && obstacles[inte].start.second <= source.coord.second) ||
                        (obstacles[id].end.second <= obstacles[inte].end.second && obstacles[inte].end.second <= source.coord.second)) {
                        add = false;
                        break;
                    }
                }
                if(add) {
                    candidates.emplace_back(obstacles[id].c2);
                }
            }
            // c1 in R4: xmin >= x, ymin <= y
            else if(obstacles[id].start.first >= source.coord.first && obstacles[id].start.second <= source.coord.second) {
                bool add = true;
                for(const auto& inte : intervals) {
                    // Check if it's blocked by I: Iymin <= y <= IyMax || Iymin <= ymin <= IyMax || ymin <= Iymin <= y || ymin <= IyMax <= y
                    if((obstacles[inte].start.second <= source.coord.second && source.coord.second <= obstacles[inte].end.second) ||
                        (obstacles[inte].start.second <= obstacles[id].start.second && obstacles[id].start.second <= obstacles[inte].end.second) ||
                        (obstacles[id].start.second <= obstacles[inte].start.second && obstacles[inte].start.second <= source.coord.second) ||
                        (obstacles[id].start.second <= obstacles[inte].end.second && obstacles[inte].end.second <= source.coord.second)) {
                        add = false;
                        break;
                    }
                }
                if(add) {
                    candidates.emplace_back(obstacles[id].c1);
                }
            }
        }

        if(oi == decOlist.size() ) {
            orremain = false;
        } else if (decOlist[oi].end.first <= source.coord.first) {
            orremain = false;
        }
        if(pi == decPlist.size()) {
            premain = false;
        } else if (decPlist[pi].coord.first < source.coord.first) {
            premain = false;
        }
        if(!orremain && !premain) {
            olremain = false;
            for(const auto& inte : intervals) {
                if(obstacles[inte].start.first >= source.coord.first) {
                    olremain = true;
                    break;
                }
            }
        }
    }

    // Add vertices of the same obstacle
    if(sourceobstacleid != -1) {
        if(source.id == obstacles[sourceobstacleid].c1 || source.id == obstacles[sourceobstacleid].c3) {
            candidates.emplace_back(obstacles[sourceobstacleid].c4);
        }
    }

    // Add edges to the OASG
    if(!candidates.empty()) {
        if(candidates[0] == source.id) {
            return;
        }
        for(auto itc = candidates.begin(); itc != candidates.end(); itc++) {
            adjList[source.id].emplace_back(*itc);
            adjList[*itc].emplace_back(source.id);
        }
    }
}