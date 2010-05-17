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
#include <stdlib.h>
#include <assert.h>
#include "bitboard.h"
#include "position.h"
#include "move.h"
#include "diag.h"

extern const bitboard _mask[64];
extern const bitboard _king[64];
extern const bitboard _knight[64];
extern const bitboard _horz[64][256];
extern const bitboard _vert[64][256];
extern const bitboard _a1h8[64][256];
extern const bitboard _h1a8[64][256];
extern const bitboard _wpawn_attack[64];
extern const bitboard _bpawn_attack[64];
extern const bitboard _wpawn_advance1[64];
extern const bitboard _bpawn_advance1[64];
extern const bitboard _wpawn_advance2[64];
extern const bitboard _bpawn_advance2[64];
extern const int _a1h8_shiftMask[64][2];
extern const int _h1a8_shiftMask[64][2];

/* Move storage.
 */
move _moveStore[64][256]; /* TODO: is 256 a decent limit?? */

bitboard counterMove;


static void clearCapturedPiece(position *pos, const move * const m);
static int movePiece(position *pos, const move * const m, const int player);
static bitboard getPieceBitboard(const position * const pos, const int piece);
static int getPieceMoves(const position * const pos, const int player, const int type, bitboard rawMoves, const int sq, move store[]);
static bitboard getRawMoves(const position * const pos, const int piece, const int player, const int type, const int sq);
static bitboard getAttackRange(const int piece, const int type, const int sq, const bitboard allPieces, const int epSquare);
static void storeMoveIfLegal(const position* const pos, move*  m, const int player, move store[], int *numMoves);
static void movePawnExtra(position *pos, const move * const m, const int player);
static void getCastlingMoves(const position * const pos, const int player, move store[], int *numMoves);
static void moveCastlingExtra(position *pos, const move * const m);

int getAllMoves(const position * const pos, const int player, const int storeIndex)
{
    int numMoves = 0;

    if(WHITE == player) {
        numMoves += getMoves(pos, player, WHITE_KING, CAPTURE, &_moveStore[storeIndex][numMoves]);
        numMoves += getMoves(pos, player, WHITE_PAWN, CAPTURE, &_moveStore[storeIndex][numMoves]);
        numMoves += getMoves(pos, player, WHITE_KNIGHT, CAPTURE, &_moveStore[storeIndex][numMoves]);
        numMoves += getMoves(pos, player, WHITE_BISHOP, CAPTURE, &_moveStore[storeIndex][numMoves]);
        numMoves += getMoves(pos, player, WHITE_ROOK, CAPTURE, &_moveStore[storeIndex][numMoves]);
        numMoves += getMoves(pos, player, WHITE_QUEEN, CAPTURE, &_moveStore[storeIndex][numMoves]);
        numMoves += getMoves(pos, player, WHITE_KING, NORMAL, &_moveStore[storeIndex][numMoves]);
        numMoves += getMoves(pos, player, WHITE_PAWN, NORMAL, &_moveStore[storeIndex][numMoves]);
        numMoves += getMoves(pos, player, WHITE_KNIGHT, NORMAL, &_moveStore[storeIndex][numMoves]);
        numMoves += getMoves(pos, player, WHITE_BISHOP, NORMAL, &_moveStore[storeIndex][numMoves]);
        numMoves += getMoves(pos, player, WHITE_ROOK, NORMAL, &_moveStore[storeIndex][numMoves]);
        numMoves += getMoves(pos, player, WHITE_QUEEN, NORMAL, &_moveStore[storeIndex][numMoves]);
    }
    else {
        numMoves += getMoves(pos, player, BLACK_KING, CAPTURE, &_moveStore[storeIndex][numMoves]);
        numMoves += getMoves(pos, player, BLACK_PAWN, CAPTURE, &_moveStore[storeIndex][numMoves]);
        numMoves += getMoves(pos, player, BLACK_KNIGHT, CAPTURE, &_moveStore[storeIndex][numMoves]);
        numMoves += getMoves(pos, player, BLACK_BISHOP, CAPTURE, &_moveStore[storeIndex][numMoves]);
        numMoves += getMoves(pos, player, BLACK_ROOK, CAPTURE, &_moveStore[storeIndex][numMoves]);
        numMoves += getMoves(pos, player, BLACK_QUEEN, CAPTURE, &_moveStore[storeIndex][numMoves]);
        numMoves += getMoves(pos, player, BLACK_KING, NORMAL, &_moveStore[storeIndex][numMoves]);
        numMoves += getMoves(pos, player, BLACK_PAWN, NORMAL, &_moveStore[storeIndex][numMoves]);
        numMoves += getMoves(pos, player, BLACK_KNIGHT, NORMAL, &_moveStore[storeIndex][numMoves]);
        numMoves += getMoves(pos, player, BLACK_BISHOP, NORMAL, &_moveStore[storeIndex][numMoves]);
        numMoves += getMoves(pos, player, BLACK_ROOK, NORMAL, &_moveStore[storeIndex][numMoves]);
        numMoves += getMoves(pos, player, BLACK_QUEEN, NORMAL, &_moveStore[storeIndex][numMoves]);
    }

    if(0 == numMoves) { /* no moves */
        if(inCheck(pos, player, pos->kingSquare[player])) {
            return LOST;
        }
        else {
            return STALEMATE;
        }
    }

    counterMove += numMoves;

    return numMoves;
}

