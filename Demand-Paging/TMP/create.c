/* create.c - create, newpid */
    
#include <conf.h>
#include <i386.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <paging.h>

LOCAL int newpid();

/*------------------------------------------------------------------------
 *  create  -  create a process to start running a procedure
 *------------------------------------------------------------------------
 */
SYSCALL create(procaddr,ssize,priority,name,nargs,args)
	int	*procaddr;		/* procedure address		*/
	int	ssize;			/* stack size in words		*/
	int	priority;		/* process priority > 0		*/
	char	*name;			/* name (for debugging)		*/
	int	nargs;			/* number of args that follow	*/
	long	args;			/* arguments (treated like an	*/
					/* array in the code)		*/
	{
	unsigned long	savsp, *pushsp;
	STATWORD 	ps;    
	int		pid;		/* stores new process id	*/
	struct	pentry	*pptr;		/* pointer to proc. table entry */
	int		i;
	unsigned long	*a;		/* points to list of args	*/
	unsigned long	*saddr;		/* stack address		*/
	int		INITRET();
	unsigned long *page_dir_base;

	disable(ps);

	if (ssize < MINSTK)
		ssize = MINSTK;
	ssize = (int) roundew(ssize);
	if (((saddr = (unsigned long *)getstk(ssize)) ==
	    (unsigned long *)SYSERR ) ||
	    (pid=newpid()) == SYSERR || priority < 1 ) {
		restore(ps);
		return(SYSERR);

	}
	
//	kprintf("Entered create");
//       kprintf("\n In Create......................");
//	kprintf("\n The value of pid : %d", pid);
	int available_frame = -1;
        get_frm(&available_frame, 0);// available_frame gets a free frame number
        //kprintf("\nIn Create....................................................................");
        //kprintf("\nThe value of pid : %d",pid);
//        kprintf("\nThe available frame is %d", available_frame);
        proctab[pid].pdbr = (FRAME0 + available_frame)*NBPG;
        pd_t *pd_1;
        //struct pd_t pd[1024];
        //&pd = proctab[i].pdbr;
        int j =0;
        pd_1 = proctab[pid].pdbr;
        //kprintf("\nThe value of pdbr is %08x", pd_1);
        while (j < 1024)
        {
        //kprintf("\nValue after shifting pd1 %08x",pd_1);
        pd_1->pd_pres  = 0;
        pd_1->pd_write = 1;
        pd_1->pd_user = 0;
        pd_1->pd_pwt = 0;
        pd_1->pd_pcd  = 0;
        pd_1->pd_acc = 1;
        pd_1->pd_mbz = 0;
        pd_1->pd_fmb = 0;
        pd_1->pd_global  = 0;
        pd_1->pd_avail = 1;
        pd_1->pd_base = 0;  // Initializing the base value to zero */
        /*if( j == 0 || j == 1 || j == 2 || j == 3)  //Populating the global page table values.
/        {
        proctab[i].pd[j]->pd_pres  = 1;
        proctab[i].pd[j]->pd_write = 0;
        proctab[i].pd[j]->pd_base = 1024*4096 + j*1024; // We have reserved the first frame for the four Page Tables
        }*/
        j++;
        pd_1 = proctab[pid].pdbr + sizeof(pd_t)*j;
        }
        //page_dir_base = (unsigned long *)getmem(4096);
        //proctab[i].pdbr = page_dir_base;

        /* Populating Global Page Table Entries in Page Directories */
        j=0;
        //pd_1 = proctab[i].pdbr + sizeof(pd_t)*2;
	//kprintf("Value of pd_1 is %08x", pd_1);
        /* Teting the Structure */
        //kprintf("\npd_1->pd_write(1) %d",pd_1->pd_write);
        //kprintf("\npd_1->pd_avail(1) %d", pd_1->pd_avail);
        pd_1 = proctab[pid].pdbr;
        while(j < 4)
        {
        pd_1->pd_pres  = 1;
        pd_1->pd_write = 0;
        pd_1->pd_base = FRAME0 + j; // We have reserved the first 4 frames for the four Page Tables
        j++;
	pd_1 = proctab[pid].pdbr + sizeof(pd_t)*j;
        }
	//Write Function For FreeMem
	frm_tab[available_frame].fr_pid = pid;
	frm_tab[available_frame].fr_status = FRM_MAPPED;
	frm_tab[available_frame].fr_type = FR_DIR;
	
		
	numproc++;
	pptr = &proctab[pid];

	pptr->fildes[0] = 0;	/* stdin set to console */
	pptr->fildes[1] = 0;	/* stdout set to console */
	pptr->fildes[2] = 0;	/* stderr set to console */

	for (i=3; i < _NFILE; i++)	/* others set to unused */
		pptr->fildes[i] = FDFREE;

	pptr->pstate = PRSUSP;
	for (i=0 ; i<PNMLEN && (int)(pptr->pname[i]=name[i])!=0 ; i++)
		;
	pptr->pprio = priority;
	pptr->pbase = (long) saddr;
	pptr->pstklen = ssize;
	pptr->psem = 0;
	pptr->phasmsg = FALSE;
	pptr->plimit = pptr->pbase - ssize + sizeof (long);	
	pptr->pirmask[0] = 0;
	pptr->pnxtkin = BADPID;
	pptr->pdevs[0] = pptr->pdevs[1] = pptr->ppagedev = BADDEV;

		/* Bottom of stack */
	*saddr = MAGIC;
	savsp = (unsigned long)saddr;

	/* push arguments */
	pptr->pargs = nargs;
	a = (unsigned long *)(&args) + (nargs-1); /* last argument	*/
	for ( ; nargs > 0 ; nargs--)	/* machine dependent; copy args	*/
		*--saddr = *a--;	/* onto created process' stack	*/
	*--saddr = (long)INITRET;	/* push on return address	*/

	*--saddr = pptr->paddr = (long)procaddr; /* where we "ret" to	*/
	*--saddr = savsp;		/* fake frame ptr for procaddr	*/
	savsp = (unsigned long) saddr;

/* this must match what ctxsw expects: flags, regs, old SP */
/* emulate 386 "pushal" instruction */
	*--saddr = 0;
	*--saddr = 0;	/* %eax */
	*--saddr = 0;	/* %ecx */
	*--saddr = 0;	/* %edx */
	*--saddr = 0;	/* %ebx */
	*--saddr = 0;	/* %esp; fill in below */
	pushsp = saddr;
	*--saddr = savsp;	/* %ebp */
	*--saddr = 0;		/* %esi */
	*--saddr = 0;		/* %edi */
	*pushsp = pptr->pesp = (unsigned long)saddr;

	restore(ps);
//	kprintf("Returning from create");
	return(pid);
}

/*------------------------------------------------------------------------
 * newpid  --  obtain a new (free) process id
 *------------------------------------------------------------------------
 */
LOCAL int newpid()
{
	int	pid;			/* process id to return		*/
	int	i;

	for (i=0 ; i<NPROC ; i++) {	/* check all NPROC slots	*/
		if ( (pid=nextproc--) <= 0)
			nextproc = NPROC-1;
		if (proctab[pid].pstate == PRFREE)
			return(pid);
	}
	return(SYSERR);
}
