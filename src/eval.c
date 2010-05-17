#include "move.h"
#include "eval.h"

static int evaluateMaterial(const position * const pos);
static int fullEvaluation(const position * const pos);

extern move _moveStore[64][256];


bitboard counterEvaluation;

static const int _knight_weight[64] = {

    -5,  0, -3, -1, -1, -3,  0, -5,
    -4, -3, -2,  2,  2, -2, -3, -4,
    -2, -1,  3,  1,  1,  3, -1, -2,
    -2,
    -2,
    -2, -1,  3,  1,  1,  3, -1, -2,
    -4, -3, -2,  2,  2, -2, -3, -4,
    -5,  0, -3, -1, -1, -3,  0, -5,

};

int evaluateRelative(const position * const pos)
{
    int status;
    int player = pos->toPlay;
    int eval;

    counterEvaluation++;

    /* special handling for checkmate and stalemate */
    status = getAllMoves(pos, player, 0);
    if(LOST == status) {
        return (WHITE == player)?(-INFINITY):(INFINITY);
    }
    else if(STALEMATE == status) {
        return 0;
    }
    else {
        eval = fullEvaluation(pos);
        return (WHITE == player)?(eval):(-eval);

    }
}


int evaluate(const position * const pos)
{
    int status;
    int player = pos->toPlay;

    counterEvaluation++;

    /* special handling for checkmate and stalemate */
    status = getAllMoves(pos, player, 0);
    if(LOST == status) {
        return (WHITE == player)?(-INFINITY):(INFINITY);
    }
    else if(STALEMATE == status) {
        return 0;
    }
    else {
        return fullEvaluation(pos);

    }
}

static int fullEvaluation(const position * const pos)
{
    return evaluateMaterial(pos);
}


static int evaluateMaterial(const position * const pos)
{
    int sq;
    int eval = 0;

    for(sq = 63; sq >= 0; sq--) {
        eval += pos->board[sq];
    }

    return eval;
}
