/******************************************************************************
 * @file: Secont_Image_Processing.h
 * @author: Kaiky França da Silva | Puc Minas
 * @brief: Este arquivo contém as funções responsáveis por converter
 * uma imagem digital em uma estrutura de dados de grafo direcionado e ponderado. A lógica principal
 * trata cada pixel como um vértice e cria arestas entre pixels vizinhos.
 * @version 0.5
 * @date 2025-12-05
 *****************************************************************************/


#ifndef IMAGE_SEGMENTER_H
#define IMAGE_SEGMENTER_H

#include "utils/Edmonds.h"        
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"
#include "utils/PixelConfiguration.h" 
#include "utils/Filters.h"            

#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <cmath>
#include <algorithm>
#include <queue>

// -----------------------------------------------------------------------------
// Configuração para ajuste fino (Sem recompilar lógica)
// -----------------------------------------------------------------------------
struct SegmentationConfig {
    int blockSize = 4;           // Tamanho do grid
    double sobelThreshold = 60.0;// Limite para considerar barreira
    double sobelPenalty = 2.0;   // Multiplicador de peso para barreiras
    double colorWeight = 0.5;    // Peso da diferença de cor (L*a*b*)
    double spatialWeight = 0.5;  // Peso da distância física
    double seedPriority = -99999.0; // Peso para ligar Ghost -> Seeds
};

// -----------------------------------------------------------------------------
// Classe Principal
// -----------------------------------------------------------------------------
class ImageSegmenter {
private:
    // Dados da Imagem
    int width, height, channels;
    std::unique_ptr<unsigned char[], void(*)(void*)> imgData; // Smart Pointer customizado para STB
    
    // Dados Processados
    std::vector<unsigned char> sobelData;
    std::vector<SuperPixel> superPixels;
    int gridW, gridH;

    // Configuração
    SegmentationConfig config;

    // Helpers Privados
    double getBoundaryMaxGradient(int x1, int y1, int x2, int y2) const {
        double maxGrad = 0.0;
        int bs = config.blockSize;

        if (x2 > x1) { // Vizinho Direita
            int boundaryX = std::min(x2 * bs, width - 1);
            int startY = y1 * bs;
            int endY = std::min((y1 + 1) * bs, height);
            
            for (int y = startY; y < endY; ++y) {
                int idx = y * width + boundaryX;
                if (sobelData[idx] > maxGrad) maxGrad = sobelData[idx];
            }
        } else if (y2 > y1) { // Vizinho Baixo
            int boundaryY = std::min(y2 * bs, height - 1);
            int startX = x1 * bs;
            int endX = std::min((x1 + 1) * bs, width);

            for (int x = startX; x < endX; ++x) {
                int idx = boundaryY * width + x;
                if (sobelData[idx] > maxGrad) maxGrad = sobelData[idx];
            }
        } else { // Diagonal (Simplificado: Pega o canto)
            int cx = std::min((x1 + 1) * bs, width - 1);
            int cy = std::min((y1 + 1) * bs, height - 1);
            maxGrad = sobelData[cy * width + cx];
        }
        return maxGrad;
    }

public:
    // Construtor: Inicializa ponteiro inteligente para STB
    ImageSegmenter() : width(0), height(0), channels(0), imgData(nullptr, stbi_image_free) {}

    // 1. Carregar Imagem
    bool load(const std::string& path) {
        int w, h, c;
        unsigned char* data = stbi_load(path.c_str(), &w, &h, &c, 3);
        if (!data) {
            std::cerr << "Erro: Nao foi possivel carregar " << path << std::endl;
            return false;
        }
        
        // Assume ownership do ponteiro
        imgData.reset(data); 
        width = w; height = h; channels = c;
        return true;
    }

    // 2. Configurar
    void setConfig(const SegmentationConfig& cfg) {
        this->config = cfg;
    }


