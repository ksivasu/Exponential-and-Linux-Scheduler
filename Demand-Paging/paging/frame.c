#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

/*-------------------------------------------------------------------------
 * init_frm - initialize frm_tab
 *-------------------------------------------------------------------------
 */

framenode *head = NULL;
//struct frame_node* current= NULL;
//struct frame_node* prev= NULL;
//head->fr_num = -1;
//head->next = head;

void init_framenode(head);
framenode *current = NULL;
framenode *prev = NULL;

fr_map_t frm_tab[];
SYSCALL init_frm()
{
	//  kprintf("To be implemented!\n");
	//kprintf("\nEntered init_frm"); 
	init_framenode(head);
	int i =0;
	while(i < NFRAMES)    // NFRAMES = 1024
	{
		frm_tab[i].fr_status = FRM_UNMAPPED;
		//frm_tab[i].fr_pid = i;
		frm_tab[i].fr_vpno = 0;// Initializing the frame value to 0.
		frm_tab[i].fr_refcnt = 0;
		frm_tab[i].fr_type = FR_PAGE; // Initializing on default to Frame Page.
		frm_tab[i].fr_dirty = 0;
		i++;
	}
	//kprintf("Leaving init_frm");
	return OK;
}

/*-------------------------------------------------------------------------
 * get_frm - get a free frame according page replacement policy
 *-------------------------------------------------------------------------
 */

