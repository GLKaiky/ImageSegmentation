#include <iostream>

// Inclui o cabeçalho da biblioteca stb_image para termos acesso às suas funções.
#include "stb/stb_image.h"

// (Opcional, mas recomendado)
// Criamos uma estrutura para guardar a cor de um pixel.
// Isso torna o código mais organizado do que usar variáveis soltas.
struct PixelColor {
    unsigned char r, g, b;
};

int main() {
    // --- 1. DECLARAÇÃO DE VARIÁVEIS E CARREGAMENTO ---

    int largura, altura, canais;
    const char* caminho_imagem = "images.jpg"; // Nome do arquivo de imagem

    // A função stbi_load tenta carregar a imagem.
    // Ela retorna um ponteiro para o primeiro byte dos dados dos pixels na memória.
    // E preenche as variáveis 'largura', 'altura' e 'canais' com as dimensões da imagem.
    unsigned char *dados = stbi_load(caminho_imagem, &largura, &altura, &canais, 0);

    // --- 2. VERIFICAÇÃO DE ERROS ---

    // É CRUCIAL verificar se o ponteiro 'dados' é nulo.
    // Se for, significa que a imagem não foi encontrada ou está corrompida.
    if (dados == nullptr) {
        std::cerr << "Erro: Nao foi possivel carregar a imagem " << caminho_imagem << std::endl;
        return 1; // Encerra o programa com um código de erro.
    }

    // --- 3. ITERAÇÃO SOBRE OS PIXELS ---

    std::cout << "Imagem carregada: " << largura << "x" << altura << " com " << canais << " canais." << std::endl;
    std::cout << "--- Lendo as cores dos 3 primeiros pixels da primeira linha ---" << std::endl;

    // O laço externo percorre as LINHAS da imagem, de y=0 (topo) até y=altura-1 (base).
    for (int y = 0; y < altura; ++y) {
        // O laço interno percorre as COLUNAS de cada linha, de x=0 (esquerda) até x=largura-1 (direita).
        for (int x = 0; x < largura; ++x) {
            
            // --- 4. A FÓRMULA MÁGICA ---
            // Esta fórmula converte a coordenada 2D (x, y) para um índice 1D na memória.
            unsigned long indice = (y * largura + x) * canais;

            // Pega os 3 bytes a partir do índice calculado para obter as cores R, G e B.
            PixelColor cor_do_pixel;
            cor_do_pixel.r = dados[indice];
            cor_do_pixel.g = dados[indice + 1];
            cor_do_pixel.b = dados[indice + 2];

            // Para este exemplo, vamos imprimir apenas os 3 primeiros pixels e parar.
            // Em um projeto real, você removeria este 'if' para processar a imagem inteira.
            if (y == 0 && x < 3) {
                 std::cout << "Pixel(" << x << ", " << y << "): ";
                 std::cout << "R=" << (int)cor_do_pixel.r << ", ";
                 std::cout << "G=" << (int)cor_do_pixel.g << ", ";
                 std::cout << "B=" << (int)cor_do_pixel.b << std::endl;
            }
        }
    }

    // --- 5. LIMPEZA DE MEMÓRIA ---
    
    // A memória alocada por stbi_load precisa ser liberada manualmente.
    // Se esquecermos esta linha, teremos um "vazamento de memória".
    stbi_image_free(dados);

    return 0; // Sucesso!
}