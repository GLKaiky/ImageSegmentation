/******************************************************************************
 * @file: Image_Processing.cpp
 * @author: Kaiky França da Silva | Puc Minas
 * @brief: Este arquivo contém as funções responsáveis por converter
 * uma imagem digital em uma estrutura de dados de grafo não direcionado e ponderado. A lógica principal
 * trata cada pixel como um vértice e cria arestas entre pixels vizinhos.
 * @version 0.1
 * @date 2025-09-25
 *****************************************************************************/

#include "stb/stb_image.h"
#include "stb/stb_image_write.h"
#include "graph/Undirected_graph.h"
#include "utils/UnionFind.h"
#include "utils/PixelConfiguration.h"
#include "utils/FH.h"
#include "utils/Converters.h"

#include <iostream>
#include <vector>
#include <utility>
#include <math.h>
#include <list>
#include <iomanip>  
#include <algorithm>    
#include <map>
#include <cstring>


unsigned char* toGaussian_blur() {
    
}

/**
 * @brief Calcula a cor média de cada segmento, cria uma nova imagem e a salva em disco.
 * @param output_filename O nome do arquivo de saída (ex: "resultado.png").
 * @param width A largura da imagem.
 * @param height A altura da imagem.
 * @param segmentos A estrutura UnionFind finalizada, que mapeia cada pixel a um ID de segmento.
 * @param original_imageData O ponteiro para os dados da imagem original, usado para calcular a cor média.
 */
void write_segmented_image(const char* output_filename, int width, int height, int channels,
                           FH& segmentador, unsigned char* original_imageData) {
    

    size_t buffer_size = width * height * channels;
    unsigned char *output_data = new unsigned char[buffer_size];


    if(original_imageData == nullptr || output_filename == nullptr) {
        std::cerr << "FUDEEEEU " << std::endl;
        return;
    }
    
    std::memcpy(output_data, original_imageData, buffer_size); // Copia a imagem original para o buffer de saída

    // Define a cor da borda (amarelo brilhante: R=255, G=255, B=0)
    unsigned char border_color[3] = {255, 255, 0}; 

    // Percorre a imagem para encontrar e pintar as bordas
    for (int y = 0; y < height - 1; ++y) { // -1 para evitar sair dos limites da imagem
        for (int x = 0; x < width - 1; ++x) { // -1 para evitar sair dos limites da imagem
            
            int current_pixel_idx = y * width + x;
            int raiz_atual = segmentador.find(current_pixel_idx);

            // Compara com o vizinho da DIREITA
            int right_pixel_idx = y * width + (x + 1);
            int raiz_direita = segmentador.find(right_pixel_idx);

            // Compara com o vizinho de BAIXO
            int down_pixel_idx = (y + 1) * width + x;
            int raiz_baixo = segmentador.find(down_pixel_idx);
            
            // Se a raiz do segmento for diferente da do vizinho, é uma borda
            if (raiz_atual != raiz_direita || raiz_atual != raiz_baixo) {
                
                // Pinta o pixel ATUAL no buffer de saída com a cor da borda
                unsigned char* pixel_out = output_data + current_pixel_idx * channels;
                for (int i = 0; i < channels; ++i) {
                    // Se a imagem tiver um canal alfa, o mantemos, senão pintamos a cor
                    if (i < 3) { // Garante que só pintamos R, G, B
                    pixel_out[i] = border_color[i];
                    }
                }
            }
        }
    }


    int stride_in_bytes = width * channels;
    stbi_write_png(output_filename, width, height, channels, output_data, stride_in_bytes);

    delete[] output_data;


}

