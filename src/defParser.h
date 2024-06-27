#ifndef DEFPARSER_H
#define DEFPARSER_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <unordered_map>

struct Chip {
    int units;
    std::pair<std::pair<int, int>, std::pair<int, int>> dieArea_pair;
    Chip() : units(0), dieArea_pair({{0, 0}, {0, 0}}) {}
    Chip(const int& u, const std::pair<std::pair<int, int>, std::pair<int, int>>& dp)
        : units(u), dieArea_pair(dp) {}
};

struct Component {
    std::string compName;
    std::string modelName;
    std::pair<int, int> position;
    std::string rotation;
    std::vector<std::pair<int, int>> chipPosition;
    Component(const std::string& c, const std::string& m, const std::pair<int, int>& p, const std::string r, std::vector<std::pair<int, int>> cp) 
        : compName(c), modelName(m), position(p), rotation(r), chipPosition(cp) {}
};

struct Region {
    std::string regionID;
    std::pair<int, int> leftBottom;
    std::pair<int, int> rightTop;
    Region(const std::string& r, const std::pair<int, int>& b, const std::pair<int, int>& t) 
        : regionID(r), leftBottom(b), rightTop(t) {}
};


class DefParser {
private:
    std::vector<Component> components;
    Chip chip;
    std::unordered_map<std::string, std::vector<Component>> mapComp;
    std::unordered_map<std::string, std::vector<Region>> mapReg;
    std::vector<Region> regions;
    std::unordered_map<std::string, std::vector<std::pair<int, int>>> blocks;
    std::vector<std::pair<int, int>> comp_blocks_pos_moved;
    std::string dieArea;
    std::string numComps;
    std::string numReg;
    std::string filename;
    std::size_t dir_pos = filename.find("def/") + 4; 
    std::string def_dir = filename.substr(0,dir_pos);
    std::vector<std::pair<int, int>> new_block_pos;
public:
    DefParser(const std::string& fname);
    Chip parseChipInfo();
    std::vector<Component> parseComponents();
    std::vector<Region> parseRegions();
    std::unordered_map<std::string, std::vector<std::pair<int, int>>>  parseBlocks(std::vector<Component>& comp);
    std::unordered_map<std::string, std::vector<Component>> mapComponents(std::vector<Component>& comp);
    std::unordered_map<std::string, std::vector<Region>> mapRegions(std::vector<Region>& reg);

    std::unordered_map<std::string, std::vector<Component>> move(std::unordered_map<std::string, std::vector<Component>>& mapComp, std::unordered_map<std::string, std::vector<std::pair<int, int>>>& blocks);

    // void printDefFile(std::vector<Component>& comp, std::vector<Region>& reg) const;
    void printDefFile(std::unordered_map<std::string, std::vector<Component>>& comp, std::unordered_map<std::string, std::vector<std::pair<int, int>>>& blk) const;

};

#endif


