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
    [TETRIS_COLOR_JAR_BG] = DARKGRAY,
    [TETRIS_COLOR_HOLD_UNAVAILABLE] = GRAY,
    [TETRIS_COLOR_TEXT_PRIMARY] = GRAY,
    [TETRIS_COLOR_TEXT_SECONDARY] = WHITE,
};

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
            if (TETRIS_JAR_AT(*s, x, y).type <= -1)
                continue;

            DrawRectangle(
                posX + cellX * x,
                posY + cellY * y,
                ceilf(cellX),
                ceilf(cellY),
                ColorBrightness(tetris_colors[TETRIS_COLOR_CELL_FIRST + TETRIS_JAR_AT(*s, x, y).type],
                    s->maskLinesFull[y+TETRIS_JAR_HEIGHT] ? 0.2 : -0.15
                )
            );
        }
    }

    #define piece (s->fallPiece)
    if (piece.locked) return;

    int lineThick = (int)roundf(fmin(cellX, cellY) * .2);
    int lineThickShadow = (int)roundf(fmin(cellX, cellY) * .3);

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
            ColorAlpha(ColorBrightness(tetris_colors[TETRIS_COLOR_CELL_FIRST + piece.type], 0.75), 0.3)
        );

    for (int y = 0; y < TETRIS_PIECE_HEIGHT; y++) {
        for (int x = 0; x < TETRIS_PIECE_WIDTH; x++) {
            if (TETRIS_PIECE_SHAPE_AT(tetris_piece_shapes_datas[piece.type][piece.rotation], x, y) == 0)
                continue;

            Color color =
                ColorAlpha(
                    WHITE,
                    fmax((float)(10 - max(s->frame - s->frameMovement, 0)) / 10, 0.2)
                );

            DrawRectangleLinesEx(
                (Rectangle) {
                    posX + cellX * (x + piece.posX) - lineThick,
                    posY + cellY * (y + piece.posY) - lineThick,
                    ceilf(cellX) + lineThick * 2,
                    ceilf(cellY) + lineThick * 2,
                },
                lineThick,
                color
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
