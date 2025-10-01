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
#include <iostream>
#include <vector>
#include <utility>
#include <math.h>
#include "utils/PixelConfiguration.h"
#include <list>
#include <iomanip>  
#include <algorithm>
#include <map>
#include "utils/FH.h"
#include <cstring>

/*
 * @brief Faz a conversão do padrão RGB para o padrão CIELAB (melhor percepção das cores)
 * @param R (red)
 * @param G (green)
 * @param B (blue)
*/
CIELAB RGBtoLab(unsigned char R, unsigned char G, unsigned char B) {

    const double color_space_transformation_matrix [3][3] = {{0.4124564, 0.3575761, 0.1804375}, 
                                                            {0.2126729, 0.7151522, 0.0721750},
                                                            {0.0193339, 0.1191920, 0.9503041}};

    /*Normalizção dos padrões para real, dividindo por 255*/
    double R_norm = R/255.0;
    double G_norm = G/255.0;
    double B_norm = B/255.0;
    
    double r_linear = (R_norm <= 0.04045) ? R_norm/12.92 : pow(((R_norm + 0.055)/1.055), 2.4);
    double g_linear = (G_norm <= 0.04045) ? G_norm/12.92 : pow(((G_norm + 0.055)/1.055), 2.4);
    double b_linear = (B_norm <= 0.04045) ? B_norm/12.92 : pow(((B_norm + 0.055)/1.055), 2.4);

    double x =  (r_linear * color_space_transformation_matrix[0][0]) + 
                (g_linear * color_space_transformation_matrix[0][1]) + 
                (b_linear * color_space_transformation_matrix[0][2]);

    double y =  (r_linear * color_space_transformation_matrix[1][0]) + 
                (g_linear * color_space_transformation_matrix[1][1]) + 
                (b_linear * color_space_transformation_matrix[1][2]);

    double z =  (r_linear * color_space_transformation_matrix[2][0]) + 
                (g_linear * color_space_transformation_matrix[2][1]) + 
                (b_linear * color_space_transformation_matrix[2][2]);
    
    double x_norm = x/ 0.95047;
    double y_norm = y/ 1.00000;
    double z_norm = z/ 1.08883;

    double x_linear = (x_norm > 0.008856) ? cbrt(x_norm) : ((7.787*x_norm) + 16.0/116.0);
    double y_linear = (y_norm > 0.008856) ? cbrt(y_norm) : ((7.787*y_norm) + 16.0/116.0);
    double z_linear = (z_norm > 0.008856) ? cbrt(z_norm) : ((7.787*z_norm) + 16.0/116.0);

    double L = (116 * y_linear) - 16;
    double a = 500* (x_linear - y_linear);
    double b = 200* (y_linear - z_linear);

    return {L, a, b, 0, 0};
}

/**
 * @brief Calcula a cor média de cada segmento, cria uma nova imagem e a salva em disco.
 * @param output_filename O nome do arquivo de saída (ex: "resultado.png").
 * @param width A largura da imagem.
 * @param height A altura da imagem.
 * @param segmentos A estrutura UnionFind finalizada, que mapeia cada pixel a um ID de segmento.
 * @param original_imageData O ponteiro para os dados da imagem original, usado para calcular a cor média.
 */
void write_segmented_image(const char* output_filename, int width, int height, 
                           UnionFind& segmentos, const unsigned char* original_imageData) {
    
    int num_pixels = width * height;

    // ETAPA 1: Calcular a cor média de cada segmento.
    
    // 1.1: Passo de Acumulação - Soma as cores e conta os pixels de cada segmento.
    std::map<int, std::tuple<unsigned long long, unsigned long long, unsigned long long>> color_sums;
    std::map<int, int> pixel_counts;

    for (int i = 0; i < num_pixels; ++i) {
        int segment_id = segmentos.find(i);
        pixel_counts[segment_id]++;
        
        // Pega a cor do pixel na imagem ORIGINAL
        unsigned char r = original_imageData[i * 3 + 0];
        unsigned char g = original_imageData[i * 3 + 1];
        unsigned char b = original_imageData[i * 3 + 2];

        // Acumula os valores de cor para o respectivo segmento
        std::get<0>(color_sums[segment_id]) += r;
        std::get<1>(color_sums[segment_id]) += g;
        std::get<2>(color_sums[segment_id]) += b;
    }

    // 1.2: Passo de Média - Calcula a cor final de cada segmento.
    std::map<int, PixelColor> segment_colors;
    for (auto const& [id, count] : pixel_counts) {
        segment_colors[id].r = std::get<0>(color_sums[id]) / count;
        segment_colors[id].g = std::get<1>(color_sums[id]) / count;
        segment_colors[id].b = std::get<2>(color_sums[id]) / count;
    }

    // ETAPA 2: Criar o buffer da nova imagem e preenchê-lo com as cores dos segmentos.
    unsigned char* output_data = new unsigned char[num_pixels * 3];

    for (int i = 0; i < num_pixels; ++i) {
        int segment_id = segmentos.find(i);
        PixelColor final_color = segment_colors[segment_id];

        output_data[i * 3 + 0] = final_color.r;
        output_data[i * 3 + 1] = final_color.g;
        output_data[i * 3 + 2] = final_color.b;
    }

    // ETAPA 3: Salvar a imagem final em disco.
    // stbi_write_png(nome_arquivo, largura, altura, canais, dados, largura_em_bytes_por_linha)
    stbi_write_png(output_filename, width, height, 3, output_data, width * 3);
    
    // ETAPA 4: Limpeza da memória alocada para o buffer de saída.
    delete[] output_data;
}

/*
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

            current.x = x;
            current.y = y;

            for (int i = 0; i < 8; i++) {
                int nextX = x + dx[i];
                int nextY = y + dy[i];

                if (nextX >= 0 && nextX < width && nextY >= 0 && nextY < height) {
                    unsigned long next_index = (nextY * width + nextX) * channels_in_memory;
                    CIELAB next = RGBtoLab(imageData[next_index], imageData[next_index + 1], imageData[next_index + 2]);

                    next.x = nextX;
                    next.y = nextY;

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
UnionFind findComponents(const std::vector<ARESTA>& forest, int total_pixels) {
    UnionFind uf(total_pixels);
    for (const ARESTA& aresta : forest) {
        uf.union_sets(aresta.u, aresta.v);
    }
    return uf;
}

void segmentate(std::vector<ARESTA> &mst, int desired_segments) {
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
}

int main() {
    Undirected_graph g;
    const char* path = "images/templates/melanoma.jpg";
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


    size_t buffer_size = width * height * channels;
    unsigned char *output_data = new unsigned char[buffer_size];
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


    // ... (o restante do seu código para salvar a imagem com stbi_write_png e liberar a memória) ...
    const char* output_filename = "resultado_bordas2.png";
    int stride_in_bytes = width * channels;
    stbi_write_png(output_filename, width, height, channels, output_data, stride_in_bytes);

    delete[] output_data;

}
