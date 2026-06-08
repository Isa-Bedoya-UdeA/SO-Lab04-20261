# =============================================================
# Makefile  —  Laboratorio 4: API de Hilos
# =============================================================
CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -g -O2
LFLAGS  = -lpthread -lm

TARGETS = pi_s pi_p fibonacci

.PHONY: all clean run_pi_s run_pi_p run_fib

all: $(TARGETS)

pi_s: pi.c
	$(CC) $(CFLAGS) -o pi_s pi.c $(LFLAGS)

pi_p: pi_p.c
	$(CC) $(CFLAGS) -o pi_p pi_p.c $(LFLAGS)

fibonacci: fibonacci.c
	$(CC) $(CFLAGS) -o fibonacci fibonacci.c $(LFLAGS)

run_pi_s: pi_s
	./pi_s 2000000000

run_pi_p: pi_p
	./pi_p 2000000000 4

run_fib: fibonacci
	./fibonacci 15

clean:
	rm -f $(TARGETS) *.o