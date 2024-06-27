// ./CGR 1 ./inputs/case5/case5_def/chip_top.def caseOO.cfg.json caseOO.connection_matrix.json
#include "defParser.h"
#include <algorithm>
#include <sstream>


DefParser::DefParser(const std::string& fname) : filename(fname) {}

Chip DefParser::parseChipInfo(){
    std::ifstream defFile(filename);
    std::string line;
    if (!defFile) {
        std::cerr << "Unable to open file:"<< filename << std::endl;
        return chip;
    }
    while (getline(defFile, line)){
        // Trim function to remove leading/trailing spaces
        line.erase(0, line.find_first_not_of(" \n\r\t"));
        line.erase(line.find_last_not_of(" \n\r\t") + 1);
        if (line.find("UNITS DISTANCE MICRONS") != std::string::npos) {
            size_t start = line.find("UNITS DISTANCE MICRONS") + std::string("UNITS DISTANCE MICRONS").length();
            // Skip any spaces after the keyword
            start = line.find_first_not_of(" ", start);
            size_t end = line.find(" ", start);
            if (start != std::string::npos && end != std::string::npos) {
                chip.units = std::stoi(line.substr(start, end - start));
            } else if (start != std::string::npos) {
                chip.units = std::stoi(line.substr(start));
            }
        }
        
        if (line.find("DIEAREA") != std::string::npos) {
            size_t start = line.find("(");
            if (start != std::string::npos) {
                size_t end = line.find(")", start);
                if (end != std::string::npos) {
                    dieArea = line.substr(start, end - start + 1) + " ";
                }
                start = line.find("(", end);
                end = line.find(")", start);
                if (start != std::string::npos && end != std::string::npos) {
                    dieArea += line.substr(start, end - start + 1);
                }
            }
        }
    }
    std::stringstream ss(dieArea);
    char ignore;
    int a, b, c, d;

    ss >> ignore >> a >> b >> ignore >> ignore >> c >> d >> ignore;

    std::pair<int, int> firstPair = std::make_pair(a, b);
    std::pair<int, int> secondPair = std::make_pair(c, d);
    
    chip.dieArea_pair = std::make_pair(firstPair, secondPair);
    defFile.close();
    return chip;
}

