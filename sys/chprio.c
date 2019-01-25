/* chprio.c - chprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include <lock.h>
/*------------------------------------------------------------------------
 * chprio  --  change the scheduling priority of a process
 *------------------------------------------------------------------------
 */
SYSCALL chprio(int pid, int newprio)
{
	STATWORD ps;    
	struct	pentry	*pptr;

	disable(ps);
	if (isbadpid(pid) || newprio<=0 ||
	    (pptr = &proctab[pid])->pstate == PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	if(pptr->pinh == 0)
		pptr->pprio = newprio;
	else
		pptr->pinh = newprio; 
	//Updating the locks max prio and other processes' pinh if necessary.
	 max_prio_inwaitq(pptr->ploc, pid);
         int i = 0;
         struct lentry *lptr;
         lptr=&locarr[pptr->ploc];
         while(lptr->proc_queue[i] != -1)
         {
          //kprintf("process held by lock : %d", lptr->proc_queue[i]);
         lock_prio_process(lptr->proc_queue[i]);
         i++;
         }

	restore(ps);
	return(newprio);
}