int getAllMovesSorted(const position * const pos, const int player, const int storeIndex)
{
    int numMoves = 0;
    int i;
    int j;
    move temp;

    numMoves = getAllMoves(pos, player, storeIndex);

    if(numMoves != LOST && numMoves != STALEMATE) {
        /* sort the moves according to evaluation */
        for(i = numMoves - 1; i >= 0; i--) {
            for(j = 1; j <= i; j++) {
                if(_moveStore[storeIndex][j - 1].eval < _moveStore[storeIndex][j].eval) {
                    /* swap the moves */
                    temp = _moveStore[storeIndex][j];
                    _moveStore[storeIndex][j] = _moveStore[storeIndex][j - 1];
                    _moveStore[storeIndex][j - 1] = temp;
                }
            }
        }
    }

    return numMoves;
}

int getMoves(const position * const pos, const int player, const int piece, const int type, move store[])
{
    bitboard rawMoves = 0;
    int sq;
    int numMoves = 0;
    int opponent;
    bitboard pieces;
    opponent = OPPONENT(player);
    /* opponent in check = player won. should be checked somewhere before this */
    assert(!inCheck(pos, opponent, pos->kingSquare[opponent]));
    pieces = getPieceBitboard(pos, piece);
    sq = 0;
    if(IS_KING(piece)) {
        /* we already know the location of the king */
        sq = pos->kingSquare[player];
        pieces = pieces >> sq;
        assert(pieces != 0); /* we should not move the king off the board */
    }
    while(pieces != 0) {
        if(LSB_SET(pieces)) {
            rawMoves = getRawMoves(pos, player, piece, type, sq);
            numMoves += getPieceMoves(pos, player, type, rawMoves, sq, &store[numMoves]);
        }
        pieces = pieces >> 1;
        sq++;
    }
    return numMoves;
}

static bitboard getRawMoves(const position * const pos, const int player, const int piece, const int type, const int sq)
{
    bitboard rawMoves = 0;
    bitboard attackRange = 0;
    bitboard allPieces = 0;
    bitboard opponentSquares = 0;
    assert(sq >= 0 && sq <= 63);
    allPieces = pos->pieces[BLACK] | pos->pieces[WHITE];
    if(CAPTURE == type) {
        attackRange = getAttackRange(piece, type, sq, allPieces, pos->epSquare);
        opponentSquares = pos->pieces[OPPONENT(player)];
        if(pos->epSquare != -1) {
            opponentSquares |= _mask[pos->epSquare];
        }
        /* captures */
        rawMoves = attackRange & opponentSquares;
    }
    else if(NORMAL == type) {
        attackRange = getAttackRange(piece, type, sq, allPieces, pos->epSquare);
        /* normal moves, no violence ;) */
        rawMoves = attackRange & ~allPieces;
    }
    else {
        assert(0); /* don't know type of move */
    }
    return rawMoves;
}

