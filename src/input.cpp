#include "input.h"
#include "defParser.h"
#include <algorithm>

// define == operator between std::pair<int, int> & std::pair<float, float>
bool operator==(const std::pair<int, int>& lhs, const std::pair<int, int>& rhs) {
    return lhs.first == rhs.first && lhs.second == rhs.second;
}

std::pair<std::pair<std::vector<std::pair<int, int>>,std::vector<Obstacles>>,std::vector<Vertex>> Input::breakBlocks(std::unordered_map<std::string, std::vector<Component>>& compMap){
    int obstacle_id = 0;
    int obs_vertices_id = 0;
    int vertice_id = 0;
    std::pair<int, int> start, end, ul, lr;
    std::pair<int, int> start_i, end_i;
    for (auto& comp:compMap) {
        obs_edges.clear();
        std::vector<Component> component = compMap[comp.first];
    
        if (component[0].chipPosition.size() == 2){

            
            start_i = component[0].chipPosition[0];
            end_i = component[0].chipPosition[1];
            if (start_i.second < end_i.second and start_i.first > end_i.first){
                start.first = end_i.first;
                end.first = start_i.first;
                start.second = start_i.second;
                end.second = end_i.second;
            } else if (start_i.second > end_i.second and start_i.first < end_i.first){
                start.second = end_i.second;
                end.second = start_i.second;
                start.first = start_i.first;
                end.first = end_i.first;

            } else if (start_i.first > end_i.first and start_i.second > end_i.second){
                start = end_i;
                end = start_i;
            } else {
                start = start_i;
                end = end_i;
            }
            

       
            ul = std::make_pair(start.first, end.second);
            lr = std::make_pair(end.first, start.second);
            obstacles.emplace_back(obstacle_id, start, end, obs_vertices_id, obs_vertices_id + 2, obs_vertices_id + 1, obs_vertices_id + 3);
            
            int start_id, end_id, ul_id, lr_id;
            obs_vertices.emplace_back(obs_vertices_id, start, false);
            start_id = obs_vertices_id;
            obs_vertices_id += 1;
            obs_vertices.emplace_back(obs_vertices_id, end, false);
            end_id = obs_vertices_id;
            obs_vertices_id += 1;
            obs_vertices.emplace_back(obs_vertices_id, ul, false);
            ul_id = obs_vertices_id;
            obs_vertices_id += 1;
            obs_vertices.emplace_back(obs_vertices_id, lr, false);
            lr_id = obs_vertices_id;
            obs_vertices_id += 1;
            output_edges.emplace_back(std::make_pair(start_id, ul_id));
            output_edges.emplace_back(std::make_pair(ul_id, end_id));
            output_edges.emplace_back(std::make_pair(end_id, lr_id));
            output_edges.emplace_back(std::make_pair(lr_id, start_id));

         
            obstacle_id += 1;
        } else {
            std::vector<std::pair<std::pair<int, int>,std::pair<int, int>>> find_dup_edges;
            std::map<std::pair<int, int>, std::vector<std::pair<int, int>>> edges;
            auto comparator_y = [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
                return a.second > b.second;
            };
            auto comparator_x_acs = [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
                return a.first < b.first;
            };

            // create edge map, each vertice only link to one (prevent repeat)

            for (int i = 0; i < component[0].chipPosition.size() - 1; i++){
                edges[component[0].chipPosition[i]].push_back(component[0].chipPosition[i+1]);
                // edges[component[0].chipPosition[i+1]].push_back(component[0].chipPosition[i]);
            }
            edges[component[0].chipPosition[component[0].chipPosition.size() - 1]].push_back(component[0].chipPosition[0]);
            // edges[component[0].chipPosition[0]].push_back(component[0].chipPosition[component[0].chipPosition.size() - 1]);

            //add all vertice

            std::sort(component[0].chipPosition.begin(), component[0].chipPosition.end(), comparator_y);
            std::vector<std::pair<int, int>> vertices_y_acs = component[0].chipPosition;
            std::sort(component[0].chipPosition.begin(), component[0].chipPosition.end(), comparator_x_acs);
            std::vector<std::pair<int, int>> vertices_x_acs = component[0].chipPosition;

            

            for (auto& edge:edges){
                // edge vertex same x, vertical edge 
                if (edges[edge.first][0].first == edge.first.first){
                    // for every y points, add the vertice to the edge if the min(edge) < y < max(edge)
                    for (int i = 0 ; i  < vertices_y_acs.size(); i++){
                        if (not (vertices_y_acs[i].second > std::max(edges[edge.first][0].second, edge.first.second) or vertices_y_acs[i].second < std::min(edges[edge.first][0].second, edge.first.second))){
                            vertices.emplace_back(vertice_id, std::make_pair(edge.first.first, vertices_y_acs[i].second), false);
                            vertice_id += 1;
                        }
                        
                    }
                } else {
                    // edge vertex same y. horizontal line.
                    for (int i = 0 ; i  < vertices_x_acs.size(); i++){
                        if (not (vertices_x_acs[i].first > std::max(edges[edge.first][0].first, edge.first.first) or vertices_x_acs[i].first < std::min(edges[edge.first][0].first, edge.first.first))){
                            vertices.emplace_back(vertice_id, std::make_pair(vertices_x_acs[i].first, edge.first.second), false);
                            vertice_id += 1;
                            
                        }
                        
                    }
                }
                auto comparator_vertices = [](const Vertex& a, const Vertex& b) {
                    return a.coord.first < b.coord.first;
                };
                std::sort(vertices.begin(), vertices.end(), comparator_vertices);

                // // Remove consecutive duplicates
                // auto last = std::unique(vertices.begin(), vertices.end());

                // vertices.resize(std::distance(vertices.begin(),last));
            }
            // std::cout <<" vertices" << std::endl;
            // for (int i = 0; i < vertices.size(); i++){
            //     std::cout << vertices[i].coord.first << "," <<vertices[i].coord.second << std::endl;
            // }
            // std::cout <<"fin";
            // group same x and same y 
            std::map<int, std::vector<int>> grouped_vertices_x;
            for (int i=0; i < vertices.size(); i++){
                if (std::find(grouped_vertices_x[vertices[i].coord.first].begin(),grouped_vertices_x[vertices[i].coord.first].end(), vertices[i].coord.second) == grouped_vertices_x[vertices[i].coord.first].end()){
                    grouped_vertices_x[vertices[i].coord.first].push_back(vertices[i].coord.second);

                }
            }

            std::map<int, std::vector<int>> grouped_vertices_y;
            for (int i=0; i < vertices.size(); i++){
                grouped_vertices_y[vertices[i].coord.second].push_back(vertices[i].coord.first);
            }

            // sort all x and y in grouped_vertices_x
            std::vector<int> xs, ys;

            for (auto& gx:grouped_vertices_x) {
                xs.emplace_back(gx.first);
                // for (int i = 0; i < grouped_vertices_x[gx.first].size(); i++){
                //     std::cout << "xsss" << grouped_vertices_x[gx.first][i] <<" ";
                // }
                // std::cout << std::endl;
                
                std::sort(grouped_vertices_x[gx.first].begin(), grouped_vertices_x[gx.first].end());
            }

            for (auto& gy:grouped_vertices_y) {
                ys.emplace_back(gy.first);
                std::sort(grouped_vertices_y[gy.first].begin(), grouped_vertices_y[gy.first].end());

            }

            int currentX, currentY, nextX, nextY;
            bool loopY = true;
            std::vector<int> intersectX(1);
            std::sort(xs.begin(), xs.end());
            std::sort(ys.begin(), ys.end());

            int j = 0;
            int k = 0;
            // loop for one x each when the partition is not reaching the top
            for (int i = 0; i < xs.size() - 1; i++){
                nextY = 0;
                j = 0;
                currentX = xs[i];
                int max_Y = *std::max_element(grouped_vertices_x[currentX].begin(), grouped_vertices_x[currentX].end());
                
                // std::cout << i << " "<< currentX << std::endl;
                while ((nextY < max_Y) and (j < grouped_vertices_x[currentX].size())){
                    //  current left position and the corresponding y
                    bool test = (nextY < max_Y);
                    // std::cout <<  nextY << " " << max_Y << "test" << test << std::endl;
                    currentY = grouped_vertices_x[currentX][j];
                    // std::cout <<currentX << currentY << " " << j << std::endl;
                    j+=1;
                    
                    // find the nextX and nextY
                    // have to find the correct next x by finding the nearest right x which share the next y
                    // loop through possible Y
                    nextX = xs[i+1];
                    bool noX = true;
                    for (int p = 0; p< grouped_vertices_x[nextX].size(); p++){
                        if (grouped_vertices_x[nextX][p] == currentY){
                            noX = false;
                        }
                    }
                    if (noX == true){
                        continue;
                    }
                    // std::cout<< grouped_vertices_x[nextX].size() <<"size" << std::endl;
                    for (int k = 0; k < grouped_vertices_x[nextX].size(); k++){
                        // std::cout << nextX << "next" << grouped_vertices_x[nextX][k] << " " << k<< std::endl;
                        nextY = grouped_vertices_x[nextX][k];
                        if (nextY <= currentY){
                            continue;
                        }
                        // ensure the corresponding new y is also correspond to the current x
                        int start_id, end_id, ul_id, lr_id;
                        if (std::find(grouped_vertices_x[currentX].begin(),grouped_vertices_x[currentX].end(), nextY) != grouped_vertices_x[currentX].end()){
                           
                            std::pair<int, int> start = std::make_pair(currentX, currentY);
                            std::pair<int, int> end = std::make_pair(nextX, nextY);
                            std::pair<int, int> ul = std::make_pair(currentX, nextY);
                            std::pair<int, int> lr = std::make_pair(nextX, currentY);

  

                            //search from existing vertex
                            bool c1 = false;
                            bool c2  = false; 
                            bool c3 = false;
                            bool c4 = false;
                            for (int o = 0; o < obs_vertices.size(); o++){
                                if (start == obs_vertices[o].coord){
                                    start_id = obs_vertices[o].id;
                                    c1 = true;
                                }
                                if (end == obs_vertices[o].coord){
                                    end_id = obs_vertices[o].id;
                                    c2 = true;
                                }
                                if (ul == obs_vertices[o].coord){
                                    // std :: cout << ul.first << " " << ul.second << std::endl;
                                    ul_id = obs_vertices[o].id;
                                    // std::cout << ul_id << std::endl;
                                    // std::cout << end.second << std::endl;
                                    // std::cout << nextY << std::endl;

                                    c3 = true;
                                }
                                if (lr == obs_vertices[o].coord){
                                    lr_id = obs_vertices[o].id;
                                    c4 = true;
                                }
                                
                            }
                            if (c1 == false){
                                start_id = obs_vertices_id;
                                obs_vertices_id += 1;
                                obs_vertices.emplace_back(start_id, start, false);
                            }
                            if (c2 == false){
                                end_id = obs_vertices_id;
                                obs_vertices_id += 1;
                                obs_vertices.emplace_back(end_id, end , false);
                            }
                            if (c3 == false){
                                ul_id = obs_vertices_id;
                                obs_vertices_id += 1;
                                obs_vertices.emplace_back(ul_id, ul, false);
                            }
                            if (c4 == false){
                                lr_id = obs_vertices_id;
                                obs_vertices_id += 1;
                                obs_vertices.emplace_back(lr_id, lr, false);
                            }
                            // std::cout << "current : "<< currentX << " " << currentY<< " next :" << nextX << " " << nextY << std::endl;
                            
                            obs_edges.emplace_back(std::make_pair(start_id,ul_id));
                            obs_edges.emplace_back(std::make_pair(start_id,lr_id));
                            obs_edges.emplace_back(std::make_pair(ul_id,end_id));
                            obs_edges.emplace_back(std::make_pair(lr_id,end_id));
                            find_dup_edges.emplace_back(std::make_pair(start,ul));
                            find_dup_edges.emplace_back(std::make_pair(start,lr));
                            find_dup_edges.emplace_back(std::make_pair(ul,end));
                            find_dup_edges.emplace_back(std::make_pair(lr,end));
                            output_edges.emplace_back(std::make_pair(lr_id, end_id));
                            output_edges.emplace_back(std::make_pair(start_id,ul_id));
                            output_edges.emplace_back(std::make_pair(start_id,lr_id));
                            output_edges.emplace_back(std::make_pair(ul_id,end_id));

                            

                            obstacles.emplace_back(obstacle_id, start, end ,start_id,ul_id, end_id,lr_id);
                            obstacle_id += 1;
                            
                            break;
                        }
                    }
                    // std::cout <<currentX << " " << currentY << " "<< nextX << " " << nextY << "\n";


                    // for (int l = 1; l < grouped_vertices_x.size(); l++){
                    //     // the nextx has to be in the same level with currect x
                    //     if (std::find(grouped_vertices_x[xs[currentX + l]].begin(),grouped_vertices_x[xs[currentX + l]].end(), currentY) != grouped_vertices_x[xs[currentX + l]].end()){
                    //         // set nextx be the next larger xs which must be the nextxs if same level
                    //         nextX = xs[i+l];
                    //         std::cout << nextX << std::endl;
                    //         // find the corresponding next y
                    //         for (int k = 0; k < grouped_vertices_x[nextX].size(); k++){
                    //             nextY = grouped_vertices_x[nextX][k];
                    //             // ensure the corresponding new y is also correspond to the current x
                    //             if (std::find(grouped_vertices_x[currentX].begin(),grouped_vertices_x[currentX].end(), nextY) != grouped_vertices_x[currentX].end()){
                    //                 break;
                    //             }
                    //         }
                    //         std::cout <<currentX << " " << currentY << " "<< nextX << " " << nextY << "\n";
                    //         break;

                    //     }
                    // }
                    

                
                }
            }
            // std::map<std::pair<std::pair<int, int>, std::pair<int, int>>, std::pair<int, int>> index_edge;
            // for (int i = 0; i < find_dup_edges.size(); i++){
            //     std::pair<int, int> obs_edge_first = find_dup_edges[i].first;
            //     std::pair<int, int> obs_edge_second = find_dup_edges[i].second;
            //     if (edges.find(obs_edge_first) == edges.end()){
            //         cut_edges.emplace_back(obs_edges[i]);
            //         index_edge.insert(std::pair<std::pair<std::pair<int, int>, std::pair<int, int>>, std::pair<int, int>> (find_dup_edges[i], obs_edges[i]));
            //     } else if (edges.find(obs_edge_second) == edges.end()){
            //         cut_edges.emplace_back(obs_edges[i]);
            //         index_edge.insert(std::pair<std::pair<std::pair<int, int>, std::pair<int, int>>, std::pair<int, int>> (find_dup_edges[i], obs_edges[i]));
            //     } 
 

            // }
            std::map<std::pair<int, int>, int> index_v_map;
            for ( int i = 0; i < obs_vertices.size(); i++){
                index_v_map.insert(std::pair<std::pair<int, int>, int> (obs_vertices[i].coord, obs_vertices[i].id));
            }

            for (int i = 0; i < find_dup_edges.size(); i++){


                // std::cout << "find_dup_edges : " << find_dup_edges[i].first.first << "," << find_dup_edges[i].first.second << "\t" <<  find_dup_edges[i].second.first << "," << find_dup_edges[i].second.second <<std::endl;
                bool isVertical;
                int min_y_dup, max_y_dup, min_x_dup, max_x_dup;
                if (find_dup_edges[i].first.first == find_dup_edges[i].second.first){ 
                    isVertical = true;
                    if (find_dup_edges[i].first.second < find_dup_edges[i].second.second){
                        min_y_dup = find_dup_edges[i].first.second;
                        max_y_dup = find_dup_edges[i].second.second;
                    } else {
                        min_y_dup = find_dup_edges[i].second.second;
                        max_y_dup = find_dup_edges[i].first.second;
                    }
                } else {
                    isVertical = false;
                    if (find_dup_edges[i].first.first < find_dup_edges[i].second.first){
                        min_x_dup = find_dup_edges[i].first.first;
                        max_x_dup = find_dup_edges[i].second.first;
                    } else {
                        min_x_dup = find_dup_edges[i].second.first;
                        max_x_dup = find_dup_edges[i].first.first;
                    }
                }
                int x_coor = find_dup_edges[i].first.first;
                int y_coor = find_dup_edges[i].first.second;
                
                for (auto& e:edges) {
                    std::vector<std::pair<int, int>> not_cut_edge;
                    bool isVertical_edge = false;
                    if (edges[e.first][0].first == e.first.first){
                        isVertical_edge = true;
                    }
                    if (isVertical == false and isVertical_edge == false){
                        if (edges[e.first][0].second != y_coor){
                            continue;
                        }
                    } else if (isVertical == true and isVertical_edge == true){
                        if (edges[e.first][0].first != x_coor){
                            continue;
                        }
                    } else {
                        continue;
                    }

                    // std::cout << "edge : " <<e.first.first << ", " << e.first.second << "\t " << edges[e.first][0].first << "," << edges[e.first][0].second <<std::endl;
                    // first vertex                        
                    int min_x , min_y, max_x, max_y;
                    if (isVertical == true){
                        if (edges[e.first][0].second < e.first.second){
                            min_y = edges[e.first][0].second;
                            max_y = e.first.second;
                        } else {
                            min_y = e.first.second;
                            max_y = edges[e.first][0].second;
                        }


                        
                        std::pair<int, int> up, down;
                        
                        
                        if (max_y_dup > max_y and min_y_dup < min_y){
                            up = std::make_pair(x_coor, max_y_dup);
                            down = std::make_pair(x_coor, min_y_dup);
                            cut_edges.emplace_back(std::make_pair(index_v_map[up], index_v_map[down]));

                            up = std::make_pair(x_coor, max_y_dup);
                            down = std::make_pair(x_coor, max_y);
                            cut_edges.emplace_back(std::make_pair(index_v_map[up], index_v_map[down]));
                            up = std::make_pair(x_coor, min_y);
                            down = std::make_pair(x_coor, min_y_dup);
                            if (std::find(not_cut_edge.begin(), not_cut_edge.end(), std::make_pair(index_v_map[up], index_v_map[down])) == not_cut_edge.end()){
                                cut_edges.emplace_back(std::make_pair(index_v_map[up], index_v_map[down]));
                            }
                            // std::cout <<"cut edge" ;
                            // std::cout<< x_coor << "," << max_y<< std::endl;
                            // std::cout<< x_coor << "," << min_y<< std::endl;
                            // std::cout<< x_coor << "," << max_y_dup<< std::endl;
                            // std::cout<< x_coor << "," << min_y_dup<< std::endl;
                        } else if (min_y_dup > max_y){

                            if (std::find(not_cut_edge.begin(), not_cut_edge.end(), std::make_pair(index_v_map[std::make_pair(x_coor, min_y_dup)], index_v_map[std::make_pair(x_coor, max_y_dup)])) == not_cut_edge.end()){
                                cut_edges.emplace_back(std::make_pair(index_v_map[std::make_pair(x_coor, max_y_dup)], index_v_map[std::make_pair(x_coor, min_y_dup)]));
                            }

                            // std::cout <<"cut edge" ;
                            // std::cout<< x_coor << "," << max_y<< std::endl;
                            // std::cout<< x_coor << "," << min_y<< std::endl;
                            // std::cout<< x_coor << "," << max_y_dup<< std::endl;
                            // std::cout<< x_coor << "," << min_y_dup<< std::endl;
                        } else if (max_y_dup < min_y){
                            
                            cut_edges.emplace_back(std::make_pair(index_v_map[std::make_pair(x_coor, max_y_dup)], index_v_map[std::make_pair(x_coor, min_y_dup)]));


                            if (std::find(not_cut_edge.begin(), not_cut_edge.end(), std::make_pair(index_v_map[std::make_pair(x_coor, min_y_dup)], index_v_map[std::make_pair(x_coor, max_y_dup)])) == not_cut_edge.end()){
                                cut_edges.emplace_back(std::make_pair(index_v_map[std::make_pair(x_coor, min_y_dup)], index_v_map[std::make_pair(x_coor, max_y_dup)]));
                            }

                        } else if (max_y_dup > max_y){
                            cut_edges.emplace_back(std::make_pair(index_v_map[std::make_pair(x_coor, max_y_dup)], index_v_map[std::make_pair(x_coor, min_y_dup)]));
                            

                            if (std::find(not_cut_edge.begin(), not_cut_edge.end(), std::make_pair(index_v_map[std::make_pair(x_coor, max_y_dup)], index_v_map[std::make_pair(x_coor, max_y)])) == not_cut_edge.end()){
                                cut_edges.emplace_back(std::make_pair(index_v_map[std::make_pair(x_coor, max_y_dup)], index_v_map[std::make_pair(x_coor, max_y)]));
                            }

                            // std::cout <<"cut edge" ;
                            // std::cout<< x_coor << "," << max_y<< std::endl;
                            // std::cout<< x_coor << "," << min_y<< std::endl;
                            // std::cout<< x_coor << "," << max_y_dup<< std::endl;
                            // std::cout<< x_coor << "," << min_y_dup<< std::endl;
                        } else if (min_y_dup < min_y){
                            cut_edges.emplace_back(std::make_pair(index_v_map[std::make_pair(x_coor, max_y_dup)], index_v_map[std::make_pair(x_coor, min_y_dup)]));


                            // std::cout <<"cut edge" ;
                            // std::cout<< x_coor << "," << max_y<< std::endl;
                            // std::cout<< x_coor << "," << min_y<< std::endl;
                            // std::cout<< x_coor << "," << max_y_dup<< std::endl;
                            // std::cout<< x_coor << "," << min_y_dup<< std::endl;
                            if (std::find(not_cut_edge.begin(), not_cut_edge.end(), std::make_pair(index_v_map[std::make_pair(x_coor, min_y)], index_v_map[std::make_pair(x_coor, min_y_dup)])) == not_cut_edge.end()){
                                cut_edges.emplace_back(std::make_pair(index_v_map[std::make_pair(x_coor, min_y)], index_v_map[std::make_pair(x_coor, min_y_dup)]));
                            }


                        }else{

                            if (std::find(cut_edges.begin(), cut_edges.end(), std::make_pair(index_v_map[std::make_pair(x_coor, min_y_dup)], index_v_map[std::make_pair(x_coor, max_y_dup)])) != cut_edges.end()){

                                cut_edges.erase(std::remove(cut_edges.begin(), cut_edges.end(), std::make_pair(index_v_map[std::make_pair(x_coor, min_y_dup)], index_v_map[std::make_pair(x_coor, max_y_dup)])));
                                
                            }
                            not_cut_edge.emplace_back(std::make_pair(index_v_map[std::make_pair(x_coor, max_y_dup)], index_v_map[std::make_pair(x_coor, min_y_dup)]));
                            
                            // std::cout <<"not cut edge" ;
                            // std::cout<< x_coor << "," << max_y<< std::endl;
                            // std::cout<< x_coor << "," << min_y<< std::endl;
                            // std::cout<< x_coor << "," << max_y_dup<< std::endl;
                            // std::cout<< x_coor << "," << min_y_dup<< std::endl;
                        }
                    } else {

                        if (edges[e.first][0].first < e.first.first){
                            min_x = edges[e.first][0].first;
                            max_x = e.first.first;
                        } else {
                            min_x = e.first.first;
                            max_x = edges[e.first][0].first;
                        }
                        
                        std::pair<int, int> up, down;
                        if (max_x_dup > max_x and min_x_dup < min_x){
                            up = std::make_pair(max_x_dup, y_coor);
                            down = std::make_pair(min_x_dup, y_coor);
                            cut_edges.emplace_back(std::make_pair(index_v_map[up], index_v_map[down]));

                            up = std::make_pair(max_x_dup, y_coor);
                            down = std::make_pair(max_x, y_coor);
                            cut_edges.emplace_back(std::make_pair(index_v_map[up], index_v_map[down]));
                            up = std::make_pair(min_x, y_coor);
                            down = std::make_pair(min_x_dup, y_coor);
                            // std::cout <<"cut edge 1 " << index_v_map[up] << " " << index_v_map[down] << "\n";

                            // std::cout <<"cut edge" ;
                            // std::cout<< max_x << "," << y_coor<< std::endl;
                            // std::cout<< min_x << "," << y_coor<< std::endl;
                            // std::cout<< max_x_dup << "," << y_coor<< std::endl;
                            // std::cout<< min_x_dup << "," << y_coor<< std::endl;
                            if (std::find(not_cut_edge.begin(), not_cut_edge.end(), std::make_pair(index_v_map[up], index_v_map[down])) == not_cut_edge.end()){
                                cut_edges.emplace_back(std::make_pair(index_v_map[up], index_v_map[down]));
                            }
                            
                        } else if (min_x_dup > max_x){
                            cut_edges.emplace_back(std::make_pair(index_v_map[std::make_pair(min_x_dup, y_coor)], index_v_map[std::make_pair(max_x_dup, y_coor)]));

                            // std::cout <<"cut edge 2 " << index_v_map[std::make_pair(min_x_dup, y_coor)] << " " << index_v_map[std::make_pair(max_x_dup, y_coor)] << "\n";

                            // std::cout <<"cut edge" ;
                            // std::cout<< max_x << "," << y_coor<< std::endl;
                            // std::cout<< min_x << "," << y_coor<< std::endl;
                            // std::cout<< max_x_dup << "," << y_coor<< std::endl;
                            // std::cout<< min_x_dup << "," << y_coor<< std::endl;
                            if (std::find(not_cut_edge.begin(), not_cut_edge.end(), std::make_pair(index_v_map[std::make_pair(min_x_dup, y_coor)], index_v_map[std::make_pair(max_x_dup, y_coor)])) == not_cut_edge.end()){
                                cut_edges.emplace_back(std::make_pair(index_v_map[std::make_pair(min_x_dup, y_coor)], index_v_map[std::make_pair(max_x_dup, y_coor)]));
                            }
                            

                        } else if (max_x_dup < min_x){
                            cut_edges.emplace_back(std::make_pair(index_v_map[std::make_pair(min_x_dup, y_coor)], index_v_map[std::make_pair(max_x_dup, y_coor)]));

                            // std::cout <<"cut edge 2 " << index_v_map[std::make_pair(min_x_dup, y_coor)] << " " << index_v_map[std::make_pair(max_x_dup, y_coor)] << "\n";

                            // std::cout <<"cut edge" ;
                            // std::cout<< max_x << "," << y_coor<< std::endl;
                            // std::cout<< min_x << "," << y_coor<< std::endl;
                            // std::cout<< max_x_dup << "," << y_coor<< std::endl;
                            // std::cout<< min_x_dup << "," << y_coor<< std::endl;
                            if (std::find(not_cut_edge.begin(), not_cut_edge.end(), std::make_pair(index_v_map[std::make_pair(min_x_dup, y_coor)], index_v_map[std::make_pair(max_x_dup, y_coor)])) == not_cut_edge.end()){
                                cut_edges.emplace_back(std::make_pair(index_v_map[std::make_pair(min_x_dup, y_coor)], index_v_map[std::make_pair(max_x_dup, y_coor)]));
                            }
                            

                        } else if (max_x_dup > max_x){
                            cut_edges.emplace_back(std::make_pair(index_v_map[std::make_pair(min_x_dup, y_coor)], index_v_map[std::make_pair(max_x_dup, y_coor)]));

                            // std::cout <<"cut edge 3 " << index_v_map[std::make_pair(max_x_dup, y_coor)] << " " << index_v_map[std::make_pair(max_x, y_coor)] << "\n";

                            // std::cout <<"cut edge" ;
                            // std::cout<< max_x << "," << y_coor<< std::endl;
                            // std::cout<< min_x << "," << y_coor<< std::endl;
                            // std::cout<< max_x_dup << "," << y_coor<< std::endl;
                            // std::cout<< min_x_dup << "," << y_coor<< std::endl;
                            if (std::find(not_cut_edge.begin(), not_cut_edge.end(), std::make_pair(index_v_map[std::make_pair(max_x_dup, y_coor)], index_v_map[std::make_pair(max_x, y_coor)])) == not_cut_edge.end()){
                                cut_edges.emplace_back(std::make_pair(index_v_map[std::make_pair(max_x_dup, y_coor)], index_v_map[std::make_pair(max_x, y_coor)]));
                            }

                        } else if (min_x_dup < min_x){
                            cut_edges.emplace_back(std::make_pair(index_v_map[std::make_pair(min_x_dup, y_coor)], index_v_map[std::make_pair(max_x_dup, y_coor)]));

                            // std::cout <<"cut edge 4 " << index_v_map[std::make_pair(min_x, y_coor)] << " " << index_v_map[std::make_pair(min_x_dup, y_coor)] << "\n";

                            // std::cout <<"cut edge" ;
                            // std::cout<< max_x << "," << y_coor<< std::endl;
                            // std::cout<< min_x << "," << y_coor<< std::endl;
                            // std::cout<< max_x_dup << "," << y_coor<< std::endl;
                            // std::cout<< min_x_dup << "," << y_coor<< std::endl;
                            if (std::find(not_cut_edge.begin(), not_cut_edge.end(), std::make_pair(index_v_map[std::make_pair(min_x, y_coor)], index_v_map[std::make_pair(min_x_dup, y_coor)])) == not_cut_edge.end()){
                                cut_edges.emplace_back(std::make_pair(index_v_map[std::make_pair(min_x, y_coor)], index_v_map[std::make_pair(min_x_dup, y_coor)]));
                            }

                        } else {
                            if (std::find(cut_edges.begin(), cut_edges.end(), std::make_pair(index_v_map[std::make_pair(min_x_dup, y_coor)], index_v_map[std::make_pair(max_x_dup, y_coor)])) != cut_edges.end()){

                                cut_edges.erase(std::remove(cut_edges.begin(), cut_edges.end(), std::make_pair(index_v_map[std::make_pair(min_x_dup, y_coor)], index_v_map[std::make_pair(max_x_dup, y_coor)])));
                                
                            }
  
                            // std::cout <<"not cut edge" << index_v_map[std::make_pair(min_x_dup, y_coor)] << " " << index_v_map[std::make_pair(max_x_dup, y_coor)] << "\n";
                            // std::cout<< max_x << "," << y_coor<< std::endl;
                            // std::cout<< min_x << "," << y_coor<< std::endl;
                            // std::cout<< max_x_dup << "," << y_coor<< std::endl;
                            // std::cout<< min_x_dup << "," << y_coor<< std::endl;
                            
                            not_cut_edge.emplace_back(std::make_pair(index_v_map[std::make_pair(min_x_dup, y_coor)], index_v_map[std::make_pair(max_x_dup, y_coor)]));

                            
                        }
                        
                    }
                    
            
                        
                
                        
                        
                        
                        
                    
          
                }
            }
            
 
            xs.clear();
            ys.clear();
            grouped_vertices_x.clear();
            grouped_vertices_y.clear();
            vertices.clear();
        }
        // std::vector<std::pair<int, int>> dup_edges;
        // for (int i = 0; i < obs_edges.size(); i++){
        //     if (std::find(dup_edges.begin(), dup_edges.end(), obs_edges[i]) == dup_edges.end()){
        //         if (std::find(dup_edges.begin(), dup_edges.end(), std::make_pair(obs_edges[i].second, obs_edges[i].first)) == dup_edges.end()){
        //             dup_edges.emplace_back(obs_edges[i]);
        //         } else {
        //             cut_edges.emplace_back(obs_edges[i]);
        //         }
        //     } else {
        //         cut_edges.emplace_back(obs_edges[i]);
        //     }
        // }
    

        

            
        

    }
    std::sort(cut_edges.begin(), cut_edges.end());
    auto last = std::unique(cut_edges.begin(), cut_edges.end());
    cut_edges.erase(last, cut_edges.end());
    
    std::pair<std::pair<std::vector<std::pair<int,int>>,std::vector<Obstacles>>,std::vector<Vertex>> partition_result = std::make_pair(std::make_pair(cut_edges, obstacles), obs_vertices);
    return partition_result;    

}

