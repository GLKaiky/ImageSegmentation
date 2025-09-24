#include <iostream>

#include "stb/stb_image.h"

struct rgb {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

int main() {
    int largura, altura, canais;
    const char* caminho_imagem = "images.jpg"; // Garanta que esta imagem exista

    // stbi_load retorna um ponteiro para os dados dos pixels
    unsigned char *dados = stbi_load(caminho_imagem, &largura, &altura, &canais, 0);

    if (dados == nullptr) {
        std::cerr << "Erro: Nao foi possivel carregar a imagem " << caminho_imagem << std::endl;
        return 1;
    }

    std::cout << "Imagem carregada: " << largura << "x" << altura << " com " << canais << " canais." << std::endl;

    // Garante que a imagem seja RGB para este exemplo
    if (canais < 3) {
        std::cerr << "A imagem precisa ter pelo menos 3 canais (RGB) para este exemplo." << std::endl;
        stbi_image_free(dados);
        return 1;
    }
    
    // --- A MÁGICA ACONTECE AQUI ---
    rgb cores;

    for(int x = 0; x < altura; ++x) {
        for(int y = 0; y< largura; ++y) {
            unsigned long indice = (x* largura + y) * canais;

            cores.r = dados[indice];
            cores.g = dados[indice + 1];
            cores.b = dados[indice+2];

            std::cout << (int) cores.r << (int) cores.g << (int) cores.b << std::endl;

        }
    }   

}