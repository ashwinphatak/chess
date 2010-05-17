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
#include <stdlib.h>
#include <time.h>
#include "bitboard.h"
#include "move.h"
#include "fen.h"
#include "position.h"
#include "diag.h"
#include "eval.h"
#include "search.h"

extern move _moveStore[64][256];

extern bitboard counterEvaluation;
extern bitboard counterMove;

int main(void)
{
    position pos;
    char test[FEN_MAX_LINE];
    FILE *p;

    init();

    p = fopen("find_mate.fen", "rt");
    if(p == NULL) {
        printf("Error opening find_mate.fen\n");
        exit(0);
    }

    while(!feof(p)) {
        if(fgets(test, FEN_MAX_LINE, p) == NULL) break;
        loadPositionFromFEN(test, &pos);
        printMatingVariations(&pos); fprintf(stderr, "\n");
        //fprintf(stderr, "Search = %d\n", search(&pos, 3, 1, -INFINITY, INFINITY, 1));
    }

    fclose(p);

    return 0;
}

