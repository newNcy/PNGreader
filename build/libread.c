#include <stdio.h>


#include <png.h>
#define PNG_BYTES_TO_CHECK 4

int load_png_image( const char *filepath,int *rw, int *rh, char * ct, char *pixels)
{
        FILE *fp;
        png_structp png_ptr;
        png_infop info_ptr;
        png_bytep* row_pointers;
        char buf[PNG_BYTES_TO_CHECK];
        int w, h, x, y, temp, color_type;

        fp = fopen( filepath, "rb" );
        if( fp == NULL ) { 
                return 0;
        }
        
        png_ptr = png_create_read_struct( PNG_LIBPNG_VER_STRING, 0, 0, 0 );
        info_ptr = png_create_info_struct( png_ptr );

        setjmp( png_jmpbuf(png_ptr) ); 
        /* 读取PNG_BYTES_TO_CHECK个字节的数据 */
        temp = fread( buf, 1, PNG_BYTES_TO_CHECK, fp );
        /* 若读到的数据并没有PNG_BYTES_TO_CHECK个字节 */
        if( temp < PNG_BYTES_TO_CHECK ) {
                fclose(fp);
                png_destroy_read_struct( &png_ptr, &info_ptr, 0);
                return 0;
        }
        /* 检测数据是否为PNG的签名 */
        temp = png_sig_cmp( (png_bytep)buf, (png_size_t)0, PNG_BYTES_TO_CHECK );
        /* 如果不是PNG的签名，则说明该文件不是PNG文件 */
        if( temp != 0 ) {
                fclose(fp);
                png_destroy_read_struct( &png_ptr, &info_ptr, 0);
                return 0;
        }
        
        /* 复位文件指针 */
        rewind( fp );
        /* 开始读文件 */
        png_init_io( png_ptr, fp ); 
        /* 读取PNG图片信息和像素数据 */
        png_read_png( png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, 0 );
        /* 获取图像的色彩类型 */
        *ct = color_type = png_get_color_type( png_ptr, info_ptr );
        /* 获取图像的宽高 */
        *rw = w = png_get_image_width( png_ptr, info_ptr );
        *rh = h = png_get_image_height( png_ptr, info_ptr );
        /* 获取图像的所有行像素数据，row_pointers里边就是rgba数据 */
        row_pointers = png_get_rows( png_ptr, info_ptr );
        /* 根据不同的色彩类型进行相应处理 */
        int k = 0;
        switch( color_type ) {
        case PNG_COLOR_TYPE_RGB_ALPHA:
            printf("RGBA\n");
                for( y=0; y<h; ++y ) {
                        for( x=0; x<w*4; ) {
                                /* 以下是RGBA数据，需要自己补充代码，保存RGBA数据 */
                                pixels[k++] = row_pointers[y][x++]; // red
                                pixels[k++] = row_pointers[y][x++]; // green
                                pixels[k++] = row_pointers[y][x++]; // blue
                                pixels[k++] = row_pointers[y][x++]; // alpha
                                int all = pixels[k-2] +pixels[k-3] +pixels[k-4];
                               // if(all>255) printf("+");
                                //else printf(".");
                                char r,g,b,a;
                                r = pixels[k-4];
                                g = pixels[k-3];
                                b = pixels[k-2];
                                a = pixels[k-1];
                                //printf("%02x:%02x:%02x:%02x ",r,g,b,a);
                                char o = (r+g+b)>255?'+':'-';
                                printf("%c%c",o,o);
                        }
                        printf("\n");
                }
                break;

        case PNG_COLOR_TYPE_RGB:
            printf("RGB\n");
                for( y=0; y<h; ++y ) {
                        for( x=0; x<w*3; ) {
                                pixels[k++] = row_pointers[y][x++]; // red
                                pixels[k++] = row_pointers[y][x++]; // green
                                pixels[k++] = row_pointers[y][x++]; // blue
                        }
                }
                break;
        /* 其它色彩类型的图像就不读了 */
        default:
                fclose(fp);
                png_destroy_read_struct( &png_ptr, &info_ptr, 0);
                return 0;
        }
        png_destroy_read_struct( &png_ptr, &info_ptr, 0);
        return 0;
}

int main (int argc,char*argv[])
{
    char pixel[4096] = {0};
    char colour_type = 0;
    int w,h; 
    load_png_image(argv[1],&w,&h,&colour_type, pixel);
    printf("w[%d]\nh[%d]\nct[%d]\n",w,h,colour_type);
}
