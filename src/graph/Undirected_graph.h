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
#include <algorithm>


class Undirected_graph : public Graph{
    private:
    
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


        
};
#endif