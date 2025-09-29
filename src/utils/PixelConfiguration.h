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

struct ARESTA {
    int u, v;
    double weight;
};

#endif