#ifndef IFT_SEGMENTATION_H
#define IFT_SEGMENTATION_H
#include <vector>

#include "graph/DirectedGraph.h"
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"
#include "utils/Filters.h"

/**
 * @brief Cria um grafo ponderado a partir de uma imagem.
 * @param imagePath Caminho para o arquivo de imagem.
 * @param graph Grafo que será populado. As arestas serão pares <vértice_vizinho, peso>.
*/
unsigned char* create_graph(const char * imagePath, DirectedGraph &graph) {
    
    int height, width, original_channels;
    
    // Força o carregamento com 3 canais (RGB)
    unsigned char * imageData = stbi_load(imagePath, &width, &height, &original_channels, 3); 

    std::cout << original_channels << std::endl;


    imageData = toGaussian_blur(imageData, width, height, 3); //Faz blur na imagem, que vai melhorar a segmentação
    imageData = toGaussian_blur(imageData, width, height, 3);
    

    unsigned char* sobelData = sobelFilter(imageData, width, height, 3);



    if(imageData == nullptr) {
        std::cerr << "Imagem vazia ou invalida " << imagePath << std::endl;
        return nullptr;
    }

    const long totalSize = height* width;
    graph.inicializar(totalSize, width, height);



    const double W = 500.0;
    
    //Verificar todos os 8 pixels em volta da imagem
    const int dx[] = {1, -1, 0, 0};
    const int dy[] = {0, 0, 1, -1};
 
    const int channels_in_memory = 3;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            unsigned long index = (y * width + x) * channels_in_memory;
            CIELAB current = RGBtoLab(imageData[index], imageData[index + 1], imageData[index + 2]);
           
            
            for (int i = 0; i < 4; i++) {
                int nextX = x + dx[i];
                int nextY = y + dy[i];

                if (nextX >= 0 && nextX < width && nextY >= 0 && nextY < height) {
                    unsigned long next_index = (nextY * width + nextX) * channels_in_memory;
                    CIELAB next = RGBtoLab(imageData[next_index], imageData[next_index + 1], imageData[next_index + 2]);


                    const int current_vertex_id = y * width + x;
                    const int next_vertex_id = nextY * width + nextX;

                    unsigned char sobel_current = sobelData[current_vertex_id];
                    unsigned char sobel_next = sobelData[next_vertex_id];
                    
                    double sobel_penalty = std::max(sobel_current, sobel_next);

                    //const double S = 150.0; //Posição absoluta dos pixels (>20 cor importa mais, <20 posição e mais importante)

                    //Calculo da distância de cores entre os pixels
                    double color_dist_sq = std::pow(current.L - next.L, 2) + std::pow(current.a - next.a, 2) + std::pow(current.b - next.b, 2); 


                    auto weight = color_dist_sq + (W * sobel_penalty);
                    
                    
                    graph.insert(current_vertex_id, next_vertex_id, weight);
                }
            }
        }
    }
    return imageData;
}


void saveSegmentation(const std::vector<int>& labels, const unsigned char* originalImg, int w, int h, const char* filename) {
    std::vector<unsigned char> out(w * h * 3);

    for (int i = 0; i < w * h; ++i) {
        if (labels[i] == 1) { 
            out[i*3]     = originalImg[i*3];
            out[i*3 + 1] = originalImg[i*3 + 1];
            out[i*3 + 2] = originalImg[i*3 + 2];
        } else {
            out[i*3]     = 0;
            out[i*3 + 1] = 0;
            out[i*3 + 2] = 0;
        }
    }
    stbi_write_png(filename, w, h, 3, out.data(), w * 3);
}

/**
 * @brief Gera uma imagem contendo apenas as bordas da segmentação IFT.
 * @param labels Vetor com os rótulos de saída da IFT (0 ou 1).
 * @param w Largura da imagem.
 * @param h Altura da imagem.
 * @param filename Caminho para salvar a imagem de borda.
 */
void saveIFTBoundaries(const std::vector<int>& labels, int w, int h, const char* filename) {
    // Cria buffer para imagem de saída (3 canais RGB), inicializado com preto (0)
    std::vector<unsigned char> out(w * h * 3, 0);

    // Deslocamentos para checar os 4 vizinhos (cima, baixo, esq, dir)
    int dx[] = {1, -1, 0, 0};
    int dy[] = {0, 0, 1, -1};

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int idx = y * w + x;
            int currentLabel = labels[idx];
            bool isBorder = false;

            // Verifica os 4 vizinhos
            for (int i = 0; i < 4; ++i) {
                int nx = x + dx[i];
                int ny = y + dy[i];

                // Verifica se o vizinho está dentro dos limites da imagem
                if (nx >= 0 && nx < w && ny >= 0 && ny < h) {
                    int nIdx = ny * w + nx;
                    
                    // SE o vizinho tem um rótulo diferente do pixel atual, detectamos uma borda
                    if (labels[nIdx] != currentLabel) {
                        isBorder = true;
                        break; // Já sabemos que é borda, não precisa checar os outros
                    }
                }
            }

            // Se for borda, pintamos de Branco (255, 255, 255)
            // Se quiser outra cor (ex: Vermelho), use: 255, 0, 0
            if (isBorder) {
                out[idx * 3]     = 255; 
                out[idx * 3 + 1] = 255;
                out[idx * 3 + 2] = 255;
            }
        }
    }

    stbi_write_png(filename, w, h, 3, out.data(), w * 3);
}
#endif