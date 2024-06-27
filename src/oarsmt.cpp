#include <algorithm>
#include <set>
#include <iostream>
#include "input.h"
#include "oarsmt.h"

using namespace std;

OARSMT::OARSMT(const vector<Vertex>& ve, vector<vector<int>>& adj) : vertices(ve) {
    // Remove duplicate vertices
    for(int i = 0; i < vertices.size(); ++i) {
        for(int j = i + 1; j < vertices.size(); ++j) {
            if(vertices[i].coord == vertices[j].coord) {
                if(!vertices[i].pin && vertices[j].pin) {
                    for(int k = 0; k < adj[i].size(); ++k) {
                        adj[j].emplace_back(adj[i][k]);
                        adj[adj[i][k]].erase(remove(adj[adj[i][k]].begin(), adj[adj[i][k]].end(), i), adj[adj[i][k]].end());
                        adj[adj[i][k]].emplace_back(j);
                    }
                    adj[i].clear();
                    break;
                } else {
                    for(int k = 0; k < adj[j].size(); ++k) {
                        adj[i].emplace_back(adj[j][k]);
                        adj[adj[j][k]].erase(remove(adj[adj[j][k]].begin(), adj[adj[j][k]].end(), j), adj[adj[j][k]].end());
                        adj[adj[j][k]].emplace_back(i);
                    }
                    adj[j].clear();
                    if(vertices[i].pin && vertices[j].pin) {
                        adj[i].emplace_back(j);
                        adj[j].emplace_back(i);
                    }
                }
            }
        }
    }

    // Remove self loops
    for(int i = 0; i < vertices.size(); ++i) {
        for(int j = 0; j < adj[i].size(); ++j) {
            if(adj[i][j] == i) {
                adj[i].erase(adj[i].begin() + j);
                j--;
            }
        }
    }

    // Remove duplicate edges
    for(int i = 0; i < vertices.size(); ++i) {
        sort(adj[i].begin(), adj[i].end());
        adj[i].erase(unique(adj[i].begin(), adj[i].end()), adj[i].end());
    }

    // Generate 2 undirected graphs
    hadjList.resize(adj.size());
    vadjList.resize(adj.size());
    for(int u = 0; u < adj.size(); ++u) {
        for(int v = 0; v < adj[u].size(); ++v) {
            if(vertices[adj[u][v]].coord == vertices[u].coord) {
                hadjList[u].push_back(adj[u][v]);
                vadjList[u].push_back(adj[u][v]);
            } else if(vertices[adj[u][v]].coord.first == vertices[u].coord.first) {
                vadjList[u].push_back(adj[u][v]);
            } else {
                hadjList[u].push_back(adj[u][v]);
            }
            
        }
    }
}

void OARSMT::printOARSMT() {
    // for(int i = 0; i < vertices.size(); ++i) {
    //     if(adjList[i].size() > 0) {
    //         cout << "Vertex " << i << " (" << vertices[i].coord.first << "," << vertices[i].coord.second << ") -> ";
    //         for(const auto& adj : adjList[i]) {
    //             if(vertices[i].coord.first == vertices[adj].coord.first || vertices[i].coord.second == vertices[adj].coord.second) {
    //                 cout << adj << "(" << vertices[adj].coord.first << "," << vertices[adj].coord.second << "),";
    //             }
    //             else {
    //                 cout << "Error " << adj << "(" << vertices[adj].coord.first << "," << vertices[adj].coord.second << "),";
    //             }
    //         }
    //         cout << endl;
    //     }
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
    for(int i = 0; i < vertices.size(); i++) {
        cout << fixed;
        cout.precision(0);
        cout << i << ": (" << vertices[i].coord.first << "," << vertices[i].coord.second << "), ";
    }
    cout << endl;
}

void OARSMT::buildOARSMT() {

    removeEdges();
    removeVertices();
    u_shapedRefinement();
    
    // Build adjacency list by combining hadjList and vadjList
    adjList.resize(vertices.size());
    for(int i = 0; i < vertices.size(); ++i) {
        for(int j = 0; j < hadjList[i].size(); ++j) {
            adjList[i].emplace_back(hadjList[i][j]);
        }
        for(int j = 0; j < vadjList[i].size(); ++j) {
            adjList[i].emplace_back(vadjList[i][j]);
        }
    }

    // Remove duplicate edges
    for(int i = 0; i < vertices.size(); ++i) {
        sort(adjList[i].begin(), adjList[i].end());
        adjList[i].erase(unique(adjList[i].begin(), adjList[i].end()), adjList[i].end());
    }
}

