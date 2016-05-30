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

//char *helper[4000];

//initializes file table
void pt_init() {
	for (int i = 0; i < 256; i++) {
		pt_proc[i] = NULL;
	}
	p_lock = lock_create("ptable lock");
}

// inserts process into file table, returns PID
pid_t insert_process_into_process_table(struct proc *newproc) {

	lock_acquire(p_lock);
	pid_t i = 2;
	for (i = 2; i < 256; i++) {
		if (pt_proc[i] == NULL) {
			newproc->pid = i;
			newproc->parent_pid = curproc->pid;
			pt_proc[i] = newproc;
			break;
		}
	}

	lock_release(p_lock);
	if (i == 256) {
		//kprintf("out of proc table slots!!\n");
		return -1;
	} else {
		return i;
	}
}

static void entrypoint(void* data1, unsigned long data2) {

	struct trapframe tf;
	data2 = data2 + 100;

	tf = *(struct trapframe *) data1;

	tf.tf_a3 = 0;
	tf.tf_v0 = 0;
	tf.tf_epc += 4;

	kfree(data1);
	as_activate();
	mips_usermode(&tf);
}

int sys_fork(struct trapframe *tf, int *retval) {

	//create new thread
//	kprintf(".");
	*retval = -1;
	struct proc *newproc;
	newproc = proc_create_runprogram("name");
	if (newproc == NULL) {
		return ENOMEM;
	}

	//Copy parents address space
	if (as_copy(curproc->p_addrspace, &newproc->p_addrspace)) {
		return ENOMEM;
	}

	//Copy parents trapframe
	struct trapframe *tf_child = kmalloc(sizeof(struct trapframe));
	if (tf_child == NULL) {
		return ENOMEM;
	}
	*tf_child = *tf;
	//kprintf("tf_child mem -> %p\n", tf_child);
	//kprintf("tf mem -> %p\n", tf);

	//copy parents filetable entries
	for (int k = 0; k < OPEN_MAX; k++) {
		if (curproc->proc_filedesc[k] != NULL) {
			lock_acquire(curproc->proc_filedesc[k]->fd_lock);
			newproc->proc_filedesc[k] = curproc->proc_filedesc[k];
			newproc->proc_filedesc[k]->fd_refcount++;
			lock_release(curproc->proc_filedesc[k]->fd_lock);

		}
	}

	newproc->p_cwd = curproc->p_cwd;
	VOP_INCREF(curproc->p_cwd);

	if (thread_fork("Child Thread", newproc, entrypoint, (void*) tf_child,
			(unsigned long) 0)) {
		return ENOMEM;
	}
	//kprintf("forked to pid->%d", newproc->pid);
	*retval = newproc->pid;
	return 0;
}

int sys_getpid(int *retval) {
	*retval = curproc->pid;
	return 0;
}

pid_t sys_waitpid(pid_t pid, int *status, int options, int *retval) {
	//kprintf("\nwaiting on pid %d", pid);
	*retval = -1;
	if (pid == curproc->parent_pid || pid == curproc->pid) {
		return ECHILD;
	}

	if (options != 0 && options != 1000) {
		return EINVAL;
	}

	if (status == (void *) 0x40000000 || status == (void *) 0x80000000) {
		return EFAULT;
	}

	if (pid > PID_MAX || pid < PID_MIN) {
		return ESRCH;
	}

	if (pt_proc[pid] == NULL) {
		//process does not exist (invalid pid)
		return ESRCH;
	}

	if (options != 1000) {
		if (pt_proc[pid]->parent_pid == curproc->parent_pid) {
			return ECHILD;
		}
	}

	if (pt_proc[pid]->isexited == false) {
		P(pt_proc[pid]->proc_sem);
	}

	*retval = pid;

	if (status != NULL) {
		int result = copyout((const void *) &(pt_proc[pid]->exitcode),
				(userptr_t) status, sizeof(int));

		if (result) {
			proc_destroy(pt_proc[pid]);
			pt_proc[pid] = NULL;
			return result;
		}
	}

	proc_destroy(pt_proc[pid]);
	pt_proc[pid] = NULL;
	//kprintf("\ndestroyed pid %d", pid);
	return 0;
}

