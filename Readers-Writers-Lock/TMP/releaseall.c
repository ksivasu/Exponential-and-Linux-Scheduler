#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>
#include <lock.h>

int releaseall(int numlocks, int ldes1,...)
{
	//kprintf("\nEntered ReleaseAll");
	//kprintf("\nnumlocks is %d", numlocks);
	unsigned long *val1 = (unsigned long *)(&ldes1);
	unsigned long *val2 = (unsigned long *)(&ldes1);
	//int numch = 0;
	//hile(numch < numlocks)
	//{
	//kprintf("\n Argument is %d", *val1++);
	//numch++;
	//}
	//kprintf("\ncount is %d", numch);

	STATWORD ps;
	//sleep(10);
	extern unsigned long ctr1000;
	unsigned long *val = (unsigned long *)(&ldes1);
	disable(ps);
	int args[numlocks];
	int i=0;
	int ret_f=0;
	// Checking whether the locks released by the process are in the locks_held of the process.
	//kprintf("Input Locks are :");
	while(i < numlocks)
	{
		//kprintf("\nEntered first while");
		args[i]=*val++;  // Arguments gets populated here too.
		//kprintf("args[i] is %d", args[i]);
		int j=0;
		while(proctab[currpid].locks_held[j] != args[i])
		{
			j++;
		}
		if(proctab[currpid].locks_held[j] == args[i])
		{
			locarr[args[i]].lstate = LFREE;
			if(locarr[args[i]].ltype == READ)
				locarr[args[i]].reader_count--;
			//locarr[args[i]].ltype   = -1;
		}
		else
		{
			ret_f = 1;
		}
		i++;
	} 
	if(ret_f == 1)
	{
		return SYSERR;
	}

	/*i=0;
	//For the waiting list of the lock, accumulate all processes that have equal wait priorities
	*/
	//i=0;
	//int lock_proc[NLOCKS][NLOCKS];
	// Initializing the 2-d array to -1
	 /*if(nonempty(locarr[args[i]].lqhead))
	{
	 kprintf("..........Queue values for testing...  The queue values are :");
	 kprintf("\n Wrong i %d args[i] %d",i, args[i]);
                        int curr_tem = locarr[args[i]].lqhead;
			int curr = q[curr_tem].qnext;
                        while(&q[curr] != &q[locarr[args[i]].lqtail])
                        {
                        kprintf("\t %d", curr);
			kprintf("\tkey :\t %d", q[curr].qkey);
                        curr = q[curr].qnext;
                        }
	}*/
	i=0;
	//kprintf("\n Before checking non empty i args[i] %d, %d", i, args[i]); 
	while(i < numlocks)
	{
	//kprintf("\n.............Entered While................");
	if(nonempty(locarr[args[i]].lqhead))
	{
	//kprintf("\n ...... The queue is not empty .......");
	int count1=0;
	/*Recently Commented */
	while(count1 < NLOCKS)
	{
		//kprintf("\nEntered second while");
		int j=0;
		while(j < NLOCKS)
		{
			lock_proc[count1][j] = 0;
			j++;
		}
		count1++;
	}
	//while( i < numlocks)
	
		//args[i] = *val1++;
		//kprintf("\n Entered while..........");
		//kprintf("\n args value is %d",args[i]);
		// kprintf("\n Correct i, args[i] %d %d", i, args[i]);
		int curr_tem = locarr[args[i]].lqhead;
		int curr = q[curr_tem].qnext; // Points to the next element to head of waiting queue of the current lock ID ( args[i])
		int max = 0;
		int k =0;
		// Should not enter for the first test case.
		//Find the Max Value in the Queue.
		//kprintf("\n The priority values in wait queue are:");
		while(&q[curr] != &q[locarr[args[i]].lqtail])
		{
		//kprintf("\t %d", curr);
		//kprintf("\t %d", q[curr].qkey);
			if(max <= q[curr].qkey)
			{
				max = q[curr].qkey;
			}
			curr=q[curr].qnext;
		}
		//kprintf("\n Max priority value found is : %d", max);
		// Check whether there are duplicate values in the queue with the maximum value, and add them to loc_proc.
		curr_tem = locarr[args[i]].lqhead;
		curr = q[curr_tem].qnext;
		// Not enter for the first test case
		while(&q[curr] != &q[locarr[args[i]].lqtail])
		{
			if(q[curr].qkey == max)
			{
				//kprintf("\n lock proc value assigned is (process)%d", curr);
				//kprintf("\t k : %d", k);
				//kprintf("\t args[i] : %d",args[i]);
				//kprintf("\t lock_proc[args[i]][k] :%d",lock_proc[args[i]][k]);
				lock_proc[args[i]][k] = curr;  // Accumulate multiple processes(change made - curr is the process) of a lock in wait queue, with same maximum priority.
				k++;
			}
			curr = q[curr].qnext;
		}
		//curr = q[curr].qnext;
	}// End of if  if(nonempty(locarr[args[i]].lqhead))
	i++;
	} //End of while(i < numlocks)
	
	i=0;
	while(i < numlocks)
	{
		// Check if the releaser type is reader and it's the last reader
		//int read_count = 0;
		//kprintf("\nThe locarr[args[i]].ltype value is %d", locarr[args[i]].ltype);
		//kprintf("\n args[i] value is %d", args[i]);
		
		   /*if(locarr[args[i]].ltype == READ)
		   {
		   kprintf("\nEntered to check if the reader is the last reader");
		   int j=0;

		*   kprintf("\n Checking the values of type in queue");
		   int ch =0;
		   while(ch < NPROC)
		   {
		   kprintf("\n Type is %d", q[ch].type);
		   ch++;
		   }
		*/
		   //kprintf("\n args[i] is %d",args[i]);
		   

		   /*while(j < NLOCKS)
		   {
		   kprintf("\nThe queue index is  %d", locarr[args[i]].proc_queue[j]);
		  // kprintf("\nThe value of read_count is %d", read_count);
		   kprintf("\nThe value of queue type is %d",q[locarr[args[i]].proc_queue[j]].type); 
		   if (q[locarr[args[i]].proc_queue[j]].type == READ)
		   //read_count++;
		   j++;
		   }
		   }*/
		//kpri\ntf("Count to check last reader is %d", read_count);

		// If the Releaser type is WRITE or the Last Reader Releases the Lock
		//kprintf("\n The value of reader count is %d", locarr[args[i]].reader_count); 
		int reader_count1 = locarr[args[i]].reader_count+1; // Having a dummy count to increment the value  including the lock that's getting released.
		//kprintf("\nThe reader_count1 value is %d", reader_count1);
		// Wont enter for the first reader releasing the lock.
		//kprintf("\n locarr[args[i].ltype %d", locarr[args[i]].ltype);
		int temp_type = locarr[args[i]].ltype;// (Holds the type of the lock to be released);
		if(reader_count1 == 1 || temp_type == WRITE)
			locarr[args[i]].ltype = -1;
		//kprintf("\n Lock Proc Values :");
		int test1=0;
		 while(test1 < NLOCKS)
                        {
				//if(lock_proc[args[i]][test1] > -1)
					//kprintf("\t  args[i] %d", args[i]);
					//kprintf("\t k  %d", test1);
                                	//kprintf("\t lock_proc[args[i]][k] : %d", lock_proc[args[i]][test1]);
                                if(lock_proc[args[i]][test1] != -1)
					{
					//kprintf("\n,,,,,,,,, lock_proc[args[i]][k] : %d", lock_proc[args[i]][test1]);
					}
					//kprintf("\n Sucess");
				test1++;
                        }
		//kprintf("Entered out of lock proc print");
		if(temp_type == WRITE || reader_count1 == 1) // When last reader is releasing there should be no readers in the queue.
		{
			//kprintf("\nEntered Writer / Last Reader");
			int k=0;
			int count = 0;
			if(nonempty(locarr[args[i]].lqhead))
			{			
			while(k < NLOCKS)
			{
				if(lock_proc[args[i]][k] != 0)
				{
				count++;
				}
				k++;
			}
			//kprintf("\nCount value is : %d",count);
			// When there is only one process with maximum priority in the lock queue.
			// Printing for testing
			//kprintf("..........Queue values for testing... Should be empty");
			int curr = locarr[args[i]].lqhead;
			while(&q[curr] != &q[locarr[args[i]].lqtail])
			{
			//kprintf("\t %d", curr);
			curr = q[curr].qnext;
			}
			//kprintf("Shouldn't enter for test case 1");
			//kprintf("count : %d", count);
			if(count == 1)
			{
				//kprintf("\n One process in queue with max priority");
				if(q[lock_proc[args[i]][0]].type == READ)
				{ 
					//kprintf("\t Making the reader acquire the lock");
					lock_acquire(lock_proc[args[i]][0], args[i]);  // Check if all required operations are done.

					// Make multiple readers in the queue acquire the lock as well
					int curr = q[locarr[args[i]].lqhead].qnext;
					while(&q[curr] != &q[locarr[args[i]].lqtail])
					{
						if(q[curr].type == READ)
						{
							lock_acquire(curr,args[i]);
						}
						curr = q[curr].qnext;
					}
				}
				else
				{
					//kprintf("\tMaking a writer acquire the lock"); 
					lock_acquire(lock_proc[args[i]][0], args[i]); 
				}


			}
			// When there are multiple process with same priority
			else
			{
				//kprintf("\nEntered else... Multiple Process with same priority");
				int max_time = 0;
				int second_time = 0;
				int max_proc;
				int second_proc;
				int l=0;

				unsigned long curr_time = ctr1000; // ctr1000 changes, so has to be stored
				
				/*Recently Commented */while(lock_proc[args[i]][l] != 0)
				{
					//kprintf("\n Entered while... in lock_proc, accessing qtab");
					if((curr_time - q[lock_proc[args[i]][l]].time) > max_time)  // Check in wait queue of process' ID and the time.
					{
						second_time = max_time;
						second_proc = max_proc;
						max_time = curr_time - q[lock_proc[args[i]][l]].time;
						max_proc = lock_proc[args[i]][l] ;
					}
				else if(((curr_time - q[lock_proc[args[i]][l]].time) > second_time) && (curr_time - q[lock_proc[args[i]][l]].time) != max_time)
					{
						second_time = curr_time - q[lock_proc[args[i]][l]].time;
						second_proc = lock_proc[args[i]][l];
					}
					l++;
				}

				//kprintf("\n max_time %d", max_time);
				//kprintf("\n second_time %d", second_time);

				if((max_time - second_time) < 1000 || (second_time - max_time) < 1000 )
				{
					//kprintf("/////////////////Entered if///////////////////");
					if(q[max_proc].type == WRITE )
					{
						lock_acquire(max_proc, args[i]);
					}

					else if(q[second_time].type == WRITE)
					{
						lock_acquire(second_proc, args[i]);
					}
				}
				// Means that i) either the time difference is not 1 sec so giving it to max_proc, or ii) both processes (max and second) are readers. 
				else
				{
					//kprintf("\n////////////////Entered else to go into lock acquire///////////////");
					// If the type of process to acquire the lock is reader, making all reader processes in the queue acquire the lock as well.
					if(q[max_proc].type == READ)
					{
						//kprintf("\n Queue of max proc");
						lock_acquire(max_proc,args[i]);
						int curr = q[locarr[args[i]].lqhead].qnext;
						while(&q[curr] != &q[locarr[args[i]].lqtail])
						{
							if(q[curr].type == READ)
							{
								lock_acquire(curr,args[i]);
							}
							curr = q[curr].qnext;
						}
					}
					// If the process acquiring the lock is not a reader.
					else
					{	
						//kprintf("\n Queue of max proc2");
						lock_acquire(max_proc,args[i]);
					}
				}
				
			}
			  
			restore(ps);
			resched();
			//kprintf("Going to exit");
			return(OK);
		}
		} // End of if where we check if this is the last reader or Writer releasing the lock.
		i++;
	} // End of while
	//kprintf("\nEntered end of release all");
	//resched();
} // End of function releaseall

