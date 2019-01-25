
#define NLOCKS 50 /* number of LOCKS */

#define LFREE '\01' /* this lock is free */
#define LUSED '\02' /* this lock is used */
#define READ  0 /* READ lock state */
#define WRITE 1 /* WRITE lock state */

struct lentry {
	char lstate;	/* the state lFREE or LUSED */
	int loccnt;	/* count for this lock */
	int lqhead;	/* q index of head of wait list */
	int lqtail;	/* q index of tail of wait list */
	int prio_max;   /* Maximum priority of all the process in wait queue  */
	int ltype; 	/* the type is either READ or WRITE */
	int proc_queue[NLOCKS]; //= {-1};
	int reader_count;	
};

extern struct lentry locarr[];
extern int nextloc;
extern int lock_proc[NPROC][NPROC];
void max_prio_inwaitq(int loc, int curr);
void lock_prio_process(int proc);
void lock_acquire(int curr, int loc);
#define isbadloc(l)	(l < 0 || l >= NLOCKS)




