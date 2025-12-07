cmake_minimum_required(VERSION 3.18)

project(ImageSegmentation VERSION 1.0 LANGUAGES CXX)

# Garante C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON) 

# --- OTIMIZAÇÃO DE PERFORMANCE (O SEGREDO) ---

# 1. Define o modo "Release" como padrão se nenhum for especificado
# Isso ativa automaticamente flags de otimização básica (-O3 no GCC/Clang ou /O2 no MSVC)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to Release as none was specified.")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

# 2. Link Time Optimization (LTO/IPO)
# Permite que o compilador otimize chamadas de função entre arquivos diferentes.
# Muito útil para bibliotecas como STB e seu código de grafos.
include(CheckIPOSupported)
check_ipo_supported(RESULT result OUTPUT output)
if(result)
    message(STATUS "IPO/LTO enabled")
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
    message(WARNING "IPO/LTO not supported: ${output}")
endif()

# --- DEFINIÇÃO DO EXECUTÁVEL ---

add_executable(${PROJECT_NAME}
    main.cpp
    libs/stb/std_image_impl.cpp 
    libs/stb/stb_image_write_impl.cpp
    # Removi a duplicata do std_image_impl.cpp e o .h (headers não precisam estar aqui para compilar)
)

target_include_directories(${PROJECT_NAME} PRIVATE 
                            "libs"
                            "src" 
                        )

# --- FLAGS ESPECÍFICAS DE PROCESSADOR ---

if(MSVC)
    # Windows (Visual Studio)
    target_compile_options(${PROJECT_NAME} PRIVATE /O2 /Ob2 /Oi /Ot /GL)
else()
    # Linux/Mac (GCC ou Clang)
    # -O3: Otimização máxima
    # -march=native: Usa instruções específicas do SEU processador (AVX, SSE)
    # -ffast-math: Acelera contas de float (pode perder precisão infíma, mas ótimo para imagem)
    target_compile_options(${PROJECT_NAME} PRIVATE -O3 -march=native -ffast-math)
endif()