#pragma once

#include <QImage>
#include <QColor>

#include <fstream>

#pragma pack(push, 1)
struct TGA_Header {
    char idlength;
    char colormaptype;
    char datatypecode;
    short colormaporigin;
    short colormaplength;
    char colormapdepth;
    short x_origin;
    short y_origin;
    short width;
    short height;
    char bitsperpixel;
    char imagedescriptor;
};
#pragma pack(pop)

class Image {
public:
    static QImage readFile(const char *filename);
private:
    Image();
    
    static unsigned char* data;
    static size_t width;
    static size_t height;
    static size_t bytespp;

    static bool load_rle_data(std::ifstream &in);
    static QImage read_tga_file(const char *filename);

    enum Format {
        GRAYSCALE = 1, RGB = 3, RGBA = 4
    };
};