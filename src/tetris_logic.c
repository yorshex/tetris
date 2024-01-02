void TetrisShuffleBagHalf(TetrisGameState *s, int32_t half) {
    TetrisShuffle((int *) s->bag.values + TETRIS_BAG_HALF_SIZE * half, TETRIS_BAG_HALF_SIZE);
}

TetrisPieceType TetrisBagNext(TetrisGameState *s) {
    TetrisPieceType piece = s->bag.values[s->bag.index++];
    s->bag.index %= TETRIS_BAG_SIZE;
    if (s->bag.index % TETRIS_BAG_HALF_SIZE == 0)
        TetrisShuffleBagHalf(s, !(s->bag.index >= TETRIS_BAG_HALF_SIZE));
    return piece;
}

void TetrisNextFallPiece(TetrisGameState *s) {
    s->fallPiece.posX = 3;
    s->fallPiece.posY = -4;
    s->fallPiece.maxY = -4;
    s->fallPiece.locked = false;
    s->fallPiece.landed = false;
    s->fallPiece.rotation = 0;
    s->fallPiece.type = TetrisBagNext(s);
}

void TetrisGameStateInit(TetrisGameState *s) {
    for (int i = 0; i < TETRIS_JAR_AREA * 2; i++) {
        s->board[i].type = TETRIS_CELL_BLANK;
    }

    for (int i = 0; i < TETRIS_BAG_SIZE; i++)
        s->bag.values[i] = i % TETRIS_BAG_HALF_SIZE;
    TetrisShuffleBagHalf(s, 0);
    TetrisShuffleBagHalf(s, 1);

    TetrisNextFallPiece(s);
}

// get the shape of the falling piece
extern inline TetrisPieceShape *TetrisFallPieceShape(TetrisGameState *s, int rotate) {
    return (TetrisPieceShape *) &tetris_piece_shapes_datas[s->fallPiece.type][repeat(TETRIS_PIECE_POSSIBLE_ROTATIONS, s->fallPiece.rotation + rotate)];
}

// check whether a cell is occupied
extern inline bool TetrisCellOccupied(TetrisGameState *s, int posX, int posY) {
    return TETRIS_JAR_AT(*s, posX, posY).type >= 0;
}

// check if the falling piece overlaps with something if it's moved and/or rotated
bool TetrisCheckCollisionDisplace(TetrisGameState *s, int displaceX, int displaceY, int rotate)
{
    TetrisPieceShape *fallPieceShape = TetrisFallPieceShape(s, rotate);

    for (int x = 0; x < TETRIS_PIECE_WIDTH; x++) {
        for (int y = 0; y < TETRIS_PIECE_WIDTH; y++) {
            if (!TETRIS_PIECE_SHAPE_AT(*fallPieceShape, x, y))
                continue;
            if (TetrisCellOccupied(
                    s,
                    s->fallPiece.posX + displaceX + x,
                    s->fallPiece.posY + displaceY + y))
            return true;
        }
    }

    return false;
}

void TetrisCheckLanded(TetrisGameState *s)
{
    bool collis = TetrisCheckCollisionDisplace(s, 0, 1, 0);
    if (collis && !s->fallPiece.landed)
        s->frameLand = s->frame;
    else if (!collis && s->fallPiece.landed)
        s->frameUnland = s->frame;
    s->fallPiece.landed = collis;
}

void TetrisFallPiecePlace(TetrisGameState *s)
{
    TetrisPieceShape *fallPieceShape = TetrisFallPieceShape(s, 0);

    for (int y = 0; y < TETRIS_PIECE_HEIGHT; y++) {
        for (int x = 0; x < TETRIS_PIECE_WIDTH; x++) {
            if (!TETRIS_PIECE_SHAPE_AT(*fallPieceShape, x, y))
                continue;
            int posX = s->fallPiece.posX + x;
            int posY = s->fallPiece.posY + y;
            TetrisCellType type = (TetrisCellType)s->fallPiece.type;
            TETRIS_JAR_AT_UNSAFE(*s, posX, posY) = (TetrisCell){.type = type};
        }
    }
}

extern inline void TetrisSetKey(TetrisGameState *s, TetrisKeyState *k, bool down) {
    k->down = down;
    if (down) k->frameDown = s->frame;
}

