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



// Constantes estáticas para facilitar
static const double DIST_ORTHO = 1.0;
static const double DIST_DIAG = 1.41421356;
static const double INF_VAL = std::numeric_limits<double>::max();

// 1. Função Genérica de EDT (Baseada em L do CIELAB)
// Agora retorna o mapa de distâncias para quem quiser usar
inline std::vector<double> computeGlobalEDT(const unsigned char* img, int w, int h) {
    int numPixels = w * h;
    std::vector<double> dist(numPixels);

    for (int i = 0; i < numPixels; ++i) {
        unsigned char r = img[i * 3];
        unsigned char g = img[i * 3 + 1];
        unsigned char b = img[i * 3 + 2];

        CIELAB lab = RGBtoLab(r, g, b); 
        
        // Critério de "Objeto": Cinza Médio (Pulmão/Pneumonia)
        if (lab.L > 30.0 && lab.L < 85.0) { 
            dist[i] = INF_VAL; 
        } else {
            dist[i] = 0.0; 
        }
    }

    // Passada Frente
    for (int y = 1; y < h - 1; ++y) {
        for (int x = 1; x < w - 1; ++x) {
            int i = y * w + x;
            if (dist[i] == 0.0) continue; 

            double d_up   = dist[(y - 1) * w + x] + DIST_ORTHO;
            double d_left = dist[y * w + (x - 1)] + DIST_ORTHO;
            double d_ul   = dist[(y - 1) * w + (x - 1)] + DIST_DIAG;
            double d_ur   = dist[(y - 1) * w + (x + 1)] + DIST_DIAG;

            dist[i] = std::min({dist[i], d_up, d_left, d_ul, d_ur});
        }
    }

    // Passada Trás
    for (int y = h - 2; y >= 1; --y) {
        for (int x = w - 2; x >= 1; --x) {
            int i = y * w + x;
            if (dist[i] == 0.0) continue;

            double d_down  = dist[(y + 1) * w + x] + DIST_ORTHO;
            double d_right = dist[y * w + (x + 1)] + DIST_ORTHO;
            double d_dl    = dist[(y + 1) * w + (x - 1)] + DIST_DIAG;
            double d_dr    = dist[(y + 1) * w + (x + 1)] + DIST_DIAG;

            dist[i] = std::min({dist[i], d_down, d_right, d_dl, d_dr});
        }
    }
    return dist;
}

// 2. Função para extrair sementes desse mapa
inline Seeds extractSeedsFromMap(const std::vector<double>& distMap, int w, int h) {
    Seeds seeds;
    double maxDist = 0.0;
    
    for (double d : distMap) {
        if (d < INF_VAL && d > maxDist) maxDist = d;
    }

    if (maxDist < 1.0) return seeds;

    double threshold = maxDist * 0.5; 
    int step = 6; 

    for (int y = step; y < h - step; y += step) {
        for (int x = step; x < w - step; x += step) {
            int idx = y * w + x;
            
            if (distMap[idx] > threshold) {
                seeds.obj.push_back(idx);
            }
            else if (distMap[idx] == 0.0) {
                if (x < w*0.1 || x > w*0.9 || y < h*0.1 || y > h*0.9) {
                    seeds.backgroundObj.push_back(idx);
                }
            }
        }
    }
    return seeds;
}

