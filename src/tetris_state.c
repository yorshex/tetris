#include <raylib.h>
#include <stdint.h>

#define TETRIS_PIECE_HEIGHT 4
#define TETRIS_PIECE_WIDTH 4
#define TETRIS_PIECE_AREA (TETRIS_PIECE_HEIGHT * TETRIS_PIECE_WIDTH)
#define TETRIS_PIECE_POSSIBLE_ROTATIONS 4
#define TETRIS_JAR_HEIGHT 20
#define TETRIS_JAR_WIDTH 10
#define TETRIS_JAR_AREA (TETRIS_JAR_HEIGHT * TETRIS_JAR_WIDTH)

typedef enum {
    TETRIS_PIECE_FIRST = 0,
    TETRIS_PIECE_O = 0,
    TETRIS_PIECE_I,
    TETRIS_PIECE_T,
    TETRIS_PIECE_L,
    TETRIS_PIECE_J,
    TETRIS_PIECE_S,
    TETRIS_PIECE_Z,
    TETRIS_PIECE_LAST,
} TetrisPieceType;

typedef enum {
    TETRIS_CELL_BLANK = TETRIS_PIECE_FIRST - 1,
    TETRIS_CELL_FIRST = TETRIS_PIECE_FIRST,
    TETRIS_CELL_O = TETRIS_PIECE_O,
    TETRIS_CELL_I = TETRIS_PIECE_I,
    TETRIS_CELL_T = TETRIS_PIECE_T,
    TETRIS_CELL_L = TETRIS_PIECE_L,
    TETRIS_CELL_J = TETRIS_PIECE_J,
    TETRIS_CELL_S = TETRIS_PIECE_S,
    TETRIS_CELL_Z = TETRIS_PIECE_Z,
    TETRIS_CELL_LAST = TETRIS_PIECE_LAST,
    TETRIS_CELL_OUT_OF_BOUNDS = TETRIS_PIECE_LAST + 1,
} TetrisCellType;

typedef enum {
    TETRIS_COLOR_CELL_FIRST = TETRIS_PIECE_FIRST,
    TETRIS_COLOR_CELL_O = TETRIS_PIECE_O,
    TETRIS_COLOR_CELL_I = TETRIS_PIECE_I,
    TETRIS_COLOR_CELL_T = TETRIS_PIECE_T,
    TETRIS_COLOR_CELL_L = TETRIS_PIECE_L,
    TETRIS_COLOR_CELL_J = TETRIS_PIECE_J,
    TETRIS_COLOR_CELL_S = TETRIS_PIECE_S,
    TETRIS_COLOR_CELL_Z = TETRIS_PIECE_Z,
    TETRIS_COLOR_CELL_LAST = TETRIS_PIECE_LAST,
} TetrisColorIndex;

typedef enum {
    TETRIS_CW,
    TETRIS_CCW,
    TETRIS_POSSIBLE_ROTATION_DIRECTIONS,
} TetrisRotationDirection;

#define WALL_KICK_TESTS_COUNT 5

typedef int8_t TetrisPieceShape[TETRIS_PIECE_AREA];
typedef TetrisPieceShape TetrisPieceShapesData[TETRIS_PIECE_POSSIBLE_ROTATIONS];
typedef int8_t TetrisPieceWallKicks[2][WALL_KICK_TESTS_COUNT][TETRIS_POSSIBLE_ROTATION_DIRECTIONS];
typedef TetrisPieceWallKicks TetrisPieceWallKicksData[TETRIS_PIECE_POSSIBLE_ROTATIONS];

typedef struct {
    const TetrisPieceShapesData *shape;
    const TetrisPieceWallKicksData *kicks;
    TetrisColorIndex colorIndex;
} TetrisPieceTypeData;

typedef struct {
    TetrisCellType type;
} TetrisCell;

typedef struct {
    TetrisPieceType type;
    int32_t posX;
    int32_t posY;
    int32_t rotation;
} TetrisFallingPiece;

typedef struct {
    TetrisCell cells[TETRIS_JAR_AREA * 2];
    TetrisFallingPiece fallingPiece;
} TetrisJar;

typedef struct {
    int32_t frame;
    int32_t frameLand;
    int32_t frameLock;
    int32_t frameSpawn;
    int32_t frameFall;
    int32_t frameMove;
    int32_t frameRepeat;
} TetrisGameStats;

typedef struct {
    int32_t delaySpawn; // spawn delay - jp. ARE
    int32_t delaySpawnClear; // ARE after filling a line
    int32_t delayLock; // lock delay
    int32_t delayRepeat; // auto repeat rate - ARR
    int32_t delayRepeatFirst; // delayed auto shift - DAS
    int32_t delayFall; // how much frames it takes for a piece to fall down for one cell
    int32_t factorSoftDrop; // soft drop factor - SDF
} TetrisGameParams;

