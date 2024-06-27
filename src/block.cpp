#include <iostream>
#include "block.h"

using namespace std;

void Block::print() const {
    cout << "Block Name: " << name << endl;
    cout << "Through Block Net Num: " << through_block_net_num << endl;
    cout << "Through Block Edge Net Num: ";
    for (const auto& edge_net_num : through_block_edge_net_num) {
        cout << "Start: (" << edge_net_num.start.first << ", " << edge_net_num.start.second << "), End: (" << edge_net_num.end.first << ", " << edge_net_num.end.second << "), Net Num: " << edge_net_num.net_num << endl;
    }
    cout << "Block Port Region: ";
    for (const auto& block_port_region : block_port_region) {
        cout << "Start: (" << block_port_region.start.first << ", " << block_port_region.start.second << "), End: (" << block_port_region.end.first << ", " << block_port_region.end.second << ")" << endl;
    }
    cout << "Is Feedthroughable: " << (is_feedthroughable ? "true" : "false") << endl;

};
