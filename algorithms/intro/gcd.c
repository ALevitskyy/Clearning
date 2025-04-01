#include <stdio.h>
int gcd(int u, int v) {
  int t;
  while (u>0){
    if (u<v){
      t = u; u=v; v=t;
    }
    u = u-v;
  }
  return v;
}
int main(void){
  int x,y;
  printf("Enter two numbers followed by space \n");
  while (scanf("%d %d", &x, &y) != EOF){
    if (x>0 && y>0)
      printf("The GCD of %d and %d is %d\n", x, y, gcd(x,y));
  }
  printf("Enter two numbers followed by space \n");
}
