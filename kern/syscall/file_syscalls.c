//#include <file_syscalls.h>

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


/**
 * Added by sammokka
 *  input:
 *  flags -> is a OR of flags
 *
 *  output: file descriptor
 *
 *  eg: O_WRONLY|O_CREAT|O_TRUNC
 */
int sys_open(char *filename, int flags, int32_t *retval) {

//call vfs_open
//if error, handles

	KASSERT(curthread->t_in_interrupt == false);
	mode_t mode = 0664; //TODO Dunno what this means but whatever.

	kprintf("\nin sys_open..\n");


	char name[100];

	int result;

//	result = copyinstr(usr_ptr_flags, flags, sizeof(flags));

//	if(result) { //memory problem
//		printf("\nSome memory problem, copyin failstryin to copy flags %d\n", result);
//		return result;
//	}

	result = copyinstr((const_userptr_t)filename, name, sizeof(name),NULL);

	if(result) { //memory problem
		kprintf("\nSome memory problem, copyin fails when copy name %d\n", result);
		return result;
	}

	struct vnode *ret; //empty nvnode

	int returner = vfs_open(name, flags, mode, &ret);

	kprintf("the returner of vfs_open is %d", returner);

	if (returner==0) {
		kprintf("successfully opened file %s\n", name);

		//add an fd to the list of fds in the thread's fd table

		//iterate over all the fds to check if there is an fd number missing, insert the fd there

		int inserted_flag = 0;
		for (int i = 3; i < OPEN_MAX; i++) { //start from 3 because 0,1,2 are reserved. Wastage of memory but whatever.



			if (curproc->proc_filedesc[i] != NULL) {
				if (curproc->proc_filedesc[i]->isempty == 1) {
					//it is empty, you can use this
				} else {
					//its being used elsewhere, check the next element for empty
					continue;
				}
			} else {
				//nothing, you can use this
			}

			kprintf("found a usable fd at %d", i);
			// if it gets to here, then it's found an index where pointer is not used up

			//now create a filedesc structure

			struct filedesc *filedesc_ptr;
			filedesc_ptr = kmalloc(sizeof(*filedesc_ptr));
			filedesc_ptr->fd_lock = lock_create(name); //not sure when i should use this lock
			filedesc_ptr->isempty = 1; //not empty
			filedesc_ptr->fd_vnode = ret; //pointer to vnode object to be stored in filedesc->vnode
			filedesc_ptr->flags = flags;
			filedesc_ptr->read_count = 1;
			filedesc_ptr->name = kstrdup(name);

			//make the thread->filedesc point to the filedesc
			curproc->proc_filedesc[i]= filedesc_ptr;

			*retval = i;	//store the value returned by vfs_open to retval
			inserted_flag = 1;
			break;
		}
		if(inserted_flag==0) {
			*retval = EMFILE;
		}

	} else {
		kprintf("some error in vfs_open()");
	}

	kprintf("returning 0");

	return 0; //returns 0 if no error.
}

// added by pranavja
int sys_write(int fd, const void *buf, size_t size, ssize_t *retval) {

/*	Use VOP_WRITE with struct iovec and struct uio.
	Init uio for write for user space buffers
	uio->uio_segflg = UIO_USERSPACE
	uio->uio_resid has bytes left after IO operation = len - uio->uio_resid
*/
	mode_t mode = 0664;

//check if fd exists, otherwise return error

	if (fd >= MAX_SIZE || fd < 0 ||
				curproc->proc_filedesc[fd]  == NULL || curproc->proc_filedesc[fd]->isempty == 1 ||
				(curproc->proc_filedesc[fd]->flags & O_RDONLY == O_RDONLY) ) {
		retval = EBADF;
		return -1;
	}

	char *write_buf;

	write_buf = kmalloc(sizeof(*buf)*size);
	if (write_buf == NULL) {
		retval = EINVAL;
		return -1;
	}

	struct iovec iov;
	struct uio uio;
	struct vnode *vn;
	int err;
	int flags;
	off_t pos=0;
	int result;
	int bytes_written = 0;

	// write code for the various flags


	//after the correct file handle has been found
	/*
	 * Initialize a uio suitable for I/O from a kernel buffer.
	 *
	 * Usage example;
	 * 	char buf[128];
	 * 	struct iovec iov;
	 * 	struct uio myuio;
	 *
	 * 	uio_kinit(&iov, &myuio, buf, sizeof(buf), 0, UIO_READ);
	 *      result = VOP_READ(vn, &myuio);
	 *      ...
	 */

	lock_acquire(curproc->proc_filedesc[fd]->fd_lock);

	result = copyinstr((userptr_t)usr_ptr_buf, write_buf,size);

	if(result) { //memory problem
		kprintf("\nSome memory problem, copying fails trying to copy buf %d\n", result);
		return -1;
	}


	uio_kinit(&iov, &uio, buf, sizeof(buf), pos, UIO_WRITE);
	uio->uio_segflg = UIO_USERSPACE;

	err = VOP_WRITE(curproc->filedesc[fd]->fd_vnode, &uio);

	if (err) {
		kprintf("%s: Write error: %s\n", name, strerror(err));
		kfree(write_buf);
		lock_release(curthread->t_proc->p_addrspace->filedesc[fd]->fd_lock);
		return -1;
		retval = err;
	}

	curcurthread->t_proc->p_addrspace->filedesc[fd]->offset = uio.uio_offset;

	bytes_written = size - uio.uio_resid;
	kfree(write_buf);
	lock_release(curthread->t_proc->p_addrspace->filedesc[fd]->fd_lock);
	//retval = bytes_written;
	return bytes_written; //TODO handle returns. only specific returns possible
}


