TetrisGameState game = {
    .gravity = TETRIS_G/60,
    .delaySpawn = 0,
    .delaySpawnClear = 20,
    .thresholdSafeMove = 15,
    .thresholdSafeRotation = 11,

    .delayRepeatFirst = 6,
    .delayRepeat = 0,
    .factorSoftDrop = -1,
    .keys = {
        [TETRIS_KEY_MOVE_RIGHT] = KEY_RIGHT,
        [TETRIS_KEY_MOVE_LEFT] = KEY_LEFT,
        [TETRIS_KEY_ROTATE_CW] = KEY_UP,
        [TETRIS_KEY_ROTATE_CCW] = KEY_Z,
        [TETRIS_KEY_SOFT_DROP] = KEY_DOWN,
        [TETRIS_KEY_HARD_DROP] = KEY_SPACE,
        [TETRIS_KEY_HOLD] = KEY_C,
    }
};

bool assets_exist = false;

#define WIDTH 800
#define HEIGHT 450

int main(void)
{
    InitWindow(WIDTH, HEIGHT, "TETЯIS");
    InitAudioDevice();

    assets_exist = ChangeDirectory("assets");
    if (assets_exist)
        TetrisLoadAssets();

    SetTraceLogLevel(LOG_INFO);

    SetTargetFPS(60);
    SetExitKey(KEY_Q);

    TetrisGameStateInit(&game);

    while (1) {
        BeginDrawing();

        ClearBackground(BLACK);

        // UI

        if (game.holdPieceType != (TetrisPieceType)TETRIS_PIECE_FIRST - 1) {
            TetrisDrawHoldPiece(&game, 255, 85, 60, 60);
            DrawText("Hold", 255, 75, 20, tetris_colors[TETRIS_COLOR_TEXT_PRIMARY]);
        }

        DrawText("Level", 255, 295, 20, tetris_colors[TETRIS_COLOR_TEXT_PRIMARY]);
        DrawText(TextFormat("%i", game.level), 255, 315, 20, tetris_colors[TETRIS_COLOR_TEXT_SECONDARY]);
        DrawText("Score", 255, 335, 20, tetris_colors[TETRIS_COLOR_TEXT_PRIMARY]);
        DrawText(TextFormat("%i", game.score), 255, 355, 20, tetris_colors[TETRIS_COLOR_TEXT_SECONDARY]);

        DrawText("Next", 485, 75, 20, tetris_colors[TETRIS_COLOR_TEXT_PRIMARY]);
        for (int i = 0; i < 5; i++)
            TetrisDrawPieceShape(
                game.bag.values[repeat(game.bag.index+i, TETRIS_BAG_SIZE)], 0,
                485, 85+i*45, 60, 60,
                tetris_colors[TETRIS_COLOR_CELL_FIRST + game.bag.values[repeat(game.bag.index+i, TETRIS_BAG_SIZE)]]
            );

        DrawText("Time", 485, 335, 20, tetris_colors[TETRIS_COLOR_TEXT_PRIMARY]);
        DrawText(TextFormat("%im%is %if", game.frame/60/60, game.frame/60%60, game.frame%60),
                485, 355, 20, tetris_colors[TETRIS_COLOR_TEXT_SECONDARY]);

        // SOUND
        if (game.frameSpawn == game.frame)
            PlaySound(tetris_sounds[game.bag.values[repeat(game.bag.index, TETRIS_BAG_SIZE)]]);

        // BOARD
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