static bitboard getAttackRange(const int piece, const int type, const int sq, const bitboard allPieces, const int epSquare)
{
    bitboard attackRange = 0;
    int state = 0;
    int rank;
    int file;
    bitboard rotAllPieces = 0;
    bitboard rot_a1h8_allPieces = 0;
    bitboard rot_h1a8_allPieces = 0;
    const bitboard *table1 = 0;
    const bitboard *table2 = 0;
    const bitboard *table_attack = 0;
    /* supports only normal and capture */
    assert(NORMAL == type || CAPTURE == type);
    if(IS_KING(piece)) {
        attackRange = _king[sq];
    }
    else if(IS_PAWN(piece)) {
        if(WHITE == COLOUR(piece)) {
            table1 = _wpawn_advance1;
            table2 = _wpawn_advance2;
            table_attack = _wpawn_attack;
        }
        else {
            table1 = _bpawn_advance1;
            table2 = _bpawn_advance2;
            table_attack = _bpawn_attack;
        }
        if(NORMAL == type) {
            attackRange = table1[sq] & ~allPieces;
            if(attackRange) {
                attackRange |= table2[sq] & ~allPieces;
            }
        }
        else { /* capture */
            attackRange = table_attack[sq];
            /* check for ep */
            if(epSquare != -1) {
                if(table_attack[sq] & _mask[epSquare]) {
                    attackRange |= _mask[epSquare];
                }
            }
        }
    }
    else if(IS_KNIGHT(piece)) {
        attackRange = _knight[sq];
    }
    else if(IS_BISHOP(piece)) {
        rot_a1h8_allPieces = rotate_a1h8(allPieces);
        state = A1H8_STATE(rot_a1h8_allPieces, sq);
        attackRange = _a1h8[sq][state];
        rot_h1a8_allPieces = rotate_h1a8(allPieces);
        state = H1A8_STATE(rot_h1a8_allPieces, sq);
        attackRange |= _h1a8[sq][state];
    }
    else if(IS_ROOK(piece)) {
        rank = sq / 8;
        file = sq % 8;
        state = RANK_STATE(allPieces, rank);
        attackRange = _horz[sq][state];
        rotAllPieces = rotate(allPieces);
        state = FILE_STATE(rotAllPieces, file);
        attackRange |= _vert[sq][state];
    }
    else if(IS_QUEEN(piece)) {
        /* rook like moves */
        rank = sq / 8;
        file = sq % 8;
        state = RANK_STATE(allPieces, rank);
        attackRange = _horz[sq][state];
        rotAllPieces = rotate(allPieces);
        state = FILE_STATE(rotAllPieces, file);
        attackRange |= _vert[sq][state];
        /* bishop like moves */
        rot_a1h8_allPieces = rotate_a1h8(allPieces);
        state = A1H8_STATE(rot_a1h8_allPieces, sq);
        attackRange |= _a1h8[sq][state];
        rot_h1a8_allPieces = rotate_h1a8(allPieces);
        state = H1A8_STATE(rot_h1a8_allPieces, sq);
        attackRange |= _h1a8[sq][state];
    }
    else {
        /* Something is wrong, I don't know which piece this is.
         * This must NEVER happen; move generation is screwed.
         */
        assert(0);
    }
    return attackRange;
}

