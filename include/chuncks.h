#ifndef _CHUNCKS_H
#define _CHUNCKS_H

struct _chunck_IHDR_
{
    int width;
    int height;
    char bit_depth;
    char colour_type;
    char compression_method;
    char filter_method;
    char interlace_method;
};
typedef struct _chunck_IHDR_ PNG_CHUNCK_IHDR;


struct _chunck_TEXT_
{
    char *keyword;
    char *string;
};
#endif
