#include <stdio.h>
#include <malloc.h>
#include <string.h>

#define debug  printf
//#define debug _nodebug_ 

void _nodebug_(const char *format,...){}

typedef struct _png_chunck 
{
    uint32_t length;
    char type[4];
    char *data;
    uint32_t crc;
    struct _png_chunck * next;
}PNG_chunck;

typedef struct _PNG
{
    char header[8];
    PNG_chunck * chuncks;
    uint32_t chunck_count;
}PNG;


/**
 * 大端转小端
 * @ bytes 大端内存段
 * @ 内存长度 
 * return 小端内存段
 */
char *to_litend(char *bytes, int size)
{
    char * li = (char *)malloc(size);
    for (int i = 0; i< size ; i++) {
        li[i] = bytes[size-i-1];
    }
    for (int i = 0; i< size ; i++) {
        bytes[i] = li[i];
    }
    free(li);
    return bytes;
}


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
    char png_tag[] = {0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a};
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
        
        byte_count = fread((void*)&(temp_chunckp->length),4, 1, png_filep);
        
        byte_count = fread((void*)temp_chunckp->type,4, 1, png_filep);

        int length = *(int*)to_litend((char*)&temp_chunckp->length,4);

        debug("length[%d] type[%s]\n",length, temp_chunckp->type);
        if (byte_count < 0) {
            debug("read error, can't read chunck.\n");
            fclose(png_filep);
            return;
        }
        
        /* 读取chunck内容 */
        temp_chunckp->data = (char *)malloc(length);
        byte_count = fread((void*)temp_chunckp->data, length, 1, png_filep);
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
    debug("%d chuncks readed.\n",png->chunck_count);
    fclose(png_filep);
}

int main (int argc, char * argv[])
{
    if (argc < 2) {
        debug("no png file to read.\n");
        return 0;
    }
    PNG png;
    png_read(&png, argv[1]);
}
