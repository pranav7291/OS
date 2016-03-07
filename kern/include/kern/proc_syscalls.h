#include <kern/fcntl.h>
#include<limits.h>

static struct proc_table {
	struct proc *pt_proc[__PID_MAX];
};