    void saveSeedsDebug(const char* filename, int w, int h, const Seeds& seeds) {
    // Cria imagem RGB preenchida com CINZA (Zona Neutra/Dúvida)
    std::vector<unsigned char> debugImg(w * h * 3, 128); 

    // Pinta Sementes de OBJETO de VERDE
    for (int idx : seeds.obj) {
        debugImg[idx * 3 + 0] = 0;   // R
        debugImg[idx * 3 + 1] = 255; // G
        debugImg[idx * 3 + 2] = 0;   // B
    }

    // Pinta Sementes de FUNDO de VERMELHO
    for (int idx : seeds.backgroundObj) {
        debugImg[idx * 3 + 0] = 255; // R
        debugImg[idx * 3 + 1] = 0;   // G
        debugImg[idx * 3 + 2] = 0;   // B
    }

    stbi_write_png(filename, w, h, 3, debugImg.data(), w * 3);
    
}

    // Versão alternativa que aceita Seeds externas (Recomendado)
    void runWithSeeds(const Seeds& seeds, const std::string& outputPath) {
        // ... (passos 1 e 2 iguais acima) ...
        
        unsigned char* rawBlur = toGaussian_blur(imgData.get(), width, height, 3);
        rawBlur = toGaussian_blur(rawBlur, width, height, 3);

        unsigned char* rawSobel = sobelFilter(rawBlur, width, height, 3);

        apply_gamma(imgData.get(), width, height, 3, 1.5f);
        
        sobelData.assign(rawSobel, rawSobel + (width * height));
        delete[] rawBlur; 
        delete[] rawSobel;

        generateSuperPixels();
        std::vector<Edge> edges = buildGraph();

        int ghostRoot = superPixels.size();
        int nextId = edges.size();
        int gW = (width + config.blockSize - 1) / config.blockSize;

        // Injeta Arestas das Sementes
        for(int s : seeds.obj) {
            int spID = ((s / width) / config.blockSize) * gW + ((s % width) / config.blockSize);
            if (spID < (int)superPixels.size()) {
                edges.push_back({ghostRoot, spID, config.seedPriority, nextId++});
            }
        }
        for(int s : seeds.backgroundObj) {
            int spID = ((s / width) / config.blockSize) * gW + ((s % width) / config.blockSize);
            
            if (spID < (int)superPixels.size()) {
                edges.push_back({ghostRoot, spID, config.seedPriority, nextId++});
            }
        }

        std::vector<Edge> tree = Edmonds::solve(ghostRoot + 1, edges, ghostRoot);
        
        // Precisamos reconstruir a lógica de labels baseada nas seeds para pintar
        renderFromTreeAndSeeds(tree, seeds, outputPath);
    }

private:
    void generateSuperPixels() {
        int bs = config.blockSize;
        gridW = (width + bs - 1) / bs;
        gridH = (height + bs - 1) / bs;
        int numSP = gridW * gridH;

        superPixels.resize(numSP);

        for (int gy = 0; gy < gridH; ++gy) {
            for (int gx = 0; gx < gridW; ++gx) {
                int id = gy * gridW + gx;
                
                double sumL=0, sumA=0, sumB=0;
                double sumR=0, sumG=0, sumB_vis=0;
                int count = 0;

                int startY = gy * bs;
                int endY = std::min((gy + 1) * bs, height);
                int startX = gx * bs;
                int endX = std::min((gx + 1) * bs, width);

                for (int y = startY; y < endY; ++y) {
                    for (int x = startX; x < endX; ++x) {
                        int idx = (y * width + x) * 3;
                        unsigned char r = imgData[idx];
                        unsigned char g = imgData[idx+1];
                        unsigned char b = imgData[idx+2];

                        sumR += r; sumG += g; sumB_vis += b;
                        
                        CIELAB lab = RGBtoLab(r, g, b);
                        sumL += lab.L; sumA += lab.a; sumB += lab.b;
                        count++;
                    }
                }
                
                if (count == 0) count = 1;

                superPixels[id].id = id;
                superPixels[id].L = sumL / count;
                superPixels[id].a = sumA / count;
                superPixels[id].b = sumB / count;
                superPixels[id].avgR = (unsigned char)(sumR / count);
                superPixels[id].avgG = (unsigned char)(sumG / count);
                superPixels[id].avgB = (unsigned char)(sumB_vis / count);
            }
        }
    }

