#include <iostream>
#include <string>
#include <chrono> 
#include "stb/image_processing.h"
#include "stb/second_image_processing.h"
#include "utils/IFT.h"
#include "stb/third_image_processing.h"

#define PATH "images/nebula.jpg"
#define OUTPUT_PATH "saida.png"
#define K_VALUE 400
#define MIN_SEGMENT_SIZE 500

/*
    K - Quanto Menor o Valor de K_Value, mais exigente com cores o algoritmo será
    MIN_SEGMENT_SIZE quanto menor o valor, menores os segmentos criados pelo algoritmo
*/

int main() {

    // 1. Instancia o processador
    ImageSegmenter segmenter;
    int width, height, channels;

    unsigned char* img = stbi_load(PATH, &width, &height, &channels, 3);

    SegmentationConfig config;
    config.blockSize = 4;
    config.sobelThreshold = 500.0; // Ajuste fino
    segmenter.setConfig(config);

    // 3. Carrega Imagem
    if (!segmenter.load(PATH)) {
        return -1;
    }

    // 4. Calcula Sementes (Externamente ou internamente)
    // Supondo que você use sua função salienceMap existente:
    std::vector<double> map = salienceMap(PATH); 
    // Nota: Idealmente, adapte salienceMap para não ler o arquivo do disco de novo
    // mas por hora, isso funciona.
    
    // Obtenha as dimensões da imagem carregada se precisar para getSeeds
    // Mas salienceMap/getSeeds atual já lida com isso.
    Seeds seeds = getSeeds(map, width, height); // Passe largura/altura corretas


    segmenter.saveSeedsDebug(OUTPUT_PATH, width, height, seeds);
    // 5. Roda e Salva
    segmenter.runWithSeeds(seeds, "output/resultado_final.png");

    std::cout << "Processamento concluido com sucesso!" << std::endl;
   
    IFT segmentador;
    std::vector<int> labels;

    // RODA O IFT AUTOMÁTICO
    segmentador.runAutomatic(img, width, height, labels);

    // SALVA O RESULTADO
    saveSegmentation(labels, img, width, height, "output/resultado_ift.png");
    
    stbi_image_free(img);
    
    return 0;
}