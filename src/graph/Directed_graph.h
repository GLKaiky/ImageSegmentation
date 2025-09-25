#ifndef DIRECTED_GRAPH_H
#define DIRECTED_GRAPH_H
#include "Graph.h"

template <typename T>
class Directed_graph : public Graph {
    public:
    
        //Construtor
        Directed_graph() : Graph() {}

        //Inserção Direcionada
        void insert(const T u, const T v) {
            this->adj[u].push_back(v);
        }


};
#endif