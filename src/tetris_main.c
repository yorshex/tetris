TetrisGameState game = {
    .delayRepeatFirst = 8,
    .delayRepeat = 1,
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
RenderTexture2D render_texture;

#define WIDTH 384
#define HEIGHT 216
#define ASPECT_RATIO ((float)WIDTH/HEIGHT)

#define WINWIDTH 800
#define WINHEIGHT 450

Rectangle destination_rect = (Rectangle){0, 0, WINWIDTH, WINHEIGHT};

void TetrisOnResize(void)
{
    ClearBackground(BLACK);

    int xw = GetScreenWidth();
    int yw = GetScreenHeight();

    if (xw/ASPECT_RATIO > yw) {
        destination_rect.width = yw * ASPECT_RATIO;
        destination_rect.height = yw;
        destination_rect.x = (xw - destination_rect.width) * .5;
        destination_rect.y = 0;
    }
    else {
        destination_rect.width = xw;
        destination_rect.height = xw / ASPECT_RATIO;
        destination_rect.y = (yw - destination_rect.height) * .5;
        destination_rect.x = 0;
    }
}

int main(void)
{
    InitWindow(WINWIDTH, WINHEIGHT, "TETЯIS");
    InitAudioDevice();

    SetWindowState(FLAG_WINDOW_RESIZABLE);

    render_texture = LoadRenderTexture(WIDTH, HEIGHT);

    assets_exist = ChangeDirectory("assets");
    if (assets_exist)
        TetrisLoadAssets();

    SetTargetFPS(60);

    TetrisGameStateInit(&game);

    while (1) {
        // SOUND
        if (game.frameSpawn == game.frame)
            PlaySound(tetris_sounds[game.bag.values[repeat(game.bag.index, TETRIS_BAG_SIZE)]]);

        BeginTextureMode(render_texture);
        TetrisDrawGame(&game);
        EndTextureMode();

        BeginDrawing();
        if (IsWindowResized())
            TetrisOnResize();
        DrawTexturePro(
                render_texture.texture,
                (Rectangle){0, HEIGHT, WIDTH, -HEIGHT},
                // reverse vertically cuz textures are upside down for gl
                destination_rect,
                (Vector2){0, 0}, 0, WHITE
            );
        EndDrawing();

        if (WindowShouldClose()) break;

        TetrisFrame(&game);

        if (game.isGameOver) break;
    }

    TetrisUnloadAssets();

    UnloadRenderTexture(render_texture);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
