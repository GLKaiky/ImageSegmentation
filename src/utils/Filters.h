/******************************************************************************
 * @file: Filters.h
 * @author: Kaiky França da Silva | Puc Minas
 * @brief: Arquivo com os filtros de pré e pós processamento de imagem
 * @version 0.1
 * @date 2025-10-11
 *****************************************************************************/

#include "PixelConfiguration.h"
#include <math.h>
#include <algorithm>

#ifndef FILTERS_H
#define FILTERS_H


/** 
 * @brief Faz a conversão do padrão RGB para o padrão CIELAB (melhor percepção das cores)
 * @param R (red)
 * @param G (green)
 * @param B (blue)
**/
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

/**
 * @brief Função que vai multiplicar os valores RGB e converter para escala de cinza
 * @param imageData dados da imagem processada
 * @param width largura da imagem
 * @param height altura da imagem
 * @param channels canais da imagem (normalmente 3)
 */

unsigned char* toGray(unsigned char* imageData, int width, int height, int channels) {
    size_t bufferSize = width * height;
    unsigned char* outputData = new unsigned char[bufferSize];

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int input_index = (y * width + x) * channels;
            
            int output_index = y * width + x;

            unsigned char r = imageData[input_index];
            unsigned char g = imageData[input_index + 1];
            unsigned char b = imageData[input_index + 2];

            double grayScale = (0.299 * r) + (0.587 * g) + (0.114 * b);

            outputData[output_index] = static_cast<unsigned char>(grayScale);
        }
    }
    return outputData;
}

/**
 * @brief Função que vai aplicar os kernels do algoritmo de Sobel, para marcar e definir as bordas 
 * das imagems
 * @param originalData dados da imagem em seu estado atual
 * @param width Largura da imagem
 * @param height Altura da imagem
 * @param channels quantos canais a imagem possui
 */

unsigned char* sobelFilter(unsigned char* originalData, int width, int height, int channels) {
    size_t bufferSize = width * height;
    PixelColor pixel;
    int gx_Kernel[3][3] = {{-1, 0, 1}, 
                           {-2, 0, 2}, 
                           {-1, 0, 1}};

    int gy_Kernel[3][3] = {{-1, -2, -1}, 
                            {0, 0, 0}, 
                            {1, 2, 1}};

    double* Magnetude_Map = new double[bufferSize];
    unsigned char* outputData = new unsigned char[bufferSize];

    unsigned char* grayScaleData = toGray(originalData, width, height, channels);

    const int dy[] = {-1, -1, -1,  0, 0, 0,  1, 1, 1};
    const int dx[] = {-1,  0,  1, -1, 0, 1, -1, 0, 1};

    for(int y = 0; y<height; ++y) {
        for(int x = 0; x<width; ++x) {
            
            double sumX = 0.0, sumY = 0.0;
            int index = (y * width) + x;

            for(int i = 0; i<9; i++){
        
                int nextX = x + dx[i];
                int nextY = y + dy[i];

                if (nextX >= 0 && nextX < width && nextY >= 0 && nextY < height) {

                    int index_pixel = (nextY * width + nextX);

                    sumX += grayScaleData[index_pixel] * gx_Kernel[i/3][i%3];
                    sumY += grayScaleData[index_pixel] * gy_Kernel[i/3][i%3];

                }       
            }

            Magnetude_Map[index] = sqrt((sumX * sumX) + (sumY * sumY));
        }
    }

    double* PmaxValue = std::max_element(Magnetude_Map, Magnetude_Map + bufferSize);
    double maxValue = *PmaxValue;

    for(int i = 0; i < bufferSize; ++i) { 
        if (maxValue > 0) { 
            outputData[i] = static_cast<unsigned char>((Magnetude_Map[i] / maxValue) * 255.0);
        } else {
            outputData[i] = 0;
        }
    }
    delete[] Magnetude_Map;
    delete[] grayScaleData;
    
    return outputData;
}


/**
 * @brief Faz os calculos utilizando um kernel para espalhar as cores, evitar ruidos nas cores da imagem,
 * o que melhora a segmentação
 * @param originalData dados da imagem original
 * @param width largura da imagem
 * @param height altura da imagem
 * @param channels quantos canais a imagem possui
 */
unsigned char* toGaussian_blur(unsigned char* originalData ,int width, int height, int channels) {
    int kernel[3][3] {
        {1,2,1}, 
        {2,4,2}, 
        {1,2,1}
    };


    constexpr double kernel_weight = 16.0;

    size_t buffer_size = width * height * channels;

    unsigned char* outputData = new unsigned char[buffer_size];

    const int dy[] = {-1, -1, -1,  0, 0, 0,  1, 1, 1};
    const int dx[] = {-1,  0,  1, -1, 0, 1, -1, 0, 1};
    
    for(int y = 0; y<height; ++y) {
        for(int x = 0; x<width; ++x) {
            int index =((y * width) + x) * channels;

            double sumR = 0.0;
            double sumG = 0.0;
            double sumB = 0.0;

            for(int i = 0; i<9; i++){
        
                int nextX = x + dx[i];
                int nextY = y + dy[i];

                if (nextX >= 0 && nextX < width && nextY >= 0 && nextY < height) {

                    int index_pixel = (nextY * width + nextX) * channels;

                    double weight = kernel[i/3][i%3];
        
                    sumR += (originalData[index_pixel] * weight);
                    sumG += (originalData[index_pixel + 1] * weight);
                    sumB += (originalData[index_pixel + 2] * weight);

                    
                }
            }

            outputData[index] = static_cast<unsigned char>(sumR/kernel_weight);
            outputData[index+1] = static_cast<unsigned char>(sumG/kernel_weight);
            outputData[index+2] = static_cast<unsigned char>(sumB/kernel_weight);

        }

    }
    return outputData;   
}

#endif