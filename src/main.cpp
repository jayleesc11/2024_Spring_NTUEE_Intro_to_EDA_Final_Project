#include <iostream>
#include <vector>
#include <map>
#include <fstream>

#include "block.h"
#include "net.h"
#include "defParser.h"
#include "jsonParser.h"
#include "output.h"
#include "input.h"
#include "netadd.h"
#include "congestion.h"
#include "oasg.h"
#include "OAST.h"
#include "OARST.h"
// #include "OASTold.h"
// #include "OARSTold.h"
#include "oarsmt.h"
#include "constant.h"

using namespace std;

pair<vector<pair<int, int>>, pair<pair<vector<pair<int, int>>,vector<Obstacles>>, vector<Vertex>>> inputPartition(vector<string>& del_blk, unordered_map<string, vector<Component>>& mapComp, unordered_map<string, vector<pair<int, int>>> blks, const string& fname, vector<pair<int, int>>& edges, DefParser defParser){
    for (int i = 0; i < del_blk.size(); i++){
        mapComp.erase(del_blk[i]);
    }
    // for (auto& comp:mapComp) {
    //     cout << comp.first <<endl;
    // }
    unordered_map<string, vector<Component>> positioned_components = defParser.move(mapComp, blks);
    Input input;

    pair<pair<vector<pair<int, int>>,vector<Obstacles>>, vector<Vertex>> partition_results = input.breakBlocks(positioned_components);
    edges = input.outputEdges();
    return  make_pair(edges, partition_results);
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        cerr << "Usage: ./CGR <tracks/um> <caseOO.def> <caseOO.cfg.json> <caseOO.connection_matrix.json>" << endl;
        return 1;
    }

    // Parse the json files
    JsonParser jsonParser;
    unordered_map<string, Block> blocks;
    vector<Net> nets;
    jsonParser.parseCfgJson(argv[3], blocks);
    jsonParser.parseCmJson(argv[4], nets);

    string caseNumstring = string(argv[4]).substr(string(argv[4]).find("case") + 4, 1);
    Output output(caseNumstring, nets.size());

    // parse and print the .def file
    string fullFile = string(argv[2]) + "/chip_top.def";
    DefParser defParser(fullFile);
    Chip chip_info = defParser.parseChipInfo();
    vector<Component> comp = defParser.parseComponents();
    unordered_map<string, vector<Component>> compMap = defParser.mapComponents(comp);

    vector<Region> reg = defParser.parseRegions();
    unordered_map<string,vector<Region>> regMap = defParser.mapRegions(reg);

    Netadd netadd;
    nets = netadd.modify(nets, compMap, regMap);

    unordered_map<string, vector<pair<int, int>>> blk = defParser.parseBlocks(comp);
    unordered_map<string, vector<Component>> positioned_components = defParser.move(compMap, blk);
    // defParser.printDefFile(positioned_components, blk);

    // Build the congestion map
    Congestion congestion(stoi(argv[1]), chip_info.units, {chip_info.dieArea_pair.second.first, chip_info.dieArea_pair.second.second}, nets);

    vector<string> delBlk;
    for(const auto &block : blocks) {
        if(block.second.is_feedthroughable) {
            delBlk.push_back(block.first);
        }
    }

    int loop = 1;
    while (loop <= 1 + RRR_ITER) {

        vector<int> routingNets;
        if(loop == 1) {
            // First iteration, route all nets
            routingNets.resize(nets.size());
            iota(routingNets.begin(), routingNets.end(), 0);
        } else {
            // RRR iterations
            routingNets = congestion.reRouteNets(nets, RRR_THRESHOLD, RRR_NUM);
        }

        for(const auto& netid : routingNets) {
            Net net = nets[netid];
            // Remove TX and RX
            if(net.tx.find("REGION") == string::npos) {
                delBlk.push_back(net.tx);
            }
            for (int j = 0; j < net.rx.size(); j++){
                if(net.rx[j].find("REGION") == string::npos) {
                    delBlk.push_back(net.rx[j]);
                }
            }

            // Partitioning
            Input input;
            vector<pair<int, int>> edges;
            pair< vector<pair<int, int>>,pair<pair<vector<pair<int, int>>,vector<Obstacles>>, vector<Vertex>>> partition_results = inputPartition(delBlk, compMap, blk, argv[2], edges, defParser);
            // input.printObstacles(partition_results.second.first.second);
            // input.printEdges(partition_results.first);
            // input.printEdges(partition_results.second.first.first);
            // input.printVertex(partition_results.second.second);

            // Preparation for OASGs
            vector<Vertex> addv;
            vector<OASG> oasgs;
            int verticesid;

            // OASG Step0: No vertices added
            int numObVertices = partition_results.second.first.second.size();
            oasgs.push_back(OASG(numObVertices));
            oasgs[oasgs.size() - 1].buildOASG(partition_results.second.first.second, partition_results.second.second, partition_results.second.first.first, net, addv);
            verticesid = oasgs[oasgs.size() - 1].vplist.size();

            // OASG Step1 (Optional)
            for(int i = congestion.cellWidth * 45.5 ; i <= congestion.cellWidth * 80.5; i += congestion.cellWidth) {
                addv.emplace_back(verticesid++, make_pair(i, congestion.cellWidth * 140 - i), false);
                addv.emplace_back(verticesid++, make_pair(i, congestion.cellWidth * 15 + i), false);
            }
            for(int i = congestion.cellWidth * 92 ; i <= congestion.cellWidth * 113; i += congestion.cellWidth) {
                addv.emplace_back(verticesid++, make_pair(i, congestion.cellWidth * 188 - i), false);
                addv.emplace_back(verticesid++, make_pair(i, - congestion.cellWidth * 17 + i), false);
            }
            oasgs.push_back(OASG(numObVertices));
            oasgs[oasgs.size() - 1].buildOASG(partition_results.second.first.second, oasgs[oasgs.size() - 2].vplist, partition_results.second.first.first, net, addv);
            addv.clear();

            // OASG Step2 (Optional)
            // for(int i = congestion.cellWidth * 45.5 ; i <= congestion.cellWidth * 80.5; i += congestion.cellWidth) {
            //     // addv.emplace_back(verticesid++, make_pair(i, congestion.cellWidth * 140 - i), false);
            //     addv.emplace_back(verticesid++, make_pair(i, congestion.cellWidth * 15 + i), false);
            // }
            // oasgs.push_back(OASG(numObVertices));
            // oasgs[oasgs.size() - 1].buildOASG(partition_results.second.first.second, oasgs[oasgs.size() - 2].vplist, partition_results.second.first.first, net, addv);
            // addv.clear();

            // OASG Step3 (Optional)
            // for(int i = congestion.cellWidth * 92 ; i <= congestion.cellWidth * 113; i += congestion.cellWidth) {
            //     addv.emplace_back(verticesid++, make_pair(i, congestion.cellWidth * 188 - i), false);
            //     // addv.emplace_back(verticesid++, make_pair(i, - congestion.cellWidth * 17 + i), false);
            // }
            // oasgs.push_back(OASG(numObVertices));
            // oasgs[oasgs.size() - 1].buildOASG(partition_results.second.first.second, oasgs[oasgs.size() - 2].vplist, partition_results.second.first.first, net, addv);
            // addv.clear();

            // OASG Step4 (Optional)
            // for(int i = congestion.cellWidth * 92 ; i <= congestion.cellWidth * 113; i += congestion.cellWidth) {
            //     // addv.emplace_back(verticesid++, make_pair(i, congestion.cellWidth * 188 - i), false);
            //     addv.emplace_back(verticesid++, make_pair(i, - congestion.cellWidth * 17 + i), false);
            // }
            // oasgs.push_back(OASG(numObVertices));
            // oasgs[oasgs.size() - 1].buildOASG(partition_results.second.first.second, oasgs[oasgs.size() - 2].vplist, partition_results.second.first.first, net, addv);
            // addv.clear();

            // Merge the OASGs
            int oasgN = oasgs.size() - 1;
            for(int i = 0; i < oasgN; i++) {
                for(int j = 0; j < oasgs[i].adjList.size(); j++) {
                    oasgs[oasgN].adjList[j].insert(oasgs[oasgN].adjList[j].end(), oasgs[i].adjList[j].begin(), oasgs[i].adjList[j].end());
                    // Delete duplicates
                    sort(oasgs[oasgN].adjList[j].begin(), oasgs[oasgN].adjList[j].end());
                    oasgs[oasgN].adjList[j].erase(unique(oasgs[oasgN].adjList[j].begin(), oasgs[oasgN].adjList[j].end()), oasgs[oasgN].adjList[j].end());
                }
            }

            // oasgs[oasgN].printOASG();

            // Here are database of choices of X:
            // for(int i = congestion.cellWidth * 45.5 ; i <= congestion.cellWidth * 80.5; i += congestion.cellWidth) {
            //     addv.emplace_back(verticesid++, make_pair(i, congestion.cellWidth * 140 - i), false);
            //     addv.emplace_back(verticesid++, make_pair(i, congestion.cellWidth * 15 + i), false);
            // }
            // for(int i = congestion.cellWidth * 45.5 ; i <= congestion.cellWidth * 80.5; i += 2*congestion.cellWidth) {
            //     addv.emplace_back(verticesid++, make_pair(i, congestion.cellWidth * 140 - i), false);
            //     addv.emplace_back(verticesid++, make_pair(i + congestion.cellWidth, congestion.cellWidth * 15.5 + i), false);
            // }
            // for(int i = congestion.cellWidth * 92 ; i <= congestion.cellWidth * 113; i += congestion.cellWidth) {
            //     addv.emplace_back(verticesid++, make_pair(i, congestion.cellWidth * 188 - i), false);
            //     addv.emplace_back(verticesid++, make_pair(i, - congestion.cellWidth * 17 + i), false);
            // }
            // for(int i = congestion.cellWidth * 92 ; i <= congestion.cellWidth * 113; i += congestion.cellWidth) {
            //     addv.emplace_back(verticesid++, make_pair(i, congestion.cellWidth * 188 - i), false);
            //     addv.emplace_back(verticesid++, make_pair(i, - congestion.cellWidth * 17.5 + i), false);
            // }


            // OAST
            vector<vector<int>> adj_list = oasgs[oasgN].adjList;
            vector<Vertex> v = oasgs[oasgN].vplist;
            adj_list = OAST(v,adj_list, THRESHOLD * congestion.capacity * congestion.cellWidth, ALPHA, congestion.congestionMap, congestion.cellWidth, OVERFLOW_TYPE);
            // printOAST(v,adj_list);

            // OARST
            adj_list = OARST(v,adj_list, congestion.congestionMap, congestion.cellWidth, USE_HEURISTIC, THRESHOLD * congestion.capacity * congestion.cellWidth);
            // printOARST(v,adj_list);

            // OARSMT
            OARSMT oarsmt(v, adj_list);
            oarsmt.buildOARSMT();
            // oarsmt.printOARSMT();

            // Update the congestion map and output netEdges
            congestion.updateCongestion(net, oarsmt.vertices, oarsmt.adjList);
            output.updateNetEdges(net, oarsmt.vertices, oarsmt.adjList);
            
            // Put TX and RX back
            delBlk.erase(remove(delBlk.begin(), delBlk.end(), net.tx), delBlk.end());
            for (int j = 0; j < net.rx.size(); j++){
                delBlk.erase(remove(delBlk.begin(), delBlk.end(), net.rx[j]), delBlk.end());
            }
        }        
        loop++;
        int newOverflow = congestion.printOverflow();
        if(newOverflow < get<0>(congestion.bestCost)) {
            output.bestNetEdges = output.netEdges;
        }
    }

    congestion.printCost();
    // congestion.writeMap("map.txt");
    output.writeOutput();

    return 0;
}