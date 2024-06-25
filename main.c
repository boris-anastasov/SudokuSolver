#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 9

void read_sdm_file(const char *filepath, char **header, char ***puzzles, int *num_puzzles) {
    FILE *file = fopen(filepath, "r");
    if (!file) {
        perror("Unable to open file");
        exit(EXIT_FAILURE);
    }

    size_t len = 0;
    ssize_t read;
    char *line = NULL;

    if ((read = getline(&line, &len, file)) != -1) {
        *header = strdup(line);
    }

    *num_puzzles = 0;
    while ((read = getline(&line, &len, file)) != -1) {
        if (strncmp(line, "Grid", 4) == 0) {
            (*num_puzzles)++;
            *puzzles = realloc(*puzzles, (*num_puzzles) * sizeof(char *));
            (*puzzles)[*num_puzzles - 1] = malloc(SIZE * SIZE * sizeof(char));
            for (int i = 0; i < SIZE; i++) {
                getline(&line, &len, file);
                for (int j = 0; j < SIZE; j++) {
                    (*puzzles)[*num_puzzles - 1][i * SIZE + j] = line[j * 2];
                }
            }
        }
    }

    free(line);
    fclose(file);
}

void write_sdm_file(const char *filepath, const char *header, char **puzzles, int num_puzzles) {
    FILE *file = fopen(filepath, "w");
    if (!file) {
        perror("Unable to open file");
        exit(EXIT_FAILURE);
    }

    fprintf(file, "%s", header);

    for (int p = 0; p < num_puzzles; p++) {
        fprintf(file, "Grid %d\n", p + 1);
        for (int i = 0; i < SIZE; i++) {
            fwrite(&(puzzles[p][i * SIZE]), sizeof(char), SIZE, file);
            fputc('\n', file);
        }
    }

    fclose(file);
}

int is_valid(int board[SIZE][SIZE], int row, int col, int num) {
    for (int x = 0; x < SIZE; x++) {
        if (board[row][x] == num || board[x][col] == num) {
            return 0;
        }
    }
    int start_row = row / 3 * 3, start_col = col / 3 * 3;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[start_row + i][start_col + j] == num) {
                return 0;
            }
        }
    }
    return 1;
}

int find_empty_location(int board[SIZE][SIZE], int *row, int *col) {
    for (*row = 0; *row < SIZE; (*row)++) {
        for (*col = 0; *col < SIZE; (*col)++) {
            if (board[*row][*col] == 0) {
                return 1;
            }
        }
    }
    return 0;
}

int solve_sudoku(int board[SIZE][SIZE]) {
    int row, col;
    if (!find_empty_location(board, &row, &col)) {
        return 1;
    }
    for (int num = 1; num <= SIZE; num++) {
        if (is_valid(board, row, col, num)) {
            board[row][col] = num;
            if (solve_sudoku(board)) {
                return 1;
            }
            board[row][col] = 0;
        }
    }
    return 0;
}

void parse_puzzle(char *raw_puzzle, int board[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            board[i][j] = raw_puzzle[i * SIZE + j] == '.' ? 0 : raw_puzzle[i * SIZE + j] - '0';
        }
    }
}

void unparse_puzzle(int board[SIZE][SIZE], char *raw_puzzle) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            raw_puzzle[i * SIZE + j] = board[i][j] == 0 ? '.' : board[i][j] + '0';
        }
    }
}

void solve_puzzles(char **puzzles, int num_puzzles) {
    for (int p = 0; p < num_puzzles; p++) {
        int board[SIZE][SIZE];
        parse_puzzle(puzzles[p], board);
        solve_sudoku(board);
        unparse_puzzle(board, puzzles[p]);
    }
}

int main() {
    const char *input_filepath = "input.sdm";
    const char *output_filepath = "output.sdm";

    char *header = NULL;
    char **puzzles = NULL;
    int num_puzzles = 0;

    read_sdm_file(input_filepath, &header, &puzzles, &num_puzzles);
    solve_puzzles(puzzles, num_puzzles);
    write_sdm_file(output_filepath, header, puzzles, num_puzzles);

    free(header);
    for (int i = 0; i < num_puzzles; i++) {
        free(puzzles[i]);
    }
    free(puzzles);

    return 0;
}
