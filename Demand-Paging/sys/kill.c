/* kill.c - kill */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <q.h>
#include <stdio.h>
#include <paging.h>
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

	freestk(pptr->pbase, pptr->pstklen);
	switch (pptr->pstate) {

	case PRCURR:	pptr->pstate = PRFREE;	/* suicide */
			resched();

	case PRWAIT:	semaph[pptr->psem].semcnt++;

	case PRREADY:	dequeue(pid);
			pptr->pstate = PRFREE;
			break;

	case PRSLEEP:
	case PRTRECV:	unsleep(pid);
						/* fall through	*/
	default:	pptr->pstate = PRFREE;
	}
   /* 1) Frames that hold any of its pages should be written to the backing store and freed*/
	kprintf("Entered Kill");
	int i =0;
	while(i < 1024)
	{
	if(frm_tab[i].fr_pid == pid)
	{
	int store = 0;
	int pageth = 0;
	bsm_lookup(pid,(frm_tab[i].fr_vpno)*NBPG, &store, &pageth);
	write_bs((frm_tab[i].fr_vpno)*NBPG, store, pageth);
	free_frm(i);
	}
	i++;
	}
     /* 2) All of it's mappings should be removed from the backing store map */
	bsm_unmap(pid, frm_tab[i].fr_vpno, 0);
     /* 3) Backing Store for the process' heap should be released. */
	i = 0;
	while(i < 8)
	{
	if(bsm_tab[i].bs_pid == pid)
	{
	release_bs(i);
	}
	i++;
	}
    /* 4) Frame Using the Page Directory Should be Released */
	i=0;
	while(i < 1024)
	{
	if(frm_tab[i].fr_pid == pid && frm_tab[i].fr_type == FR_DIR)
	{
	free_frm(i);
	break;
	}
	i++;
	}

	restore(ps);
	return(OK);
}
