// fibonacci.c
// Laboratorio 4 - Sección 2: Generador de la secuencia de Fibonacci
// usando un único hilo trabajador.
//
// F(-2) = 0, F(-1) = 1, F(n) = F(n-1) + F(n-2) para n >= 0.
// Secuencia resultante: 0, 1, 1, 2, 3, 5, 8, 13, 21, 34, ...
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/**
 * struct FibData - Parámetros compartidos entre main y el hilo trabajador.
 * @array: Puntero al arreglo dinámico donde se almacenará la secuencia.
 * @N:     Cantidad de elementos a generar.
 */
typedef struct {
    long long *array;
    int        N;
} FibData;

/**
 * fibonacci_worker() - Hilo que calcula y almacena los N números.
 * @arg: Puntero a FibData con el arreglo compartido y N.
 *
 * Llena el arreglo secuencialmente: array[0]=0, array[1]=1,
 * y array[i] = array[i-1] + array[i-2] para i >= 2.
 *
 * Return: NULL.
 */
void *fibonacci_worker(void *arg) {
    FibData *data = (FibData *) arg;
    if (data->N > 0) data->array[0] = 0;
    if (data->N > 1) data->array[1] = 1;
    for (int i = 2; i < data->N; i++) {
        data->array[i] = data->array[i - 1] + data->array[i - 2];
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <N>\n", argv[0]);
        return 1;
    }
    int N = atoi(argv[1]);
    if (N <= 0) {
        fprintf(stderr, "Error: N debe ser un entero positivo.\n");
        return 1;
    }

    // main asigna memoria dinámica para el arreglo compartido
    long long *array = (long long *) malloc(N * sizeof(long long));
    if (!array) {
        perror("malloc");
        return 1;
    }

    FibData data = { array, N };
    pthread_t worker;

    // Crear el hilo trabajador pasando el puntero al arreglo y N
    if (pthread_create(&worker, NULL, fibonacci_worker, &data) != 0) {
        perror("pthread_create");
        free(array);
        return 1;
    }

    // main se bloquea hasta que el trabajador complete su ejecución
    pthread_join(worker, NULL);

    // main imprime la secuencia
    printf("===========================================\n");
    printf("  Secuencia de Fibonacci (N = %d)\n", N);
    printf("===========================================\n");
    for (int i = 0; i < N; i++) {
        printf("%lld", array[i]);
        if (i < N - 1) printf(", ");
        if ((i + 1) % 10 == 0) printf("\n");
    }
    printf("\n===========================================\n");

    free(array);
    return 0;
}