#ifndef _PNG_H
#define _PNG_H
#include <malloc.h>
#include <string.h>
#include <zlib.h>
#include "chuncks.h"
#define byte  char
#define debug  printf
//#define debug _nodebug_ 
void _nodebug_(const char *format,...){}
#ifndef uint32_t
#define uint32_t unsigned int
#endif



/* colour type */
#define GRAY        0b000
#define RGB         0b010
#define INDEXED     0b011
#define GRAY_A      0b100
#define RGB_A       0b110



/**
 * PNG 里的 chunck 
 * length chunck长度 
 * type chunck类型
 * data chunck内容
 * crc crc校检值
 */
typedef struct _png_chunck 
{
    uint32_t length;
    byte type[4];
    byte *data;
    uint32_t crc;
    struct _png_chunck * next;
}PNG_chunck;



typedef struct _PNG
{
    byte header[8];
    PNG_chunck * chuncks;
    uint32_t chunck_count;
}PNG;


/**
 * 大端转小端
 * @ bytes 大端内存段
 * @ 内存长度 
 * return 小端内存段
 */
byte *to_litend(byte *bytes, int size)
{
    byte * li = (byte *)malloc(size);
    for (int i = 0; i< size ; i++) {
        li[i] = bytes[size-i-1];
    }
    for (int i = 0; i< size ; i++) {
        bytes[i] = li[i];
    }
    free(li);
    return bytes;
}
#define to_bigend to_litend
/** 读取png到内存 
 * @ png PNG结构体指针
 * @ filename 要读取的文件名
 */
void png_read(PNG * png, const char * filename)
{
    memset((char *)png, 0, sizeof(PNG));
    FILE * png_filep = fopen(filename, "rb");
    if (png_filep == NULL) {
        debug("can't open file \'%s\'.\n",filename);
        return;
    }
    int byte_count = 0;
    byte_count = fread((void*)png->header, 8, 1, png_filep);
    if (byte_count < 0) {
        debug("failed to read file header.\n");
        fclose(png_filep);
        return;
    }
    byte png_tag[] = {0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a};
    if (memcmp(png->header, png_tag, 8) != 0) {
        debug("not a png file.\n");
        fclose(png_filep);
        return;
    }

    PNG_chunck * last = NULL;
    png->chuncks = NULL;


    png->chunck_count = 0;
    while (1) {
        PNG_chunck * temp_chunckp  = (PNG_chunck*)malloc(sizeof(PNG_chunck));
        memset(temp_chunckp, 0, sizeof(PNG_chunck));

        /** 读取 PNG_chunk.length
         * PNG_chunck.type
         */
        
        byte_count = fread((void*)&(temp_chunckp->length),1, 4, png_filep);
        
        byte_count = fread((void*)temp_chunckp->type,1, 4, png_filep);

        int length = *(int*)to_litend((byte*)&temp_chunckp->length,4);

        //debug("length[%d] type[%s]\n",length, temp_chunckp->type);
        if (byte_count < 0) {
            debug("read error, can't read chunck.\n");
            fclose(png_filep);
            return;
        }
        
        /* 读取chunck内容 */
        temp_chunckp->data = (byte *)malloc(length);
        byte_count = fread((void*)temp_chunckp->data, 1, length, png_filep);
        if (byte_count < 0 ) {
            debug("can't read chunck data.\n");
            fclose(png_filep);
            return;
        }
        
        /* 读取crc校检 */
        byte_count = fread((void*)&temp_chunckp->crc, sizeof(temp_chunckp->crc), 1, png_filep);

        if (png->chuncks == NULL) {
            png->chuncks = last = temp_chunckp;
        }else {
            last->next = temp_chunckp;
            last = last->next;
        }
        last->next = NULL;
        png->chunck_count ++;
        if(length == 0) break;
    }

    fclose(png_filep);
}

/** 
 * 获取ihdr 
 * @ png 目标png
 */
PNG_CHUNCK_IHDR png_get_ihdr(PNG * png)
{
    PNG_CHUNCK_IHDR ihdr_data;
    memset(&ihdr_data, 0, sizeof(PNG_CHUNCK_IHDR));
    if (png == NULL) return ihdr_data;
    PNG_chunck * ihdr = png->chuncks;
    if (ihdr == NULL) return ihdr_data;
    memcpy(&ihdr_data,ihdr->data,ihdr->length);
    to_litend((char*)&ihdr_data.width,4);
    to_litend((char*)&ihdr_data.height,4);
    return ihdr_data;
}
/** 获取图像 
 * @ png 图像结构体
 */
unsigned int png_get_width(PNG * png)
{
    return png_get_ihdr(png).width;
}
/** 获取图像高度
 * @ png 图像结构体
 */
unsigned int png_get_height(PNG * png)
{
    return png_get_ihdr(png).height;
}

/** 获取bit depth
 * @ png 目标png结构体指针
 */
char png_get_bit_depth(PNG * png)
{
    return png_get_ihdr(png).bit_depth;
}

char png_get_colour_type(PNG *png)
{
    return png_get_ihdr(png).colour_type;
}

char png_get_compression_method(PNG *png)
{
    return png_get_ihdr(png).compression_method;
}
char png_get_filter_method(PNG *png)
{
    return png_get_ihdr(png).filter_method;
}

char png_get_interlace_method(PNG *png)
{
    return png_get_ihdr(png).interlace_method;
}


void png_get_text_list(PNG * png)
{
    PNG_chunck * ch = png->chuncks;
    while (ch!= NULL) {
        if (strcmp(ch->type,"tEXt") == 0) {
            for (int i= 0;i<ch->length;i++) {
                if(ch->data[i] == 0) printf("\n");
                printf("%c",ch->data[i]);
            }
            printf("\n");
        }
        ch = ch->next;
    }
}

char * png_get_pixels(PNG* png)
{
    PNG_chunck * ch = png->chuncks;
    while (ch) {
        if(!strcmp(ch->type,"IDAT")) {

        }
        ch = ch->next;
    }
    return NULL;
}
/** 释放png 各个chunck
 * @ chunck 要释放的chunck指针
 */
void png_chunck_free(PNG_chunck * chunck)
{
    if (chunck == NULL) return;
    png_chunck_free(chunck->next);
    free(chunck->data);
    free(chunck);
    return;
}

/* 释放png */
void png_free(PNG *png)
{
    png_chunck_free(png->chuncks);
}

#endif
