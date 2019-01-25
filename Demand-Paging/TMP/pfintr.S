/* pfintr.S - pfintr */


    	   .text
pferrcode: .long 0
           .globl  pfintr,pferrcode 
pfintr:
	popl pferrcode  // 1) Storing the error code 
	pushfl 		// 2) Storing the flag register
	cli		// 3) Cleaing all interrupts
	pushal		// 4) Save all general registers
	call pfint	// 5) Page Fault Handling
	popal		// 6) Restore General Registers
	popfl		// 7) Restore Flag Register
	iret		// 8) iret


