#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sched.h>
#include <lab1.h>

scheduler_class = 0;

void setschedclass(int class_value)
{
scheduler_class = class_value;
}

int getschedclass(){

return(scheduler_class);
}

















































