TetrisGameState game = {
    .gravity = TETRIS_G/60,
    .delayLock = 30,
    .delaySpawn = 0,
    .thresholdSafeMove = 15,
    .thresholdSafeRotation = 11,

    .delayRepeatFirst = 10,
    .delayRepeat = 0,
    .factorSoftDrop = -1,
};

#define WIDTH 800
#define HEIGHT 450

int main(void)
{
    SetTraceLogLevel(LOG_DEBUG);

    InitWindow(WIDTH, HEIGHT, "TETЯIS");
    SetTargetFPS(60);
    SetExitKey(KEY_Q);

    TetrisGameStateInit(&game);

    while (1) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        if (game.holdPieceType != (TetrisPieceType)TETRIS_PIECE_FIRST - 1) {
            TetrisDrawHoldPiece(&game, 255, 85, 60, 60);
            DrawText("Hold", 245, 75, 20, BLACK);
        }
        TetrisDrawGameJar(&game, 325, 75, 150, 300);
        EndDrawing();

        TetrisFrame(&game);

        TetrisPrintVerboseInfo(&game);

        if (game.isGameOver || WindowShouldClose()) break;
    }

    return 0;
}
