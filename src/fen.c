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

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "position.h"
#include "fen.h"

extern const bitboard _mask[64];

static int loadPostionSection(const char * const fen, int length, position *pos);
static int loadPlayerSection(const char * const fen, int i, int length, position *pos);
static int loadCastlingSection(const char * const fen, int i, int length, position *pos);
static int loadEnPassantSquare(const char *fen, int i, int length, position *pos);


/*
 * Load the position section
 */
static int loadPostionSection(const char *fen, const int length, position *pos)
{
    int i;
    int sq;

    sq = 63; /* FEN notation starts with A8 */
    for(i = 0; i < length; i++) {
        if(' ' == fen[i]) {
            if(0 == i) {
                /* board position section is empty */
                return ERR_PARSE;
            }
            else {
                /* go to next position to continue parsing */
                i++;
                break;
            }
        }
        else if(fen[i] >= '1' && fen[i] <= '9') {
            /* number */
            sq -= fen[i] - '0'; /* ugly? maybe, but it works */
        }
        else if('/' == fen[i]) {
            /* sq -= 8; */
        }
        else if('K' == fen[i]) {
            pos->board[sq] = WHITE_KING;
            pos->king[WHITE] |= _mask[sq];
            pos->kingSquare[WHITE] = sq;
            sq--;
        }
        else if('k' == fen[i]) {
            pos->board[sq] = BLACK_KING;
            pos->king[BLACK] |= _mask[sq];
            pos->kingSquare[BLACK] = sq;
            sq--;
        }
        else if('Q' == fen[i]) {
            pos->board[sq] = WHITE_QUEEN;
            pos->queens[WHITE] |= _mask[sq];
            sq--;
        }
        else if('q' == fen[i]) {
            pos->board[sq] = BLACK_QUEEN;
            pos->queens[BLACK] |= _mask[sq];
            sq--;
        }
        else if('R' == fen[i]) {
            pos->board[sq] = WHITE_ROOK;
            pos->rooks[WHITE] |= _mask[sq];
            sq--;
        }
        else if('r' == fen[i]) {
            pos->board[sq] = BLACK_ROOK;
            pos->rooks[BLACK] |= _mask[sq];
            sq--;
        }
        else if('B' == fen[i]) {
            pos->board[sq] = WHITE_BISHOP;
            pos->bishops[WHITE] |= _mask[sq];
            sq--;
        }
        else if('b' == fen[i]) {
            pos->board[sq] = BLACK_BISHOP;
            pos->bishops[BLACK] |= _mask[sq];
            sq--;
        }
        else if('N' == fen[i]) {
            pos->board[sq] = WHITE_KNIGHT;
            pos->knights[WHITE] |= _mask[sq];
            sq--;
        }
        else if('n' == fen[i]) {
            pos->board[sq] = BLACK_KNIGHT;
            pos->knights[BLACK] |= _mask[sq];
            sq--;
        }
        else if('P' == fen[i]) {
            pos->board[sq] = WHITE_PAWN;
            pos->pawns[WHITE] |= _mask[sq];
            sq--;
        }
        else if('p' == fen[i]) {
            pos->board[sq] = BLACK_PAWN;
            pos->pawns[BLACK] |= _mask[sq];
            sq--;
        }
        else {
            /* illegal character */
            return ERR_PARSE;
        }

        /* assert(sq >= 0); */
    }

    pos->pieces[WHITE] = pos->king[WHITE] | pos->queens[WHITE] |
        pos->rooks[WHITE] | pos->bishops[WHITE] |
        pos->knights[WHITE] | pos->pawns[WHITE];

    pos->pieces[BLACK] = pos->king[BLACK] | pos->queens[BLACK] |
        pos->rooks[BLACK] | pos->bishops[BLACK] |
        pos->knights[BLACK] | pos->pawns[BLACK];

    return i;
}


static int loadPlayerSection(const char *fen, int i, int length, position *pos)
{
    assert(i < length);

    if('w' == fen[i] || 'W' == fen[i]) {
        pos->toPlay = WHITE;
    }
    else if('b' == fen[i] || 'B' == fen[i]) {
        pos->toPlay = BLACK;
    }
    else {
        /* What colour is that? */
        return ERR_PARSE;
    }

    i++; /* skip the player character */
    if(i >= length) {
        return ERR_PARSE;
    }

    if(' ' != fen[i]) {
        /* this section is only one char long */
        return ERR_PARSE;
    }

    i++; /* skip space */

    return i;
}

/* Set the castling flags */
static int loadCastlingSection(const char *fen, int i, int length, position *pos)
{
    assert(i < length);

    if('-' == fen[i]) {
        /* nobody can castle */
        i++;
        if(' ' == fen[i]) {
            i++; /* skip space */
        }
        else {
            /* illegal character */
            return ERR_PARSE;
        }    
    }
    else {
        for(; i < length; i++) {
            if('K' == fen[i]) {
                pos->castleFlags |= CASTLE_WK;
            }
            else if('Q' == fen[i]) {
                pos->castleFlags |= CASTLE_WQ;
            }
            else if('k' == fen[i]) {
                pos->castleFlags |= CASTLE_BK;
            }
            else if('q' == fen[i]) {
                pos->castleFlags |= CASTLE_BQ;
            }
            else if(' ' == fen[i]) {
                i++; /* skip space */
                break;
            }
            else {
                /* illegal character */
                return ERR_PARSE;
            }
        }
    }

    return i;
}

