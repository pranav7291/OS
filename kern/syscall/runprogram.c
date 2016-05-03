/*
 * Copyright (c) 2000, 2001, 2002, 2003, 2004, 2005, 2008, 2009
 *	The President and Fellows of Harvard College.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE UNIVERSITY AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE UNIVERSITY OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Sample/test code for running a user program.  You can use this for
 * reference when implementing the execv() system call. Remember though
 * that execv() needs to do more than runprogram() does.
 */

#include <types.h>
#include <kern/errno.h>
#include <kern/fcntl.h>
#include <lib.h>
#include <proc.h>
#include <current.h>
#include <addrspace.h>
#include <vm.h>
#include <vfs.h>
#include <syscall.h>
#include <test.h>
#include <synch.h>

/*
 * Load program "progname" and start running it in usermode.
 * Does not return except on error.
 *
 * Calls vfs_open on progname and thus may destroy it.
 */
int
runprogram(char *progname)
{

//	kprintf("\n\n\n***Inside runprogram.c\n\n\n");

	struct addrspace *as;
	struct vnode *v;
	vaddr_t entrypoint, stackptr;
	int result;

	/* Open the file. */
	result = vfs_open(progname, O_RDONLY, 0, &v);
	if (result) {
		return result;
	}

	/* We should be a new process. */
	KASSERT(proc_getas() == NULL);

	/* Create a new address space. */
	as = as_create();
	if (as == NULL) {
		vfs_close(v);
		return ENOMEM;
	}

	/* Switch to it and activate it. */
	proc_setas(as);
	as_activate();
	//logic for creating console streams

	//console in
	if(curproc->proc_filedesc[0]==NULL){
	struct vnode *ret_in; //empty nvnode

	struct filedesc *filedesc_ptr_in;
	filedesc_ptr_in = kmalloc(sizeof(*filedesc_ptr_in));
	if(filedesc_ptr_in == NULL){
		vfs_close(v);
		return ENOMEM;
	}
	mode_t mode = 0664;
	filedesc_ptr_in->name = kstrdup("con:");
	filedesc_ptr_in->flags = O_RDONLY;

	int returner_in = vfs_open(filedesc_ptr_in->name, filedesc_ptr_in->flags, mode,
			&ret_in);
	if (returner_in) {
		vfs_close(v);
		kfree(filedesc_ptr_in);
		return returner_in;
	}

	filedesc_ptr_in->fd_lock = lock_create("con:input"); //not sure when i should use this lock
	if(filedesc_ptr_in->fd_lock == NULL){
		vfs_close(v);
		kfree(filedesc_ptr_in);
		return ENOMEM;
	}
	filedesc_ptr_in->isempty = 0; //not empty
	filedesc_ptr_in->fd_vnode = ret_in; //pointer to vnode object to be stored in filedesc->vnode
	filedesc_ptr_in->read_count = 1;
	filedesc_ptr_in->offset = 0;
	filedesc_ptr_in->fd_refcount = 1;



	curproc->proc_filedesc[0] = filedesc_ptr_in;

	//output stream
	struct vnode *ret_out; //empty nvnode

	struct filedesc *filedesc_ptr_out;
	mode = 0664;
	filedesc_ptr_out = kmalloc(sizeof(*filedesc_ptr_out));
	if(filedesc_ptr_out == NULL){
		vfs_close(v);
		kfree(filedesc_ptr_in);
		return ENOMEM;
	}
	filedesc_ptr_out->flags = O_WRONLY;
	filedesc_ptr_out->name = kstrdup("con:");

	int returner_out = vfs_open(filedesc_ptr_out->name, filedesc_ptr_out->flags, mode,
			&ret_out);

	if (returner_out) {
		vfs_close(v);
		kfree(filedesc_ptr_in);
		kfree(filedesc_ptr_out);
		return returner_out;
	}

	filedesc_ptr_out->fd_lock = lock_create("con:output"); //not sure when i should use this lock
	if(filedesc_ptr_out->fd_lock == NULL){
		vfs_close(v);
		kfree(filedesc_ptr_in);
		kfree(filedesc_ptr_out);
		return ENOMEM;
	}
	filedesc_ptr_out->isempty = 0; //not empty
	filedesc_ptr_out->fd_vnode = ret_out; //pointer to vnode object to be stored in filedesc->vnode
	filedesc_ptr_out->read_count = 1;
	filedesc_ptr_out->offset = 0;
	filedesc_ptr_out->fd_refcount = 1;
	curproc->proc_filedesc[1] = filedesc_ptr_out;


	//console err

	struct vnode *ret_err; //empty nvnode

	struct filedesc *filedesc_ptr_err;
	mode = 0664;
	filedesc_ptr_err = kmalloc(sizeof(*filedesc_ptr_err));
	if(filedesc_ptr_err == NULL){
		vfs_close(v);
		kfree(filedesc_ptr_in);
		kfree(filedesc_ptr_out);
		return ENOMEM;
	}
	filedesc_ptr_err->flags = O_WRONLY;
	filedesc_ptr_err->name = kstrdup("con:");

	int returner_err = vfs_open(filedesc_ptr_err->name, filedesc_ptr_err->flags, mode,
			&ret_err);
	if (returner_err) {
		vfs_close(v);
		kfree(filedesc_ptr_in);
		kfree(filedesc_ptr_out);
		kfree(filedesc_ptr_err);
		return returner_err;
	}

	filedesc_ptr_err->fd_lock = lock_create("con:error"); //not sure when i should use this lock
	if(filedesc_ptr_err->fd_lock == NULL){
		vfs_close(v);
		kfree(filedesc_ptr_in);
		kfree(filedesc_ptr_out);
		kfree(filedesc_ptr_err);
		return ENOMEM;
	}
	filedesc_ptr_err->isempty = 0; //not empty
	filedesc_ptr_err->fd_vnode = ret_err; //pointer to vnode object to be stored in filedesc->vnode
	filedesc_ptr_err->read_count = 1;
	filedesc_ptr_err->offset = 0;
	filedesc_ptr_err->fd_refcount = 1;


	curproc->proc_filedesc[2] = filedesc_ptr_err;
	}

	/* Load the executable. */
	result = load_elf(v, &entrypoint);
	if (result) {
		/* p_addrspace will go away when curproc is destroyed */
		vfs_close(v);
		kfree(curproc->proc_filedesc[0]);
		kfree(curproc->proc_filedesc[1]);
		kfree(curproc->proc_filedesc[2]);
		return result;
	}

	/* Done with the file now. */
	vfs_close(v);

	/* Define the user stack in the address space */
	result = as_define_stack(as, &stackptr);
	if (result) {
		/* p_addrspace will go away when curproc is destroyed */
		return result;
	}

	/* Warp to user mode. */
	enter_new_process(0 /*argc*/, NULL /*userspace addr of argv*/,
			  NULL /*userspace addr of environment*/,
			  stackptr, entrypoint);

	/* enter_new_process does not return. */
	panic("enter_new_process returned\n");
//	kprintf("process created, with name %s", curproc->p_name);
//	kprintf("\n\n\n***Exiting runprogram.c\n\n\n");

	return EINVAL;
}
