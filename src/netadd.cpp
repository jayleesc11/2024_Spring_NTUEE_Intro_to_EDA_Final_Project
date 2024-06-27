#include "netadd.h"    
    
vector<Net> Netadd::modify(vector<Net>& nets, std::unordered_map<std::string, std::vector<Component>>& comp, std::unordered_map<std::string, std::vector<Region>>& regs){

    int lbx, lby;
    for ( int i = 0; i < nets.size(); i++){
        if (nets[i].tx.find("REGION") != std::string::npos){

            // if (regs.find(nets[i].tx) != regs.end()){
            //     std::cout << regs[nets[i].tx][0].leftBottom.first << "\t";
            // }

            nets[i].txCoord.first += regs[nets[i].tx][0].leftBottom.first;
            nets[i].txCoord.second += regs[nets[i].tx][0].leftBottom.second;
        } else {
            nets[i].txCoord.first += comp[nets[i].tx][0].position.first;
            nets[i].txCoord.second += comp[nets[i].tx][0].position.second;
        }
        for (int j = 0; j < nets[i].rx.size(); j++){
            if (nets[i].rx[j].find("REGION") != std::string::npos){
                nets[i].rxCoord[j].first += regs[nets[i].rx[j]][0].leftBottom.first;
                nets[i].rxCoord[j].second += regs[nets[i].rx[j]][0].leftBottom.second;
            } else {
                nets[i].rxCoord[j].first += comp[nets[i].rx[j]][0].position.first;
                nets[i].rxCoord[j].second += comp[nets[i].rx[j]][0].position.second;
            }
        }
    }
    return nets;
}