    std::vector<Edge> buildGraph() {
        std::vector<Edge> edges;
        // Estimativa de reserva: 4 vizinhos por nó
        edges.reserve(superPixels.size() * 4);

        int dx[] = {1,  0,  1, -1};
        int dy[] = {0,  1,  1,  1};
        double distFactor[] = {1.0, 1.0, 1.414, 1.414}; 
        
        int edgeIdCounter = 0;

        for (int gy = 0; gy < gridH; ++gy) {
            for (int gx = 0; gx < gridW; ++gx) {
                int u = gy * gridW + gx;

                for (int i = 0; i < 4; ++i) {
                    int nx = gx + dx[i];
                    int ny = gy + dy[i];

                    if (nx >= 0 && nx < gridW && ny >= 0 && ny < gridH) {
                        int v = ny * gridW + nx;

                        // Cálculos de Peso
                        double dL = superPixels[u].L - superPixels[v].L;
                        double dA = superPixels[u].a - superPixels[v].a;
                        double dB = superPixels[u].b - superPixels[v].b;
                        double colorDist = std::sqrt(dL*dL + dA*dA + dB*dB);

                        double barrier = getBoundaryMaxGradient(gx, gy, nx, ny);
                        
                        double edgePenalty = 0.0;
                        if (barrier > config.sobelThreshold) {
                            edgePenalty = barrier * config.sobelPenalty;
                        }

                        double weight = (colorDist * config.colorWeight) + 
                                        edgePenalty + 
                                        (distFactor[i] * config.spatialWeight);

                        // Grafo Direcionado: Insere ida e volta
                        edges.push_back({u, v, weight, edgeIdCounter++});
                        edges.push_back({v, u, weight, edgeIdCounter++});
                    }
                }
            }
        }
        return edges;
    }

    // Método interno para renderizar
    void renderFromTreeAndSeeds(const std::vector<Edge>& tree, const Seeds& seeds, const std::string& filename) {
        int numSPs = superPixels.size();
        
        // 1. Árvore de Adjacência
        std::vector<std::vector<int>> adj(numSPs);
        for (const auto& e : tree) {
            // Ignora arestas conectadas ao GhostNode (que tem ID >= numSPs)
            if (e.u < numSPs && e.v < numSPs) {
                adj[e.u].push_back(e.v);
            }
        }

        // 2. Propagar Labels (0=Fundo, 1=Objeto, 2=Unknown)
        std::vector<int> labels(numSPs, 2);
        std::queue<int> q;

        // Mapear pixels semente para SuperPixels
        auto mapPixelToSP = [&](int pixelIdx) {
            int py = pixelIdx / width;
            int px = pixelIdx % width;
            return (py / config.blockSize) * gridW + (px / config.blockSize);
        };

        for (int s : seeds.obj) {
            int sp = mapPixelToSP(s);
            if (sp < numSPs && labels[sp] == 2) { labels[sp] = 1; q.push(sp); }
        }
        for (int s : seeds.backgroundObj) {
            int sp = mapPixelToSP(s);
            if (sp < numSPs && labels[sp] == 2) { labels[sp] = 0; q.push(sp); }
        }

        // BFS na Árvore
        while(!q.empty()) {
            int u = q.front(); q.pop();
            for (int v : adj[u]) {
                if (labels[v] == 2) {
                    labels[v] = labels[u];
                    q.push(v);
                }
            }
        }

        // 3. Pintar
        std::vector<unsigned char> output(width * height * 3);
        int bs = config.blockSize;

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                int sp = (y / bs) * gridW + (x / bs);
                if (sp >= numSPs) sp = numSPs - 1;

                int idx = (y * width + x) * 3;
                int lbl = labels[sp];

                if (lbl == 1) { // Objeto -> Cor Média
                    output[idx]   = superPixels[sp].avgR;
                    output[idx+1] = superPixels[sp].avgG;
                    output[idx+2] = superPixels[sp].avgB;
                } else { // Fundo -> Preto ou escuro
                    output[idx]   = 0;
                    output[idx+1] = 0;
                    output[idx+2] = 0;
                }
            }
        }

        stbi_write_png(filename.c_str(), width, height, 3, output.data(), width * 3);
    }
    
};

#endif // IMAGE_SEGMENTER_H