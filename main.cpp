#include <iostream>
#include <string>
#include <chrono> 
#include "stb/image_processing.h"
#include "stb/second_image_processing.h"
#include "utils/IFT.h"
#include "stb/third_image_processing.h"

#define PATH "images/52577236876_7fe171479b_k.jpg"
#define OUTPUT_PATH "saida.png"
#define K_VALUE 400
#define MIN_SEGMENT_SIZE 500

/*
    K - Quanto Menor o Valor de K_Value, mais exigente com cores o algoritmo será
    MIN_SEGMENT_SIZE quanto menor o valor, menores os segmentos criados pelo algoritmo
*/
int main() {
    // ========================================================================
    // 1. ABORDAGEM BASEADA EM MST (Felzenszwalb & Huttenlocher)
    // ========================================================================
    // Aplica a segmentação baseada em grafos utilizando o critério de 
    // mesclagem de componentes (Kruskal modificado).
    // Parâmetros: Caminho, Saída, K (escala de observação), Tamanho Mínimo.
    processImage(PATH, OUTPUT_PATH, K_VALUE, MIN_SEGMENT_SIZE);


    // ========================================================================
    // 2. ABORDAGEM BASEADA EM ARBORESCÊNCIA (Edmonds-Chu-Liu)
    // ========================================================================
    // Nesta etapa, buscamos a Arborescência Geradora Mínima enraizada em
    // sementes específicas, ideal para garantir conectividade direcionada.
    
    ImageSegmenter segmenter;
    
    // Carregamento da imagem bruta para uso posterior no IFT
    int width, height, channels;
    unsigned char* img = stbi_load(PATH, &width, &height, &channels, 3);

    // Configuração dos parâmetros de sensibilidade do algoritmo de Edmonds
    SegmentationConfig config;
    config.blockSize = 5;       // Tamanho do bloco para análise local
    config.sobelThreshold = 500.0; // Limiar para detecção de gradientes fortes
    segmenter.setConfig(config);

    // Carrega a imagem no contexto do segmentador
    if (!segmenter.load(PATH)) {
        return -1; // Falha ao carregar recurso
    }

    // --- Etapa de Pré-Processamento e Extração de Sementes ---
    // Gera um mapa de saliência (intensidade + viés central) para identificar
    // automaticamente os objetos de interesse e o fundo.
    std::vector<double> map = salienceMap(PATH); 
    Seeds seeds = getSeeds(map, width, height); 

    // Execução do algoritmo de Edmonds usando as sementes extraídas
    // O resultado é salvo diretamente no disco.
    segmenter.runWithSeeds(seeds, "output/edmonds/resultado_final_edmonds.png");

    std::cout << "Processamento Edmonds concluido com sucesso!" << std::endl;
   

    // ========================================================================
    // 3. ABORDAGEM BASEADA EM IFT (Image Foresting Transform)
    // ========================================================================
    // Aplica a Transformada Imagem-Floresta, uma abordagem baseada em 
    // competição de caminhos de custo mínimo a partir de sementes/rótulos.
    
    IFT segmentador;
    std::vector<int> labels;

    // Executa a segmentação automática (custo de caminho ótimo)
    segmentador.runAutomatic(img, width, height, labels);

    // Pós-processamento: Salva o mapa de rótulos coloridos e as bordas
    saveSegmentation(labels, img, width, height, "output/ift/resultado_ift.png");
    saveIFTBoundaries(labels, width, height, "output/ift/bordas_ift.png");
    
    // Liberação de memória da imagem bruta carregada via stbi
    stbi_image_free(img);
    
    return 0;
}