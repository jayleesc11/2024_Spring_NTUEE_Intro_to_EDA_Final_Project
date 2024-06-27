#ifndef BLOCK_H
#define BLOCK_H

#include <string>
#include <vector>

using namespace std;

struct ThroughEdgeNet {
    pair<float, float> start;
    pair<float, float> end;
    int net_num;

    ThroughEdgeNet(const pair<float, float>& s, const pair<float, float>& e, int num) : start(s), end(e), net_num(num) {}
};

struct BlockPortRegion {
    pair<float, float> start;
    pair<float, float> end;

    BlockPortRegion(const pair<float, float>& s, const pair<float, float>& e) : start(s), end(e) {}
};

class Block {
public:
    string name;
    int through_block_net_num;
    int current_through;
    vector<ThroughEdgeNet> through_block_edge_net_num;
    vector<BlockPortRegion> block_port_region;
    bool is_feedthroughable;
    bool is_tile;

    Block(
        string blockname,
        int net_num, 
        int current,
        const vector<ThroughEdgeNet>& edge_net_num,
        const vector<BlockPortRegion>& port_region,
        bool feedthroughable,
        bool tile
    ) : name(blockname),
        through_block_net_num(net_num),
        current_through(0),
        through_block_edge_net_num(edge_net_num),
        block_port_region(port_region),
        is_feedthroughable(feedthroughable),
        is_tile(tile) {}

    // Methods
    void print() const;
};

#endif