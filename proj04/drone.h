// Project Identifier: 1761414855B69983BD8035097EFBD312EB0527F0
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <cstring>
#include <cmath>
#include <getopt.h>
#include <stack>
#include "drone.hpp"
#include <iomanip>
#include <queue>

using namespace std;

struct vcompare{
    int operator()(const Vertex &v1, const Vertex &v2){
        return v1.deg > v2.deg;
    }
};

class Drones{
private:
    vector<Drop> drops;
    vector<uint32_t> path;
    size_t num_drops;
    vector<vector<int>> adj = {};

    double weight =  0, length = 0;

    // FUNCTIONS
    double eudista(Drop &a, Drop &b, bool mst = false){
        if (mst){
            if (((a.campus == Campus::medical && b.campus == Campus::normal) || (a.campus == Campus::normal && b.campus == Campus::medical)))
                return infinity;
        }
        return double(sqrt( ( pow((double)a.x - (double)b.x, 2) ) + pow( (double)a.y - (double)b.y, 2)));
    }

    

    // MAIN FUNCTIONS
vector<Vertex> pathA(bool MST = false){
        // std::cout << "THIS IS SEEING PATH A\n";
        vector<Vertex> vertices(num_drops);
        path.resize(num_drops, 0);

        vertices[0].dedge = 0;
        vertices[0].pidx = -1;

        uint32_t cidx = 0, tofill = 1;
        
        for (uint32_t i = 0; i < num_drops; i++){
            double sdist = infinity;
            for (size_t g = 0; g < num_drops; g++){
                if (!vertices[g].visited && (vertices[g].dedge < sdist)){
                    sdist = vertices[g].dedge;
                    cidx = int(g);
                }
            }
            vertices[cidx].visited = true;
            if (cidx > 0 && !MST){
                vertices[cidx].deg++;
                vertices[vertices[cidx].pidx].deg++;
                // vertices[vertices[cidx].pidx].next = cidx;l;
                path[tofill] = cidx;
                // cout << "THIS IS ADDED TO PATH: " << cidx << endl;
                tofill++;

            }

            for (uint32_t j = 0; j < num_drops; j++){
                if (!vertices[j].visited){
                    double check = eudista(drops[cidx], drops[j], MST);
                    // std::cout << "this is between " << cidx << " || " << j << " = " << check << endl;
                    if (check < vertices[j].dedge && MST){
                        vertices[j].dedge = check;
                        vertices[j].pidx = cidx;
                    }
                    if(!MST){
                        vertices[j].dedge = check;
                        vertices[j].pidx = cidx;
                    }
                }
            }
        }
        for (Vertex v : vertices)
            if (v.pidx >= 0)
                weight += v.dedge;
        return vertices;
    }

    void printa(vector<Vertex> vs){

        cout << weight << '\n';
        for (size_t i = 0; i < vs.size(); i++)
            if (vs[i].pidx >= 0)
                // cout << vs[i].pidx << " " << i << '\n';
                cout << (size_t(vs[i].pidx) < i ? vs[i].pidx : i) << ' ' << (size_t(vs[i].pidx) > i ? vs[i].pidx : i) << '\n';
    }

