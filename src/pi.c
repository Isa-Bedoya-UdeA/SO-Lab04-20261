// pi.c
// Laboratorio 4 - Sección 1: Cálculo serial de π mediante integración numérica.
// Aproxima π usando la regla del punto medio sobre la integral:
//   ∫[0,1] 4/(1 + x^2) dx = π
#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

/**
 * f() - Función a integrar: f(x) = 4 / (1 + x^2).
 * @x: Valor de evaluación en el intervalo [0, 1].
 *
 * Return: Valor de la función en x.
 */
double f(double x) {
    return 4.0 / (1.0 + x * x);
}

/**
 * CalcPi() - Calcula π usando la regla del punto medio (serial).
 * @n: Número de rectángulos (iteraciones) a utilizar.
 *
 * Divide el intervalo [0, 1] en n subintervalos de ancho fH = 1/n.
 * Para cada subintervalo evalúa la función en el punto medio y
 * acumula el área del rectángulo correspondiente.
 *
 * Return: Aproximación de π.
 */
double CalcPi(int n) {
    const double fH = 1.0 / (double) n;
    double fSum = 0.0;
    double fX;
    for (int i = 0; i < n; i++) {
        fX = fH * ((double) i + 0.5);
        fSum += f(fX);
    }
    return fH * fSum;
}

/**
 * GetTime() - Obtiene el tiempo actual en segundos (alta resolución).
 *
 * Return: Tiempo en segundos con precisión de nanosegundos.
 */
double GetTime(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <n_iteraciones>\n", argv[0]);
        return 1;
    }
    int n = atoi(argv[1]);
    if (n <= 0) {
        fprintf(stderr, "Error: n debe ser un entero positivo.\n");
        return 1;
    }

    double t0 = GetTime();
    double pi = CalcPi(n);
    double t1 = GetTime();

    printf("===========================================\n");
    printf("  Cálculo SERIAL de π\n");
    printf("===========================================\n");
    printf("  Iteraciones : %d\n", n);
    printf("  Valor de π  : %.15f\n", pi);
    printf("  Tiempo (Ts) : %.6f segundos\n", t1 - t0);
    printf("===========================================\n");
    return 0;
}