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

#include <stdio.h>
#include <assert.h>
#include "bitboard.h"
#include "position.h"
#include "move.h"
#include "fen.h"
#include "diag.h"

extern const bitboard _mask[64];
extern const bitboard _king[64];
extern const bitboard _knight[64];
extern const bitboard _horz[64][256];
extern const bitboard _vert[64][256];

/* Print a bitboard */
void print(const bitboard b)
{
    int i;

    for(i = 63; i >=0; i--) {
        if(b & _mask[i]) {
            fputc('1', stderr);
        }
        else {
            fputc('0', stderr);
        }

        if(i % 8 == 0) {
            fputc('\n', stderr);
        }
    }

    fputc('\n', stderr);
}

void printPosition(const position * const pos)
{
    int sq;
    char rank;
    char file;
    char castle[5] = "----";

    for(sq = 63; sq >= 0; sq--) {
        if(WHITE_KING == pos->board[sq]) {
            fputc('K', stderr);
        }
        else if(BLACK_KING == pos->board[sq]) {
            fputc('k', stderr);
        }
        else if(WHITE_QUEEN == pos->board[sq]) {
            fputc('Q', stderr);
        }
        else if(BLACK_QUEEN == pos->board[sq]) {
            fputc('q', stderr);
        }
        else if(WHITE_ROOK == pos->board[sq]) {
            fputc('R', stderr);
        }
        else if(BLACK_ROOK == pos->board[sq]) {
            fputc('r', stderr);
        }
        else if(WHITE_BISHOP == pos->board[sq]) {
            fputc('B', stderr);
        }
        else if(BLACK_BISHOP == pos->board[sq]) {
            fputc('b', stderr);
        }
        else if(WHITE_KNIGHT == pos->board[sq]) {
            fputc('N', stderr);
        }
        else if(BLACK_KNIGHT == pos->board[sq]) {
            fputc('n', stderr);
        }
        else if(WHITE_PAWN == pos->board[sq]) {
            fputc('P', stderr);
        }
        else if(BLACK_PAWN == pos->board[sq]) {
            fputc('p', stderr);
        }
        else if(0 == pos->board[sq]) {
            fputc('-', stderr);
        }
        else {
            assert(0);
        }

        /* fputc(' ', stderr); */

        if(sq % 8 == 0) {
            if(56 == sq) {
                if(WHITE == pos->toPlay) {
                    fprintf(stderr, " [White]");
                }
                else {
                    fprintf(stderr, " [Black]");
                }
            }
            else if(48 == sq) {
                if(pos->epSquare != -1) {
                    rank = RANK_NOTATION(pos->epSquare);
                    file = FILE_NOTATION(pos->epSquare);

                    fprintf(stderr, " [%c%c]", file, rank);
                }
                else {
                    fprintf(stderr, " [-]");
                }
            }
            else if(40 == sq) {
                if(pos->castleFlags & CASTLE_WK) {
                    castle[0] = 'K';
                }

                if(pos->castleFlags & CASTLE_WQ) {
                    castle[1] = 'Q';
                }

                if(pos->castleFlags & CASTLE_BK) {
                    castle[2] = 'k';
                }

                if(pos->castleFlags & CASTLE_BQ) {
                    castle[3] = 'q';
                }

                fprintf(stderr, " [%s]", castle);
            }

            fputc('\n', stderr);
        }
    }

    fputc('\n', stderr);
}


void printPositionAsFEN(const position * const pos)
{
    char fen[FEN_MAX_LINE];

    savePositionToFEN(pos, fen);
    fprintf(stderr, "%s\n", fen);
}

void printMove(const move * const m)
{
    char piece;

    if(IS_CASTLE(m)) {
        if(1 == m->to || 57 == m->to) {
            fprintf(stderr, "0-0");
        }
        else if(5 == m->to || 61 == m->to) {
            fprintf(stderr, "0-0-0");
        }
    }
    else {
        fprintf(stderr, "%c%c%c%c%c", FILE_NOTATION(m->from),
                            RANK_NOTATION(m->from), (IS_CAPTURE(m)?'x':'-'),
                            FILE_NOTATION(m->to),
                            RANK_NOTATION(m->to));

        if(IS_EP(m)) {
            fprintf(stderr, "(e.p.)");
        }

        if(IS_PROMOTION(m)) {
            if(IS_KNIGHT(m->promoteTo)) {
                piece = 'N';
            }
            else if(IS_BISHOP(m->promoteTo)) {
                piece = 'B';
            }
            else if(IS_ROOK(m->promoteTo)) {
                piece = 'R';
            }
            else if(IS_QUEEN(m->promoteTo)) {
                piece = 'Q';
            }

            fprintf(stderr, "=%c", piece);
        }
    }

    if(IS_CHECK(m)) {
        fprintf(stderr, "+");
    }

    /*fprintf(stderr, " [%d]", m->eval);*/
}
