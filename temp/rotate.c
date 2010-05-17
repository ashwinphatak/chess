#include <stdio.h>

#define SET_BIT(x, y) (x |= _mask[(y)])
#define RANK_STATE(b, rank) (((b) >> ((rank) << 3)) & 0xff)

typedef unsigned long long bitboard;

bitboard _mask[16];
bitboard _horz[16][256];
bitboard _a1h8[16][256];

bitboard diag[16] = {

 3,   6,   9,  12,

 0,   7,  10,  13,

 2,   5,   8,  15,

 1,   4,  11 , 14

};

int isolate[16][2] = {
    { 4,  0x1},
    {12, 0x3},
    { 8,  0x7},
    { 0,  0xf},
    {12, 0x3},
    { 8,  0x7},
    { 0,  0xf},
    { 4,  0xe},
    { 8,  0x7},
    { 0,  0xf},
    { 4,  0xe},
    {12, 0xc},
    { 0,  0xf},
    { 4,  0xe},
    {12, 0xc},
    { 8, 0x8},
};

int new_sq[16] = {
    0, 0, 0, 0,
    1, 1, 1, 1,
    2, 2, 2, 2,
    3, 3, 3, 3,
};

void init_mask(void)
{
    bitboard b = 1;
    int i;

    for(i = 0; i < 16; i++) {
        _mask[i] = b;
        b = b << 1;

    }
}

bitboard rotate(bitboard x)
{
    bitboard y = 0;
    int i;

    for(i = 15; i >=0; i--) {
        if(x & _mask[diag[i]]) {
            SET_BIT(y, i);
        }
    }

    return y;
}

bitboard unRotate(bitboard x)
{
    bitboard y = 0;
    int i;

    for(i = 15; i >=0; i--) {
        if(x & _mask[i]) {
            SET_BIT(y, diag[i]);
        }
    }

    return y;
}

int xform(bitboard x, int sq)
{
    bitboard y = 0;

    y = (x >> isolate[sq][0]) & (isolate[sq][1]);

    return y;
}

bitboard horz_slide(int pos, int s)
{
    bitboard b = 0;
    bitboard state = (bitboard)s;
    int i;

    /* current square also attacked(assume) */
    b |= _mask[pos];

    /* go left */
    for(i = pos + 1; i < 4; i++) {
        b |= _mask[i];

        if(state & _mask[i]) {
            break;
        }
    }

    /* go right */
   for(i = pos - 1; i >= 0; i--) {
       b |= _mask[i];

       if(state & _mask[i]) {
           break;
       }
   }

   return b;
}

void init_horz(void)
{
    int i;
    int j;
    int rank;


    for(i = 0; i < 8; i++) {
        for(j = 0; j < 256; j++) {
            _horz[i][j] = horz_slide(i, j);
        }
    }

    /* other ranks are just 1st one shifted up */
    for(i = 8; i < 16; i++) {
        rank = i / 8;

        for(j = 0; j < 256; j++) {
            _horz[i][j] = _horz[i % 8][j] << (rank << 3);
        }
    }
}


/* Print a bitboard */
void print(const bitboard b)
{
    int i;

    for(i = 15; i >=0; i--) {
        if(b & _mask[i]) {
            fputc('1', stdout);
        }
        else {
            fputc('0', stdout);
        }

        if(i % 4 == 0) {
            fputc('\n', stdout);
        }
    }
}

void init_a1h8(void)
{
    int i, j;
    int state;
    //a1h8 [i] [j mask] = f[0-4] j: mask, shift, unrotate
    for(i = 0; i < 16; i++) {
        for(j = 0; j < 16; j++) {
            state = j & isolate[i][1];
            _a1h8[i][state] = unRotate(   (_horz[new_sq[i]][j] & isolate[i][1]) << isolate[i][0]);
        }
    }
}

int main(void)
{
    int i;
    bitboard b;

    init_mask();
    printf("Init mask\n");

    init_horz();
    printf("Init horz\n");

    init_a1h8();
    printf("Init a1h8\n");


    print(_a1h8[7][0xe]);



    return 0;
}