static int getPieceMoves(const position * const pos, const int player, const int type, bitboard rawMoves, const int sq, move store[])
{
    int numMoves = 0;
    int target = 0;
    move m;
    move temp;
    int valueCaptured;
    int valueThis;
    while(rawMoves != 0) {
        if(LSB_SET(rawMoves)) {
            m.from = sq;
            m.to = target;
            m.flags = 0;
            m.promoteTo = 0;
            m.thisPiece = pos->board[sq];
            m.capturedPiece = 0;
            m.eval = 0;
            if(CAPTURE == type) {
                m.flags |=  CAPTURE;
            }
            if(target == pos->epSquare) { /* deal with en passant */
                m.flags |= EN_PASSANT;
                if(WHITE == player) {
                    m.capturedPiece = BLACK_PAWN;
                }
                else {
                    m.capturedPiece = WHITE_PAWN;
                }
                m.eval += WHITE_PAWN; /* worth one pawn */
            }
            else {
                m.capturedPiece = pos->board[target]; /* will be 0 if target is empty square */
                /* following logic works even if non-capture, i.e. to square is empty */
                valueCaptured = abs(m.capturedPiece);
                valueThis = abs(m.thisPiece);
                m.eval += valueCaptured;
                if(valueCaptured > valueThis) {
                    m.eval += valueCaptured - valueThis;
                }
            }
            if(IS_PAWN(m.thisPiece) && IS_PROMOTION_SQUARE(player, m.to)) {
                m.flags |= PROMOTION;
                temp = m;
                if(WHITE == player) {
                    /* promote to queen */
                    m.promoteTo = WHITE_QUEEN;
                    m.eval += WHITE_QUEEN;
                    storeMoveIfLegal(pos, &m, player, store, &numMoves);
                    m = temp;
                    /* promote to rook */
                    m.promoteTo = WHITE_ROOK;
                    m.eval += WHITE_ROOK;
                    storeMoveIfLegal(pos, &m, player, store, &numMoves);
                    m = temp;
                    /* promote to bishop */
                    m.promoteTo = WHITE_BISHOP;
                    m.eval += WHITE_BISHOP;
                    storeMoveIfLegal(pos, &m, player, store, &numMoves);
                    m = temp;
                    /* promote to knight */
                    m.promoteTo = WHITE_KNIGHT;
                    m.eval += WHITE_KNIGHT;
                    storeMoveIfLegal(pos, &m, player, store, &numMoves);
                }
                else {
                    /* promote to queen */
                    m.promoteTo = BLACK_QUEEN;
                    storeMoveIfLegal(pos, &m, player, store, &numMoves);
                    m = temp;
                    /* promote to rook */
                    m.promoteTo = BLACK_ROOK;
                    storeMoveIfLegal(pos, &m, player, store, &numMoves);
                    m = temp;
                    /* promote to bishop */
                    m.promoteTo = BLACK_BISHOP;
                    storeMoveIfLegal(pos, &m, player, store, &numMoves);
                    m = temp;
                    /* promote to knight */
                    m.promoteTo = BLACK_KNIGHT;
                    storeMoveIfLegal(pos, &m, player, store, &numMoves);
                }
            }
            else {
                storeMoveIfLegal(pos, &m, player, store, &numMoves);
            }
        }
        rawMoves = rawMoves >> 1;
        target++;
    }
    /* castling */
    if(NORMAL == type && IS_KING(pos->board[sq])) {
        getCastlingMoves(pos, player, store, &numMoves);
    }
    return numMoves;
}

