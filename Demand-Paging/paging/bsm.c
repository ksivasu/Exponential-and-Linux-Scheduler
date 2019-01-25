/* bsm.c - manage the backing store mapping*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>

/*-------------------------------------------------------------------------
 * init_bsm- initialize bsm_tab
 *-------------------------------------------------------------------------
 */

bs_map_t bsm_tab[];

SYSCALL init_bsm()
{
STATWORD ps;
disable(ps);

int i;
for (i=0; i <=7; i++)
{
bsm_tab[i].bs_status = BSM_UNMAPPED;
bsm_tab[i].bs_pid = -1;   //initializing to a negative variable(pid using this slot)
bsm_tab[i].bs_vpno = -1; //starting virtual page no.
bsm_tab[i].bs_npages = 0; // number of pages in the store
bsm_tab[i].bs_sem = 0; // Indicating no semaphore mechanism
}

restore(ps);
return OK;
}

/*-------------------------------------------------------------------------
 * get_bsm - get a free entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL get_bsm(int* avail)
{
STATWORD ps;	
int m;
disable(ps);
for( m =0; m < 7; m++)
{

if(bsm_tab[m].bs_status == BSM_UNMAPPED)
	{ 
	*avail = m;	
	break;
	}
}

restore(ps);
return OK;
}


/*-------------------------------------------------------------------------
 * free_bsm - free an entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL free_bsm(int i)
{
STATWORD ps;
disable(ps);

if(i < 0 || i > 7 || bsm_tab[i].bs_pid != currpid)
{
restore(ps);
return SYSERR;
}
bsm_tab[i].bs_status = BSM_UNMAPPED;
bsm_tab[i].bs_pid = -1;   //initializing to a negative variable(pid using this slot)
bsm_tab[i].bs_vpno = -1; //starting virtual page no.
bsm_tab[i].bs_npages = 0; // number of pages in the store
bsm_tab[i].bs_sem = 0; // Indicating no semaphore mechanism


restore(ps);
return OK;
}


/*-------------------------------------------------------------------------
 * bsm_lookup - lookup bsm_tab and find the corresponding entry
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_lookup(int pid, long vaddr, int* store, int* pageth)
{
STATWORD ps;
//kprintf("\n ............. Entered BSM Lookup ...............");
int page_no = vaddr/NBPG;
//kprintf("\n page_no: %ld", page_no);
//kprintf("\n vaddr : %ld",vaddr); 
disable(ps);
//kprintf("Enter BSM-LOOKUP");
//kprintf("\nbsm_tab[1].bs_pid %d", bsm_tab[1].bs_pid);
/*if(isbadpid(pid))
	{
	kprintf("\npid : %d", pid);
	kprintf("\nIt's a badpid, my bad");
	restore(ps);
	return SYSERR;
	}*/
int i;
for (i =0; i <= 7; i++)
{
if(bsm_tab[i].bs_pid == pid)
{
*store = i;
//kprintf("........INSIDE BSMLOOKUP............");
//kprintf("..........................................");
//kprintf("\n bsm_tab[i].bs_vpno %d", bsm_tab[i].bs_vpno);
//kprintf("\n page_no : %d", page_no);
//kprintf("..........................................");
*pageth = (page_no - bsm_tab[i].bs_vpno); //Changed after final code 	
//kprintf("\n bsm_tab[i].bs_vpno %d", (bsm_tab[i].bs_vpno));
//kprintf("\n pageth %d", *pageth); 
//kprintf("\n..........BS FOUND.........");
//kprintf("\n leaving BSM_LOOKUP"); 
//kprintf("\n pid %d", bsm_tab[i].bs_pid);
//kprintf("\n store %d", i);
restore(ps);
return OK;
}
//restore(ps);
//return OK;
}

//kprintf(".........BS_NOT FOUND.........");
restore(ps);
return SYSERR;

}
/*-------------------------------------------------------------------------
 * bsm_map - add an mapping into bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_map(int pid, unsigned int vpno, int source, int npages)
{
STATWORD ps;
disable(ps);
//kprintf("........Entered bsm_map........");
if(isbadpid(pid) || npages > 256 || npages == 0 )
        {
        restore(ps);
//	kprintf("Going to return SYSERR");
        return SYSERR;
        }
//kprintf("\n Store %d",source);
//kprintf("\n Pid %d", pid);
//kprintf("................................"); 
bsm_tab[source].bs_status = BSM_MAPPED;
bsm_tab[source].bs_pid = pid;   //initializing to a negative variable(pid using this slot)
bsm_tab[source].bs_vpno = vpno; // how to do the mapping between virtual page numbers? ; //starting virtual page no.
bsm_tab[source].bs_npages = npages; // number of pages in the store
bsm_tab[source].bs_sem = 0;
proctab[pid].vhpno = vpno;
proctab[pid].store = source;
restore(ps);
return OK;
}




/*-------------------------------------------------------------------------
 * bsm_unmap - delete an mapping from bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_unmap(int pid, int vpno, int flag)
{

//kprintf("Entered bsm_unmap");
STATWORD ps;
disable(ps);

if(isbadpid(pid))
        {
        restore(ps);
        return SYSERR;
        }
int i;
for (i =0; i <= 7; i++)
{
if(bsm_tab[i].bs_pid == pid)
{
bsm_tab[i].bs_status = BSM_MAPPED;
bsm_tab[i].bs_pid = -1;   //initializing to a negative variable(pid using this slot)
bsm_tab[i].bs_vpno = -1; // how to do the mapping between virtual page numbers? ; //starting virtual page no.
bsm_tab[i].bs_npages = 0; // number of pages in the store
bsm_tab[i].bs_sem = 0;
}
}
}
	
