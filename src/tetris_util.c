void TetrisPrintVerboseInfo(TetrisGameState *s)
{
#define LOG_EXPR_INT(expr) TraceLog(LOG_DEBUG, #expr"\t%i", (expr))
    LOG_EXPR_INT(s->frame);
    LOG_EXPR_INT(s->frameLand);
    LOG_EXPR_INT(s->frameLock);
    LOG_EXPR_INT(s->frameSpawn);
    LOG_EXPR_INT(s->frameFall);
    LOG_EXPR_INT(s->frameMove);
    LOG_EXPR_INT(s->frameRepeat);
    LOG_EXPR_INT(s->delaySpawn);
    LOG_EXPR_INT(s->fallPieceLanded);
#undef LOG_EXPR_INT
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
}
