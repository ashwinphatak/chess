#ifndef _EVAL_H_
#define _EVAL_H

#include "position.h"


#define INFINITY 32767


int evaluate(const position * const pos);

int evaluateRelative(const position * const pos);

#endif
