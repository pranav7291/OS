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
#include <syscall.h>
#include <lib.h>
#include <stdarg.h>
#include <types.h>
#include <kern/fcntl.h>
#include <kern/seek.h>

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

	//KASSERT(curthread->t_in_interrupt == false);
	mode_t mode = 0664; // Dunno what this means but whatever.

	char name[100];
	int result;
	//added by pranavja
//	if(!(flags==O_RDONLY || flags==O_WRONLY || flags==O_RDWR || flags==(O_RDWR|O_CREAT|O_TRUNC))) {
//		return EINVAL;
//	}
	if(filename==NULL || filename==(void *)0x40000000){
		*retval = -1;
		return EFAULT;
	}
	if(flags > 66){
		*retval = -1;
		return EINVAL;
	}

	size_t length;
	result = copycheck1((const_userptr_t) filename, PATH_MAX, &length);
	if(result){
		*retval = -1;
		return result;
	}
	//end pranavja

	result = copyinstr((const_userptr_t)filename, name, sizeof(name),NULL);

	if(result) { //memory problem
//		//printf("\nSome memory problem, copyin fails when copy name %d\n", result);
		*retval = -1;
		return result;
	}

	struct vnode *ret; //empty nvnode
	int returner = vfs_open(name, flags, mode, &ret);

//	//printf("the returner of vfs_open is %d", returner);

	if (returner==0) {
//		//printf("successfully opened file %s\n", name);
		//first add the default fd's (0,1,2) to the file table because the kernel shouldn't have to open these
		//add an fd to the list of fds in the thread's fd table
		//iterate over all the fds to check if there is an fd number missing, insert the fd there

		int inserted_flag = 0;
		for (int i = 3; i < OPEN_MAX; i++) {
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

//			//printf("found a usable fd at %d", i);
			// if it gets to here, then it's found an index where pointer is not used up
			//now create a filedesc structure
			struct filedesc *filedesc_ptr;
			filedesc_ptr = kmalloc(sizeof(*filedesc_ptr));
			filedesc_ptr->fd_lock = lock_create(name); //not sure when i should use this lock
			filedesc_ptr->isempty = 0; //not empty
			filedesc_ptr->fd_vnode = ret; //pointer to vnode object to be stored in filedesc->vnode
			filedesc_ptr->flags = flags;
			filedesc_ptr->read_count = 1;
			filedesc_ptr->name = kstrdup(name);
			filedesc_ptr->fd_refcount = 1;

			if((flags & O_APPEND) == O_APPEND) {
//				//printf("Opening in append mode\n");
				struct stat *stat_obj;
				stat_obj = kmalloc(sizeof(struct stat));
				int vopstat_returner = VOP_STAT(ret, stat_obj);
				if (vopstat_returner != 0) {
//					//printf("vopstat_returner did not return 0, some error\n");
				}
				filedesc_ptr->offset = stat_obj->st_size;
			} else {
				filedesc_ptr->offset = 0;
			}
			//make the thread->filedesc point to the filedesc
			lock_acquire(filedesc_ptr->fd_lock);
			curproc->proc_filedesc[i]= filedesc_ptr;
			//pranavja add
			if(i > curproc->count_filedesc)
			curproc->count_filedesc++;
			//pranavja end
			lock_release(curproc->proc_filedesc[i]->fd_lock);

			*retval = i;	//store the value returned by vfs_open to retval
			inserted_flag = 1;
			break;
		}
		if(inserted_flag==0) {
//			//printf("Error! Out of file descriptors");
//			*retval = EMFILE;
			return EMFILE;
		}
	} else {
//		//printf("some error in vfs_open()");
	}

//	//printf("returning 0");
	return 0; //returns 0 if no error.
}


