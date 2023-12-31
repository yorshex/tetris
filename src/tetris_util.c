extern inline int max(int a, int b) {
    if (b > a) return b;
    return a;
}

extern inline void TetrisSwap(int *restrict a, int *restrict b) {
    static int temp;
    temp = *a;
    *a = *b;
    *b = temp;
}

void TetrisShuffle(int *arr, size_t n) {
    for (int i = n - 1; i > 0; i--) {
        int j = GetRandomValue(0, i);
        TetrisSwap(&arr[i], &arr[j]); 
    } 
}

void TetrisPrintVerboseInfo(TetrisGameState *s)
{
#define LOG_EXPR_INT(expr) TraceLog(LOG_DEBUG, #expr"\t%i", (expr))
#define LOG_EXPR_BOOL(expr) TraceLog(LOG_DEBUG, #expr"\t%s", (expr) ? "true" : "false")
#define LOG_EXPR_KEY(expr) TraceLog(LOG_DEBUG, #expr"\t%s, down at %i", (expr).down ? "down" : "up", (expr).frameDown)
    LOG_EXPR_INT(s->frame);
    LOG_EXPR_INT(s->frameLand);
    LOG_EXPR_INT(s->frameUnland);
    LOG_EXPR_INT(s->frameLock);
    LOG_EXPR_INT(s->frameSpawn);
    LOG_EXPR_INT(s->frameFall);
    LOG_EXPR_INT(s->frameMovement);
    LOG_EXPR_INT(s->countSafeMove);
    LOG_EXPR_INT(s->countSafeRotation);
    LOG_EXPR_KEY(s->keyRight);
    LOG_EXPR_KEY(s->keyLeft);
    LOG_EXPR_KEY(s->keySoftDrop);
    LOG_EXPR_BOOL(s->fallPiece.landed);
    LOG_EXPR_BOOL(s->fallPiece.locked);
    LOG_EXPR_INT(s->bag.index);
    TraceLog(LOG_DEBUG, "s->bag\t%i %i %i %i %i %i %i\n"
                            "\t\t%i %i %i %i %i %i %i",
            s->bag.values[0],
            s->bag.values[1],
            s->bag.values[2],
            s->bag.values[3],
            s->bag.values[4],
            s->bag.values[5],
            s->bag.values[6],
            s->bag.values[7],
            s->bag.values[8],
            s->bag.values[9],
            s->bag.values[10],
            s->bag.values[11],
            s->bag.values[12],
            s->bag.values[13]
            );
#undef LOG_EXPR_INT
#undef LOG_EXPR_BOOL
}
