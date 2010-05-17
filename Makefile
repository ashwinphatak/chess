WARN_FLAGS=-Wall
PLATFORM=-D LINUX
CC=gcc


CORE_OBJS=chess.o diag.o bitboard.o fen.o position.o move.o eval.o search.o

chess: $(CORE_OBJS)
	$(CC) $(WARN_FLAGS) $(PLATFORM) -o chess $(CORE_OBJS)

chess.o: src/chess.c
	$(CC) $(WARN_FLAGS) $(PLATFORM) -c src/chess.c

diag.o: src/diag.c src/bitboard.h
	$(CC) $(WARN_FLAGS) $(PLATFORM) -c src/diag.c

bitboard.o: src/bitboard.c src/bitboard.h
	$(CC) $(WARN_FLAGS) $(PLATFORM) -c src/bitboard.c

fen.o: src/fen.c src/fen.h src/position.h
	$(CC) $(WARN_FLAGS) $(PLATFORM) -c src/fen.c

position.o: src/position.c src/position.h
	$(CC) $(WARN_FLAGS) $(PLATFORM) -c src/position.c

move.o: src/move.c src/bitboard.h src/position.h src/move.h
	$(CC) $(WARN_FLAGS) $(PLATFORM) -c src/move.c

eval.o: src/eval.c src/eval.h 
	$(CC) $(WARN_FLAGS) $(PLATFORM) -c src/eval.c

search.o: src/search.c src/search.h 
	$(CC) $(WARN_FLAGS) $(PLATFORM) -c src/search.c

test: test_mate
	

test_fen: test_fen.o bitboard.o move.o position.o fen.o diag.o 
	$(CC) $(WARN_FLAGS) $(PLATFORM) -o test_fen test_fen.o bitboard.o move.o position.o fen.o diag.o
	./test_fen

test_fen.o: test/test_fen.c src/bitboard.h src/move.h src/position.h src/fen.h src/diag.h
	$(CC) $(WARN_FLAGS) $(PLATFORM) -I src -c test/test_fen.c

test_moves: test_moves.o bitboard.o move.o position.o fen.o diag.o eval.o search.o
	$(CC) $(WARN_FLAGS) $(PLATFORM) -o test_moves test_moves.o bitboard.o move.o position.o fen.o diag.o eval.o search.o
	./test_moves

test_moves.o: test/test_moves.c src/bitboard.h src/move.h src/position.h src/fen.h src/diag.h
	$(CC) $(WARN_FLAGS) $(PLATFORM) -I src -c test/test_moves.c

test_mate: test_mate.o bitboard.o move.o position.o fen.o diag.o eval.o search.o
	$(CC) $(WARN_FLAGS) $(PLATFORM) -o test_mate test_mate.o bitboard.o move.o position.o fen.o diag.o eval.o search.o
	./test_mate

test_mate.o: test/test_mate.c src/bitboard.h src/move.h src/position.h src/fen.h src/diag.h
	$(CC) $(WARN_FLAGS) $(PLATFORM) -I src -c test/test_mate.c

clean:
	rm -rf *.o