// added by pranavja
int sys_write(int fd, const void *buf, size_t size, ssize_t *retval) {
	if(fd < 0 || fd >= OPEN_MAX){
		*retval = -1;
		return EBADF;
	}
	if (curproc->proc_filedesc[fd]  == NULL ||
				(curproc->proc_filedesc[fd]->flags & O_ACCMODE) == O_RDONLY){
		*retval = -1;
		return EBADF;
	}
	size_t length;
	int err1;
	err1 = copycheck1((const_userptr_t) buf, size, &length);
	if(err1){
		*retval = -1;
		return EFAULT;
	}
	if (buf == NULL || buf ==(void *)0x40000000){
		*retval = -1;
		return EFAULT;
	}


	lock_acquire(curproc->proc_filedesc[fd]->fd_lock);

//	//printf("Inside write with fd %d\n", fd);

/*	Use VOP_WRITE with struct iovec and struct uio.
	Init uio for write for user space buffers
	uio->uio_segflg = UIO_USERSPACE
	uio->uio_resid has bytes left after IO operation = len - uio->uio_resid
*/
//check if fd exists, otherwise return error


	//dummy comment for git test

//	if (fd > OPEN_MAX || fd < 0 ||
//				curproc->proc_filedesc[fd]  == NULL || curproc->proc_filedesc[fd]->isempty == 1 ||
//				((curproc->proc_filedesc[fd]->flags & O_ACCMODE) == O_RDONLY) ) {
//
//		if(curproc->proc_filedesc[fd]  == NULL ) {
//			//printf("filedesc[fd] is null...\n");
//		} else if(curproc->proc_filedesc[fd]->isempty == 1) {
//			//printf("is empty=1\n");
//		}else if((curproc->proc_filedesc[fd]->flags & O_ACCMODE) == O_RDONLY) {
//			//printf("is read only...\n");
//			//printf("the flags value is set to %d",curproc->proc_filedesc[fd]->flags );
//		}
//		lock_release(curproc->proc_filedesc[fd]->fd_lock);
//		//printf("Some error, returning EBADF for fd=%d..\n",fd);
//		*retval = -1;
//		return EBADF;
//	}

//	//printf("File descriptor %d exists in the file table. Yay.", fd );

	void *write_buf;

	write_buf = kmalloc(sizeof(*buf)*size);
	if (write_buf == NULL) {
		lock_release(curproc->proc_filedesc[fd]->fd_lock);
		*retval = -1;
		return EINVAL;
	}

	struct iovec iov;
	struct uio uio_obj;
	int err;
	off_t pos= curproc->proc_filedesc[fd]->offset;
	int result;


	// todo write code for the various flags




//	//printf("the write buffer before copyin %s", buf);

//	size_t got;
//	result = copyinstr((const_userptr_t)buf, write_buf,size, &got);

	result = copyin((const_userptr_t)buf,write_buf,size);

//	//printf("the write buffer %s", write_buf);

	if(result) { //memory problem
//		//printf("\ncopyinstr failed with return code %d\n", result);

		//free memory
		kfree(write_buf);

		//release the lock before returning error
		lock_release(curproc->proc_filedesc[fd]->fd_lock);
		*retval = -1;
		return EINVAL;
	}

	//copying code from load_elf.c
	iov.iov_ubase = (userptr_t) buf;
	iov.iov_len = size;
	uio_obj.uio_iov = &iov;
	uio_obj.uio_iovcnt = 1;
	uio_obj.uio_offset = pos;
	uio_obj.uio_resid = size;
	uio_obj.uio_segflg = UIO_USERSPACE;
	uio_obj.uio_rw = UIO_WRITE;
	uio_obj.uio_space = curproc->p_addrspace;




	err = VOP_WRITE(curproc->proc_filedesc[fd]->fd_vnode, &uio_obj);

	if (err) {
//		//printf("%s: Write error: %s\n", curproc->proc_filedesc[fd]->name, strerror(err));
		kfree(write_buf);
		lock_release(curproc->proc_filedesc[fd]->fd_lock);
		*retval = -1;
		return err;
	}

	curproc->proc_filedesc[fd]->offset = uio_obj.uio_offset;

	*retval = size - uio_obj.uio_resid;

	kfree(write_buf);
	lock_release(curproc->proc_filedesc[fd]->fd_lock);
	//retval = bytes_written;
	return 0; //done: handle returns. only specific returns possible
}

