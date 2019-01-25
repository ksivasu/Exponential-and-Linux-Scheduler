/* kill.c - kill */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <q.h>
#include <stdio.h>
#include <lock.h>

/*------------------------------------------------------------------------
 * kill  --  kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
SYSCALL kill(int pid)
{

	STATWORD ps;    
	struct	pentry	*pptr;		/* points to proc. table for pid*/
	int	dev;

	disable(ps);
	if (isbadpid(pid) || (pptr= &proctab[pid])->pstate==PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	if (--numproc == 0)
		xdone();
	//kprintf("\n %d", pid);
	dev = pptr->pdevs[0];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->pdevs[1];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->ppagedev;
	if (! isbaddev(dev) )
		close(dev);
	
	send(pptr->pnxtkin, pid);
	//kprintf("Entered kill - pid %d",pid);
	freestk(pptr->pbase, pptr->pstklen);
	switch (pptr->pstate) {

	case PRCURR:	process_holding_lock(pid);
			pptr->pstate = PRFREE;	/* suicide */
			//kprintf("Entered case PRCURR");
			resched();
			//kprintf("Entered case PRCURR");

	case PRWAIT:	semaph[pptr->psem].semcnt++;
	
	case PRREADY:	process_holding_lock(pid);
			dequeue(pid);
			pptr->pstate = PRFREE;
			break;

	case PRSLEEP:
	case PRTRECV:	process_holding_lock(pid);
			unsleep(pid);
	
	case LRWAIT :	dequeue(pid);
             		//pptr->pstate = PRFREE;
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
			pptr->pstate = PRFREE;			
						/* fall through	*/
	default:	pptr->pstate = PRFREE;
	}
	//kprintf("\nLeaving Kill");
	restore(ps);
	return(OK);

}


void process_holding_lock(int curr)
{

	// Check if  the process holds a lock
	//kprintf("\n Process Holding Lock");
	int present;
	int k=0;
	while(proctab[curr].locks_held[k] != -1)
	{
	present = 1;
	k++;
	}

	// Do the following only if the process holds a lock 
	if(present == 1)
	{
	//Accumulate locks tp be changed
	int locks[NLOCKS] = {0};

	int i=0;
	while(proctab[curr].locks_held[i] != -1)
	{
		locks[i] = proctab[curr].locks_held[i];
		i++;
	}

	// Change the locks held tab of the process

	i =0;
	while(proctab[curr].locks_held[i] != -1)
	{
		proctab[curr].locks_held[i] = -1;
		i++;
	}

	// Change the process held tab of the lock
	int j=0;
	while(locks[j] != 0)
	{
		k=0;
		while(locarr[locks[j]].proc_queue[k] != -1)
		{
			if(locarr[locks[j]].proc_queue[k] == curr)
				locarr[locks[j]].proc_queue[k] = -1;
			k++;
		}
	j++;
	}

	}
//kprintf("\nOut of process holding lock");
}


/*void max_prio_inwaitq(int loc, int curr)
{
struct lentry *lptr;
lptr = &locarr[loc];
int curr_temp = locarr[loc].lqhead;
          curr = q[curr_temp].qnext;
          int max = 0;

        while(&q[curr] != &q[locarr[loc].lqtail])
          {
            if(proctab[curr].pinh = 0)
            {
             if(max <= proctab[curr].pprio)
              max = proctab[curr].pprio;
            }
            else
           {
             if(max <= proctab[curr].pinh)
                 max = proctab[curr].pinh;
           }
           }
         lptr->prio_max = max;
}

void lock_prio_process(int proc)
{
int j=0;
int max = 0;
struct pentry *pptr;
          pptr = &proctab[proc];
       while(pptr->locks_held[j] != -1 )
          {
          // We check if the maximum priority of the lock descriptor's wait queue is greater than or equal to the max value.
          if(locarr[pptr->locks_held[j]].prio_max >= max)
          {
          max = locarr[pptr->locks_held[j]].prio_max;
          }
          j++;
          }
          if(pptr->pprio < max)
          {
          pptr->pinh = max;
          }
          else
          pptr->pinh = 0;
}
*/

