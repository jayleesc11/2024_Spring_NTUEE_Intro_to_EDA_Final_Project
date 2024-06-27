#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include "nlohmann/json.hpp"
#include "block.h"
#include "net.h"
#include "jsonParser.h"


using namespace std;
using json = nlohmann::json;

void JsonParser::parseCfgJson(const string& cfgFile, unordered_map<string, Block>& blocks) {
    // cfg.json file
    ifstream cfgis(cfgFile);
    if (!cfgis.is_open()) {
        cerr << "Error opening file: " << cfgFile << endl;
        return;
    }

    json cfgjson;
    cfgis >> cfgjson;

    // Iterate over the array of blocks, create Block objects and store them
    for (const auto& block_json : cfgjson) {
        string blockname = block_json["block_name"].get<string>();
        int through_block_net_num = block_json["through_block_net_num"].get<int>();

        vector<ThroughEdgeNet> through_block_edge_net_num;
        for (const auto& edge_net : block_json["through_block_edge_net_num"]) {
            pair<float, float> start = { edge_net[0][0].get<float>(), edge_net[0][1].get<float>() };
            pair<float, float> end = { edge_net[1][0].get<float>(), edge_net[1][1].get<float>() };
            int net_num = edge_net[2].get<int>();
            through_block_edge_net_num.emplace_back(start, end, net_num);
        }

        vector<BlockPortRegion> block_port_region;
        for (const auto& port_reg : block_json["block_port_region"]) {
            pair<float, float> start = { port_reg[0][0].get<float>(), port_reg[0][1].get<float>() };
            pair<float, float> end = { port_reg[1][0].get<float>(), port_reg[1][1].get<float>() };
            block_port_region.emplace_back(start, end);
        }

        bool is_feedthroughable = block_json["is_feedthroughable"].get<string>() == "True";
        bool is_tile = block_json["is_tile"].get<string>() == "True";

        blocks.insert({blockname, Block(blockname, through_block_net_num, 0, through_block_edge_net_num, block_port_region, is_feedthroughable, is_tile)});
    }

    cfgis.close();
}

void JsonParser::parseCmJson(const string& cmFile, vector<Net>& nets) {
    // connection_matrix.json file
    ifstream cmis(cmFile);
    if (!cmis.is_open()) {
        cerr << "Error opening file: " << cmFile << endl;
        return;
    }

    json cmjson;
    cmis >> cmjson;

    // Iterate over the array of nets, create Net objects and store them
    for (const auto& net_json : cmjson) {
        int id = net_json["ID"].get<int>();
        string tx = net_json["TX"].get<string>();
        vector<string> rx = net_json["RX"].get<vector<string>>();
        int numTracks = net_json["NUM"].get<int>();

        // Lambda function to create MustThroughRegion objects
        auto createMustThroughRegions = [](const json& mustThroughJson) {
            vector<MustThroughRegion> mustThrough;
            for (const auto& region : mustThroughJson) {
                string regionName = region[0].get<string>();
                pair<float, float> startPoint1 = { region[1][0].get<float>(), region[1][1].get<float>() };
                pair<float, float> endPoint1 = { region[1][2].get<float>(), region[1][3].get<float>() };
                pair<float, float> startPoint2 = { region[2][0].get<float>(), region[2][1].get<float>() };
                pair<float, float> endPoint2 = { region[2][2].get<float>(), region[2][3].get<float>() };
                mustThrough.emplace_back(regionName, startPoint1, endPoint1, startPoint2, endPoint2);
            }
            return mustThrough;
        };

        vector<MustThroughRegion> mustThrough = createMustThroughRegions(net_json["MUST_THROUGH"]);
        vector<MustThroughRegion> hmftMustThrough = createMustThroughRegions(net_json["HMFT_MUST_THROUGH"]);

        pair<float, float> txCoord = { net_json["TX_COORD"][0].get<int>(), net_json["TX_COORD"][1].get<int>() };

        vector<pair<float, float>> rxCoord;
        for (const auto& coord : net_json["RX_COORD"]) {
            rxCoord.push_back({ coord[0].get<float>(), coord[1].get<float>() });
        }

        nets.emplace_back(id, tx, rx, numTracks, mustThrough, hmftMustThrough, txCoord, rxCoord);
    }

    cmis.close();
}