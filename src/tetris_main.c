TetrisGameState game = {
    .gravity = TETRIS_G/60,
    .delayLock = 30,
    .delaySpawn = 0,
    .delaySpawnClear = 14,
    .thresholdSafeMove = 15,
    .thresholdSafeRotation = 11,

    .delayRepeatFirst = 6,
    .delayRepeat = 0,
    .factorSoftDrop = -1,
};

#define WIDTH 800
#define HEIGHT 450

int main(void)
{
    InitWindow(WIDTH, HEIGHT, "TETЯIS");
    InitAudioDevice();

    assets_exist = ChangeDirectory("assets");
    if (assets_exist)
        TetrisLoadAssets();

    SetTraceLogLevel(LOG_DEBUG);

    SetTargetFPS(60);
    SetExitKey(KEY_Q);

    TetrisGameStateInit(&game);

    while (1) {
        BeginDrawing();

        ClearBackground(BLACK);

        if (game.holdPieceType != (TetrisPieceType)TETRIS_PIECE_FIRST - 1) {
            TetrisDrawHoldPiece(&game, 255, 85, 60, 60);
            DrawText("Hold", 255, 75, 20, tetris_colors[TETRIS_COLOR_TEXT_PRIMARY]);
        }

        DrawText("Next", 485, 75, 20, tetris_colors[TETRIS_COLOR_TEXT_PRIMARY]);
        for (int i = 0; i < 5; i++)
            TetrisDrawPieceShape(
                game.bag.values[repeat(game.bag.index+i, TETRIS_BAG_SIZE)], 0,
                485, 85+i*45, 60, 60,
                tetris_colors[TETRIS_COLOR_CELL_FIRST + game.bag.values[repeat(game.bag.index+i, TETRIS_BAG_SIZE)]]
            );

        if (game.frameSpawn == game.frame)
            PlaySound(tetris_sounds[game.bag.values[repeat(game.bag.index, TETRIS_BAG_SIZE)]]);

        TetrisDrawGameJar(&game, 325, 75, 150, 300);

        EndDrawing();

        if (WindowShouldClose()) break;

        TetrisFrame(&game);
        /* TetrisPrintVerboseInfo(&game); */

        if (game.isGameOver) break;
    }

    TetrisUnloadAssets();

    CloseAudioDevice();
    CloseWindow();

    return 0;
}
