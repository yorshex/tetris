void TetrisShuffleBagHalf(TetrisGameState *s, int32_t half)
{
    TetrisShuffle((int *) s->bag.values + TETRIS_BAG_HALF_SIZE * half, TETRIS_BAG_HALF_SIZE);
}



TetrisPieceType TetrisBagNext(TetrisGameState *s)
{
    TetrisPieceType piece = s->bag.values[s->bag.index++];

    s->bag.index %= TETRIS_BAG_SIZE;
    if (s->bag.index % TETRIS_BAG_HALF_SIZE == 0)
        TetrisShuffleBagHalf(s, !(s->bag.index >= TETRIS_BAG_HALF_SIZE));

    return piece;
}



// check whether a cell is occupied
extern inline bool TetrisCellOccupied(const TetrisGameState *s, int x, int y)
{
    return TETRIS_JAR_AT(*s, x, y).type >= 0;
}



void TetrisCheckFullLines(TetrisGameState *s)
{
    s->countLinesFull = 0;

    for (int y = -TETRIS_JAR_HEIGHT; y < TETRIS_JAR_HEIGHT; y++) {
        for (int x = 0; x < TETRIS_JAR_WIDTH; x++)
            if (!TetrisCellOccupied(s, x, y))
                goto TetrisNotFull;
        s->countLinesFull += 1;
        s->maskLinesFull[y+TETRIS_JAR_HEIGHT] = true;
        continue;
        TetrisNotFull:
        s->maskLinesFull[y+TETRIS_JAR_HEIGHT] = false;
    }
}



void TetrisFallPieceInit(TetrisGameState *s, TetrisPieceType type, bool held)
{
    s->fallPiece.posX = 3;
    s->fallPiece.posY = -4;
    s->fallPiece.maxY = -4;
    s->fallPiece.locked = false;
    s->fallPiece.landed = false;
    s->fallPiece.rotation = 0;
    s->fallPiece.type = type;
    s->fallPiece.held = held;
    s->g = TETRIS_G;
}



