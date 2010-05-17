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

#include <assert.h>
#include <string.h>
#include "position.h"

/* Clear the chess position */
void clearPosition(position *pos)
{
    memset(pos, 0, sizeof(position));
    pos->toPlay = -1;
    pos->epSquare = -1;
}


void assertPosition(const position * const pos)
{
    bitboard allPieces;

    /* check that kings are present and in correct positions */
    assert(pos->king[WHITE] != 0);
    assert(pos->king[BLACK] != 0);

    /* white and black kings cannot be in the same place */
    assert(pos->kingSquare[WHITE] != pos->kingSquare[BLACK]);

    /* pieces bitboard is consistent with the other bitboards */
    allPieces = pos->king[WHITE] | pos->queens[WHITE] |
        pos->rooks[WHITE] | pos->bishops[WHITE] |
        pos->knights[WHITE] | pos->pawns[WHITE];
    assert(allPieces == pos->pieces[WHITE]);

    allPieces = pos->king[BLACK] | pos->queens[BLACK] |
        pos->rooks[BLACK] | pos->bishops[BLACK] |
        pos->knights[BLACK] | pos->pawns[BLACK];
    assert(allPieces == pos->pieces[BLACK]);

    /* either white or black has the move */
    assert(WHITE == pos->toPlay || BLACK == pos->toPlay);
}
