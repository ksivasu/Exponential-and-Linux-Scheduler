
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>

unsigned long currSP;	/* REAL sp of current process */
extern int ctxsw(int, int, int, int);
/*-----------------------------------------------------------------------
 * resched  --  reschedule processor to highest priority ready process
 *
 * Notes:	Upon entry, currpid gives current process id.
 *		Proctab[currpid].pstate gives correct NEXT state for
 *			current process if other than PRREADY.
 *------------------------------------------------------------------------
 */
int resched()
{
	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */

	/* no switch needed if current process priority higher than next*/
	//kprintf("\nEntered resched");	
	//kprintf("\tPriority of currpid %d", getprio(currpid));
	//kprintf("\tPriority of lastkey %d", lastkey(rdytail)); 
	if ( ( (optr= &proctab[currpid])->pstate == PRCURR) &&
	   (lastkey(rdytail)<optr->pprio)) {
		//kprintf("\n returning OK, no context switch");
		return(OK);
	}
	
	/* force context switch */

	if (optr->pstate == PRCURR) {
		optr->pstate = PRREADY;
		insert(currpid,rdyhead,optr->pprio);
	}

	/* remove highest priority process at end of ready list */
	/*kprintf("\n The Value of Ready Queue is:");
	int test = q[rdyhead].qnext;
	while(&q[test] != &q[rdyhead]) 
	{
	//kprintf("%d", rdyhead);
	kprintf("\t %d", test);
	test = q[test].qnext;
	}*/
	nptr = &proctab[ (currpid = getlast(rdytail)) ];
	nptr->pstate = PRCURR;		/* mark it currently running	*/
#ifdef	RTCLOCK
	preempt = QUANTUM;		/* reset preemption counter	*/
#endif
	
	//kprintf("\n Old Process Name %s", optr->pname);
	//kprintf("\n New Process Name %s", nptr->pname);
	ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
	//kprintf("\n After Context Switch");
	//kprintf("\t New Process Name %s", nptr->pname);
	//kprintf("\n Leaving Resched");
	/* The OLD process returns here when resumed. */
	return OK;
}

