# Versão mínima do CMake e nome do projeto
cmake_minimum_required(VERSION 3.18)
project(ImageSegmentation LANGUAGES CXX C)

# Define o padrão do C++ para C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Dependências Externas ---
# Encontra a biblioteca GLFW, que é necessária para criar a janela
find_package(glfw3 3.3 REQUIRED)
# Encontra a biblioteca OpenGL
find_package(OpenGL REQUIRED)


# --- Arquivos-Fonte do Projeto ---

# Lista todos os arquivos .cpp do Dear ImGui que precisam ser compilados
set(IMGUI_SOURCES
    libs/imgui/imgui.cpp
    libs/imgui/imgui_draw.cpp
    libs/imgui/imgui_tables.cpp
    libs/imgui/imgui_widgets.cpp
    libs/imgui/backends/imgui_impl_glfw.cpp
    libs/imgui/backends/imgui_impl_opengl3.cpp
)

# Arquivo-fonte da biblioteca de diálogo de arquivos
set(TINYFILEDIALOGS_SOURCES
    libs/tinyfiledialogs/tinyfiledialogs.c
)

# Adicione aqui os seus próprios arquivos .cpp do algoritmo
# Ex: ImageSegmenter.cpp, Undirected_graph.cpp, etc.
set(ALGORITHM_SOURCES
    src/stb/image_processing.cpp
    # ImageSegmenter.cpp # Descomente e adicione os seus

)

# --- Executável Principal ---

# Cria o executável principal com todos os fontes necessários
add_executable(${PROJECT_NAME}
    main.cpp
    ${ALGORITHM_SOURCES}
    ${IMGUI_SOURCES}
    ${TINYFILEDIALOGS_SOURCES}
)

# --- Configuração de Inclusão e Vínculo ---

# CORRIGIDO
# Define os diretórios de "include" para que o compilador encontre os headers (.h)
target_include_directories(${PROJECT_NAME} PRIVATE
    .                     # Diretório raiz do projeto
    ${glfw3_INCLUDE_DIRS} # Diretório de include do GLFW
    libs                  # Adiciona a pasta 'libs' como um todo
    libs/imgui            # Adiciona a pasta específica do imgui
    libs/imgui/backends   # Adiciona a pasta de backends do imgui
    libs/stb              # Adiciona a pasta do stb
    libs/tinyfiledialogs  # Adiciona a pasta do tinyfiledialogs
    src
)

# Vincula (linka) o executável com as bibliotecas necessárias
target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw # A biblioteca GLFW
    OpenGL::GL # A biblioteca OpenGL
)

# Adiciona bibliotecas específicas do Windows, se aplicável
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE gdi32 imm32)
endif()

# Define uma macro para a implementação do stb_image em um único lugar
# Isso evita erros de "definição múltipla" do linker
target_compile_definitions(${PROJECT_NAME} PRIVATE STB_IMAGE_IMPLEMENTATION STB_IMAGE_WRITE_IMPLEMENTATION)