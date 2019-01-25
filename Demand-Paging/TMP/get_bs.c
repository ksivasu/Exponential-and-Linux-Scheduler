#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>


int get_bs(bsd_t bs_id, unsigned int npages) {

  /* requests a new mapping of npages with ID map_id */
//kprintf("............Entered get_bs................");    
STATWORD ps;
disable(ps);
/* When npages size more than 256 or less than or equal to 0 is requested */
    if(npages > 256 || npages <=0 )
	{
	//kprintf("Error at get_bs, requested invalid amount of pages");
	return ((int) SYSERR);
    	}	
/* When the requested backing store already exists in the bsm_tab */

    int i = 0;
    //while (i < 8)
//	{
	if(bsm_tab[bs_id].bs_status == BSM_MAPPED)
	{
	   restore(ps);
	   return bsm_tab[i].bs_npages;
	}

/* Creating a mapping in the bsm_map for the new backing store */
   bsm_map(currpid, 0, bs_id, npages);
   //kprintf("To be implemented!\n");
  restore(ps); 
  return npages;
}