// This function updates the prio_max of the loc
void max_prio_inwaitq(int loc, int curr)
{
	//kprintf("\nEntered max_prio_inwaitq");
	struct lentry *lptr;
	lptr = &locarr[loc];
	int curr_temp = locarr[loc].lqhead;
	curr = q[curr_temp].qnext;
	int max = 0;

	while(&q[curr] != &q[locarr[loc].lqtail])
	{
		if(proctab[curr].pinh == 0)
		{
			if(max <= proctab[curr].pprio)
				max = proctab[curr].pprio;
		}
		else
		{
			if(max <= proctab[curr].pinh)
				max = proctab[curr].pinh;
		}
		curr = q[curr].qnext;
	}
	//kprintf("\n Lock is %d", loc);
	//kprintf("\n lptr->prio_max in max_prio_inwaitq %d",lptr->prio_max); 
	lptr->prio_max = max;
	//kprintf("\n lptr->prio_max in max_prio_inwaitq %d",lptr->prio_max);
}

// This function checks the prio_max of all locks(hence max priority of the process in wait queue) held by the process and updates the pinh
void lock_prio_process(int proc)
{
	
	//kprintf("\nEntered lock_prio_process %d", proc);
	struct lentry *lptr = &locarr[0];
	//kprintf("\n prio max :%d", lptr->prio_max);
	int j=0;
	int max = 0;
	struct pentry *pptr;
	pptr = &proctab[proc];
	while(pptr->locks_held[j] != -1 )
	{
	//kprintf("\n entered while in pptr->locks_held");
	//kprintf("\n pptr->lock %d", pptr->locks_held[j]);
	//kprintf("\n prio max %d", locarr[pptr->locks_held[j]].prio_max);
		// We check if the maximum priority of the lock descriptor's wait queue is greater than or equal to the max value.
		if(locarr[pptr->locks_held[j]].prio_max >= max)
		{
			
			max = locarr[pptr->locks_held[j]].prio_max;
		}
		j++;
	}
	//kprintf("\n max %d", max);
	if(pptr->pprio < max)
	{
		pptr->pinh = max;
	}
	else
		pptr->pinh = 0;
}

