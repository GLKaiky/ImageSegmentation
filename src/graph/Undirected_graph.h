#ifndef UNDIRECTED_GRAPH_H
#define UNDIRECTED_GRAPH_H
#include "Graph.h"

template <typename T>
class Undirected_graph : public Graph<T> {
    public:
        //Construtor
        Undirected_graph() { }

        //Lógica não direcionada
        void insert(const T u, const T v) {
            this->adj[u];
        }

        void inicializar(int size) {
            this->setSize(size);
        }
        
};
#endif