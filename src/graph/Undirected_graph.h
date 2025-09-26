#ifndef UNDIRECTED_GRAPH_H
#define UNDIRECTED_GRAPH_H
#include "Graph.h"

class Undirected_graph : public Graph{
    public:
        //Construtor
        Undirected_graph() { }

        //Lógica não direcionada
        void insert(const int u, const int v, const double weight) {
            if(u >= this->size || v >= this->size) return;

            this->adj[u].push_back({v, weight});
            this->adj[v].push_back({u, weight});
        }

        void inicializar(int size) {
            this->setSize(size);
        }
        
};
#endif