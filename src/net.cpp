#include <iostream>
#include <vector>
#include "net.h"

using namespace std;

void Net::print() const {
    cout << "ID: " << id << endl;
    cout << "TX: " << tx << endl;
    cout << "RX: ";
    for (const auto& receiver : rx) {
        cout << receiver << " ";
    }
    cout << endl;
    cout << "Num Tracks: " << numTracks << endl;
    cout << "MUST_THROUGH: " << endl;
    for (const auto& region : mustThrough) {
        cout << "Block Name: " << region.regionName << endl;
        cout << "Start Point 1: (" << region.startPoint1.first << ", " << region.startPoint1.second << ")" << endl;
        cout << "End Point 1: (" << region.endPoint1.first << ", " << region.endPoint1.second << ")" << endl;
        cout << "Start Point 2: (" << region.startPoint2.first << ", " << region.startPoint2.second << ")" << endl;
        cout << "End Point 2: (" << region.endPoint2.first << ", " << region.endPoint2.second << ")" << endl;
    }
    cout << "HMFT_MUST_THROUGH: " << endl;
    for (const auto& region : hmftMustThrough) {
        cout << "Block Name: " << region.regionName << endl;
        cout << "Start Point 1: (" << region.startPoint1.first << ", " << region.startPoint1.second << ")" << endl;
        cout << "End Point 1: (" << region.endPoint1.first << ", " << region.endPoint1.second << ")" << endl;
        cout << "Start Point 2: (" << region.startPoint2.first << ", " << region.startPoint2.second << ")" << endl;
        cout << "End Point 2: (" << region.endPoint2.first << ", " << region.endPoint2.second << ")" << endl;
    }
    cout << "TX Coord: (" << txCoord.first << ", " << txCoord.second << ")" << endl;
    cout << "RX Coord: ";
    for (const auto& coord : rxCoord) {
        cout << "(" << coord.first << ", " << coord.second << ") ";
    }
    cout << endl;
}