#include <file_syscalls.h>

#include <types.h>
#include <clock.h>
#include <copyinout.h>
#include <current.h>




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

	result = copyinstr(filename, name, sizeof(name));

	if(result) { //memory problem
		kprintf("\nSome memory problem, copyin fails when copy name %d\n", result);
		return result;
	}

	struct vnode *ret; //empty nvnode

	int returner = vfs_open(name, flags, mode, &ret);

	if (returner==0) {
		kprintf("successfully opened file %s\n", name);

		//add an fd to the list of fds in the thread's fd table

		//iterate over all the fds to check if there is an fd number missing, insert the fd there

		int inserted_flag = 0;
		for(int i = 3; i<50; i++) { //start from 3 because 0,1,2 are reserved. Wastage of memory but whatever.
			if(curproc->thread_filedesc[i]!=NULL) {
				continue;
			}
			if(curproc->thread_filedesc[i]->isempty!=0) {
				continue;
			}

			// if it gets to here, then it's found an index where pointer is not used up

			//now create a filedesc structure

			struct filedesc *filedesc_ptr;
			filedesc_ptr=kmalloc(sizeof(*filedesc_ptr));
			filedesc_ptr->fd_lock = lock_create(); 			//not sure when i should use this lock
			filedesc_ptr->isempty = 1; //not empty
			filedesc_ptr->fd_vnode = ret; //pointer to vnode object to be stored in filedesc->vnode
			filedesc_ptr->flags = flags;
			filedesc_ptr->read_count = 1;


			//make the thread->filedesc point to the filedesc
			curthread->thread_filedesc= filedesc_ptr;

			*retval = i;
			break;
		}
		if(inserted_flag==0) {
			panic("oops, ran out of fd's lel.\n");
		}

	} else {
		kprintf("some error in vfs_open()");
	}

	return 0; //returns 0 if no error.
}


