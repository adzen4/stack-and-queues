#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "maze.h"
#include "stack.h"

#define NOT_FOUND -1
#define ERROR -2
#define STACK_SIZE 4000

static int ***new_graph(int maze_size)
{
    int ***graph = malloc(maze_size * sizeof(int **));

    for (int i = 0; i < maze_size; i++) {
        graph[i] = malloc(maze_size * sizeof(int *));

        for (int j = 0; j < maze_size; j++) {
            graph[i][j] = malloc(sizeof(int) * 2);
        }
    }

    return graph;
}

static void free_graph(int ***graph, int maze_size)
{
    for (int i = 0; i < maze_size; i++) {
        for (int j = 0; j < maze_size; j++) {
            free(graph[i][j]);
        }
        free(graph[i]);
    }
    free(graph);
}

static void ulog(const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    fprintf(stderr, "dfs_solve_helper: ");
    vfprintf(stderr, fmt, va);
    va_end(va);
}

/**
 * dfs_solve_helper -- solves a maze using Depth-First Search
 * @m: the maze to solve
 * @sr: start row
 * @sc: start column
 * @dr: destination row
 * @dc: destination column
 *
 * Return: the length of the path, if found; otherwise, NOT_FOUND if
 *         or ERROR if an error occured.
 */
int dfs_solve_helper(struct maze *m, int sr, int sc, int dr, int dc)
{
    ulog("start           = (%d, %d).\n", sr, sc);
    ulog("destination     = (%d, %d).\n", dr, dc);

    struct stack *rstack = stack_init(STACK_SIZE);
    if (rstack == NULL) {
        return ERROR;
    }

    struct stack *cstack = stack_init(STACK_SIZE);
    if (cstack == NULL) {
        stack_cleanup(rstack);
        return ERROR;
    }

    int ***graph = new_graph(maze_size(m));
    if (graph == NULL) {
        stack_cleanup(rstack);
        stack_cleanup(cstack);
        return ERROR;
    }

    stack_push(rstack, sr);
    stack_push(cstack, sc);

    while (1) {
        int r = stack_peek(rstack);
        int c = stack_peek(cstack);

        maze_set(m, r, c, VISITED);

        if (r == dr && c == dc) {
            int path_length = 0;
            while (1) {
                if (r == sr && c == sc) {
                    free_graph(graph, maze_size(m));
                    stack_cleanup(rstack);
                    stack_cleanup(cstack);
                    return path_length;
                }

                maze_set(m, r, c, PATH);
                int tmp = r;
                r = graph[tmp][c][0];
                c = graph[tmp][c][1];
                path_length++;
            }
        }

        bool dead_end = true;

        for (size_t direction = 0; direction < N_MOVES; direction++) {
            int nr = r + m_offsets[direction][0];
            int nc = c + m_offsets[direction][1];

            if (maze_get(m, nr, nc) == FLOOR) {
                dead_end = false;
                stack_push(rstack, nr);
                stack_push(cstack, nc);
                maze_set(m, nr, nc, VISITED);
                graph[nr][nc][0] = r;
                graph[nr][nc][1] = c;
                ulog("next found at     (%d, %d).\n", nr, nc);
            } else {
                ulog("blocking found at (%d, %d) is '%c'.\n",
                     nr, nc, maze_get(m, nr, nc));
            }
        }

        if (dead_end) {
            if (stack_size(rstack) < 1 || stack_size(cstack) < 1) {
                ulog("nothing found;\n"
                     "    stack_size(rstack) == %zu;\n",
                     "    stack_size(cstack) == %zu;\n",
                     stack_size(rstack), stack_size(cstack));
                stack_cleanup(rstack);
                stack_cleanup(cstack);
                free_graph(graph, maze_size(m));
                return ERROR;
            }

            stack_pop(rstack);
            stack_pop(cstack);
        }
    }
}

/**
 * dfs_solve -- solves a maze using Depth-First Search
 * @m: the maze to solve
 *
 * This function determines the coordinates of the start and destination.
 * It verifies that there is exactly one of each and then it passes
 * the coordinates to dfs_solve_helper.
 *
 * Return: the length of the path, if found; otherwise, NOT_FOUND if
 *         or ERROR if an error occured.
 */
int dfs_solve(struct maze *m) {
    int sr = -1;
    int sc = -1;
    int dr = -1;
    int dc = -1;

    for (int r = 0; r < maze_size(m); r++) {
        for (int c = 0; c < maze_size(m); c++) {
            if (maze_at_start(m, r, c)) {
                if (sr != -1 || sc != -1) {
                    ulog("dfs_solve: found start twice.\n");
                    return ERROR;
                }

                sr = r;
                sc = c;
            } else if (maze_at_destination(m, r, c)) {
                if (dr != -1 || dc != -1) {
                    ulog("dfs_solve: found destination twice.\n");
                    return ERROR;
                }

                dr = r;
                dc = c;
            }
        }
    }

    if (sr == -1 || sc == -1 || dr == -1 || dc == -1) {
        ulog("dfs_solve: coudn't find start/destination;\n"
                        "    sr=%d, sc=%d, dr=%d, dc=%d.\n", sr, sc, dr, dc);
        return ERROR;
    }

    return dfs_solve_helper(m, sr, sc, dr, dc);
}

int main(void) {
    /* read maze */
    struct maze *m = maze_read();
    if (!m) {
        printf("Error reading maze\n");
        return 1;
    }

    /* solve maze */
    int path_length = dfs_solve(m);
    if (path_length == ERROR) {
        printf("dfs failed\n");
        maze_cleanup(m);
        return 1;
    } else if (path_length == NOT_FOUND) {
        printf("no path found from start to destination\n");
        maze_cleanup(m);
        return 1;
    }
    printf("dfs found a path of length: %d\n", path_length);

    /* print maze */
    maze_print(m, false);
    maze_output_ppm(m, "out.ppm");

    maze_cleanup(m);
    return 0;
}
