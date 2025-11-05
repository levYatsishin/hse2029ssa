#include <iostream>
#include <cstdio>

int Ackerman(int m, int n) {
    if (m == 0) {
        return n + 1;
    } else if (n == 0) {
        return Ackerman(m - 1, 1);
    } else {
        return Ackerman(m - 1, Ackerman(m, n - 1));
    }
}

int main() {
    int m;
    int n;

    printf("Введите m и n: ");
    std::cin >> m >> n;

    int result = Ackerman(m, n);

    printf("A(%d, %d) = %d\n", m, n, result);

    return 0;
}
