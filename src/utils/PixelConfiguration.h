/******************************************************************************
 * @file: PixelConfiguration.h.cpp
 * @author: Kaiky França da Silva | Puc Minas
 * @brief: O arquivo que contém configurações convenientes relacionadas ao pixel das imagens como 
 * sua representação em RGB, CIELAB e a configuração das arestas baseada neles
 * @version 0.1
 * @date 2025-09-25
 *****************************************************************************/


#ifndef PIXELCONFIGURATION_H
#define PIXELCONFIGURATION_H
struct PixelColor {
    unsigned char r,g,b;
};

struct CIELAB {
    double L;
    double a;
    double b;
};

struct MeanColor {
    double r, g, b;
};

struct SuperPixel {
    int id;
    double L, a, b;         // Para o cálculo de distância (Grafo)
    unsigned char avgR, avgG, avgB; // NOVA: Para a pintura (Visualização)
    int x_center, y_center;
};

struct MapStats {
    double mean;
    double stdDev;
    double maxVal;
};

struct ARESTA {
    int u, v;
    double weight;
};

struct ColorSum {
    // Usamos long long para evitar overflow com segmentos grandes
    long long r = 0, g = 0, b = 0;
    int count = 0;
};

struct grayPixel {
    int pixelIndex;
    unsigned char scale;
};

struct pixelLocation {
    int id;
    long r, g, b;
};

struct Seeds{
    std::vector<int> backgroundObj;
    std::vector<int> obj;
};

struct Edge {
    int u;
    int v;
    double weight;
    int edgeId;
};

#endif