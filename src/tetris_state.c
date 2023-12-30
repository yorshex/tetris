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

#define TETRIS_BAG_HALF_SIZE (TETRIS_PIECE_LAST - TETRIS_PIECE_FIRST)
#define TETRIS_BAG_SIZE (TETRIS_BAG_HALF_SIZE * 2)

typedef struct {
    size_t index;
    TetrisPieceType values[TETRIS_BAG_SIZE];
} TetrisPieceBag;

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
    TETRIS_COLOR_JAR_BG = TETRIS_COLOR_CELL_LAST,
    TETRIS_COLOR_LAST,
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
    TetrisCellType type;
} TetrisCell;

typedef struct {
    TetrisPieceType type;
    int32_t posX;
    int32_t posY;
    int32_t rotation;
} TetrisFallingPiece;

typedef struct {
    int32_t frame; // current frame
    int32_t frameLand; // the frame when the current piece landed on something
    int32_t frameLock; // the frame when the current piece was locked
    int32_t frameSpawn; // the frame when the current piece was spawned
    int32_t frameFall; // last frame a piece was moved down by gravitation
    int32_t frameMove; // last right/left key press frame
    int32_t frameRepeat; // last auto-repeat frame

    int32_t delaySpawn; // spawn delay - jp. ARE
    int32_t delaySpawnClear; // ARE after filling a line
    int32_t delayLock; // lock delay
    int32_t delayRepeat; // auto repeat rate - ARR
    int32_t delayRepeatFirst; // delayed auto shift - DAS
    int32_t delayFall; // how much frames it takes for a piece to fall down for one cell
    int32_t factorSoftDrop; // soft drop factor - SDF

    bool fallPieceLanded;
    TetrisPieceBag bag;
    TetrisCell board[TETRIS_JAR_AREA * 2];
    TetrisFallingPiece fallPiece;
} TetrisGameState;

#define TETRIS_PIECE_SHAPE_AT(shape, x, y) \
    (shape)[x + TETRIS_PIECE_WIDTH * y]

#define TETRIS_JAR_AT(game, x, y) \
    ( ((x) > TETRIS_JAR_WIDTH || (x) < 0 || (y) >= TETRIS_JAR_HEIGHT || (y) < -TETRIS_JAR_HEIGHT) ? \
      (TetrisCell){.type = TETRIS_CELL_OUT_OF_BOUNDS} : \
      (game).board[x + TETRIS_JAR_WIDTH * (y + TETRIS_JAR_HEIGHT)] )

const Color tetris_colors[TETRIS_COLOR_LAST] = {
    [TETRIS_COLOR_CELL_O] = YELLOW,
    [TETRIS_COLOR_CELL_I] = SKYBLUE,
    [TETRIS_COLOR_CELL_T] = PURPLE,
    [TETRIS_COLOR_CELL_L] = ORANGE,
    [TETRIS_COLOR_CELL_J] = BLUE,
    [TETRIS_COLOR_CELL_S] = GREEN,
    [TETRIS_COLOR_CELL_Z] = RED,
    [TETRIS_COLOR_JAR_BG] = LIGHTGRAY,
};

const TetrisPieceWallKicksData tetris_piece_wall_kicks_datas[2] = {
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

const TetrisPieceShapesData tetris_piece_shapes_datas[TETRIS_PIECE_LAST] = {
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
