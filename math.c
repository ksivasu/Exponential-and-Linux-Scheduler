#include<stdio.h>
#include<stdlib.h>
//#include<conf.h>
/*#include<kernel.h>
#include<proc.h>
#include <lab1.h>
#include <sleep.h>*/

double pow1(double x, int y)
{
double res = 1;
int i=1;
for(; i <= y; i++)
{
res = res*x;
}
//printf("%f \n", res);
return res;
}

double log1(double x)
{
double y = (1-x);
int n = 20;
double result=0;
//double result_10;
int i =1;
//double val = (x - 1)/(x+1);
for(; i <= n; i++)
{
//double p = 2*i + 1;
//double p1 = 1/p;
result = result + (pow1(y,i)/(double)i);
//printf("The value of result is %f \n", result);
}
//result_10=result*2*0.43429;
return -(result);
}

double expdev(double lambda) {
//kprintf("The gettime is: %d\n", gettime());
//srand(1234);
//double value = (double) rand();
//kprintf("The value of rand is %d\n", (int)value); 
double dummy;
do
dummy= (double) rand() / RAND_MAX;
while (dummy == 0.0);
//kprintf("The value of dummy is %d\n", (int)dummy);
//printf("The value of log is %f\n", log1(0.3));
return -log1(dummy)/lambda;
}

int main()
{
int i, val;
int a=0;
int b=0;
int c=0;
srand(time(NULL));
for(i=0;i < 100000;i++)
{
val = (int) expdev(0.1);
//printf("\nVAL=%d",val);
//", val);
if(val > 0 && val <= 10)
	a++;
if(val > 10 && val <= 20)
        b++;
if(val > 20)
        c++;
}
printf("The ratio is %d:%d:%d",a,b,c);
return 0;
}















































































































