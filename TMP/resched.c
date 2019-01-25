/* resched.c  -  resched */
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include<lab1.h>
#include <sched.h>
unsigned long currSP;	/* REAL sp of current process */
extern int ctxsw(int, int, int, int);
/*-----------------------------------------------------------------------
 * resched  --  reschedule processor to highest priority ready process
 *
 * Notes:	Upon entry, currpid gives current process id.
 *	`	Proctab[currpid].pstate gives correct NEXT state for
 *			current process if other than PRREADY.
 *------------------------------------------------------------------------
 */
int resched()
{
	//kprintf("Entered resched");
	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */

	/* no switch needed if current process priority higher than next*/



	//EXPDEV Implementation
	int value = getschedclass();
	if (value == EXPDISTSCHED)
	{
	int val = (int) expdev(0.1);  //Get the Exp Random Value
	int ptr;
	int next;
	//struct qent *ptr;
	struct qent *ptr1;
	struct qent *ptr2;
	struct qent *ptrp;
	optr = &proctab[currpid];
	
	// Forcing Context Switch
	if (optr->pstate == PRCURR) 
	{
                optr->pstate = PRREADY;
                insert(currpid,rdyhead,optr->pprio);
        }
	
	ptr = q[rdyhead].qnext;
	
	// Finding the minimum value with priority greater than random value obtained.
	while((q[ptr].qkey < val) && (ptr < NPROC))
	{
		ptr = q[ptr].qnext;
	}
		
	if (ptr >= NPROC)		//Handling the case where there are no processes,and null process has to be run or the highest priority element in the queue is lesser than the 
					//random value generated
		next = getlast(rdytail);
	else 
		next = ptr;
	currpid = dequeue(next);
	nptr = &proctab[currpid];
      	nptr->pstate = PRCURR;	
			/* mark it currently running    */
     	#ifdef RTCLOCK
      	      preempt = QUANTUM;              /* reset preemption counter     */
      	#endif
	ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
     	return (OK);
	}

	else if(value == LINUXSCHED)
	{
	int epoch_f = 0;
	int max = 0;
	int thispid = 0;
	int old_prio;
	struct pentry *thisproc;
	optr=&proctab[currpid];
	int i =0;
	if(preempt <= 0)
	{
	preempt = 0;
	optr->goodness = 0;
	optr->counter = 0;
	}
	old_prio = optr->goodness - optr->counter;
	optr->goodness = old_prio + preempt;  //Updating the goodness value of the previous process
	optr->counter=preempt;     				    // Updating the counter value to the left over preempt for previous process

	while(i < NPROC)
	{
		thisproc = &proctab[i];
		//kprintf("entered 1st loop");
		if(thisproc->pstate == PRREADY || thisproc->pstate == PRCURR)
			{
			if(thisproc->counter > 0)
			{
			epoch_f=1;                  //Signals an old epoch when there is counter value left over for valid processes.
			break;
			}
			}
	i++;
          		}
	
	if(epoch_f == 0)			    //New Epoch, Therefore setting the values of quantum,conuter and goodness. 
		{
	for (i = 0; i < NPROC; i++)
		{	
		thisproc = &proctab[i];
		if (thisproc->counter > 0){
		thisproc->quantum = (thisproc->counter/2) + thisproc->pprio;
			thisproc->counter = thisproc->quantum;
		}
	
		else{
			thisproc->quantum = thisproc->pprio;
			thisproc->counter = thisproc->quantum;
			}
			thisproc->goodness = thisproc->counter + thisproc->pprio;
			
		}		
		}	
					
	for(i = 0; i < NPROC ; i++)		      //Finding the maximum goodness of all the processes in the queue.
		{
		thisproc=&proctab[i];
		if((thisproc->pstate == PRREADY)&&(max < thisproc->goodness))
		{
		max = thisproc->goodness;
		thispid = i;
		}
		}
		
	int j;					      //Checking if there are any valid processes to be run
	int pid_flag=0;
	for(j=0; j < NPROC; j++)
		{
		thisproc = &proctab[j];
		if(thisproc->pstate == PRREADY || thisproc->pstate == PRCURR)

			{
				pid_flag=1;
				break;
			}
		}
		if(pid_flag == 0)
		{
			thispid=0;
                       
		}
		 
		if((optr->pstate == PRCURR) && (max <= optr->goodness) 	) //Running the old process until the counter becomes zero (when max goodness is less than or equal to old process' goodness)							     
		{	    						     
		preempt = optr->counter;
		return OK;
		}
		
		else
		{
		//Forcing Context Switch
		if (optr->pstate == PRCURR){
				optr->pstate = PRREADY;
				insert(currpid,rdyhead,optr->pprio);
			}
		
			currpid=dequeue(thispid);
			nptr = &proctab[thispid];
			nptr->pstate = PRCURR;
			#ifdef RTCLOCK
			preempt = nptr->counter; 
			#endif
			ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
			return OK;
		}

		}	
	else 								//Running the default scheduler when the sched class flags are not set.
	{
	if ( ( (optr= &proctab[currpid])->pstate == PRCURR) &&
	   (lastkey(rdytail)<=optr->pprio)) {
		return(OK);
	}
	//kprintf("entered default");	
	if(optr->pstate == PRCURR)
	{
	optr->pstate = PRREADY;
	insert(currpid,rdyhead,optr->pprio);
	}
	
	nptr = &proctab[ (currpid = getlast(rdytail)) ];
	nptr->pstate = PRCURR;			
	#ifdef RTCLOCK
	preempt = QUANTUM;		 
	#endif
	 
	ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
	return OK;
     }	
     

    
}