int sys_close(int fd, ssize_t *retval) {
//	//printf("In close");

	//lock_acquire(curproc->proc_filedesc[fd]->fd_lock);
//	if(curproc->proc_filedesc[fd]==NULL) {
//		//printf("fd does not exist.");
//		return EBADF;
//	}
	//pranavja add
	if (fd < 0 || fd > OPEN_MAX ) {
		*retval = -1;
		return EBADF;
	} else if (curproc->proc_filedesc[fd]==NULL){
		return EBADF;
	}	else {
		curproc->proc_filedesc[fd]->fd_refcount--;
		////kprintf("\n close() the ref count is %d", curproc->proc_filedesc[fd]->fd_refcount);
		if (curproc->proc_filedesc[fd]->fd_refcount == 0) {
			//lock_release(curproc->proc_filedesc[fd]->fd_lock);
			vfs_close(curproc->proc_filedesc[fd]->fd_vnode);
			lock_destroy(curproc->proc_filedesc[fd]->fd_lock);
			kfree(curproc->proc_filedesc[fd]->name);
			kfree(curproc->proc_filedesc[fd]);
			curproc->proc_filedesc[fd] = NULL;
			//add pranavja
			curproc->count_filedesc--;
			//end pranavja
		}
		*retval = 0;
		return 0;
	}
}

int sys_read(int fd,void *buf, size_t buflen, ssize_t *retval) {
	//mostly same as sys_write kinda sorta

	//same fd conditions
	if (fd >= OPEN_MAX || fd < 0 ||
				curproc->proc_filedesc[fd]  == NULL || curproc->proc_filedesc[fd]->isempty == 1 ||
				((curproc->proc_filedesc[fd]->flags & O_ACCMODE) == O_WRONLY)  ) {
		*retval = -1;
		return EBADF;
	}
	//added by pranavja
	size_t length;
	int result;
	result = copycheck1((const_userptr_t) buf, buflen, &length);
	if(result){
		*retval = -1;
		return result;
	}

	if (buf == NULL || buf == (void *)0x40000000){
		*retval = -1;
		return EFAULT;
	}
	//end pranavja
	struct iovec iov;
	struct uio uio_obj;

	//copying code from load_elf.c
	lock_acquire(curproc->proc_filedesc[fd]->fd_lock);

	iov.iov_ubase = (userptr_t) buf;
	iov.iov_len = buflen;
	uio_obj.uio_iov = &iov;
	uio_obj.uio_iovcnt = 1;
	off_t pos= curproc->proc_filedesc[fd]->offset;
	uio_obj.uio_offset = pos;
	uio_obj.uio_resid = buflen;
	uio_obj.uio_segflg = UIO_USERSPACE;
	uio_obj.uio_rw = UIO_READ;
	uio_obj.uio_space = curproc->p_addrspace;

	// todo write code for the various flags

//	lock_acquire(curproc->proc_filedesc[fd]->fd_lock);
	int err = VOP_READ(curproc->proc_filedesc[fd]->fd_vnode, &uio_obj);
	if(err) {
		lock_release(curproc->proc_filedesc[fd]->fd_lock);
		return EINVAL;
	}
	curproc->proc_filedesc[fd]->offset = uio_obj.uio_offset;
	*retval = buflen - uio_obj.uio_resid;
	lock_release(curproc->proc_filedesc[fd]->fd_lock);
	return 0;

}

int sys_dup2(int oldfd, int newfd, ssize_t *retval){
	int result = 0;
	//pranavja adding checks separately
	if (oldfd > OPEN_MAX || oldfd < 0 || newfd > OPEN_MAX || newfd < 0){
		*retval = -1;
		return EBADF;
	}
	if (curproc->proc_filedesc[oldfd]  == NULL){
		*retval = -1;
		return EBADF;
	}

	lock_acquire(curproc->proc_filedesc[oldfd]->fd_lock);
	if(curproc->proc_filedesc[newfd] != NULL){
		result = sys_close(newfd,retval);
		if (result)
			return EBADF;
	}

//	lock_acquire(curproc->proc_filedesc[oldfd]->fd_lock);

//	curproc->proc_filedesc[newhandle] = (struct filedesc *)kmalloc(sizeof(struct filedesc *));
//	curproc->proc_filedesc[newhandle]->fd_vnode = curproc->proc_filedesc[filehandle]->fd_vnode;
//	curproc->proc_filedesc[newhandle]->fd_lock = lock_create("dup2 file lock");
//	curproc->proc_filedesc[newhandle]->isempty = curproc->proc_filedesc[filehandle]->isempty; //not empty
//	curproc->proc_filedesc[newhandle]->flags = curproc->proc_filedesc[filehandle]->flags;
//	curproc->proc_filedesc[newhandle]->offset = curproc->proc_filedesc[filehandle]->offset;
//	curproc->proc_filedesc[newhandle]->read_count = curproc->proc_filedesc[filehandle]->read_count;
////	strcpy(curproc->proc_filedesc[newhandle]->name, curproc->proc_filedesc[filehandle]->name);
//	curproc->proc_filedesc[newhandle]->name=kstrdup(curproc->proc_filedesc[filehandle]->name);
//	curproc->proc_filedesc[newhandle]->fd_refcount = curproc->proc_filedesc[filehandle]->fd_refcount;

	curproc->proc_filedesc[newfd] = curproc->proc_filedesc[oldfd];
	curproc->proc_filedesc[newfd]->fd_refcount++;

	lock_release(curproc->proc_filedesc[oldfd]->fd_lock);

	*retval = newfd;
	return 0;

}

