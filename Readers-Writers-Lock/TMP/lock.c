#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>
#include <lock.h>
#include <sleep.h>
#include <i386.h>

/*------------------------------------------------------------------------
 *  * lock  --  make current process wait on a lock
 *   *------------------------------------------------------------------------
 *    */
int lock(int loc, int type, int priority)
{
	
	//kprintf("\n .....Entered lock.c");
	STATWORD ps;
	struct  lentry  *lptr;
	lptr = &locarr[loc];
	struct  pentry  *pptr;
	extern unsigned long ctr1000;
	disable(ps);
	if (isbadsem(loc)){ 
		restore(ps);
		//kprintf("Shouldnt enter here");
		return(SYSERR);
	}
	/* Calclulate the highest priority writer process waiting for the lock. */
	pptr=&proctab[currpid];
	int curr = lptr->lqhead;
	int temp = -1;
	while(&q[curr] != &q[lptr->lqtail])
	{
		if(q[curr].qkey >= temp && q[curr].type == WRITE)
		{	
			temp=q[curr].qkey;
		} 
		curr = q[curr].qnext;
	}	
	/* temp has the highest priority writer process waiting for the lock. */
	//int count = --(lptr->loccnt);
	/*
	kprintf("\nThe value of locarr[loc].ltype is %d", locarr[loc].ltype);
	kprintf("\n The value of lock is %d", loc);
	kprintf(" \t The name of process calling the lock is %s", proctab[currpid].pname);
	kprintf("\t The highest writer process priority is %d", temp); 
	*/
	//kprintf("\t lock type : %d \t type input : %d \t priority : %d \t max priority of writer in queue :%d", locarr[loc].ltype, type, priority, temp); 
	if((locarr[loc].ltype != READ && locarr[loc].ltype != WRITE)|| (locarr[loc].ltype == READ && type == READ && priority >= temp )) // This means that there are no readers / writers on the lock and the lock can be acquired.
	{
		//kprintf("\n Going to acquire the lock");
		int i =0;
		//Update the process queue in a lock descriptor and Update the locks held by the process.

		while(locarr[loc].proc_queue[i] != -1)
		{
			i++;
		}

		locarr[loc].proc_queue[i]=currpid;
		locarr[loc].lstate = LUSED;
		//kprintf("\nThe assigned value of locarr[loc].proc_queue[i] is %d",locarr[loc].proc_queue[i]); 
		i=0;
		while(pptr->locks_held[i] != -1)
		{
			i++;
		}
		//kprintf("\n pptr->name %s", pptr->pname);
		pptr->locks_held[i]=loc;
		//kprintf("\t  i %d pptr->locks_held[i] %d", i, pptr->locks_held[i]);
		// Update the type of lock for the loc descriptor as READ or WRITE  
		if(type == READ)
		{
			//kprintf("\nIncremening reader count");
			locarr[loc].ltype = READ;	 
			locarr[loc].reader_count++;
			//kprintf("\nSetting the type as READ");
		}
		//kprintf("\nThe value of reader count is %d", locarr[loc].reader_count);
		else if(type == WRITE)
		{
			locarr[loc].ltype = WRITE;
			//kprintf("Setting the type as WRITE");
		}	
		i=0;
		int max=-1;
		int waitq_tem=locarr[pptr->locks_held[i]].lqhead;
		int waitq_pos = q[waitq_tem].qnext;
		while(pptr->locks_held[i] != -1 )
		{
			// We check if the maximum priority of the loc descriptor's wait queue is greater than or equal to the max value.
			if(locarr[pptr->locks_held[i]].prio_max >= max)
			{
				max = locarr[pptr->locks_held[i]].prio_max;
			}   
			i++;
		}

		if(pptr->pprio < max)
		{
			pptr->pinh = max;
		}
		else 
			pptr->pinh = 0;
	}
	//kprintf("\t Entered before else if");
	//kprintf("\t %d", locarr[loc].ltype);
	//kprintf("\t %d", type
	else if(locarr[loc].ltype == WRITE || (locarr[loc].ltype == READ && type == WRITE) || (locarr[loc].ltype == READ && priority < temp))
	{
		//kprintf("\t ...........entered else if to add the process to queue..........");
		(pptr = &proctab[currpid])->pstate = LRWAIT;
		
		pptr->ploc = loc;
		//struct lentry *lptr;
		lptr = &locarr[loc];
		insert(currpid,lptr->lqhead, priority);
		pptr->lwaitret = OK;
		//locarr[loc].ltype = type;
		q[currpid].type = type;
		q[currpid].time = ctr1000;
		int curr_temp = locarr[loc].lqhead;
		curr = q[curr_temp].qnext;
		int max = 0;
		//Finding the maximum of process priority of processes in wait queue and we update them   
		while(&q[curr] != &q[lptr->lqtail])
		{
			//kprintf("\n Entered while");
			if(proctab[curr].pinh == 0)
			{
			//kprintf("\n Entered if");
				if(max <= proctab[curr].pprio)
					max = proctab[curr].pprio;
			}
			else
			{
			//kprintf("\n Entered else");
				if(max <= proctab[curr].pinh)
					max = proctab[curr].pinh;	
			}
			curr = q[curr].qnext;  
		}
		lptr->prio_max = max;
		//kprintf("\n Lock is %d",loc); 
		//kprintf("\n The max priority is : %d", max);
		// We Update the lock priority of all processes held by the lock ( As we have a new process in the wait queue)
		//kprintf("\n Exited first while");
		int i =0;
		while(lptr->proc_queue[i] != -1)
		{
			//kprintf("\n Before updating the inherited priority:");
			//kprintf("\t %d", lptr->proc_queue[i]);
			pptr = &proctab[lptr->proc_queue[i]];    
			int l =0;
			while(pptr->locks_held[l] != -1 )
			{
				// We check if the maximum priority of the loc descriptor's wait queue is greater than or equal to the max value.
				if(locarr[pptr->locks_held[l]].prio_max >= max)
				{
					max = locarr[pptr->locks_held[l]].prio_max;
				}
				l++;
			}
			if(pptr->pprio < max)
			{
				//kprintf("\n .........Entered updating part");
				//kprintf("\t %d", max);
				pptr->pinh = max;
			}
			else
				pptr->pinh = 0;
			//kprintf("Entered while");
			i++;
		}
		//kprintf("\nExited last while");
		resched();
		//locarr[loc].ltype = type;
		restore(ps);
		return pptr->lwaitret;	
	}

	/*
	   if(locarr[loc].ltype != READ || type != READ || priority < temp) // Check whether we have to put locarr[loc].type != READ.
	   {
	   if ((count) < 0) {
	   (pptr = &proctab[currpid])->pstate = LRWAIT;
	   pptr->ploc = loc;
	   insert(currpid,lptr->lqhead, priority);
	   pptr->lwaitret = OK;
	   resched();
	   locarr[loc].ltype = type;
	   restore(ps);
	   return pptr->lwaitret;
	   }
	   }

	   locarr[loc].ltype = type;
	   restore(ps);
	   return(OK);
	   */
}


