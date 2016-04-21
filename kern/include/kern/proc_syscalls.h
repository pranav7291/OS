#include <kern/fcntl.h>
#include <limits.h>

#include <mips/trapframe.h>

extern struct proc *pt_proc[256];



extern struct lock *p_lock;


void pt_init(void);

pid_t insert_process_into_process_table(struct proc *newproc);

void sys__exit(int exitcode);

int sys_fork(struct trapframe *tf, int *retval);

int sys_execv(const char *program, char **uargs, int *retval);

int sys_getpid(int *retval);

pid_t
sys_waitpid(pid_t pid, int *status, int options, int *retval);

int sys_exit(pid_t pid);

int sys_sbrk(int amt, int *retval);
