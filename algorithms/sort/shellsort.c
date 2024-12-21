#include <stdio.h>

void shellsort(int a[], int N){
int i,j,h,v;
for (h=1; h<=N/9; h=3*h+1) ;
for (; h>0; h/=3)
  for (i=h+1; i<=N; i+=1){
    v = a[i]; j=i;
    while (j>h && a[j-h]>v){
      a[j] = a[j-h];
      j-=h;
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

    shellsort(a, N);

    printf("Sorted numbers:\n");
    for (i = 1; i < N; i++) {
        printf("%d ", a[i]);
    }
    printf("\n");

    return 0;
}
