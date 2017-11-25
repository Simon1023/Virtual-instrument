
#include <stdio.h>
#include <stdlib.h>

unsigned char* Load_bmp(const char *fname_s, int *Height, int *Width, int *Depth)
{
  FILE          *fp_s = NULL;    // source file handler
  unsigned int  x,y;             // for loop counter
  unsigned int  width, height;   // image width, image height
  unsigned char R, G, B, Y;      // color of R, G, B
  unsigned char *image_t = NULL; // source image array
  unsigned char *image_s = NULL; // source image array
  unsigned int y_avg;            // average of y axle
  unsigned int y_t;              // target of y axle
   /*
   unsigned char header[54] = {
     0x42,        // identity : B
     0x4d,        // identity : M
     0, 0, 0, 0,  // file size
     0, 0,        // reserved1
     0, 0,        // reserved2
     54, 0, 0, 0, // RGB data offset
     40, 0, 0, 0, // struct BITMAPINFOHEADER size
     0, 0, 0, 0,  // bmp width
     0, 0, 0, 0,  // bmp height
     1, 0,        // planes
     24, 0,       // bit per pixel
     0, 0, 0, 0,  // compression
     0, 0, 0, 0,  // data size
     0, 0, 0, 0,  // h resolution
     0, 0, 0, 0,  // v resolution
     0, 0, 0, 0,  // used colors
     0, 0, 0, 0   // important colors
   };  */
   
   unsigned char Palette[256*4];
   int nbpal;
   int padding;

   unsigned int file_size;           // file size
   unsigned int rgb_raw_data_offset; // RGB raw data offset
   unsigned short bit_per_pixel;     // bit per pixel
   unsigned short byte_per_pixel;    // byte per pixel

   fp_s = fopen(fname_s, "rb");
   if (fp_s == NULL) {
     printf("fopen fp_s error\n");
     return NULL;
   }

   // move offset to 10 to find rgb raw data offset
   fseek(fp_s, 10, SEEK_SET);
   fread(&rgb_raw_data_offset, sizeof(unsigned int), 1, fp_s);
   // move offset to 18    to get width & height;
   fseek(fp_s, 18, SEEK_SET);
   fread(&width,  sizeof(unsigned int), 1, fp_s);
   fread(&height, sizeof(unsigned int), 1, fp_s);
   // get  bit per pixel  //addr=0x1c
   fseek(fp_s, 28, SEEK_SET);
   fread(&bit_per_pixel, sizeof(unsigned short), 1, fp_s);
   byte_per_pixel = bit_per_pixel / 8;
   // move offset to rgb_raw_data_offset to get RGB raw data
   //fseek(fp_s, rgb_raw_data_offset, SEEK_SET);
   fseek(fp_s, rgb_raw_data_offset, SEEK_SET);

   //check width%4==0, if not width space need offset
   padding = (width * byte_per_pixel) % 4;
   if(padding!=0){
   		padding = 4-padding;
   }
   
   image_s = (unsigned char *)malloc((size_t)(width * byte_per_pixel + padding)* height);
   if (image_s == NULL) {
     printf("malloc image_s error\n");
     return NULL;
   }
   image_t = (unsigned char *)malloc((size_t)width * height);
   if (image_t == NULL) {
     printf("malloc image_t error\n");
     return NULL;
   }  

   fread(image_s, sizeof(unsigned char), (size_t)(long)( (width * byte_per_pixel + padding)* height) , fp_s);

   //vertical inverse algorithm
   y_avg = 0 + (height-1);
   if(byte_per_pixel>1)
   {
     for(y = 0; y != height; ++y)
     {
       y_t = y_avg - y;
       for(x = 0; x != width; ++x)
       {
         R = *(image_s + byte_per_pixel * (width *  y + x) + (padding * y) + 2);
         G = *(image_s + byte_per_pixel * (width *  y + x) + (padding * y) + 1);
         B = *(image_s + byte_per_pixel * (width *  y + x) + (padding * y) + 0);
         Y = 0.3*R+0.59*G+0.11*B;
         *(image_t + (width * y_t + x))=Y;
       }
     }
   }
   else if(byte_per_pixel==1)
   {
     for(y = 0; y != height; ++y)
     {
       y_t = y_avg - y;
       for(x = 0; x != width; ++x)
       {
         Y=*(image_s + ( (width+padding) *  y + x));
         *(image_t + (width * y_t + x))=Y;
       }
     }
   }
   else
   {
     printf("image format error\n");
     return NULL;
   }

   *Height=height;
   *Width=width;
   *Depth=(int)bit_per_pixel;
   fclose(fp_s);
   free(image_s);
   return image_t;
}

