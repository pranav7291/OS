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

	mode_t mode = 0664; // Dunno what this means but whatever.

	char name[100];
	int result;

	if (filename == NULL || filename == (void *) 0x40000000) {
		*retval = -1;
		return EFAULT;
	}
	if (flags > 66) {
		*retval = -1;
		return EINVAL;
	}

	size_t length;
	result = copycheck1((const_userptr_t) filename, PATH_MAX, &length);
	if (result) {
		*retval = -1;
		return result;
	}

	result = copyinstr((const_userptr_t) filename, name, sizeof(name), NULL);

	if (result) { //memory problem
		*retval = -1;
		return result;
	}

	struct vnode *ret; //empty nvnode
	int returner = vfs_open(name, flags, mode, &ret);

	if ((returner == 0)) { // && (name != fd_name)) {
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
			// if it gets to here, then it's found an index where pointer is not used up
			//now create a filedesc structure
			struct filedesc *filedesc_ptr;
			filedesc_ptr = kmalloc(sizeof(*filedesc_ptr));
			if (filedesc_ptr == NULL) {
				*retval = -1;
				return ENOMEM;
			}
			filedesc_ptr->fd_lock = lock_create(name); //not sure when i should use this lock
			if (filedesc_ptr->fd_lock == NULL) {
				kfree(filedesc_ptr);
				*retval = -1;
				return ENOMEM;
			}
			filedesc_ptr->isempty = 0; //not empty
			filedesc_ptr->fd_vnode = ret; //pointer to vnode object to be stored in filedesc->vnode
			filedesc_ptr->flags = flags;
			filedesc_ptr->read_count = 1;
			filedesc_ptr->name = kstrdup(name);
			filedesc_ptr->fd_refcount = 1;

			if ((flags & O_APPEND) == O_APPEND) {
				struct stat *stat_obj;
				stat_obj = kmalloc(sizeof(struct stat));
				if (stat_obj == NULL) {
					kfree(filedesc_ptr);
					*retval = -1;
					return ENOMEM;
				}
				int vopstat_returner = VOP_STAT(ret, stat_obj);
				if (vopstat_returner != 0) {
				}
				filedesc_ptr->offset = stat_obj->st_size;
				kfree(stat_obj);
			} else {
				filedesc_ptr->offset = 0;
			}
			//make the thread->filedesc point to the filedesc
			lock_acquire(filedesc_ptr->fd_lock);
			curproc->proc_filedesc[i] = filedesc_ptr;
			lock_release(curproc->proc_filedesc[i]->fd_lock);

			*retval = i;	//store the value returned by vfs_open to retval
			inserted_flag = 1;
			break;
		}
		if (inserted_flag == 0) {
			return EMFILE;
		}
	}

	return 0; //returns 0 if no error.
}

int sys_write(int fd, const void *buf, size_t size, ssize_t *retval) {
	if (fd < 0 || fd >= OPEN_MAX) {
		*retval = -1;
		return EBADF;
	}
	if (curproc->proc_filedesc[fd] == NULL
			|| (curproc->proc_filedesc[fd]->flags & O_ACCMODE) == O_RDONLY) {
		*retval = -1;
		return EBADF;
	}
	size_t length;
	int err1;
	err1 = copycheck1((const_userptr_t) buf, size, &length);
	if (err1) {
		*retval = -1;
		return EFAULT;
	}
	if (buf == (void *) 0x40000000) {
		*retval = -1;
		return EFAULT;
	}

	lock_acquire(curproc->proc_filedesc[fd]->fd_lock);

	struct iovec iov;
	struct uio uio_obj;
	int err;
	off_t pos = curproc->proc_filedesc[fd]->offset;

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
		lock_release(curproc->proc_filedesc[fd]->fd_lock);
		*retval = -1;
		return err;
	}

	curproc->proc_filedesc[fd]->offset = uio_obj.uio_offset;

	*retval = size - uio_obj.uio_resid;

	lock_release(curproc->proc_filedesc[fd]->fd_lock);
	return 0; //done: handle returns. only specific returns possible
}

