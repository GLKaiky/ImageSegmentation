/******************************************************************************
 * @file: Image_Processing.cpp
 * @author: Kaiky França da Silva
 * @brief: Este arquivo contém as funções responsáveis por converter
 * uma imagem digital em uma estrutura de dados de grafo. A lógica principal
 * trata cada pixel como um vértice e cria arestas entre pixels vizinhos.
 * @version 0.1
 * @date 2025-09-25
 *****************************************************************************/

#include "stb/stb_image.h"
#include "graph/Undirected_graph.h"
#include <iostream>
#include <vector>
#include <utility>
#include <math.h>
#include "utils/PixelConfiguration.h"


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

    return {L, a, b};
}

/*
 * @brief Cria um grafo ponderado a partir de uma imagem.
 * @param imagePath Caminho para o arquivo de imagem.
 * @param graph Grafo que será populado. As arestas serão pares <vértice_vizinho, peso>.
 * @param pixel_colors Vetor que será preenchido com as cores de cada pixel/vértice.
*/
void create_graph(const char * imagePath, Undirected_graph<std::pair<int, int>> &graph, 
    std::vector<PixelColor>& pixelColor) {
    
    int height, width, chanels;
    
    /* capta o array de pixels da imagem, tamanho, largura e a quantidade de canais
        por hora serão forçado produzir apenas 3 canais
    */
    unsigned char * imageData = stbi_load(imagePath, &height, &width, &chanels, 3); 
    if(imageData == nullptr) {
        std::cerr << "Imagem vazia ou inválida " << imagePath << std::endl;
        return;
    }

    const long totalSize = height* width;

    graph.inicializar(totalSize);
    pixelColor.resize(totalSize);
    
}   