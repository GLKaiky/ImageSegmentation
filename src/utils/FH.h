#ifndef FH_H
#define FH_H

#include <vector>
#include <algorithm>
#include <numeric>
#include "PixelConfiguration.h"
class FH {
    private:
        std::vector<double> internal_index;
        std::vector<int> parent;
        std::vector<int> component_size;
        int k;
        int size;

    public:
        FH(int k, int size){
            this->size = size;
            internal_index.assign(size, 0);
            component_size.assign(size, 1);
            parent.resize(size);

            std::iota(parent.begin(), parent.end(), 0);

            this->k = k;
        }
        ~FH() { }

    void FelzenszwalbNHuttenlocher(ARESTA edge) {
        int root_u = find(edge.u);
        int root_v = find(edge.v);

        if (root_u != root_v) {
            int t1 = k / (double)component_size[root_v];
            int t2 = k / (double)component_size[root_u];

            double M_Int = std::min(internal_index[root_u] + t2, internal_index[root_v] + t1);
            
            if (edge.weight <= M_Int) { 
                
                int new_parent, child;
                if (component_size[root_u] < component_size[root_v]) {
                    new_parent = root_v;
                    child = root_u;
                } else {
                    new_parent = root_u;
                    child = root_v;
                }

                parent[child] = new_parent;
                
                component_size[new_parent] += component_size[child];
                
                internal_index[new_parent] = std::max({internal_index[root_u], 
                                                       internal_index[root_v], 
                                                        edge.weight});
            }
        }
    }
    int find(int i) {
        if(this->parent[i] == i)
            return i;
        
        return this->parent[i] = find(this->parent[i]);
    }

    int getParent(int i) {
        return this->parent[i];
    }
};

#endif