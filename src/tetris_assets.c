typedef enum {
    TETRIS_SOUND_JINGLE_O,
    TETRIS_SOUND_JINGLE_I,
    TETRIS_SOUND_JINGLE_T,
    TETRIS_SOUND_JINGLE_L,
    TETRIS_SOUND_JINGLE_J,
    TETRIS_SOUND_JINGLE_S,
    TETRIS_SOUND_JINGLE_Z,
    TETRIS_SOUND_LAST,
} TetrisSoundType;

Sound tetris_sounds[TETRIS_SOUND_LAST];

#define TETRIS_LOAD_SOUND(sound_type, file) \
    tetris_sounds[sound_type] = LoadSound("sfx/"file)

void TetrisLoadAssets(void) {
    TETRIS_LOAD_SOUND(TETRIS_SOUND_JINGLE_O, "jingles/o.wav");
    TETRIS_LOAD_SOUND(TETRIS_SOUND_JINGLE_I, "jingles/i.wav");
    TETRIS_LOAD_SOUND(TETRIS_SOUND_JINGLE_T, "jingles/t.wav");
    TETRIS_LOAD_SOUND(TETRIS_SOUND_JINGLE_L, "jingles/l.wav");
    TETRIS_LOAD_SOUND(TETRIS_SOUND_JINGLE_J, "jingles/j.wav");
    TETRIS_LOAD_SOUND(TETRIS_SOUND_JINGLE_S, "jingles/s.wav");
    TETRIS_LOAD_SOUND(TETRIS_SOUND_JINGLE_Z, "jingles/z.wav");
}

#define TETRIS_UNLOAD_SOUND(sound_type) \
    UnloadSound(tetris_sounds[sound_type])

void TetrisUnloadAssets(void) {
    TETRIS_UNLOAD_SOUND(TETRIS_SOUND_JINGLE_O);
    TETRIS_UNLOAD_SOUND(TETRIS_SOUND_JINGLE_I);
    TETRIS_UNLOAD_SOUND(TETRIS_SOUND_JINGLE_T);
    TETRIS_UNLOAD_SOUND(TETRIS_SOUND_JINGLE_L);
    TETRIS_UNLOAD_SOUND(TETRIS_SOUND_JINGLE_J);
    TETRIS_UNLOAD_SOUND(TETRIS_SOUND_JINGLE_S);
    TETRIS_UNLOAD_SOUND(TETRIS_SOUND_JINGLE_Z);
}