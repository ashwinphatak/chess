/*
AshChess - A simple and stupid computer chess program
Copyright (C) 2005 Ashwin Phatak

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "bitboard.h"

/* Internal bitboards */
bitboard _mask[64];
bitboard _king[64];
bitboard _rook_horz[64][256];
bitboard _rook_vert[64][256];
bitboard _knight[64];
bitboard _horz[64][256];
bitboard _vert[64][256];
bitboard _a1h8[64][256];
bitboard _h1a8[64][256];
bitboard _wpawn_advance1[64];
bitboard _bpawn_advance1[64];
bitboard _wpawn_advance2[64];
bitboard _bpawn_advance2[64];
bitboard _wpawn_attack[64];
bitboard _bpawn_attack[64];

/* Lookup table used for bitboard rotation */
const int _rotate[64] = {
     0,  8, 16, 24, 32, 40, 48, 56,
     1,  9, 17, 25, 33, 41, 49, 57,
     2, 10, 18, 26, 34, 42, 50, 58,
     3, 11, 19, 27, 35, 43, 51, 59,
     4, 12, 20, 28, 36, 44, 52, 60,
     5, 13, 21, 29, 37, 45, 53, 61,
     6, 14, 22, 30, 38, 46, 54, 62,
     7, 15, 23, 31, 39, 47, 55, 63,
};


const int _rot_a1h8[64] =  {
    7, 14, 21, 28, 35, 42, 49, 56,
    6, 13, 20, 27, 34, 41, 48, 63,
    0, 15, 22, 29, 36, 43, 50, 57,
    1,  8, 23, 30, 37, 44, 51, 58,
    5, 12, 19, 26, 33, 40, 55, 62,
    2,  9, 16, 31, 38, 45, 52, 59,
    4, 11, 18, 25, 32, 47, 54, 61,
    3, 10, 17, 24, 39, 46, 53, 60, 
};

const int _rot_h1a8[64] =  {
    0,  9, 18, 27, 36, 45, 54, 63,
    1, 10, 19, 28, 37, 46, 55, 56,
    7,  8, 17, 26, 35, 44, 53, 62,
    6, 15, 16, 25, 34, 43, 52, 61,
    2, 11, 20, 29, 38, 47, 48, 57,
    5, 14, 23, 24, 33, 42, 51, 60,
    3, 12, 21, 30, 39, 40, 49, 58,
    4, 13, 22, 31, 32, 41, 50, 59,
};

const int _a1h8_shiftMask[64][2] = {
    {16, 0x1}, {24, 0x3}, {40, 0x7}, {56, 0xf}, {48, 0x1f}, {32, 0x3f}, {8, 0x7f}, {0, 0xff},
    {24, 0x3}, {40, 0x7}, {56, 0xf}, {48, 0x1f}, {32, 0x3f}, {8, 0x7f}, {0, 0xff}, {16, 0xfe},
    {40, 0x7}, {56, 0xf}, {48, 0x1f}, {32, 0x3f}, {8, 0x7f}, {0, 0xff}, {16, 0xfe}, {24, 0xfc},
    {56, 0xf}, {48, 0x1f}, {32, 0x3f}, {8, 0x7f}, {0, 0xff}, {16, 0xfe}, {24, 0xfc}, {40, 0xf8},
    {48, 0x1f}, {32, 0x3f}, {8, 0x7f}, {0, 0xff}, {16, 0xfe}, {24, 0xfc}, {40, 0xf8}, {56, 0xf0},
    {32, 0x3f}, {8, 0x7f}, {0, 0xff}, {16, 0xfe}, {24, 0xfc}, {40, 0xf8}, {56, 0xf0}, {48, 0xe0},
    {8, 0x7f}, {0, 0xff}, {16, 0xfe}, {24, 0xfc}, {40, 0xf8}, {56, 0xf0}, {48, 0xe0}, {32, 0xc0},
    {0, 0xff}, {16, 0xfe}, {24, 0xfc}, {40, 0xf8}, {56, 0xf0}, {48, 0xe0}, {32, 0xc0}, {8, 0x80},
};

