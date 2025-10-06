// --- Includes ---
// ImGui (com os backends)
#include "libs/imgui/imgui.h"
#include "libs/imgui/backends/imgui_impl_glfw.h"
#include "libs/imgui/backends/imgui_impl_opengl3.h"

// Outras bibliotecas
#include <stdio.h>
#include <GLFW/glfw3.h> // Para a janela
#include "libs/tinyfiledialogs/tinyfiledialogs.h" // Para a seleção de arquivos

// Lógica do seu projeto
#include "src/stb/ImageSegmenter.h" // A classe que refatoramos no exemplo anterior
#include <string>
#include <vector>

// --- Função Principal ---
int main(int, char**) {
    // 1. Inicializar GLFW e criar a janela
    if (!glfwInit()) return 1;
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Segmentador de Imagem", NULL, NULL);
    if (window == NULL) return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Ativa V-Sync

    // 2. Inicializar Dear ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark(); // Define o tema escuro
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // 3. Variáveis de Estado da nossa GUI
    static int k_value = 850;
    static char input_path[2048] = "Nenhuma imagem selecionada";
    static std::string status_text = "Pronto.";
    bool processing = false;

    // Instância do seu algoritmo
    ImageSegmenter segmenter;

    // 4. Loop Principal da Aplicação
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Inicia um novo frame do ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 5. Desenhando a nossa janela
        {
            ImGui::Begin("Painel de Controle");

            ImGui::Text("Selecione a imagem de entrada:");

            // Botão para abrir o diálogo de seleção de arquivo
            if (ImGui::Button("Procurar...")) {
                char const * filterPatterns[2] = { "*.jpg", "*.png" };
                char const * selectedFile = tinyfd_openFileDialog(
                    "Escolha uma imagem",
                    "",
                    2,
                    filterPatterns,
                    NULL,
                    0
                );
                if (selectedFile) {
                    strcpy(input_path, selectedFile);
                }
            }
            ImGui::SameLine();
            ImGui::Text("%s", input_path);

            ImGui::Separator(); // Uma linha divisória

            ImGui::Text("Ajuste o parametro K:");
            ImGui::SliderInt("K", &k_value, 1, 2000);

            ImGui::Separator();

            // Desabilita o botão se já estiver processando
            if (processing) {
                ImGui::BeginDisabled();
            }

            if (ImGui::Button("Iniciar Segmentacao", ImVec2(200, 40))) { // Botão maior
                std::string current_path_str(input_path);
                if (current_path_str == "Nenhuma imagem selecionada") {
                    status_text = "Erro: Por favor, selecione uma imagem primeiro.";
                } else {
                    processing = true;
                    status_text = "Processando... Isso pode levar um momento.";
                    
                    // --- ATENÇÃO: BLOQUEIO DA GUI ---
                    // Em uma aplicação simples como esta, a GUI vai congelar aqui.
                    // A solução correta usa threads, mas para um executor simples, isso é aceitável.
                    bool success = segmenter.processImage(
                        current_path_str,
                        "resultado_bordas.png",
                        "resultado_cores_medias.png",
                        k_value
                    );
                    
                    if (success) {
                        status_text = "Concluido! Imagens salvas como 'resultado_bordas.png' e 'resultado_cores_medias.png'.";
                    } else {
                        status_text = "Erro durante o processamento da imagem.";
                    }
                    processing = false;
                }
            }

            if (processing) {
                ImGui::EndDisabled();
            }

            ImGui::Separator();
            ImGui::Text("Status:");
            ImGui::TextWrapped("%s", status_text.c_str()); // TextWrapped quebra a linha automaticamente

            ImGui::End();
        }

        // 6. Renderização
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // 7. Limpeza
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}