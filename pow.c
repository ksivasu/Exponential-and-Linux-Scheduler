#include <stdio.h>
int main ()
{
double x = 14;
int y = 5;
printf("%f\n",x);
double res = 1;
int i = 1;
for (; i <= y; i++)
{
res = res*x;
}
printf("%f\n",res);
}
