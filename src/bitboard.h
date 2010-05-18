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

#ifndef _BITBOARD_H_
#define _BITBOARD_H_

#define LSB_SET(x) ((x) & 1)
#define SET_BIT(x, y) (x |= _mask[(y)])
#define CLR_BIT(x, y) (x &= ~_mask[(y)])

#define RANK_STATE(b, rank) (((b) >> ((rank) << 3)) & 0xff)
#define FILE_STATE(rotb, file) (( (rotb) >> ((file) << 3)) & 0xff)


#define SHIFT 0
#define MASK 1

#define A1H8_STATE(rotb, sq) ((rotb >> _a1h8_shiftMask[sq][SHIFT]) & _a1h8_shiftMask[sq][MASK])
#define H1A8_STATE(rotb, sq) ((rotb >> _h1a8_shiftMask[sq][SHIFT]) & _h1a8_shiftMask[sq][MASK])

/* for windows */
/* #define WIN32 1 */

#ifdef LINUX

/* 64-bit unsigned data type */
typedef unsigned long long bitboard;

#endif

#ifdef WIN32

/* 64-bit unsigned data type */
typedef unsigned __int64 bitboard;

#endif


/* Init the internal bitboards */
void init(void);

/* Rotate a bitboard.
 * Files become ranks and vice versa.
 */
bitboard rotate(bitboard);

bitboard rotate_a1h8(bitboard x);
bitboard rotate_h1a8(bitboard x);

#endif

