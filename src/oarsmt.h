#ifndef OARSMT_H
#define OARSMT_H

#include <vector>
#include "input.h"

using namespace std;

class OARSMT {
public:
    vector<Vertex> vertices;
    vector<vector<int>> adjList;
    vector<vector<int>> hadjList;
    vector<vector<int>> vadjList;

    OARSMT(const vector<Vertex>& ve, vector<vector<int>>& adj);

    // Methods
    void buildOARSMT();
    void printOARSMT();

private:
    // Methods
    void removeEdges();
    void removeVertices();
    void u_shapedRefinement();
};

#endif