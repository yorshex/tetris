#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <raylib.h>

#define WIDTH 800
#define HEIGHT 450
#define JAR_WIDTH 10
#define JAR_HEIGHT 20

size_t tetris_dump;

typedef enum {
    TETRIS_CELL_OUT_OF_BOUNDS = -1,
    TETRIS_CELL_BLANK = 0,
    TETRIS_CELL_S,
    TETRIS_CELL_Z,
    TETRIS_CELL_L,
    TETRIS_CELL_J,
    TETRIS_CELL_I,
    TETRIS_CELL_O,
    TETRIS_CELL_LAST,
} TetrisCellType;

typedef enum {
    TETRIS_PIECE_S,
    TETRIS_PIECE_Z,
    TETRIS_PIECE_L,
    TETRIS_PIECE_J,
    TETRIS_PIECE_I,
    TETRIS_PIECE_O,
    TETRIS_PIECE_LAST,
} TetrisPieceType;

typedef char TetrisPiece[16];

typedef enum {
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
} TetrisColorIndex;

static const Color tetris_colors[TETRIS_COLORSCHEME_LAST] = {
    [ TETRIS_COLORSCHEME_BG ] =             RAYWHITE,
    [ TETRIS_COLORSCHEME_TEXT_PRIMARY ] =   GRAY,
    [ TETRIS_COLORSCHEME_TEXT_SECONDARY ] = DARKGRAY,
    [ TETRIS_COLORSCHEME_JAR_GRID ] =       GRAY,
    [ TETRIS_COLORSCHEME_CELL_BLANK ] =     LIGHTGRAY,
    [ TETRIS_COLORSCHEME_CELL_S ] =         GREEN,
    [ TETRIS_COLORSCHEME_CELL_Z ] =         RED,
    [ TETRIS_COLORSCHEME_CELL_L ] =         ORANGE,
    [ TETRIS_COLORSCHEME_CELL_J ] =         BLUE,
    [ TETRIS_COLORSCHEME_CELL_I ] =         SKYBLUE,
    [ TETRIS_COLORSCHEME_CELL_O ] =         YELLOW,
};

typedef struct {
    int posX;
    int posY;
    TetrisPieceType piece;
    int rotation;
} TetrisFalling;

typedef struct {
    int height;
    int width;
    TetrisFalling falling;
    TetrisCellType *cells;
} TetrisJar;

typedef struct {
    int frame;
    int lastLandFrame;
    int lastMoveButtonFrame; // last frame a move button (right/left) was pressed
    int lastRepeatFrame; // last frame an auto shift was made
} TetrisGameStats;

typedef struct {
    int spawnDelay; // are
    int lockDelay; // lock delay
    // these 3 can be treated as preferences
    int repeatDelay; // das
    int repeatSpeed; // arr
    int softDropFactor; // sdf
} TetrisGameParams;

typedef struct {
    TetrisJar jar;
    TetrisGameStats stats;
    TetrisGameParams params;
} TetrisGame;

#define TETRIS_JAR_AT(jar, x, y) \
    ((x) < 0 || (x) >= (jar.width) || (y) >= (jar).height || (y) < -(jar).height ? \
     TETRIS_CELL_OUT_OF_BOUNDS : \
     (jar).cells[(x) + ((y)+(jar).height) * (jar).width])

inline void TetrisInitGame(TetrisGame *, int jar_height, int jar_width);
void TetrisDrawGameJar(TetrisGame, int posX, int posY, int width, int height);

TetrisGame tetris_game;

int main(int argc, char **argv)
{
    InitWindow(WIDTH, HEIGHT, "TETЯIS");

    ClearBackground(tetris_colors[TETRIS_COLORSCHEME_BG]);

    TetrisInitGame(&tetris_game, JAR_HEIGHT, JAR_WIDTH);

    while (!WindowShouldClose()) {
        BeginDrawing();
        TetrisDrawGameJar(tetris_game, 325, 75, 150, 300);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}

extern inline void TetrisInitGame(TetrisGame *game, int jar_height, int jar_width) {
    game->jar.height = jar_height;
    game->jar.width = jar_width;
    if (game->jar.cells != NULL) free(game->jar.cells);
    game->jar.cells = calloc(2 * jar_height * jar_width, sizeof(TetrisCellType));
    // there are 2 time more cells to allow overflowing
}

void TetrisDrawGameJar(TetrisGame game, int posX, int posY, int width, int height) {
#define jar game.jar
    float cellX = (float)width / jar.width;
    float cellY = (float)height / jar.height;

    DrawRectangle(posX, posY, width, height, tetris_colors[TETRIS_COLORSCHEME_CELL_BLANK]);

    for (int y = -2; y < JAR_HEIGHT; y++) {
        for (int x = 0; x < JAR_WIDTH; x++) {
            if (TETRIS_JAR_AT(jar, x, y) == TETRIS_CELL_BLANK) continue;
            DrawRectangle(
                posX + cellX * x,
                posY + cellY * y,
                cellX,
                cellY,
                tetris_colors[TETRIS_COLORSCHEME_CELL_BLANK + TETRIS_JAR_AT(jar, x, y)] );
        }
    }
#undef jar
}
