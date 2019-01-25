#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>

/*-------------------------------------------------------------------------
 * pfint - paging fault ISR
 *-------------------------------------------------------------------------
 */

SYSCALL pfint()
{
STATWORD ps;
unsigned long virt_add = read_cr2(); //Contains the page fault linear address.
int store_no;
int pageth_1;
disable(ps);
virt_addr_t a = *(virt_addr_t *)&virt_add;
unsigned int page_off = a.pg_offset;
unsigned int table_off = a.pt_offset;
unsigned int pd_off = a.pd_offset;
//kprintf("page_off : %u", page_off);
//kprintf("\n table_off : %u", table_off);
//kprintf("pd_off : %u", pd_off);

//kprintf("Entered pfint");
//kprintf("currpid value is : %d",currpid);
if(bsm_lookup(currpid, virt_add, &store_no, &pageth_1) == SYSERR)
{
//kprintf("The address is not legal");
kill(currpid);
}
pd_t *pd_temp1;
pd_temp1 = proctab[currpid].pdbr + pd_off*sizeof(pd_t);
unsigned long *page_table_base = pd_temp1->pd_base;
int frame_no = -1;
//pt_t *init_pt;
//init_pt = (FRAME0 + frame_no)*4096;

if(pd_temp1->pd_pres !=  1)
{
//kprintf("\nHave to Create a New Page Table");
get_frm(&frame_no, 0);
frm_tab[frame_no].fr_status = FRM_MAPPED;
frm_tab[frame_no].fr_pid = currpid;
frm_tab[frame_no].fr_refcnt = 0;
frm_tab[frame_no].fr_type = FR_TBL;

// Initializing the page directory entries for the page table as below :

pd_temp1->pd_pres = 1;
pd_temp1->pd_write = 1;
pd_temp1->pd_user = 0;
pd_temp1->pd_pwt =0;
pd_temp1->pd_pcd = 0;
pd_temp1->pd_acc = 0;
pd_temp1->pd_mbz = 0;
pd_temp1->pd_fmb = 0;
pd_temp1->pd_global = 0;
pd_temp1->pd_avail =0;
pd_temp1->pd_base = (FRAME0 + frame_no);
}

 store_no = -1;
 pageth_1 = -1;
bsm_lookup(currpid, virt_add, &store_no, &pageth_1);
/*
kprintf("\n.............................. Before Reading From BS - pfint.c");
kprintf("\nStore Number %d",store_no);
kprintf("\nPage Number %d", pageth_1);
kprintf("\n..............................");
//kprintf("Pageth_1 %d", pageth_1);*/
//Increasing the fr_refcnt for the frame corresponding to frame holding pt.
pd_t *pd_temp2;
pd_temp2 = (proctab[currpid].pdbr + pd_off*sizeof(pd_t));
frm_tab[pd_temp2->pd_base - FRAME0].fr_refcnt++;
int frame_no1 = 0;
get_frm(&frame_no1, 1);
//framenode *node1;
/*Insertng the new frame into the head of circular queue */
if(head->fr_num == -1)
{
  //kprintf("Inserting frame into head of circular queue");
  head->fr_num = frame_no1;
  head->next = head;
  //kprintf("Entered head->fr_num : -1.................");
}

else
{
//kprintf("Entered else.......");
framenode *node1 = (framenode*)getmem(sizeof(framenode));
node1->fr_num = frame_no1;
framenode *node3;
node3 = head;
while(node3->next != head)
{
node3 = node3->next;
}

node3->next = node1;
node1->next = head;
//prev=node3;
//head=node1;
}

//kprintf("\n State of queue after insertion is:");
framenode *node2;
node2 = head->next;
//kprintf("\t%d",head->fr_num);
//kprintf("\n%d", current->fr_num);
while(node2 != head)
{
//kprintf("Entered while.............");
//kprintf("\t%d", node2->fr_num);
node2=node2->next;
}
frm_tab[frame_no1].fr_status = FRM_MAPPED;
frm_tab[frame_no1].fr_pid = currpid;
frm_tab[frame_no1].fr_refcnt = 0;
frm_tab[frame_no1].fr_type = FR_PAGE;
frm_tab[frame_no1].fr_vpno = virt_add/NBPG;
pt_t *init_pt = ((pd_temp2->pd_base))*NBPG+table_off*sizeof(pt_t);
read_bs((FRAME0+frame_no1)*NBPG,store_no,pageth_1);
init_pt->pt_pres  = 1;
init_pt->pt_base  = (FRAME0+frame_no1);
init_pt->pt_acc = 1;
//kprintf("\n PTE address :%u for frame %d", init_pt, frame_no1);
//kprintf("\n Going to write to cr3");
write_cr3(proctab[currpid].pdbr);
restore(ps);
return OK;

//  kprintf("To be implemented!\n");
// return OK;
}

