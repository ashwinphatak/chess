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
#include "fen.h"
#include "position.h"
#include "diag.h"
#include "eval.h"
#include "search.h"

extern move _moveStore[64][256];

extern bitboard counterEvaluation;
extern bitboard counterMove;
extern move _variation[64];

int main(void)
{
    int n;
    int i;
    position pos;
    int e;
    /* position newPos; */

    /* sample FEN */
    /*char test1[] = "8/1n6/B3K1k1/8/8/8/1RR5/8 w - -";*/
    /*char test1[] = "8/P7/4K1k1/8/8/P7/8/8 w - -";*/
    /*char test1[] = "1k6/3P4/8/2pP4/8/8/P4PPP/R3K2R w KQ c6";*/
    /* char test1[] = "7q/8/8/8/8/8/5k2/7K w - -";*/
    /* char test1[] = "8/8/8/8/8/7q/5k2/7K w - -"; */
    /*char test1[] = "K4Q1b/2p4N/p6P/3Pkb2/Rn6/q1pp3P/1N3BB1/8 w - -";*/
    /*char test1[] = "r1b4q/ppk2N2/nbpp2p1/8/3P1B2/6P1/PPP4P/R3R1K1 w - -";*/
    /*char test1[] = "3brrb1/2N4B/8/2p4Q/2p2k2/5P2/4P1KR/2N2RB1 w - -";*/
    /*char test1[] = "8/4pb1N/3BkN2/1p3p2/1Qp5/K6B/8/8 w - -";*/
    /*char test1[] = "8/5b1N/3Bkp2/1p3p2/1Qp5/K7/6B1/8 w - -";*/
    /*char test1[] = "8/1PP1kPPP/1nKNNn2/8/8/8/8/8 w - -";*/
    /* char test1[] = "2B1k2r/5p2/5B2/7p/8/KR6/8/8 w k -";*/
    /*char test1[] = "2B1k2r/5p2/5B2/8/8/KR6/8/8 w - -";*/
    /*char test1[] = "8/8/8/6B1/2B3K1/b1Q5/r3P3/3k4 w - -";*/
    /*char test1[] = "8/8/8/6pq/3Q2nk/2R3br/5PP1/K2B4 w - -";*/
    /* char test1[] = "5bb1/2K1p3/2p5/7r/k7/1q4R1/8/2QBB3 w - -";*/
    /* char test1[] = "B7/1K6/2r2p2/3kp3/N1N3Q1/8/2R5/1n6 w - -";*/
    /*char test1[] = "8/8/3nB3/7p/5Q2/2pkP1p1/8/R3K3 w Q -";*/
    char test1[] = "3brrb1/2N4B/8/2p4Q/2p2k2/5P2/4P1KR/2N2RB1 w - - []";
    /* mate in 3 char test1[] = "2r2bnr/1b3ppp/p1k5/B1p5/P7/5NP1/2P1QP1P/1R4K1 w - - []";*/

    init();


    loadPositionFromFEN(test1, &pos);
    printPosition(&pos);

    /*n = getAllMovesSorted(&pos, WHITE, 1);
    if(LOST == n) {
        fprintf(stderr, "Player lost\n");
    }
    else if(STALEMATE == n) {
        fprintf(stderr, "Stalemate\n");
    }
    else {
        fprintf(stderr, "Number of moves: %d\n", n);

        for(i = 0; i < n; i++) {
            printMove(&_moveStore[1][i]);
            fprintf(stderr, ", ");
        }

        fprintf(stderr, "\n");
    }

    fprintf(stderr, "Position evaluation: %d\n", evaluate(&pos));*/

    //printMatingVariations(&pos);
    //e = search(&pos, 2, 1);
    //fprintf(stderr, "Evaluation: %d\n", e);
    //printMove(&_moveStore[1][0]);
    
    
    fprintf(stderr, "Starting search...\n");
    i = search(&pos, 3, 1, -INFINITY, INFINITY, 1);
    fprintf(stderr, "Search = %d: ", i);
    
    printMove(&_variation[1]); fprintf(stderr, " ");
    printMove(&_variation[2]); fprintf(stderr, " ");
    printMove(&_variation[3]);

    fprintf(stderr, "\nPositions evaluated: %lld\n", counterEvaluation);
    fprintf(stderr, "Moves generated: %lld\n", counterMove);

    return 0;
}

