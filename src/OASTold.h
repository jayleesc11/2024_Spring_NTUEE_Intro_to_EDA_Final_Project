#include<iostream>
#include<set>
#include<list>
#include<numeric>
#include<algorithm>
#include <queue>
#include <functional>
#include <limits>
#include<stdio.h>
#include<cmath>
#include<vector>

#include"input.h"
using namespace std;

void printOAST(vector<Vertex> &vs, vector<vector<int>> &adj_list)
{
    cout << "Edges:" << endl;
    for(int i = 0; i < adj_list.size(); i++) {
        for(const auto& edge : adj_list[i]) {
            if(i < edge) {
                cout << "(" << i << "," <<  edge << "), ";
            }
        }
    }
    cout << endl << endl << "Vertices: " << endl;
    for(int i = 0; i < vs.size(); i++) {
        cout << fixed;
        cout.precision(0);
        cout << i << ": (" << vs[i].coord.first << "," << vs[i].coord.second << "), ";
    }
    cout << endl;
}


/*struct Vertex {
    int id;                     // Index in the vector<Vertex> vertices
    std::pair<float, float> coord;   // Position
    bool pin = false;           // False if it is an obstacle vertex

    Vertex(int id, std::pair<float, float> c, bool p)
        : id(id), coord(c), pin(p) {}
};*/
vector<vector<int>> prim(const vector<vector<int>>& adj_list, const vector<vector<float>>& weight) ;
void dijkstra(const vector<vector<int>>& adj_list, const vector<vector<float>>& weight,int start, vector<float> &dist,vector<int> &parent);
/*vector<pair<float,float>> OARST(vector<vertex> vertexes, vector<vector<int>>adj_list, vector<int> shortest_path)
{
    int i = 0;
    vector<float> edge;
    vector<v> steiner_points;
    bool *used ;
    used = new bool [shortest_path.size()];
    for(int i=0; i<shortest_path.size()-1; i++)
    {
        edge[i] = abs(vertexes[shortest_path[i]].coord.x-vertexes[shortest_path[i+1]].coord.x) + 
                  abs(vertexes[shortest_path[i]].coord.y-vertexes[shortest_path[i+1]].coord.y);        
    }
    vector <size_t> indices( edge.size(), 0 );
    iota( indices.begin(), indices.end(), 0 );  // found in <numeric>
    sort( indices.begin(), indices.end(), compare );
    while(!indices.empty())
    {
        if(vertexes[shortest_path[indices[0]]].coord.x == vertexes[shortest_path[indices[0]+1]].coord.x
        || vertexes[shortest_path[indices[0]]].coord.y == vertexes[shortest_path[indices[0]+1]].coord.y)
            used[i] = 1;
        else
        {
            
            if(indices[i] == 0 || indices[i] == edge.size())
            {

            }
            else
            {
                v v1, v2, v3, v4;
                v1.x = vertexes[shortest_path[indices[0]-1]].coord.x;
                v2.x = vertexes[shortest_path[indices[0]]].coord.x;
                v3.x = vertexes[shortest_path[indices[0]+1]].coord.x;
                v4.x = vertexes[shortest_path[indices[0]+2]].coord.x;
                v1.y = vertexes[shortest_path[indices[0]-1]].coord.y;
                v2.y = vertexes[shortest_path[indices[0]]].coord.y;
                v3.y = vertexes[shortest_path[indices[0]+1]].coord.y;
                v4.y = vertexes[shortest_path[indices[0]+2]].coord.y;


                float shared_path1 = shared_path(v1.x, v2.x, v3.x) +  shared_path(v1.x,v2.x,v3.x);
                float shared_path2 = shared_path(v1.y, v2.y, v3.y) +  shared_path(v1.y,v2.y,v3.y);
                if(shared_path1 < shared_path2)
                {
                    v1.x = v2.x;
                    v2.x = v3.x;
                    v3.x = v4.x;
                    v1.y = v2.y;
                    v2.y = v3.y;
                    v3.y = v4.y;
                }
                v tmp, tmp2;
                if(!shared_path1||(used[indices[i]+1]&&used[indices[i]-1]) )
                {
                    tmp.x = v2.x;
                    tmp.y = v3.y;
                    steiner_points.push_back(tmp);
                }
                else if((v1.x-v2.x)*(v3.x-v2.x)>=0 && (v1.y-v2.y)*(v3.y-v2.y)>0)
                {
                    if(abs(v1.x-v2.x) + abs(v3.y-v2.y) < abs(v1.y-v2.y) + abs(v3.x-v2.x))
                    {
                        tmp.x = v1.x;
                        tmp.y = v3.y;
                        steiner_points.push_back(tmp);
                    }
                    else
                    {
                        tmp.x = v3.x;
                        tmp.y = v1.y;
                        steiner_points.push_back(tmp);
                    }
                    tmp2.x = v2.x;
                    tmp2.y = tmp.y;
                    steiner_points.push_back(tmp2);
                        
                }
                else
                {
                    if((v3.x-v2.x)*(v1.x-v2.x)<0)
                    {
                        tmp.x = v2.x;
                        tmp.y = v1.y;
                        tmp2.x = v2.x;
                        tmp2.y = v3.y;
                    }
                    else
                    {
                        tmp.y = v2.y;
                        tmp.x = v1.x;
                        tmp2.y = v2.y;
                        tmp2.x = v3.x;
                    }
                    steiner_points.push_back(tmp);
                    steiner_points.push_back(tmp2);
                }
                
            }
            

        }
    }
    
}*/
/*struct Vertex
{
    pair<float,float> coord;
    bool pin;
};*/
vector<vector<int>> OAST(vector<Vertex> vertexes, vector<vector<int>> adj_list)
{   
    //vertex coor (pair) pin 1 for pin 0 for obstacle
    //printf("size = %d, adj size = %d",vertexes.size(),adj_list.size());
    vector<vector<float>> weight(adj_list.size());
    vector<vector<int>> shortest_path;
    vector<int> prev(adj_list.size());
    vector<float> dist(adj_list.size());
    int vertex_num  = adj_list.size();
    int pin_count=0;
    vector<int> pin;
    for(int i=0; i<vertex_num; i++)
    {
        if(vertexes[i].pin)
            pin.push_back(i);
    }
    
   

    for(int i=0; i<vertex_num; i++)
    {
        if(vertexes[i].pin)
        {
            pin_count++;
           
        }
    }
    
    for(int i=0; i<adj_list.size(); i++)
    {
        for(int j=0; j<adj_list[i].size(); j++)
        {
            weight[i].push_back(fabsf(vertexes[i].coord.first - vertexes[adj_list[i][j]].coord.first) + fabsf(vertexes[i].coord.second - vertexes[adj_list[i][j]].coord.second));
            
            
        }
    }

    vector<vector<int>> pin_adj_list(pin_count);
    vector<vector<float>> pin_weight(pin_count);
    int c1=0 ,c2 = 0;
    for(int i=0; i<vertex_num; i++)
    {
       
        dijkstra(adj_list,weight,i,dist,prev);
       
        c2 = 0;
        for(int j= 0;j<vertex_num; j++)
        {
            
            if(vertexes[i].pin && vertexes[j].pin)
            {
                if(i!=j)
                {
                    
                pin_weight[c1].push_back(dist[j]);
                
                pin_adj_list[c1].push_back(c2);
                }
                c2++;

            }
        }
        if(vertexes[i].pin) c1++;
    }
    for(int i=0; i<pin_adj_list.size();i++)
    {
    //   printf("%d\n", mst_adj_list[i].size());
        for(int j=0; j<pin_weight[i].size(); j++)
        {
            
        //    printf("i = %d j = %f, ",i,pin_weight[i][j]);
        }
    //    printf("\n");
    }
    vector<vector<int>> mst_adj_list = prim(pin_adj_list,pin_weight);
        for(int i=0; i<mst_adj_list.size();i++)
    {
    //   printf("%d\n", mst_adj_list[i].size());
        for(int j=0; j<mst_adj_list[i].size(); j++)
        {
            
        //    printf("i = %d j = %d, ",i,mst_adj_list[i][j]);
        }
    //    printf("\n");
    }
    
    
    vector<int> pin_index;
    
    for(int i=0; i<vertex_num; i++)
    {
        if(vertexes[i].pin)
        {
            pin_index.push_back(i);
        //    printf("%d ",i);
            
        }
    }
    
    vector<vector<int>> d_adj_list(vertex_num);
    int counter=0;
    bool *used ;
    used = new bool[vertex_num];
    for(int i=0; i<vertex_num; i++)
        used[i] = 0;
    
    for(int i=0; i<vertex_num; i++)
    {
            if(vertexes[i].pin)
            {
                
            dijkstra(adj_list,weight,i,dist,prev);
            for(int j=0; j<mst_adj_list[counter].size();j++)
            {
            //    printf("here\n");
                int prev_node = pin_index[mst_adj_list[counter][j]];
                if(!used[prev_node])
                {
                    
                    while(prev[prev_node]!=i && prev_node!=i)
                    {
                        d_adj_list[prev_node].push_back(prev[prev_node]);
                        d_adj_list[prev[prev_node]].push_back(prev_node);
                    //   printf("i = %d prev_node = %d, prev = %d\n",i,prev_node,prev[prev_node]);
                        prev_node =prev[prev_node];
                    }
                    if(prev_node!=i)
                    {
                        d_adj_list[prev_node].push_back(i);
                        d_adj_list[i].push_back(prev_node);
                    }
                }
                
            }   
            counter++;
        }
        used[i] = 1;
    }
    //return d_adj_list;
    //("here");
    for(int i=0; i<vertex_num; i++)
    {
        for(int j=0; j<adj_list[i].size(); j++)
        {
            if(!d_adj_list[adj_list[i][j]].empty() &&!d_adj_list[i].empty()&& find(d_adj_list[i].begin(),d_adj_list[i].end(),adj_list[i][j]) ==d_adj_list[i].end())
            {
                d_adj_list[i].push_back(adj_list[i][j]);
            }
        }
    }
    
    vector<vector<float>>d_weight(d_adj_list.size());
    for(int i=0; i<d_adj_list.size();i++)
    {
    //   printf("%d\n", mst_adj_list[i].size());
        for(int j=0; j<d_adj_list[i].size(); j++)
        {
            
        //    printf("i = %d j = %d, ",i,d_adj_list[i][j]);
        }
    //    printf("\n");
    }
    
    for(int i=0; i<d_adj_list.size(); i++)
    {
        for(int j=0; j<d_adj_list[i].size(); j++)
        {
            d_weight[i].push_back(fabsf(vertexes[i].coord.first - vertexes[d_adj_list[i][j]].coord.first) + fabsf(vertexes[i].coord.second - vertexes[d_adj_list[i][j]].coord.second));
        }
    }
    for(int i=0; i<d_weight.size();i++)
    {
    //   printf("%d\n", mst_adj_list[i].size());
        for(int j=0; j<d_weight[i].size(); j++)
        {
            
        //    printf("i = %d j = %f, ",i,d_weight[i][j]);
        }
      //  printf("\n");
    }
    vector<vector<int>> final_adj_list = prim(d_adj_list, d_weight);
    
    for(int i=0; i<final_adj_list.size();i++)
    {
    //   printf("%d\n", mst_adj_list[i].size());
        for(int j=0; j<final_adj_list[i].size(); j++)
        {
            
        //    printf("final i = %d j = %d, ",i,final_adj_list[i][j]);
        }
    //    printf("\n");
    }
    
    return final_adj_list;

}


