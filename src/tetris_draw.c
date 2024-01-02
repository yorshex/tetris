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
    Color color = ColorBrightness(tetris_colors[TETRIS_COLOR_CELL_FIRST + type], s->fallPiece.held ? -.5 : 0);

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
            ColorAlpha(ColorBrightness(tetris_colors[TETRIS_COLOR_CELL_FIRST + piece.type], -0.75), 0.3)
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
