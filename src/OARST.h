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

void printOARST(vector<Vertex> &vs, vector<vector<int>> &adj_list)
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

struct vertex
{
    pair<double,double> coord;
    bool pin;
};
struct edge
{
    Vertex v1, v2;
    int idx1, idx2;
    edge(Vertex v1, Vertex v2, int idx1, int idx2)
    : v1(v1), v2(v2), idx1(idx1), idx2(idx2) {}
};
double shared_path(Vertex a, Vertex b , Vertex c)
{
    double sx, sy;
    if(fabs(a.coord.first-b.coord.first) + fabs(c.coord.first-b.coord.first) == fabs(c.coord.first-a.coord.first) )
        sx = 0;
    else 
        sx = min(fabs(a.coord.first-b.coord.first),fabs(c.coord.first-b.coord.first));

    if(fabs(a.coord.second-b.coord.second) + fabs(c.coord.second-b.coord.second) == fabs(c.coord.second-a.coord.second) )
        sy = 0;
    else 
        sy = min(fabs(a.coord.second-b.coord.second),fabs(c.coord.second-b.coord.second));

    return sx + sy;

    
}
double edgelength(const edge& e)
{
    return fabs(e.v1.coord.first - e.v2.coord.first) + fabs(e.v1.coord.second - e.v2.coord.second);
}
bool compare_edge(const edge& e1, const edge& e2)
{
    return edgelength(e1)>edgelength(e2);
}

