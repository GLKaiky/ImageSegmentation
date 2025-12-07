/******************************************************************************
 * @file: Graph.h
 * @author: Kaiky França da Silva | Puc Minas
 * @brief: O arquivo que vai ser a base do grafo para as heranças direcionadas e não direcionadas
 * @version 0.1
 * @date 2025-09-25
 *****************************************************************************/

#ifndef GRAPH_H
#define GRAPH_H
#include <vector>
#include <list>
#include <utility>
#include <iostream>


class Graph {
    protected:
        std::vector<std::vector<std::pair<int, double> >> adj;
        int size;
    
    public:
        Graph(): size(0) { }
        virtual ~Graph(){ }

        void setSize(int size) {
            this->adj.resize(size);
            this->size = size;
        }

        int getSize() const { return size; }
};

#endif