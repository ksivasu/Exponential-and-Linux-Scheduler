#include <conf.h>
#include <i386.h>
#include <kernel.h>
#include <mark.h>
#include <bufpool.h>
#include <proc.h>
#include <sem.h>
#include <sleep.h>
#include <mem.h>
#include <tty.h>
#include <q.h>
#include <io.h>
#include <stdio.h>
#include <lock.h>

 
//struct  lentry  locarr[NLOCKS];
void linit()
{ 
kprintf("entered linit");
struct lentry *lptr;
int i;
for (i=0 ; i<NLOCKS ; i++) {      
                (lptr = &locarr[i])->lstate = LFREE;
                lptr->lqtail = 1 + (lptr->lqhead = newqueue());
        }
}

