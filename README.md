# SO - Lab04 - 20261

## Equipo

* Rafael Angel Alemán Castillo. [rafael.aleman@udea.edu.co](mailto:rafael.aleman@udea.edu.co). CC. 1001560844
* Isabela Bedoya Gaviria. [isabela.bedoya@udea.edu.co](mailto:isabela.bedoya@udea.edu.co). CC. 1020106520

---

## Cómo ejecutar

### Requisitos previos

* Sistema operativo Linux (o WSL en Windows).
* GCC instalado (`gcc --version`).
* `make` instalado (`make --version`).
* Python 3 con `jupyter`, `pandas` y `matplotlib` para el notebook de análisis.

### Pasos para compilar y ejecutar

```bash
# 1. Activar WSL (se puede saltar hasta el paso 4 si el SO es Linux)
wsl

# 2. Entrar a la ruta del proyecto
cd ruta/donde/tienes/el/repo/clonado

# 3. Abrir el editor de código en WSL (o se puede continuar desde la terminal)

code .

# 4. Compilar todos los programas (en la CMD, Powershell, o terminar directa de VSCode, por ejemplo)
make

# 3. Ejecutar la versión serial de π
./pi_s 2000000000

# 4. Ejecutar la versión paralela de π (n=2000000000, T=4 hilos)
./pi_p 2000000000 4

# 5. Generar los primeros 15 números de Fibonacci
./fibonacci 15

# 6. Abrir el notebook de análisis, o si se tiene abierto el editor de código, ejecutar las celdas
jupyter notebook analisis.ipynb

# 7. Limpiar binarios
make clean
```

### Si causa problemas por la descarga de librerias

```bash
sudo apt update && sudo apt install python3-pandas -y
```

---

## Documentación

### Estructura del proyecto

```plain text
SO-Lab04-20261/
├── pi.c              # Cálculo serial de π
├── pi_p.c            # Cálculo paralelo de π con Pthreads
├── fibonacci.c       # Generador multihilo de Fibonacci
├── analisis.ipynb    # Notebook de análisis y métricas
├── Makefile          # Compilación automatizada
└── README.md         # Este informe
```

### `pi.c` — Cálculo serial de π

| Función | Descripción |
| ------- | ----------- |
| `double f(double x)` | Evalúa la función a integrar: `f(x) = 4 / (1 + x²)`. |
| `double CalcPi(int n)` | Aproxima π usando la regla del punto medio con `n` rectángulos. Acumula el área total en un bucle `for` serial. |
| `double GetTime(void)` | Retorna el tiempo actual en segundos usando `clock_gettime(CLOCK_MONOTONIC)`. |
| `int main(int argc, char *argv[])` | Valida el argumento `n`, mide el tiempo de ejecución de `CalcPi` y muestra el resultado. |

### `pi_p.c` — Cálculo paralelo de π

| Función | Descripción |
| ------- | ----------- |
| `double f(double x)` | Igual que en `pi.c`. |
| `void *worker(void *arg)` | Función ejecutada por cada hilo. Recibe una estructura `ThreadData` con el sub-rango `[start, end)` asignado y calcula la suma parcial **local** (sin mutex). Guarda el resultado en `data->partial_sum`. |
| `double GetTime(void)` | Igual que en `pi.c`. |
| `int main(int argc, char *argv[])` | Recibe `n` y `T` (número de hilos). Calcula el tamaño del sub-rango `chunk = n/T`, crea `T` hilos con `pthread_create`, espera a todos con `pthread_join`, suma las sumas parciales y multiplica por `fH` para obtener π. |

### `fibonacci.c` — Generador multihilo de Fibonacci

| Función | Descripción |
| ------- | ----------- |
| `void *fibonacci_worker(void *arg)` | Hilo trabajador. Recibe una estructura `FibData` con el puntero al arreglo compartido y `N`. Llena el arreglo con los `N` números de Fibonacci. |
| `int main(int argc, char *argv[])` | Recibe `N`, reserva memoria dinámica con `malloc` para el arreglo, crea un único hilo trabajador con `pthread_create`, espera con `pthread_join` y luego imprime la secuencia. |

---

## Pruebas

### Compilación exitosa

```bash
$ make
gcc -Wall -Wextra -std=c11 -g -O2 -o pi_s pi.c -lpthread -lm
gcc -Wall -Wextra -std=c11 -g -O2 -o pi_p pi_p.c -lpthread -lm
gcc -Wall -Wextra -std=c11 -g -O2 -o fibonacci fibonacci.c -lpthread
```

### Cálculo serial de π (n = 2×10⁹)

```bash
$ ./pi_s 2000000000
===========================================
  Cálculo SERIAL de π
===========================================
  Iteraciones : 2000000000
  Valor de π  : 3.141592653589842
  Tiempo (Ts) : 5.432187 segundos
===========================================
```

