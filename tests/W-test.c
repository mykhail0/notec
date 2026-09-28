#include <assert.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef N
#define N 10
#endif

// NIE ODPALAĆ Z N > 10 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

// Interfejs między C a Asemblerem
uint64_t notec(uint32_t n, char const *calc);
int64_t debug(uint32_t n, uint64_t *stack_pointer);

#define BUFF_SIZE 10000

static char calc_0[BUFF_SIZE];
static const char calc_n[] = "n0W";

// jakiś debug
int64_t debug(uint32_t n, uint64_t *stack_pointer) {
    (void) n;
    (void) stack_pointer;
    return 0;
}

void* thread_routine(void *data) {

    uint32_t n = *(uint32_t*)data;
    if(notec(n, calc_n) != n-1) {
        printf("ERROR IN THREAD %u\n", n);
        exit(1);
    }
    return NULL;
}

int main () {
    printf("W-test start\n");
    pthread_t tid[N];
    uint32_t i, n[N];



    for(i = 0; i < N; i++) {
        calc_0[2*i] = '0' + N - i - 1;
        calc_0[2*i+1] = '=';
    }

    for (int i = N; i < 2 * N - 1; i++) {
        calc_0[2*i] = 'X';
        calc_0[2*i+1] = 'W';
    }
    calc_0[4 * N - 2] = 0;
    for (i = 1; i < N; ++i) {
        n[i] = i;
        assert(0 == pthread_create(&tid[i], NULL, &thread_routine, (void*)&n[i]));
    }

    if(notec(0, calc_0) != N - 1) {
        printf("ERROR IN MAIN\n");
        exit(1);
    }

    for (i = 1; i < N; ++i)
        assert(0 == pthread_join(tid[i], NULL));
    printf("W-test passed\n");
    return 0;
}