static int loadEnPassantSquare(const char *fen, int i, int length, position *pos)
{
    int rank = 0;
    int file = 0;

    assert(i < length);

    if('-' == fen[i]) {
        /* no ep square */
        pos->epSquare = -1;
    }
    else if('a' == fen[i] || 'A' == fen[i]) {
        /*
         * "Keep to the code"
         * 'a' file is number 7 for us. 'h' is number 0.
         */
        file = 7;
    }
    else if('b' == fen[i] || 'B' == fen[i]) {
        file = 6;
    }
    else if('c' == fen[i] || 'C' == fen[i]) {
        file = 5;
    }
    else if('d' == fen[i] || 'D' == fen[i]) {
        file = 4;
    }
    else if('e' == fen[i] || 'E' == fen[i]) {
        file = 3;
    }
    else if('f' == fen[i] || 'F' == fen[i]) {
        file = 2;
    }
    else if('g' == fen[i] || 'G' == fen[i]) {
        file = 1;
    }
    else if('h' == fen[i] || 'H' == fen[i]) {
        file = 0;
    }
    else {
        return ERR_PARSE;
    }

    i++;
    if(i >= length) {
        return ERR_PARSE;
    }

    /* need a rank now */
    if(fen[i] < '1' || fen[i] > '8') {
        return ERR_PARSE;
    }

    rank = fen[i] - '1';
    pos->epSquare = (rank * 8) + file;

    i++;
    if(' ' != fen[i]) {
        /* this section is only 2 chars long */
        return ERR_PARSE;
    }

    i++; /* skip the space */

    return i;
}

int savePositionToFEN(const position * const pos, char *fen)
{
    int sq;
    int i = 0;
    int empty = 0;
    char rank;
    char file;

    assertPosition(pos);
    assert(fen != NULL);

    for(sq = 63; sq >= 0; sq--) {
        if(0 == pos->board[sq]) {
            empty++;
            if(0 == sq % 8) {
                if(empty != 0) {
                    fen[i++] = '0' + empty;
                }

                if(sq != 0) {
                    fen[i++] = '/';
                }

                empty = 0;
            }
            continue;
        }

        if(empty != 0) {
            fen[i++] = '0' + empty;
            empty = 0;
        }

        if(WHITE_KING == pos->board[sq]) {
            fen[i++] = 'K'; empty = 0;
        }
        else if(BLACK_KING == pos->board[sq]) {
            fen[i++] = 'k'; empty = 0;
        }
        else if(WHITE_QUEEN == pos->board[sq]) {
            fen[i++] = 'Q'; empty = 0;
        }
        else if(BLACK_QUEEN == pos->board[sq]) {
            fen[i++] = 'q'; empty = 0;
        }
        else if(WHITE_ROOK == pos->board[sq]) {
            fen[i++] = 'R'; empty = 0;
        }
        else if(BLACK_ROOK == pos->board[sq]) {
            fen[i++] = 'r'; empty = 0;
        }
        else if(WHITE_BISHOP == pos->board[sq]) {
            fen[i++] = 'B'; empty = 0;
        }
        else if(BLACK_BISHOP == pos->board[sq]) {
            fen[i++] = 'b'; empty = 0;
        }
        else if(WHITE_KNIGHT == pos->board[sq]) {
            fen[i++] = 'N'; empty = 0;
        }
        else if(BLACK_KNIGHT == pos->board[sq]) {
            fen[i++] = 'n'; empty = 0;
        }
        else if(WHITE_PAWN == pos->board[sq]) {
            fen[i++] = 'P'; empty = 0;
        }
        else if(BLACK_PAWN == pos->board[sq]) {
            fen[i++] = 'p'; empty = 0;
        }
        else {
            assert(0);
        }


        if(0 == sq % 8) {
            fen[i++] = '/';
        }
    }

    fen[i++] = ' ';
    fen[i++] = (pos->toPlay == WHITE) ? 'w' : 'b';
    fen[i++] = ' ';

    if(-1 == pos->epSquare) {
        fen[i++] = '-';
    }
    else {
        rank = RANK_NOTATION(sq);
        file = FILE_NOTATION(sq);
    }

    fen[i++] = ' ';

    if(0 == pos->castleFlags) {
        fen[i++] = '-';
    }
    else {
        if(pos->castleFlags & CASTLE_WK) {
            fen[i++] = 'K';
        }

        if(pos->castleFlags & CASTLE_WQ) {
            fen[i++] = 'Q';
        }

        if(pos->castleFlags & CASTLE_BK) {
            fen[i++] = 'k';
        }

        if(pos->castleFlags & CASTLE_BQ) {
            fen[i++] = 'q';
        }
    }

    fen[i++] = '\0';

    return 0;
}

/* Load position from FEN string */
int loadPositionFromFEN(const char * const fen, position *pos)
{
    int offset;
    int length;

    /* start with a blank board */
    clearPosition(pos);

    length = strlen(fen);

    /*
     * Check if the string is too short or too long.
     * Yeah, the lower bound check is not accurate,
     * but I'm too lazy to factor in all the stuff. DIY.
     */
    if(length <= 2 || length > FEN_MAX_LINE) {
        return ERR_PARSE;
    }

    offset = loadPostionSection(fen, length, pos);
    if(offset >= length) {
        return ERR_PARSE;
    }

    offset = loadPlayerSection(fen, offset, length, pos);
    if(offset >= length) {
        return ERR_PARSE;
    }

    offset = loadCastlingSection(fen, offset, length, pos);
    if(offset >= length) {
        return ERR_PARSE;
    }

    offset = loadEnPassantSquare(fen, offset, length, pos);

    /* TODO parse full, half moves */

    /* Do some basic validity checks */
    if(0 == pos->king[WHITE]) {
        return ERR_NO_WKING;
    }

    if(0 == pos->king[BLACK]) {
        return ERR_NO_BKING;
    }

    /* TODO: add other basic checks */

    return 0; /* FEN loaded successfully */
}
