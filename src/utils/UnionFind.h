#ifndef UNIONFIND_H
#define UNIONFIND_H

#include <vector>
#include <numeric>

class UnionFind {
    private:
        std::vector<int> parent;
        // Removemos o vetor 'size' pois não vamos usar a otimização de troca

    public:
        UnionFind(int n) {
            parent.resize(n);
            std::iota(parent.begin(), parent.end(), 0); // 0, 1, 2...
        }

        // Função FIND Iterativa (Previne Crash/Stack Overflow em imagens grandes)
        int find(int i) {
            int root = i;
            while (root != parent[root]) {
                root = parent[root];
            }
            
            // Compressão de caminho (Otimização segura)
            int curr = i;
            while (curr != root) {
                int next = parent[curr];
                parent[curr] = root;
                curr = next;
            }
            return root;
        }

        // Função UNION Determinística
        // O segundo argumento (child) SEMPRE vira filho do primeiro (parent)
        void union_sets(int parent_idx, int child_idx) {
            int root_p = find(parent_idx);
            int root_c = find(child_idx);

            if (root_p != root_c) {
                parent[root_c] = root_p; // Sem swap! Obedece a ordem.
            }
        }
};

#endif