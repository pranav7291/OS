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


struct lock *p_lock;

struct proc *pt_proc[256];

//initializes file table
void pt_init() {
	//kprintf("creating the proc table");
	for (int i = 0; i < 256; i++) {
		pt_proc[i]= NULL;
	}

	p_lock = lock_create("ptable lock");
}

// inserts process into file table, returns PID

pid_t insert_process_into_process_table(struct proc *newproc) {

	lock_acquire(p_lock);
	pid_t i = 1;
	for (i = 1; i < 256; i++) {
		if (pt_proc[i] == NULL) {
			//kprintf("inserting process slot at pid->%d\n", i);
			newproc->pid = i;
			newproc->parent_pid = curproc->pid;
			pt_proc[i] = newproc;
			break;
		}
	}

	lock_release(p_lock);
	if (i == 256) {
		//kprintf("out of proc table slots!!\n");
		return ENOMEM;
	} else {
		return i;
	}
}


/**
 * sammmokka
 */

static void entrypoint(void* data1, unsigned long data2) {

	struct trapframe tf;
	data2=data2+100;

	tf = *(struct trapframe *) data1;

	tf.tf_a3 = 0;
	tf.tf_v0 = 0;
	tf.tf_epc += 4;

	kfree(data1);
	as_activate();
	mips_usermode(&tf);
}



int sys_fork(struct trapframe *tf, int *retval)  {

	//create new thread

	struct proc *newproc;
	newproc = proc_create_runprogram("name");

	//Copy parent’s address space
	if (as_copy(curproc->p_addrspace,&newproc->p_addrspace)) {
		return ENOMEM;
	}

	//Copy parents trapframe
	struct trapframe *tf_child = kmalloc(sizeof(struct trapframe));
	*tf_child = *tf;
	//kprintf("tf_child mem -> %p\n", tf_child);
	//kprintf("tf mem -> %p\n", tf);

	//copy parents filetable entries
	for (int k = 0; k < OPEN_MAX; k++) {
		if (curproc->proc_filedesc[k]!=NULL) {
			newproc->proc_filedesc[k] = curproc->proc_filedesc[k];
			newproc->proc_filedesc[k]->fd_refcount++;
		}
	}

	*retval = newproc->pid;

	if(thread_fork("Child Thread", newproc,
					entrypoint,  (void* ) tf_child,
					(unsigned long)0)) {
		return ENOMEM;
	}

	//kprintf("forked to pid->%d", newproc->pid);


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
	*retval=-1;
	if(options!=0 && options!=909) {
		return EINVAL;
	}

	if (pid == curproc->pid) {
			return ECHILD;
		}

	if(status==NULL || status==(void *)0x40000000 || status==(void *)0x80000000)
		return EFAULT;

//	if(pid<PID_MIN)
//		return EINVAL;
//
//	if(pid>PID_MAX)
//		return ESRCH;

	if (pt_proc[pid] == NULL) {
		return ESRCH;
	}


	if (pt_proc[pid]->isexited == true) {
		*retval = pid;
	}

	//if (pt_proc[pid]->isexited == false) {
			P(pt_proc[pid]->proc_sem);
	//}

	if(copyout((const void *) &(pt_proc[pid]->isexited), (userptr_t) status,
				sizeof(int))) {
		return EFAULT;
	}
	sem_destroy(pt_proc[pid]->proc_sem);
	proc_destroy(pt_proc[pid]);
	pt_proc[pid] = NULL;
	return 0;

	//note: status CAN be null

}

int sys_exit(int code) {
	//kprintf("exiting ...");
	curproc->isexited = true;
	V(curproc->proc_sem);
	curproc->exitcode = _MKWAIT_EXIT(code);
	thread_exit();
	return 0;
}
