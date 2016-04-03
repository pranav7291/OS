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
#include <syscall.h>


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

	//Copy parents address space
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
			lock_acquire(curproc->proc_filedesc[k]->fd_lock);
			newproc->proc_filedesc[k] = curproc->proc_filedesc[k];
			newproc->proc_filedesc[k]->fd_refcount++;
			lock_release(curproc->proc_filedesc[k]->fd_lock);

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
	if (pid == curproc->parent_pid || pid == curproc->pid) {
			return ECHILD;
		}

	if(options!=0) {
		return EINVAL;
	}

	if (pt_proc[pid] == NULL) {
		//process does not exist (invalid pid)
//		*retval = -1;
		return ESRCH;
	}


	if (pt_proc[pid]->isexited == true) {
		*retval = pid;
	}

	if (pt_proc[pid]->isexited == false) {
			P(pt_proc[pid]->proc_sem);
	}


	if(copyout((const void *) &(pt_proc[pid]->isexited), (userptr_t) status,
				sizeof(int))) {
		return EFAULT;
	}
	*retval = pid;
	sem_destroy(pt_proc[pid]->proc_sem);
	*status = pt_proc[pid]->exitcode;
	proc_destroy(pt_proc[pid]);
	pt_proc[pid] = NULL;
	return 0;

	//note: status CAN be null

	//hello

}

int sys_execv(const char *program, char **uargs, int *retval){

	struct vnode *v;
	vaddr_t entrypoint, stackptr;
	int result, argmax;
	size_t length;
	char *name;
	int i = 0;

	if (uargs == NULL || program == NULL){
		*retval = -1;
		return EFAULT;
	}

	*retval = -1;

	name = kmalloc(sizeof(char) *PATH_MAX);
	result = copyinstr((const_userptr_t)program, name, sizeof(name),&length);
	if (result){
		kfree(name);
		return EFAULT;
	}

	char **args = (char **) kmalloc(sizeof(char**));
	result = copyin((const_userptr_t)uargs, args, sizeof(args));

	if (result){
		kfree(name);
		kfree(args);
		return EFAULT;
	}

	//copy arguments from user space to kernel space
	while (uargs[i] != NULL ) {
		args[i] = (char *) kmalloc(sizeof(uargs[i]));
		result = copyinstr((const_userptr_t) uargs[i], args[i], PATH_MAX, &length);
		if (length > ARG_MAX)
			return E2BIG;
		if (result) {
			kfree(name);
			kfree(args);
			return EFAULT;
		}
		i++;
	}
	argmax = i;

	//Now proceeding as in runprogram
	/* Open the file. */
	result = vfs_open(name, O_RDONLY, 0, &v);
	if (result) {
		kfree(name);
		kfree(args);
		return result;
	}

	/* We should be a new process. */
//	KASSERT(proc_getas() == NULL);

	/* Create a new address space. */
	curproc->p_addrspace = as_create();
	if (curproc->p_addrspace == NULL) {
		kfree(name);
		kfree(args);
		vfs_close(v);
		return ENOMEM;
	}

	/* Switch to it and activate it. */
	proc_setas(curproc->p_addrspace);
	as_activate();

	/* Load the executable. */
	result = load_elf(v, &entrypoint);
	if (result) {
		/* p_addrspace will go away when curproc is destroyed */
		kfree(name);
		kfree(args);
		vfs_close(v);
		return result;
	}

	/* Done with the file now. */
	vfs_close(v);

	/* Define the user stack in the address space */
	result = as_define_stack(curproc->p_addrspace, &stackptr);
	if (result) {
		/* p_addrspace will go away when curproc is destroyed */
		kfree(name);
		kfree(args);
		return result;
	}

	//Copying arguments to user space
	i = 0;

	while(i < argmax){
		length = strlen(args[i]) + 1;	//for \0
		int actlen = length;

		if ((length % 4)!= 0){
			length = length - (length % 4) + 4;
		}

		char *curarg = kmalloc(sizeof(length));

		for (int j = 0; j < (int)length; j++){
			if (j < actlen)
				curarg[j] = args[i][j];
			else
				curarg[j] = '\0';
		}

		stackptr = stackptr - length;

		result = copyout((const void *)curarg, (userptr_t)stackptr, length);
		if (result){
			kfree(name);
			kfree(args);
			kfree(curarg);
			return EFAULT;	//not sure whether to return this or result
		}

		args[i] = (char *)stackptr;

		kfree(curarg);

		i++;
	}

	stackptr = stackptr - 4*sizeof(char);

	//Copying the pointers
	for(i = argmax - 1; i >= 0; i--){
		stackptr = stackptr - sizeof(char *);
		result = copyout((const void *)(args + i), (userptr_t)stackptr, sizeof(char *));
		if (result){
			kfree(name);
			kfree(args);
			return EFAULT;	//again, result or this?
		}
	}

	/* Warp to user mode. */
	enter_new_process(argmax /*argc*/, NULL /*userspace addr of argv*/,
				 NULL /*userspace addr of environment*/,
				 stackptr, entrypoint);

	/* enter_new_process does not return. */
	panic("panic - execv, After enter_new_process\n");
	return EINVAL;

}

int sys_exit(int code) {
	//kprintf("exiting ...");
	curproc->isexited = true;
	curproc->exitcode = _MKWAIT_EXIT(code);
	V(curproc->proc_sem);
	thread_exit();
	return 0;
}
