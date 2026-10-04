#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_N 1000000

static uint8_t memo[MAX_N+1] = {0};

// checks all vals from 3 to sqrt(n) --> O(sqrt(n)) time complexity, O(1) space
bool is_prime(uint32_t n){
  if (n <= MAX_N && memo[n] != 0) return memo[n] == 1;
  // prime means that if n = a * b, then either a or b is 1 and the other is n

  // if even or less than 2, it is not prime
  if ((n > 2 && (n & 1) == 0) || n  < 2) return false;

  // if test val over sqrt(n), then if n % test == 0, n / test = a, a < test
  for (uint32_t test = 3; (uint64_t)test * test <= n; ++test){
    if ((test & 1) == 0) continue; // skip even test divisors
    if (n % test == 0) return false;
  }
  return true;
}

// for testing
bool slow_is_prime(uint32_t n){
  if (n < 2) return false;
  for (uint32_t test = 2; test < (n/2)+ 1; ++test){
    if (n % test == 0) return false;
  }
  return true;
}

void precompute(void){
  // precomputing requires MAX_N + 1 bytes memory and time complexity of O(MAX_N);
  // hot path calcs are O(1) for n <= MAX_N and O(sqrt(n)) else
  memo[0] = 2;
  memo[1] = 2;
  for (uint32_t i = 2; i <= MAX_N; ++i){
    if (memo[i] != 0) continue;
    memo[i] = 1;
    if (i>1){
      uint32_t mult = 2;
      uint32_t res = i * mult;
      while (res <= MAX_N){
        memo[res] = 2;
        mult++;
        res = i * mult;
      }
    }
  }
}

int main(void){
  precompute();

  // test up to MAX_N + 4 (fallback)
  // takes a long time to run
  for (uint32_t x = 0; x < MAX_N + 5; ++x){
    assert(is_prime(x) == slow_is_prime(x));
  }
  
  puts("ALL TEST CASES PASSED\n");

}