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

#ifndef _MOVE_H_
#define _MOVE_H_

#include "position.h"


#define NORMAL          1   /* normal move, i.e. not one of the special ones mentioned below */

/* additional attributes a move may have. Note that these can be present for both normal and special moves */
#define CHECK           2   /* move puts the opponent in check */
#define CAPTURE         4   /* move represents a capture */


/* These are the special moves */
#define CASTLE          8   /* castling move */
#define EN_PASSANT      16  /* en passant move */
#define PROMOTION       32  /* move is a pawn promotion */


#define STALEMATE       -1  /* stalemate */
#define WON             -2  /* player to move has checkmated opponent */
#define LOST            -3  /* player to move has been checkmated */
#define ILLEGAL         -4  /* position is illegal */

#define IS_CAPTURE(m)   (m->flags & CAPTURE)
#define IS_CHECK(m)     (m->flags & CHECK)
#define IS_CASTLE(m)    (m->flags & CASTLE)
#define IS_PROMOTION(m) (m->flags & PROMOTION)
#define IS_EP(m)        (m->flags & EN_PASSANT)

#define DISABLE_EP(pos) (pos->epSquare = -1)

#define IS_PROMOTION_SQUARE(player, sq) ((WHITE == player)?(sq >= 56):(sq <= 7))


/* Represents a particular _legal_ move playable in a position */
typedef struct {
    int from;           /* square from */
    int to;             /* square to */
    int flags;          /* special flags e.g. check, capture */
    int promoteTo;      /* piece to promote the pawn to e.g. WHITE_QUEEN */
    int thisPiece;      /* piece being moved e.g. WHITE_ROOK */
    int capturedPiece;  /* captured piece e.g. BLACK_QUEEN */
    int eval;           /* evaluation of the move */
} move;


int inCheck(const position * const pos, const int player, const int targetSquare);
int getMoves(const position * const pos, const int player, const int piece, const int type, move store[]);
int playMove(const position * const pos, position *newPosition, const move * const m, const int player);
int getAllMoves(const position * const pos, const int player, const int storeIndex);
int getAllMovesSorted(const position * const pos, const int player, const int storeIndex);

#endif

