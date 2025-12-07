#ifndef EDMONDS_H
#define EDMONDS_H

#include <vector>
#include <algorithm>
#include <iostream>
#include "PixelConfiguration.h"


class Edmonds {
public:
    static std::vector<Edge> solve(int numNodes, const std::vector<Edge>& edges, int rootId) {
        if (edges.empty()) return {};
        
        std::cout << "--- Iniciando Edmonds Otimizado (Nos: " << numNodes << ") ---" << std::endl;
        
        // Criamos um vetor de lookup com o tamanho MÁXIMO possível (numero total de pixels/nós originais)
        // Isso evita alocações repetidas na recursão.
        // O tamanho deve ser suficiente para cobrir qualquer ID de pixel 'v'.
        // Assumimos que numNodes inicial cobre todos os IDs.
        std::vector<int> lookup(numNodes + 1, -1);

        return solveRecursive(numNodes, edges, rootId, edges, 0, lookup);
    }

private:
    static std::vector<Edge> solveRecursive(int numNodes, 
                                            const std::vector<Edge>& activeEdges, 
                                            int rootId, 
                                            const std::vector<Edge>& globalOriginalEdges,
                                            int depth,
                                            std::vector<int>& lookup) { // Passamos lookup por referência
        
        // Log reduzido para não poluir o terminal, mas mostrar vida
        if (depth % 100 == 0) { 
            std::cout << "[Depth " << depth << "] Processando " << numNodes << " nos..." << std::endl;
        }

        // 1. FASE GULOSA (Min-In)
        std::vector<int> minIn(numNodes, -1);
        for (int i = 0; i < (int)activeEdges.size(); ++i) {
            const auto& e = activeEdges[i];
            // Filtros de segurança
            if (e.u == e.v || e.v == rootId || e.u >= numNodes || e.v >= numNodes) continue;
            
            if (minIn[e.v] == -1 || e.weight < activeEdges[minIn[e.v]].weight) {
                minIn[e.v] = i;
            }
        }

        // 2. DETECÇÃO DE CICLOS
        std::vector<int> group(numNodes, -1);
        std::vector<int> visited(numNodes, -1);
        std::vector<bool> nodeInCycle(numNodes, false);
        int newNumNodes = 0;
        int cycleCount = 0;

        for (int i = 0; i < numNodes; ++i) {
            if (i == rootId || minIn[i] == -1 || group[i] != -1) continue;
            int curr = i;
            while (curr != rootId && minIn[curr] != -1 && visited[curr] != i && group[curr] == -1) {
                visited[curr] = i;
                curr = activeEdges[minIn[curr]].u;
            }
            if (curr != rootId && minIn[curr] != -1 && visited[curr] == i) {
                int cycleStart = curr;
                int temp = cycleStart;
                do {
                    group[temp] = newNumNodes;
                    nodeInCycle[temp] = true;
                    temp = activeEdges[minIn[temp]].u;
                } while (temp != cycleStart);
                newNumNodes++;
                cycleCount++;
            }
        }

        // 3. CASO BASE (Sem ciclos)
        if (cycleCount == 0) {
            std::vector<Edge> result;
            result.reserve(numNodes);
            for (int i = 0; i < numNodes; ++i) {
                if (i != rootId && minIn[i] != -1) {
                    result.push_back(globalOriginalEdges[activeEdges[minIn[i]].edgeId]);
                }
            }
            return result;
        }

        // 4. CONTRAÇÃO
        for (int i = 0; i < numNodes; ++i) {
            if (group[i] == -1) group[i] = newNumNodes++;
        }

        std::vector<Edge> nextLevelEdges;
        nextLevelEdges.reserve(activeEdges.size());
        
        for (const auto& e : activeEdges) {
            if (e.u >= numNodes || e.v >= numNodes) continue;
            int newU = group[e.u];
            int newV = group[e.v];
            
            if (newU != newV) {
                double newWeight = e.weight;
                if (nodeInCycle[e.v]) newWeight -= activeEdges[minIn[e.v]].weight;
                nextLevelEdges.push_back({newU, newV, newWeight, e.edgeId});
            }
        }

        // 5. RECURSÃO
        auto recursiveResult = solveRecursive(newNumNodes, nextLevelEdges, group[rootId], globalOriginalEdges, depth + 1, lookup);

        // 6. EXPANSÃO OTIMIZADA (O(N) em vez de O(N^2))
        std::vector<Edge> finalResult = recursiveResult; // Copia as arestas da recursão
        std::vector<bool> isCovered(numNodes, false);

        // --- INICIO DA OTIMIZAÇÃO ---
        
        // Passo A: Preenche a tabela de Lookup
        // Mapeia: "Pixel Real de Destino" -> "Índice do Nó de Ciclo Atual"
        std::vector<int> cleanupList; // Para limpar o vetor lookup rapidamente depois
        cleanupList.reserve(cycleCount * 5); // Estimativa

        for (int i = 0; i < numNodes; ++i) {
            if (nodeInCycle[i]) {
                // Descobre qual é o pixel real que a aresta interna deste ciclo aponta
                int internalEdgeOriginalId = activeEdges[minIn[i]].edgeId;
                int realDestPixel = globalOriginalEdges[internalEdgeOriginalId].v;

                // Se o pixel real for válido (dentro do tamanho do lookup)
                if (realDestPixel < (int)lookup.size()) {
                    lookup[realDestPixel] = i; // Registra que o nó 'i' espera uma aresta neste pixel
                    cleanupList.push_back(realDestPixel);
                }
            }
        }

        // Passo B: Verifica as arestas externas em O(1)
        for (const auto& resEdge : recursiveResult) {
            int resDestReal = resEdge.v; // O destino real da aresta escolhida lá em cima

            // Verificamos instantaneamente se esse destino coincide com alguém do ciclo
            if (resDestReal < (int)lookup.size() && lookup[resDestReal] != -1) {
                int cycleNodeIndex = lookup[resDestReal];
                isCovered[cycleNodeIndex] = true;
            }
        }

        // Passo C: Limpa o Lookup para o próximo uso (Importante!)
        for (int idx : cleanupList) {
            lookup[idx] = -1;
        }
        // --- FIM DA OTIMIZAÇÃO ---

        // Adiciona as arestas internas dos ciclos que não foram quebrados
        for (int i = 0; i < numNodes; ++i) {
            if (nodeInCycle[i] && !isCovered[i]) {
                finalResult.push_back(globalOriginalEdges[activeEdges[minIn[i]].edgeId]);
            }
        }

        return finalResult;
    }
};

#endif