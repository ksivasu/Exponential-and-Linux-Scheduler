
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

int ldelete(int loc)
{
	STATWORD ps;
	int pid;
	struct lentry *lptr;
	disable(ps);
	if(isbadloc(loc) || locarr[loc].lstate == LFREE)
	{
	    restore(ps);
	    return(SYSERR);
	}					
	lptr = &locarr[loc];
	lptr->lstate = LFREE;
	lptr->ltype = -1;
	lptr->reader_count = 0;
	int j=0;
	// Reinitializing the values of proc_queue to -1.
	while(j < NPROC)
	{
	   lptr->proc_queue[j] = -1;	
	   j++;
	}

	if(nonempty(lptr->lqhead)) {
		while( (pid = getfirst(lptr->lqhead)) != EMPTY)
		{
		proctab[pid].lwaitret = DELETED;
		// Find the lock in locks held and set the value to -1.
		int i =0;
		while(proctab[pid].locks_held[i] != loc)
		{
		i++;
		}
		proctab[pid].locks_held[i] = -1;
		dequeue(pid);
		ready(pid, RESCHNO);
		}
	resched();
        }
	restore(ps);
	return(OK);
}