void OARSMT::removeEdges() { 
    for(int h1 = 0; h1 < hadjList.size(); ++h1) {
        bool hdeleted;
        for(int hj = 0; hj < hadjList[h1].size(); ++hj) {
            hdeleted = false;
            int h2 = hadjList[h1][hj];
            // designate h1x < h2x (h1y = h2y already) to avoid repeat due to undirected graph
            if(vertices[h1].coord.first < vertices[h2].coord.first) {
                for(int v1 = 0; v1 < vadjList.size(); ++v1) {
                    if(h1 == v1 || h2 == v1 || vertices[h1].coord == vertices[v1].coord || vertices[h2].coord == vertices[v1].coord) {
                        continue;
                    }
                    for(int vj = 0; vj < vadjList[v1].size(); ++vj) {
                        int v2 = vadjList[v1][vj];
                        if(h1 == v2 || h2 == v2 || vertices[h1].coord == vertices[v2].coord || vertices[h2].coord == vertices[v2].coord) {
                            continue;
                        }
                        // designate v1y < v2y (v1x = v2x already) to avoid repeat due to undirected graph
                        if(vertices[v1].coord.second < vertices[v2].coord.second) {
                            // Case1: h1x < v1x < h2x, v1y < h1y < v2y
                            if((vertices[h1].coord.first < vertices[v1].coord.first && vertices[v1].coord.first < vertices[h2].coord.first) && 
                            (vertices[v1].coord.second < vertices[h1].coord.second && vertices[h1].coord.second < vertices[v2].coord.second)) {
                                int addv = vertices.size();
                                for(int i = 0; i < vertices.size(); i++) {
                                    if(vertices[i].coord.first == vertices[v1].coord.first && vertices[i].coord.second == vertices[h1].coord.second) {
                                        addv = i;
                                        break;
                                    }
                                }
                                hadjList[h1].emplace_back(addv);
                                hadjList[h2].emplace_back(addv);
                                vadjList[v1].emplace_back(addv);
                                vadjList[v2].emplace_back(addv);
                                if(addv == vertices.size()) {
                                    // Add edges
                                    hadjList.emplace_back(vector<int>{h1,h2});
                                    vadjList.emplace_back(vector<int>{v1,v2});
                                    // Add vertex
                                    vertices.emplace_back(vertices.size(), pair<float, float>(vertices[v1].coord.first, vertices[h1].coord.second), false);
                                } else {
                                    // Add edges
                                    hadjList[addv].emplace_back(h1);
                                    hadjList[addv].emplace_back(h2);
                                    vadjList[addv].emplace_back(v1);
                                    vadjList[addv].emplace_back(v2);
                                }
                                // Remove edge (v1, v2)
                                vadjList[v2].erase(remove(vadjList[v2].begin(), vadjList[v2].end(), v1), vadjList[v2].end());
                                vadjList[v1].erase(vadjList[v1].begin() + vj);
                                vj--;
                                // Remove edge (h1, h2)
                                hadjList[h2].erase(remove(hadjList[h2].begin(), hadjList[h2].end(), h1), hadjList[h2].end());
                                hadjList[h1].erase(hadjList[h1].begin() + hj);
                                hdeleted = true;
                            }
                            // Case2-1: h1x < v1x < h2x, v1y < h1y, v2y = h1y
                            else if((vertices[h1].coord.first < vertices[v1].coord.first && vertices[v1].coord.first < vertices[h2].coord.first) &&
                            (vertices[v1].coord.second < vertices[h1].coord.second) && (vertices[v2].coord.second == vertices[h1].coord.second)) {
                                // Add edges
                                hadjList[v2].emplace_back(h1);
                                hadjList[v2].emplace_back(h2);
                                hadjList[h1].emplace_back(v2);
                                hadjList[h2].emplace_back(v2);
                                // Remove edge (h1, h2)
                                hadjList[h2].erase(remove(hadjList[h2].begin(), hadjList[h2].end(), h1), hadjList[h2].end());
                                hadjList[h1].erase(hadjList[h1].begin() + hj);
                                hdeleted = true;
                            }
                            // Case2-2: h1x < v1x < h2x, v1y = h1y, h1y < v2y
                            else if((vertices[h1].coord.first < vertices[v1].coord.first && vertices[v1].coord.first < vertices[h2].coord.first) &&
                            (vertices[v1].coord.second == vertices[h1].coord.second) && (vertices[h1].coord.second < vertices[v2].coord.second)) {
                                // Add edges
                                hadjList[v1].emplace_back(h1);
                                hadjList[v1].emplace_back(h2);
                                hadjList[h1].emplace_back(v1);
                                hadjList[h2].emplace_back(v1);
                                // Remove edge (h1, h2)
                                hadjList[h2].erase(remove(hadjList[h2].begin(), hadjList[h2].end(), h1), hadjList[h2].end());
                                hadjList[h1].erase(hadjList[h1].begin() + hj);
                                hdeleted = true;
                            }
                            // Case2-3: h1x < v1x, h2x = v1x, v1y < h1y < v2y
                            else if((vertices[h1].coord.first < vertices[v1].coord.first) && (vertices[h2].coord.first == vertices[v1].coord.first) &&
                            (vertices[v1].coord.second < vertices[h1].coord.second && vertices[h1].coord.second < vertices[v2].coord.second)) {
                                // Add edges
                                vadjList[h2].emplace_back(v1);
                                vadjList[h2].emplace_back(v2);
                                vadjList[v1].emplace_back(h2);
                                vadjList[v2].emplace_back(h2);
                                // Remove edge (v1, v2)
                                vadjList[v2].erase(remove(vadjList[v2].begin(), vadjList[v2].end(), v1), vadjList[v2].end());
                                vadjList[v1].erase(vadjList[v1].begin() + vj);
                                vj--;
                            }
                            // Case2-4: h1x = v1x, v1x < h2x, v1y < h1y < v2y
                            else if((vertices[h1].coord.first == vertices[v1].coord.first) && (vertices[v1].coord.first < vertices[h2].coord.first) &&
                            (vertices[v1].coord.second < vertices[h1].coord.second && vertices[h1].coord.second < vertices[v2].coord.second)) {
                                // Add edges
                                vadjList[h1].emplace_back(v1);
                                vadjList[h1].emplace_back(v2);
                                vadjList[v1].emplace_back(h1);
                                vadjList[v2].emplace_back(h1);
                                // Remove edge (v1, v2)
                                vadjList[v2].erase(remove(vadjList[v2].begin(), vadjList[v2].end(), v1), vadjList[v2].end());
                                vadjList[v1].erase(vadjList[v1].begin() + vj);
                                vj--;
                            }
                        }
                        if(hdeleted) {
                            break;
                        }
                    }
                    if(hdeleted) {
                        break;
                    }
                }
            }
            if(hdeleted) {
                hj--;
            }
        }
    }

    bool operation = false;
    do {
        operation = false;

        // Case3: Horizontal
        for(int h1 = 0; h1 < hadjList.size(); ++h1) {
            // Sort and delete duplicate
            sort(hadjList[h1].begin(), hadjList[h1].end(), [&](int a, int b) {
                return vertices[a].coord.first < vertices[b].coord.first;
            });
            auto last = unique(hadjList[h1].begin(), hadjList[h1].end());
            hadjList[h1].erase(last, hadjList[h1].end());

            if(hadjList[h1].size() >= 2) {
                if(vertices[hadjList[h1][1]].coord.first < vertices[h1].coord.first && vertices[hadjList[h1][0]].coord.first != vertices[hadjList[h1][1]].coord.first) {
                    operation = true;
                    int lastlinked = hadjList[h1][0];
                    for(int hj = 1; vertices[hadjList[h1][hj]].coord.first < vertices[h1].coord.first && hj < hadjList[h1].size(); ) {
                        int h2 = hadjList[h1][hj];
                        // Add edge
                        hadjList[h2].emplace_back(lastlinked);
                        hadjList[lastlinked].emplace_back(h2);
                        // Remove edge (h1, h2)
                        lastlinked = h2;
                        hadjList[h2].erase(remove(hadjList[h2].begin(), hadjList[h2].end(), h1), hadjList[h2].end());
                        hadjList[h1].erase(hadjList[h1].begin() + hj);
                    }
                }
                int offset = 0;
                if(vertices[hadjList[h1][0]].coord.first < vertices[h1].coord.first && vertices[hadjList[h1][0]].coord.first != vertices[hadjList[h1][1]].coord.first) {
                    offset = 1;
                }
                if(offset + 1 < hadjList[h1].size() && vertices[hadjList[h1][offset]].coord.first != vertices[h1].coord.first && vertices[hadjList[h1][offset]].coord.first != vertices[hadjList[h1][offset + 1]].coord.first) {
                    operation = true;
                    int lastlinked = hadjList[h1][offset];
                    for(int hj = offset + 1; hj < hadjList[h1].size(); ) {
                        int h2 = hadjList[h1][hj];
                        // Add edge
                        hadjList[h2].emplace_back(lastlinked);
                        hadjList[lastlinked].emplace_back(h2);
                        // Remove edge (h1, h2)
                        lastlinked = h2;
                        hadjList[h2].erase(remove(hadjList[h2].begin(), hadjList[h2].end(), h1), hadjList[h2].end());
                        hadjList[h1].erase(hadjList[h1].begin() + hj);
                    }
                }
            }
        }

        // Case3: Vertical
        for(int v1 = 0; v1 < vadjList.size(); ++v1) {
            // Sort and delete duplicate
            sort(vadjList[v1].begin(), vadjList[v1].end(), [&](int a, int b) {
                return vertices[a].coord.second < vertices[b].coord.second;
            });
            auto last = unique(vadjList[v1].begin(), vadjList[v1].end());
            vadjList[v1].erase(last, vadjList[v1].end());
            
            if(vadjList[v1].size() >= 2) {
                if(vertices[vadjList[v1][1]].coord.second < vertices[v1].coord.second && vertices[vadjList[v1][0]].coord.second != vertices[vadjList[v1][1]].coord.second) {
                    operation = true;
                    int lastlinked = vadjList[v1][0];
                    for(int vj = 1; vertices[vadjList[v1][vj]].coord.second < vertices[v1].coord.second && vj < vadjList[v1].size(); ) {
                        int v2 = vadjList[v1][vj];
                        // Add edge
                        vadjList[v2].emplace_back(lastlinked);
                        vadjList[lastlinked].emplace_back(v2);
                        // Remove edge (v1, v2)
                        lastlinked = v2;
                        vadjList[v2].erase(remove(vadjList[v2].begin(), vadjList[v2].end(), v1), vadjList[v2].end());
                        vadjList[v1].erase(vadjList[v1].begin() + vj);
                    }
                }
                
                int offset = 0;
                if(vertices[vadjList[v1][0]].coord.second < vertices[v1].coord.second && vertices[vadjList[v1][0]].coord.second != vertices[vadjList[v1][1]].coord.second) {
                    offset = 1;
                }
                if(offset + 1 < vadjList[v1].size() && vertices[vadjList[v1][offset]].coord.second != vertices[v1].coord.second && vertices[vadjList[v1][offset]].coord.second != vertices[vadjList[v1][offset + 1]].coord.second) {
                    for(int vj = offset + 1; vj < vadjList[v1].size(); ) {
                        operation = true;
                        int lastlinked = vadjList[v1][offset];
                        int v2 = vadjList[v1][vj];
                        // Add edge
                        vadjList[v2].emplace_back(lastlinked);
                        vadjList[lastlinked].emplace_back(v2);
                        // Remove edge (v1, v2)
                        lastlinked = v2;
                        vadjList[v2].erase(remove(vadjList[v2].begin(), vadjList[v2].end(), v1), vadjList[v2].end());
                        vadjList[v1].erase(vadjList[v1].begin() + vj);
                    }
                }
            }
        }

        // Case4,5: Horizontal
        // designate ux < vx (uy = vy already) to avoid repeat due to undirected graph
        auto hcomp = [&](pair<int, int> a, pair<int, int> b) { return vertices[a.first].coord.first < vertices[b.first].coord.first; };
        set<pair<int, int>, decltype(hcomp)> hedges(hcomp);
        for(int u = 0; u < hadjList.size(); ++u) {
            for(int v = 0; v < hadjList[u].size(); ++v) {
                if(vertices[u].coord.first < vertices[hadjList[u][v]].coord.first)
                    hedges.insert({u, hadjList[u][v]});
            }
        }
        
        // Check from left to right
        if(!hedges.empty()) {
            bool change;
            auto min = hedges.begin();
            int count = 0;
            do {
                change = false;
                for(auto next = std::next(min); next != hedges.end(); ++next) {
                    // next = std::next(min);
                    if(vertices[(*next).first].coord.second == vertices[(*min).second].coord.second) {
                        // Case4: next1x < min2x < next2x
                        if(vertices[(*next).first].coord.first < vertices[(*min).second].coord.first && 
                        vertices[(*min).second].coord.first < vertices[(*next).second].coord.first) {
                            pair<int,int> minEdge = *min;
                            pair<int,int> nextEdge = *next;
                            // Remove edge
                            hadjList[nextEdge.first].erase(remove(hadjList[nextEdge.first].begin(), hadjList[nextEdge.first].end(), nextEdge.second), hadjList[nextEdge.first].end());
                            hadjList[nextEdge.second].erase(remove(hadjList[nextEdge.second].begin(), hadjList[nextEdge.second].end(), nextEdge.first), hadjList[nextEdge.second].end());
                            hadjList[minEdge.first].erase(remove(hadjList[minEdge.first].begin(), hadjList[minEdge.first].end(), minEdge.second), hadjList[minEdge.first].end());
                            hadjList[minEdge.second].erase(remove(hadjList[minEdge.second].begin(), hadjList[minEdge.second].end(), minEdge.first), hadjList[minEdge.second].end());
                            // Add edge
                            hadjList[minEdge.first].emplace_back(nextEdge.first);
                            hadjList[nextEdge.first].emplace_back(minEdge.first);
                            hadjList[minEdge.second].emplace_back(nextEdge.first);
                            hadjList[nextEdge.first].emplace_back(minEdge.second);
                            hadjList[minEdge.second].emplace_back(nextEdge.second);
                            hadjList[nextEdge.second].emplace_back(minEdge.second);
                            hedges.insert({minEdge.first, nextEdge.first});
                            hedges.insert({nextEdge.first, minEdge.second});
                            hedges.insert({minEdge.second, nextEdge.second});
                            // Remove edge from set
                            hedges.erase(minEdge);
                            hedges.erase(nextEdge);
                            minEdge = {nextEdge.first, minEdge.second};
                            min = find(hedges.begin(), hedges.end(), minEdge);
                            change = true;
                            break;
                        }
                        // Case5: (i1x <) i2x < j2x
                        if( vertices[(*next).second].coord.first < vertices[(*min).second].coord.first) {
                            pair<int,int> minEdge = *min;
                            pair<int,int> nextEdge = *next;
                            // Remove edge
                            hadjList[minEdge.first].erase(remove(hadjList[minEdge.first].begin(), hadjList[minEdge.first].end(), minEdge.second), hadjList[minEdge.first].end());
                            hadjList[minEdge.second].erase(remove(hadjList[minEdge.second].begin(), hadjList[minEdge.second].end(), minEdge.first), hadjList[minEdge.second].end());
                            // Add edge
                            hadjList[minEdge.first].emplace_back(nextEdge.first);
                            hadjList[nextEdge.first].emplace_back(minEdge.first);
                            hadjList[nextEdge.second].emplace_back(minEdge.second);
                            hadjList[minEdge.second].emplace_back(nextEdge.second);
                            hedges.insert({minEdge.first, nextEdge.first});
                            hedges.insert({nextEdge.second, minEdge.second});
                            // Remove edge from set
                            hedges.erase(minEdge);
                            min = find(hedges.begin(), hedges.end(), nextEdge);
                            change = true;
                            break;
                        }
                    }
                }
                count++;
            } while(change);

            if(count > 1) {
                operation = true;
            }
        }

        // Case4,5: Vertical
        // designate uy < vy (ux = vx already) to avoid repeat due to undirected graph
        auto vcomp = [&](pair<int, int> a, pair<int, int> b) { return vertices[a.first].coord.second < vertices[b.first].coord.second; };
        set<pair<int, int>, decltype(vcomp)> vedges(vcomp);
        for(int u = 0; u < vadjList.size(); ++u) {
            for(int v = 0; v < vadjList[u].size(); ++v) {
                if(vertices[u].coord.second < vertices[vadjList[u][v]].coord.second)
                    vedges.insert({u, vadjList[u][v]});
            }
        }
        // Check from bottom to top
        if(!vedges.empty()) {
            bool change;
            auto min = vedges.begin();
            int count = 0;
            do {
                change = false;
                for(auto next = std::next(min); next != vedges.end(); ++next) {
                    if(vertices[(*next).first].coord.first == vertices[(*min).second].coord.first) {
                        // Case4: next1y < min2y < next2y
                        if(vertices[(*next).first].coord.second < vertices[(*min).second].coord.second && 
                        vertices[(*min).second].coord.second < vertices[(*next).second].coord.second) {
                            pair<int,int> minEdge = *min;
                            pair<int,int> nextEdge = *next;
                            // Remove edge
                            vadjList[nextEdge.first].erase(remove(vadjList[nextEdge.first].begin(), vadjList[nextEdge.first].end(), nextEdge.second), vadjList[nextEdge.first].end());
                            vadjList[nextEdge.second].erase(remove(vadjList[nextEdge.second].begin(), vadjList[nextEdge.second].end(), nextEdge.first), vadjList[nextEdge.second].end());
                            vadjList[minEdge.first].erase(remove(vadjList[minEdge.first].begin(), vadjList[minEdge.first].end(), minEdge.second), vadjList[minEdge.first].end());
                            vadjList[minEdge.second].erase(remove(vadjList[minEdge.second].begin(), vadjList[minEdge.second].end(), minEdge.first), vadjList[minEdge.second].end());
                            // Add edge
                            vadjList[minEdge.first].emplace_back(nextEdge.first);
                            vadjList[nextEdge.first].emplace_back(minEdge.first);
                            vadjList[minEdge.second].emplace_back(nextEdge.first);
                            vadjList[nextEdge.first].emplace_back(minEdge.second);
                            vadjList[minEdge.second].emplace_back(nextEdge.second);
                            vadjList[nextEdge.second].emplace_back(minEdge.second);
                            vedges.insert({minEdge.first, nextEdge.first});
                            vedges.insert({nextEdge.first, minEdge.second});
                            vedges.insert({minEdge.second, nextEdge.second});
                            // Remove edge from set
                            vedges.erase(minEdge);
                            vedges.erase(nextEdge);
                            minEdge = {minEdge.first, nextEdge.first};
                            min = find(vedges.begin(), vedges.end(), minEdge);
                            change = true;
                            break;
                        }
                        // Case5: (i1y <) i2y < j2y
                        if(vertices[(*next).second].coord.second < vertices[(*min).second].coord.second) {
                            pair<int,int> minEdge = *min;
                            pair<int,int> nextEdge = *next;
                            // Remove edge
                            vadjList[minEdge.first].erase(remove(vadjList[minEdge.first].begin(), vadjList[minEdge.first].end(), minEdge.second), vadjList[minEdge.first].end());
                            vadjList[minEdge.second].erase(remove(vadjList[minEdge.second].begin(), vadjList[minEdge.second].end(), minEdge.first), vadjList[minEdge.second].end());
                            // Add edge
                            vadjList[minEdge.first].emplace_back(nextEdge.first);
                            vadjList[nextEdge.first].emplace_back(minEdge.first);
                            vadjList[nextEdge.second].emplace_back(minEdge.second);
                            vadjList[minEdge.second].emplace_back(nextEdge.second);
                            vedges.insert({minEdge.first, nextEdge.first});
                            vedges.insert({nextEdge.second, minEdge.second});
                            // Remove edge from set
                            vedges.erase(minEdge);
                            vedges.erase(nextEdge);
                            minEdge = {minEdge.first, nextEdge.first};
                            min = find(vedges.begin(), vedges.end(), minEdge);
                            change = true;
                            break;
                        }
                    }
                }
                count++;
            } while(change);

            if(count > 1) {
                operation = true;
            }
        }
    } while(operation);
}