    void pathB(bool FTSP = false){ 
        // cout << "THIS IS FASTTSP MODE" << endl;
        vector<Vertex> vertices = pathA();

        // cout << "FSSTTSP" << endl;
        uint32_t last = 0;
        for (uint32_t i = 1; i < num_drops; i++){
            if (vertices[i].deg < 2) last = i;
        }
        //fix up some last connections
        vertices[0].dedge = eudista(drops[0], drops[last], false);
        weight += vertices[0].dedge;
        vertices[0].pidx = last; vertices[last].deg = vertices[0].deg = 2;

        // //some testing print statements
        // cout << "THESE ARE DEGREES" << endl;
        // for (auto &e : vertices) cout << e.deg << " ";
        // cout << endl;
        // cout << "THESE ARE PIDX" << endl;
        // for (auto &e : vertices) cout << e.pidx << " ";
        // cout << endl;
        // double temp = 0;
        // for (auto &e : vertices) temp += e.dedge;
        // cout << temp << endl;

        // cout << "THIS IS PATH: ";
        // for (auto &e: path) cout << e << " ";
        // cout << endl;
        // cout << "Weight before: " << weight << endl; 
        for (uint32_t i = 0; i < 3; i++){
            opt2(vertices);
        }
        // cout << "Weight after: " << weight << endl; 

        // cout << "THIS IS PATH2: ";
        // for (auto &e: path) cout << e << " ";

        // cout << endl <<"this is something" << endl;
        if (FTSP){
            printTSP(weight);
        }   
    }
    void opt2(vector<Vertex> &vertices){
        // double d1 = 0, d2 = 0, d3 = 0, d4 = 0;
        double d2 = 0, d3 = 0;
        uint32_t v1, v2, v3, v4;
        double auxw = weight;
        for (uint32_t i = 1; i < num_drops; i++){
            
            for (uint32_t j = i + 1; j < num_drops; j++){
                double ubound  = weight;
                v1 = path[i]; v2 = path[((i+1) % num_drops)];
                v3 = path[j % num_drops]; v4 = path[((j+1) % num_drops)];
                // d1 = eudista(drops[v1], drops[v2], false);
                d2 = eudista(drops[v1], drops[v3], false);
                d3 = eudista(drops[v2], drops[v4], false);
                // d4 = eudista(drops[v3], drops[v4], false);
                // cout << "THIS IS: " << i << ": " << d1 << " " << d2 << " " << d3 << " " << d4 << endl;
                // cout << "THIS IS v1234: " << v1 << " " << v2 << " " << v3 << " " << v4 << endl; 
                // cout << "THIS IS 24: " << (eudista(drops[v1], drops[v2], false) + eudista(drops[v3], drops[v4], false)) << endl;
                // cout << "THIS IS 13: " << (eudista(drops[v1], drops[v3], false) + eudista(drops[v2], drops[v4], false)) << endl << endl;

                // cout << "This is path b4: ";
                // for (auto t : path) cout << t << " ";
                // cout << endl;
                if ((eudista(drops[v1], drops[v2], false) + eudista(drops[v3], drops[v4], false)) >
                    (eudista(drops[v1], drops[v3], false) + eudista(drops[v2], drops[v4], false))){
                        swap(path[((i+1) % num_drops)], path[j % num_drops]);
                        // cout << "some changes are happening" << endl;
                        // cout << "This is path after: ";
                        // for (auto t : path) cout << t << " ";
                        // cout << endl;

                        ubound -= (eudista(drops[v1], drops[v2], false) + eudista(drops[v3], drops[v4], false));
                        ubound += (d2 + d3);
                        // cout << "weight bfore1: " << auxw << endl;
                        if (ubound < auxw){
                            auxw = ubound;
                            vertices[v3].pidx = v1; vertices[v3].dedge = d2;
                            vertices[v4].pidx = v2; vertices[v4].dedge = d3;
                            vertices[v2].pidx = v3; vertices[v2].dedge = eudista(drops[v2], drops[v3], false);
                            // vertices[v1].pidx = v4; vertices[v1].dedge = eudista(drops[v1], drops[v4], false);
                        }
                        // cout << "weight after1: " << auxw << endl;

                        

                    // cout << "THIS IS NEWPATH: ";
                    // for (auto &e: path) cout << e << " ";

                }
            }

        }if (auxw < weight) weight = auxw;
    }


    void pathC(){

    }

    void printTSP(double length){
        cout << length << '\n';
        for (size_t i = 0; i < path.size(); i++){
            cout << path[i] << " ";
        }cout << '\n';
        // for (size_t i = 0; i < path.size(); i++){
        //     cout << '(' << drops[path[i]].x << ", " << drops[path[i]].y << ") || " << vertices[path[i]].pidx << " -> " << path[i] << " and dedge: " << vertices[path[i]].dedge << endl;
        // }

        // cout << "RUNNING IT THRU VERTICES" << endl;
        // uint32_t counter = 0;
        // for (auto &v : vertices){
        //     cout << v.pidx << " -> " << counter++ << " " << v.dedge << endl;
        // }
    }

public:

    void deliver(uint32_t m){
        bool border = false, normal = false, medical = false;

        cin >> num_drops;

        drops.reserve(num_drops);
        for (uint32_t i = 0; i < num_drops; i++){
            int x, y;
            cin >> x >> y;
            drops.emplace_back(x, y);

            if (m == 1){
                if (drops[i].campus == Campus::border)
                    border = true;
                if (drops[i].campus == Campus::medical)
                    medical = true;
                if (drops[i].campus == Campus::normal)
                    normal = true;
            }
        }

        if (m == 1){ //MST
            if (normal && medical && !border){
                cerr << "Cannot construct MST\n";
                exit(1);
            }else printa(pathA(true));
        }
        else if (m == 2) //FASTTSP
            pathB(true);
        else //mode == 3 -> OPTTSP
            pathC();
    }

};