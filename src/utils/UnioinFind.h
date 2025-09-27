#ifndef UNIONFIND_H
#define UNIONFIND_H

#include <vector>
#include <iostream>
#include <numeric>
#include "PixelConfiguration.h"
class UnionFind {
    private:
        std::vector<int> size;
        std::vector<int> parent;

    public:
        
    UnionFind(int size){
        this->parent.resize(size);
        std::iota(parent.begin(), parent.end(), 0);
        this->size.assign(size, 1); //Inicializar todos os conjuntos com tamanho 1
    }

    int find(int i) {
        if(this->parent[i] == i)
            return i;
        
        return this->parent[i] = find(this->parent[i]);
    }
};

#endif