// get the shape of the falling piece
extern inline TetrisPieceShape *TetrisFallPieceShape(const TetrisGameState *s, int r)
{
    return (TetrisPieceShape *)
        &tetris_piece_shapes_datas
         [s->fallPiece.type]
         [repeat(s->fallPiece.rotation + r, TETRIS_PIECE_POSSIBLE_ROTATIONS)];
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



void TetrisFallPieceLock(TetrisGameState *s)
{
    s->frameLock = s->frame;
    TetrisFallPiecePlace(s);
    s->fallPiece.locked = true;
    TetrisCheckFullLines(s);
}



void TetrisNextFallPiece(TetrisGameState *s)
{
    TetrisFallPieceInit(s, TetrisBagNext(s), false);
}



void TetrisHoldFallPiece(TetrisGameState *s)
{
    TetrisPieceType type =
        s->holdPieceType != (TetrisPieceType)TETRIS_PIECE_FIRST - 1 ?
        s->holdPieceType :
        TetrisBagNext(s);
    s->holdPieceType = s->fallPiece.type;
    TetrisFallPieceInit(s, type, true);
}



void TetrisGameStateInit(TetrisGameState *s)
{
    for (int i = 0; i < TETRIS_JAR_AREA * 2; i++) {
        s->board[i].type = TETRIS_CELL_BLANK;
    }

    for (int i = 0; i < TETRIS_BAG_SIZE; i++)
        s->bag.values[i] = i % TETRIS_BAG_HALF_SIZE;
    TetrisShuffleBagHalf(s, 0);
    TetrisShuffleBagHalf(s, 1);

    TetrisNextFallPiece(s);

    s->holdPieceType = TETRIS_PIECE_FIRST - 1;
    s->delayLock = 30;

    s->gravity = TETRIS_G/60;
    s->delaySpawn = 20;
    s->delaySpawnClear = 15;
    s->thresholdSafeMove = 15;
    s->thresholdSafeRotation = 7;
}



// check if the falling piece overlaps with something if it's moved and/or rotated
bool TetrisCheckCollisionDisplace(const TetrisGameState *s, int dx, int dy, int r)
{
    TetrisPieceShape *fallPieceShape = TetrisFallPieceShape(s, r);

    for (int x = 0; x < TETRIS_PIECE_WIDTH; x++) {
        for (int y = 0; y < TETRIS_PIECE_WIDTH; y++) {
            if (!TETRIS_PIECE_SHAPE_AT(*fallPieceShape, x, y))
                continue;
            if (TetrisCellOccupied(
                    s,
                    s->fallPiece.posX + dx + x,
                    s->fallPiece.posY + dy + y))
            return true;
        }
    }

    return false;
}



void TetrisCheckLanded(TetrisGameState *s)
{
    bool collis = TetrisCheckCollisionDisplace(s, 0, 1, 0);
    if (collis && !s->fallPiece.landed) {
        s->frameLand = s->frame;
        s->g = 0;
    }
    s->fallPiece.landed = collis;
}



extern inline void TetrisSetKey(TetrisGameState *s, TetrisKeyState *k, bool down) {
    k->down = down;
    if (down) k->frameDown = s->frame;
}



bool TetrisDisplaceFallPiece(TetrisGameState *s, int dx, int dy, int r)
{
    bool result = TetrisCheckCollisionDisplace(s, dx, dy, r);

    if (result == false) {
        s->fallPiece.posX += dx;
        s->fallPiece.posY += dy;
        s->fallPiece.rotation = repeat(s->fallPiece.rotation + r, TETRIS_PIECE_POSSIBLE_ROTATIONS);
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
    int dx, dy;
    for (int i = 0; i < TETRIS_WALL_KICK_TESTS_COUNT; i++) {
        dx = WALL_KICKS[i][0];
        dy = -WALL_KICKS[i][1];
        if (TetrisDisplaceFallPiece(s, dx, dy, r ? -1 : 1))
            return true;
    }
    return false;
}

#undef WALL_KICKS



bool TetrisCheckBlockOut(const TetrisGameState *s)
{
    for (int y = 0; y < 2; y++)
        for (int x = 0; x < 4; x++)
            if (TetrisCellOccupied(s, 3+x, -3+y))
                return true;
    return false;
}



void TetrisRemoveFullLines(TetrisGameState *s) {
    int y = TETRIS_JAR_HEIGHT-1;
    int yNew = TETRIS_JAR_HEIGHT-1;

    for (; y >= -TETRIS_JAR_HEIGHT; y--) {
        if (s->maskLinesFull[y+TETRIS_JAR_HEIGHT])
            continue;
        for (int x = 0; x < TETRIS_JAR_WIDTH; x++)
            TETRIS_JAR_AT_UNSAFE(*s, x, yNew) = TETRIS_JAR_AT_UNSAFE(*s, x, y);
        yNew--;
    }

    for (; yNew >= -TETRIS_JAR_HEIGHT; yNew--)
        for (int x = 0; x < TETRIS_JAR_WIDTH; x++)
            TETRIS_JAR_AT_UNSAFE(*s, x, yNew) = (TetrisCell){.type = TETRIS_CELL_BLANK};
}



void TetrisHardDropFallPiece(TetrisGameState *s) {
    while (!TetrisCheckCollisionDisplace(s, 0, 1, 0)) {
        s->fallPiece.posY += 1;
        s->frameFall = s->frame;
    }
    TetrisCheckLanded(s);
    TetrisFallPieceLock(s);
}



void TetrisHandleInput(TetrisGameState *s)
{
    bool pressedRight = false;
    bool pressedLeft = false;

    // register keypresses
    if (IsKeyPressed(s->keys[TETRIS_KEY_MOVE_RIGHT])) {
        TetrisSetKey(s, &s->keyRight, true);
        pressedRight = true;
    }

    if (IsKeyReleased(s->keys[TETRIS_KEY_MOVE_RIGHT]))
        TetrisSetKey(s, &s->keyRight, false);

    if (IsKeyPressed(s->keys[TETRIS_KEY_MOVE_LEFT])) {
        TetrisSetKey(s, &s->keyLeft, true);
        pressedLeft = true;
    }

    if (IsKeyReleased(s->keys[TETRIS_KEY_MOVE_LEFT]))
        TetrisSetKey(s, &s->keyLeft, false);

    if (IsKeyPressed(s->keys[TETRIS_KEY_SOFT_DROP]))
        TetrisSetKey(s, &s->keySoftDrop, true);

    if (IsKeyReleased(s->keys[TETRIS_KEY_SOFT_DROP]))
        TetrisSetKey(s, &s->keySoftDrop, false);

    // we don't wanna do anything to the piece if it's locked
    if (s->fallPiece.locked) return;

    // make actions approptiate to the keypresses
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

    if (IsKeyPressed(s->keys[TETRIS_KEY_ROTATE_CW])) {
        if (TetrisRotateFallPieceSRS(s, TETRIS_CW)) {
            s->frameMovement = s->frame;
            s->countSafeRotation += 1;
        }
    }

    if (IsKeyPressed(s->keys[TETRIS_KEY_ROTATE_CCW])) {
        if (TetrisRotateFallPieceSRS(s, TETRIS_CCW)) {
            s->frameMovement = s->frame;
            s->countSafeRotation += 1;
        }
    }

    if (IsKeyPressed(s->keys[TETRIS_KEY_HOLD]) && !s->fallPiece.held) {
        s->frameSpawn = s->frame;
        TetrisHoldFallPiece(s);
        TetrisCheckLanded(s);
    }

    if (IsKeyPressed(s->keys[TETRIS_KEY_HARD_DROP]) && !s->fallPiece.locked) {
        TetrisHardDropFallPiece(s);
    }

    // repeat moves right or left

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



void TetrisDoMidAir(TetrisGameState *s)
{
    if (s->gravity < 0 || (s->keySoftDrop.down && s->factorSoftDrop < 0)) {
        s->frameFall = s->frame;
        while (!TetrisCheckCollisionDisplace(s, 0, 1, 0)) {
            s->fallPiece.posY += 1;
        }
        TetrisCheckLanded(s);
        return;
    }

    int addedG = s->gravity;

    if (s->keySoftDrop.down)
        addedG *= s->factorSoftDrop;

    s->g += addedG;

    for (; s->g > TETRIS_G && !s->fallPiece.landed; s->g -= TETRIS_G) {
        s->frameFall = s->frame;
        s->fallPiece.posY += 1;
        TetrisCheckLanded(s);
    }
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



void TetrisDoAdvance(TetrisGameState *s)
{
    int sectionLevel = s->level % 100;

    sectionLevel += (1 + (s->countLinesFull > 2 ? (s->countLinesFull-1)*2 : s->countLinesFull));

    if (sectionLevel > 99 && !s->countLinesFull)
        sectionLevel = 99;

    s->level = s->level / 100 * 100 + sectionLevel;

    int section = s->level / 100;

            /* section */
    if      /* 0 to 4 */ (section <= 4) s->gravity = TETRIS_G/60 + TETRIS_G * ((double)(s->level*s->level)/40000.0);
    else    /* 5 to _ */                s->gravity = -1;

    if      /* 0 to 2 */ (section <= 2) s->delayLock = 30;
    else if /* 3 to 4 */ (section <= 4) s->delayLock = 25;
    else if /* 5 to 6 */ (section <= 6) s->delayLock = 20;
    else if /* 7 to 8 */ (section <= 8) s->delayLock = 15;
    else    /* 9 to _ */                s->delayLock = 10;

    if      /* 0 to 0 */ (section <= 0) s->delaySpawn = 20;
    else if /* 1 to 4 */ (section <= 4) s->delaySpawn = 15;
    else if /* 5 to 8 */ (section <= 8) s->delaySpawn = 10;
    else    /* 9 to _ */                s->delaySpawn = 6;

    if      /* 0 to 2 */ (section <= 2) s->delaySpawnClear = 15;
    else if /* 3 to 6 */ (section <= 6) s->delaySpawnClear = 10;
    else    /* 7 to _ */                s->delaySpawnClear = 6;


    if (s->countLinesFull > 0) {
        int lineClearMultiplier = 0;
        switch (s->countLinesFull) {
            case 4: lineClearMultiplier += 3;
            case 3: lineClearMultiplier += 2;
            case 2: lineClearMultiplier += 2;
            case 1: lineClearMultiplier += 1;
        }
        double levelTimeMultiplier = (double)max(s->level - s->frame / 60, 0) / 50 + 1;
        s->score += 100 * lineClearMultiplier * levelTimeMultiplier;
    }
}



void TetrisFrame(TetrisGameState *s)
{
    s->frame++;

    if (s->fallPiece.locked) {
        bool succeededSpawnDelay = s->frame - s->frameLock >= (s->delaySpawn + (s->countLinesFull > 0 ? s->delaySpawnClear : 0));

        if (succeededSpawnDelay)
        {
            s->frameSpawn = s->frame;

            TetrisDoAdvance(s);
            TetrisRemoveFullLines(s);
            TetrisNextFallPiece(s);
            TetrisCheckLanded(s);
        }
    }

    TetrisHandleInput(s);

    if (!s->fallPiece.landed && !s->fallPiece.locked)
        TetrisDoMidAir(s);

    if (!(s->fallPiece.posY <= s->fallPiece.maxY)) {
        s->countSafeRotation = 0;
        s->countSafeMove = 0;
    }

    if (s->fallPiece.landed && !s->fallPiece.locked)
        TetrisDoLanded(s);

    s->fallPiece.maxY = max(s->fallPiece.maxY, s->fallPiece.posY);

    s->isGameOver = TetrisCheckBlockOut(s);
}