static void getCastlingMoves(const position * const pos, const int player, move store[], int *numMoves)
{
    int colour;
    int sq;
    move m;
    sq = pos->kingSquare[player];
    colour = COLOUR(pos->board[sq]);
    m.from = sq;
    m.flags = CASTLE;
    m.thisPiece = pos->board[sq];
    m.promoteTo = 0;
    m.capturedPiece = 0;
    m.eval = 0;
    if(WHITE == player && !inCheck(pos, WHITE, pos->kingSquare[WHITE])) {
        if((pos->castleFlags & CASTLE_WK) && (0 == pos->board[1]) && (0 == pos->board[2])) {
              if(!inCheck(pos, WHITE, 1) && !inCheck(pos, WHITE, 2)) {

                    assert(WHITE_ROOK == pos->board[0]);

                    m.to = 1;
                    storeMoveIfLegal(pos, &m, WHITE, store, numMoves);
              }
        }
        if((pos->castleFlags & CASTLE_WQ) && (0 == pos->board[4]) && (0 == pos->board[5]) && (0 == pos->board[6])) {
              if(!inCheck(pos, WHITE, 4) && !inCheck(pos, WHITE, 5)) {

                    assert(WHITE_ROOK == pos->board[7]);

                    m.to = 5;
                    storeMoveIfLegal(pos, &m, WHITE, store, numMoves);
              }
        }
    }
    else if(!inCheck(pos, BLACK, pos->kingSquare[BLACK])) {
        if((pos->castleFlags & CASTLE_BK) && (0 == pos->board[57]) && (0 == pos->board[58])) {
              if(!inCheck(pos, BLACK, 57) && !inCheck(pos, BLACK, 58)) {

                    assert(BLACK_ROOK == pos->board[56]);

                    m.to = 57;
                    storeMoveIfLegal(pos, &m, BLACK, store, numMoves);
              }
        }
        if((pos->castleFlags & CASTLE_BQ) && (0 == pos->board[62]) && (0 == pos->board[61]) && (0 == pos->board[60])) {
              if(!inCheck(pos, BLACK, 61) && !inCheck(pos, BLACK, 60)) {

                    assert(BLACK_ROOK == pos->board[63]);

                    m.to = 61;
                    storeMoveIfLegal(pos, &m, BLACK, store, numMoves);
              }
        }
    }
}

static void storeMoveIfLegal(const position* const pos, move* m, const int player, move store[], int *numMoves)
{
    position newPosition;
    int status;
    int opponent;
    status = playMove(pos, &newPosition, m, player);

    if(status != ILLEGAL) { /* move is legal */
        /* check that the move does not put players king in check, which would be illegal */
        if(!inCheck(&newPosition, player, newPosition.kingSquare[player])) {
            /* calculate evaluation */
            opponent = OPPONENT(player);
            if(inCheck(&newPosition, opponent, newPosition.kingSquare[opponent])) {
                m->flags |= CHECK;
                m->eval += EVAL_CHECK;
            }
            store[(*numMoves)++] = *m;
        }
    }
}

static bitboard getPieceBitboard(const position * const pos, const int piece)
{
    bitboard b = 0;
    int colour = COLOUR(piece);
    if(IS_KING(piece)) {
        b = pos->king[colour];
    }
    else if(IS_PAWN(piece)) {
        b = pos->pawns[colour];
    }
    else if(IS_KNIGHT(piece)) {
        b = pos->knights[colour];
    }
    else if(IS_BISHOP(piece)) {
        b = pos->bishops[colour];
    }
    else if(IS_ROOK(piece)) {
        b = pos->rooks[colour];
    }
    else if(IS_QUEEN(piece)) {
        b = pos->queens[colour];
    }
    else {
        /* Something is wrong, I don't know which piece this is.
         * This must NEVER happen; move generation is screwed.
         */
        assert(0);
    }
    return b;
}

/*
    Types of moves:
    1) King normal move
    2) King captures
    3) King castles
    4)
*/
int playMove(const position * const pos, position *newPosition, const move * const m, const int player)
{
    int opponent;
    opponent = OPPONENT(player);
    /* assert consistency of position */
    assertPosition(pos);
    /* opponent cannot be check, this should be handled elsewhere */
    assert(!inCheck(pos, opponent, pos->kingSquare[opponent]));
    /* init new position to old position */
    *newPosition = *pos;
    /* opponent has to play in the new position */
    newPosition->toPlay = opponent;
    movePiece(newPosition, m, player);
    if(IS_CAPTURE(m)) {
        /* clear bitmask for captured piece */
        clearCapturedPiece(newPosition, m);
    }
    /* ep is disabled after any move played */
    DISABLE_EP(newPosition);
    /* assert consistency of new position */
    assertPosition(newPosition);
    return 0;
}

