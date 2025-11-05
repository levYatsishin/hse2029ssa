#include <iostream>
#include <cstdio>

int McCarthy(int n) {
    if (n > 100) {
        return n - 10;
    } else {
        return McCarthy(McCarthy(n + 11));
    }
}

int main() {
    int n;

    printf("Введите n: ");
    std::cin >> n;

    int result = McCarthy(n);

    printf("M(n) = %d\n", result);

    return 0;
}