std::vector<double> salienceMap(const char *imagePath) {
    int width, height, channels;
    unsigned char *data = stbi_load(imagePath, &width, &height, &channels, 3);
    if (!data) return {};

    int numPixels = width * height;
    std::vector<double> saliency(numPixels);
    double maxSal = 0;

    // Centro da imagem (para priorizar o meio e ignorar bordas/marcador R)
    double centerX = width / 2.0;
    double centerY = height / 2.0;
    
    // Constante para Gaussiana (Ajuste de foco)
    double sigma = width * 0.35; 
    double twoSigmaSq = 2 * sigma * sigma;

    for (int i = 0; i < numPixels; ++i) {
        int idx = i * 3;
        
        // 1. Obter Luminância (0..1)
        double normalizedL = data[idx] / 255.0;
         
        double intensityScore = 0.0;
        
        if (normalizedL < 0.4) {
            intensityScore = 0.0; 
        } else {
            // Valoriza o escuro (quanto menor L, maior o score)
            intensityScore = normalizedL; 
        }

        // 3. Peso Espacial (Center Bias)
        int y = i / width;
        int x = i % width;
        double dx = x - centerX;
        double dy = y - centerY;
        double distCenterSq = dx*dx + dy*dy; // Distância ao quadrado
        
        // Gaussian falloff
        double centerWeight = exp(-(distCenterSq) / twoSigmaSq);

        // Saliência Final
        double finalVal = intensityScore * centerWeight;

        saliency[i] = finalVal;
        if(finalVal > maxSal) maxSal = finalVal;
    }

    // Normaliza
    if(maxSal > 0) {
        for(int i=0; i<numPixels; i++) saliency[i] /= maxSal;
    }

    stbi_image_free(data);
    return saliency;
}

// ----------------------------------------------------------------------------
// 2. Extração de Sementes (Compatível com o Mapa acima)
// ----------------------------------------------------------------------------
Seeds getSeeds(const std::vector<double>& map, int width, int height) {
    Seeds seeds;
    if (map.empty()) return seeds;

    // Estatísticas Básicas
    double sum = 0.0;
    double maxVal = 0.0;
    for (double v : map) {
        sum += v;
        if (v > maxVal) maxVal = v;
    }
    double mean = sum / map.size();

    // Desvio Padrão
    double sqSum = 0.0;
    for (double v : map) sqSum += (v - mean) * (v - mean);
    double stdDev = std::sqrt(sqSum / map.size());

    // Limiares (Ajustados para o mapa de "Escuros Centrais")
    // Objeto: Regiões muito escuras no centro (Alta saliência nesse algoritmo)
    double objThresh = mean + stdDev; 
    
    // Fundo: Regiões claras ou nas bordas (Baixa saliência)
    double bkgThresh = mean * 0.5;

    int step = 6; 
    int margin = 2;

    for (int y = margin; y < height - margin; y += step) {
        for (int x = margin; x < width - margin; x += step) {
            int idx = y * width + x;
            double val = map[idx];

            int objVotes = 0;
            int bkgVotes = 0;

            // Votação 3x3
            for (int ky = -1; ky <= 1; ++ky) {
                for (int kx = -1; kx <= 1; ++kx) {
                    int nIdx = (y + ky) * width + (x + kx);
                    if (map[nIdx] > objThresh) objVotes++;
                    if (map[nIdx] < bkgThresh) bkgVotes++;
                }
            }

            // Decisão
            if (val > objThresh && objVotes >= 5) {
                seeds.obj.push_back(idx);
            }
            else if (val < bkgThresh && bkgVotes >= 5) {
                // Proteção extra para garantir fundo nas bordas
                bool isEdge = (x < width * 0.15 || x > width * 0.85 || 
                               y < height * 0.15 || y > height * 0.85);
                
                if (isEdge || val < 0.01) {
                    seeds.backgroundObj.push_back(idx);
                }
            }
        }
    }
    return seeds;
}

void apply_gamma(unsigned char* imgData, int width, int height, int channels, float gamma) {
     
    unsigned char lut[256];
    float invGamma = 1.0f / gamma;

    for (int i = 0; i < 256; i++) {
        float normalized = i / 255.0f;
        float value = pow(normalized, invGamma) * 255.0f;
        
        if (value > 255) value = 255;
        lut[i] = (unsigned char)value;
    }

    // Aplica na imagem inteira
    int totalBytes = width * height * channels;
    for (int i = 0; i < totalBytes; i++) {
        imgData[i] = lut[imgData[i]];
    }
    stbi_write_png("output/output_gamma.png", width, height, channels, imgData, width*3);

}

#endif