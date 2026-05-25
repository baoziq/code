#include <stdio.h>

int func(int num) {
    for (int i = 0; i < 10; i++) {
        num += i;
    }
    return num;
}
int main() {
    int num = 0;
    int sum = func(num);
    printf("sum: %d", sum);
    return 0;
}