### Cálculo paralelo de π (n = 2×10⁹, T = 4)

```bash
$ ./pi_p 2000000000 4
===========================================
  Cálculo PARALELO de π
===========================================
  Iteraciones : 2000000000
  Hilos (T)   : 4
  Valor de π  : 3.141592653589842
  Tiempo (Tp) : 1.487213 segundos
===========================================
```

### Generador de Fibonacci (N = 15)

```bash
$ ./fibonacci 15
===========================================
  Secuencia de Fibonacci (N = 15)
===========================================
0, 1, 1, 2, 3, 5, 8, 13, 21, 34,
55, 89, 144, 233, 377
===========================================
```

### Manejo de errores

```bash
$ ./pi_s
Uso: ./pi_s <n_iteraciones>

$ ./pi_p 100
Uso: ./pi_p <n_iteraciones> <num_hilos>

$ ./fibonacci
Uso: ./fibonacci <N>

$ ./pi_s -5
Error: n debe ser un entero positivo.
```

---

## Problemas presentados y soluciones

| Problema | Solución |
| -------- | -------- |
| Al crear los hilos con `pthread_create` y sumar dentro del bucle con un mutex, el Speedup se degradaba significativamente. | Se cambió la estrategia: cada hilo acumula una suma **local** sin contención y solo se comparte el resultado final vía `ThreadData`. Esto elimina por completo el uso de mutex. |
| Con valores muy grandes de `n` (> 10⁹), la suma acumulada perdía precisión. | Se mantuvo el tipo `double` para las sumas y se verificó que el error absoluto con π matemático se mantuviera en el orden de 10⁻¹². |
| Al lanzar más hilos que núcleos físicos, el tiempo no mejoraba. | Es el comportamiento esperado: el sistema operativo debe alternar entre hilos (context switch), introduciendo overhead. El análisis lo recoge en la sección de eficiencia. |

---

## Video

[Fallback Video]

> **Contenido del video:** (1) Estrategia de paralelización en `pi_p.c`; (2) paso de parámetros y sincronización en `fibonacci.c`; (3) demostración en vivo; (4) análisis del gráfico de Speedup y conclusiones.

---

## Manifiesto de transparencia

Se utilizó IA generativa (Claude Sonnet 4.6) en los siguientes puntos del desarrollo:

* **Documentación del README.md:** las tablas de documentación y secciones del informe se redactaron con apoyo de IA a partir del código escrito manualmente.
* **Generación del Makefile:** la IA asistió en la redacción del `Makefile` con objetivos para cada programa.
* **Comentarios en el código:** los bloques Doxygen-style se apoyaron en IA para mayor claridad.

El código C fue escrito, compilado y probado por el equipo. El análisis conceptual y las conclusiones son propios del equipo y se contrastaron con el material del curso (OSTEP capítulos 26 y 27).

---

## Conclusiones

1. **El paralelismo tiene un costo inherente.** El overhead de `pthread_create` y `pthread_join` hace que la versión paralela con un solo hilo sea marginalmente más lenta que la serial. El paralelismo solo se justifica cuando la carga de trabajo es suficientemente grande para amortizar este costo.
2. **La Ley de Amdahl limita la aceleración máxima.** Aunque el cálculo de π es casi 100% paralelizable, la porción secuencial (creación de hilos, acumulación final) impide que el Speedup supere el número de núcleos físicos.
3. **Evitar mutex mejora drásticamente la escalabilidad.** La estrategia de sumas parciales locales (sin bloqueos) en `pi_p.c` permite que los hilos trabajen independientemente, logrando un Speedup cercano al ideal hasta el límite físico del hardware.
4. **`pthread_join` es una barrera de sincronización esencial.** En `fibonacci.c`, el `join` garantiza que `main` no imprima resultados antes de que el hilo trabajador haya completado el cálculo, evitando condiciones de carrera y resultados inconsistentes.
5. **No todos los algoritmos son paralelizables.** Fibonacci es intrínsecamente secuencial (cada término depende de los dos anteriores). Paralelizarlo requeriría reformular el problema (p. ej. con exponenciación de matrices), lo que demuestra que la naturaleza del algoritmo determina su paralelización.
6. **El paso de parámetros mediante estructuras es la forma correcta de compartir datos.** Pasar una estructura con múltiples campos (`ThreadData`, `FibData`) es más limpio y extensible que usar `void *` directamente con un solo valor.
7. **La eficiencia decrece con el número de hilos.** Mientras más hilos se lancen, menor será la eficiencia (`Speedup / N`) debido a la contención de recursos, el scheduling del SO y el overhead acumulado.