const int _h1a8_shiftMask[64][2] = {
    {0, 0xff}, {8, 0x7f}, {32, 0x3f}, {48, 0x1f}, {56, 0xf}, {40, 0x7}, {24, 0x3}, {16, 0x1},
    {16, 0xfe}, {0, 0xff}, {8, 0x7f}, {32, 0x3f}, {48, 0x1f}, {56, 0xf}, {40, 0x7}, {24, 0x3},
    {24, 0xfc}, {16, 0xfe}, {0, 0xff}, {8, 0x7f}, {32, 0x3f}, {48, 0x1f}, {56, 0xf}, {40, 0x7},
    {40, 0xf8}, {24, 0xfc}, {16, 0xfe}, {0, 0xff}, {8, 0x7f}, {32, 0x3f}, {48, 0x1f}, {56, 0xf},
    {56, 0xf0}, {40, 0xf8}, {24, 0xfc}, {16, 0xfe}, {0, 0xff}, {8, 0x7f}, {32, 0x3f}, {48, 0x1f},
    {48, 0xe0}, {56, 0xf0}, {40, 0xf8}, {24, 0xfc}, {16, 0xfe}, {0, 0xff}, {8, 0x7f}, {32, 0x3f},
    {32, 0xc0}, {48, 0xe0}, {56, 0xf0}, {40, 0xf8}, {24, 0xfc}, {16, 0xfe}, {0, 0xff}, {8, 0x7f},
    {8, 0x80},  {32, 0xc0}, {48, 0xe0}, {56, 0xf0}, {40, 0xf8}, {24, 0xfc}, {16, 0xfe}, {0, 0xff},
};

const int _diag_sq[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 2,
    3, 3, 3, 3, 3, 3, 3, 3,
    4, 4, 4, 4, 4, 4, 4, 4,
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 6, 6, 6, 6, 6, 6, 6,
    7, 7, 7, 7, 7, 7, 7, 7,
};

/* These functions are not needed by anyone outside the file */
static void init_mask(void);
static void init_king(void);
static void init_knight(void);
static void init_horz(void);
static void init_vert(void);
static void init_a1h8(void);
static void init_h1a8(void);
static void init_pawns(void);
static bitboard horz_slide(int pos, int state);
static bitboard unrotate_a1h8(bitboard x);
static bitboard unrotate_h1a8(bitboard x);

void init(void)
{
    init_mask();
    init_king();
    init_knight();
    init_horz();
    init_vert();
    init_a1h8();
    init_h1a8();
    init_pawns();
}

bitboard rotate(bitboard b)
{
    int i;
    bitboard r = 0;

    for(i = 63; i >= 0; i--) {
        if(b & _mask[i]) {
            /* This 'if' statement looks rather inelegant. How to remove it? */
            SET_BIT(r, _rotate[i]);
        }
    }

    return r;
}


bitboard rotate_a1h8(bitboard x)
{
    bitboard y = 0;
    int i;

    for(i = 63; i >=0; i--) {
        if(x & _mask[_rot_a1h8[i]]) {
            SET_BIT(y, i);
        }
    }

    return y;
}

static bitboard unrotate_a1h8(bitboard x)
{
    bitboard y = 0;
    int i;

    for(i = 63; i >=0; i--) {
        if(x & _mask[i]) {
            SET_BIT(y, _rot_a1h8[i]);
        }
    }

    return y;
}

bitboard rotate_h1a8(bitboard x)
{
    bitboard y = 0;
    int i;

    for(i = 63; i >=0; i--) {
        if(x & _mask[_rot_h1a8[i]]) {
            SET_BIT(y, i);
        }
    }

    return y;
}

static bitboard unrotate_h1a8(bitboard x)
{
    bitboard y = 0;
    int i;

    for(i = 63; i >=0; i--) {
        if(x & _mask[i]) {
            SET_BIT(y, _rot_h1a8[i]);
        }
    }

    return y;
}

static void init_mask(void)
{
    bitboard b = 1;
    int i;

    for(i = 0; i < 64; i++) {
        _mask[i] = b;
        b = b << 1;

    }
}

static void init_king(void)
{
    bitboard temp = 0;
    int i;
    int file;
    int rank;

    for(i = 0; i < 64; i++) {
        temp = 0;

        file = i % 8;
        rank = i / 8;

        /* left */
        if(file < 7) {
            temp |= _mask[i] << 1;
        }

        /* right */
        if(file > 0) {
            temp |= _mask[i] >> 1;
        }

        /* up */
        if(rank < 7) {
            temp |= _mask[i] << 8;
        }

        /* down */
        if(rank > 0) {
            temp |= _mask[i] >> 8;
        }

        /* left up */
        if(file < 7 && rank < 7) {
            temp |= _mask[i] << 9;
        }

        /* left down */
        if(file < 7 && rank > 0) {
            temp |= _mask[i] >> 7;
        }

        /* right up */
        if(file > 0 && rank < 7) {
            temp |= _mask[i] << 7;
        }

        /* right down */
        if(file > 0 && rank > 0) {
            temp |= _mask[i] >> 9;
        }


        _king[i] = temp;

    }

}

