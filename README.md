# Segmentação de Imagens com Grafos
<p align="center">
<img alt="C++ Badge" src="https://img.shields.io/badge/C++-%2300599c?style=for-the-badge&logo=cplusplus&logoColor=white">
<img alt="Graph Theory Badge" src="https://img.shields.io/badge/Graph%20Theory-MST%20%26%20Arborescence-green?style=for-the-badge">
</p>

**Trabalho Prático da disciplina de Teoria dos Grafos e Computabilidade**  
**Pontifícia Universidade Católica de Minas Gerais**  
**Prof. Matheus B. Pereira - 2025/2**

Este projeto implementa métodos de segmentação de imagens baseados em algoritmos de **árvores geradoras mínimas/máximas** (para grafos não-direcionados) e **arborescências mínimas/máximas** (para grafos direcionados).

## 📋 Objetivos do Projeto

O trabalho está dividido em duas entregas principais:

### 1ª Entrega - Árvores Geradoras (Grafos Não-Direcionados)
- ✅ Implementação dos algoritmos de Kruskal e Prim
- ✅ Aplicação em grafos construídos a partir de imagens
- ✅ Segmentação baseada em árvores geradoras mínimas/máximas
- 📅 **Entrega:** 02/11/2025

### 2ª Entrega - Arborescências (Grafos Direcionados)  
- 🔄 Implementação do algoritmo de Edmonds/Chu-Liu
- 🔄 Aplicação em grafos direcionados para segmentação
- 🔄 Comparação de resultados com trabalhos existentes
- 📅 **Entrega:** 08/12/2025
- 🎤 **Apresentações:** 09/12 e 10/12/2025

## 👥 Integrantes do Grupo


[Kaiky França](https://github.com/GLKaiky)
[Giuseppe Cordeiro](https://github.com/giusfds) 
[Beltrano de Souza](https://github.com/usuario-beltrano)

## 🚀 Tecnologias e Ferramentas

- **C++17** - Linguagem principal
- **CMake** - Sistema de build multiplataforma  
- **stb_image** - Biblioteca para carregamento de imagens
- **União-Busca (Union-Find)** - Para algoritmo de Kruskal
- **Heap Binário** - Para algoritmo de Prim

## 📦 Pré-requisitos

### Windows
- **Git:** [https://git-scm.com/](https://git-scm.com/)
- **CMake:** [https://cmake.org/download/](https://cmake.org/download/) 
  - ⚠️ Marcar "Add CMake to the system PATH" durante instalação
- **Visual Studio Community 2019+** com workload "Desenvolvimento para desktop com C++"

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

### 1. Clone o Repositório
```bash
git clone https://github.com/GLKaiky/ImageSegmentation
cd ImageSegmentation
```

### 2. Configure com CMake
```bash
cmake -B build
```

### 3. Compile o Projeto
```bash
cmake --build build
```

### 4. Execute o Programa

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
1. Coloque sua imagem de teste na pasta `images/`
2. Formatos suportados: PNG, JPG, BMP, TGA
3. Para melhores resultados, use imagens de:
   - 🏥 **Médicas** (radiografias, ressonâncias)
   - 🛰️ **Sensoriamento remoto** (satélites, drones)  
   - 🌆 **RGB comuns** (fotografias convencionais)

### Execução
```bash
# Exemplo com imagem médica
./build/ImageSegmentation --input images/brain_scan.png --algorithm kruskal --output results/

# Exemplo com imagem de satélite  
./build/ImageSegmentation --input images/satellite.jpg --algorithm prim --segments 5
```

### Parâmetros Disponíveis
- `--input`: Caminho para imagem de entrada
- `--algorithm`: Algoritmo a usar (`kruskal`, `prim`)
- `--output`: Diretório de saída dos resultados
- `--segments`: Número desejado de segmentos (padrão: 4)
- `--weight-type`: Tipo de peso das arestas (`euclidean`, `color-diff`, `intensity`)

## 📁 Estrutura do Projeto

```
ImageSegmentation/
├── CMakeLists.txt              # Configuração do build
├── README.md                   # Este arquivo  
├── docs/                       # Documentação
│   ├── relatorio.tex          # Relatório em LaTeX
│   └── slides_apresentacao.pdf # Slides da apresentação
├── src/                        # Código fonte
│   ├── main.cpp               # Programa principal
│   ├── algorithms/            # Implementação dos algoritmos
│   │   ├── kruskal.cpp       # Algoritmo de Kruskal
│   │   ├── prim.cpp          # Algoritmo de Prim
│   │   └── edmonds.cpp       # Algoritmo de Edmonds (2ª entrega)
│   ├── graph/                 # Estruturas de grafos
│   │   ├── graph.cpp         # Classe base do grafo
│   │   ├── undirected_graph.cpp # Grafo não-direcionado
│   │   └── directed_graph.cpp   # Grafo direcionado
│   └── utils/                 # Utilitários
│       ├── image_loader.cpp   # Carregamento de imagens
│       ├── union_find.cpp     # Estrutura União-Busca
│       └── segmentation.cpp   # Funções de segmentação
├── libs/                      # Bibliotecas externas
│   └── stb/                   # Biblioteca stb_image
├── images/                    # Imagens de teste
│   ├── medical/              # Imagens médicas
│   ├── satellite/            # Imagens de satélite  
│   └── common/               # Imagens RGB comuns
├── results/                   # Resultados da segmentação
└── tests/                     # Testes unitários
```

## 🔬 Metodologia de Segmentação

### Construção do Grafo
1. **Vértices**: Cada pixel da imagem representa um vértice
2. **Arestas**: Conectam pixels vizinhos (4-conectividade ou 8-conectividade)
3. **Pesos**: Calculados baseados em:
   - Diferença de intensidade/cor entre pixels
   - Distância euclidiana no espaço RGB
   - Gradiente local da imagem

### Extração da Partição
- MST/Arborescence é calculada sobre o grafo
- Arestas de maior peso são removidas para criar segmentos
- Componentes conexas resultantes formam os segmentos finais

## 📊 Avaliação e Resultados

### Critérios de Avaliação
- **Qualidade do código (20%)**: Estruturação, comentários, eficiência
- **Qualidade do texto (30%)**: Relatório técnico bem estruturado
- **Correção da solução (50%)**: Funcionamento correto dos algoritmos

### Métricas de Comparação
- Tempo de execução dos algoritmos
- Qualidade da segmentação (análise visual)
- Comparação com métodos tradicionais (watershed, k-means)

## 📚 Referências

- Böther, M., et al. "Efficiently Computing Directed Minimum Spanning Trees." ALENEX 2023.
- Edmonds, J. "Optimum Branchings." Journal of Research of the National Bureau of Standards, 1967.
- Gabow, H. N., et al. "Efficient algorithms for finding minimum spanning trees." Combinatorica, 1986.
- Tarjan, R. E. "Finding Optimum Branchings." Networks, 1977.

## 📝 Notas Importantes

- ⚠️ **Cópias serão sumariamente zeradas**
- 📄 **Relatório obrigatório**: Máximo 4 páginas no formato JBCS
- 🎯 **Entrega das fontes .tex é obrigatória**
- 🎤 **Apresentação individual**: Cada membro será questionado sobre sua contribuição

## 📞 Suporte

Em caso de dúvidas sobre o projeto, entre em contato com os integrantes do grupo através dos links do GitHub acima.