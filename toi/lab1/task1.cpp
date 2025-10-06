#include <cstdio>

int main() {
  float S;
  unsigned char N = 11;

  S = 1.0f / N;

  int iters = 0;
  while (S <= 1.0f) {
    printf("iters=%d, S=%.9E\n", iters, S);
    S = S + 1.0f / N;
    ++iters;
  }

  printf("iters=%d, S=%.9E\n", iters, S);
}
