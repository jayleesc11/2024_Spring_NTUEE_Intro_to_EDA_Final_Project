#include <stack>
#include <iomanip>
#include "output.h"

using namespace std;

Output::Output(const string& caseNum, const int numNets) {
    outputFileName = "outputs/case" + caseNum + "_net.rpt";
    netEdges.resize(numNets);
}

void Output::updateNetEdges(const Net& net, const vector<Vertex>& vertices, const vector<vector<int>>& adjList) {
    
    netEdges[net.id].clear();

    int txid;
    for(int i = 0; i < vertices.size(); i++) {
        if(vertices[i].coord == net.txCoord && vertices[i].pin) {
            txid = i;
            break;
        }
    }

    vector<bool> visited(vertices.size(), false);
    stack<int> s;
    s.push(txid);
    visited[txid] = true;

    // DFS
    while (!s.empty()) {
        int v = s.top();
        s.pop();

        for (const auto& u : adjList[v]) {
            if (!visited[u]) {
                visited[u] = true;
                netEdges[net.id].push_back(OutputEdge(vertices[v].coord, vertices[u].coord));
                s.push(u);
            }
        }
    }
}

void Output::writeOutput() {
    ofstream fout(outputFileName);
    if (!fout) {
        cerr << "Error opening file for writing." << endl;
        exit(1);
    }

    for(int i = 0; i < bestNetEdges.size(); i++) {
        fout << "[" << i << "]" << endl;
        for(const auto& edge : bestNetEdges[i]) {
            // Don't use scientific notation
            fout << fixed << setprecision(0);
            fout << "(" << edge.start.first << ", " << edge.start.second << "),(" << edge.end.first << ", " << edge.end.second << ")" << endl;
        }
    }

    fout.close();
}
    