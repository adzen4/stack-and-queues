# Set the default compiler to gcc
CC = gcc
# CC = clang

# Use C11, debugging and warnings flags
define CFLAGS
-std=c11 \
-g3 \
-fsanitize=address \
-Wpedantic \
-Wall \
-Wextra \
-Wformat=2 \
-Wsign-conversion \
-Wswitch-enum \
-Wno-switch-default \
-Wunused \
-Wfloat-equal \
-Wconversion \
-Wpointer-arith \
-Wtype-limits \
-Wcast-qual \
-Wenum-compare \
-Wsizeof-pointer-memaccess \
`pkg-config --cflags check` \
-Wstrict-prototypes
endef

# Turn on the address sanitizer and link math library
LDFLAGS = -fsanitize=address -lm

# Flags needed for the check library
CHECK_LDFLAGS = $(LDFLAGS) `pkg-config --libs check`

PROG = maze_solver_dfs maze_solver_bfs
TESTS = check_stack check_queue check_malloc check_null

all: $(PROG) $(TESTS)

valgrind: LDFLAGS=-lm
valgrind: CFLAGS=-Wall -g3
valgrind: $(PROG) $(TESTS)

release: LDFLAGS=-lm
release: CFLAGS=-O3
release: $(PROG)

stack.o: stack.c stack.h

queue.o: queue.c queue.h

maze.o: maze.c maze.h

maze_solver_dfs: maze_solver_dfs.o maze.o stack.o
	$(CC) -o $@ $^ $(LDFLAGS)

maze_solver_bfs: maze_solver_bfs.o maze.o queue.o
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -f *.o $(PROG) $(TESTS)

tarball: maze_solver_submit.tar.gz

maze_solver_submit.tar.gz: maze_solver_dfs.c maze_solver_bfs.c \
			queue.c queue.h stack.c stack.h Makefile
	tar -czf $@ $^

check_stack: check_stack.o stack.o
	$(CC) -o $@ $^ $(CHECK_LDFLAGS)

check_queue: check_queue.o queue.o
	$(CC) -o $@ $^ $(CHECK_LDFLAGS)

check_malloc: LDFLAGS=$(shell pkg-config --libs check) -ldl -fsanitize=address
check_malloc: CFLAGS=-std=c11 `pkg-config --cflags check` -g3 -Wall -fsanitize=address
check_malloc: check_malloc.o stack.o queue.o
	$(CC) -o $@ $^ $(LDFLAGS)

check_null: check_null.o stack.o queue.o
	$(CC) -o $@ $^ $(CHECK_LDFLAGS)

check: all
	@echo
	@echo "Testing the stack implementation..."
	./check_stack
	@echo
	@echo "Testing the queue implementation..."
	./check_queue
	@echo
	@echo "Testing if null arguments are handled correctly"
	./check_null
	@echo
	@echo "Testing if errors in functions are checked"
	./check_malloc
	@echo
	@echo "Running maze solver tests..."
	./check_maze_solvers.sh