typedef struct {
    TetrisGameStats stats;
    TetrisGameParams params;
    TetrisJar jar;
} TetrisGameState;

static const Color tetris_colors[TETRIS_PIECE_LAST] = {
    [TETRIS_PIECE_O] = YELLOW,
    [TETRIS_PIECE_I] = SKYBLUE,
    [TETRIS_PIECE_T] = PURPLE,
    [TETRIS_PIECE_L] = ORANGE,
    [TETRIS_PIECE_J] = BLUE,
    [TETRIS_PIECE_S] = GREEN,
    [TETRIS_PIECE_Z] = RED,
};

static const TetrisPieceWallKicksData tetris_piece_wall_kicks_datas[2] = {
    { // regular
        { // 0
            { { 0, 0}, {-1, 0}, {-1,+1}, { 0,-2}, {-1,-2}, }, // CW, to 1
            { { 0, 0}, {+1, 0}, {+1,+1}, { 0,-2}, {+1,-2}, }, // CCW, to 3
        },
        { // 1
            { { 0, 0}, {+1, 0}, {+1,-1}, { 0,+2}, {+1,+2}, }, // CW, to 2
            { { 0, 0}, {+1, 0}, {+1,-1}, { 0,+2}, {+1,+2}, }, // CCW, to 0
        },
        { // 2
            { { 0, 0}, {+1, 0}, {+1,+1}, { 0,-2}, {+1,-2}, }, // CW, to 3
            { { 0, 0}, {-1, 0}, {-1,+1}, { 0,-2}, {-1,-2}, }, // CCW, to 1
        },
        { // 3
            { { 0, 0}, {-1, 0}, {-1,-1}, { 0,+2}, {-1,+2}, }, // CW, to 0
            { { 0, 0}, {-1, 0}, {-1,-1}, { 0,+2}, {-1,+2}, }, // CCW, to 2
        },
    },
    { // I-specific
        {
            { { 0, 0}, {-2, 0}, {+1, 0}, {-2,-1}, {+1,+2}, }, // CW, to 1
            { { 0, 0}, {-1, 0}, {+2, 0}, {-1,+2}, {+2,-1}, }, // CCW, to 3
        },
        {
            { { 0, 0}, {-1, 0}, {+2, 0}, {-1,+2}, {+2,-1}, }, // CW, to 2
            { { 0, 0}, {+2, 0}, {-1, 0}, {+2,+1}, {-1,-2}, }, // CCW, to 0
        },
        {
            { { 0, 0}, {+2, 0}, {-1, 0}, {+2,+1}, {-1,-2}, }, // CW, to 3
            { { 0, 0}, {+1, 0}, {-2, 0}, {+1,-2}, {-2,+1}, }, // CCW, to 1
        },
        {
            { { 0, 0}, {+1, 0}, {-2, 0}, {+1,-2}, {-2,+1}, }, // CW, to 0
            { { 0, 0}, {-2, 0}, {+1, 0}, {-2,-1}, {+1,+2}, }, // CCW, to 2
        },
    },
};

