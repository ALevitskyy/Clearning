#include <stdio.h>

void insertion(int a[], int *p[], int N){
  int i, j, *v;
  for(i=0; i<=N; i++){
    p[i] = &a[i];
  }
  for(i=2;i<=N;i++){
    v = p[i]; j=i;
    while (*p[j-1]>*v){
      p[j] = p[j-1];
      j--;
    }
    p[j] = v;
  }
}


int main(void) {
    int a[100], *p[100], N = 1, i;
    a[0] = -1;

    printf("Enter numbers (end with EOF, e.g., Ctrl+D on Unix or Ctrl+Z on Windows):\n");
    while (scanf("%d", &a[N]) != EOF) {
        N++;
    }

    insertion(a, p, N);


    printf("Sorted numbers:\n");
    for (i = 1; i < N; i++) {
        printf("%d ", *p[i]);
    }
    printf("\n");

    return 0;
}
