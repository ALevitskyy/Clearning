#include <stdio.h>
#include <stdlib.h>

void sieve(int N) {
    int *a = malloc((N + 1) * sizeof(int));
    if (a == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    for (int i = 2; i <= N; i++) a[i] = 1;

    for (int i = 2; i * i <= N; i++) {
        if (a[i]) {
            for (int j = i * i; j <= N; j += i) {
                a[j] = 0;
            }
        }
    }

    for (int i = 2; i <= N; i++) {
        if (a[i]) printf("%4d ", i);
    }
    printf("\n");

    free(a);
}

int main(void) {
    int N;
    printf("Enter the value of N: ");
    if (scanf("%d", &N) != 1 || N < 2) {
        fprintf(stderr, "Invalid input\n");
        return 1;
    }

    sieve(N);
    return 0;
}