bool TetrisDisplaceFallPiece(TetrisGameState *s, int displaceX, int displaceY, int rotate)
{
    bool result = TetrisCheckCollisionDisplace(s, displaceX, displaceY, rotate);

    if (result == false) {
        s->fallPiece.posX += displaceX;
        s->fallPiece.posY += displaceY;
        s->fallPiece.rotation = repeat(TETRIS_PIECE_POSSIBLE_ROTATIONS, s->fallPiece.rotation + rotate);
    }
    TetrisCheckLanded(s);

    return !result;
}

#define WALL_KICKS (tetris_piece_wall_kicks_datas \
            [s->fallPiece.type == TETRIS_PIECE_I] \
            [s->fallPiece.rotation] \
            [r])

bool TetrisRotateFallPieceSRS(TetrisGameState *s, TetrisRotationDirection r)
{
    int dpx, dpy;
    for (int i = 0; i < TETRIS_WALL_KICK_TESTS_COUNT; i++) {
        dpx = WALL_KICKS[i][0];
        dpy = -WALL_KICKS[i][1];
        if (TetrisDisplaceFallPiece(s, dpx, dpy, r ? -1 : 1))
            return true;
    }
    return false;
}

#undef WALL_KICKS

bool TetrisCheckToppedOut(TetrisGameState *s) {
    for (int y = 0; y < 2; y++)
        for (int x = 0; x < 4; x++)
            if (TetrisCellOccupied(s, 3+x, -3+y))
                return true;
    return false;
}

void TetrisCheckFullLines(TetrisGameState *s) {
    s->countLinesFull = 0;

    for (int y = -TETRIS_JAR_HEIGHT; y < TETRIS_JAR_HEIGHT; y++) {
        for (int x = 0; x < TETRIS_JAR_WIDTH; x++)
            if (!TetrisCellOccupied(s, x, y))
                goto TetrisNotFull;
        s->countLinesFull += 1;
        s->maskLinesFull[y+TETRIS_JAR_HEIGHT] = true;
        TetrisNotFull:
        (void) 0; // a line that does nothing to avoid -Wc2x-extension
    }
}

void TetrisRemoveFullLines(TetrisGameState *s) {
    int y = TETRIS_JAR_HEIGHT-1;
    int yNew = TETRIS_JAR_HEIGHT-1;

    for (; y >= -TETRIS_JAR_HEIGHT; y--) {
        if (s->maskLinesFull[y+TETRIS_JAR_HEIGHT]) {
            s->maskLinesFull[y+TETRIS_JAR_HEIGHT] = false;
            continue;
        }
        for (int x = 0; x < TETRIS_JAR_WIDTH; x++)
            TETRIS_JAR_AT_UNSAFE(*s, x, yNew) = TETRIS_JAR_AT_UNSAFE(*s, x, y);
        yNew--;
    }

    for (; yNew >= -TETRIS_JAR_HEIGHT; yNew--)
        for (int x = 0; x < TETRIS_JAR_WIDTH; x++)
            TETRIS_JAR_AT_UNSAFE(*s, x, yNew) = (TetrisCell){.type = TETRIS_CELL_BLANK};
}

