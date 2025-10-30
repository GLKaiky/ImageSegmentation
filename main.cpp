#include <iostream>
#include <string>
#include <chrono> 
#include "stb/image_processing.h"

#define PATH "images/vinishow.jpg"
#define OUTPUT_PATH "/output/saida.png"
#define K_VALUE 500
#define MIN_SEGMENT_SIZE 600

int main() {
    std::cout << "Processando imagem: " << PATH << std::endl;
    std::cout << "K=" << K_VALUE << ", MinSize=" << MIN_SEGMENT_SIZE << std::endl;

    auto start = std::chrono::high_resolution_clock::now();

    processImage(PATH, OUTPUT_PATH, K_VALUE, MIN_SEGMENT_SIZE);
    auto stop = std::chrono::high_resolution_clock::now();

    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    
    std::chrono::duration<double> duration_sec = stop - start;

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Tempo de execucao: " << duration_ms.count() << " ms" << std::endl;
    std::cout << "Tempo de execucao: " << duration_sec.count() << " s" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
}