#include <stdio.h>
int gcd(int u, int v) {
    if (v == 0)
        return u;
    return gcd(v, u % v);
}

int main(void){
  int x,y;
  while (scanf("%d %d", &x, &y) != EOF){
    if (x>0 && y>0)
      printf("%d %d %d\n", x, y, gcd(x,y));
  }
}