void lock_acquire(int curr, int loc)
{
	//kprintf("\nEntered lock_acquire");  
	// Dequeue the process from the wait queue and put it in ready queue;
	int l_type = q[curr].type;
	dequeue(curr);
	ready(curr,RESCHNO);
	// Insert into proctab the lock value by going through the array.

	int j=0;
	while(proctab[curr].locks_held[j] != -1)
	{
		j++;
	}
	proctab[curr].locks_held[j] = loc;

	//Insert into lock descriptor entry by adding the process to the array.
	struct pentry *pptr;
	pptr = &proctab[currpid];
	j=0;
	while((locarr[loc].proc_queue[j]) != -1)
	{
		j++;
	}
	locarr[loc].proc_queue[j] = curr;

	// Set all parameters related to the lock.
	locarr[loc].lstate = LUSED;
	locarr[loc].ltype =  q[curr].type;
	if(l_type == READ)
	{
		locarr[loc].reader_count++;
	}
	// Find Maximum of Process Priority of Processes in Wait Queue and Update in lptr->prio_max
	max_prio_inwaitq(loc, curr); // prio_max(lock)

	// We update pinh of all the processes currently holding the lock(including the process that recently acquired the lock), as the max_prio of the lock has changed.
	int k=0;
	while((locarr[loc].proc_queue[k]) != -1)
	{
		lock_prio_process(locarr[loc].proc_queue[k]);
		k++;
	}
}


