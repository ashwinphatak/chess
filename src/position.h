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

#ifndef _POSITION_H_
#define _POSITION_H_

#include "bitboard.h"

/* player to move */
#define WHITE 1
#define BLACK 0

/*
 * Chess pieces and their weights.
 * The king of course, is invaluable, and the value
 * is used only for the purpose of identification.
 */
#define WHITE_KING    100
#define WHITE_QUEEN   90
#define WHITE_ROOK    50
#define WHITE_BISHOP  32
#define WHITE_KNIGHT  30
#define WHITE_PAWN    10

#define BLACK_KING    -100
#define BLACK_QUEEN   -90
#define BLACK_ROOK    -50
#define BLACK_BISHOP  -32
#define BLACK_KNIGHT  -30
#define BLACK_PAWN    -10

#define EVAL_CHECK    1000

/* castling flags */
#define CASTLE_WK       1 /* white may castle king side */
#define CASTLE_WQ       2 /* white may castle queen side */
#define CASTLE_BK       4 /* black may castle king side */
#define CASTLE_BQ       8 /* black may castle queen side */

#define CASTLE_BITS(c) (WHITE == (c) ? 0x3 : 0xc )

#define IS_KING(x) (WHITE_KING == (x) || BLACK_KING == (x))
#define IS_QUEEN(x) (WHITE_QUEEN == (x) || BLACK_QUEEN == (x))
#define IS_ROOK(x) (WHITE_ROOK == (x) || BLACK_ROOK == (x))
#define IS_BISHOP(x) (WHITE_BISHOP == (x) || BLACK_BISHOP == (x))
#define IS_KNIGHT(x) (WHITE_KNIGHT == (x) || BLACK_KNIGHT == (x))
#define IS_PAWN(x) (WHITE_PAWN == (x) || BLACK_PAWN == (x))

#define OPPONENT(x) ((x) ^ 1)

/* use on a piece to get it's colour */
#define COLOUR(x) ( (x) > 0? WHITE : BLACK)


#define RANK_NOTATION(sq) ('0' + (1 + ((sq) / 8)))
#define FILE_NOTATION(sq) ('a' + (7 - ((sq) % 8)))

/* represents a chess position */
typedef struct {
    bitboard king[2];
    bitboard queens[2];
    bitboard rooks[2];
    bitboard bishops[2];
    bitboard knights[2];
    bitboard pawns[2];
    bitboard pieces[2];
    int board[64];
    int kingSquare[2];      /* king squares */
    int epSquare;           /* ep target square */
    int castleFlags;        /* can castle?  */
    int toPlay;             /* player to move */
} position;

/*
 * Clear the chess position. This consists of
 * 1. Setting all bitboards to 0
 * 2. Setting elements of board array to 0
 * 3. Setting ep square and castling flags to 0
 * 4. Setting player to move to -1
 */
void clearPosition(position*);

/* Check if position is internally consistent */
void assertPosition(const position * const pos);

#endif

