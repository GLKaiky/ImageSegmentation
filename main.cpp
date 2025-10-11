#include <iostream>
#include <string>
#include "stb/image_processing.h"
struct rgb {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

#define PATH "input/val2017/W.jpeg"
#define OUTPUT_PATH "/output/saida.png"
#define K_VALUE 1200
#define MIN_SEGMENT_SIZE 1500

int main() {
    processImage(PATH, OUTPUT_PATH, K_VALUE, MIN_SEGMENT_SIZE);
}