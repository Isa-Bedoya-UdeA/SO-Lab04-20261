
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
int main(int argc, char *argv[]) {
    int N = atoi(argv[1]);
    long long *arr = malloc(N * sizeof(long long));
    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);
    if (N > 0) arr[0] = 0;
    if (N > 1) arr[1] = 1;
    for (int i = 2; i < N; i++) arr[i] = arr[i-1] + arr[i-2];
    clock_gettime(CLOCK_MONOTONIC, &t1);
    double ms = (t1.tv_sec - t0.tv_sec)*1000.0 + (t1.tv_nsec - t0.tv_nsec)/1e6;
    printf("Serial: %.3f ms (último valor: %lld)\n", ms, arr[N-1]);
    free(arr);
    return 0;
}
