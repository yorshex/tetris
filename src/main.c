#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>

#define WIDTH 800
#define HEIGHT 450
#define CELL_SIZE 20
#define JAR_WIDTH 10
#define JAR_HEIGHT 20

typedef enum {
    TETRIS_CELL_BLANK = 0,
    TETRIS_CELL_S,
    TETRIS_CELL_Z,
    TETRIS_CELL_L,
    TETRIS_CELL_J,
    TETRIS_CELL_I,
    TETRIS_CELL_O,
    TETRIS_CELL_LAST,
} TetrisCell;

enum {
    TETRIS_COLORSCHEME_BG,
    TETRIS_COLORSCHEME_TEXT_PRIMARY,
    TETRIS_COLORSCHEME_TEXT_SECONDARY,
    TETRIS_COLORSCHEME_JAR_BG,
    TETRIS_COLORSCHEME_JAR_GRID,
    TETRIS_COLORSCHEME_CELL_BLANK,
    TETRIS_COLORSCHEME_CELL_S,
    TETRIS_COLORSCHEME_CELL_Z,
    TETRIS_COLORSCHEME_CELL_L,
    TETRIS_COLORSCHEME_CELL_J,
    TETRIS_COLORSCHEME_CELL_I,
    TETRIS_COLORSCHEME_CELL_O,
    TETRIS_COLORSCHEME_LAST,
};

static const Color tetris_colorscheme[TETRIS_COLORSCHEME_LAST] = {
    [ TETRIS_COLORSCHEME_BG ] =             RAYWHITE,
    [ TETRIS_COLORSCHEME_TEXT_PRIMARY ] =   GRAY,
    [ TETRIS_COLORSCHEME_TEXT_SECONDARY ] = DARKGRAY,
    [ TETRIS_COLORSCHEME_JAR_GRID ] =       GRAY,
    [ TETRIS_COLORSCHEME_CELL_BLANK ] =     LIGHTGRAY,
    [ TETRIS_COLORSCHEME_CELL_S ] =         RED,
    [ TETRIS_COLORSCHEME_CELL_Z ] =         GREEN,
    [ TETRIS_COLORSCHEME_CELL_L ] =         ORANGE,
    [ TETRIS_COLORSCHEME_CELL_J ] =         DARKBLUE,
    [ TETRIS_COLORSCHEME_CELL_I ] =         BLUE,
    [ TETRIS_COLORSCHEME_CELL_O ] =         YELLOW,
};

typedef struct {
    int height;
    int width;
    TetrisCell *cells;
} TetrisJar;

inline void TetrisInitJar(TetrisJar *, int height, int width);
#define TETRIS_JAR_CELL(jar, x, y) (jar.cells[x + (y+jar.height) * jar.width])

TetrisJar tetris_jar;

int main(int argc, char **argv) 
{
    InitWindow(WIDTH, HEIGHT, "TETЯIS");

    ClearBackground(tetris_colorscheme[TETRIS_COLORSCHEME_BG]);

    TetrisInitJar(&tetris_jar, JAR_HEIGHT, JAR_WIDTH);
    TETRIS_JAR_CELL(tetris_jar, 1, 1) = TETRIS_CELL_S;

    while (!WindowShouldClose()) {
        BeginDrawing();
        for (int y = -2; y < JAR_HEIGHT; y++) {
            for (int x = 0; x < JAR_WIDTH; x++) {
                DrawRectangle(CELL_SIZE * x, CELL_SIZE * y, CELL_SIZE, CELL_SIZE,
                    tetris_colorscheme[TETRIS_COLORSCHEME_CELL_BLANK + TETRIS_JAR_CELL(tetris_jar, x, y)]);
            }
        }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}

extern inline void TetrisInitJar(TetrisJar *jar, int height, int width) {
    jar->height = height;
    jar->width = width;
    if (jar->cells != NULL) free(jar->cells);
    jar->cells = calloc(2 * height * width, sizeof(TetrisCell));
    // there are 2 time more cells to allow overflowing
}
