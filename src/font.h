#ifndef __FONT__
#define __FONT__

// FONT //

typedef struct font_char {
   int l;
   int* e;
   float* p;
} font_char;

font_char font[] = {
/* ÿ */ [0xFF] = { 6,(int[]){0,3,2,1,3,2,4,5,6,7,1,0,},(float[]){2.0,0.0,0.0,0.0,0.0,4.0,2.0,4.0,0.5,3.5,1.5,0.5,0.5,0.5,1.5,3.5,}},
/* ! */ [0x21] = { 8,(int[]){1,0,3,1,0,2,2,3,5,4,4,7,7,6,6,5,},(float[]){0.4,1.2,0.4,1.0,0.6,1.2,0.6,1.0,0.4,1.4,0.6,1.4,0.6,3.0,0.4,3.0,}},
/* # */ [0x23] = { 4,(int[]){0,1,2,3,4,5,6,7,},(float[]){-0.0,2.5,2.0,2.5,0.5,3.0,0.5,1.0,-0.0,1.5,2.0,1.5,1.5,3.0,1.5,1.0,}},
/* + */ [0x2B] = { 4,(int[]){4,1,5,3,0,4,2,5,},(float[]){0.5,2.0,1.5,2.0,1.0,2.5,1.0,1.5,1.0,2.0,1.0,2.0,}},
/* - */ [0x2D] = { 1,(int[]){0,1,},(float[]){0.5,2.0,1.5,2.0,}},
/* : */ [0x3A] = { 8,(int[]){1,0,3,1,0,2,2,3,5,4,7,5,4,6,6,7,},(float[]){0.4,0.9,0.6,0.9,0.4,1.1,0.6,1.1,0.4,2.9,0.6,2.9,0.4,3.1,0.6,3.1,}},
/* Z */ [0x5A] = { 3,(int[]){0,1,2,0,1,3,},(float[]){2.0,4.0,-0.0,0.0,0.0,4.0,2.0,0.0,}},
/* Y */ [0x59] = { 3,(int[]){0,2,1,3,2,4,},(float[]){2.0,4.0,0.0,4.0,1.0,2.0,1.0,2.0,1.0,0.0,}},
/* X */ [0x58] = { 2,(int[]){0,2,1,3,},(float[]){2.0,4.0,0.0,4.0,-0.0,0.0,2.0,0.0,}},
/* W */ [0x57] = { 4,(int[]){1,2,3,2,0,1,4,3,},(float[]){2.0,4.0,2.0,0.0,1.0,2.0,0.0,0.0,0.0,4.0,}},
/* V */ [0x56] = { 2,(int[]){0,1,2,1,},(float[]){2.0,4.0,1.0,-0.0,0.0,4.0,}},
/* U */ [0x55] = { 5,(int[]){0,1,1,2,2,3,4,3,5,4,},(float[]){2.0,4.0,2.0,0.5,1.5,-0.0,0.5,-0.0,0.0,0.5,0.0,4.0,}},
/* Q */ [0x51] = { 9,(int[]){2,1,0,2,0,3,3,4,4,5,6,5,7,6,1,7,8,9,},(float[]){2.0,3.5,0.5,4.0,1.5,4.0,2.0,0.5,1.5,-0.0,0.5,-0.0,0.0,0.5,0.0,3.5,1.0,1.0,2.0,0.0,}},
/* T */ [0x54] = { 2,(int[]){0,1,2,3,},(float[]){1.0,0.0,1.0,4.0,2.0,4.0,0.0,4.0,}},
/* S */ [0x53] = {12,(int[]){2,1,0,2,12,3,3,4,4,5,6,5,9,6,1,7,8,11,10,8,7,10,11,12,},(float[]){2.0,3.5,0.5,4.0,1.5,4.0,2.0,0.5,1.5,-0.0,0.5,-0.0,0.0,0.5,0.0,3.5,0.5,2.5,0.0,1.0,0.0,3.0,1.5,2.0,2.0,1.5,}},
/* R */ [0x52] = { 8,(int[]){0,1,0,2,2,3,3,4,6,5,1,6,3,7,7,8,},(float[]){2.0,3.5,1.5,4.0,2.0,2.5,1.5,2.0,0.5,2.0,0.0,-0.0,0.0,4.0,2.0,1.5,2.0,0.0,}},
/* P */ [0x50] = { 7,(int[]){2,1,0,2,0,3,3,4,4,5,7,6,1,7,},(float[]){2.0,3.5,0.5,4.0,1.5,4.0,2.0,2.5,1.5,2.0,0.0,2.0,0.0,-0.0,0.0,4.0,}},
/* O */ [0x4F] = { 8,(int[]){2,1,0,2,0,3,3,4,4,5,6,5,7,6,1,7,},(float[]){2.0,3.5,0.5,4.0,1.5,4.0,2.0,0.5,1.5,-0.0,0.5,-0.0,0.0,0.5,0.0,3.5,}},
/* N */ [0x4E] = { 3,(int[]){2,3,2,1,3,0,},(float[]){2.0,4.0,0.0,0.0,0.0,4.0,2.0,-0.0,}},
/* M */ [0x4D] = { 4,(int[]){0,1,3,4,3,2,4,1,},(float[]){2.0,0.0,2.0,4.0,0.0,0.0,0.0,4.0,1.0,2.0,}},
/* L */ [0x4C] = { 3,(int[]){2,1,0,2,3,0,},(float[]){2.0,0.0,0.0,4.0,0.0,-0.0,2.0,0.5,}},
/* K */ [0x4B] = { 6,(int[]){1,0,2,3,0,5,4,6,5,7,5,4,},(float[]){2.0,2.5,2.0,4.0,-0.0,4.0,0.0,0.0,2.0,1.5,1.5,2.0,2.0,0.0,0.0,2.0,}},
/* J */ [0x4A] = { 7,(int[]){4,5,2,3,5,2,1,4,0,1,6,0,3,7,},(float[]){0.0,0.5,0.5,0.0,2.0,4.0,0.0,4.0,1.5,0.0,2.0,0.5,0.0,1.0,0.0,3.5,}},
/* I */ [0x49] = { 3,(int[]){0,1,2,3,4,5,},(float[]){1.0,0.0,1.0,4.0,1.5,4.0,0.5,4.0,0.5,-0.0,1.5,0.0,}},
/* H */ [0x48] = { 3,(int[]){0,1,2,3,4,5,},(float[]){2.0,0.0,2.0,4.0,-0.0,4.0,0.0,0.0,0.0,2.0,2.0,2.0,}},
/* G */ [0x47] = {10,(int[]){3,2,1,3,0,1,2,4,5,6,6,7,7,8,9,8,4,9,10,5,},(float[]){2.0,3.0,2.0,3.5,0.5,4.0,1.5,4.0,-0.0,3.5,2.0,1.5,2.0,0.5,1.5,-0.0,0.5,-0.0,0.0,0.5,1.5,1.5,}},
/* F */ [0x46] = { 4,(int[]){2,1,0,2,4,5,1,3,},(float[]){2.0,3.5,-0.0,4.0,2.0,4.0,0.0,-0.0,0.0,2.0,1.0,2.0,}},
/* E */ [0x45] = { 6,(int[]){2,1,0,2,6,7,3,4,4,5,1,5,},(float[]){2.0,3.5,-0.0,4.0,2.0,4.0,2.0,0.5,2.0,-0.0,0.0,-0.0,0.0,2.0,1.0,2.0,}},
/* D */ [0x44] = { 6,(int[]){2,1,0,2,0,3,3,4,4,5,1,5,},(float[]){2.0,3.5,-0.0,4.0,1.5,4.0,2.0,0.5,1.5,-0.0,0.0,-0.0,}},
/* C */ [0x43] = { 9,(int[]){3,2,1,3,0,1,2,4,5,6,6,7,7,8,9,8,4,9,},(float[]){2.0,3.0,2.0,3.5,0.5,4.0,1.5,4.0,-0.0,3.5,2.0,1.0,2.0,0.5,1.5,-0.0,0.5,-0.0,0.0,0.5,}},
/* B */ [0x42] = {10,(int[]){4,3,2,4,0,1,1,2,5,0,0,6,6,7,7,8,8,9,3,9,},(float[]){1.5,2.0,2.0,2.5,2.0,3.5,-0.0,4.0,1.5,4.0,0.5,2.0,2.0,1.5,2.0,0.5,1.5,-0.0,0.0,-0.0,}},
/* A */ [0x41] = { 6,(int[]){4,3,2,4,3,0,1,2,0,5,6,7,},(float[]){0.0,3.5,2.0,0.0,2.0,3.5,0.5,4.0,1.5,4.0,0.0,0.0,0.0,2.0,2.0,2.0,}},
/* . */ [0x2E] = { 4,(int[]){1,0,3,1,0,2,2,3,},(float[]){0.4,0.1,0.4,-0.1,0.6,0.1,0.6,-0.1,}},
/* 9 */ [0x39] = { 9,(int[]){2,3,4,0,9,4,3,1,1,5,5,6,6,7,7,8,8,9,},(float[]){0.5,0.0,0.0,2.0,1.5,1.5,0.5,1.5,1.5,0.0,0.0,3.5,0.5,4.0,1.5,4.0,2.0,3.5,2.0,0.5,}},
/* 8 */ [0x38] = {15,(int[]){1,2,2,0,0,3,3,4,4,5,5,6,6,7,1,8,8,9,9,10,10,11,11,12,12,13,7,1,13,2,},(float[]){2.0,2.0,0.5,2.5,1.5,2.5,2.0,0.5,1.5,0.0,0.5,0.0,0.0,0.5,0.0,2.0,0.0,3.0,0.0,3.5,0.5,4.0,1.5,4.0,2.0,3.5,2.0,3.0,}},
/* 7 */ [0x37] = { 4,(int[]){0,1,2,0,4,2,3,4,},(float[]){0.0,4.0,0.0,3.5,2.0,4.0,0.5,0.0,2.0,3.5,}},
/* 6 */ [0x36] = { 9,(int[]){2,3,4,0,9,4,3,1,1,5,5,6,6,7,7,8,8,9,},(float[]){1.5,4.0,2.0,2.0,0.5,2.5,1.5,2.5,0.5,4.0,2.0,0.5,1.5,0.0,0.5,0.0,0.0,0.5,0.0,3.5,}},
/* 5 */ [0x35] = { 9,(int[]){2,3,0,2,4,0,3,1,1,5,5,6,6,7,7,8,8,9,},(float[]){0.0,4.0,2.0,2.0,0.0,2.5,1.5,2.5,2.0,4.0,2.0,0.5,1.5,0.0,0.5,0.0,0.0,0.5,0.0,1.0,}},
/* 4 */ [0x34] = { 3,(int[]){1,3,0,2,4,0,},(float[]){0.0,1.0,2.0,4.0,2.0,1.0,2.0,0.0,1.0,4.0,}},
/* 3 */ [0x33] = {12,(int[]){4,3,2,4,3,0,1,2,5,1,6,5,7,6,8,7,9,8,10,9,11,10,0,12,},(float[]){0.0,3.5,2.0,2.5,2.0,3.5,0.5,4.0,1.5,4.0,1.5,2.0,2.0,1.5,2.0,0.5,1.5,0.0,0.5,0.0,0.0,0.5,0.0,1.0,0.0,3.0,}},
/* 2 */ [0x32] = { 9,(int[]){4,3,2,4,3,0,1,2,5,1,6,5,7,6,0,8,9,7,},(float[]){0.0,3.5,2.0,2.5,2.0,3.5,0.5,4.0,1.5,4.0,0.0,0.5,0.0,0.0,2.0,0.0,0.0,3.0,2.0,0.5,}},
/* 1 */ [0x31] = { 3,(int[]){4,3,2,1,3,0,},(float[]){0.0,3.0,2.0,0.0,0.0,0.0,1.0,4.0,1.0,0.0,}},
/* 0 */ [0x30] = { 9,(int[]){2,0,1,3,7,6,0,3,5,4,4,1,0,5,6,2,3,7,},(float[]){0.0,0.5,2.0,0.5,0.0,3.5,2.0,3.5,1.5,0.0,0.5,0.0,0.5,4.0,1.5,4.0,}},
};

#endif // __FONT__