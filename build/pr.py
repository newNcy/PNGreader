#!/usr/bin/env python
# coding=utf-8
from PIL import Image
import sys
im = Image.open(sys.argv[1])
width = im.size[0]
height = im.size[1]
print ("/* width:%d */"%(width))
print ("/* height:%d */"%(height))
print(im.mode)
a = im.getpalette
b = im.getcolors
count = 0
for h in range(0, height):
    for w in range(0, width):
        pixel = im.getpixel((w, h))
        if pixel[0] + pixel[1] + pixel[2] > 255:
            print("+",end=''),
        else:
            print(".",end='')
    print('')

