extern inline int max(int a, int b) {
    if (b > a) return b;
    return a;
}

extern inline int repeat(int n, int len) {
    if ((n %= len) < 0) n += len;
    return n;
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
