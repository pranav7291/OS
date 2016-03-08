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

struct proc_table *p_table;


/*int pt_init() {
	if (p_table == NULL) {
		p_table = kmalloc(sizeof(p_table));
	}
	if (p_table->pt_proc == NULL) {
		p_table->pt_proc = malloc(sizeof(p_table->pt_proc) * __PID_MAX);
	}
	return 0;
}
*
 * inserts process into file table, returns PID

pid_t insert_process_into_file_table(struct proc *newproc) {
	pid_t i = 0;
	for (i = 0; i < __PID_MAX; i++) {
		if (p_table->pt_proc[i] != NULL) {
			continue;
		} else {
			p_table->pt_proc[i] = newproc;
			break;
		}
	}
	return i;

}*/

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
	newproc = kmalloc(sizeof(*newproc));

	//Copy parent’s address space
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
	}

	if(thread_fork("Child Thread", newproc,
					entrypoint,  (void* ) tf_child,
					(unsigned long)child_addr)) {
		return ENOMEM;
	}

	//sammokka
	if (p_table == NULL) {
		p_table = kmalloc(sizeof(p_table));
	}

	//find a null entry in p_table
	int j = 1;
	for (int j = 1; j < __PID_MAX; j++) {
		if (p_table->pt_proc[j] == NULL) {
			p_table->pt_proc[j] = kmalloc(sizeof(struct proc));
			break; //j is the index of the null entry
		}
	}

	//put the process in the file table, return pid.


	newproc->pid = j;

	p_table->pt_proc[j] = newproc;
	if (newproc == NULL) {
			return ENOMEM;
		}

	*retval = newproc->pid;

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







/*

void sys__exit(int exitcode) {
	//kprintf("Child PID: %d, Parent PID %d\n",curthread->pid, curthread->ppid);

	struct process_table_entry * temp1;
	for(temp1=process_table; temp1->pid!=curthread->ppid || temp1 == NULL; temp1=temp1->next);

	if (temp1 == NULL ) {
		//destroy_process(curthread->pid);
	} else if (temp1->procs->exited == false) {

		struct process_table_entry * temp2;
		for(temp2=process_table; temp2->pid!=curthread->pid || temp2==NULL; temp2=temp2->next);
		if(temp2 == NULL){
			kprintf("Process with PID %d not present in process table to exit/n",curthread->pid);
		}
		temp2->procs->exitcode = _MKWAIT_EXIT(exitcode);
		//kprintf("Exitcode: %d\n",process_table[curthread->pid]->exitcode);
		temp2->procs->exited = true;
		V(temp2->procs->exitsem);
	} else {
		destroy_process(curthread->pid);
	}

	thread_exit();
}
*/
