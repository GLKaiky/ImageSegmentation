/******************************************************************************
 * @file: Undirected_graph.h
 * @author: Kaiky França da Silva | Puc Minas
 * @brief: O arquivo que vai tratar da estrutura do grafo em sí e suas operações
 *         herdadas da classe principal Graph.h
 * @version 0.1
 * @date 2025-09-25
 *****************************************************************************/


#ifndef UNDIRECTED_GRAPH_H
#define UNDIRECTED_GRAPH_H
#include "Graph.h"
#include "utils/PixelConfiguration.h"
#include "utils/FH.h"
#include <algorithm>
#include "utils/UnionFind.h"


class Undirected_graph : public Graph{
    private:

        int width;
        int height;



    public:
        //Construtor
        Undirected_graph() { 
            this->width = 0;
            this->height = 0;
        }

        //Lógica não direcionada
        void insert(const int u, const int v, const double weight) {
            if(u >= this->size || v >= this->size) return;

            this->adj[u].push_back({v, weight});
            this->adj[v].push_back({u, weight});
        }

        void inicializar(int size) {
            this->setSize(size);
        }

        void printNeighbors(int u) const {
            if (u >= this->size) {
                std::cout << "Nó " << u << " está fora dos limites do grafo." << std::endl;
                return;
            }

            std::cout << "Vizinhos do nó " << u << ":" << std::endl;
            if (this->adj[u].empty()) {
                std::cout << "  (Nenhum vizinho encontrado)" << std::endl;
                return;
            }

            for (const auto& edge : this->adj[u]) {
                std::cout << "  -> Nó " << edge.first << " | Peso: " << edge.second << std::endl;
            }
        }

    /**
    * @brief Ordena as arestas do grafo de acordo com o peso para union find
    * @param a prórpia lista de adjacência do grafo
    */
    
        std::vector<ARESTA> sort_edges() {
            std::vector<ARESTA> all_edges;
        
            for (int u = 0; u < this->size; ++u) {
                for (const auto& edge_pair : this->adj[u]) {
                    int v = edge_pair.first;
                    double weight = edge_pair.second;

                    if (u < v) {
                        all_edges.push_back({u, v, weight});
                    }
                }
            }

            std::sort(all_edges.begin(), all_edges.end(), [](const ARESTA& a, const ARESTA& b) {
                return a.weight < b.weight;
            });

            // Retornar o vetor ordenado
            return all_edges;
        }

        
    /*
    * @brief Faz o algoritmo de Kruskal para criar em um vector a MST do Grafo, ou AGM (Arvore Geradora Minima)
    * @param A própria lista de adjacência do grafo
    */

        std::vector<ARESTA> Kruskal() {
            std::vector<ARESTA> sortedEdges = sort_edges();
            std::vector<ARESTA> mst;
            UnionFind unionFind(this->size);
            int cont = 0;
            for(const ARESTA& aresta : sortedEdges) {
                
                if(mst.size() == this->size - 1) {
                    return mst;
                }

                if(unionFind.find(aresta.u) != unionFind.find(aresta.v)) {
                    unionFind.union_sets(aresta.u, aresta.v);

                    mst.push_back(aresta);
                }
            }
            
            return mst;

        }

        FH MST_Forest(int k, const std::vector<ARESTA>& sortedEdges) {
            
            FH segmentador(k, this->size);

            for(const auto& aresta: sortedEdges) {
                segmentador.FelzenszwalbNHuttenlocher(aresta);
            }
            return segmentador;
        }


        UnionFind findComponents(std::vector<ARESTA> &mst) {

            UnionFind uf(this->size);

            for(const ARESTA& aresta: mst) {
                uf.union_sets(aresta.u, aresta.v);
            }

            return uf;
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