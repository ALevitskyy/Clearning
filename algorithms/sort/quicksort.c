#include <stdio.h>

void quicksort(int a[], int l, int r){
  int v,i,j,t;
  if(r>l){
    v = a[r]; i=l-1; j=r;
    for(;;){
      while (a[++i]<v);
      while (a[--j]>v);
      if(i>=j) break;
      t = a[i]; a[i] = a[j]; a[j] = t;
    }
    t = a[i]; a[i] = v; a[r] = t; // place pivot on the correct position
    quicksort(a,l,i-1);
    quicksort(a,i+1,r);
  }
}

int main(void) {
    int a[100], N = 1, i;
    a[0] = -1;

    printf("Enter numbers (end with EOF, e.g., Ctrl+D on Unix or Ctrl+Z on Windows):\n");
    while (scanf("%d", &a[N]) != EOF) {
        N++;
    }

    quicksort(a, 1, N);

    printf("Sorted numbers:\n");
    for (i = 1; i < N; i++) {
        printf("%d ", a[i]);
    }
    printf("\n");

    return 0;
}