int sys_close(int fd, ssize_t *retval) {

	if (fd < 0 || fd > OPEN_MAX) {
		*retval = -1;
		return EBADF;
	} else if (curproc->proc_filedesc[fd] == NULL) {
		return EBADF;
	} else {
		if ((curproc->proc_filedesc[fd] != NULL) && (fd > 2)) {
			int refcount;
			lock_acquire(curproc->proc_filedesc[fd]->fd_lock);
			curproc->proc_filedesc[fd]->fd_refcount--;
			lock_release(curproc->proc_filedesc[fd]->fd_lock);
			refcount = curproc->proc_filedesc[fd]->fd_refcount;

			if (refcount == 0) {
				vfs_close(curproc->proc_filedesc[fd]->fd_vnode);
				lock_destroy(curproc->proc_filedesc[fd]->fd_lock);
				kfree(curproc->proc_filedesc[fd]->name);
				kfree(curproc->proc_filedesc[fd]);
				curproc->proc_filedesc[fd] = NULL;
			}
		}
		*retval = 0;
		return 0;
	}
}

int sys_read(int fd, void *buf, size_t buflen, ssize_t *retval) {

	if (fd >= OPEN_MAX || fd < 0 ||
	curproc->proc_filedesc[fd] == NULL
			|| curproc->proc_filedesc[fd]->isempty == 1
			|| ((curproc->proc_filedesc[fd]->flags & O_ACCMODE) == O_WRONLY)) {
		*retval = -1;
		return EBADF;
	}
	size_t length;
	int result;
	result = copycheck1((const_userptr_t) buf, buflen, &length);
	if (result) {
		*retval = -1;
		return result;
	}

	if (buf == (void *) 0x40000000) {
		*retval = -1;
		return EFAULT;
	}
	struct iovec iov;
	struct uio uio_obj;

	lock_acquire(curproc->proc_filedesc[fd]->fd_lock);

	iov.iov_ubase = (userptr_t) buf;
	iov.iov_len = buflen;
	uio_obj.uio_iov = &iov;
	uio_obj.uio_iovcnt = 1;
	off_t pos = curproc->proc_filedesc[fd]->offset;
	uio_obj.uio_offset = pos;
	uio_obj.uio_resid = buflen;
	uio_obj.uio_segflg = UIO_USERSPACE;
	uio_obj.uio_rw = UIO_READ;
	uio_obj.uio_space = curproc->p_addrspace;

	int err = VOP_READ(curproc->proc_filedesc[fd]->fd_vnode, &uio_obj);
	if (err) {
		lock_release(curproc->proc_filedesc[fd]->fd_lock);
		return EINVAL;
	}
	curproc->proc_filedesc[fd]->offset = uio_obj.uio_offset;
	*retval = buflen - uio_obj.uio_resid;
	lock_release(curproc->proc_filedesc[fd]->fd_lock);
	return 0;

}

int sys_dup2(int oldfd, int newfd, ssize_t *retval) {

	int result = 0;
	if (oldfd > OPEN_MAX || oldfd < 0 || newfd < 0 || newfd >= OPEN_MAX) {
		*retval = -1;
		return EBADF;
	}
	if (newfd >= OPEN_MAX) {
		*retval = -1;
		return EMFILE;
	}

	if (curproc->proc_filedesc[oldfd] == NULL) {
		*retval = -1;
		return EBADF;
	}

	lock_acquire(curproc->proc_filedesc[oldfd]->fd_lock);
	if (curproc->proc_filedesc[newfd] != NULL) {
		result = sys_close(newfd, retval);
		if (result) {
			lock_release(curproc->proc_filedesc[oldfd]->fd_lock);
			return EBADF;
		}
	}

	curproc->proc_filedesc[oldfd]->fd_refcount++;
	curproc->proc_filedesc[newfd] = curproc->proc_filedesc[oldfd];

	lock_release(curproc->proc_filedesc[oldfd]->fd_lock);

	*retval = newfd;
	return 0;
}

