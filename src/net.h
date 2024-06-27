#ifndef NET_H
#define NET_H

#include <string>
#include <vector>

using namespace std;

// Region that a net must pass through
struct MustThroughRegion {
    string regionName;
    pair<float, float> startPoint1;
    pair<float, float> endPoint1;
    pair<float, float> startPoint2;
    pair<float, float> endPoint2;

    MustThroughRegion(const string& name, pair<float, float> start1, pair<float, float> end1, pair<float, float> start2, pair<float, float> end2)
        : regionName(name), startPoint1(start1), endPoint2(end1), startPoint2(start2), endPoint1(end2) {}
};

class Net {
public:
    int id;
    string tx;
    vector<string> rx;
    int numTracks;
    vector<MustThroughRegion> mustThrough;
    vector<MustThroughRegion> hmftMustThrough;
    pair<float, float> txCoord;
    vector<pair<float, float>> rxCoord;

    Net(int netId,
        const string& txBlock,
        const vector<string>& rxBlocks,
        int num,
        const vector<MustThroughRegion>& mustThroughRegions,
        const vector<MustThroughRegion>& hmftMustThroughRegions,
        pair<float, float> txCoordinate,
        const vector<pair<float, float>>& rxCoordinates)
        : id(netId),
          tx(txBlock),
          rx(rxBlocks),
          numTracks(num),
          mustThrough(mustThroughRegions),
          hmftMustThrough(hmftMustThroughRegions),
          txCoord(txCoordinate),
          rxCoord(rxCoordinates) {}

    // Methods
    void print() const;
};

#endif