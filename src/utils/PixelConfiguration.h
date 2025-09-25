#ifndef PIXELCONFIGURATION_H
#define PIXELCONFIGURATION_H
struct PixelColor {
    unsigned char r,g,b;
};

struct CIELAB {
    double L;
    double a;
    double b;
};

#endif