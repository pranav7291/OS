#include "../include/file_syscalls.h"

#include <types.h>
#include <clock.h>
#include <copyinout.h>
#include <syscall.h>
#include <unistd.h>


struct filedesc {
	struct vnode *vnode;
	int refcount;
	int index;

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

	mode_t mode = 0664; //TODO Dunno what this means but whatever.




	int flags;
	int result;

	result = copyout(flags, usr_ptr_flags, sizeof(flags));

	if(result) { //memory problem
		printf("\nSome memory problem, copyout fails %d\n", result);
		return result;
	}


	struct vnode *ret; //empty nvnode


	//TODO file descriptor insertion logic



	int returner = vfs_open(filename, flags, mode, &ret);

	//todo if returner is positive, add it to file table


	return returner; //TODO handle returns. only specific returns possible
}


