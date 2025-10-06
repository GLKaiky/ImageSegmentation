#ifndef IMAGESEGMENTER_H
#define IMAGESEGMENTER_H

#include <string>
#include "src/graph/Undirected_graph.h"
#include "utils/FH.h"

class ImageSegmenter {
public:
    // Construtor
    ImageSegmenter();

    // O método principal que executa toda a lógica
    bool processImage(const std::string& inputPath, 
                      const std::string& outputPathBorders, 
                      const std::string& outputPathAverage, 
                      int k);

private:
    // Você pode mover funções auxiliares para cá se preferir
};

#endif // IMAGESEGMENTER_H