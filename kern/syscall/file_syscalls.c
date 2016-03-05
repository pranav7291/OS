#include "../include/file_syscalls.h"

#include <types.h>
#include <clock.h>
#include <copyinout.h>
#include <syscall.h>
#include <unistd.h>
#include <current.h>



struct filedesc {
	struct vnode *fd_vnode;
	int fd_refcount;
	int fd_count;
	struct lock *fd_lock
	char name[100];
	int offset;
	int isempty;
};

/**
 * Added by sammokka
 *  input:
 *  flags -> is a OR of flags
 *
 *  output: file descriptor
 *
 *  eg: O_WRONLY|O_CREAT|O_TRUNC
 */
int sys_open(userptr_t usr_ptr_filename, userptr_t  usr_ptr_flags) {

//call vfs_open
//if error, handles

	KASSERT(curthread->t_in_interrupt == false);


	mode_t mode = 0664; //TODO Dunno what this means but whatever.

	printf("\nin sys_open..\n");


	char name[100];

	int *flags;
	int result;

	result = copyin(usr_ptr_flags, flags, sizeof(flags));

	if(result) { //memory problem
		printf("\nSome memory problem, copyin failstryin to copy flags %d\n", result);
		return result;
	}

	result = copyin(usr_ptr_filename, name, sizeof(name));

	if(result) { //memory problem
		printf("\nSome memory problem, copyin fails when copy name %d\n", result);
		return result;
	}

	struct vnode *ret; //empty nvnode




	//TODO file descriptor insertion logic





	int returner = vfs_open(name, flags, mode, &ret);

	if (returner > 2) {
		printf("successfully opened file %s", name);

		//add an fd to the list of fds in the thread's fd table

		//iterate over all the fds to check if there is an fd number missing, insert the fd there
		for(int i = 0; i<50; i++) {
			if(curthread->filedesc[i]!=NULL) {
				continue;
			}



		}


		//allocate memory for a new fd
		struct filedesc *filedesc;
		filedesc = malloc(sizeof(filedesc));
		curthread->filedesc = malloc(sizeof(*filedesc));


		//




	} else {
		printf("some error in vfs_open()");

	}

	//todo if returner is positive, add it to file table


	//todo copy the return value to memrory in copyin

	return 0; //TODO handle returns. only specific returns possible, and returns the descriptor
}


