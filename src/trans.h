#ifndef _TRANS_H_
#define _TRANS_H_

#include "bitboard.h"
#include "position.h"

/* 256 KB hash table */
#define HASHTABLE_SIZE  (256 * 1024)

typedef struct {
    int hash;
    int eval;
    int depth;
    int alpha;
    int beta;
    bitboard fullHash;
    move bestMove;
    
} transEntry;


int storeInTable(const position * const pos, const int alpha, const int beta, const move * const bestMove);
int loadFromTable(const position * const pos, transEntry * e);

#endif
