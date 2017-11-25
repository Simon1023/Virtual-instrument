#ifndef BMP_C_H
#define BMP_C_H

unsigned char* Load_bmp(const char *fname_s, int *Height, int *Width, int *Depth);
int Save_bmp_8bit(const char *fname_t, unsigned char *image_s, int height, int width);
int Save_bmp_24bit(const char *fname_t, unsigned char *image_s, int height, int width);
unsigned char * T8bitTo24bit(unsigned char *ima, size_t Height, size_t Width);
unsigned char * T24bitTo8bit(unsigned char *ima, size_t Height, size_t Width);

#endif
