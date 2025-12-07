#ifndef IFT_H
#define IFT_H

#include <vector>
#include <cmath>
#include <algorithm>
#include <limits>
#include <queue>

// Seus headers existentes
#include "Filters.h"            // Obrigatório: Deve conter RGBtoLab
#include "PixelConfiguration.h" // Obrigatório: Structs Node, Seeds, CIELAB
#include "graph/DirectedGraph.h"      // Sua classe de Grafo


struct IFTNode {
    int index;
    double cost;
    bool operator>(const IFTNode& other) const {
        return cost > other.cost;
    }
};

class IFT {
private:
    const double DIST_ORTHO = 1.0;
    const double DIST_DIAG = 1.41421356;
    static constexpr double INF = std::numeric_limits<double>::max();


    double deltaE(const CIELAB& a, const CIELAB& b) {
        double dL = a.L - b.L;
        double da = a.a - b.a;
        double db = a.b - b.b;
        return std::sqrt(dL*dL + da*da + db*db);
    }

public:

    DirectedGraph buildGraph(const unsigned char* img, int w, int h) {
        DirectedGraph graph;
        int numPixels = w * h;
        graph.inicializar(numPixels, w, h);

        int dx[] = {1, -1, 0, 0};
        int dy[] = {0, 0, 1, -1};
        
        std::vector<CIELAB> labs(numPixels);
        for(int i=0; i<numPixels; i++) {
            labs[i] = RGBtoLab(img[i*3], img[i*3+1], img[i*3+2]);
        }

        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                int u = y * w + x;

                for (int i = 0; i < 4; ++i) {
                    int nx = x + dx[i];
                    int ny = y + dy[i];

                    if (nx >= 0 && nx < w && ny >= 0 && ny < h) {
                        int v = ny * w + nx;
                        
                        // Peso da aresta = Diferença de Cor CIELAB
                        double weight = deltaE(labs[u], labs[v]);
                        graph.insert(u, v, weight);
                    }
                }
            }
        }
        return graph;
    }

    // 4. Executa o Pipeline Completo
    // Gera sementes -> Monta Grafo -> Resolve Watershed
    void runAutomatic(const unsigned char* img, int w, int h, std::vector<int>& outputLabels) {
        
        // A. Sementes Automáticas via EDT (Usando L do CIELAB)
        std::vector<double> distMap = computeGlobalEDT(img, w, h);
        Seeds seeds = extractSeedsFromMap(distMap, w, h);

        // B. Construir Grafo (Usando Delta E do CIELAB)
        DirectedGraph graph = buildGraph(img, w, h);

        // C. Resolver (IFT / Watershed)
        solve(graph, seeds, outputLabels);
    }

    static void solve(DirectedGraph& graph, const Seeds& seeds, std::vector<int>& outputLabels) {
        int numNodes = graph.getSize();

        outputLabels.assign(numNodes, -1);
        std::vector<double> costs(numNodes, INF);
        
        std::priority_queue<IFTNode, std::vector<IFTNode>, std::greater<IFTNode>> pq;

        for (int idx : seeds.backgroundObj) {
            costs[idx] = 0.0;
            outputLabels[idx] = 0; 
            pq.push({idx, 0.0});
        }
        for (int idx : seeds.obj) {
            costs[idx] = 0.0;
            outputLabels[idx] = 1; 
            pq.push({idx, 0.0});
        }

        while (!pq.empty()) {
            IFTNode current = pq.top();
            pq.pop();
            
            int u = current.index;
            if (current.cost > costs[u]) continue;

            const auto& neighbors = graph.getNeighbors(u);

            for (const auto& edge : neighbors) {
                int v = edge.first;
                double weight = edge.second; 
                
                // Custo Max-Arc (Watershed)
                double newCost = std::max(current.cost, weight);

                if (newCost < costs[v]) {
                    costs[v] = newCost;
                    outputLabels[v] = outputLabels[u];
                    pq.push({v, newCost});
                }
            }
        }
    }



};

#endif