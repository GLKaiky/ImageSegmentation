#include "PixelConfiguration.h"
#include <math.h>

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
    size_t bufferSize = width * height * channels;
    PixelColor pixel;
    unsigned char * outputData  = new unsigned char[bufferSize];

    for(int y = 0; y< height; ++y){
        for(int x = 0; x < width; ++x){
            int index = ((y * width) + x) * channels;

            pixel.r = imageData[index];
            pixel.g = imageData[index+1];
            pixel.b = imageData[index+2];


            double grayScale = (0.299 * pixel.r) + (0.587 * pixel.g) + (0.114 * pixel.b);
            outputData[index] = static_cast<unsigned char>(grayScale);
            outputData[index+1] = static_cast<unsigned char>(grayScale);
            outputData[index+2] = static_cast<unsigned char>(grayScale);
            
        }
    }
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

            outputData[index] = static_cast<unsigned char>(sumR/16.0);
            outputData[index+1] = static_cast<unsigned char>(sumG/16.0);
            outputData[index+2] = static_cast<unsigned char>(sumB/16.0);

        }

    }
    return outputData;   
}

#endif