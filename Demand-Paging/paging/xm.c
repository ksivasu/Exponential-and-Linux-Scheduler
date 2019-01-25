/* xm.c = xmmap xmunmap */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>


/*-------------------------------------------------------------------------
 * xmmap - xmmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmmap(int virtpage, bsd_t source, int npages)
{
  //kprintf("xmmap - to be implemented!\n");
  STATWORD ps;
  disable(ps);
//kprintf("Entered xmmap");
if(source < 0 || source > 7 || npages <= 0 || npages > 256)
{
//kprintf("Error encountered at xmmap!");
restore(ps);
return SYSERR;
}
//kprintf("Inside xmmap - currpid %d",currpid);
bsm_map(currpid, virtpage, source, npages);

  restore(ps);
  return OK;
}




/*-------------------------------------------------------------------------
 * xmunmap - xmunmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmunmap(int virtpage)
{
  //kprintf("To be implemented!");
  STATWORD ps;
  disable(ps);

 //kprintf("Error encountered at xmunmap");
 bsm_unmap(currpid, virtpage, 0);
 
 restore(ps); 
 return OK;
}
