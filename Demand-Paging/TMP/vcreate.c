/* vcreate.c - vcreate */
    
#include <conf.h>
#include <i386.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <paging.h>

/*
static unsigned long esp;
*/

LOCAL	newpid();
/*------------------------------------------------------------------------
 *  create  -  create a process to start running a procedure
 *------------------------------------------------------------------------
 */
SYSCALL vcreate(procaddr,ssize,hsize,priority,name,nargs,args)
	int	*procaddr;		/* procedure address		*/
	int	ssize;			/* stack size in words		*/
	int	hsize;			/* virtual heap size in pages	*/
	int	priority;		/* process priority > 0		*/
	char	*name;			/* name (for debugging)		*/
	int	nargs;			/* number of args that follow	*/
	long	args;			/* arguments (treated like an	*/
					/* array in the code)		*/
{
STATWORD ps;
disable(ps);
int pid = create(procaddr,ssize,priority,name,nargs,args);
//kprintf("The value of pid in vcreate is %d", pid);
int free = 0;
get_bsm(&free);
get_bs(free, hsize);
bsm_map(pid, 4096, free, hsize);
proctab[pid].store = free;
proctab[pid].vhpno = 4096;
proctab[pid].vhpnpages = hsize;
//proctab[pid].vmemlist = getmem(sizeof(struct mblock));
(proctab[pid].vmemlist)->mnext = BACKING_STORE_BASE+(free*BACKING_STORE_UNIT_SIZE);
//kprintf("\n The address is %x", BACKING_STORE_BASE+(free*BACKING_STORE_UNIT_SIZE));



struct mblock *temporary;
temporary = BACKING_STORE_BASE+(free*BACKING_STORE_UNIT_SIZE);
temporary->mlen = hsize*NBPG;
temporary->mnext = NULL;


//proctab[pid].vmemlist->mnext = 4096*4096;

//kprintf("\nReturning From Vcreate");
restore(ps);
return pid;
}

/*------------------------------------------------------------------------
 * newpid  --  obtain a new (free) process id
 *------------------------------------------------------------------------
 */

/*LOCAL	newpid()
{
//	int	pid;			* process id to return		*/
//	int	i;
//
//	for (i=0 ; i<NPROC ; i++) {	/* check all NPROC slots	*/
//		if ( (pid=nextproc--) <= 0)
//			nextproc = NPROC-1;
//		if (proctab[pid].pstate == PRFREE)
//			return(pid);
//	}
//	return(SYSERR);
//}
