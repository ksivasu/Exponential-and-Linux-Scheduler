#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lock.h>
#include <sem.h>
#include <stdio.h>

void reader1 (char *msg, int lck, int prio)
{
	//kprintf("\n %s", msg);
        lock (lck, READ, prio);
        kprintf (" \n %s: acquired lock, sleep 2s\n", msg);
        sleep (5);
        kprintf ("\n  %s: to release lock\n", msg);
        releaseall (1, lck);
}

void writer1 (char *msg, int lck, int prio)
{
        //kprintf("\n %s", msg);
        lock (lck, WRITE, prio);
        kprintf (" \n %s: acquired lock, sleep 2s\n", msg);
        sleep (5);
        kprintf ("\n  %s: to release lock\n", msg);
        releaseall (1, lck);
}

void reader2 (char *msg, int sem)
{
	//kprintf("\n %d", sem);
	//kprintf("\n %s", msg);
	int val = wait(sem); 
        kprintf ("\n %s: acquired lock, sleep 2s\n", msg);
        sleep (5);
        kprintf (" \n %s: to release lock\n", msg);
        signal(sem);
}


int main()
{

int sem, lck,sd1,sd2,sd3,rd1,rd2,rd3,wr1; 
// Semaphore Implmentation
kprintf("\n Semaphore Implementation");
sem = screate(1);

sd1 = create(reader2, 2000, 20, "sd1", 2, "A", sem);
sd2 = create(reader2, 2000, 20, "sd2", 2, "B", sem);
sd3 = create(reader2, 2000, 20, "sd3", 2, "C", sem);
kprintf("\n Starting A");
resume(sd1);
//sleep(1);
kprintf("\n Starting B");
resume(sd2);
sleep(1);
kprintf("\n Starting C");
resume(sd3);

sleep(15);

// Lock Implementation
kprintf("\n Lock Implementation");
lck = lcreate();
rd1 = create(reader1, 2000, 20, "rd1", 3, "A", lck, 40);
rd2 = create(reader1, 2000, 20, "rd2", 3, "B", lck, 35);
wr1 = create(writer1, 2000, 20, "rd3", 3, "C", lck, 20);
kprintf("\n Starting A");
resume(rd1);
//sleep(1);
kprintf("\n Starting B");
resume(rd2);
sleep(1);
kprintf("\n Starting C");
resume(wr1);
//kprintf("Entered Sleep");
sleep(15);
shutdown();
return 0;
}


