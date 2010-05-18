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

#ifndef _FEN_H_
#define _FEN_H_

#include "position.h"

/* A FEN string (line) cannot be more than this limit */
#define FEN_MAX_LINE 256

/* Error codes for FEN parsing */
#define ERR_PARSE       -1  /* General parse error */
#define ERR_MAX_LENGTH  -2  /* FEN string is too long */
#define ERR_NO_WKING    -3  /* white king missing */
#define ERR_NO_BKING    -4  /* black king missing */

/*
 * Load position from the given FEN string.
 * Return 0 on success or error code.
 */
int loadPositionFromFEN(const char* const fen, position *pos);

/*
 * Save position to the given char array in FEN notation
 * The caller should ensure the array length is FEN_MAX_LINE
 */
int savePositionToFEN(const position * const pos, char *fen);

#endif


