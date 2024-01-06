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
    TETRIS_COLOR_HOLD_UNAVAILABLE,
    TETRIS_COLOR_TEXT_PRIMARY,
    TETRIS_COLOR_TEXT_SECONDARY,
    TETRIS_COLOR_LAST,
} TetrisColorIndex;



const Color tetris_colors[TETRIS_COLOR_LAST] = {
    [TETRIS_COLOR_CELL_O] = YELLOW,
    [TETRIS_COLOR_CELL_I] = SKYBLUE,
    [TETRIS_COLOR_CELL_T] = PURPLE,
    [TETRIS_COLOR_CELL_L] = ORANGE,
    [TETRIS_COLOR_CELL_J] = BLUE,
    [TETRIS_COLOR_CELL_S] = GREEN,
    [TETRIS_COLOR_CELL_Z] = RED,
    [TETRIS_COLOR_JAR_BG] = LIGHTGRAY,
    [TETRIS_COLOR_HOLD_UNAVAILABLE] = GRAY,
    [TETRIS_COLOR_TEXT_PRIMARY] = GRAY,
    [TETRIS_COLOR_TEXT_SECONDARY] = DARKGRAY,
};



typedef enum {
    TETRIS_PARTICLE_CELL_SHARD,
} TetrisParticleBehavior;

typedef int32_t TetrisParticleParams[8];

typedef struct {
    TetrisParticleBehavior bh;
    TetrisParticleParams param;
} TetrisParticleData;

typedef struct {
    ssize_t op;
    ssize_t on;
    TetrisParticleData p;
} TetrisParticle;

#define TETRIS_MAX_PARTICLES 1024

ssize_t tetris_particles_op_first;
TetrisParticle tetris_particles[TETRIS_MAX_PARTICLES];

void TetrisAddParticle(TetrisParticleData p)
{
    if (tetris_particles_op_first > 0) {
        tetris_particles[0].data = p;
    }
}

void TetrisCreateParticle(TetrisParticleBehavior bh, ...)
{
}


void TetrisDrawPieceShape(TetrisPieceType type, int rotation, int posX, int posY, int width, int height, Color color)
{
    float cellX = (float)width / TETRIS_PIECE_WIDTH;
    float cellY = (float)height / TETRIS_PIECE_HEIGHT;

    for (int y = 0; y < TETRIS_PIECE_HEIGHT; y++) {
        for (int x = 0; x < TETRIS_PIECE_HEIGHT; x++)
        {
            if (TETRIS_PIECE_SHAPE_AT(tetris_piece_shapes_datas[type][repeat(rotation, TETRIS_PIECE_POSSIBLE_ROTATIONS)], x, y) == 0)
                continue;

            DrawRectangle(
                posX + cellX * x,
                posY + cellY * y,
                ceilf(cellX),
                ceilf(cellY),
                color
            );
        }
    }
}



void TetrisDrawPieceShapeLines(TetrisPieceType type, int rotation, int posX, int posY, int width, int height, int lineThick, Color color)
{
    float cellX = (float)width / TETRIS_PIECE_WIDTH;
    float cellY = (float)height / TETRIS_PIECE_HEIGHT;

    for (int y = 0; y < TETRIS_PIECE_HEIGHT; y++) {
        for (int x = 0; x < TETRIS_PIECE_HEIGHT; x++)
        {
            if (TETRIS_PIECE_SHAPE_AT(tetris_piece_shapes_datas[type][repeat(rotation, TETRIS_PIECE_POSSIBLE_ROTATIONS)], x, y) == 0)
                continue;

            DrawRectangleLinesEx(
                (Rectangle){
                    posX + cellX * x,
                    posY + cellY * y,
                    ceilf(cellX),
                    ceilf(cellY),
                },
                lineThick,
                color
            );
        }
    }
}



void TetrisDrawHoldPiece(TetrisGameState *s, int posX, int posY, int width, int height) {
    TetrisPieceType type = s->holdPieceType;
    Color color = s->fallPiece.held ? tetris_colors[TETRIS_COLOR_HOLD_UNAVAILABLE] : tetris_colors[TETRIS_COLOR_CELL_FIRST + type];

    TetrisDrawPieceShape(type, 0, posX, posY, width, height, color);
}



