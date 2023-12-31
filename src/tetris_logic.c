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

extern inline int repeat(int len, int n) {
    if ((n %= len) < 0) n += len;
    return n;
}

extern inline TetrisPieceShape *TetrisFallPieceShape(TetrisGameState *s, int rotate) {
    return (TetrisPieceShape *) &tetris_piece_shapes_datas[s->fallPiece.type][repeat(TETRIS_PIECE_POSSIBLE_ROTATIONS, s->fallPiece.rotation + rotate)];
}

extern inline bool TetrisCellOccupied(TetrisGameState *s, int posX, int posY) {
    return TETRIS_JAR_AT(*s, posX, posY).type >= 0;
}

// check if the falling piece collides with something if it's displaced like this
bool TetrisCheckCollisionDisplace(TetrisGameState *s, int displaceX, int displaceY, int rotate)
{
    TetrisPieceShape *fallPieceShape = TetrisFallPieceShape(s, rotate);

    for (int x = 0; x < TETRIS_PIECE_WIDTH; x++) {
        for (int y = 0; y < TETRIS_PIECE_WIDTH; y++) {
            if (! TETRIS_PIECE_SHAPE_AT(*fallPieceShape, x, y))
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
    if (collis && ! s->fallPiece.landed)
        s->frameLand = s->frame;
    else if (! collis && s->fallPiece.landed)
        s->frameUnland = s->frame;
    s->fallPiece.landed = collis;
}

void TetrisFallPiecePlace(TetrisGameState *s)
{
    TetrisPieceShape *fallPieceShape = TetrisFallPieceShape(s, 0);

    for (int y = 0; y < TETRIS_PIECE_HEIGHT; y++)
    {
        for (int x = 0; x < TETRIS_PIECE_WIDTH; x++)
        {
            if (! TETRIS_PIECE_SHAPE_AT(*fallPieceShape, x, y))
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

bool TetrisDisplaceFallPiece(TetrisGameState *s, int displaceX, int displaceY, int rotate) {
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

void TetrisFrame(TetrisGameState *s)
{
    s->frame++;

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


    bool succeededSafeMoveThreshold = s->countSafeMove >= s->thresholdSafeMove;
    bool succeededSafeRotationThreshold = s->countSafeRotation >= s->thresholdSafeRotation;

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

        bool rightPriority = s->keyRight.frameDown >= s->keyLeft.frameDown;
        bool succeededRepeatDelay = s->frame - s->frameRepeat >= s->delayRepeat;
        bool succeededFirstRepeatDelayRight = s->frame - s->keyRight.frameDown >= s->delayRepeatFirst;
        bool succeededFirstRepeatDelayLeft = s->frame - s->keyLeft.frameDown >= s->delayRepeatFirst;

        if (rightPriority && s->keyRight.down && (succeededFirstRepeatDelayRight && succeededRepeatDelay)) {
            if (TetrisDisplaceFallPiece(s, 1, 0, 0)) {
                s->frameMovement = s->frame;
                s->frameRepeat = s->frame;
            }
        }
        else if (s->keyLeft.down && (succeededFirstRepeatDelayLeft && succeededRepeatDelay)) {
            if (TetrisDisplaceFallPiece(s, -1, 0, 0)) {
                s->frameMovement = s->frame;
                s->frameRepeat = s->frame;
            }
        }
    }

    if (!s->fallPiece.landed && !s->fallPiece.locked)
    {
        int32_t factoredFallDelay = s->delayFall / (s->keySoftDrop.down ? s->factorSoftDrop : 1);
        bool succeededFallDelay = s->frame - max(s->frameFall, s->frameUnland) >= factoredFallDelay;

        if (succeededFallDelay) {
            s->frameFall = s->frame;
            s->fallPiece.posY += 1;
            s->countSafeMove = 0;
            TetrisCheckLanded(s);

            if (!s->fallPiece.landed)
                s->countSafeRotation = 0;
            else if (succeededSafeRotationThreshold)
                s->fallPiece.locked = true;
        }
    }
    else if (!s->fallPiece.locked)
    {
        bool succeededLockDelay = s->frame - max(s->frameLand, s->frameMovement) >= s->delayLock;

        if (succeededLockDelay || succeededSafeMoveThreshold || succeededSafeRotationThreshold) {
            s->frameLock = s->frame;
            s->fallPiece.locked = true;
        }
    }
    else if (s->fallPiece.locked)
    {
        bool succeededSpawnDelay = s->frame - s->frameLock >= s->delaySpawn;

        if (succeededSpawnDelay) {
            s->frameSpawn = s->frame;
            TetrisFallPiecePlace(s);
            TetrisNextFallPiece(s);
            s->fallPiece.locked = false;
            s->fallPiece.landed = false;
            TetrisCheckLanded(s);
        }
    }
}
