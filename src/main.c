#include <stdio.h>
#include "png.h"

int main (int argc, char * argv[])
{
    if (argc < 2) {
        printf("no png file to read.\n");
        return 0;
    }
    PNG png;
    png_read(&png, argv[1]);
    int w = png_get_width(&png);
    int h = png_get_height(&png);
    char dp = png_get_bit_depth(&png);
    char ct = png_get_colour_type(&png);
    char cm = png_get_compression_method(&png);
    char fm = png_get_filter_method(&png);
    char im = png_get_interlace_method(&png);
    printf("image width %d.\n",w);
    printf("image height %d.\n",h);
    printf("image bit depth: %d.\n",dp);
    printf("image colour type: %d.\n",ct);
    printf("image compression method %d.\n",cm);
    printf("image filter method %d.\n",fm);
    printf("image interlace method %d.\n",im);
    for(PNG_chunck *p=png.chuncks; p != NULL; p=p->next) {
        for (int j = 0;j<4;j++) {
            debug("%c",p->type[j]);
        }
        debug(" %d bytes.\n",p->length);
    }
    debug("%d chuncks readed.\n",png.chunck_count);
    png_get_text_list(&png);
    png_free(&png);
}
