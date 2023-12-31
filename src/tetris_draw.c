void TetrisDrawGameJar(TetrisGameState *s, int posX, int posY, int width, int height)
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
                tetris_colors[TETRIS_COLOR_CELL_FIRST + TETRIS_JAR_AT(*s, x, y).type]
            );
        }
    }

    #define piece (s->fallPiece)
    for (int y = 0; y < TETRIS_PIECE_HEIGHT; y++)
    {
        for (int x = 0; x < TETRIS_PIECE_WIDTH; x++)
        {
            if (TETRIS_PIECE_SHAPE_AT(tetris_piece_shapes_datas[piece.type][piece.rotation], x, y) == 0)
                continue;

            DrawRectangle(
                posX + cellX * (x + piece.posX),
                posY + cellY * (y + piece.posY),
                ceilf(cellX),
                ceilf(cellY),
                tetris_colors[TETRIS_COLOR_CELL_FIRST + piece.type]
            );
        }
    }
    #undef piece
}