int sys_execv(const char *program, char **uargs, int *retval) {

	struct vnode *v;
	vaddr_t entrypoint, stackptr;
	int result, argmax;
	size_t length;
	char *name;
	int i = 0;

	*retval = -1;

	if (uargs == NULL || program == NULL) {
		return EFAULT;
	}

	size_t length1;
	result = copycheck1((const_userptr_t) program, PATH_MAX, &length1);
	if (result) {
		return result;
	}

	result = copycheck1((const_userptr_t) uargs, PATH_MAX, &length1);
	if (result) {
		return result;
	}

	name = (char *) kmalloc(sizeof(char) * PATH_MAX);
	if (name == NULL) {
		return ENOMEM;
	}
	result = copyinstr((const_userptr_t) program, name, PATH_MAX, &length);
	if (result) {
		kfree(name);
		return EFAULT;
	}

	if ((void *) program == (void *) 0x40000000
			|| (void *) uargs == (void *) 0x40000000) {
		kfree(name);
		return EFAULT;
	}

	if ((char *) program == NULL || (char *) program == '\0') {
		kfree(name);
		return EINVAL;
	}

	if (length < 2 || length > PATH_MAX) {
		kfree(name);
		return EINVAL;
	}

	if ((void *) uargs[1] == (void *) 0x40000000) {
		kfree(name);
		return EFAULT;
	}

	int len = 0;
	int act = 0;
	int x = 0;
	int totallen = 0;
	while (uargs[x] != NULL) {
		len = strlen(uargs[x]) + 1;
		if ((len % 4) != 0) {
			len = len - (len % 4) + 4;
		}
		totallen = totallen + len;
		x++;
	}

	char *buf = (char *) kmalloc(sizeof(char) * (totallen + 1));
	if (buf == NULL) {
		kfree(name);
		return ENOMEM;
	}
	char **ptrbuf = (char **) kmalloc(sizeof(char **) * x);
	if (ptrbuf == NULL) {
		kfree(name);
		kfree(buf);
		return ENOMEM;
	}
	char *temp = buf;
	while (uargs[i] != NULL) {
		length = 0;
		len = strlen(uargs[i]) + 1;
		act = len;
		if ((len % 4) != 0) {
			len = len - (len % 4) + 4;
		}
		result = copyinstr((const_userptr_t) uargs[i], temp,
				(sizeof(char) * (strlen(uargs[i]) + 1)), &length);
		if (result) {
			kfree(name);
			kfree(ptrbuf);
			kfree(buf);
			return EFAULT;
		}

		temp = temp + (strlen(uargs[i]) + 1);
		while (act < len) {
			*temp = '\0';
			temp++;
			act++;
		}

		i++;
	}
	argmax = i;

	//Now proceeding as in runprogram
	/* Open the file. */
	result = vfs_open(name, O_RDONLY, 0, &v);
	if (result) {
		kfree(name);
		kfree(ptrbuf);
		kfree(buf);
		return result;
	}

	/* We should be a new process. */
	as_destroy(curproc->p_addrspace);
	curproc->p_addrspace = NULL;
	KASSERT(proc_getas() == NULL);

	/* Create a new address space. */
	curproc->p_addrspace = as_create();
	if (curproc->p_addrspace == NULL) {
		vfs_close(v);
		kfree(name);
		kfree(ptrbuf);
		kfree(buf);
		return ENOMEM;
	}

	/* Switch to it and activate it. */
	proc_setas(curproc->p_addrspace);
	as_activate();

	/* Load the executable. */
	result = load_elf(v, &entrypoint);
	if (result) {
		/* p_addrspace will go away when curproc is destroyed */
		vfs_close(v);
		kfree(name);
		kfree(ptrbuf);
		kfree(buf);
		return result;
	}

	/* Done with the file now. */
	vfs_close(v);

	/* Define the user stack in the address space */
	result = as_define_stack(curproc->p_addrspace, &stackptr);
	if (result) {
		/* p_addrspace will go away when curproc is destroyed */
		kfree(name);
		kfree(ptrbuf);
		kfree(buf);
		return result;
	}

	//Copying arguments to user space
	i = 0;
	temp = buf;
	char *temp1 = buf;
	while (i < argmax) {
		length = 0;
		len = 0;
		while (*temp != '\0') {
			temp++;
			len++;
		}
		temp++;
		len++;
		if ((len % 4) != 0) {
			temp = temp + 4 - (len % 4);
			len = len - (len % 4) + 4;
		}

		stackptr = stackptr - len;
		act = len;
		result = copyout((const void *) (temp1), (userptr_t) stackptr, len);
		if (result) {
			kfree(name);
			kfree(ptrbuf);
			kfree(buf);
			return EFAULT;	//not sure whether to return this or result
		}
		temp1 = temp1 + (act);
		ptrbuf[i] = (char *) stackptr;

		i++;
	}

	stackptr = stackptr - 4 * sizeof(char);

	//Copying the pointers
	for (i = argmax - 1; i >= 0; i--) {
		stackptr = stackptr - sizeof(char *);
		result = copyout((const void *) (ptrbuf + i), (userptr_t) stackptr,
				sizeof(char *));
		if (result) {
			kfree(name);
			kfree(ptrbuf);
			kfree(buf);
			return EFAULT;	//again, result or this?
		}
	}
	kfree(name);
	kfree(ptrbuf);
	kfree(buf);

	/* Warp to user mode. */
	enter_new_process(argmax /*argc*/,
			(userptr_t) stackptr /*userspace addr of argv*/,
			NULL /*userspace addr of environment*/, stackptr, entrypoint);

	/* enter_new_process does not return. */
	panic("panic - execv, After enter_new_process\n");
	return EINVAL;

}