std::vector<Component> DefParser::parseComponents() {
    std::ifstream defFile(filename);
    std::string line;
    components.clear();

    if (!defFile) {
        std::cerr << "Unable to open file" << std::endl;
        return components;
    }

    while (getline(defFile, line)) {
        // Trim function to remove leading/trailing spaces
        line.erase(0, line.find_first_not_of(" \n\r\t"));
        line.erase(line.find_last_not_of(" \n\r\t") + 1);
        

        if (line.find("COMPONENTS") != std::string::npos) {
            size_t start = line.find(" ");
            size_t end = line.find(";");
            numComps = line.substr(start + 1, end - start - 1);

            bool inComponents = true;
            while (inComponents && getline(defFile, line)) {
                if (line.find("END COMPONENTS") != std::string::npos) {
                    inComponents = false;
                    break;
                }

                std::istringstream iss(line);
                std::string token;
                int x1, y1;

                if (getline(iss, token, '-')) { // Skip initial '-'
                    if (getline(iss, token, ' ')) {
                        if (getline(iss, token, ' ')) {
                            std::string compName = token;
                            if (getline(iss, token, ' ')) {
                                std::string modelName = token;
                                if (getline(iss, token, '(')) {
                                    if (getline(iss, token, ' ')) {
                                        if(getline(iss, token, ' ')){
                                            x1 = std::stoi(token);
                                            if(getline(iss, token, ' ')){
                                                y1 = std::stoi(token);
                                                if(getline(iss, token, ' ')){
                                                    if(getline(iss, token, ' ')){
                                                        components.emplace_back(compName, modelName, std::make_pair(x1, y1), token, comp_blocks_pos_moved);
                                                    }
                                                }
                                                

                                            }
                                        }
                                    }  
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    defFile.close();
    return components;
}

std::vector<Region> DefParser::parseRegions() {
    std::ifstream defFile(filename);
    std::string line;
    regions.clear();

    if (!defFile) {
        std::cerr << "Unable to open file" << std::endl;
        return regions;
    }

    while (getline(defFile, line)) {
        // Trim function to remove leading/trailing spaces
        line.erase(0, line.find_first_not_of(" \n\r\t"));
        line.erase(line.find_last_not_of(" \n\r\t") + 1);

        if (line.find("REGIONS") != std::string::npos) {
            size_t start = line.find(" ");
            size_t end = line.find(";");
            numReg = line.substr(start + 1, end - start - 1);
            bool inRegions = true;
            while (inRegions && getline(defFile, line)) {
                if (line.find("END REGIONS") != std::string::npos) {
                    inRegions = false;
                    break;
                }

                std::istringstream iss(line);
                std::string token, detail;
                if (line.find("-") != std::string::npos) {
                    std::istringstream iss(line);
                    std::string token, regionID;
                    int x1, y1, x2, y2;
                    if (getline(iss, token, '-')) { // Skip initial '-'
                        if (getline(iss, token, ' ')) {
                            if (getline(iss, token, ' ')) {
                                regionID = token;
                                regionID.erase(0, regionID.find_first_not_of(" \n\r\t"));
                                regionID.erase(regionID.find_last_not_of(" \n\r\t") + 1);
                                
                                if (getline(iss, token, ' ')) {
                                    if(getline(iss, token, ' ')){
                                        x1 = std::stoi(token);
                                        if(getline(iss, token, ' ')){
                                            y1 = std::stoi(token);
                                            if(getline(iss, token, ' ')){
                                                if(getline(iss, token, ' ')){
                                                    if(getline(iss, token, ' ')){
                                                        x2 = std::stoi(token);
                                                        if(getline(iss, token, ' ')){
                                                            y2 = std::stoi(token);
                                                            regions.emplace_back(regionID, std::make_pair(x1, y1), std::make_pair(x2, y2));
                                                        }
                                                    }
                                                }
                                            }
                                            
                                        }
                                    }
                                }                               
                            }
                        }
                    }
                }

                
            }
        }
    }
    defFile.close();
    return regions;
}

std::unordered_map<std::string, std::vector<std::pair<int, int>>> DefParser::parseBlocks(std::vector<Component>& comp){

    std::vector<std::string> blk_dir;
    std::vector<std::string> blk_id;
    std::unordered_map<std::string, std::string> blk_dir_dict;
    std::string total_dir;
    std::string blk_diearea;
    std::vector<std::pair<int, int>> blk_vertex;

    for (const Component& comp : components) {
        total_dir = def_dir;
        total_dir = total_dir + comp.modelName + ".def";
        blk_dir.emplace_back(total_dir);
        blk_id.emplace_back(comp.modelName);
        blk_dir_dict[total_dir] = comp.modelName;
    }
    blk_dir.erase( unique( blk_dir.begin(), blk_dir.end() ), blk_dir.end() );
    blk_id.erase( unique( blk_id.begin(), blk_id.end() ), blk_id.end() );


    for(int i=0; i < blk_dir.size(); i++){
        blk_vertex.clear();
        std::ifstream defFile(blk_dir[i]);
        std::string line;

        if (!defFile) {
            std::cerr << "Unable to open file" << std::endl;
            return blocks;
        }

        while (getline(defFile, line)) {
            if (line.find("DIEAREA") != std::string::npos) {

                size_t start = line.find(" ");
                size_t end = line.find(";");
                int x1, y1, x2, y2;

                blk_diearea = line.substr(start + 1, end - start - 1);

                std::istringstream iss(blk_diearea);
                std::string token;

                while (getline(iss, token, '(')){
                    if(getline(iss, token, ' ')){
                        if(getline(iss, token, ' ')){
                            x1 = std::stoi(token);
                            if(getline(iss, token, ' ')){
                                y1 = std::stoi(token);
                                blk_vertex.emplace_back(std::make_pair(x1, y1));
                            }
                        }  
                    }
                        

                    

                }
            }
        }
        std::string id = blk_dir_dict[blk_dir[i]];
        blocks[id] = blk_vertex;

    }


    return blocks;
}

// void DefParser::printDefFile(std::vector<Component>& comp, std::vector<Region>& reg) const {
//     for (const Component& comp : components) {
//         std::cout << "Component Name: " << comp.compName << ", Model Name: " << comp.modelName << ", Position: (" << comp.position.first << "," << comp.position.second << ")" << std::endl;
//     }
//     for (const Region& reg : regions) {
//         std::cout << "Region Name: " << reg.regionID << ", left bottom: (" << reg.leftBottom.first << "," << reg.leftBottom.second << ") , right top: (" << reg.rightTop.first << "," << reg.rightTop.second << ") " << std::endl;
//     }
//     std::cout << "numComps: " << numComps << std::endl;
//     std::cout << "numReg: " << numReg << std::endl;
//     std::cout << "DIEAREA: " << dieArea << std::endl;
// }
std::unordered_map<std::string, std::vector<Component>> DefParser::mapComponents(std::vector<Component>& comp){
    for (const Component& comp : components) {
        mapComp[comp.compName].push_back(comp);
    }
    // for (const Component& comp : mapComp["BLOCK_1"]) {
    //     std::cout << "Component Name: " << comp.compName << ", Model Name: " << comp.modelName << ", Position: (" << comp.position.first << "," << comp.position.second << ")" << ", Rotation: " << comp.rotation <<std::endl;
    // }
    return mapComp;
}
std::unordered_map<std::string, std::vector<Region>> DefParser::mapRegions(std::vector<Region>& reg){
    for (const Region& reg : regions) {
        mapReg[reg.regionID].push_back(reg);
    }
    // for (const Component& comp : mapComp["BLOCK_1"]) {
    //     std::cout << "Component Name: " << comp.compName << ", Model Name: " << comp.modelName << ", Position: (" << comp.position.first << "," << comp.position.second << ")" << ", Rotation: " << comp.rotation <<std::endl;
    // }
    return mapReg;
}

std::unordered_map<std::string, std::vector<Component>> DefParser::move(std::unordered_map<std::string, std::vector<Component>>& compMap, std::unordered_map<std::string, std::vector<std::pair<int, int>>>& blk){

    for (auto& comp: compMap) {
        int comp_position_x;
        int comp_position_y;
        std::string comp_rotation;
        std::string comp_id;
        std::string comp_name;

        

        for (const Component& c : compMap[comp.first]) {
            comp_position_x = c.position.first;
            comp_position_y = c.position.second;

            comp_rotation = c.rotation;
            comp_id = c.modelName;
            comp_name = c.compName;
        }

        std::vector<std::pair<int, int>> comp_block = blk[comp_id];
        int max_X = 0;
        int max_Y = 0;

        for(int i=0; i < comp_block.size(); i++){            
            if (comp_block[i].first >= max_X){
                max_X = comp_block[i].first;
            }
            if (comp_block[i].second >= max_Y){
                max_Y = comp_block[i].second;
            }
        }
        int x,y;
        new_block_pos.resize(comp_block.size());
        for(int i=0; i < comp_block.size(); i++){ 
            if (comp_rotation == "N"){
                x = comp_block[i].first;
                y = comp_block[i].second;
                new_block_pos[i].first = x;
                new_block_pos[i].second = y;
            }
            if (comp_rotation == "S"){
                x = comp_block[i].first;
                y = comp_block[i].second;
                new_block_pos[i].first = max_X - x;
                new_block_pos[i].second = max_Y - y;
                
            }
            if (comp_rotation == "W"){
                x = comp_block[i].first;
                y = comp_block[i].second;
                new_block_pos[i].first = max_Y - y;
                new_block_pos[i].second = x;
            }
            if (comp_rotation == "E"){
                x = comp_block[i].first;
                y = comp_block[i].second;
                new_block_pos[i].first = y;
                new_block_pos[i].second = max_X - x;
            }
            if (comp_rotation == "FS"){
                x = comp_block[i].first;
                y = comp_block[i].second;
                new_block_pos[i].first = x;
                new_block_pos[i].second = max_Y - y;
            }
            if (comp_rotation == "FW"){
                x = comp_block[i].first;
                y = comp_block[i].second;
                new_block_pos[i].first = y;
                new_block_pos[i].second = x;
            }
            if (comp_rotation == "FE"){
                x = comp_block[i].first;
                y = comp_block[i].second;
                new_block_pos[i].first = max_Y - y;
                new_block_pos[i].second = max_X - x;
            }
            if (comp_rotation == "FN"){
                x = comp_block[i].first;
                y = comp_block[i].second;
                new_block_pos[i].first = max_X - x;
                new_block_pos[i].second = y;
            }
            

            int min_X, min_Y = 0;
            for(int i=0; i < new_block_pos.size(); i++){
                if (new_block_pos[i].first <= min_X){
                    min_X = new_block_pos[i].first;
                }
                if (new_block_pos[i].second <= min_Y){
                    min_Y = new_block_pos[i].second;
                }
            }
            x = comp_position_x - min_X;
            y = comp_position_y - min_Y;
            new_block_pos[i].first += x;
            new_block_pos[i].second += y;


        }
        for (Component& c : compMap[comp.first]) {
            c.chipPosition = new_block_pos;
        }

    }

    return compMap;

}


void DefParser::printDefFile(std::unordered_map<std::string, std::vector<Component>>& positionedComp, std::unordered_map<std::string, std::vector<std::pair<int, int>>>& blk) const {
    for (auto& compMap: positionedComp) {
        for (const Component& comp : positionedComp[compMap.first]) {
            std::cout << "Component Name: " << comp.compName << ", Model Name: " << comp.modelName << ", Position: (" << comp.position.first << "," << comp.position.second << ")" << ", Rotation: " << comp.rotation <<std::endl;
            for(int i=0; i < comp.chipPosition.size(); i++){
                std::cout << "(" << comp.chipPosition[i].first << ","<< comp.chipPosition[i].second << ")" << " ";
            }
            std::cout << std::endl; 
        }
    }
    for (auto& it: blk) {
        std::cout << it.first << std::endl;
        for(int i=0; i < it.second.size(); i++){
            std::cout << "(" << it.second[i].first << ","<< it.second[i].second << ")" << std::endl;
        }

    }

    std::cout << "numComps: " << numComps << std::endl;
    std::cout << "numReg: " << numReg << std::endl;
    std::cout << "DIEAREA: " << dieArea << std::endl;
}