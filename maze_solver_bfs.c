/*
 * maze_solver_bfs.c -- a breadth-first search maze solver
 *
 * Artsiom Dzenisiuk 16141253
 * Universiteit van Amsterdam
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "maze.h"
#include "queue.h"

#define NOT_FOUND -1
#define ERROR -2
#define QUEUE_SIZE 4000

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
    fprintf(stderr, "bfs_solve_helper: ");
    vfprintf(stderr, fmt, va);
    va_end(va);
}

/**
 * bfs_solve_helper -- solves a maze using Breadth-First Search
 * @m: the maze to solve
 * @sr: start row
 * @sc: start column
 * @dr: destination row
 * @dc: destination column
 *
 * Return: the length of the path, if found; otherwise, NOT_FOUND if
 *         or ERROR if an error occured.
 */
int bfs_solve_helper(struct maze *m, int sr, int sc, int dr, int dc)
{
    ulog("start           = (%d, %d).\n", sr, sc);
    ulog("destination     = (%d, %d).\n", dr, dc);

    struct queue *rqueue = queue_init(QUEUE_SIZE);
    if (rqueue == NULL) {
        return ERROR;
    }

    struct queue *cqueue = queue_init(QUEUE_SIZE);
    if (cqueue == NULL) {
        queue_cleanup(rqueue);
        return ERROR;
    }

    int ***graph = new_graph(maze_size(m));
    if (graph == NULL) {
        queue_cleanup(rqueue);
        queue_cleanup(cqueue);
        return ERROR;
    }

    queue_push(rqueue, sr);
    queue_push(cqueue, sc);

    while (1) {
        int r = queue_peek(rqueue);
        int c = queue_peek(cqueue);

        maze_set(m, r, c, VISITED);

        if (r == dr && c == dc) {
            int path_length = 0;
            while (1) {
                if (r == sr && c == sc) {
                    free_graph(graph, maze_size(m));
                    queue_cleanup(rqueue);
                    queue_cleanup(cqueue);
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
                queue_push(rqueue, nr);
                queue_push(cqueue, nc);
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
            if (queue_size(rqueue) < 1 || queue_size(cqueue) < 1) {
                ulog("nothing found;\n"
                     "    queue_size(rqueue) == %zu;\n",
                     "    queue_size(cqueue) == %zu;\n",
                     queue_size(rqueue), queue_size(cqueue));
                queue_cleanup(rqueue);
                queue_cleanup(cqueue);
                free_graph(graph, maze_size(m));
                return ERROR;
            }

            queue_pop(rqueue);
            queue_pop(cqueue);
        }
    }
}

/**
 * bfs_solve -- solves a maze using Breadth-First Search
 * @m: the maze to solve
 *
 * This function determines the coordinates of the start and destination.
 * It verifies that there is exactly one of each and then it passes
 * the coordinates to bfs_solve_helper.
 *
 * Return: the length of the path, if found; otherwise, NOT_FOUND if
 *         or ERROR if an error occured.
 */
int bfs_solve(struct maze *m) {
    int sr = -1;
    int sc = -1;
    int dr = -1;
    int dc = -1;

    for (int r = 0; r < maze_size(m); r++) {
        for (int c = 0; c < maze_size(m); c++) {
            if (maze_at_start(m, r, c)) {
                if (sr != -1 || sc != -1) {
                    ulog("bfs_solve: found start twice.\n");
                    return ERROR;
                }

                sr = r;
                sc = c;
            } else if (maze_at_destination(m, r, c)) {
                if (dr != -1 || dc != -1) {
                    ulog("bfs_solve: found destination twice.\n");
                    return ERROR;
                }

                dr = r;
                dc = c;
            }
        }
    }

    if (sr == -1 || sc == -1 || dr == -1 || dc == -1) {
        ulog("bfs_solve: coudn't find start/destination;\n"
                        "    sr=%d, sc=%d, dr=%d, dc=%d.\n", sr, sc, dr, dc);
        return ERROR;
    }

    return bfs_solve_helper(m, sr, sc, dr, dc);
}


int main(void) {
    /* read maze */
    struct maze *m = maze_read();
    if (!m) {
        printf("Error reading maze\n");
        return 1;
    }

    /* solve maze */
    int path_length = bfs_solve(m);
    if (path_length == ERROR) {
        printf("bfs failed\n");
        maze_cleanup(m);
        return 1;
    } else if (path_length == NOT_FOUND) {
        printf("no path found from start to destination\n");
        maze_cleanup(m);
        return 1;
    }
    printf("bfs found a path of length: %d\n", path_length);

    /* print maze */
    maze_print(m, false);
    maze_output_ppm(m, "out.ppm");

    maze_cleanup(m);
    return 0;
}
