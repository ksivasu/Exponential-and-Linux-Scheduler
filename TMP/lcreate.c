#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>
#include <lock.h>

LOCAL int newloc();

SYSCALL lcreate()
{
	STATWORD ps;
	int loc;
	disable(ps);
	if((loc=newloc()) == SYSERR)
	{
	   restore(ps);
	   return(SYSERR);
	}
	//locarr[loc].loccnt = 1;
	locarr[loc].lstate = LFREE;
	restore(ps);
	return(loc);
}


LOCAL int newloc()
{
int loc;
int i;
for(i=0; i < NLOCKS ; i++)
	{
	loc = nextloc--;
	if(nextloc < 0)
		nextloc = NLOCKS-1;
	if(locarr[loc].lstate == LFREE)
	{
	locarr[loc].lstate = LUSED;
	return(loc);
	}
}
return(SYSERR);
}























