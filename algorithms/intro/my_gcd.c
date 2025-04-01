#include <stdio.h>
int gcd(int u, int v) {
    if (v == 0)
        return u;
    return gcd(v, u % v);
}

int main(void){
  int x,y;
  printf("Enter two numbers followed by space \n");
  while (scanf("%d %d", &x, &y) != EOF){
    if (x>0 && y>0)
    printf("The GCD of %d and %d is %d\n", x, y, gcd(x,y));
    printf("Enter two numbers followed by space \n");
  }

}
