/******************************************************************************
 * @file: UnionFind.h
 * @author: Kaiky França da Silva | Puc Minas
 * @brief: Classe Union find e suas aplicações algoritmicas para a segmentação de imagem (
 * está na pasta utils mas não sei se deveria estar aqui)
 * @version 0.1
 * @date 2025-09-25
 *****************************************************************************/


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
    

    /*
    * @brief Union sets vai fazer as operações de find e fazer a união dos conjuntos 
    *CASO ELE NÃO FECHE CICLOS, caso feche, ele ignora
    * 
    * @param v vértice 
    * @param u vértice
    */

    void union_sets(int u, int v) {
        int root_u = find(u);
        int root_v = find(v);

        if (root_u != root_v) {
            if (size[root_u] < size[root_v]) {
                std::swap(root_u, root_v);
            }
            parent[root_v] = root_u;
            size[root_u] += size[root_v];
        }
    }

    /*
    * @brief Função find vai encontrar o parent daquele conjunto, inicialmente sendo ele mesmo, porém quando o conjunto cresce ele
    * se modificar para a raiz principal para otimização
    * @param i o elemento que estou procurando seu parent
    */

    int find(int i) {
        if(this->parent[i] == i)
            return i;
        
        return this->parent[i] = find(this->parent[i]);
    }

    int find(int i) const { // A palavra 'const' aqui é a chave!
        while (parent[i] != i) {
            i = parent[i];
        }
        return i;
    }

};

#endif