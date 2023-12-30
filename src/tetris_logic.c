void TetrisInitBagHalf(TetrisGameState *s, uint32_t half) {
    TetrisPieceType *newValues = (TetrisPieceType *)
        LoadRandomSequence(
                TETRIS_BAG_HALF_SIZE,
                TETRIS_PIECE_FIRST,
                TETRIS_PIECE_LAST - 1);
    memcpy(
            &(s->bag.values[TETRIS_BAG_HALF_SIZE * half]),
            newValues,
            TETRIS_BAG_HALF_SIZE * sizeof(TetrisPieceType));
    UnloadRandomSequence((int *) newValues);
}

void TetrisInitBag(TetrisGameState *s) {
    TetrisInitBagHalf(s, 0);
    TetrisInitBagHalf(s, 1);
}

TetrisPieceType TetrisBagNext(TetrisGameState *s) {
    TetrisPieceType piece = s->bag.values[s->bag.index++];
    s->bag.index %= TETRIS_BAG_SIZE;
    if (s->bag.index % TETRIS_BAG_HALF_SIZE == 0)
        TetrisInitBagHalf(s, ! (s->bag.index >= TETRIS_BAG_HALF_SIZE));
    return piece;
}

void TetrisInitFallPiece(TetrisGameState *s) {
    s->fallPiece.posX = 3;
    s->fallPiece.posY = -4;
    s->fallPiece.type = TetrisBagNext(s);
}

void TetrisGameStateInit(TetrisGameState *s) {
    for (int i = 0; i < TETRIS_JAR_AREA * 2; i++) {
        s->board[i].type = TETRIS_CELL_BLANK;
    }

    s->delayFall = 12;

    TetrisInitBag(s);
    TetrisInitFallPiece(s);
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
    s->fallPieceLanded = collis;
}

//void TetrisInput(TetrisGameState *s, TetrisInput *i);

void TetrisFrame(TetrisGameState *s)
{
    s->frame++;

    if (! s->fallPieceLanded && s->frame - s->frameFall >= s->delayFall)
    {
        s->frameFall = s->frame;
        s->fallPiece.posY += 1;
        TetrisCheckLanded(s);
    }
}