static void movePawnExtra(position *pos, const move * const m, const int player)
{
    /* if ep, remove the opponents pawn */
    if(IS_EP(m)) {
        if(WHITE == player) {
            CLR_BIT(pos->pieces[BLACK], pos->epSquare - 8);
            CLR_BIT(pos->pawns[BLACK], pos->epSquare - 8);
            pos->board[pos->epSquare - 8] = 0;
        }
        else {
            CLR_BIT(pos->pieces[WHITE], pos->epSquare + 8);
            CLR_BIT(pos->pawns[WHITE], pos->epSquare + 8);
            pos->board[pos->epSquare + 8] = 0;
        }
    }
    else if(IS_PROMOTION(m)) { /* if promotion, replace 'to' pawn by promoted piece */
        pos->board[m->to] = m->promoteTo; /* change board */
        CLR_BIT(pos->pawns[player], m->to); /* pawn gone */
        /* add new piece to its bitmap */
        if(IS_KNIGHT(m->promoteTo)) {
            SET_BIT(pos->knights[player], m->to);
        }
        else if(IS_BISHOP(m->promoteTo)) {
            SET_BIT(pos->bishops[player], m->to);
        }
        else if(IS_ROOK(m->promoteTo)) {
            SET_BIT(pos->rooks[player], m->to);
        }
        else if(IS_QUEEN(m->promoteTo)) {
            SET_BIT(pos->queens[player], m->to);
        }
        else {
            assert(0);
        }
    }
    else {
        /* check if double advance and set ep square */
        if(16 == abs(m->to - m->from)) {
            if(WHITE == player) {
                pos->epSquare = m->to - 8;
            }
            else {
                pos->epSquare = m->to + 8;
            }
        }
    }
}

static void moveRookExtra(position *pos, const move * const m, const int player)
{
    int castleBits;
    /* check if castle flags need to be updated for appropriate player and (k | q) side */
    if(0 == m->from) { /* H1 rook */
        /* disable white king side castling */
        castleBits = CASTLE_WK; pos->castleFlags &= ~castleBits;
    }
    else if(7 == m->from) { /* A1 rook */
        /* disable white queen side castling */
        castleBits = CASTLE_WQ; pos->castleFlags &= ~castleBits;
    }
    else if(56 == m->from) { /* H8 rook */
        /* disable black king side castling */
        castleBits = CASTLE_BK; pos->castleFlags &= ~castleBits;
    }
    else if(63 == m->from) { /* A8 rook */
        /* disable black queen side castling */
        castleBits = CASTLE_BQ; pos->castleFlags &= ~castleBits;
    }
}

static void moveCastlingExtra(position *pos, const move * const m)
{
    if(1 == m->to) {
        pos->board[0] = 0;
        CLR_BIT(pos->pieces[WHITE], 0);
        CLR_BIT(pos->rooks[WHITE], 0);
        pos->board[2] = WHITE_ROOK;
        SET_BIT(pos->pieces[WHITE], 2);
        SET_BIT(pos->rooks[WHITE], 2);
    }
    else if(5 == m->to) {
        pos->board[7] = 0;
        CLR_BIT(pos->pieces[WHITE], 7);
        CLR_BIT(pos->rooks[WHITE], 7);
        pos->board[4] = WHITE_ROOK;
        SET_BIT(pos->pieces[WHITE], 4);
        SET_BIT(pos->rooks[WHITE], 4);
    }
    else if(57 == m->to) {
        pos->board[56] = 0;
        CLR_BIT(pos->pieces[BLACK], 56);
        CLR_BIT(pos->rooks[BLACK], 56);
        pos->board[58] = BLACK_ROOK;
        SET_BIT(pos->pieces[BLACK], 58);
        SET_BIT(pos->rooks[BLACK], 58);
    }
    else if(61 == m->to) {
        pos->board[63] = 0;
        CLR_BIT(pos->pieces[BLACK], 63);
        CLR_BIT(pos->rooks[BLACK], 63);
        pos->board[60] = BLACK_ROOK;
        SET_BIT(pos->pieces[BLACK], 60);
        SET_BIT(pos->rooks[BLACK], 60);
    }
    else {
        assert(0);
    }
}

