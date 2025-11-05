#include <cstdio>

const int H = 17;
const int W = 17;

// '.' - пусто, '#' - граница, '+' - цвет заливки
char picture[H][W + 1] = {
    ".................",
    ".....###.###.....",
    "....#..###..#....",
    "...#......+..#...",
    "..#...........#..",
    ".#.....###.....#.",
    ".#....#...#....#.",
    ".#....#...#....#.",
    ".#....#...#....#.",
    ".#....#...#....#.",
    ".#....#...#....#.",
    ".#.....###.....#.",
    ".#.............#.",
    ".#.............#.",
    ".#.............#.",
    ".###############.",
    "................."
};

void print_picture() {
    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            char c = picture[y][x];

            if (c == '#') {
                // граница - красный фон
                printf("\x1b[41m  \x1b[0m");
            } else if (c == '+') {
                // заливка - зелёный фон
                printf("\x1b[42m  \x1b[0m");
            } else {
                // фон - просто пробелы
                printf("  ");
            }
        }
        printf("\n");
    }
}

void fill(int x, int y, char border_color, char the_color) {
    if (x < 0 || x >= W || y < 0 || y >= H) {
        return;
    }

    char c = picture[y][x];

    if (c != border_color && c != the_color) {
        picture[y][x] = the_color;

        fill(x - 1, y, border_color, the_color);
        fill(x + 1, y, border_color, the_color);
        fill(x, y - 1, border_color, the_color);
        fill(x, y + 1, border_color, the_color);
    }
}

int main() {
    int start_x = -1;
    int start_y = -1;

    printf("Before fill:\n");
    print_picture();

    // ищем единственный плюсик
    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            if (picture[y][x] == '+') {
                start_x = x;
                start_y = y;
            }
        }
    }

    if (start_x != -1) {
        picture[start_y][start_x] = '.';
        fill(start_x, start_y, '#', '+');
    }

    printf("\nAfter fill:\n");
    print_picture();

    return 0;
}