void Input::printEdges(std::vector<std::pair<int, int>>& cut_edges){
    std::cout << "{ ";
    for (int i = 0; i < cut_edges.size(); i++){
        std::cout << "(" << cut_edges[i].first << "," << cut_edges[i].second << "), " ;
    }
    std::cout << " }";
}

void Input::printObstacles(std::vector<Obstacles>& obstacles){
    for (int i = 0; i < obstacles.size(); i++){
        std::cout << "obstacleid : " << obstacles[i].obstacleid << '\t';
        std::cout << "start, end : " << "(" <<  obstacles[i].start.first << "," << obstacles[i].start.second << ") "<< "(" <<  obstacles[i].end.first << "," << obstacles[i].end.second << ") " << "\t";
        std::cout << "vertices : " << obstacles[i].c1 << " " << obstacles[i].c2 << " " << obstacles[i].c3 << " " << obstacles[i].c4 << std::endl;
    }
}

void Input::printVertex(std::vector<Vertex>& vertices){
    std::cout<< "{ ";
    for (int i = 0; i < vertices.size(); i++){
        std::cout << vertices[i].id << ": ";
        std::cout << "(" <<  vertices[i].coord.first << "," << vertices[i].coord.second << "), ";
    }
    std::cout<< "}";
}

std::vector<std::pair<int, int>> Input::outputEdges(){
    for (int i = 0; i < cut_edges.size() ; i++){
        for (int j = 0; j < output_edges.size(); j++){
            if (cut_edges[i].first == output_edges[j].first and  cut_edges[i].second == output_edges[j].second){
                output_edges.erase(output_edges.begin()+j);
            }
            if (cut_edges[i].second == output_edges[j].first and  cut_edges[i].first == output_edges[j].second){
                output_edges.erase(output_edges.begin()+j);
            }
        }
    }
    return output_edges;
}
