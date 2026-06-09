// pi_p.c
// Laboratorio 4 - Sección 1: Cálculo PARALELO de π mediante Pthreads.
// Estrategia: Data Parallelism — el rango [0, n-1] se divide en T
// sub-rangos; cada hilo calcula una suma parcial local (sin mutex)
// y la retorna al hilo principal vía pthread_join.
#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

/**
 * struct ThreadData - Datos que se pasan a cada hilo trabajador.
 * @start:         Índice inicial (incluido) del sub-rango.
 * @end:           Índice final (excluido) del sub-rango.
 * @fH:            Ancho de cada rectángulo (1/n).
 * @partial_sum:   Suma parcial acumulada por el hilo (salida).
 */
typedef struct {
    int    start;
    int    end;
    double fH;
    double partial_sum;
} ThreadData;

/**
 * f() - Función a integrar: f(x) = 4 / (1 + x^2).
 */
double f(double x) {
    return 4.0 / (1.0 + x * x);
}

/**
 * worker() - Función ejecutada por cada hilo.
 * @arg: Puntero a ThreadData con el sub-rango asignado.
 *
 * Itera únicamente sobre [start, end), acumulando el área
 * en una variable local. No se usan mutex para evitar contención.
 *
 * Return: NULL.
 */
void *worker(void *arg) {
    ThreadData *data = (ThreadData *) arg;
    double sum = 0.0;
    for (int i = data->start; i < data->end; i++) {
        double fX = data->fH * ((double) i + 0.5);
        sum += f(fX);
    }
    data->partial_sum = sum;
    return NULL;
}

/**
 * GetTime() - Obtiene el tiempo actual en segundos (alta resolución).
 */
double GetTime(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <n_iteraciones> <num_hilos>\n", argv[0]);
        return 1;
    }
    int n = atoi(argv[1]);
    int T = atoi(argv[2]);
    if (n <= 0 || T <= 0) {
        fprintf(stderr, "Error: n y T deben ser enteros positivos.\n");
        return 1;
    }

    pthread_t  *threads = malloc(T * sizeof(pthread_t));
    ThreadData *data    = malloc(T * sizeof(ThreadData));
    if (!threads || !data) {
        perror("malloc");
        return 1;
    }

    double fH   = 1.0 / (double) n;
    int    chunk = n / T;

    double t0 = GetTime();

    // Crear T hilos, cada uno con su sub-rango
    for (int i = 0; i < T; i++) {
        data[i].start       = i * chunk;
        data[i].end         = (i == T - 1) ? n : (i + 1) * chunk;
        data[i].fH          = fH;
        data[i].partial_sum = 0.0;
        if (pthread_create(&threads[i], NULL, worker, &data[i]) != 0) {
            perror("pthread_create");
            return 1;
        }
    }

    // Sincronización: esperar a todos los hilos y acumular sumas parciales
    double total_sum = 0.0;
    for (int i = 0; i < T; i++) {
        pthread_join(threads[i], NULL);
        total_sum += data[i].partial_sum;
    }

    double t1 = GetTime();
    double pi = fH * total_sum;

    printf("===========================================\n");
    printf("  Cálculo PARALELO de π\n");
    printf("===========================================\n");
    printf("  Iteraciones : %d\n", n);
    printf("  Hilos (T)   : %d\n", T);
    printf("  Valor de π  : %.15f\n", pi);
    printf("  Tiempo (Tp) : %.6f segundos\n", t1 - t0);
    printf("===========================================\n");

    free(threads);
    free(data);
    return 0;
}