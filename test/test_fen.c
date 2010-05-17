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
#include <string.h>
#include "bitboard.h"
#include "move.h"
#include "position.h"
#include "fen.h"
#include "diag.h"

int main(void)
{
    char fen[FEN_MAX_LINE];
    position pos;

    /* sample FEN */
    char test1[] = "8/8/4K1k1/8/8/8/8/8 w - -";

    init();

    loadPositionFromFEN(test1, &pos);
    savePositionToFEN(&pos, fen);

    if(strcmp(test1, fen) != 0) {
        printf("Test failed!\n");
        printf("Expected: %s\n", test1);
        printf("Actual: %s\n", fen);

        return -1;
    }


    return 0;
}

