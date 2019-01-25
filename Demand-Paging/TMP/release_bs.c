#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

SYSCALL release_bs(bsd_t bs_id) {

  /* release the backing store with ID bs_id */
STATWORD ps;
disable(ps);    
free_bsm(bs_id);
 //  kprintf("To be implemented!\n");
restore(ps);   
return OK;

}

