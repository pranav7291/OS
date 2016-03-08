#include <kern/fcntl.h>
#include <limits.h>

#include <mips/trapframe.h>

struct proc_table {
	struct proc *pt_proc[__PID_MAX ];
};

extern struct proc_table *p_table;

int pt_init();

int insert_process_into_file_table();

void sys__exit(int exitcode);
int pt_init();

int sys_fork(struct trapframe *tf, int *retval);

int sys_getpid(int *retval);