vector<vector<int>> OARST(vector<Vertex>& vs, vector<vector<int>> adj_list, const vector<vector<int>>& congestion_map, float cell_width, bool heuristic, int threshold)
{
    vector<edge>  A;
    vector<Vertex> outv;
    vector<vector<int>> out_adj_list(adj_list.size());
    
    for(int i=0; i<adj_list.size(); i++)
    {
        for(int j=0; j<adj_list[i].size(); j++)
        {
            if(i<adj_list[i][j])
            {
                Vertex t1(0,{0,0},0),t2(0,{0,0},0);
                edge tmp(t1,t2,0,0);
                tmp.idx1 = i;
                tmp.idx2 = adj_list[i][j];
                tmp.v1 = vs[i];
                tmp.v2 = vs[adj_list[i][j]];
                A.push_back(tmp);
            }
        }
    }
    for(int i=0; i<A.size();i++)
        {
        //    printf("i = %d, (%f,%f) (%f,%f)\n",i,A[i].v1.coord.first,A[i].v1.coord.second,A[i].v2.coord.first,A[i].v2.coord.second);
        }
    for(int i=0; i<A.size(); i++)
    {
        edge e = A[i];
        
    }
    while(!A.empty())
    {
       // printf("up\n");
        
            sort(A.begin(),A.end(),compare_edge);
            for(int i=0; i<A.size();i++)
        {
        //    printf("first i = %d, (%f,%f) (%f,%f)\n",i,A[i].v1.coord.first,A[i].v1.coord.second,A[i].v2.coord.first,A[i].v2.coord.second);
        }
       // printf("down\n");
        
        edge e = A.front();
        
        Vertex v1(0,{0,0},0), v2(0,{0,0},0), v3(0,{0,0},0);
        int vdx1, vdx2, vdx3;
        Vertex t1(0,{0,0},0),t2(0,{0,0},0);
        edge e2(t1,t2,0,0);
        int erase_index;
        A.erase(A.begin());
        for(int i=0; i<A.size();i++)
        {
        //    printf("second i = %d, (%f,%f) (%f,%f)\n",i,A[i].v1.coord.first,A[i].v1.coord.second,A[i].v2.coord.first,A[i].v2.coord.second);
        }
        for(int i=0; i<A.size();i++)
        {
        //     printf("i = %d, (%f,%f) (%f,%f)\n",i,A[i].v1.coord.first,A[i].v1.coord.second,A[i].v2.coord.first,A[i].v2.coord.second);
        }
        if(e.v1.coord.first==e.v2.coord.first || e.v1.coord.second==e.v2.coord.second)
        {
        //    printf("same : (%f,%f),(%f,%f)\n",e.v1.coord.first, e.v1.coord.second,e.v2.coord.first,e.v2.coord.second);
            out_adj_list[e.idx1].push_back(e.idx2);
            out_adj_list[e.idx2].push_back(e.idx1);
            continue;
        }
        else
        {
            if((adj_list[e.idx1].size()==1 &&adj_list[e.idx2].size()==1 )|| A.size()==1)
            {
                v1 = e.v1;
                vdx1 = e.idx1;
                v2 = e.v2;
                vdx2 = e.idx2;
            }
            double max_s = -1;
            int idx1, idx2;
            for(int i=0; i<adj_list[e.idx1].size(); i++)
            {
                if(adj_list[e.idx1][i]!=e.idx2)
                {
                    double s;
                    s = shared_path(e.v2,e.v1,vs[adj_list[e.idx1][i]]);
                    if(s>max_s)
                    {
                        
                        v1 = e.v2;
                        vdx1 = e.idx2;
                        v2 = e.v1;
                        vdx2 = e.idx1;
                        v3 = vs[adj_list[e.idx1][i]];
                        vdx3 = adj_list[e.idx1][i];
                        max_s = s;
                        idx1 = adj_list[e.idx1][i];
                        idx2 = e.idx1;
                        
                    }
                }
            }
            for(int i=0; i<adj_list[e.idx2].size(); i++)
            {
                if(adj_list[e.idx2][i]!=e.idx1)
                {

                    double s;
                    s = shared_path(e.v1,e.v2,vs[adj_list[e.idx2][i]]);
                    if(s>max_s)
                    {
                        
                        v1 = e.v1;
                        vdx1 = e.idx1;
                        v2 = e.v2;
                        vdx2 = e.idx2;
                        v3 = vs[adj_list[e.idx2][i]];
                        vdx3 = adj_list[e.idx2][i];
                        max_s = s;
                        idx1 = adj_list[e.idx2][i];
                        idx2 =  e.idx2;
                        
                    }
                }
            }
            erase_index = -1;
            if(idx1<idx2)
            {
                for(int i=0; i<A.size(); i++)
                {
                    if(A[i].idx1==idx1 && A[i].idx2==idx2)
                    {
                        e2 = A[i];
                        erase_index = i;
                    }
                }
            }
            else
            {
                for(int i=0; i<A.size(); i++)
                {
                    if(A[i].idx2==idx1 && A[i].idx1==idx2)
                    {
                        e2 = A[i];
                        erase_index = i;
                    }
                }
            }
        //   printf("e2 v1 = (%lf,%lf) v2 = (%lf,%lf) v3 = (%lf,%lf)\n",v1.coord.first, v1.coord.second,v2.coord.first, v2.coord.second,v3.coord.first, v3.coord.second);

            if(((A.size()==0)||max_s==-1 || ((v1.coord.first-v2.coord.first)*(v2.coord.first-v3.coord.first)>=0 && (v1.coord.second-v2.coord.second)*(v2.coord.second-v3.coord.second)>=0))||!heuristic)
            {
            //    printf("case1\n");
            float overflow1 = 0, overflow2 = 0;
            Vertex tmp(0,{0,0},0);
            int c1x = v1.coord.first/cell_width;
            int c1y = v1.coord.second/cell_width;
            int c2x = v2.coord.first/cell_width;
            int c2y = v2.coord.second/cell_width;
            if(c1x>=c2x && c1y>=c2y)
            {
                for(int m = c2x; m<=c1x; m++)
                {
                    if(congestion_map[m][c2y]>threshold)
                        overflow1 += congestion_map[m][c2y];
                }
                for(int m = c2y; m<=c1y;m++)
                {
                    if(congestion_map[c1x][m]>threshold)
                        overflow1 += congestion_map[c1x][m];
                }
                for(int m = c2x; m<=c1x; m++)
                {
                    if(congestion_map[m][c1y]>threshold)
                        overflow2 += congestion_map[m][c1y];
                }
                for(int m = c2y; m<=c1y;m++)
                {
                    if(congestion_map[c2x][m]>threshold)
                        overflow2 += congestion_map[c2x][m];
                }
                if(overflow1>overflow2)
                {
                    tmp.coord.first = v2.coord.first;
                    tmp.coord.second= v1.coord.second;
                }
                else
                {
                    tmp.coord.first = v1.coord.first;
                    tmp.coord.second= v2.coord.second;
                }

            }
            else if(c1x<=c2x && c1y>=c2y)
            {
                for(int m = c1x; m<=c2x; m++)
                {
                    if(congestion_map[m][c2y]>threshold)
                        overflow1 += congestion_map[m][c2y];
                }
                for(int m = c2y; m<=c1y;m++)
                {
                    if(congestion_map[c1x][m]>threshold)
                        overflow1 += congestion_map[c1x][m];
                }
                for(int m = c1x; m<=c2x; m++)
                {
                    if(congestion_map[m][c1y]>threshold)
                        overflow2 += congestion_map[m][c1y];
                }
                for(int m = c2y; m<=c1y;m++)
                {
                    if(congestion_map[c2x][m]>threshold)
                        overflow2 += congestion_map[c2x][m];
                }
                if(overflow1>overflow2)
                {
                    tmp.coord.first = v2.coord.first;
                    tmp.coord.second= v1.coord.second;
                }
                else
                {
                    tmp.coord.first = v1.coord.first;
                    tmp.coord.second= v2.coord.second;
                }
            }
            else if(c1x<=c2x && c1y<=c2y)
            {
                for(int m = c1x; m<=c2x; m++)
                {
                    if(congestion_map[m][c1y]>threshold)
                        overflow1 += congestion_map[m][c1y];
                }
                for(int m = c1y; m<=c2y;m++)
                {
                    if(congestion_map[c2x][m]>threshold)
                        overflow1 += congestion_map[c2x][m];
                }
                for(int m = c1x; m<=c2x; m++)
                {
                    if(congestion_map[m][c2y]>threshold)
                        overflow2 += congestion_map[m][c2y];
                }
                for(int m = c1y; m<=c2y;m++)
                {
                    if(congestion_map[c1x][m]>threshold)
                        overflow2 += congestion_map[c1x][m];
                }
                if(overflow1>overflow2)
                {
                    tmp.coord.first = v1.coord.first;
                    tmp.coord.second= v2.coord.second;
                }
                else
                {
                    tmp.coord.first = v2.coord.first;
                    tmp.coord.second= v1.coord.second;
                }
            }
            else
            {
                
                for(int m = c2x; m<=c1x; m++)
                {
                    if(congestion_map[m][c1y]>threshold)
                        overflow1 += congestion_map[m][c1y];
                }
                for(int m = c1y; m<=c2y;m++)
                {
                    if(congestion_map[c2x][m]>threshold)
                        overflow1 += congestion_map[c2x][m];
                }
                for(int m = c2x; m<=c1x; m++)
                {
                    if(congestion_map[m][c2y]>threshold)
                        overflow2 += congestion_map[m][c2y];
                }
                for(int m = c1y; m<=c2y;m++)
                {
                    if(congestion_map[c1x][m]>threshold)
                        overflow2 += congestion_map[c1x][m];
                }
                if(overflow1>overflow2)
                {
                    tmp.coord.first = v1.coord.first;
                    tmp.coord.second= v2.coord.second;
                }
                else
                {
                    tmp.coord.first = v2.coord.first;
                    tmp.coord.second= v1.coord.second;
                }
            }
               
            //   tmp.coord.first = v1.coord.first;
            //   tmp.coord.second= v2.coord.second;
               vector<Vertex> tmpp;
               tmpp.push_back(tmp);
               vs.push_back(tmp);
               vector<int> tmp2;
               tmp2.push_back(e.idx1);
               tmp2.push_back(e.idx2);
               out_adj_list.push_back(tmp2);
               out_adj_list[e.idx1].push_back(out_adj_list.size()-1);
               out_adj_list[e.idx2].push_back(out_adj_list.size()-1);
               out_adj_list[out_adj_list.size()-1].push_back(e.idx2);
               out_adj_list[out_adj_list.size()-1].push_back(e.idx1);


            }
            else if((v1.coord.first-v2.coord.first)*(v2.coord.first-v3.coord.first)<=0 && (v1.coord.second-v2.coord.second)*(v2.coord.second-v3.coord.second)<0)
            {
                if(erase_index!=-1)
                A.erase(A.begin()+erase_index);
                if(fabs(v1.coord.first-v2.coord.first)<(v2.coord.first-v3.coord.first))
                {
                //    printf("case3\n");
                    Vertex tmp(0,{0,0},0),tmp1(0,{0,0},0);
                    tmp.coord.first = v1.coord.first;
                    tmp.coord.second= v2.coord.second;
                    vs.push_back(tmp);
                    tmp1.coord.first = v1.coord.first;
                    tmp1.coord.second= v3.coord.second;
                    vs.push_back(tmp1);
                    vector<int> tmp2, tmp3;
                    tmp2.push_back(vdx2);
                    tmp2.push_back(vs.size()-1);
                    tmp3.push_back(vs.size()-2);
                    tmp3.push_back(vdx3);
                    tmp3.push_back(vdx1);
                    out_adj_list.push_back(tmp2);
                    out_adj_list.push_back(tmp3);
                    out_adj_list[vdx2].push_back(vs.size()-2);
                    out_adj_list[vdx1].push_back(vs.size()-1);
                    out_adj_list[vdx3].push_back(vs.size()-1);
                }
                else
                {
                //    printf("case3\n");
                    Vertex tmp(0,{0,0},0),tmp1(0,{0,0},0);
                    tmp.coord.first = v3.coord.first;
                    tmp.coord.second= v2.coord.second;
    
                    vs.push_back(tmp);
                    tmp1.coord.first = v3.coord.first;
                    tmp1.coord.second= v1.coord.second;
                    
                    vs.push_back(tmp1);
                    vector<int> tmp2, tmp3;
                    tmp2.push_back(vdx2);
                    tmp2.push_back(vs.size()-1);
                    tmp3.push_back(vs.size()-2);
                    tmp3.push_back(vdx3);
                    tmp3.push_back(vdx1);
                    out_adj_list.push_back(tmp2);
                    out_adj_list.push_back(tmp3);
                    out_adj_list[vdx2].push_back(vs.size()-2);
                    out_adj_list[vdx1].push_back(vs.size()-1);
                    out_adj_list[vdx3].push_back(vs.size()-1);
                }
            }
            else
            {
            //    printf("case2end size = %d\n",A.size());
                if(erase_index!=-1)
                A.erase(A.begin()+erase_index);
                if((v3.coord.first-v2.coord.first)*(v1.coord.first-v2.coord.first)<0)
                {
            //        printf("r1\n");
                    Vertex tmp(0,{0,0},0),tmp1(0,{0,0},0);
                    tmp.coord.first = v2.coord.first;
                    tmp.coord.second= v1.coord.second;
                    vs.push_back(tmp);
                    tmp1.coord.first = v2.coord.first;
                    tmp1.coord.second= v3.coord.second;
                    vs.push_back(tmp1);
                    vector<int> tmp2, tmp3;
                    tmp2.push_back(vdx2);
                    tmp2.push_back(vdx1);
                    tmp3.push_back(vdx2);
                    tmp3.push_back(vdx3);
                    out_adj_list.push_back(tmp2);
                    out_adj_list.push_back(tmp3);
                    out_adj_list[vdx2].push_back(vs.size()-2);
                    out_adj_list[vdx2].push_back(vs.size()-1);
                    out_adj_list[vdx1].push_back(vs.size()-2);
                    out_adj_list[vdx3].push_back(vs.size()-1);
            //        printf("r1 end\n");
                }
                else
                {
            //        printf("r2 end size = %d\n",A.size());
                    Vertex tmp(0,{0,0},0),tmp1(0,{0,0},0);
                    tmp.coord.first = v1.coord.first;
                    tmp.coord.second= v2.coord.second;
                    vs.push_back(tmp);
                    tmp1.coord.first = v3.coord.first;
                    tmp1.coord.second= v2.coord.second;
                    vs.push_back(tmp1);
                    vector<int> tmp2, tmp3;
                    tmp2.push_back(vdx2);
                    tmp2.push_back(vdx1);
                    tmp3.push_back(vdx2);
                    tmp3.push_back(vdx3);
                    out_adj_list.push_back(tmp2);
                    out_adj_list.push_back(tmp3);
                    out_adj_list[vdx2].push_back(vs.size()-2);
                    out_adj_list[vdx2].push_back(vs.size()-1);
                    out_adj_list[vdx1].push_back(vs.size()-2);
                    out_adj_list[vdx3].push_back(vs.size()-1);
                //    printf("r2 end size = %d\n",A.size());
                }
            }
            

        }
    }
    return out_adj_list;

}
/*int main()
{
    vector<vector<int>> solve;
    vector<Vertex> v(7);
    vector<vector<int>> adj_list(7);
    v[0].coord.first = 0;
    v[0].coord.second = 4;

    v[1].coord.first = 1;
    v[1].coord.second = 1;

    v[2].coord.first = 2;
    v[2].coord.second = 3;

    v[3].coord.first = 3;
    v[3].coord.second = 3;

    v[4].coord.first = 4;
    v[4].coord.second = 2;

    v[5].coord.first = 5;
    v[5].coord.second = 0;

    v[6].coord.first = 7;
    v[6].coord.second = 1;

    adj_list[0].push_back(2);
    adj_list[1].push_back(2);
    adj_list[2].push_back(0);
    adj_list[2].push_back(1);
    adj_list[2].push_back(3);
    adj_list[3].push_back(2);
    adj_list[3].push_back(4);
    adj_list[4].push_back(3);
    adj_list[4].push_back(5);
    adj_list[4].push_back(6);
    adj_list[5].push_back(4);
    adj_list[6].push_back(4);

    solve = OARST(v,adj_list);
    for(int i=0; i<solve.size(); i++)
    {
        for(int j=0; j<solve[i].size();j++)
        {
            printf("i = %d j = %d",i,solve[i][j]);
        }
        printf("\n");
    }
    for(int i=0; i<v.size(); i++)
    {
        printf("i = %d x = %lf y = %lf\n",i,v[i].coord.first, v[i].coord.second);
    }


}*/