static int movePiece(position *pos, const move * const m, const int player)
{
    int piece;
    int castleBits;
    piece = m->thisPiece;
    /* move piece to new location */
    pos->board[m->to] = pos->board[m->from];
    pos->board[m->from] = 0;
    CLR_BIT(pos->pieces[player], m->from);
    SET_BIT(pos->pieces[player], m->to);
    if(IS_KING(piece)) {
        CLR_BIT(pos->king[player], m->from);
        SET_BIT(pos->king[player], m->to);
        /* king position is tracked using kingSquare too */
        pos->kingSquare[player] = m->to;
        if(IS_CASTLE(m)) {
            moveCastlingExtra(pos, m);
        }
        /* king has moved, cannot castle now */
        castleBits = CASTLE_BITS(player);
        pos->castleFlags &= ~castleBits;
    }
    else if(IS_PAWN(piece)) {
        /* move the pawn as usual */
        CLR_BIT(pos->pawns[player], m->from);
        SET_BIT(pos->pawns[player], m->to);
        /* handle ep, promotion if required */
        movePawnExtra(pos, m, player);
    }
    else if(IS_KNIGHT(piece)) {
        CLR_BIT(pos->knights[player], m->from);
        SET_BIT(pos->knights[player], m->to);
    }
    else if(IS_BISHOP(piece)) {
        CLR_BIT(pos->bishops[player], m->from);
        SET_BIT(pos->bishops[player], m->to);
    }
    else if(IS_ROOK(piece)) {
        CLR_BIT(pos->rooks[player], m->from);
        SET_BIT(pos->rooks[player], m->to);
        /* disable castling if required */
        moveRookExtra(pos, m, player);
    }
    else if(IS_QUEEN(piece)) {
        CLR_BIT(pos->queens[player], m->from);
        SET_BIT(pos->queens[player], m->to);
    }
    else {
        printPosition(pos);
        printMove(m);
        fprintf(stderr, "Piece = %d\n", piece);

        /* Something is wrong, I don't know which piece this is.
         * This must NEVER happen; move generation is screwed.
         */
        assert(0);
    }
    return 0;
}

static void clearCapturedPiece(position *pos, const move * const m)
{
    int piece;
    int colour;
    int castleBits;

    /*
        ep is handled in movePiece(...) method and not here
        this only handles the "simple" capture
        TODO: change later?
     */
    piece = m->capturedPiece;
    colour = COLOUR(piece);
    assert(!IS_KING(piece));
    /* clear all pieces bitboard */
    CLR_BIT(pos->pieces[colour], m->to);
    /*
        Do NOT set m->to to 0, as board[m->to] contains the moved piece!
        Just update the bitboards.
     */
    if(IS_PAWN(piece)) {
        CLR_BIT(pos->pawns[colour], m->to);
    }
    else if(IS_KNIGHT(piece)) {
        CLR_BIT(pos->knights[colour], m->to);
    }
    else if(IS_BISHOP(piece)) {
        CLR_BIT(pos->bishops[colour], m->to);
    }
    else if(IS_ROOK(piece)) {
        CLR_BIT(pos->rooks[colour], m->to);
    }
    else if(IS_QUEEN(piece)) {
        CLR_BIT(pos->queens[colour], m->to);
    }
    else {
        /* Something is wrong, I don't know which piece this is.
         * This must NEVER happen; move generation is screwed.
         */
        assert(0);
    }

    /* check if rook square has been captured and disable castling */
    if(m->to == 0) {
        castleBits = CASTLE_WK;
        pos->castleFlags &= ~castleBits;
    }
    else if(m->to == 7) {
        castleBits = CASTLE_WQ;
        pos->castleFlags &= ~castleBits;

    }
    else if(m->to == 56) {
        castleBits = CASTLE_BK;
        pos->castleFlags &= ~castleBits;

    }
    else if(m->to == 63) {
        castleBits = CASTLE_BQ;
        pos->castleFlags &= ~castleBits;

    }

}