static const TetrisPieceShapesData tetris_piece_shapes_datas[TETRIS_PIECE_LAST] = {
    [ TETRIS_PIECE_O ] = {
        {
            0, 0, 0, 0,
            0, 1, 1, 0,
            0, 1, 1, 0,
            0, 0, 0, 0,
        },
        {
            0, 0, 0, 0,
            0, 1, 1, 0,
            0, 1, 1, 0,
            0, 0, 0, 0,
        },
        {
            0, 0, 0, 0,
            0, 1, 1, 0,
            0, 1, 1, 0,
            0, 0, 0, 0,
        },
        {
            0, 0, 0, 0,
            0, 1, 1, 0,
            0, 1, 1, 0,
            0, 0, 0, 0,
        },
    },
    [ TETRIS_PIECE_I ] = {
        {
            0, 0, 0, 0,
            1, 1, 1, 1,
            0, 0, 0, 0,
            0, 0, 0, 0,
        },
        {
            0, 0, 1, 0,
            0, 0, 1, 0,
            0, 0, 1, 0,
            0, 0, 1, 0,
        },
        {
            0, 0, 0, 0,
            0, 0, 0, 0,
            1, 1, 1, 1,
            0, 0, 0, 0,
        },
        {
            0, 1, 0, 0,
            0, 1, 0, 0,
            0, 1, 0, 0,
            0, 1, 0, 0,
        },
    },
    [ TETRIS_PIECE_T ] = {
        {
            0, 0, 0, 0,
            0, 1, 0, 0,
            1, 1, 1, 0,
            0, 0, 0, 0,
        },
        {
            0, 0, 0, 0,
            0, 1, 0, 0,
            0, 1, 1, 0,
            0, 1, 0, 0,
        },
        {
            0, 0, 0, 0,
            0, 0, 0, 0,
            1, 1, 1, 0,
            0, 1, 0, 0,
        },
        {
            0, 0, 0, 0,
            0, 1, 0, 0,
            1, 1, 0, 0,
            0, 1, 0, 0,
        },
    },
    [ TETRIS_PIECE_L ] = {
        {
            0, 0, 0, 0,
            0, 0, 1, 0,
            1, 1, 1, 0,
            0, 0, 0, 0,
        },
        {
            0, 0, 0, 0,
            0, 1, 0, 0,
            0, 1, 0, 0,
            0, 1, 1, 0,
        },
        {
            0, 0, 0, 0,
            0, 0, 0, 0,
            1, 1, 1, 0,
            1, 0, 0, 0,
        },
        {
            0, 0, 0, 0,
            1, 1, 0, 0,
            0, 1, 0, 0,
            0, 1, 0, 0,
        },
    },
    [ TETRIS_PIECE_J ] = {
        {
            0, 0, 0, 0,
            1, 0, 0, 0,
            1, 1, 1, 0,
            0, 0, 0, 0,
        },
        {
            0, 0, 0, 0,
            0, 1, 1, 0,
            0, 1, 0, 0,
            0, 1, 0, 0,
        },
        {
            0, 0, 0, 0,
            0, 0, 0, 0,
            1, 1, 1, 0,
            0, 0, 1, 0,
        },
        {
            0, 0, 0, 0,
            0, 1, 0, 0,
            0, 1, 0, 0,
            1, 1, 0, 0,
        },
    },
    [ TETRIS_PIECE_S ] = {
        {
            0, 0, 0, 0,
            0, 1, 1, 0,
            1, 1, 0, 0,
            0, 0, 0, 0,
        },
        {
            0, 0, 0, 0,
            0, 1, 0, 0,
            0, 1, 1, 0,
            0, 0, 1, 0,
        },
        {
            0, 0, 0, 0,
            0, 0, 0, 0,
            0, 1, 1, 0,
            1, 1, 0, 0,
        },
        {
            0, 0, 0, 0,
            1, 0, 0, 0,
            1, 1, 0, 0,
            0, 1, 0, 0,
        },
    },
    [ TETRIS_PIECE_Z ] = {
        {
            0, 0, 0, 0,
            1, 1, 0, 0,
            0, 1, 1, 0,
            0, 0, 0, 0,
        },
        {
            0, 0, 0, 0,
            0, 0, 1, 0,
            0, 1, 1, 0,
            0, 1, 0, 0,
        },
        {
            0, 0, 0, 0,
            0, 0, 0, 0,
            1, 1, 0, 0,
            0, 1, 1, 0,
        },
        {
            0, 0, 0, 0,
            0, 1, 0, 0,
            1, 1, 0, 0,
            1, 0, 0, 0,
        },
    },
};

static const TetrisPieceTypeData tetris_piece_type_datas[TETRIS_PIECE_LAST] = {
    [ TETRIS_PIECE_O ] = {
        &tetris_piece_shapes_datas[TETRIS_PIECE_O],
        &tetris_piece_wall_kicks_datas[0],
        TETRIS_COLOR_CELL_O,
    },
    [ TETRIS_PIECE_I ] = {
        &tetris_piece_shapes_datas[TETRIS_PIECE_I],
        &tetris_piece_wall_kicks_datas[1],
        TETRIS_COLOR_CELL_I,
    },
    [ TETRIS_PIECE_T ] = {
        &tetris_piece_shapes_datas[TETRIS_PIECE_T],
        &tetris_piece_wall_kicks_datas[0],
        TETRIS_COLOR_CELL_T,
    },
    [ TETRIS_PIECE_L ] = {
        &tetris_piece_shapes_datas[TETRIS_PIECE_L],
        &tetris_piece_wall_kicks_datas[0],
        TETRIS_COLOR_CELL_L,
    },
    [ TETRIS_PIECE_J ] = {
        &tetris_piece_shapes_datas[TETRIS_PIECE_J],
        &tetris_piece_wall_kicks_datas[0],
        TETRIS_COLOR_CELL_J,
    },
    [ TETRIS_PIECE_S ] = {
        &tetris_piece_shapes_datas[TETRIS_PIECE_S],
        &tetris_piece_wall_kicks_datas[0],
        TETRIS_COLOR_CELL_S,
    },
    [ TETRIS_PIECE_Z ] = {
        &tetris_piece_shapes_datas[TETRIS_PIECE_Z],
        &tetris_piece_wall_kicks_datas[0],
        TETRIS_COLOR_CELL_Z,
    },
};
