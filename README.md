-----

# Segmentação de Imagens com Grafos
<p>
    <a href="#">
        <img alt="Linguagem: C++" src="https://img.shields.io/badge/C++-%2300599c?style=for-the-badge&logo=cplusplus&logoColor=white">
    </a>
    <br>
    <a href="#">
        <img alt="Grafos" src="https://img.shields.io/badge/Grafos-2ea44f?style=for-the-badge">
    </a>
    <br>
    <a href="https://www.gnu.org/licenses/gpl-3.0">
        <img alt="License: GPL v3" src="https://img.shields.io/badge/License-GPLv3-blue.svg?style=for-the-badge">
    </a>
</p>

Este projeto implementa o algoritmo de segmentação de imagens de **Felzenszwalb & Huttenlocher**, um método eficiente que modela a imagem como um grafo e agrupa pixels em regiões de forma perceptualmente significativa, baseado em um critério de união derivado da teoria de Árvores Geradoras Mínimas (MST).

## 📋 Objetivos do Projeto

O objetivo principal é implementar o algoritmo de Felzenszwalb & Huttenlocher para segmentação de imagens. O projeto também explora o uso de filtros de pré-processamento, como o **Gaussian Blur**, para suavizar a imagem e melhorar a qualidade dos segmentos resultantes, reduzindo o ruído e a super-segmentação.