SYSCALL get_frm(int* avail, int flag)
{
	//kprintf("\nEntered get_frm");
	STATWORD ps;
	
	int k = 0;
	disable(ps);
	framenode *node_next1;
	framenode *current1;
	while(k < NFRAMES) //NFRAMES = 1024  // Iterating through the 1024 frames
	{
		//kprintf("\n %d%d",k,frm_tab[k].fr_status);
		//kprintf("\nEntered while %d", i);
		//if(frm_tab[i].fr_type == FR_PAGE)
		//{
			//kprintf("\nEntered if");
			if(frm_tab[k].fr_status == FRM_UNMAPPED)
			{
				//frm_tab[i].fr_status = FRM_MAPPED;
				*avail = k;
				//kprintf("\nSetting the value as %d",k);		
				
			/* If flag == 1, insert into queue */
			        /*if(head->fr_num == -1)
				{
				head->fr_num = k;
				head->next = head;
				}
				//framenode *node_next1;
				//framenode *current1;
				else
				{
				  //framenode *node_next1;
				  //framenode *current1;
				  kprintf("\nInserting the frame selected into queue");
				  current1=head;
				  while(current1->next != head)
				  {
					current1 = current1->next;
			          }
				  node_next1->fr_num = k;
				  current1->next = node_next1;
				  node_next1->next = head;
				}*/
				restore(ps);
				return OK;
				//kprintf("\nleaving get_frm");
				//break;
			}
		k=k+1;
	}

	//kprintf("\nAssigning i=%d,setting=%d",i,frm_tab[i].fr_status);
	//if(i){
	//	restore(ps);
	//		return OK;
	//}

	//Iterate through the nodes with frame number and check the values
	//sleep(10);
	//kprintf("\nFrames not found");
	pd_t *pt_temp1;
	pd_t *pt_temp2;
	pt_t *pt_temp3;
	pt_t *pt_temp4;
	
	//framenode *current2;
	//kprintf("\nThe content of the queue is:");
	//current2 = head;
	//while(current2->next != head)
	//{
	//kprintf("\t %d", current2->fr_num);
	//}
	int i = 0;
        current = head;
	while(current->next != head)
	{
		current = current->next;
	}
	prev = current;
	current = head;
	//kprintf("\nThe current current->fr_num is %d", current->fr_num);
	unsigned long page_directory_offset = (frm_tab[current->fr_num].fr_vpno >> 10);
	pt_temp1 = (proctab[frm_tab[current->fr_num].fr_pid].pdbr + page_directory_offset*sizeof(pd_t));
	unsigned long page_table_base = (pt_temp1->pd_base)*NBPG;
	//kprintf("\n Head - current->fr_num %d", current->fr_num);
	unsigned long page_table_offset = (((frm_tab[current->fr_num].fr_vpno) & (2047)));
	pt_temp3 = page_table_base + page_table_offset*sizeof(pt_t);
	/*kprintf("\n ........Printing all pt_acc values..........\n");
	
	framenode *test;
	test = head->next;
	pt_t *pt_test1;
	pd_t *pd_test1;
	 
	unsigned long page_directory_offset1 = (frm_tab[head->fr_num].fr_vpno >> 10);
        pd_test1 = (proctab[frm_tab[head->fr_num].fr_pid].pdbr + page_directory_offset1*sizeof(pd_t));
        unsigned long page_table_base1 = (pd_test1->pd_base)*NBPG;
        kprintf("Frame Number %d", head->fr_num);
        unsigned long page_table_offset1 = (((frm_tab[head->fr_num].fr_vpno)) & (2047));
        pt_test1 = page_table_base1 + page_table_offset1*sizeof(pt_t);
	kprintf("\t %u", pt_test1);
	kprintf("\t %d",pt_test1->pt_acc);

	while(test != head)
	{
	 unsigned long page_directory_offset1 = (frm_tab[test->fr_num].fr_vpno >> 10);
        pd_test1 = (proctab[frm_tab[test->fr_num].fr_pid].pdbr + page_directory_offset1*sizeof(pd_t));
        unsigned long page_table_base1 = (pd_test1->pd_base)*NBPG;
        kprintf("Frame Number %d", test->fr_num);
        unsigned long page_table_offset1 = (((frm_tab[test->fr_num].fr_vpno)) & (2047));
        pt_test1 = page_table_base1 + page_table_offset1*sizeof(pt_t);
	kprintf("\t %u", pt_test1);
	kprintf("\t %d", pt_test1->pt_acc);
	test=test->next;
	}
	kprintf("\n...................................................\n");
	//sleep(10);
	*/
	while(pt_temp3->pt_acc != 0)
	{
		//kprintf("\nEntered pt_acc while");
		//kprintf("\n current->fr_num : %d, pt_temp3->pt_acc : %d", current->fr_num, pt_temp3->pt_acc);
		prev = current;
		pt_temp3->pt_acc = 0;
		current = current->next;
		//if(current->fr_num == 19)
		  //sleep(10);
		page_directory_offset = (frm_tab[current->fr_num].fr_vpno >> 10);
		pt_temp1 = (proctab[frm_tab[current->fr_num].fr_pid].pdbr + page_directory_offset*sizeof(pd_t));
		page_table_base = (pt_temp1->pd_base)*NBPG;
		
		page_table_offset = (((frm_tab[current->fr_num].fr_vpno) & (2047)));
		//kprintf("\t page_table_offset %08x", page_table_offset);
		//kprintf("\t fr_vpno %08x", frm_tab[current->fr_num].fr_vpno);
		pt_temp3 = page_table_base + page_table_offset*sizeof(pt_t);
		//kprintf("\n pt_temp3 %u", pt_temp3);
	        //kprintf("\t After performing current->next : current->fr_num : %d, pt_temp3->pt_acc : %d", current->fr_num, pt_temp3->pt_acc);
	}
	//kprintf("\n After Selection current->fr_num %d", current->fr_num);
	pt_temp3->pt_pres = 0;
	//kprintf("\nLeft pt_acc while, found a frame with pt_acc = 0");
	//kprintf("\nfrm_tab[current->fr_num].fr_pid : %d", frm_tab[current->fr_num].fr_pid);
	//kprintf("\ncurrpid : %d", currpid);
	if(frm_tab[current->fr_num].fr_pid == currpid)
	{
		//kprintf("\n entering TLB clear if");
		//Clear the TLB for that page;
		asm volatile("invlpg (%0)" ::"r" ((frm_tab[current->fr_num].fr_vpno)*NBPG) : "memory");
	}
	//kprintf("\n Cleared TLB");
	//Find the frame occupied by the page table.
	/*if(--frm_tab[page_table_base - FRAME0].fr_refcnt == 0)
	{
		kprintf("\nEntered pd_pres=0");
		pt_temp1 = (proctab[frm_tab[current->fr_num].fr_pid].pdbr + page_directory_offset*sizeof(pd_t));
		pt_temp1->pd_pres = 0;
	}*/
	//kprintf("\nLeft First If");
	//pt_temp3 = (page_table_base + page_table_offset*sizeof(pt_t));
	if((pt_temp3)->pt_dirty == 1)
	{
		//kprintf("\nEnterd writing to bs if dirty");
		int store_1;
		int pageth;
		if(bsm_lookup(frm_tab[current->fr_num].fr_pid,(frm_tab[current->fr_num].fr_vpno)*NBPG, &store_1, &pageth) == SYSERR)
		{
			//kprintf("\nEntering kill");
			kill(frm_tab[current->fr_num].fr_pid);   // Check whether to exit here in description.
		}
		else
		{
			//kprintf("\n .............................. for writing into BS - frame.c");
			//kprintf("\n frame no - %d", current->fr_num);
			//if(current->fr_num == 19)
                         // sleep(10);
			//kprintf("\n store %d", store_1);
			//kprintf("\n pageth %d", pageth);
			//kprintf("\n ..............................");
			//kprintf("Entered else to write back the modified page to BS");
			//pt_temp3 = page_table_base + page_table_offset ;
			write_bs((pt_temp3->pt_base)*NBPG, store_1, pageth);
		}

	}
	//kprintf("Returning from frame.c");
	//
	*avail = current->fr_num;
	//kprintf(debug);
	if(debug == SC)
		{
		kprintf("\n %d", current->fr_num);
		}
	prev->next = current->next; // Removing the obtained frame.
	current = current->next; // Or current = head
	head = current;
	//current = head;
	//pt_temp3->pt_acc = 1;
	//kprintf("\n State of the queue after removing is:");
		
	//kprintf("\t %d",head->fr_num);
	framenode *node1 = head->next;
	while(node1 != head)
	{
	//kprintf("\t %d", node1->fr_num);
	node1=node1->next;
	}
	restore(ps);
	return OK;
}

