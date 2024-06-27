/*
Instruction: given a vector<Obstacles> obstacles, vector<Vertex> vertices, vector<pair<int,int>> removeEdges, and a Net net, build the OASG by:
    OASG oasg;
    oasg.buildOASG(obstacles, vertices, removeEdges, net);
Then you can get the output by:
    vector<Vertex> vplist = oasg.vplist;
    vector<vector<int>> adjList = oasg.adjList;
*/

#ifndef OASG_H
#define OASG_H

#include <vector>
#include "block.h"
#include "net.h"
#include "input.h"

using namespace std;

class OASG {
public:
    // Outputs
    vector<Vertex> vplist;
    vector<vector<int>> adjList;

    OASG(const int numOV) : numObVertices(numOV) {}
    
    // Methods
    void buildOASG(const vector<Obstacles>&, const vector<Vertex>&, const vector<pair<int,int>>&, const Net&, vector<Vertex>&);
    // obstacles: rectangular obstacles already cut
    // vertices: vertices of obstacles, duplicates removal beforehand needed
    // removeEdges: edges (vertexid, vertexid) to be removed from the OASG (the cuts)
    void printOASG() const;

private:
    // Inputs
    int numObVertices;
    vector<Obstacles> obstacles;
    // Left to right
    vector<Obstacles> incOlist;
    vector<Vertex> incPlist;
    // Right to left
    vector<Obstacles> decOlist;
    vector<Vertex> decPlist;

    // Methods
    void OASG_R1(const Vertex& v, const int sourceobstacleid);
    void OASG_R2(const Vertex& v, const int sourceobstacleid);
    void OASG_R3(const Vertex& v, const int sourceobstacleid);
    void OASG_R4(const Vertex& v, const int sourceobstacleid);
};

#endif