## 👥 Integrantes do Grupo

  - [Kaiky França](https://github.com/GLKaiky)
  - [Luís Fernando Rdorigues Braga](https://github.com/Trinalsolid)
  - [Mateus Resende](https://github.com/Mateus-Resende-Ottoni)
  - [Mateus Ribeiro](https://github.com/L0L0VIS)
  - [Raquel Motta](https://github.com/raksmotta)
  - [Domynic Barros Lima](https://github.com/domynicbl)
### Agradecimentos

- Obrigado pelo `README.md` - [Giuseppe Cordeiro](https://github.com/giusfds)

## 🚀 Tecnologias e Ferramentas

  - **C++17**: Linguagem principal para performance e controle.
  - **CMake**: Sistema de build multiplataforma para fácil compilação.
  - **stb\_image**: Biblioteca *single-header* para carregamento e salvamento de imagens.

## 📦 Pré-requisitos

### Windows

  - **Git:** [https://git-scm.com/](https://git-scm.com/)
  - **CMake:** [https://cmake.org/download/](https://cmake.org/download/)
      - ⚠️ Marcar "Add CMake to the system PATH" durante a instalação.
  - **Visual Studio Community 2019+** com o workload "Desenvolvimento para desktop com C++".

### Linux (Ubuntu/Debian)

```bash
sudo apt-get update
sudo apt-get install build-essential g++ cmake git
```

### macOS

```bash
# Com Homebrew
brew install cmake git
# Xcode Command Line Tools
xcode-select --install
```

## ⚙️ Compilação e Execução

### 1\. Clone o Repositório

```bash
git clone https://github.com/GLKaiky/ImageSegmentation
cd ImageSegmentation
```

### 2\. Configure com CMake

Cria a pasta `build` com os arquivos de projeto.

```bash
cmake -B build
```

### 3\. Compile o Projeto

Compila o código fonte e gera o executável dentro da pasta `build`.

```bash
cmake --build build
```

### 4\. Execute o Programa

**Linux/macOS:**

```bash
./build/ImageSegmentation
```

**Windows:**

```bash
.\build\Debug\ImageSegmentation.exe
```

## 🖼️ Como Usar

### Preparação da Imagem

1.  Coloque sua imagem de teste na pasta `input/`.
2.  Formatos suportados: PNG, JPG, BMP, TGA.

### Execução

O programa é controlado via linha de comando, ajustando os parâmetros do algoritmo.

```bash
# Exemplo básico com valores padrão
./build/ImageSegmentation --input input/val2017/test.jpg --output output/result.png

# Exemplo ajustando os parâmetros do algoritmo
./build/ImageSegmentation --input input/val2017/brain_scan.png --output output/brain_segmented.png --k 500 --sigma 0.8 --min-size 100
```

### Parâmetros Disponíveis

  - `--input`: (Obrigatório) Caminho para a imagem de entrada.
  - `--output`: (Obrigatório) Caminho para salvar a imagem segmentada.
  - `--k`: (Opcional) Constante de escala para o limiar. Valores maiores criam segmentos maiores. Padrão: `300`.
  - `--sigma`: (Opcional) Desvio padrão do filtro Gaussiano aplicado antes da segmentação. Ajuda a suavizar a imagem. Padrão: `0.5`.
  - `--min-size`: (Opcional) Tamanho mínimo de um componente na fase de pós-processamento. Regiões menores que isso são unidas a regiões vizinhas. Padrão: `50`.

## 📁 Estrutura do Projeto

```
/
├── .gitignore
├── CMakeLists.txt
├── LICENSE
├── README.md
├── build/
├── input/
│   └── val2017/
├── libs/
│   └── stb/
│       ├── stb_image.h
│       ├── stb_image_impl.cpp
│       ├── stb_image_write.h
│       └── stb_image_write_impl.cpp
├── main.cpp
├── main.py
├── output/
├── src/
│   ├── graph/
│   │   ├── Directed_graph.h
│   │   ├── Graph.h
│   │   └── Undirected_graph.h
│   ├── stb/
│   │   └── image_processing.h
│   └── utils/
│       ├── FH.h
│       ├── Filters.h
│       ├── PixelConfiguration.h
│       └── UnionFind.h
└── venv/
```

## 🔬 Metodologia de Segmentação

O algoritmo de Felzenszwalb & Huttenlocher opera em duas fases principais:

### 1\. Construção e Processamento do Grafo

1.  **Vértices e Arestas**: A imagem é tratada como um grafo não-direcionado $G=(V, E)$, onde cada pixel é um vértice $v \in V$ e as arestas $e \in E$ conectam pixels vizinhos (usando 8-conectividade).

2.  **Pesos das Arestas**: O peso $w(e)$ de uma aresta que conecta dois pixels é a diferença de cor ou intensidade entre eles (e.g., distância Euclidiana no espaço de cor LAB e Bordas do Algoritmo de Sobel).

3.  **Ordenação**: As arestas são ordenadas em ordem não-decrescente de peso.

4.  **Critério de Fusão**: O algoritmo processa cada aresta e decide se deve unir os dois componentes (segmentos) que ela conecta. Dois componentes $C_1$ e $C_2$ são unidos se o peso da aresta $w(e)$ for pequeno em relação à diferença interna dos componentes, definida pelo predicado:

    $$w(e) \le \min(\text{Int}(C_1) + \tau(C_1), \text{Int}(C_2) + \tau(C_2))$$

    Onde:

      - $\text{Int}(C)$ é a **diferença interna** do componente $C$ (o maior peso de aresta dentro da MST de $C$).
      - $\tau(C) = k / |C|$ é um **limiar** que depende do tamanho $|C|$ do componente e de uma constante $k$ definida pelo usuário.

### 2\. Pós-processamento

Após a segmentação inicial, arestas conectando componentes com tamanho inferior ao `min-size` são reavaliadas para uni-los a componentes vizinhos, evitando a criação de regiões muito pequenas e ruidosas.

## 📊 Avaliação e Resultados

### Critérios de Avaliação

  - **Qualidade do código (20%)**: Estruturação, comentários, eficiência.
  - **Qualidade do texto (30%)**: Relatório técnico bem estruturado.
  - **Correção da solução (50%)**: Funcionamento correto do algoritmo e resultados consistentes.

## 📚 Referências

  - **Felzenszwalb, P. F., & Huttenlocher, D. P. (2004). Efficient graph-based image segmentation. *International journal of computer vision*, 59(2), 167-181.**
  - Gabow, H. N., et al. "Efficient algorithms for finding minimum spanning trees." *Combinatorica*, 1986.

## 📞 Suporte

Em caso de dúvidas sobre o projeto, entre em contato com os integrantes do grupo através dos links do GitHub acima.

## 📜 Licença

Copyright (c) 2025 Kaiky França

Este projeto é um Software Livre: você pode redistribuí-lo e/ou modificá-lo sob os termos da **GNU General Public License** conforme publicada pela Free Software Foundation, seja a versão 3 da Licença, ou (a seu critério) qualquer versão posterior.

Este programa é distribuído na esperança de que seja útil, mas SEM NENHUMA GARANTIA; sem mesmo a garantia implícita de COMERCIALIZAÇÃO ou ADEQUAÇÃO A UM DETERMINADO FIM. Consulte a GNU General Public License para mais detalhes.

Você deve ter recebido uma cópia da GNU General Public License junto com este programa. Se não, veja <https://www.gnu.org/licenses/>.

O texto completo da licença está disponível no arquivo [LICENSE](LICENSE).