static void init_knight(void)
{
    bitboard temp = 0;
    int i;
    int file;
    int rank;

    for(i = 0; i < 64; i++) {
        temp = 0;

        file = i % 8;
        rank = i / 8;

        /* left side */

        if(file < 6 && rank < 7) {
            temp |= _mask[i] << 10;
        }

        if(file < 7 && rank < 6) {
            temp |= _mask[i] << 17;
        }

        if(file < 6 && rank > 0) {
            temp |= _mask[i] >> 6;
        }

        if(file < 7 && rank > 1) {
            temp |= _mask[i] >> 15;
        }



        /* right side */

        if(file > 1 && rank < 7) {
            temp |= _mask[i] << 6;
        }


        if(file > 0 && rank < 6) {
            temp |= _mask[i] << 15;
        }

        if(file > 1 && rank > 0) {
            temp |= _mask[i] >> 10;
        }

        if(file > 0 && rank > 1) {
            temp |= _mask[i] >> 17;
        }

        _knight[i] = temp;
    }
}


static void init_horz(void)
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
    for(i = 8; i < 64; i++) {
        rank = i / 8;
        for(j = 0; j < 256; j++) {
            _horz[i][j] = _horz[i % 8][j] << (rank << 3);
        }
    }
}

static bitboard horz_slide(int pos, int s)
{
    bitboard b = 0;
    bitboard state = (bitboard)s;
    int i;

    /* current square also attacked(assume) */
    b |= _mask[pos];

    /* go left */
    for(i = pos + 1; i < 8; i++) {
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

static void init_vert(void)
{
    int i;
    int j;

    for(i = 0; i < 64; i++) {
        for(j = 0; j < 256; j++) {
            _vert[_rotate[i]][j] = rotate(_horz[i][j]);
        }
    }
}

static void init_a1h8(void)
{
    int i, j;
    int state;
    for(i = 0; i < 64; i++) {
        for(j = 0; j < 256; j++) {
            state = j & _a1h8_shiftMask[i][MASK];
            _a1h8[i][state] = unrotate_a1h8(
                (_horz[_diag_sq[i]][j] & _a1h8_shiftMask[i][MASK]) << _a1h8_shiftMask[i][SHIFT]);
        }
    }
}

static void init_h1a8(void)
{
    int i, j;
    int state;
    for(i = 0; i < 64; i++) {
        for(j = 0; j < 256; j++) {
            state = j & _h1a8_shiftMask[i][MASK];
            _h1a8[i][state] = unrotate_h1a8(
                (_horz[_diag_sq[i]][j] & _h1a8_shiftMask[i][MASK]) << _h1a8_shiftMask[i][SHIFT]);
        }
    }
}

static void init_pawns(void)
{
    int sq;
    int file;

    /* white pawn advances (hard to resist?) */
    for(sq = 8; sq < 56; sq++) {
        _wpawn_advance1[sq] = _mask[sq] | (_mask[sq] << 8);
        if(sq < 16) {
            _wpawn_advance2[sq] = _mask[sq] << 16;
        }
    }

    /* black pawn advances */
    for(sq = 55; sq >= 8; sq--) {
        _bpawn_advance1[sq] = _mask[sq] | (_mask[sq] >> 8);
        if(sq >= 48) {
            _bpawn_advance2[sq] = _mask[sq] >> 16;
        }
    }

    /* white pawn captures */
    for(sq = 8; sq < 56; sq++) {
        _wpawn_attack[sq] = _mask[sq];
        file = sq % 8;
        if(file > 0) {
            _wpawn_attack[sq] |= _mask[sq] << 7;
        }
        if(file < 7) {
            _wpawn_attack[sq] |= _mask[sq] << 9;
        }
    }

    /* black pawn captures */
    for(sq = 55; sq >= 8; sq--) {
        _bpawn_attack[sq] = _mask[sq];
        file = sq % 8;
        if(file > 0) {
            _bpawn_attack[sq] |= _mask[sq] >> 9;
        }
        if(file < 7) {
            _bpawn_attack[sq] |= _mask[sq] >> 7;
        }
    }
}
