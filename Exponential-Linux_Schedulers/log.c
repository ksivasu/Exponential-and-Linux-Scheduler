#include<stdio.h>
#include<stdlib.h>

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
int n = 20;
double result=0;
double result_10;
double y;
int i =0;
double val = (x - 1)/(x+1);
for(; i < n; i++)
{
double p = 2*i + 1;
double p1 = 1/p; 
result = result + p1*pow1(val,p); 
//printf("The value of result is %f \n", result);
}
result_10=result*2*0.43429;
return result_10;
}

double expdev(double lambda) {
    srand(time(0));
    double dummy;
    do
        dummy= (double) rand() / RAND_MAX;
    while (dummy == 0.0);
    //printf("The value of dummy is %f", dummy);
    //printf("The value of log1 is %f", log1(dummy));
    return -log1(dummy) / lambda;
}

int main()
{
double val = expdev(0.1);
printf("The value is expdev  is %f \n", val);
double dummy1;
dummy1 = (double) rand() / RAND_MAX;
//printf("The value of dummy1 is %f", dummy1);
return 0;
}