int inCheck(const position * const pos, const int player, const int targetSquare)
{
    bitboard attacks = 0;
    int opponent = OPPONENT(player);
    int sq = 0;
    bitboard knights = 0;
    bitboard allPieces = 0;
    bitboard rooks = 0;
    bitboard bishops = 0;
    bitboard pawns = 0;
    int rankState;
    int fileState;
    int rank;
    int file;
    bitboard rot_a1h8;
    bitboard rot_h1a8;
    int state;
    bitboard target;
    /* should be used only for kings or empty squares */
    assert(((targetSquare == pos->kingSquare[player])) ||
        ((_mask[targetSquare] & (pos->pieces[WHITE] | pos->pieces[BLACK])) == 0));
    if(targetSquare == pos->kingSquare[player]) {
        target = pos->king[player];
    }
    else {
        target = _mask[targetSquare];
    }
    /* opponent king attacks */
    attacks |= _king[pos->kingSquare[opponent]];
    /* incremental checking... */
    if(attacks & target) {
        return 1;
    }
    /* opponent knight attacks */
    knights = pos->knights[opponent];
    sq = 0;
    while(knights != 0) {
        if(LSB_SET(knights)) {
            attacks |= _knight[sq];
            /* incremental checking... */
            if(attacks & target) {
                return 1;
            }
        }
        knights = knights >> 1;
        sq++;
    }
    allPieces = pos->pieces[WHITE] | pos->pieces[BLACK];

    /* rook and queen attacks */
    rooks = pos->rooks[opponent] | pos->queens[opponent];

    sq = 0;
    while(rooks != 0) {
        if(LSB_SET(rooks)) {
            rank = sq / 8;
            rankState = RANK_STATE(allPieces, rank);
            attacks |= _horz[sq][rankState];
            file = sq % 8;
            fileState = FILE_STATE(rotate(allPieces), file);
            attacks |= _vert[sq][fileState];


            /* incremental checking... */
            if(attacks & target) {
                return 1;
            }
        }
        rooks = rooks >> 1;
        sq++;
    }
    /* bishop and queen attacks */
    bishops = pos->bishops[opponent] | pos->queens[opponent];
    sq = 0;

    attacks = 0; // remove
    while(bishops != 0) {
        if(LSB_SET(bishops)) {

            rot_a1h8 = rotate_a1h8(allPieces);
            state = A1H8_STATE(rot_a1h8, sq);
            attacks |= _a1h8[sq][state];
            rot_h1a8 = rotate_h1a8(allPieces);
            state = H1A8_STATE(rot_h1a8, sq);
            attacks |= _h1a8[sq][state];

            /* incremental checking... */
            if(attacks & target) {
                return 1;
            }
        }
        bishops = bishops >> 1;
        sq++;
    }
    /* pawn attacks */
    pawns = pos->pawns[opponent];
    sq = 0;
    while(pawns != 0) {
        if(LSB_SET(pawns)) {
            if(WHITE == opponent) {
                attacks |= _wpawn_attack[sq];
            }
            else {
                attacks |= _bpawn_attack[sq];
            }
            /* incremental checking... */
            if(attacks & target) {
                return 1;
            }
        }
        pawns = pawns >> 1;
        sq++;
    }
    return 0;
}
