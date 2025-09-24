# Image Segmentation

Um projeto base em C++ para trabalho prático da disciplina de Teoria de Grafos e Computabilidade. Este projeto utiliza `CMake` como sistema de build e a biblioteca `stb_image` para carregamento de imagens, garantindo uma compilação simples e sem dependências externas em múltiplos sistemas operacionais.

## Tecnologias Utilizadas

  * **C++17**
  * **CMake** - Sistema de build multiplataforma
  * **stb\_image** - Biblioteca de arquivo único para carregamento de imagens

-----

## Integrantes do Grupo

* [Kaiky França](https://github.com/GLKaiky)
* [Ciclana da Silva](https://github.com/usuario-ciclana)
* [Beltrano de Souza](https://github.com/usuario-beltrano)

## Pré-requisitos

Antes de começar, garanta que você tenha as ferramentas necessárias para o seu sistema operacional.

### Para Windows

  * **Git:** [https://git-scm.com/](https://git-scm.com/)
  * **CMake:** [https://cmake.org/download/](https://cmake.org/download/)
      * Durante a instalação, marque a opção "Add CMake to the system PATH".
  * **Compilador C++:** A maneira mais fácil é instalar o **Visual Studio Community 2019 (ou superior)**.
      * Durante a instalação, selecione a carga de trabalho (workload) **"Desenvolvimento para desktop com C++"**.

### Para Linux (Ubuntu/Debian)

  * **Git, CMake e Ferramentas de Build:** A maioria pode ser instalada com um único comando no terminal.
    ```bash
    sudo apt-get update
    sudo apt-get install build-essential g++ cmake git
    ```

-----

## Como Compilar e Executar

O processo de compilação é padronizado pelo CMake, tornando os passos quase idênticos no Windows e no Linux.

**1. Clone o Repositório**

Abra seu terminal ou PowerShell e clone o projeto. O `stb_image` já está incluído.

```bash
git clone <URL_DO_SEU_REPOSITORIO_NO_GITHUB>
cd ImageSegmentation
```

**2. Configure o Projeto com CMake**

Este comando cria uma pasta `build` e prepara os arquivos de compilação.

```bash
cmake -B build
```

**3. Compile o Projeto**

Este comando invoca o compilador (Make no Linux, MSBuild no Windows) para construir o executável dentro da pasta `build`.

```bash
cmake --build build
```

**4. Execute o Programa**

O executável estará dentro da pasta `build`.

  * **No Linux:**
    ```bash
    ./build/ImageSegmentation
    ```
  * **No Windows (Terminal/PowerShell):**
    ```bash
    .\build\Debug\ImageSegmentation.exe
    ```
    *(O caminho pode ser `.\build\Release\ImageSegmentation.exe` dependendo da configuração).*

-----

## Como Usar

Atualmente, o programa está configurado para carregar uma imagem com o nome `sua_imagem.png` que deve estar na raiz do projeto.

1.  **Adicione uma imagem:** Coloque o arquivo de imagem que você deseja processar na pasta principal do projeto (a mesma do `CMakeLists.txt`).
2.  **Ajuste o código:** Certifique-se de que o nome do arquivo no `main.cpp` corresponde ao nome da sua imagem, ou renomeie sua imagem.

<!-- end list -->

```cpp
// dentro de main.cpp
const char* caminho_imagem = "sua_imagem.png";
```

-----

## Estrutura do Projeto

```
.
├── CMakeLists.txt         # Arquivo de configuração do build
├── README.md              # Este arquivo
├── main.cpp               # Ponto de entrada principal do programa
└── libs/
    └── stb/
        ├── stb_image.h          # Biblioteca para carregar imagens
        └── stb_image_impl.cpp   # Arquivo de implementação da stb
```