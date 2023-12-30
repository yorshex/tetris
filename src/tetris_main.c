TetrisGameState game;

#define WIDTH 800
#define HEIGHT 450

int main(void)
{
    InitWindow(WIDTH, HEIGHT, "TETЯIS");
    SetTargetFPS(60);

    TetrisGameStateInit(&game);

    SetTraceLogLevel(LOG_DEBUG);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        TetrisDrawGameJar(&game, 50, 50, 150, 300);
        TetrisPrintVerboseInfo(&game);
        EndDrawing();
        TetrisFrame(&game);
    }

    return 0;
}
