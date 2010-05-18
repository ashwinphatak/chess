#include <stdio.h>
#include "move.h"
#include "search.h"
#include "diag.h"
#include "eval.h"

/* stores the best variation */
extern move _moveStore[64][256];

move _variation[64];

void printMatingVariations(const position * const pos)
{
    position newPos1, newPos2, newPos3;
    int numMoves1;
    int numMoves2;
    int numMoves3;
    int e;
    int i, j, k;
    int best1;
    int worst2;
    int best3;
    move m1, m2, m3;
    int alpha, beta;

    numMoves1 = getAllMovesSorted(pos, pos->toPlay, 1);
    if(numMoves1 == LOST || numMoves1 == STALEMATE) {
        fprintf(stderr, "White lost or stalemate ");
        return;
    }


    alpha = -INFINITY;
    beta = INFINITY;

    best1 = -INFINITY;
    for(i = 0; i < numMoves1; i++) {
        m1 = _moveStore[1][i];
        playMove(pos, &newPos1, &_moveStore[1][i], pos->toPlay);
        numMoves2 = getAllMovesSorted(&newPos1, newPos1.toPlay, 2);
        if(numMoves2 == LOST) {
            printMove(&_moveStore[1][i]); fprintf(stderr, " ");
            return;
        }
        else if(numMoves2 != STALEMATE) {
            worst2 = INFINITY;
            for(j = 0; j < numMoves2; j++) {
                m2 = _moveStore[2][j];
                playMove(&newPos1, &newPos2, &_moveStore[2][j], newPos1.toPlay);
                numMoves3 = getAllMovesSorted(&newPos2, newPos2.toPlay, 3);
                if(numMoves3 != LOST && numMoves3 != STALEMATE) {
                    best3 = -INFINITY;
                    for(k = 0; k < numMoves3; k++) {
                        m3 = _moveStore[3][k];
                        playMove(&newPos2, &newPos3, &_moveStore[3][k], newPos2.toPlay);
                        e = evaluate(&newPos3);

                        if(e > best3) {
                            best3 = e;
                            if(best3 == INFINITY) break;
                        }

                    }
                }

                if(best3 < worst2) {
                    worst2 = best3;
                    if(worst2 == -INFINITY) break;
                }

            }
        }

        if(best1 < worst2) {
            best1 = worst2;

            if(best1 == INFINITY) {
                printMove(&m1); fprintf(stderr, " ");
                printMove(&m2); fprintf(stderr, " ");
                printMove(&m3); fprintf(stderr, " ");
                break;
            }
        }
    }


    return;
}


int search(const position * const pos, int depth, int store, int alpha, int beta, int isMaximizing)
{
    int i;
    position newPos;
    int e;
    int numMoves;

    /*fprintf(stderr, "Depth = %d, Store = %d, alpha = %d, beta = %d, isMaximizing = %d\n",
                        depth, store, alpha, beta, isMaximizing);*/
    
    if(depth == 0) {
        return evaluate(pos);
    }
    
    
    numMoves = getAllMovesSorted(pos, pos->toPlay, store);
    if(numMoves == LOST) {
        return (pos->toPlay == WHITE)?(-INFINITY):(INFINITY);
    }
    else if(numMoves == STALEMATE) {
        return 0;
    }
    
    for(i = 0; i < numMoves; i++) {
        playMove(pos, &newPos, &_moveStore[store][i], pos->toPlay);
        e = search(&newPos, depth - 1, store + 1, alpha, beta, isMaximizing ^ 1);
        if(isMaximizing) {
            if(e > alpha) {
                alpha = e;
                _variation[store] = _moveStore[store][i];
            }
            
            if(beta <= alpha) {
                return beta;
            }
        }
        else {
            if(e < beta) {
                beta = e;
                _variation[store] = _moveStore[store][i];
            }
            
            if(beta <= alpha) {
                return alpha;
            }
        }
    }
    
    return (isMaximizing)?(alpha):(beta);
}
