#include "Image.h"
#include <math.h>

Image::Image (unsigned long  width, unsigned long  height, unsigned int dpi) :
    width(width), height(height), dpi(dpi) {

    vector<vector<Color> > p(height, vector<Color>(width));
    pixels = p;
}

Image::Image (vector<vector<Color>> p) {
    pixels = p;
}

void Image::normalize () {
    unsigned long  height = pixels.size();
    unsigned long  width = pixels[0].size();
    double mx = 0;
    for (unsigned long  i = 0; i < height; i++) {
        for (unsigned long j = 0; j < width; j++) {
            mx = max(max(mx, pixels[i][j].r), max(pixels[i][j].g, pixels[i][j].b));
        }
    }
    if (mx != 0) {
        for (unsigned long  i = 0; i < height; i++) {
            for (unsigned long j = 0; j < width; j++) {
                pixels[i][j].r = (int)(pixels[i][j].r*(255.0/mx));
                pixels[i][j].g = (int)(pixels[i][j].g*(255.0/mx));
                pixels[i][j].b = (int)(pixels[i][j].b*(255.0/mx));
            }
        }
    }
}

void Image::antiAlias() {
    Image auxiliarImage(pixels[0].size(), pixels.size(), dpi);
    for (unsigned long  i = 1; i < height-1; i++) {
        for (unsigned long j = 1; j < width-1; j++) {
            auxiliarImage.pixels[i][j] = (
                        pixels[i+1][j]*1
                    +   pixels[i-1][j]*1
                    +   pixels[i][j+1]*1
                    +   pixels[i+1][j+1]*(1/sqrt(2))
                    +   pixels[i+1][j-1]*(1/sqrt(2))
                    +   pixels[i-1][j+1]*(1/sqrt(2))
                    +   pixels[i-1][j-1]*(1/sqrt(2))
            )*(1.0/(4+4/sqrt(2)));
        }
    }
    for (unsigned long  i = 0; i < height; i++) {
        for (unsigned long j = 0; j < width; j++) {
            pixels[i][j] = auxiliarImage.pixels[i][j];
        }
    }
}

void Image::saveToFile (const char * filename, int dpi) const{
    FILE *f;
    unsigned long  height = pixels.size();
    unsigned long  width = pixels[0].size();
    unsigned long  size = 4*width*height;
    unsigned long  filesize = 54 + size;

    int ppm = dpi*static_cast<int>(39.375);

    unsigned char bmpFileHeader[14] = {'B','M', 0,0,0,0, 0,0,0,0, 54,0,0,0};
    unsigned char bmpInfoHeader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0,24,0};

    bmpFileHeader[ 2] = (unsigned char)(filesize);
    bmpFileHeader[ 3] = (unsigned char)(filesize>>8);
    bmpFileHeader[ 4] = (unsigned char)(filesize>>16);
    bmpFileHeader[ 5] = (unsigned char)(filesize>>24);

    bmpInfoHeader[ 4] = (unsigned char)(width);
    bmpInfoHeader[ 5] = (unsigned char)(width>>8);
    bmpInfoHeader[ 6] = (unsigned char)(width>>16);
    bmpInfoHeader[ 7] = (unsigned char)(width>>24);

    bmpInfoHeader[ 8] = (unsigned char)(height);
    bmpInfoHeader[ 9] = (unsigned char)(height>>8);
    bmpInfoHeader[10] = (unsigned char)(height>>16);
    bmpInfoHeader[11] = (unsigned char)(height>>24);

    bmpInfoHeader[21] = (unsigned char)(size);
    bmpInfoHeader[22] = (unsigned char)(size>>8);
    bmpInfoHeader[23] = (unsigned char)(size>>16);
    bmpInfoHeader[24] = (unsigned char)(size>>24);

    bmpInfoHeader[25] = (unsigned char)(ppm);
    bmpInfoHeader[26] = (unsigned char)(ppm>>8);
    bmpInfoHeader[27] = (unsigned char)(ppm>>16);
    bmpInfoHeader[28] = (unsigned char)(ppm>>24);

    bmpInfoHeader[29] = (unsigned char)(ppm);
    bmpInfoHeader[30] = (unsigned char)(ppm>>8);
    bmpInfoHeader[31] = (unsigned char)(ppm>>16);
    bmpInfoHeader[32] = (unsigned char)(ppm>>24);

    f = fopen(filename,"wb");

    fwrite(bmpFileHeader,1,14,f);
    fwrite(bmpInfoHeader,1,40,f);

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            unsigned char color[3] = {(unsigned char) pixels[i][j].b,
                                      (unsigned char) pixels[i][j].g,
                                      (unsigned char) pixels[i][j].r};
            fwrite(color,1,3,f);
        }
    }

    fclose(f);
}
