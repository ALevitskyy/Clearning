#include <stdio.h>

void insertion(int a[], int N){
  int i,j,v;
  for(i=2;i<=N;i++){
    v = a[i]; j=i;
    while (a[j-1]>v){
      a[j] = a[j-1];
      j--;
    }
    a[j] = v;
  }
}


int main(void) {
    int a[100], N = 1, i;
    a[0] = -1;

    printf("Enter numbers (end with EOF, e.g., Ctrl+D on Unix or Ctrl+Z on Windows):\n");
    while (scanf("%d", &a[N]) != EOF) {
        N++;
    }

    insertion(a, N);


    printf("Sorted numbers:\n");
    for (i = 1; i < N; i++) {
        printf("%d ", a[i]);
    }
    printf("\n");

    return 0;
}
