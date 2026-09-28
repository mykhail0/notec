#include <assert.h>
#include <pthread.h>
#include <stdint.h>

// Interface between C and Assembly.
uint64_t notec(uint32_t n, char const* calc);
int64_t debug(uint32_t n, uint64_t* stack_pointer);

// We want to start all calculations as close to simultaneous if possible.
volatile unsigned wait = 1;

// Start at most one calc_1 and an even number of calc_2.
static const char calc_1[] = "6N8ZXab=12-+3*~FFF&cDe09|g";
static const char calc_2[] = "nY1^W";
static const uint64_t result_1 = (~((0xab - 0x12) * 3) & 0xfff) | 0xcde09;

// debug is called exclusively inside calc_1 to check the correctness.
int64_t debug(uint32_t n, uint64_t* stack_pointer) {
  assert(n == N - 1 && (n & 1) == 0);
  assert(*stack_pointer == result_1);

  // Remove the result from the stack.
  return 1;
}

void* thread_routine(void* data) {
  uint32_t n = *(uint32_t*)data;
  const char* calc;

  if (n == N - 1 && (n & 1) == 0)
    calc = calc_1;  // At most in one thread.
  else
    calc = calc_2;  // In even number of threads.

  while (wait);

  uint64_t result = notec(n, calc);

  if (n == N - 1 && (n & 1) == 0)
    assert(result == 6);
  else
    assert(result == (n ^ 1));

  return NULL;
}

int main() {
  pthread_t tid[N];
  uint32_t i, n[N];

  for (i = 0; i < N; ++i) {
    n[i] = i;
    assert(0 == pthread_create(&tid[i], NULL, &thread_routine, (void*)&n[i]));
  }

  wait = 0;

  for (i = 0; i < N; ++i) assert(0 == pthread_join(tid[i], NULL));

  return 0;
}