void init_framenode(framenode *node)
{
	//kprintf("Entered init_framenode");
	node->fr_num = -1;
	node->next = node;
}


/*-------------------------------------------------------------------------
 * free_frm - free a frame
 *-------------------------------------------------------------------------
 */


SYSCALL free_frm(int i)
{
	STATWORD ps;
	frm_tab[i].fr_status = FRM_UNMAPPED;
	frm_tab[i].fr_type = FR_PAGE;
	frm_tab[i].fr_pid = 0;
	frm_tab[i].fr_refcnt = 0;
	disable(ps);
	while(current->next != head )
	{
		current=current->next;
	}
	prev = current;
	current = head;
	//Removing the value from circular queue
	/*while(current != head)
	{
		if(current->fr_num == i)
		{
			prev->next = current->next;
			current=current->next;
			head = current;
		}
		prev=current;
		current=current->next;
	}
	*/
	if(frm_tab[i].fr_type == FR_DIR || frm_tab[i].fr_type == FR_TBL || frm_tab[i].fr_dirty == 0)
	{
		restore(ps);
		return OK;
	}
	/* Find the Backing Store and Backing Store Offset for the Frame */
	int vp_no_f = frm_tab[i].fr_vpno;
	int offset_store = vp_no_f - proctab[currpid].vhpno;
	int j = 0;

	while(j < 7)                            // We iterate through all the 8 stores to find the store corresponding to the pid.
	{
		if(bsm_tab[j].bs_pid == currpid)
		{
			break;
		}
		j++;
	}
	write_bs((FRAME0 + i)*NBPG, j, offset_store); // Assuming base frame is at FRAME0 and each page has 4KB size.
	restore(ps);
	return OK;
}