bool compare(int a, int b, float* data)
{
    return data[a]>data[b];
}
float shared_path(float a, float b ,float c)
{
    if ((a>b & b >c)||(a<b) & (b<c))
    {
        return 0;
    }
    else 
    {
        return abs(abs(a-b) - abs(b-c));
    }
}
struct v
{
    float x;
    float y;
};
/*vector<vector<int>> prim(vector<vector<int>> adj_list, vector<vector<float>> weight,int n)
{
    vector<int> visited_list;
    vector<vector<int>> adj_list_out(n,vector<int>(0));
    bool *visited = new bool [n];
    visited[0] = 1;
    visited_list.push_back(0);
    while(visited_list.size()!=n)
    {
        int min = 1000000000;
        int min_node, node2;
        for(int i=0; i<visited_list.size(); i++)
        {
            for(int j=0; j<adj_list.size(); j++)
            {
                if(weight[visited_list[i]][adj_list[visited_list[i]][j]]<min && !visited[adj_list[visited_list[i]][j]])
                {
                    min = weight[visited_list[i]][adj_list[visited_list[i]][j]];
                    min_node = adj_list[visited_list[i]][j];
                    node2 = visited_list[i];
                }
                
            }
        }
        visited[min_node] = 1;
        visited_list.push_back(min_node);
        adj_list[min_node].push_back(node2);
        adj_list[node2].push_back(min_node);
    }
    return adj_list;
}*/

