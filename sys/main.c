/* user.c - main */

/*------------------------------------------------------------------------
 *  main  --  user main program
 *------------------------------------------------------------------------
 */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lock.h>

#define DEFAULT_LOCK_PRIO 20

#define assert(x,error) if(!(x)){ \
            kprintf(error);\
            return;\
            }
int mystrncmp(char* des,char* target,int n){
    int i;
    for (i=0;i<n;i++){
        if (target[i] == '.') continue;
        if (des[i] != target[i]) return 1;
    }
    return 0;
}

/*--------------------------------Test 1--------------------------------*/
 
void reader1 (char *msg, int lck)
{
	lock (lck, READ, DEFAULT_LOCK_PRIO);
	kprintf ("  %s: acquired lock, sleep 2s\n", msg);
	sleep (2);
	kprintf ("  %s: to release lock\n", msg);
	releaseall (1, lck);
}

void test1 ()
{
	int	lck;
	int	pid1;
	int	pid2;
	//kprintf("Entered test1");
	kprintf("\nTest 1: readers can share the rwlock\n");
	lck  = lcreate ();
	//kprintf("\nThe assigned value of lock in Test 1 : %d", lck);
	assert (lck != SYSERR, "Test 1 failed");
	pid1 = create(reader1, 2000, 20, "reader a", 2, "reader a", lck);
	pid2 = create(reader1, 2000, 20, "reader b", 2, "reader b", lck);
	resume(pid1);
	resume(pid2);
	sleep (10);
	ldelete (lck);
	kprintf ("Test 1 ok\n");
}

/*----------------------------------Test 2---------------------------*/
char output2[10];
int count2;
void reader2 (char msg, int lck, int lprio)
{
        int ret;
        kprintf("  %c: to acquire lock\n", msg);
        lock (lck, READ, lprio);
        output2[count2++]=msg;
        kprintf ("  %c: acquired lock, sleep 3s\n", msg);
        sleep (3);
        output2[count2++]=msg;
	//kprintf("\n Reader Output before release :");
	//kprintf("\t %s", output2);
        kprintf ("  %c: to release lock\n", msg);
	releaseall (1, lck);
}

void writer2 (char msg, int lck, int lprio)
{
	kprintf ("  %c: to acquire lock\n", msg);
        lock (lck, WRITE, lprio);
        output2[count2++]=msg;
        kprintf ("  %c: acquired lock, sleep 3s\n", msg);
        sleep (1);
        output2[count2++]=msg;
	//kprintf("\n Writer Output before release :");
        //kprintf("\t %s", output2);
        //kprintf ("  %c: to release lock\n", msg);
        releaseall (1, lck);
}

void test2 ()
{
        count2 = 0;
        int     lck;
        int     rd1, rd2, rd3, rd4;
        int     wr1;

        kprintf("\nTest 2: wait on locks with priority. Expected order of"
		" lock acquisition is: reader A, reader B, reader D, writer C & reader E\n");
        lck  = lcreate ();
        //kprintf("\nLock value in Test 2 : %d", lck);
	assert (lck != SYSERR, "Test 2 failed");
	//kprintf("\nThe pid values are:");
	rd1 = create(reader2, 2000, 20, "reader2", 3, 'A', lck, 20);
	//kprintf("\t A %d", rd1);
	rd2 = create(reader2, 2000, 20, "reader2", 3, 'B', lck, 30);
	//kprintf("\t B %d", rd2);
	rd3 = create(reader2, 2000, 20, "reader2", 3, 'D', lck, 25);
	//kprintf("\t D %d", rd3);
	rd4 = create(reader2, 2000, 20, "reader2", 3, 'E', lck, 20);
	//kprintf("\t E %d", rd4);
        wr1 = create(writer2, 2000, 20, "writer2", 3, 'C', lck, 25);
	//kprintf("\t C %d", wr1);

	
        kprintf("-start reader A, then sleep 1s. lock granted to reader A\n");
        resume(rd1);
        sleep (1);

        kprintf("-start writer C, then sleep 1s. writer waits for the lock\n");
        resume(wr1);
        sleep (1);


        kprintf("-start reader B, D, E. reader B is granted lock.\n");
        resume (rd2);
	resume (rd3);
	resume (rd4);


        sleep (15);
        kprintf("output=%s\n", output2);
    // ABD(ABD in arbitrary orders)CCEE
        assert(mystrncmp(output2,"ABABDDCCEE",10)==0,"Test 2 FAILED\n");
        kprintf ("Test 2 OK\n");
}

/*----------------------------------Test 3---------------------------*/
void reader3 (char *msg, int lck)
{
        int     ret;

        kprintf ("  %s: to acquire lock\n", msg);
        lock (lck, READ, DEFAULT_LOCK_PRIO);
        kprintf ("  %s: acquired lock\n", msg);
        kprintf ("  %s: to release lock\n", msg);
        releaseall (1, lck);
}

void writer3 (char *msg, int lck)
{
        kprintf ("  %s: to acquire lock\n", msg);
        lock (lck, WRITE, DEFAULT_LOCK_PRIO);
        kprintf ("  %s: acquired lock, sleep 10s\n", msg);
        sleep (10);
        kprintf ("  %s: to release lock\n", msg);
        releaseall (1, lck);
}

void test3 ()
{
        int     lck;
        int     rd1, rd2;
        int     wr1;

        kprintf("\nTest 3: test the basic priority inheritence\n");
        lck  = lcreate ();
        assert (lck != SYSERR, "Test 3 failed");

        rd1 = create(reader3, 2000, 25, "reader3", 2, "reader A", lck);
	//kprintf("\n pid of rd1 %d", rd1);
        rd2 = create(reader3, 2000, 30, "reader3", 2, "reader B", lck);
	//kprintf("\n pid of rd2 %d", rd2);
        wr1 = create(writer3, 2000, 20, "writer3", 2, "writer", lck);
	//kprintf("\n pid of wr1 %d", wr1);
        kprintf("-start writer, then sleep 1s. lock granted to write (prio 20)\n");
        resume(wr1);
        sleep (1);

        kprintf("-start reader A, then sleep 1s. reader A(prio 25) blocked on the lock\n");
        resume(rd1);
        sleep (1);
	//kprintf("\t  getprio(wr1) %d",getprio(wr1));
	assert (getprio(wr1) == 25, "Test 3 failed");

        kprintf("-start reader B, then sleep 1s. reader B(prio 30) blocked on the lock\n");
        resume (rd2);
	sleep (1);
	//kprintf("\n getprio(wr1) %d",getprio(wr1));
	assert (getprio(wr1) == 30, "Test 3 failed");
	kprintf("-kill reader B, then sleep 1s\n");
	kill (rd2);
	sleep (1);
	//kprintf("\n getprio(wr1) %d",getprio(wr1));
	assert (getprio(wr1) == 25, "Test 3 failed");

	kprintf("-kill reader A, then sleep 1s\n");
	kill (rd1);
	sleep(1);
	assert(getprio(wr1) == 20, "Test 3 failed");

        sleep (8);
        kprintf ("Test 3 OK\n");
}

int main( )
{
 /*      * These test cases are only used for test purpose.
         * The provided results do not guarantee your correctness.
         * You need to read the PA2 instruction carefully.
         */
	//kprintf("Entered the main function");
	test1();
	test2();
	test3();

        /* The hook to shutdown QEMU for process-like execution of XINU.
         * This API call exists the QEMU process.
         */
        shutdown();
}









