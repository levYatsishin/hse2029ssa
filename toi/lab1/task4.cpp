#include <cstdio>
#include <cmath>

int calculate_float() {
  float Sn = 1.0f, Se = 1.0f, Se0 = 1.0f, A = 1.0f; 
  float x, e; 
  int n;

  printf("Введите x: ");
  scanf("%f", &x);
  printf("Введите n: ");
  scanf("%d", &n);
  printf("Введите e (точность вычислений): ");
  scanf("%f", &e);

  // а) Sn: сумма по заданному n 
  for (int k = 1; k <= n; ++k) {
    A *= ( - (x * x) / ( (2.0f * k) * (2.0f * k - 1.0f) )
           * ( (2.0f * k * k + 1.0f) / ( 2.0f * (k - 1) * (k - 1) + 1.0f ) ) );
    Sn += A;
  }
  printf("\nSn = %g, n = %d, A_N=%gf\n", Sn, n, A);

  // б) Se: сумма до заданной точности e 
  A = 1.0f;
  int i = 1;
  while (abs(A) >= e) {
    A *= ( - (x * x) / ( (2.0f * i) * (2.0f * i - 1.0f) )
           * ( (2.0f * i * i + 1.0f) / ( 2.0f * (i - 1) * (i - 1) + 1.0f ) ) );
    Se += A;
    ++i;
  }
  printf("Количество слагаемых для Se: %d (точность до e = %gf)\n", i, e);
  printf("Se = %g\n", Se);

  // в) e: сумма с точностью до машинного нуля 
  A = 1.0f;
  i = 1;
  while (abs(A) > 0.0f) {
    A *= ( - (x * x) / ( (2.0f * i) * (2.0f * i - 1.0f) )
           * ( (2.0f * i * i + 1.0f) / ( 2.0f * (i - 1) * (i - 1) + 1.0f ) ) );
    Se0 += A;
    ++i;
  }
  printf("Количество слагаемых для Se0: %d (точность до e = 0.0f)\n", i);
  printf("Se0 = %g\n", Se0);

  return 0;
}

int calculate_double(void) {
  double Sn = 1.0, Se = 1.0, Se0 = 1.0, A = 1.0;
  double x, e;
  int n;

  printf("Введите x: ");
  scanf("%lf", &x);
  printf("Введите n: ");
  scanf("%d", &n);
  printf("Введите e (точность вычислений): ");
  scanf("%lf", &e);

  // а) Sn: сумма по заданному n 
  for (int k = 1; k <= n; ++k) {
    A *= ( - (x * x) / ((2.0 * k) * (2.0 * k - 1.0))
           * ((2.0 * k * k + 1.0)
              / (2.0 * (k - 1) * (k - 1) + 1.0)) );
    Sn += A;
  }
  printf("\nSn = %g, n = %d, A_N = %g\n", Sn, n, A);

  // б) Se: сумма до заданной точности e 
  A = 1.0;
  int i = 1;
  while (abs(A) >= e) {
    A *= ( - (x * x) / ((2.0 * i) * (2.0 * i - 1.0))
           * ((2.0 * i * i + 1.0) / (2.0 * (i - 1) * (i - 1) + 1.0)) );
    Se += A;
    ++i;
  }
  printf("Количество слагаемых для Se: %d (точность до e = %g)\n", i, e);
  printf("Se = %g\n", Se);

  // в) e: сумма с точностью до машинного нуля 
  A = 1.0;
  i = 1;
  while (fabs(A) > 0.0) {
    A *= ( - (x * x) / ((2.0 * i) * (2.0 * i - 1.0))
           * ((2.0 * i * i + 1.0) / (2.0 * (i - 1) * (i - 1) + 1.0)) );
    Se0 += A;
    ++i;
  }
  printf("Количество слагаемых для Se0: %d (точность до e = 0.0)\n", i);
  printf("Se0 = %g\n", Se0);

  return 0;
}

int main(void)
{
  // calculate_float();
  calculate_double();

  return 0;
}
