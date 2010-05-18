#ifndef _SEARCH_H_
#define _SEARCH_H_

#include "position.h"

void printMatingVariations(const position * const pos);

int search(const position * const pos, int depth, int store, int alpha, int beta, int isMaximizing);

#endif
