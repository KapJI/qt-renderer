#include <iostream>

#include <QString>

#include "image.h"

unsigned char* Image::data;
size_t Image::width;
size_t Image::height;
size_t Image::bytespp;

Image::Image() {
}

QImage Image::read_tga_file(const char *filename) {
    data = NULL;
    std::ifstream in;
    in.open(filename, std::ios::binary);
    if (!in.is_open()) {
        std::cerr << "can't open file " << filename << "\n";
        in.close();
        return QImage();
    }
    TGA_Header header;
    in.read((char*)&header, sizeof(header));
    if (!in.good()) {
        in.close();
        std::cerr << "an error occured while reading the header\n";
        return QImage();
    }
    width = header.width;
    height = header.height;
    bytespp = header.bitsperpixel >> 3;
    if (width <= 0 || height <= 0 || (bytespp != GRAYSCALE && bytespp != RGB && bytespp != RGBA)) {
        in.close();
        std::cerr << "bad bpp (or width/height) value\n";
        return QImage();
    }
    unsigned long nbytes = bytespp * width * height;
    data = new unsigned char[nbytes];
    if (header.datatypecode == 2 || header.datatypecode == 3) {
        in.read((char *)data, nbytes);
        if (!in.good()) {
            in.close();
            std::cerr << "an error occured while reading the data\n";
            return QImage();
        }
    } else if (header.datatypecode == 10 || header.datatypecode == 11) {
        if (!load_rle_data(in)) {
            in.close();
            std::cerr << "an error occured while reading the data\n";
            return QImage();
        }
    } else {
        in.close();
        std::cerr << "unknown file format " << (int)header.datatypecode << "\n";
        return QImage();
    }
    QImage res(width, height, QImage::Format_RGB32);
    for (size_t x = 0; x < width; ++x) {
        for (size_t y = 0; y < height; ++y) {
            unsigned char bgra[4];
            bgra[3] = 0;
            memcpy(bgra, data + (x + y * width) * bytespp, bytespp);
            if (bytespp == GRAYSCALE) {
                bgra[2] = bgra[1] = bgra[0];
            }
            QRgb color = *(unsigned int*)bgra;
            res.setPixel(x, y, color);
        }
    }
    if (!(header.imagedescriptor & 0x20)) {
        res = res.mirrored();
    }
    if (header.imagedescriptor & 0x10) {
        res = res.mirrored(true, false);
    }
    std::cerr << filename << ": " << width << "x" << height << "/" << bytespp * 8 << "\n";
    in.close();
    delete[] data;
    return res;
}

bool Image::load_rle_data(std::ifstream &in) {
    unsigned long pixelcount = width * height;
    unsigned long currentpixel = 0;
    unsigned long currentbyte  = 0;
    char colorbuffer[4];
    do {
        unsigned char chunkheader = 0;
        chunkheader = in.get();
        if (!in.good()) {
            std::cerr << "an error occured while reading the data\n";
            return false;
        }
        if (chunkheader < 128) {
            chunkheader++;
            for (size_t i = 0; i < chunkheader; i++) {
                in.read(colorbuffer, bytespp);
                if (!in.good()) {
                    std::cerr << "an error occured while reading the header\n";
                    return false;
                }
                for (size_t t = 0; t < bytespp; t++) {
                    data[currentbyte++] = colorbuffer[t];
                }
                currentpixel++;
                if (currentpixel > pixelcount) {
                    std::cerr << "Too many pixels read\n";
                    return false;
                }
            }
        } else {
            chunkheader -= 127;
            in.read(colorbuffer, bytespp);
            if (!in.good()) {
                std::cerr << "an error occured while reading the header\n";
                return false;
            }
            for (size_t i = 0; i < chunkheader; i++) {
                for (size_t t = 0; t < bytespp; t++) {
                    data[currentbyte++] = colorbuffer[t];
                }
                currentpixel++;
                if (currentpixel > pixelcount) {
                    std::cerr << "Too many pixels read\n";
                    return false;
                }
            }
        }
    } while (currentpixel < pixelcount);
    return true;
}

QImage Image::readFile(const char *filename) {
    QString file(filename);
    if (file.endsWith(QString(".tga"), Qt::CaseInsensitive)) {
        return Image::read_tga_file(filename);
    } else {
        return QImage(filename);
    }
}