off_t sys_lseek(int filehandle, off_t pos, int code, ssize_t *retval,
		ssize_t *retval2) {
	if (filehandle > OPEN_MAX
			|| filehandle < 0|| curproc->proc_filedesc[filehandle] == NULL) {
		*retval = -1;
		return EBADF;
	}
	int result;
	off_t offset;
	off_t filesize;
	struct stat file_stat;

	lock_acquire(curproc->proc_filedesc[filehandle]->fd_lock);
	result = VOP_ISSEEKABLE(curproc->proc_filedesc[filehandle]->fd_vnode);
	if (!result) { //file not seekable
		lock_release(curproc->proc_filedesc[filehandle]->fd_lock);
		return ESPIPE;
	}

	result = VOP_STAT(curproc->proc_filedesc[filehandle]->fd_vnode, &file_stat);
	if (result) { //VOP_STAT failed
		lock_release(curproc->proc_filedesc[filehandle]->fd_lock);
		return result;
	}
	filesize = file_stat.st_size;
	if (code == SEEK_SET) { //the new position is pos
		offset = pos;
	} else if (code == SEEK_CUR) { // the new position is the current position plus pos
		offset = pos + curproc->proc_filedesc[filehandle]->offset;
	} else if (code == SEEK_END) { //the new position is the position of end-of-file plus pos
		offset = pos + filesize;
	} else {
		lock_release(curproc->proc_filedesc[filehandle]->fd_lock);
		return EINVAL;
	}
	if (offset < (off_t) 0) {
		lock_release(curproc->proc_filedesc[filehandle]->fd_lock);
		return EINVAL;
	}
	curproc->proc_filedesc[filehandle]->offset = offset;

	*retval = (offset & 0xFFFFFFFF00000000) >> 32;
	*retval2 = offset & 0xFFFFFFFF;
	lock_release(curproc->proc_filedesc[filehandle]->fd_lock);
	return 0;
}

int sys_chdir(const char *path) {
	int result;
	char *path2;
	size_t size;
	path2 = (char *) kmalloc(sizeof(char) * PATH_MAX);
	if (path2 == NULL) {
		return ENOMEM;
	}

	result = copyinstr((const_userptr_t) path, path2, PATH_MAX, &size);

	if (result) {
		kfree(path2);
		return EFAULT;
	}
	result = vfs_chdir(path2);
	if (result) {
		kfree(path2);
		return result;
	}
	kfree(path2);
	return 0;
}

int sys___getcwd(char *buf, size_t buflen, int *retval) {

	if (buf == (void *) 0x40000000) {
		*retval = -1;
		return EFAULT;
	}
	char *cwdbuf;
	cwdbuf = kmalloc(sizeof(*buf) * buflen);
	if (cwdbuf == NULL) {
		*retval = -1;
		return ENOMEM;
	}

	int result;
	size_t size;
	result = copyinstr((const_userptr_t) buf, cwdbuf, PATH_MAX, &size);
	if (result) {
		kfree(cwdbuf);
		return EFAULT;
	}

	struct iovec iov;
	struct uio uio;

	iov.iov_ubase = (userptr_t) buf;
	iov.iov_len = buflen;
	uio.uio_iov = &iov;
	uio.uio_iovcnt = 1;
	uio.uio_offset = (off_t) 0;
	uio.uio_resid = buflen;
	uio.uio_segflg = UIO_USERSPACE;
	uio.uio_rw = UIO_READ;
	uio.uio_space = curproc->p_addrspace;

	result = vfs_getcwd(&uio);
	if (result) {
		kfree(cwdbuf);
		return result;
	}
	*retval = strlen(buf);

	kfree(cwdbuf);
	return 0;
}
