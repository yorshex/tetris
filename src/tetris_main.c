TetrisGameState game = {
    .delayFall = 60,
    .delayLock = 30,
    .delaySpawn = 30,
    .thresholdSafeMove = 11,
    .thresholdSafeRotation = 11,

    .delayRepeatFirst = 12,
    .delayRepeat = 0,
    .factorSoftDrop = 1000,
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
        TetrisDrawGameJar(&game, 50, 50, 150, 300);
        EndDrawing();

        TetrisPrintVerboseInfo(&game);

        if (WindowShouldClose()) break;

        TetrisFrame(&game);
    }

    return 0;
}