void TetrisHandleInput(TetrisGameState *s)
{
    bool pressedRight = false;
    bool pressedLeft = false;

    if (IsKeyPressed(KEY_RIGHT)) {
        TetrisSetKey(s, &s->keyRight, true);
        pressedRight = true;
    }
    if (IsKeyReleased(KEY_RIGHT))
        TetrisSetKey(s, &s->keyRight, false);
    if (IsKeyPressed(KEY_LEFT)) {
        TetrisSetKey(s, &s->keyLeft, true);
        pressedLeft = true;
    }
    if (IsKeyReleased(KEY_LEFT))
        TetrisSetKey(s, &s->keyLeft, false);
    if (IsKeyPressed(KEY_DOWN))
        TetrisSetKey(s, &s->keySoftDrop, true);
    if (IsKeyReleased(KEY_DOWN))
        TetrisSetKey(s, &s->keySoftDrop, false);


    if (!s->fallPiece.locked) {
        if (pressedRight) {
            if (TetrisDisplaceFallPiece(s, 1, 0, 0)) {
                s->frameMovement = s->frame;
                s->countSafeMove += 1;
            }
        }
        if (pressedLeft) {
            if (TetrisDisplaceFallPiece(s, -1, 0, 0)) {
                s->frameMovement = s->frame;
                s->countSafeMove += 1;
            }
        }

        if (IsKeyPressed(KEY_UP)) {
            if (TetrisRotateFallPieceSRS(s, TETRIS_CW)) {
                s->frameMovement = s->frame;
                s->countSafeRotation += 1;
            }
        }

        if (IsKeyPressed(KEY_Z)) {
            if (TetrisRotateFallPieceSRS(s, TETRIS_CCW)) {
                s->frameMovement = s->frame;
                s->countSafeRotation += 1;
            }
        }

        bool doRepeatRight = s->keyRight.down && (!s->keyLeft.down || s->keyRight.frameDown >= s->keyLeft.frameDown);
        bool doRepeatLeft = s->keyLeft.down && (!s->keyRight.down || s->keyRight.frameDown < s->keyLeft.frameDown);
        bool succeededRepeatDelay = s->frame - s->frameRepeat >= s->delayRepeat;
        bool succeededFirstRepeatDelayRight = s->frame - s->keyRight.frameDown >= s->delayRepeatFirst;
        bool succeededFirstRepeatDelayLeft = s->frame - s->keyLeft.frameDown >= s->delayRepeatFirst;

        if (doRepeatRight && (succeededFirstRepeatDelayRight && succeededRepeatDelay)) {
            if (TetrisDisplaceFallPiece(s, 1, 0, 0)) {
                s->frameMovement = s->frame;
                s->frameRepeat = s->frame;
                s->countSafeMove += 1;
            }
        }
        else if (doRepeatLeft && (succeededFirstRepeatDelayLeft && succeededRepeatDelay)) {
            if (TetrisDisplaceFallPiece(s, -1, 0, 0)) {
                s->frameMovement = s->frame;
                s->frameRepeat = s->frame;
                s->countSafeMove += 1;
            }
        }
    }
}

void TetrisDoGravity(TetrisGameState *s)
{
    int32_t factoredFallDelay = s->delayFall / (s->keySoftDrop.down ? s->factorSoftDrop : 1);
    bool succeededFallDelay = s->frame - max(max(s->frameFall, s->frameUnland), s->frameSpawn) >= factoredFallDelay;

    if (succeededFallDelay) {
        s->frameFall = s->frame;
        s->fallPiece.posY += 1;
        TetrisCheckLanded(s);
    }
}

void TetrisFallPieceLock(TetrisGameState *s)
{
    s->frameLock = s->frame;
    TetrisFallPiecePlace(s);
    s->fallPiece.locked = true;
    TetrisCheckFullLines(s);
}

void TetrisDoLanded(TetrisGameState *s)
{
    bool succeededSafeMoveThreshold = s->countSafeMove >= s->thresholdSafeMove;
    bool succeededSafeRotationThreshold = s->countSafeRotation >= s->thresholdSafeRotation;
    bool succeededLockDelay = s->frame - max(s->frameLand, s->frameMovement) >= s->delayLock;

    if (succeededLockDelay || succeededSafeMoveThreshold || succeededSafeRotationThreshold) {
        TetrisFallPieceLock(s);
    }
}

void TetrisFrame(TetrisGameState *s)
{
    s->frame++;

    TetrisHandleInput(s);

    if (!s->fallPiece.landed && !s->fallPiece.locked)
        TetrisDoGravity(s);

    if (!(s->fallPiece.posY <= s->fallPiece.maxY)) {
        s->countSafeRotation = 0;
        s->countSafeMove = 0;
    }

    if (s->fallPiece.landed && !s->fallPiece.locked)
        TetrisDoLanded(s);

    if (s->fallPiece.locked) {
        bool succeededSpawnDelay = s->frame - s->frameLock >= s->delaySpawn;

        if (succeededSpawnDelay) {
            s->frameSpawn = s->frame;
            TetrisRemoveFullLines(s);
            TetrisNextFallPiece(s);
            TetrisCheckLanded(s);
        }
    }

    s->fallPiece.maxY = max(s->fallPiece.maxY, s->fallPiece.posY);

    s->isGameOver = TetrisCheckToppedOut(s);
}
