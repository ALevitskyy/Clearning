#include <stdio.h>

void selection(int a[], int N) {
  int i, j, min, t;
  for (i=0; i<N-1; i++){
    min = i;
    for (j=i+1; j< N;j++){
      if (a[j]<a[min]) min=j;

    }
    t = a[min]; a[min] = a[i]; a[i] = t;
  }
}

int main(void) {
    int a[100], N = 0, i;

    printf("Enter numbers (end with EOF, e.g., Ctrl+D on Unix or Ctrl+Z on Windows):\n");
    while (scanf("%d", &a[N]) != EOF) {
        N++;
    }

    selection(a, N);

    printf("Sorted numbers:\n");
    for (i = 0; i < N; i++) {
        printf("%d ", a[i]);
    }
    printf("\n");

    return 0;
}