/**
 * @brief Cria um grafo ponderado a partir de uma imagem.
 * @param imagePath Caminho para o arquivo de imagem.
 * @param graph Grafo que será populado. As arestas serão pares <vértice_vizinho, peso>.
*/
unsigned char* create_graph(const char * imagePath, Undirected_graph &graph) {
    
    int height, width, original_channels;
    
    // Força o carregamento com 3 canais (RGB)
    unsigned char * imageData = stbi_load(imagePath, &width, &height, &original_channels, 3); 

    if(imageData == nullptr) {
        std::cerr << "Imagem vazia ou invalida " << imagePath << std::endl;
        return nullptr;
    }

    graph.setWidth(width);
    graph.setHeight(height);

    const long totalSize = height* width;
    graph.inicializar(totalSize);


    //Verificar todos os 8 pixels em volta da imagem

    const int dx[] = {-1, -1, -1,  0, 0,  1, 1, 1};
    const int dy[] = {-1,  0,  1, -1, 1, -1, 0, 1};
 
    const int channels_in_memory = 3; //limites de canais da imagem (temporário)

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            unsigned long index = (y * width + x) * channels_in_memory;
            CIELAB current = RGBtoLab(imageData[index], imageData[index + 1], imageData[index + 2]);


            for (int i = 0; i < 8; i++) {
                int nextX = x + dx[i];
                int nextY = y + dy[i];

                if (nextX >= 0 && nextX < width && nextY >= 0 && nextY < height) {
                    unsigned long next_index = (nextY * width + nextX) * channels_in_memory;
                    CIELAB next = RGBtoLab(imageData[next_index], imageData[next_index + 1], imageData[next_index + 2]);


                    const int current_vertex_id = y * width + x;
                    const int next_vertex_id = nextY * width + nextX;

                    //const double S = 150.0; //Posição absoluta dos pixels (>20 cor importa mais, <20 posição e mais importante)

                    //Calculo da distância de cores entre os pixels
                    double color_dist_sq = std::pow(current.L - next.L, 2) + std::pow(current.a - next.a, 2) + std::pow(current.b - next.b, 2); 

                    //Calculo da Distância fisica dos pixels
                    //double spatial_dist_sq = std::pow(current.x - next.x, 2) + std::pow(current.y - next.y, 2);


                    /*
                        O 'custo' para conectar dois pixels é a diferença de cor deles, mais a diferença de posição, 
                        só que a importância da posição é drasticamente reduzida pelo fator S:
                    auto weight = color_dist_sq + (spatial_dist_sq / (S*S));

                        auto weight = color_dist_sq + (spatial_dist_sq / (S*S));
                    */

                    auto weight = color_dist_sq;
                    
                    
                    graph.insert(current_vertex_id, next_vertex_id, weight);
                }
            }
        }
    }
    return imageData;
}

// Encontrar os componentes conexos da imagem para colorir
/*UnionFind findComponents(const std::vector<ARESTA>& forest, int total_pixels) {
    UnionFind uf(total_pixels);
    for (const ARESTA& aresta : forest) {
        uf.union_sets(aresta.u, aresta.v);
    }
    return uf;
}*/

/*void segmentate(std::vector<ARESTA> &mst, int desired_segments) {
    if (mst.empty() || desired_segments <= 1) {
        return;
    }

    // PASSO 1: Ordene a MST do MAIOR para o MENOR peso.
    std::sort(mst.begin(), mst.end(), [](const ARESTA& a, const ARESTA& b) {
        return a.weight > b.weight; // A mágica está aqui: '>' ao invés de '<'
    });

    int cuts_to_make = desired_segments - 1;
    if (cuts_to_make < 0) cuts_to_make = 0;
    
    if (cuts_to_make > mst.size()) {
        cuts_to_make = mst.size();
    }
    
    for(int i = 0; i < cuts_to_make; i++){
        mst.pop_back();
    }
}*/

int main() {
    Undirected_graph g;
    const char* path = "images/templates/monaliza.jpg";
    const char* output_path = "images/luisS.png";
    const int K = 800; 

    std::cout << "Carregando imagem e criando grafo..." << std::endl;
    unsigned char* original_imageData = create_graph(path, g);

    if (original_imageData == nullptr) {
        return 1;
    }

    std::cout << "Executando o algoritmo de Kruskal..." << std::endl;
    FH segmentador = g.MST_Forest(K);
    int width = g.getWidth();
    int height = g.getHeight();
    int channels = 3;

    std::cout << "Segmentando" << std::endl;
    write_segmented_image("imagemT.png", width, height, channels, segmentador, original_imageData);

}
