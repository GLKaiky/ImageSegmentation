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

    std::vector<int> getParents() {
        return this->parent;
    }

    std::vector<int> getSize() {
        return this->size;
    }
        
    UnionFind(int size){
        this->parent.resize(size);
        std::iota(parent.begin(), parent.end(), 0);
        this->size.assign(size, 1); //Inicializar todos os conjuntos com tamanho 1
    }

    void union_sets(int u, int v) {
        int root_u = find(u);
        int root_v = find(v);

        if(root_u != root_v) {
            if(size[root_u] < size[root_v]) {
                parent[root_u] = root_v;
                size[root_v] += size[root_u]; //anexar a menor raiz da MST à maior
            } else {
                parent[root_v] = root_u;
                size[root_u] += size[root_v];
            }
        }
    }

    int find(int i) {
        if(this->parent[i] == i)
            return i;
        
        return this->parent[i] = find(this->parent[i]);
    }

};

#endif