int Save_bmp_8bit(const char *fname_t, unsigned char *image_s, int height, int width)
{
   FILE          *fp_t = NULL;    // target file handler
   unsigned int  x,y;             // for loop counter
   unsigned char *image_t = NULL; // source image array
   unsigned char Y;         // Y
   unsigned int y_avg;            // average of y axle
   unsigned int y_t;              // target of y axle
   int padding, p;

   unsigned char header[54] = {
     0x42,        // identity : B
     0x4d,        // identity : M
     0, 0, 0, 0,  // file size
     0, 0,        // reserved1
     0, 0,        // reserved2
     54, 0, 0, 0, // RGB data offset
     40, 0, 0, 0, // struct BITMAPINFOHEADER size
     0, 0, 0, 0,  // bmp width
     0, 0, 0, 0,  // bmp height
     1, 0,        // planes
     24, 0,       // bit per pixel
     0, 0, 0, 0,  // compression
     0, 0, 0, 0,  // data size
     0, 0, 0, 0,  // h resolution
     0, 0, 0, 0,  // v resolution
     0, 0, 0, 0,  // used colors
     0, 0, 0, 0   // important colors
   };
   unsigned char Palette[256*4];
   int nbpal;

   unsigned int file_size;           // file size
   unsigned int rgb_raw_data_offset=1078; // RGB raw data offset
   unsigned short bit_per_pixel=8;     // bit per pixel
   unsigned short byte_per_pixel=1;    // byte per pixel

   // write to new bmp
   fp_t = fopen(fname_t, "wb");
   if (fp_t == NULL)
   {
     printf("fopen fname_t error\n");
     return 0;
   }

   // file size
   file_size = width * height * byte_per_pixel + rgb_raw_data_offset;
   header[2] = (unsigned char)(file_size & 0x000000ff);
   header[3] = (file_size >> 8)  & 0x000000ff;
   header[4] = (file_size >> 16) & 0x000000ff;
   header[5] = (file_size >> 24) & 0x000000ff;

   header[10] = (unsigned char)(rgb_raw_data_offset & 0x000000ff);
   header[11] = (rgb_raw_data_offset >> 8)  & 0x000000ff;


   // width
   header[18] = width & 0x000000ff;
   header[19] = (width >> 8)  & 0x000000ff;
   header[20] = (width >> 16) & 0x000000ff;
   header[21] = (width >> 24) & 0x000000ff;

   // height
   header[22] = height &0x000000ff;
   header[23] = (height >> 8)  & 0x000000ff;
   header[24] = (height >> 16) & 0x000000ff;
   header[25] = (height >> 24) & 0x000000ff;

   // bit per pixel
   header[28] = bit_per_pixel;

   // write header
   //if(byte_per_pixel==1)
   //{
   fwrite(header, sizeof(unsigned char), 54, fp_t); //rgb_raw_data_offset

   //Palette
   for(nbpal=0;nbpal<256;nbpal++)
   {
    Palette[4*nbpal+0]=nbpal;
    Palette[4*nbpal+1]=nbpal;
    Palette[4*nbpal+2]=nbpal;
    Palette[4*nbpal+3]=0;
   }
   fwrite(Palette, sizeof(unsigned char), 256*4, fp_t);

   /***  row data is saved to nearest multible of four
		determine empty bytes for row data  ***/
   padding = width%4;
   if(padding != 0) 
   {
       padding = 4 - padding;
   } 
   image_t = (unsigned char *)malloc((size_t)(width+padding) * height);
   if (image_t == NULL) {
     printf("malloc image_t error\n");
     return 0;
   }
     
   //vertical inverse algorithm
   y_avg = 0 + (height-1);
   for(y = 0; y < height; y++)
   {
     y_t = y_avg - y;
     for(x = 0; x < width; x++)
     {
       Y=*(image_s + (width * y  + x));
       *(image_t + ((width+padding) * y_t + x))=Y;
     }
     for(p=0;p<padding;p++){
         *(image_t + ( (width+padding)* y_t + x + p))=0;
     }
   }

   // write image
   fwrite(image_t, sizeof(unsigned char), (size_t)(long)(width + padding) * height, fp_t);

   fclose(fp_t);
   free(image_t);
   return 1;
}

