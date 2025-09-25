#ifndef GRAPH_H
#define GRAPH_H
#include <vector>
#include <list>

template <typename T>
class Graph {
    protected:
        std::vector<std::list<T>> adj;
        int size;
    
    public:
        Graph(): size(0) { }
        virtual ~Graph(){ }

        void setSize(int size) {
            this->adj.resize(size);
            this->size = size;
        }
};

#endif