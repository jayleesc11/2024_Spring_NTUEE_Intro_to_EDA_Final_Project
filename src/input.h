#ifndef INPUT_H
#define INPUT_H

#include <iostream>

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>

#include <fstream>
#include <string>
#include <sstream>


#include <map>
#include "defParser.h"

struct Vertex {
    int id;                     // Index in the vector<Vertex> vertices
    std::pair<float, float> coord;   // Position
    bool pin = false;           // False if it is an obstacle vertex

    Vertex(int id, std::pair<float, float> c, bool p)
        : id(id), coord(c), pin(p) {}
};

struct Obstacles {          // Rectanugular obstacles
    int obstacleid;         // Index in the vector<Obstacles> obstacles
    std::pair<int,int> start;    // Lower left corner
    std::pair<int,int> end;      // Upper right corner
    int c1;                 // Id of lower left corner vertex
    int c2;                 // Id of upper left corner vertex
    int c3;                 // Id of upper right corner vertex
    int c4;                 // Id of lower right corner vertex

    Obstacles(int id, std::pair<int,int> s, std::pair<int,int> e, int c1, int c2, int c3, int c4)
        : obstacleid(id), start(s), end(e), c1(c1), c2(c2), c3(c3), c4(c4) {}
};

class Input{
private:
    std::vector<Vertex> vertices;
    std::vector<Vertex> obs_vertices;
    
    std::vector<std::pair<int, int>> output_edges;
    std::vector<std::pair<int, int>> cut_edges;
    
    std::vector<Obstacles> obstacles;
    std::vector<std::pair<int, int>> obs_edges;
public:
    Input() {};
    std::pair<std::pair<std::vector<std::pair<int, int>>,std::vector<Obstacles>>, std::vector<Vertex>> breakBlocks(std::unordered_map<std::string, std::vector<Component>>& compMap);

    void printEdges(std::vector<std::pair<int, int>>& cut_edges);
    void printObstacles(std::vector<Obstacles>& obstacles);
    void printVertex(std::vector<Vertex>& vertices);
    std::vector<std::pair<int, int>> outputEdges();
    

};





#endif