void TetrisDrawGameJar(const TetrisGameState *s, int posX, int posY, int width, int height)
{
    float cellX = (float)width / TETRIS_JAR_WIDTH;
    float cellY = (float)height / TETRIS_JAR_HEIGHT;

    DrawRectangle(
        posX,
        posY,
        width,
        height,
        tetris_colors[TETRIS_COLOR_JAR_BG]
    );

    for (int y = -TETRIS_JAR_HEIGHT; y < TETRIS_JAR_HEIGHT; y++)
    {
        for (int x = 0; x < TETRIS_JAR_WIDTH; x++)
        {
            if (TETRIS_JAR_AT(*s, x, y).type <= -1 || (s->maskLinesFull[y+TETRIS_JAR_HEIGHT] && s->fallPiece.locked))
                continue;

            DrawRectangle(
                posX + cellX * x,
                posY + cellY * y,
                ceilf(cellX),
                ceilf(cellY),
                ColorBrightness(tetris_colors[TETRIS_COLOR_CELL_FIRST + TETRIS_JAR_AT(*s, x, y).type], -0.25)
            );
        }
    }

    #define piece (s->fallPiece)
    if (piece.locked) return;

    int lineThick = 1;
    int lineThickShadow = 2;

    int shadowPosY = 0;

    while (!TetrisCheckCollisionDisplace(s, 0, 1+shadowPosY, 0))
            shadowPosY += 1;

    if (shadowPosY > 0)
        TetrisDrawPieceShapeLines(
            piece.type,
            piece.rotation,
            posX + cellX * (piece.posX),
            posY + cellY * (piece.posY+shadowPosY),
            cellX * 4,
            cellY * 4,
            lineThickShadow,
            ColorAlpha(ColorBrightness(tetris_colors[TETRIS_COLOR_CELL_FIRST + piece.type], -0.75), 0.3)
        );

    if (s->frame - max(s->frameMovement, s->frameSpawn) <= 6)
        for (int y = 0; y < TETRIS_PIECE_HEIGHT; y++) {
            for (int x = 0; x < TETRIS_PIECE_WIDTH; x++) {
                if (TETRIS_PIECE_SHAPE_AT(tetris_piece_shapes_datas[piece.type][piece.rotation], x, y) == 0)
                    continue;

                DrawRectangleLinesEx(
                    (Rectangle) {
                        posX + cellX * (x + piece.posX) - lineThick,
                        posY + cellY * (y + piece.posY) - lineThick,
                        ceilf(cellX) + lineThick * 2,
                        ceilf(cellY) + lineThick * 2,
                    },
                    lineThick,
                    WHITE
                );
            }
        }

    TetrisDrawPieceShape(
        piece.type,
        piece.rotation,
        posX + cellX * (piece.posX),
        posY + cellY * (piece.posY),
        cellX * 4,
        cellY * 4,
        tetris_colors[TETRIS_COLOR_CELL_FIRST + piece.type]
    );
    #undef piece
}

void TetrisDrawGame(TetrisGameState *s)
{
    ClearBackground(WHITE);

    // UI

    if (s->holdPieceType != (TetrisPieceType)TETRIS_PIECE_FIRST - 1) {
        DrawText("Hold", 116, 28, 10, tetris_colors[TETRIS_COLOR_TEXT_PRIMARY]);
        TetrisDrawHoldPiece(s, 116, 32, 32, 32);
    }

    DrawText("Level", 116, 148, 10, tetris_colors[TETRIS_COLOR_TEXT_PRIMARY]);
    DrawText(TextFormat("%i", s->level), 116, 158, 10, tetris_colors[TETRIS_COLOR_TEXT_SECONDARY]);
    DrawText("Score", 116, 168, 10, tetris_colors[TETRIS_COLOR_TEXT_PRIMARY]);
    DrawText(TextFormat("%i", s->score), 116, 178, 10, tetris_colors[TETRIS_COLOR_TEXT_SECONDARY]);

    DrawText("Next", 232, 28, 10, tetris_colors[TETRIS_COLOR_TEXT_PRIMARY]);
    for (int i = 0; i < 5; i++)
        TetrisDrawPieceShape(
            s->bag.values[repeat(s->bag.index+i, TETRIS_BAG_SIZE)], 0,
            236, 32+i*24, 32, 32,
            tetris_colors[TETRIS_COLOR_CELL_FIRST + s->bag.values[repeat(s->bag.index+i, TETRIS_BAG_SIZE)]]
        );

    DrawText("Time", 236, 168, 10, tetris_colors[TETRIS_COLOR_TEXT_PRIMARY]);
    DrawText(TextFormat("%im%is %if", s->frame/60/60, s->frame/60%60, s->frame%60),
            236, 178, 10, tetris_colors[TETRIS_COLOR_TEXT_SECONDARY]);

    // BOARD
    TetrisDrawGameJar(s, 152, 28, 80, 160);
}