off_t sys_lseek(int filehandle, off_t pos, int code, ssize_t *retval, ssize_t *retval2){
	if (filehandle > OPEN_MAX || filehandle < 0 || curproc->proc_filedesc[filehandle]  == NULL){
		*retval=-1;
		return EBADF;
	}
	//pranavja add
	if(filehandle > curproc->count_filedesc + 2){
		*retval=-1;
		return EBADF;
	}
	//pranavja end

	int result;
	off_t offset;
	off_t filesize;
	struct stat file_stat;

	lock_acquire(curproc->proc_filedesc[filehandle]->fd_lock);
	result = VOP_ISSEEKABLE(curproc->proc_filedesc[filehandle]->fd_vnode);
	if(!result){//file not seekable
			lock_release(curproc->proc_filedesc[filehandle]->fd_lock);
			return result;
	}

	result = VOP_STAT(curproc->proc_filedesc[filehandle]->fd_vnode, &file_stat);
	if(result){//VOP_STAT failed
		lock_release(curproc->proc_filedesc[filehandle]->fd_lock);
		return result;
	}
	filesize = file_stat.st_size;
	if (code == SEEK_SET){//the new position is pos
		offset = pos;
	}
	else if(code == SEEK_CUR){// the new position is the current position plus pos
		offset = pos + curproc->proc_filedesc[filehandle]->offset;
	}
	else if(code == SEEK_END){//the new position is the position of end-of-file plus pos
		offset = pos + filesize;
	}
	else {
		lock_release(curproc->proc_filedesc[filehandle]->fd_lock);
		return EINVAL;
	}
	if (offset < (off_t) 0){
		lock_release(curproc->proc_filedesc[filehandle]->fd_lock);
		return EINVAL;
	}
	curproc->proc_filedesc[filehandle]->offset = offset;

	*retval = (offset & 0xFFFFFFFF00000000) >> 32;
	*retval2 = offset & 0xFFFFFFFF;
	lock_release(curproc->proc_filedesc[filehandle]->fd_lock);
	return 0;
}

int sys_chdir(const char *path){
	int result;
	char *path2;
	size_t size;
	path2 = (char *) kmalloc(sizeof(char)*PATH_MAX);

	result = copyinstr((const_userptr_t)path, path2, PATH_MAX,&size);

	if(result) {
		kfree(path2);
		return EFAULT;
	}
	result = vfs_chdir(path2);
	if (result){
		kfree(path2);
		return result;
	}
	kfree(path2);
	return 0;
}

int sys___getcwd(char *buf, size_t buflen, int *retval){

	if (buf == NULL || buf==(void *)0x40000000){
		*retval = -1;
		return EFAULT;
	}
	char *cwdbuf;
	cwdbuf = kmalloc(sizeof(*buf) * buflen);
	if (cwdbuf == NULL) {
		return EINVAL;
	}

	int result;
	size_t size;
	result = copyinstr((const_userptr_t)buf, cwdbuf, PATH_MAX,&size);
	if(result) {
		kfree(cwdbuf);
		return EFAULT;
	}

	struct iovec iov;
	struct uio uio;

	iov.iov_ubase = (userptr_t) buf;
	iov.iov_len = buflen;
	uio.uio_iov = &iov;
	uio.uio_iovcnt = 1;
	//off_t pos= curproc->proc_filedesc[fd]->offset;
	uio.uio_offset = (off_t)0;
	uio.uio_resid = buflen;
	uio.uio_segflg = UIO_USERSPACE;
	uio.uio_rw = UIO_READ;
	uio.uio_space = curproc->p_addrspace;

	result = vfs_getcwd(&uio);
	if (result){
		kfree(cwdbuf);
		return result;
	}
	*retval = strlen(buf);

	kfree(cwdbuf);
	return 0;
}