int Save_bmp_24bit(const char *fname_t, unsigned char *image_s, int height, int width)
{
   FILE          *fp_t = NULL;    // target file handler
   unsigned int  x,y;             // for loop counter
   unsigned char *image_t = NULL; // source image array
   unsigned char R, G, B;         // R, G, B
   unsigned int y_avg;            // average of y axle
   unsigned int y_t;              // target of y axle
   int padding, p;

   unsigned char header[54] = {
     0x42,        // identity : B
     0x4d,        // identity : M
     0, 0, 0, 0,  // file size
     0, 0,        // reserved1
     0, 0,        // reserved2
     54, 0, 0, 0, // RGB data offset
     40, 0, 0, 0, // struct BITMAPINFOHEADER size
     0, 0, 0, 0,  // bmp width
     0, 0, 0, 0,  // bmp height
     1, 0,        // planes
     24, 0,       // bit per pixel
     0, 0, 0, 0,  // compression
     0, 0, 0, 0,  // data size
     0, 0, 0, 0,  // h resolution
     0, 0, 0, 0,  // v resolution
     0, 0, 0, 0,  // used colors
     0, 0, 0, 0   // important colors
   };
   unsigned char Palette[256*4];
   int nbpal;

   unsigned int file_size;           // file size
   unsigned int rgb_raw_data_offset=54; // RGB raw data offset
   unsigned short bit_per_pixel=24;     // bit per pixel
   unsigned short byte_per_pixel=3;    // byte per pixel

   // write to new bmp
   fp_t = fopen(fname_t, "wb");
   if (fp_t == NULL)
   {
     printf("fopen fname_t error\n");
     return 0;
   }

   // file size
   file_size = width * height * byte_per_pixel + rgb_raw_data_offset;
   header[2] = (unsigned char)(file_size & 0x000000ff);
   header[3] = (file_size >> 8)  & 0x000000ff;
   header[4] = (file_size >> 16) & 0x000000ff;
   header[5] = (file_size >> 24) & 0x000000ff;

   header[10] = (unsigned char)(rgb_raw_data_offset & 0x000000ff);
   header[11] = (rgb_raw_data_offset >> 8)  & 0x000000ff;


   // width
   header[18] = width & 0x000000ff;
   header[19] = (width >> 8)  & 0x000000ff;
   header[20] = (width >> 16) & 0x000000ff;
   header[21] = (width >> 24) & 0x000000ff;

   // height
   header[22] = height &0x000000ff;
   header[23] = (height >> 8)  & 0x000000ff;
   header[24] = (height >> 16) & 0x000000ff;
   header[25] = (height >> 24) & 0x000000ff;

   // bit per pixel
   header[28] = bit_per_pixel;

   // write header
   //if(byte_per_pixel==1)
   //{
   fwrite(header, sizeof(unsigned char), rgb_raw_data_offset, fp_t); //rgb_raw_data_offset

   //Palette
   //24 bit bmp dosen't need to set palette
   /*
   for(nbpal=0;nbpal<256;nbpal++)
   {
    Palette[4*nbpal+0]=nbpal;
    Palette[4*nbpal+1]=nbpal;
    Palette[4*nbpal+2]=nbpal;
    Palette[4*nbpal+3]=0;
   }
   fwrite(Palette, sizeof(unsigned char), 256*4, fp_t);*/
   

   /***  row data is saved to nearest multible of four
		determine empty bytes for row data  ***/
   padding = (width*byte_per_pixel)%4;
   if(padding != 0) 
   {
       padding = 4 - padding;
   } 
   image_t = (unsigned char *)malloc((size_t)(byte_per_pixel * width + padding) * height);
   if (image_t == NULL) {
     printf("malloc image_t error\n");
     return 0;
   }   
   
   //vertical inverse algorithm
   y_avg = 0 + (height-1);
   for(y = 0; y < height; y++)
   {
     y_t = y_avg - y;
     for(x = 0; x < width; x++)
     {
       R = *(image_s + byte_per_pixel * (width * y  + x) +2);
       G = *(image_s + byte_per_pixel * (width * y  + x) +1);
       B = *(image_s + byte_per_pixel * (width * y  + x) +0);
       *(image_t + ( byte_per_pixel * (width * y_t + x) + padding * y_t +2)) = R;
       *(image_t + ( byte_per_pixel * (width * y_t + x) + padding * y_t +1)) = G;
       *(image_t + ( byte_per_pixel * (width * y_t + x) + padding * y_t +0)) = B;
     }     
   }

   // write image
   fwrite(image_t, sizeof(unsigned char), (size_t)(long)(width * byte_per_pixel + padding) * height, fp_t);

   fclose(fp_t);
   free(image_t);
   return 1;
}

unsigned char * T8bitTo24bit(unsigned char *ima, size_t Height, size_t Width)
{
   int x,y;
   unsigned char *ima_dst;
   int byte_per_pixel=3;

   ima_dst=(unsigned char *)malloc(sizeof(unsigned char)*Height * Width * byte_per_pixel);

   if (ima_dst == NULL)
   {
     printf("malloc ima error\n");
     return NULL;
   }

   for(y=0;y<Height;y++)
   for(x=0;x<Width;x++)
   {
     *(ima_dst + byte_per_pixel * (Width * y + x) + 2) = *(ima+Width*y+x);   //R
     *(ima_dst + byte_per_pixel * (Width * y + x) + 1) = *(ima+Width*y+x);   //G
     *(ima_dst + byte_per_pixel * (Width * y + x) + 0) = *(ima+Width*y+x);   //B
   }
   return ima_dst;
}

unsigned char * T24bitTo8bit(unsigned char *ima, size_t Height, size_t Width)
{
   int x,y;
   unsigned char *ima_dst;
   int byte_per_pixel=3;
   float  R, G, B;

   ima_dst=(unsigned char *)malloc(sizeof(unsigned char)*Height * Width * byte_per_pixel);

   if (ima_dst == NULL)
   {
     printf("malloc ima error\n");
     return NULL;
   }

   for(y=0;y<Height;y++)
   for(x=0;x<Width;x++)
   {
     R = *(ima + byte_per_pixel * (Width * y + x) + 2);   //R
     G = *(ima + byte_per_pixel * (Width * y + x) + 1);   //G
     B = *(ima + byte_per_pixel * (Width * y + x) + 0);   //B
     *(ima_dst+Width*y+x) = (unsigned char)(0.3*R+0.59*G+0.11*B);
   }
   return ima_dst;
}