typedef pair<float, int> pdi; 
void dijkstra(const vector<vector<int>>& adj_list, const vector<vector<float>>& weight, int start, vector<float> &dist,vector<int> &parent)
 {
    int n = adj_list.size();
    //printf("%d size\n",n);
    for(int i=0; i<n; i++)
    {
        dist[i] = numeric_limits<float>::infinity();
        parent[i] = -1;
    }
    priority_queue<pdi, vector<pdi>, greater<pdi>> pq;  

    
    dist[start] = 0.0;
    pq.push({0.0, start});

    while (!pq.empty()) {
        float current_dist = pq.top().first;
        int u = pq.top().second;
        pq.pop();

        
        if (current_dist > dist[u])
            continue;

        for (size_t i = 0; i < adj_list[u].size(); ++i) {
            int v = adj_list[u][i];
            float weight_uv = weight[u][i];

           
            if (dist[u] + weight_uv < dist[v]) {
                dist[v] = dist[u] + weight_uv;
                parent[v] = u;  
                pq.push({dist[v], v});
            }
        }
    }

    

    
}

vector<vector<int>> prim(const vector<vector<int>>& adj_list, const vector<vector<float>>& weight) 
{
    int n = adj_list.size(); 
    vector<bool> inMST(n, false); 
    vector<float> minWeight(n, numeric_limits<float>::infinity()); 
    priority_queue<pdi, vector<pdi>, greater<pdi>> pq; 
    vector<int> parent(n, -1); 
    int tmp=0;
    for(int i=0; i<adj_list.size();i++)
    {
        if(adj_list[i].size())
        {
            for(int j=0; j<adj_list[i].size(); j++)
            {
    //            printf("nearby edges = %d\n",adj_list[i][j]);
            }
            tmp = i;
    //        printf("pin id = %d", tmp);
            break;
        }
            
    }
    
    minWeight[tmp] = 0.0;
    pq.push({0.0, tmp});
    
    while (!pq.empty()) {
    //    float weight = pq.top().first;
        int u = pq.top().second;
    //    printf("%d", u);
        pq.pop();
        
        
        if (inMST[u]) continue;
        
        
        inMST[u] = true;
        
        
        for (int i = 0; i < adj_list[u].size(); ++i) {
            int v = adj_list[u][i];
            float w = weight[u][i];
        //    printf("traverse to %d\n",adj_list[u][i]);
        //    printf("!inMST = %d, w = %f, minweight = %f\n",!inMST[v],w,minWeight[v]);
            
            if (!inMST[v] && w < minWeight[v]) {
                minWeight[v] = w;
                pq.push({w, v});
                parent[v] = u;
            }
        }
    }
    
    
    vector<vector<int>> mst_adj_list(n);
    for (int v = 1; v < n; ++v) {
        
        if (parent[v] != -1) {
            
            mst_adj_list[v].push_back(parent[v]);
            mst_adj_list[parent[v]].push_back(v);
        }
    }
    
    return mst_adj_list;
}
/*int main()
{
    
    vector<Vertex> v(12);
    vector<vector<int>> adj_list(12);
    v[0].coord.first = 0;
    v[0].coord.second = 5;
    v[0].pin = 0;

    v[1].coord.first = 0;
    v[1].coord.second = 6;
    v[1].pin = 0;

    v[2].coord.first = 1;
    v[2].coord.second = 2;
    v[2].pin = 1;

    v[3].coord.first = 2;
    v[3].coord.second = 0;
    v[3].pin = 0;

    v[4].coord.first = 2;
    v[4].coord.second = 4;
    v[4].pin = 0;

    v[5].coord.first = 2;
    v[5].coord.second = 7;
    v[5].pin = 1;

    v[6].coord.first = 3;
    v[6].coord.second = 0;
    v[6].pin = 0;

    v[7].coord.first = 3;
    v[7].coord.second = 4;
    v[7].pin = 0;

    v[8].coord.first = 4;
    v[8].coord.second = 3;
    v[8].pin = 1;

    v[9].coord.first = 6;
    v[9].coord.second = 5;
    v[9].pin = 0;

    v[10].coord.first = 6;
    v[10].coord.second = 6;
    v[10].pin = 0;

    v[11].coord.first = 7;
    v[11].coord.second = 2;
    v[11].pin = 1;

    adj_list[0].push_back(1);
    adj_list[0].push_back(2);
    adj_list[0].push_back(4);
    adj_list[1].push_back(5);
    adj_list[1].push_back(0);
    adj_list[2].push_back(0);
    adj_list[2].push_back(3);
    adj_list[2].push_back(4);
    adj_list[3].push_back(2);
    adj_list[3].push_back(6);
    adj_list[4].push_back(2);
    adj_list[4].push_back(3);
    adj_list[4].push_back(7);
    adj_list[5].push_back(10);
    adj_list[5].push_back(1);
    adj_list[6].push_back(3);
    adj_list[6].push_back(8);
    adj_list[6].push_back(11);
    adj_list[7].push_back(4);
    adj_list[7].push_back(6);
    adj_list[7].push_back(8);
    adj_list[7].push_back(9);
    adj_list[8].push_back(6);
    adj_list[8].push_back(7);
    adj_list[8].push_back(9);
    adj_list[8].push_back(11);
    adj_list[9].push_back(7);
    adj_list[9].push_back(8);
    adj_list[9].push_back(10);
    adj_list[9].push_back(11);
    adj_list[10].push_back(5);
    adj_list[10].push_back(9);
    adj_list[11].push_back(9);
    adj_list[11].push_back(8);
    adj_list[11].push_back(6);

    vector<vector<int>> sol = OAST(v,adj_list);
  
    return 0;
}
*/
