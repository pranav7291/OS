#include <types.h>
#include <clock.h>
#include <copyinout.h>
#include <current.h>
#include <vfs.h>
#include <vnode.h>
#include <synch.h>
#include <proc.h>
#include <limits.h>
#include <kern/errno.h>
#include <kern/stat.h>
#include <kern/iovec.h>
#include <uio.h>
#include <proc.h>
#include <addrspace.h>
#include <mips/trapframe.h>
#include <kern/proc_syscalls.h>
#include <kern/wait.h>


//struct lock *p_lock;

struct proc *pt_proc[256];

//initializes file table
void pt_init() {
	for (int i = 1; i < 256; i++) {
		pt_proc[i]= NULL;
	}
}

// inserts process into file table, returns PID

pid_t insert_process_into_process_table(struct proc *newproc) {

//	lock_acquire(p_lock);
	pid_t i = 1;
	for (i = 1; i < 256; i++) {
		if (pt_proc[i] == NULL) {
			kprintf("inserting process slot at pid->%d\n", i);
			newproc->pid = i;
			newproc->parent_pid = curproc->pid;
			pt_proc[i] = newproc;
			kprintf("mem of newproc %p, ", newproc );
			kprintf("mem of pt_proc[i] %p\n",pt_proc[i] );
			kprintf("pid of newproc %d, ", newproc->pid );
			kprintf("pid of pt_proc[i] %d\n", pt_proc[i]->pid);
			break;
		}
	}

//	lock_release(p_lock);
	if (i == 256) {
		kprintf("out of proc table slots!!\n");
		return ENOMEM;
	} else {
		return i;
	}
}


/**
 * sammmokka
 */

static void entrypoint(void* data1, unsigned long data2) {
	struct trapframe *tf,  temptf;
	struct addrspace * addr;

	tf = (struct trapframe *) data1;
	addr = (struct addrspace *) data2;

	tf->tf_a3 = 0;
	tf->tf_v0 = 0;
	tf->tf_epc += 4;

	curproc->p_addrspace = addr;
	as_activate();

	temptf = *tf; //because apparently i  cant pass the direct pointer for some reason o.O
	mips_usermode(&temptf);
}



int sys_fork(struct trapframe *tf, int *retval)  {

	//create new thread

	struct proc *newproc;
	newproc = proc_create_runprogram("name");

	//Copy parent�s address space
	struct addrspace * child_addr;
	if (as_copy(curproc->p_addrspace, &child_addr)) {
		return ENOMEM;
	}

	//Copy parents trapframe
	struct trapframe *tf_child = kmalloc(sizeof(struct trapframe));
	*tf_child = *tf;

	//copy parents filetable entries
	for (int k = 0; k < OPEN_MAX; k++) {
		newproc->proc_filedesc[k] = curproc->proc_filedesc[k];
		if (newproc->proc_filedesc[k]!=NULL) {
			newproc->proc_filedesc[k]->fd_refcount++;
		}
	}
	*retval = newproc->pid;

	if(thread_fork("Child Thread", newproc,
					entrypoint,  (void* ) tf_child,
					(unsigned long)child_addr)) {
		return ENOMEM;
	}


	//sammokka end

	return 0;
}

/**
 * added by sammokka
 */
int sys_getpid(int *retval) {
	*retval = curproc->pid;
	return 0;
}


/**added by sammokka
 *
 *
 */
pid_t
sys_waitpid(pid_t pid, int *status, int options, int *retval) {
	if (pid == curproc->pid) {
		return ECHILD;
	}

	if(options!=0) {
		return EINVAL;
	}

	if (pt_proc[pid] == NULL) {
		//process does not exist (invalid pid)
		*retval = -1;
		return ESRCH;
	}

	if (pt_proc[pid]->isexited == true) {
		*retval = pid;
	}

	if (pt_proc[pid]->isexited == false) {
			V(pt_proc[pid]->proc_sem);
	}

	if(copyout((const void *) &(pt_proc[pid]->isexited), (userptr_t) status,
				sizeof(int))) {
		return EFAULT;
	}
	sem_destroy(pt_proc[pid]->proc_sem);
	kfree(pt_proc[pid]->proc_sem);
	kfree(pt_proc[pid]);
	proc_destroy(pt_proc[pid]);

	return 0;

	//note: status CAN be null

}

int sys_exit(pid_t pid) {
	pt_proc[pid]->isexited = true;
	P(pt_proc[pid]->proc_sem);
	curproc->exitcode = _MKWAIT_EXIT(pid);
	thread_exit();
	return 0;
}