void OARSMT::removeVertices() {
    for(int i = 0; i < vertices.size(); ++i) {
        if(vertices[i].pin) {
            continue;
        }
        // Remove horizontal redundant vertices
        if( hadjList[i].size() == 2 && vadjList[i].size() == 0 ) {
            hadjList[hadjList[i][0]].erase(remove(hadjList[hadjList[i][0]].begin(), hadjList[hadjList[i][0]].end(), i), hadjList[hadjList[i][0]].end());
            hadjList[hadjList[i][1]].erase(remove(hadjList[hadjList[i][1]].begin(), hadjList[hadjList[i][1]].end(), i), hadjList[hadjList[i][1]].end());
            hadjList[hadjList[i][0]].emplace_back(hadjList[i][1]);
            hadjList[hadjList[i][1]].emplace_back(hadjList[i][0]);
            hadjList[i].clear();
        } // Remove vertical redundant vertices
        else if( vadjList[i].size() == 2 && hadjList[i].size() == 0 ) {
            vadjList[vadjList[i][0]].erase(remove(vadjList[vadjList[i][0]].begin(), vadjList[vadjList[i][0]].end(), i), vadjList[vadjList[i][0]].end());
            vadjList[vadjList[i][1]].erase(remove(vadjList[vadjList[i][1]].begin(), vadjList[vadjList[i][1]].end(), i), vadjList[vadjList[i][1]].end());
            vadjList[vadjList[i][0]].emplace_back(vadjList[i][1]);
            vadjList[vadjList[i][1]].emplace_back(vadjList[i][0]);
            vadjList[i].clear();
        }
    }
}

void OARSMT::u_shapedRefinement() {}