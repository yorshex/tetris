void TetrisShuffleBagHalf(TetrisGameState *s, int32_t half) {
    TetrisShuffle((int *) s->bag.values + TETRIS_BAG_HALF_SIZE * half, TETRIS_BAG_HALF_SIZE);
}

TetrisPieceType TetrisBagNext(TetrisGameState *s) {
    TetrisPieceType piece = s->bag.values[s->bag.index++];
    s->bag.index %= TETRIS_BAG_SIZE;
    if (s->bag.index % TETRIS_BAG_HALF_SIZE == 0)
        TetrisShuffleBagHalf(s, ! (s->bag.index >= TETRIS_BAG_HALF_SIZE));
    return piece;
}

void TetrisNextFallPiece(TetrisGameState *s) {
    s->fallPiece.posX = 3;
    s->fallPiece.posY = -4;
    s->fallPiece.type = TetrisBagNext(s);
}

void TetrisGameStateInit(TetrisGameState *s) {
    for (int i = 0; i < TETRIS_JAR_AREA * 2; i++) {
        s->board[i].type = TETRIS_CELL_BLANK;
    }

    s->delayFall = 60;
    s->delayLock = 30;

    TetrisNextFallPiece(s);

    for (int i = 0; i < TETRIS_BAG_SIZE; i++)
        s->bag.values[i] = i % TETRIS_BAG_HALF_SIZE;
    TetrisShuffleBagHalf(s, 0);
    TetrisShuffleBagHalf(s, 1);
}

extern inline TetrisPieceShape *TetrisFallPieceShape(TetrisGameState *s) {
    return (signed char (*)[16]) &tetris_piece_shapes_datas[s->fallPiece.type][s->fallPiece.rotation];
}

extern inline bool TetrisCellOccupied(TetrisGameState *s, int posX, int posY) {
    return TETRIS_JAR_AT(*s, posX, posY).type >= 0;
}

// check if the falling piece collides with something if it's displaced like this
bool TetrisCheckCollisionDisplace(TetrisGameState *s, int posX, int posY)
{
    TetrisPieceShape *fallPieceShape = TetrisFallPieceShape(s);

    for (int x = 0; x < TETRIS_PIECE_WIDTH; x++) {
        for (int y = 0; y < TETRIS_PIECE_WIDTH; y++) {
            if (! TETRIS_PIECE_SHAPE_AT(*fallPieceShape, x, y))
                continue;
            if (TetrisCellOccupied(
                    s,
                    s->fallPiece.posX + posX + x,
                    s->fallPiece.posY + posY + y))
            return true;
        }
    }

    return false;
}

void TetrisCheckLanded(TetrisGameState *s)
{
    bool collis = TetrisCheckCollisionDisplace(s, 0, 1);
    if (collis && ! s->fallPieceLanded)
        s->frameLand = s->frame;
    else if (! collis && s->fallPieceLanded)
        s->frameUnland = s->frame;
    s->fallPieceLanded = collis;
}

//void TetrisInput(TetrisGameState *s, TetrisInput *i);

void TetrisFrame(TetrisGameState *s)
{
    s->frame++;

    bool succeededSafeMovementThreshold = s->safeMovementCount > s->safeMovementThreshold;

    if (!s->fallPieceLanded)
    {
        bool succeededFallDelay = s->frame - max(s->frameFall, s->frameUnland) >= s->delayFall;

        if (succeededFallDelay || succeededSafeMovementThreshold) {
            s->frameFall = s->frame;
            s->fallPiece.posY += 1;
            s->safeMovementCount = 0;
            TetrisCheckLanded(s);
        }
    }
    else if (!s->fallPieceLocked)
    {
        bool succeededLockDelay = s->frame - max(s->frameLand, s->frameMovement) >= s->delayLock;

        if (succeededLockDelay || succeededSafeMovementThreshold) {
            s->frameLock = s->frame;
            s->fallPieceLocked = true;
        }
    }
    else
    {
    }
}