int sys_exit(int code) {
	//kprintf("exiting pid %d", curproc->pid);
	struct proc * proc = curproc;
	(void) proc;
	curproc->isexited = true;
	if (code == 0) {
		curproc->exitcode = _MKWAIT_EXIT(code);
	} else {
		curproc->exitcode = _MKWAIT_SIG(code);
	}
	V(curproc->proc_sem);

	thread_exit();

	return 0;
}

int sys_sbrk(int amt, int *retval) {
	struct addrspace * as = curproc->p_addrspace;
	vaddr_t heap_top = curproc->p_addrspace->heap_top;
	vaddr_t heap_bottom = curproc->p_addrspace->heap_bottom;
	*retval = -1;
	if (amt == 0) {
		*retval = heap_top;
		return 0;
	}

	if ((amt % 4) != 0) {	//check if it's aligned by 4
		return EINVAL;
	}

	if (amt < 0) {	//heap size decreased
		if ((long) heap_top + (long) amt < (long) heap_bottom) {
			return EINVAL;
		}
		amt *= -1;
		//		vm_tlbshootdown_all();
		if (amt >= PAGE_SIZE) {
			for (unsigned i = ((int) (heap_top & PAGE_FRAME) - amt);
					i < (heap_top & PAGE_FRAME); i = i + PAGE_SIZE) {
				struct PTE *prev, *curr;
				if (as->pte->vpn == i) {
					curr = as->pte;
					as->pte = as->pte->next;
					if (swapping) {
						if (curr->state == MEM) {
							page_free(curr->ppn);
						}
					} else {
						page_free(curr->ppn);
					}
					vm_tlbshootdownvaddr(curr->vpn);
					if (swapping) {
						lock_destroy(curr->pte_lock);
					}
					kfree(curr);
				} else {
					prev = as->pte;
					for (curr = as->pte->next; curr != NULL; curr =
							curr->next) {
						if (curr->vpn == i) {
							if (swapping) {
								if (curr->state == MEM) {
									page_free(curr->ppn);
								}
							} else {
								page_free(curr->ppn);
							}
							vm_tlbshootdownvaddr(curr->vpn);
							prev->next = curr->next;
							if (swapping) {
								lock_destroy(curr->pte_lock);
							}
							kfree(curr);
							break;
						}
						prev = prev->next;
					}
				}

			}
			*retval = heap_top;
			heap_top -= (amt & PAGE_FRAME);
		} else {
			*retval = heap_top;
			heap_top -= (amt & PAGE_FRAME);
			vm_tlbshootdownvaddr(heap_top);
		}
	} else {
		if ((heap_top + amt) > (USERSTACK - MYVM_STACKPAGES * PAGE_SIZE)) {
			return ENOMEM;
		}

		if ((long) amt >= (long) PAGE_SIZE) {
			if ((long) amt > (long) (as->stack_ptr - as->heap_top)) {
				return ENOMEM;
			}
		}
		*retval = heap_top;
		heap_top += amt;
	}
	curproc->p_addrspace->heap_top = heap_top;
	return 0;
}
