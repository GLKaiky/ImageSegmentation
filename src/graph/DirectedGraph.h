#ifndef DIRECTEDGRAPH_H
#define DIRECTEDGRAPH_H

#include <vector>
#include <iostream>
#include "Graph.h"
#include "utils/PixelConfiguration.h"


class DirectedGraph : public Graph {
    private:
        int width;
        int height;
    public:
        DirectedGraph() {
            this->width = 0;
            this->height = 0;
        }

        void insert(const int u, const int v, const double weight) {
            if(u >= this->size || v >= this->size) return;

            this->adj[u].push_back({v, weight});
        }

        std::vector<Edge> toEdgeList() {
            std::vector<Edge>  edgeList;

            edgeList.reserve(this->size * 4);
            int id_counter = 0;

            for(int i = 0; i< this->size; i++) {
                for(auto& neightbord: this->adj[i]){
                    int v = neightbord.first;
                    double w = neightbord.second;

                    edgeList.push_back({i, v, w, id_counter++});
                }
            }
            return edgeList;
        }

        const std::vector<std::pair<int, double>>& getNeighbors(int u) const {
                return this->adj[u];
            }


        void inicializar(int size, int w, int h) {
            this->setSize(size);
            this->width = w;
            this->height = h;

            this->adj.assign(size, std::vector<std::pair<int, double>>());
        }

        void setWidth(int w) {
            this->width = w;
        }

        void setHeight(int h) {
            this->height = h;
        } 

        int getSize() {
            return this->size;
        }

        int getWidth() {
            return this->width;
        }

        int getHeight() {
            return this->height;
        }

};

#endif